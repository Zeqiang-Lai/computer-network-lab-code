#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <ctime>
#pragma comment(lib,"ws2_32.lib")
using namespace std; 

//IP报头
typedef struct{    
	unsigned char hdr_len:4;         //4位头部长度    
	unsigned char version:4;         //4位版本号    
	unsigned char tos;               //8位服务类型    
	unsigned short total_len;        //16位总长度    
	unsigned short identifier;       //16位标识符    
	unsigned short frag_and_flags;   //3位标志加13位片偏移    
	unsigned char ttl;               //8位生存时间    
	unsigned char protocol;          //8位上层协议号    
	unsigned short checksum;         //16位效验和    
	unsigned long sourceIP;          //32位源IP地址    
	unsigned long destIP;            //32位目的IP地址
}IP_HEADER; 

//ICMP报头
typedef struct{    
	BYTE type;     //8位类型字段    
	BYTE code;     //8位代码字段    
	USHORT cksum;  //16位效验和    
	USHORT id;     //16位标识符    
	USHORT seq;    //16位序列号
}ICMP_HEADER;  

//报文解码结构
typedef struct{    
	USHORT usSeqNo;          //序列号    
	DWORD dwRoundTripTime;   //返回时间    
	in_addr dwIPaddr;        //返回报文的IP地址
}DECODE_RESULT; 

//计算网际效验和函数
USHORT checksum(USHORT *pBuf,int iSize)
{
	unsigned long cksum=0;
	while(iSize>1)
	{
		cksum+=*pBuf++;
		iSize-=sizeof(USHORT);
	}
	if(iSize)
	{
		cksum+=*(USHORT*)pBuf;
	}
	cksum=(cksum>>16)+(cksum&0xffff);
	cksum+=(cksum>>16);
	return(USHORT)(~cksum);
} 

//对数据包进行解码
BOOL DecodeIcmpResponse(char *pBuf,int iPacketSize,DECODE_RESULT &DecodeResult,BYTE ICMP_ECHO_REPLY,BYTE ICMP_TIMEOUT)
{    
	//检查数据报大小的合法性    
	IP_HEADER *pIpHdr=(IP_HEADER*)pBuf;    
	int iIpHdrLen=pIpHdr->hdr_len*4;    
	if(iPacketSize<(int)(iIpHdrLen+sizeof(ICMP_HEADER)))        
		return FALSE;    //根据ICMP报文类型提取ID字段和序列号字段    
	ICMP_HEADER *pIcmpHdr=(ICMP_HEADER*)(pBuf+iIpHdrLen);    
	USHORT usID,usSquNo;    
	if(pIcmpHdr->type==ICMP_ECHO_REPLY)    //ICMP回显应答报文    
	{        
		usID=pIcmpHdr->id;   //报文ID        
		usSquNo=pIcmpHdr->seq;  //报文序列号    
	}    
	else if(pIcmpHdr->type==ICMP_TIMEOUT)   //ICMP超时差错报文       
	{           
		char *pInnerIpHdr=pBuf+iIpHdrLen+sizeof(ICMP_HEADER);  //载荷中的IP头            
		int iInnerIPHdrLen=((IP_HEADER*)pInnerIpHdr)->hdr_len*4; //载荷中的IP头长            
		ICMP_HEADER *pInnerIcmpHdr=(ICMP_HEADER*)(pInnerIpHdr+iInnerIPHdrLen);//载荷中的ICMP头            
		usID=pInnerIcmpHdr->id;  //报文ID            
		usSquNo=pInnerIcmpHdr->seq;  //序列号        
	}
	else
	{
		return false;
	}
	//检查ID和序列号以确定收到期待数据报        
	if(usID!=(USHORT)GetCurrentProcessId()||usSquNo!=DecodeResult.usSeqNo) 
	{
		return false;
	}
	//记录IP地址并计算往返时间        
	DecodeResult.dwIPaddr.s_addr=pIpHdr->sourceIP; 
	DecodeResult.dwRoundTripTime=GetTickCount()-DecodeResult.dwRoundTripTime;
	//处理正确收到的ICMP数据报        
	if(pIcmpHdr->type==ICMP_ECHO_REPLY||pIcmpHdr->type==ICMP_TIMEOUT)
	{
		return true;
	}
	else
	{
		return false;
	}
	return true;
}

int main(int argc,char **argv)
{
	//初始化Windows sockets网络环境    
	WSADATA wsa;
	WSAStartup(MAKEWORD(2,2),&wsa);
	if (argc < 2)
		exit(-1);
	//得到IP地址    
	u_long ulDestIP=inet_addr(argv[1]);
	if(ulDestIP==INADDR_NONE)
	{
		hostent *pHostent=gethostbyname(argv[1]);
		if(pHostent)
		{
			ulDestIP=(*(in_addr*)pHostent->h_addr).s_addr;
		}
		else
		{
			cout<<"输入的IP地址或域名无效"<<endl;
			WSACleanup();
			return 0;
		}
	}
	 sockaddr_in destSockAddr;
	 ZeroMemory(&destSockAddr,sizeof(sockaddr_in));
	 destSockAddr.sin_family=AF_INET;
	 destSockAddr.sin_addr.s_addr=ulDestIP;
	 SOCKET sockRaw=WSASocket(AF_INET,SOCK_RAW,IPPROTO_ICMP,NULL,0,WSA_FLAG_OVERLAPPED);
	 int iTimeout=3000;
	 setsockopt(sockRaw, SOL_SOCKET, SO_RCVTIMEO, (char*)&iTimeout, sizeof(iTimeout));
	 setsockopt(sockRaw, SOL_SOCKET, SO_SNDTIMEO, (char*)&iTimeout, sizeof(iTimeout));
	 const BYTE ICMP_ECHO_REQUEST = 8;
	 const BYTE ICMP_ECHO_REPLY = 0;
	 const BYTE ICMP_TIMEOUT = 11;
	 const int DEF_ICMP_DATA_SIZE=32;
	 const int MAX_ICMP_PACKET_SIZE=1024;
	 const DWORD DEF_ICMP_TIMEOUT=3000;
	 const int DEF_MAX_HOP=4;
	 char IcmpSendBuf[sizeof(ICMP_HEADER)+DEF_ICMP_DATA_SIZE];
	 memset(IcmpSendBuf,0,sizeof(IcmpSendBuf));
	 char IcmpRecvBuf[MAX_ICMP_PACKET_SIZE];
	 memset(IcmpRecvBuf,0,sizeof(IcmpRecvBuf));
	 ICMP_HEADER *pIcmpHeader=(ICMP_HEADER*)IcmpSendBuf;
	 pIcmpHeader->type=ICMP_ECHO_REQUEST;
	 pIcmpHeader->code = 0;
	 pIcmpHeader->id = (USHORT)GetCurrentProcessId();
	 memset(IcmpSendBuf+sizeof(ICMP_HEADER),'E',DEF_ICMP_DATA_SIZE);
	 USHORT usSeqNo=0;
	 int iTTL=64;         
	 int r=0,o=0;
	 BOOL bReachDestHost=FALSE;
	 int iMaxHot=DEF_MAX_HOP;
	 DECODE_RESULT DecodeResult;
	 clock_t maxtime = 0, mintime = 0x3f3f3f3f, sumtime = 0;
	 int sum = 4;
	 struct  hostent *ht = gethostbyname(argv[1]);
	 int i = 0;
	 while(true)
	 {
		 if (ht->h_addr_list[i] != NULL)
		 {
			 cout << "正在 Ping " << argv[1] << " [" << inet_ntoa(*((struct in_addr *)ht->h_addr_list[i])) << "] 具有 32 字节的数据：" << endl;
			 break;
		 }
		 ++i;
	 }
	 while(!bReachDestHost&&iMaxHot--)
	 {
		 BOOL timeout_flag = false;
		 clock_t start = clock();
		 setsockopt(sockRaw,IPPROTO_IP,IP_TTL,(char*)&iTTL,sizeof(iTTL)); 
		 ((ICMP_HEADER*)IcmpSendBuf)->cksum = 0;
		 ((ICMP_HEADER*)IcmpSendBuf)->seq=htons(usSeqNo++);
		 ((ICMP_HEADER*)IcmpSendBuf)->cksum=checksum((USHORT*)IcmpSendBuf,sizeof(ICMP_HEADER)+DEF_ICMP_DATA_SIZE);
		 DecodeResult.usSeqNo=((ICMP_HEADER*)IcmpSendBuf)->seq;
		 DecodeResult.dwRoundTripTime=GetTickCount();
		 sendto(sockRaw, IcmpSendBuf, sizeof(IcmpSendBuf), 0, (sockaddr*)&destSockAddr, sizeof(destSockAddr));
		 sockaddr_in from;    
		 int iFromLen=sizeof(from);
		 int iReadDataLen;
		 while(1)
		 {
			 iReadDataLen = recvfrom(sockRaw, IcmpRecvBuf, MAX_ICMP_PACKET_SIZE, 0, (sockaddr*)&from, &iFromLen);
			 if(iReadDataLen!=SOCKET_ERROR)
			 {             
				 if(DecodeIcmpResponse(IcmpRecvBuf,iReadDataLen,DecodeResult,ICMP_ECHO_REPLY,ICMP_TIMEOUT))
				 {
					 if(DecodeResult.dwIPaddr.s_addr==destSockAddr.sin_addr.s_addr){                  
					 if(DecodeResult.dwRoundTripTime)
						 cout<<"来自 "<<inet_ntoa(DecodeResult.dwIPaddr)<<" 的回复：字节=32 时间="<<DecodeResult.dwRoundTripTime<<"ms TTL="<<iTTL<<endl;        
					 else
						 cout<<"来自 "<<inet_ntoa(DecodeResult.dwIPaddr)<<" 的回复：字节=32 时间< 1ms TTL:"<<iTTL<<endl;
					 r++;
					 break;
					 }
				 }
			 }
			 else if(WSAGetLastError()==WSAETIMEDOUT)
			 {
				 cout<<"请求超时。"<<endl;
				 timeout_flag = true;
				 o++;             
				 break;
			 }
			 else
			 {
				 break;
			 }
		 }
		 clock_t end = clock();
		 clock_t time = end - start;
		 if (!timeout_flag)
		 {
			 sumtime += time;
			 if (time > maxtime)
				 maxtime = time;
			 if (time < mintime)
				 mintime = time;
		 }
		 else
			 sum--;
		 Sleep(1000);
	 }
	 if (o != 4)
	 {
		 cout << endl;
		 cout << inet_ntoa(DecodeResult.dwIPaddr) << " 的 Ping 统计信息：" << endl;
		 cout << "    数据包：已发送 = 4，已接收 = " << r << "，丢失 = " << o << " (" << o * 100 / 4 << "% 丢失)，" << endl;
		 cout << "往返行程的估计时间(以毫秒为单位):" << endl;
		 cout << "    最短 = " << mintime << "ms，最长 = " << maxtime << "ms，平均 = ";
		 if (sum != 0)
			 cout << sumtime / sum;
		 else
			 cout << "0";
		 cout << "ms" << endl;
	 }
}
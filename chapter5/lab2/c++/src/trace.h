#ifndef _ITRACERT_H_
#define _ITRACERT_H_
#pragma pack(1)
//IP���ݱ�ͷ
typedef struct
{
	unsigned char hdr_len :4;  // length of the header
	unsigned char version :4;  // version of IP
	unsigned char tos;   // type of service
	unsigned short total_len;  // total length of the packet
	unsigned short identifier;  // unique identifier
	unsigned short frag_and_flags; // flags
	unsigned char ttl;   // time to live
	unsigned char protocol;  // protocol (TCP, UDP etc)
	unsigned short checksum;  // IP checksum
	unsigned long sourceIP;  // source IP address
	unsigned long destIP;   // destination IP address
} IP_HEADER;
//ICMP���ݱ�ͷ
typedef struct
{
	BYTE type;  //8λ����
	BYTE code;  //8λ����
	USHORT cksum;  //16λУ���
	USHORT id;   //16λ��ʶ��
	USHORT seq;  //16λ���к�
} ICMP_HEADER;
//������
typedef struct
{
	USHORT usSeqNo;   //�����к�
	DWORD dwRoundTripTime; //����ʱ��
	in_addr dwIPaddr;  //�Զ�IP��ַ
} DECODE_RESULT;
#pragma pack()
//ICMP�����ֶ�
const BYTE ICMP_ECHO_REQUEST = 8; //�������
const BYTE ICMP_ECHO_REPLY  = 0; //����Ӧ��
const BYTE ICMP_TIMEOUT   = 11; //���䳬ʱ
const DWORD DEF_ICMP_TIMEOUT = 3000; //Ĭ�ϳ�ʱʱ�䣬��λms
const int DEF_ICMP_DATA_SIZE = 32; //Ĭ��ICMP���ݲ��ֳ���
const int MAX_ICMP_PACKET_SIZE = 1024; //���ICMP���ݱ��Ĵ�С
const int DEF_MAX_HOP = 30;    //�����վ��
USHORT GenerateChecksum(USHORT* pBuf, int iSize);
BOOL DecodeIcmpResponse(char* pBuf, int iPacketSize, DECODE_RESULT& stDecodeResult);
#endif // _ITRACERT_H_


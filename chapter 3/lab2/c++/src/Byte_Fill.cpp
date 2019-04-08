#include<iostream>
#include<string.h>
#include<math.h> 
#include"readConfigFile.h"
using namespace std;

//十六进制异或运算// 

//ASCII码中将字符转换成对应的十进制数
int char2int( char input )
{
    return input>64?(input-55):(input-48);
}

//ASCII码中将十进制数转换成对应的字符
int int2char( char input )
{
    return input>9?(input+55):(input+48);
}

//将十六进制字符串HexStr1和HexStr2异或得到HexStr
void hexstrxor(string Str1, string Str2, char * HexStr )
{
	
	char* HexStr1 = (char*)Str1.data();
	char* HexStr2 = (char*)Str2.data();
	
    int i, iHexStr1Len, iHexStr2Len, iHexStrLenLow, iHexStrLenGap;

    iHexStr1Len = strlen( HexStr1 );
    iHexStr2Len = strlen( HexStr2 );

    //获取最小的长度
    iHexStrLenLow = iHexStr1Len<iHexStr2Len?iHexStr1Len:iHexStr2Len;

    //获取长度差值
    iHexStrLenGap = abs( iHexStr1Len-iHexStr2Len );

    //两个十六进制的字符串进行异或
    for( i=0; i<iHexStrLenLow; i++ )
    {
        *(HexStr+i) = char2int( HexStr1[i] ) ^ char2int( HexStr2[i] );
        *(HexStr+i) = int2char( *(HexStr+i) );
    }
    if( iHexStr1Len>iHexStr2Len )
        memcpy( HexStr+i, HexStr1+i, iHexStrLenGap );
    else if( iHexStr1Len<iHexStr2Len )
        memcpy( HexStr+i, HexStr2+i, iHexStrLenGap );
    *( HexStr+iHexStrLenLow+iHexStrLenGap ) = 0x00;
}

//字节填充部分 

//'7D'替换为'7D5D'
string convert_7D(string str){
	string tmp = str;
	int count = 0;
	int begin = -1;
	//找到所有包含"7D"的起始位置，并在后面插入"5D" 
	while((begin=str.find("7D",begin+1))!=string::npos)
	{
		count++;
		//每次插入2个字符之后，下一次插入向后移动两个位置	
    	tmp = tmp.insert(begin+2*count,"5D");
	}
	return tmp;
}

//'7E'替换为'7D5E' 
string convert_7E(string str){
	string tmp = str;
	int count = 0;
	int begin = -1;
	//找到所有包含"7D"的起始位置，将"7D"代替为"7D5E"
	while((begin=str.find("7E",begin+1))!=string::npos)
	{
		//将begin开始的两个字符用"7D5E"代替	 
    	tmp = tmp.replace(begin+2*count,2,"7D5E");
    	count++;
	}
	return tmp;
}


//小于20的字符，1X,0X都需要与20作异或运算，得出结果XX，前面再加上7D 

//'1X'替换为'7DXX',XX为'1X'与'20'异或的结果
string convert_1X(string str){
	string tmp = str;
	int count = 0;
	int begin = -1;
	//找到所有包含"1"的起始位置，将"1X"与"20"作异或运算代
	while((begin=str.find("1",begin+1))!=string::npos){
		//每两个字符进行一次提取，例如1110，只需提取11和10 
	 	if(begin%2==0){		
	 		char result[5]={0};
	 		//提取begin位置的两个字符与20作异或运算得到结果XX 
		    hexstrxor( str.substr(begin,2), "20", result );	
		    // 结果为7DXX 
	 		string replace="7D";
	 		replace = replace.append(result);
	 		//替换原来的字符 
			tmp=tmp.replace(begin+2*count,2,replace);
			count++;
		} 
	 }
	return tmp;
} 

//'0X'替换为'7DXX',XX为'0X'与'20'异或的结果，同上 
string convert_0X(string str){
	string tmp = str;
	int count = 0;
	int begin = -1;

	while((begin=str.find("0",begin+1))!=string::npos){
	 	if(begin%2==0){		
	 		char result[5]={0};
		    hexstrxor( str.substr(begin,2), "20", result );	
	 		string replace="7D";
	 		replace = replace.append(result);
			tmp=tmp.replace(begin+2*count,2,replace);
			count++;
		} 
	 }
	 return tmp;
}

string Byte_Fill(string str){
	//注意先转换7D 
	string str1=convert_7D(str);
	string str2=convert_7E(str1);
	string str3=convert_1X(str2);
	string str4=convert_0X(str3);
	return str4;
}

int main(int argc,char* argv[]){
	
	string InfoString="";
	char* ini_path="../bin/Byte_Fill_data.ini";
	if(argc==2)
	{
		ini_path=argv[1];
	}
	readConfigFile(ini_path,"InfoString",InfoString);
	cout<<"-------------读取配置文件中待发送的数据信息-----"<<endl;
	cout<<InfoString<<endl;
	//字节填充 
	cout<<"-------------字节填充之后的数据信息-------------"<<endl;
	string Fill_string = Byte_Fill(InfoString); 
	cout<<Fill_string<<endl;
	cout<<"-------------帧首部的四部分---------------------"<<endl;
	cout<<"帧起始标志:7E  "<<"A:FF  "<<"C:03  "<<"协议：0021 或 C021 或 8021"<<endl;
	cout<<"-------------帧尾部的两部分---------------------"<<endl;
	cout<<"FCS:XX  "<<"帧结束标志：7E"<<endl; 
    cout<<"-------------字节填充后的发送帧为---------------"<<endl;
    cout<<"若为IP数据报：                "<<"7E FF 03 0021 "<<Fill_string<<" XX"<<" 7E"<<endl;
    cout<<"若为ppp链路控制协议LCP的数据："<<"7E FF 03 C021 "<<Fill_string<<" XX"<<" 7E"<<endl;
    cout<<"若为网络层的控制数据：        "<<"7E FF 03 8021 "<<Fill_string<<" XX"<<" 7E"<<endl;
    cout<<"-------------字节删除后的接收帧为---------------"<<endl;
    cout<<"若为IP数据报：                "<<"7E FF 03 0021 "<<InfoString<<" XX"<<" 7E"<<endl;
    cout<<"若为ppp链路控制协议LCP的数据："<<"7E FF 03 C021 "<<InfoString<<" XX"<<" 7E"<<endl;
    cout<<"若为网络层的控制数据：        "<<"7E FF 03 8021 "<<InfoString<<" XX"<<" 7E"<<endl;
    
    cout<<"-------------按任意键退出程序-------------------"<<endl;
    getchar();
    return 0;
} 

#include<iostream>
#include<string.h>
#include<math.h> 
#include"readConfigFile.h"
using namespace std;

//ʮ�������������// 

//ASCII���н��ַ�ת���ɶ�Ӧ��ʮ������
int char2int( char input )
{
    return input>64?(input-55):(input-48);
}

//ASCII���н�ʮ������ת���ɶ�Ӧ���ַ�
int int2char( char input )
{
    return input>9?(input+55):(input+48);
}

//��ʮ�������ַ���HexStr1��HexStr2���õ�HexStr
void hexstrxor(string Str1, string Str2, char * HexStr )
{
	
	char* HexStr1 = (char*)Str1.data();
	char* HexStr2 = (char*)Str2.data();
	
    int i, iHexStr1Len, iHexStr2Len, iHexStrLenLow, iHexStrLenGap;

    iHexStr1Len = strlen( HexStr1 );
    iHexStr2Len = strlen( HexStr2 );

    //��ȡ��С�ĳ���
    iHexStrLenLow = iHexStr1Len<iHexStr2Len?iHexStr1Len:iHexStr2Len;

    //��ȡ���Ȳ�ֵ
    iHexStrLenGap = abs( iHexStr1Len-iHexStr2Len );

    //����ʮ�����Ƶ��ַ����������
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

//�ֽ���䲿�� 

//'7D'�滻Ϊ'7D5D'
string convert_7D(string str){
	string tmp = str;
	int count = 0;
	int begin = -1;
	//�ҵ����а���"7D"����ʼλ�ã����ں������"5D" 
	while((begin=str.find("7D",begin+1))!=string::npos)
	{
		count++;
		//ÿ�β���2���ַ�֮����һ�β�������ƶ�����λ��	
    	tmp = tmp.insert(begin+2*count,"5D");
	}
	return tmp;
}

//'7E'�滻Ϊ'7D5E' 
string convert_7E(string str){
	string tmp = str;
	int count = 0;
	int begin = -1;
	//�ҵ����а���"7D"����ʼλ�ã���"7D"����Ϊ"7D5E"
	while((begin=str.find("7E",begin+1))!=string::npos)
	{
		//��begin��ʼ�������ַ���"7D5E"����	 
    	tmp = tmp.replace(begin+2*count,2,"7D5E");
    	count++;
	}
	return tmp;
}


//С��20���ַ���1X,0X����Ҫ��20��������㣬�ó����XX��ǰ���ټ���7D 

//'1X'�滻Ϊ'7DXX',XXΪ'1X'��'20'���Ľ��
string convert_1X(string str){
	string tmp = str;
	int count = 0;
	int begin = -1;
	//�ҵ����а���"1"����ʼλ�ã���"1X"��"20"����������
	while((begin=str.find("1",begin+1))!=string::npos){
		//ÿ�����ַ�����һ����ȡ������1110��ֻ����ȡ11��10 
	 	if(begin%2==0){		
	 		char result[5]={0};
	 		//��ȡbeginλ�õ������ַ���20���������õ����XX 
		    hexstrxor( str.substr(begin,2), "20", result );	
		    // ���Ϊ7DXX 
	 		string replace="7D";
	 		replace = replace.append(result);
	 		//�滻ԭ�����ַ� 
			tmp=tmp.replace(begin+2*count,2,replace);
			count++;
		} 
	 }
	return tmp;
} 

//'0X'�滻Ϊ'7DXX',XXΪ'0X'��'20'���Ľ����ͬ�� 
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
	//ע����ת��7D 
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
	cout<<"-------------��ȡ�����ļ��д����͵�������Ϣ-----"<<endl;
	cout<<InfoString<<endl;
	//�ֽ���� 
	cout<<"-------------�ֽ����֮���������Ϣ-------------"<<endl;
	string Fill_string = Byte_Fill(InfoString); 
	cout<<Fill_string<<endl;
	cout<<"-------------֡�ײ����Ĳ���---------------------"<<endl;
	cout<<"֡��ʼ��־:7E  "<<"A:FF  "<<"C:03  "<<"Э�飺0021 �� C021 �� 8021"<<endl;
	cout<<"-------------֡β����������---------------------"<<endl;
	cout<<"FCS:XX  "<<"֡������־��7E"<<endl; 
    cout<<"-------------�ֽ�����ķ���֡Ϊ---------------"<<endl;
    cout<<"��ΪIP���ݱ���                "<<"7E FF 03 0021 "<<Fill_string<<" XX"<<" 7E"<<endl;
    cout<<"��Ϊppp��·����Э��LCP�����ݣ�"<<"7E FF 03 C021 "<<Fill_string<<" XX"<<" 7E"<<endl;
    cout<<"��Ϊ�����Ŀ������ݣ�        "<<"7E FF 03 8021 "<<Fill_string<<" XX"<<" 7E"<<endl;
    cout<<"-------------�ֽ�ɾ����Ľ���֡Ϊ---------------"<<endl;
    cout<<"��ΪIP���ݱ���                "<<"7E FF 03 0021 "<<InfoString<<" XX"<<" 7E"<<endl;
    cout<<"��Ϊppp��·����Э��LCP�����ݣ�"<<"7E FF 03 C021 "<<InfoString<<" XX"<<" 7E"<<endl;
    cout<<"��Ϊ�����Ŀ������ݣ�        "<<"7E FF 03 8021 "<<InfoString<<" XX"<<" 7E"<<endl;
    
    cout<<"-------------��������˳�����-------------------"<<endl;
    getchar();
    return 0;
} 

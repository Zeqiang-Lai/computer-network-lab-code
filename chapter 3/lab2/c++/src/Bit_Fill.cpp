#include<iostream>
#include<string.h>
#include"readConfigFile.h"
using namespace std;

//0������亯�� 
string Bit_Fill(string str){
	string tmp = str;
	int count = 0;//��¼11111���ֵĴ��� 
	int begin = -1;
	while((begin=str.find("11111",begin+1))!=string::npos)
	{
		//����0��λ��Ϊ����ʼλ��+5+count 
    	tmp = tmp.insert(begin+5+count,"0");
    	count++;
    	begin = begin+4;
	}
	return tmp;
}

int main(int argc,char* argv[]){
	string InfoString="";
	char* ini_path="../bin/Bit_Fill_data.ini";
	if(argc==2)
	{
		ini_path=argv[1];
	}
	readConfigFile(ini_path,"InfoString",InfoString);
	cout<<"-------------��ȡ�����ļ��д����͵�������Ϣ-----"<<endl;
	cout<<InfoString<<endl;
	cout<<"-------------�������֮���������Ϣ-------------"<<endl;
	string Fill_string = Bit_Fill(InfoString);
	cout<<Fill_string<<endl;
	cout<<"-------------֡�ײ����Ĳ���---------------------"<<endl;
	cout<<"֡��ʼ��־:01111110"<<endl;
	cout<<"A:11111111"<<endl;
	cout<<"C:00000011"<<endl;
	cout<<"Э�飺0000000000100001 �� 1010000000100001 �� 1000000000100001"<<endl;
	cout<<"-------------֡β����������---------------------"<<endl;
	cout<<"FCS:XXXXXXXX"<<endl;
	cout<<"֡������־��01111110"<<endl; 
    cout<<"-------------��������ķ���֡Ϊ---------------"<<endl;
    cout<<"��ΪIP���ݱ���                "<<"01111110 11111111 00000011 0000000000100001 "<< Fill_string <<" XXXXXXXX"<<" 01111110"<<endl;
    cout<<"��Ϊppp��·����Э��LCP�����ݣ�"<<"01111110 11111111 00000011 1010000000100001 "<< Fill_string <<" XXXXXXXX"<<" 01111110"<<endl;
    cout<<"��Ϊ�����Ŀ������ݣ�        "<<"01111110 11111111 00000011 1000000000100001 "<< Fill_string <<" XXXXXXXX"<<" 01111110"<<endl;
	cout<<"-------------����ɾ����Ľ���֡Ϊ---------------"<<endl;
	cout<<"��ΪIP���ݱ���                "<<"01111110 11111111 00000011 0000000000100001 "<< InfoString <<" XXXXXXXX"<<" 01111110"<<endl;
    cout<<"��Ϊppp��·����Э��LCP�����ݣ�"<<"01111110 11111111 00000011 1010000000100001 "<< InfoString <<" XXXXXXXX"<<" 01111110"<<endl;
    cout<<"��Ϊ�����Ŀ������ݣ�        "<<"01111110 11111111 00000011 1000000000100001 "<< InfoString <<" XXXXXXXX"<<" 01111110"<<endl;
    
	cout<<"-------------��������˳�����-------------------"<<endl;
    getchar();
	return 0;
} 

s#include<iostream>
#include<string.h>
#include"readConfigFile.h"
using namespace std;

//0比特填充函数 
string Bit_Fill(string str){
	string tmp = str;
	int count = 0;//记录11111出现的次数 
	int begin = -1;
	while((begin=str.find("11111",begin+1))!=string::npos)
	{
		//插入0的位置为，起始位置+5+count 
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
	cout<<"-------------读取配置文件中待发送的数据信息-----"<<endl;
	cout<<InfoString<<endl;
	cout<<"-------------比特填充之后的数据信息-------------"<<endl;
	string Fill_string = Bit_Fill(InfoString);
	cout<<Fill_string<<endl;
	cout<<"-------------帧首部的四部分---------------------"<<endl;
	cout<<"帧起始标志:01111110"<<endl;
	cout<<"A:11111111"<<endl;
	cout<<"C:00000011"<<endl;
	cout<<"协议：0000000000100001 或 1010000000100001 或 1000000000100001"<<endl;
	cout<<"-------------帧尾部的两部分---------------------"<<endl;
	cout<<"FCS:XXXXXXXX"<<endl;
	cout<<"帧结束标志：01111110"<<endl; 
    cout<<"-------------比特填充后的发送帧为---------------"<<endl;
    cout<<"若为IP数据报：                "<<"01111110 11111111 00000011 0000000000100001 "<< Fill_string <<" XXXXXXXX"<<" 01111110"<<endl;
    cout<<"若为ppp链路控制协议LCP的数据："<<"01111110 11111111 00000011 1010000000100001 "<< Fill_string <<" XXXXXXXX"<<" 01111110"<<endl;
    cout<<"若为网络层的控制数据：        "<<"01111110 11111111 00000011 1000000000100001 "<< Fill_string <<" XXXXXXXX"<<" 01111110"<<endl;
	cout<<"-------------比特删除后的接受帧为---------------"<<endl;
	cout<<"若为IP数据报：                "<<"01111110 11111111 00000011 0000000000100001 "<< InfoString <<" XXXXXXXX"<<" 01111110"<<endl;
    cout<<"若为ppp链路控制协议LCP的数据："<<"01111110 11111111 00000011 1010000000100001 "<< InfoString <<" XXXXXXXX"<<" 01111110"<<endl;
    cout<<"若为网络层的控制数据：        "<<"01111110 11111111 00000011 1000000000100001 "<< InfoString <<" XXXXXXXX"<<" 01111110"<<endl;
    
	cout<<"-------------按任意键退出程序-------------------"<<endl;
    getchar();
	return 0;
} 

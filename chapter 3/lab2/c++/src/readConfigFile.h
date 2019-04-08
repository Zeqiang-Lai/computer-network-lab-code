#include <iostream>
#include <string>
#include <fstream>
using namespace std;
string readConfigFile(const char * cfgfilepath, const string & key, string & value)
{
    fstream cfgFile;
    cfgFile.open(cfgfilepath);//打开文件
    if( ! cfgFile.is_open())
    {
        cout<<"can not open cfg file!"<<endl;
    }
    string line;
    while(getline(cfgFile, line))//循环读取每一行
    {
        size_t pos = line.find('=');//找到每行的“=”号位置，之前是key之后是value
        if(pos==string::npos) return NULL;
        string tmpKey = line.substr(0,pos);//取=号之前
        if(key==tmpKey)
        {
            value = line.substr(pos+1);//取=号之后
			return value;
        }
    }

}



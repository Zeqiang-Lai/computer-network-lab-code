# -*- coding: utf-8 -*-

import sys
import readConfigFile
import string


#0bit填充函数
def Bit_Fill(str1):
    tmp = str1
    count = 0
    begin = -1
    while True:
        #插入0的位置
        begin = str1.find("11111",begin+1,len(str1))
        if begin == -1:
            break

        else:
            tmp = tmp[:begin+5+count]+'0'+tmp[begin+5+count:]
            count += 1
            begin = begin+4
        
    return tmp

if __name__ == "__main__":
    InfoString = ""
    ini_path = "./Bit_Fill_data.ini"
    if len(sys.argv)==2:
        ini_path = sys.argv[1]
    
    InfoString = readConfigFile.readConfigFile(ini_path,"InfoString",InfoString)
    InfoString = InfoString[:32]
    print("-------------读取配置文件中待发送的数据信息-----")
    print(InfoString)
    print("-------------比特填充之后的数据信息-------------")
    Fill_string = Bit_Fill(InfoString)
    print(Fill_string)
    print("-------------帧首部的四部分---------------------")
    print("帧起始标志:01111110")
    print("A:11111111")
    print("C:00000011")
    print("协议：0000000000100001 或 1010000000100001 或 1000000000100001")
    print("-------------帧尾部的两部分---------------------")
    print("FCS:XXXXXXXX")
    print("帧结束标志：01111110") 
    print("-------------比特填充后的发送帧为---------------")
    print("若为IP数据报：                ","01111110 11111111 00000011 0000000000100001 ", Fill_string ," XXXXXXXX"," 01111110")
    print("若为ppp链路控制协议LCP的数据：","01111110 11111111 00000011 1010000000100001 ", Fill_string ," XXXXXXXX"," 01111110")
    print("若为网络层的控制数据：        ","01111110 11111111 00000011 1000000000100001 ", Fill_string ," XXXXXXXX"," 01111110")
    print("-------------比特删除后的接受帧为---------------")
    print("若为IP数据报：                ","01111110 11111111 00000011 0000000000100001 ", InfoString ," XXXXXXXX"," 01111110")
    print("若为ppp链路控制协议LCP的数据：","01111110 11111111 00000011 1010000000100001 ", InfoString ," XXXXXXXX"," 01111110")
    print("若为网络层的控制数据：        ","01111110 11111111 00000011 1000000000100001 ", InfoString ," XXXXXXXX"," 01111110")
    
    print("-------------按任意键退出程序-------------------")
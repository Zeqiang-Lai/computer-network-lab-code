# -*- coding: utf-8 -*-

import socket
import struct
import time 
import readConfigFile
import math
import sys


#字符串转为整数
#int(x, base=10)

#整数转为字符串
#chr(x)


#将16进制字符串Str1，Str2异或得到Hexstr
def hexstrxor (str1, str2, HexStr):

    #获取字符串长度
    iHexStr1Len = len(str1)
    iHexStr2Len = len(str2)

    #获取最小长度
    iHexStrLenLow = min (iHexStr1Len,iHexStr2Len)

    #获取长度差值
    iHexStrLenGap = abs(iHexStr1Len-iHexStr2Len)

    i=0
    #两个十六进制字符串异或
    for i in iHexStrLenLow:
        HexStr [iHexStrLenLow+i] = chr(int(str1[i])^int(str2[i]))

    #用长度大的字符串补足新建字符串
    if iHexStr1Len>iHexStr2Len:
        for j in range (i,iHexStr1Len):
            HexStr[j] = str1[j]
    else:
        for j in range (i,iHexStr2Len):
            HexStr[j] = str2[j]
    
    #????这句的作用是将十进制转换程16进制吗？括号里为什么要加low和gap？C++
    hex(HexStr)



#'7D'替换为'7D5D'
def convert_7D(str1):
    tmp = str1
    count = 0
    begin = -1

    #找到所有包含"7D"的起始位置，将“7D”换为“7D5E”,7E还是7D?
    while True:
        begin = str1.find("7D",begin+1,len(str1))
        if begin == -1:
            break
        count +=1
        #tmp.insert(begin+2*count,"5D")
        tmp[:begin+2*count]+'5D'+ tmp[begin+2*count:]  

    return tmp


#7E替换为7D5E
def convert_7E(str1):
    tmp = str1
    count = 0
    begin = -1

    #找到所有包含"7D"的起始位置，将“7D”换为“7D5E”,7E还是7D?
    while  True:
        begin = str1.find("7E",begin+1,len(str1))
        if begin ==-1:
            break
        
        tmp = tmp.replace(tmp[begin+2*count:begin+2*count+2],"7D5E")   
        count += 1

    return tmp

#小于20的字符，1X,0X都需要与20作异或运算，得出结果XX，前面再加上7D

#'1X'替换为'7DXX',XX为'1X'与'20'异或的结果
def convert_1X(str1):
    tmp = str1
    count = 0
    begin = -1
    while  True:
        begin = str1.find("1",begin+1,len(str1))
        if begin ==-1:
            break
        
        #每两个字符进行一次提取
        if begin%2 == 0:
            #定义一个5个元素的数组
            result=[0]*5
            
            #提取begin位置的两个字符与20作异或运算得到结果XX
            hexstrxor(str1[begin:2],"20",result)
            
            #结果为7DXX
            replace = "7D"
            replace = replace+result
            
            #替换原来字符
            tmp = tmp.replace(tmp[begin+2*count:begin+2*count+2],replace)
            count +=1
        
    return tmp


#'0X'替换为'7DXX',XX为'0X'与'20'异或的结果，同上 
def convert_0X(str1):
    tmp = str1
    count = 0
    begin = -1

    while  True:
        begin = str1.find("0",begin+1,len(str1))
        if begin ==-1:
            break
        if begin%2 == 0:
            
            result=[0]*5
            
            hexstrxor(str1[begin:2],"20",result)
            
            replace = "7D"
            replace = replace+result

            tmp = tmp.replace(tmp[begin+2*count:begin+2*count+2],replace)

            count+=1
        
    return tmp


def Byte_Fill(str0):
    str1 = convert_7D(str0)
    str2 = convert_7E(str1)
    str3 = convert_1X(str2)
    str4 = convert_0X(str3)
    return str4


if __name__ == "__main__":
    InfoString = ""
    ini_path="./Byte_Fill_data.ini"
    if len(sys.argv) == 2:
        ini_path = sys.argv[1]
    
    InfoString = readConfigFile.readConfigFile(ini_path,"InfoString",InfoString)
    InfoString = InfoString[:16]
    print (len(InfoString))
    print ("-------------字节填充之后的数据信息-------------")
    Fill_string = Byte_Fill(InfoString)
    print (Fill_string) 
    print ("-------------帧首部的四部分---------------------")
    print ("帧起始标志:7E  ","A:FF  ","C:03  ","协议：0021 或 C021 或 8021")	
    print ("-------------帧尾部的两部分---------------------")
    print ("FCS:XX  ","帧结束标志：7E" )
    print ("-------------字节填充后的发送帧为---------------")
    print ("若为IP数据报：                "+"7E FF 03 0021" ,Fill_string , " XX", " 7E")
    print ("若为ppp链路控制协议LCP的数据：","7E FF 03 C021 ", Fill_string , " XX"," 7E")
    print ("若为网络层的控制数据：        ","7E FF 03 8021 ",Fill_string," XX"," 7E")
    print ("-------------字节删除后的接收帧为---------------")
    print ("若为IP数据报：                ","7E FF 03 0021 ",InfoString," XX"," 7E")
    print ("若为ppp链路控制协议LCP的数据：","7E FF 03 C021 ",InfoString," XX"," 7E")
    print ("若为网络层的控制数据：        ","7E FF 03 8021 ",InfoString," XX"," 7E")
    
    print ("-------------按任意键退出程序-------------------")

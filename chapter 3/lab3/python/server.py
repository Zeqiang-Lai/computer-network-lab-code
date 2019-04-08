# -*- coding: utf-8 -*-
import socket
import struct
import time
import crc
import random
'''
以下读取配置文件，并初始化变量
'''
init = crc.read_configuration('crc.ini')
poly = ''.join(init[3]).replace("\n", "")
ip=init[6].replace('\n', '').split('=')[1]
port=int(init[7].split('=')[1])
framerro=int(init[9].split('=')[1])
ip_port=(ip,port)
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind(ip_port)
print('Bind UDP on %d...'%port)
frame_expect=0
count=0#用于模拟帧出错的计数器
while True:
    count=(count+1)%framerro
    #接受数据并解析
    frame, addr=s.recvfrom(1024)
    frame=struct.unpack('h100s', frame)
    if frame[0]==frame_expect:
        data=str(frame[1], 'utf-8').replace('\0', '')
        if count==framerro-1:#每framerro次模拟一次出错,可以是改变接受数据的某一位
            ans = list(data)
            ans[1] = crc.XOR(ans[1], "1")
            data = ''.join(ans)
        print('收到编号:' + str(frame[0]) + ',帧内容:' + data)
        if int(crc.crc_check(data,poly))==0:#正确返回ACK并修改frame_expect
            ack = struct.pack('h3s', frame_expect, b'ACK')
            s.sendto(ack, addr)
            frame_expect = 1 - frame_expect
        else:#错误返回NAK并显示CRC校验余数
            nak = struct.pack('h3s', frame_expect, b'NAK')
            s.sendto(nak, addr)
            print("帧出错,CRC校验余数:"+crc.crc_check(data,poly))


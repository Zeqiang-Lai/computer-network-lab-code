# -*- coding: utf-8 -*-
import socket
import struct
import  threading
import  crc
import time
import random

def send_repeat(seq,data,ip_addr):#处理重传
    print('time_out!')
    print("retransmit frame %d"%seq)
    frame=struct.pack('h100s',next_frame_to_send,bytes(data,'utf-8')) #成帧
    s.sendto(frame,ip_addr)
    timer.cancel()
'''
以下读取配置文件，并初始化变量
'''
init = crc.read_configuration('crc.ini')
ip=init[6].replace('\n', '').split('=')[1]
port=int(init[7].split('=')[1])
filelost=int(init[8].split('=')[1])
framerro=int(init[9].split('=')[1])
send_string = ''.join(init[4]).replace("\n", "")
poly = ''.join(init[3]).replace("\n", "")
ip_port=(ip,port)#IP地址及端口号
next_frame_to_send=0#要发送的帧
count1=0#用于模拟帧丢失、传输出错的计数器
count2=0
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
global timer#计时器
while True:
    count1=(count1+1)%filelost#count模filelost累加
    count2=(count2+1)%framerro
    #计算校验码
    crc_code_remainder = crc.crc_remainder(send_string, poly)
    data = send_string + crc_code_remainder
    datacopy=data
    if count2 == framerro - 1:  # 每framerro次模拟一次出错,可以是改变接受数据的某一位
        data=list(data)
        data[1]=crc.XOR(data[1],"1")
        data=''.join(data)
        count2=(count2+1)%framerro
    frame=struct.pack('h100s',next_frame_to_send,bytes(data,'utf-8')) #成帧
    timer=threading.Timer(5, send_repeat, [next_frame_to_send,datacopy, ip_port])#设定计时器,5秒无回复则重传
    # print('即将发送数据帧：%d:%s'%(next_frame_to_send,data))
    timer.start()
    if count1==filelost-3:#每filelost次模拟一次帧丢失
        time.sleep(5)
    else:
        print('next_frame_to_send:%d ' % next_frame_to_send)
        s.sendto(frame, ip_port)
    time.sleep(1)
    #接受数据并解析
    recv = s.recv(1024)
    recv = struct.unpack('h3s', recv)
    print(str(recv[1], 'utf-8') + str(recv[0]))
    if (recv[0] == next_frame_to_send) & (recv[1]==b'ACK'):#ACK,修改next_frame_to_send
        #send_string += random.choice(['0','1',''])---允许在此处修改下次传送数据
        next_frame_to_send = 1-next_frame_to_send
        timer.cancel()
    else:
        time.sleep(5)




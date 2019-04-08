# -*- coding: utf-8 -*-
import socket
import struct
import  threading
import  crc
import time
import random
'''
客户端使用UDP时，首先仍然创建基于UDP的Socket，然后，不需要调用connect()，直接通过sendto()给服务器发数据：
'''
def send_repeat(seq,data,ip_addr):#处理重传
    print('帧丢失!!')
    s.sendto(frame,ip_addr)
    print("重传帧%d:%s 循环计数%d"%(seq,data,count))
    timer.cancel()
'''
以下读取配置文件，并初始化变量
'''
init = crc.read_configuration('crc.ini')
ip=init[6].replace('\n', '').split('=')[1]
port=int(init[7].split('=')[1])
filelost=int(init[8].split('=')[1])
send_string = ''.join(init[4]).replace("\n", "")
poly = ''.join(init[3]).replace("\n", "")
ip_port=(ip,port)#IP地址及端口号
next_frame_to_send=0#要发送的帧
count=0#用于模拟帧丢失的计数器
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
global timer#计时器
while True:
    count=(count+1)%filelost#count模filelost累加
    #计算校验码
    crc_code_remainder = crc.crc_remainder(send_string, poly)
    data = send_string + crc_code_remainder

    frame=struct.pack('h100s',next_frame_to_send,bytes(data,'utf-8')) #成帧

    timer=threading.Timer(5, send_repeat, [next_frame_to_send,data, ip_port])#设定计时器,5秒无回复则重传
    # print('即将发送数据帧：%d:%s'%(next_frame_to_send,data))
    timer.start()
    if count==filelost-3:#每filelost次模拟一次帧丢失
        time.sleep(5)
    else:
        s.sendto(frame, ip_port)
        print('已发送数据帧：%d:%s 循环计数%d' % (next_frame_to_send, data,count))
    time.sleep(1)
    #接受数据并解析
    recv = s.recv(1024)
    recv = struct.unpack('h3s', recv)
    print(str(recv[1], 'utf-8') + str(recv[0]))
    if (recv[0] == next_frame_to_send) & (recv[1]==b'ACK'):#ACK,修改next_frame_to_send
        #send_string += random.choice(['0','1',''])---允许在此处修改下次传送数据
        next_frame_to_send = 1-next_frame_to_send
        timer.cancel()
    elif recv[1]==b'NAK':#NAK,帧出错
        timer.cancel()




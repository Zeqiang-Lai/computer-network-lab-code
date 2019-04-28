# -*- coding: utf-8 -*-
import socket
import struct
import time
import crc
import random
def inc(n):
    if n<MAX_SEQ:
        n+=1
    else:
        n=0
    return n
'''
以下读取配置文件，并初始化变量
'''

init = crc.read_configuration('lab_4.ini')
send_string = ''.join(init[4]).replace("\n", "")
poly = ''.join(init[3]).replace("\n", "")
ip=init[6].replace('\n', '').split('=')[1]
port=int(init[7].split('=')[1])
framerro=int(init[9].split('=')[1])
ip_port=(ip,port)
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind(ip_port)
print('Bind UDP on %d...'%port)
MAX_SEQ=7
ack_expected=MAX_SEQ
frame_expect=0
frame_seq=0
# count=0#用于模拟帧出错的计数器
while True:
    # count=(count+1)%framerro
    #接受数据并解析
    print('---------------------')
    frame, addr=s.recvfrom(1024)
    frame=struct.unpack('hh100s', frame)
    print("frame_expected: %d,ack_expected: %d" % (frame_expect,ack_expected))
    if frame[0]==frame_expect:
        data=str(frame[2], 'utf-8').replace('\0', '')
        print("received frame:",frame[0],",ack:",frame[1])
        print('Checking...')
        if int(crc.crc_check(data,poly))==0:#正确返回ACK并修改frame_expect
            # frame_expect=(frame_expect+MAX_SEQ)%(MAX_SEQ)
            ack = struct.pack('hh100s',frame_seq, frame_expect,bytes(send_string,'utf-8'))
            print('frame: %d is valid'% frame[0])
            s.sendto(ack, addr)
            print('sended frame %d,ack %d'%(frame_seq,frame_expect))
            if ack_expected==frame[1]:
                ack_expected=inc(ack_expected)
            frame_expect=inc(frame_expect)
            frame_seq=inc(frame_seq)
        else:#错误,显示CRC校验余数,不回复
            # nak = struct.pack('hh100s', 0,frame_expect, b'NAK')
            # s.sendto(nak, addr)
            print("Invalid:CRC=["+crc.crc_check(data,poly)+"]")


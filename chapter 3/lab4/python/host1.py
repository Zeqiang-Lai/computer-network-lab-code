import socket
import time
import threading
import struct
import crc
import queue
'''
以下为各参数初始化过程
'''
init = crc.read_configuration('lab_4.ini')
ip=init[6].replace('\n', '').split('=')[1]
port=int(init[7].split('=')[1])
filelost=int(init[8].split('=')[1])#帧丢失频率
framerro=int(init[9].split('=')[1])#帧出错频率
send_string = ''.join(init[4]).replace("\n", "")
poly = ''.join(init[3]).replace("\n", "")
ip_port=(ip,port)#IP地址及端口号
# ip_portsender=('127.0.0.1',8888)
# ip_portreceiver=('127.0.0.1',9999)
s=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
# s.bind(ip_portsender)
ack_expected=0
next_frame_to_send=0
frame_expected=0
nbuffered=0
buffer=['01101111011011010000101000101101']*9 #待发送消息缓冲区
rece_frame=''
MAX_SEQ=7
case=0
'''
case=0,无任何事件
case=1,超时
case=2,收到信息
'''
count_lost=0
count_erro=0
que=queue.Queue()

sema=threading.Semaphore(0)#设置信号量，用于wait_event()
def inc(n):
    if n<MAX_SEQ:
        n+=1
    else:
        n=0
    return n
def between(a,b,c):
    if(((a<=b)&(b<c))|((c<a)&(a<=b))|((b<c)&(c<a))):
        return True
    else:
        return False
def send_data(frame_nr,frame_expected,buffer):#正常发送
    time.sleep(2)
    crc_code_remainder = crc.crc_remainder(buffer[frame_nr], poly)
    global count_erro
    seq = frame_nr
    # ack=frame_expected
    ack = (frame_expected + MAX_SEQ) % (MAX_SEQ + 1)
    data = send_string + crc_code_remainder
    print('待发送帧:[Num=%d],[Ack=%d]'%(frame_nr,ack))
    if count_erro==5:
        data = list(send_string)
        data[1] = crc.XOR(data[1], "1")
        data = ''.join(data)
        count_erro = (count_erro + 1) % framerro
        print("模拟：传输出错")
    else:
        print("模拟：正确发送")
    frame=struct.pack('hh100s',seq,ack,bytes(data,'utf-8'))#seq+ack+data
    s.sendto(frame,ip_port)
    start_timer(frame_nr)
def send_data2(frame_nr,frame_expected,buffer):#处理重传
    time.sleep(2)
    crc_code_remainder = crc.crc_remainder(buffer[frame_nr], poly)
    data = send_string + crc_code_remainder
    seq=frame_nr
    # ack=frame_expected
    ack=(frame_expected+MAX_SEQ)%(MAX_SEQ+1)
    frame=struct.pack('hh100s',seq,ack,bytes(data,'utf-8'))#seq+ack+data
    s.sendto(frame,ip_port)
    print('待发送帧:[Num=%d],[Ack=%d]'%(frame_nr,ack))
    # start_timer(frame_nr)
def start_timer(frame_nr):
    timer=threading.Timer(2.1,time_out)#设置超时时间
    timer_list[frame_nr]=timer
    # que.put_nowait(timer)
    timer.start()
def time_out():
    global case
    case=1
    sema.release()
def recv_frame():
    while True:
        recv, addr = s.recvfrom(1024)
        recv = struct.unpack('hh100s', recv)  # frame=seq+ack+data
        global rece_frame
        rece_frame = recv
        global case
        case = 2
        sema.release()

rec=threading.Thread(target=recv_frame)
# rec.start()
recflag=True
timer_list=[threading.Timer(4,time_out)]*20 #计时器列表
while True:
    count_lost=(count_lost+1)%filelost
    count_erro=(count_erro+1)%framerro
    print('---------发送------------')
    if nbuffered<MAX_SEQ:
        nbuffered += 1
        # if count_lost==3:
        #     next_frame_to_send+=1
        print("Ack_expected=%d,Next_frame_to_send=%d,frame_expected=%d"%(ack_expected,next_frame_to_send,frame_expected))
        send_data(next_frame_to_send, frame_expected, buffer)
        next_frame_to_send = inc(next_frame_to_send)
        # print("frame_expected: %d"%frame_expected)
        # print("ack_expected: %d"%ack_expected)
    if recflag:
        rec.start()
        recflag=False
    sema.acquire() #wait_event()
    if case==2:#收到消息
        print('---------接受------------')
        case=0

        if rece_frame[0]==frame_expected :#此处可加CRC校验
            print('收到帧：[Num=%d] [Ack=%d]'%(rece_frame[0],rece_frame[1]))
            print('frame_expected=%d,Ack_expected=%d'%(frame_expected,ack_expected))
            data = str(rece_frame[2], 'utf-8').replace('\0', '')
            if int(crc.crc_check(data, poly)) == 0:  # 正确返回ACK并修改frame_expect
                print("CRC校验失败")
            else:
                print("CRC校验成功")
            frame_expected=inc(frame_expected)
            # print("frame_ack:",rece_frame[1])
        while between(ack_expected,rece_frame[1],next_frame_to_send):
            # print('nbuffered:',nbuffered)
            nbuffered-=1
            timer_list[ack_expected].cancel()
            # que.get().cancel()
            # print('活跃线程数：',threading.activeCount())
            ack_expected=inc(ack_expected)

    elif case==1:#超时
        print('---------超时------------')
        case=0
        print("Ack_expected=%d,Next_frame_to_send=%d,frame_expected=%d"%(ack_expected,next_frame_to_send,frame_expected))
        next_frame_to_send=ack_expected
        for i in range(1,nbuffered+1):
            send_data2(next_frame_to_send,frame_expected,buffer)
            next_frame_to_send=inc(next_frame_to_send)




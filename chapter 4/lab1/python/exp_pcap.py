#coding=utf-8

import psutil
#安装第三方库psutil
#获取网卡名称和其ip地址，不包括回环
def get_netcard():
    netcard_info = []
    info = psutil.net_if_addrs()
    for k,v in info.items():
        for item in v:
            if item[0] == 2 and not item[1]=='127.0.0.1':
                #k为网卡名字，item[1]是IP地址，下面的address是MAC地址？
                netcard_info.append(k)
            address = item[1]
            if '-' in address and len(address)==17:
                print (address)
    j=0
    for i in netcard_info:
        print(j, i)
        j+=1
    #选择所需的网卡
    print ("choose the netcard you want")
    netcard_num = int(input())
    ifname = netcard_info[netcard_num]
    print ifname
    return ifname 


from scapy.sendrecv import sniff
from scapy.utils import wrpcap
from scapy.all import *
import pcap
import dpkt
import socket
import datetime
import string 



def get_pcap(ifs,ip=None,size=100):
    # 获取指定 ifs(网卡), 指定数量size 的数据包;
    # 如果有指定ip，则这里只接收tcp，80端口，指定ip的包 #
    filter = ""
    if ip:
        filter += "ip src %s and tcp and tcp port 80" %ip
        dpkt = sniff(iface=ifs,filter=filter,count=size)
    else:
        dpkt = sniff(iface=ifs,count=size)
    wrpcap("demo.pcap",dpkt) # 保存数据包到文件
    
    return dpkt



# def printPcap(pcap):
#     try:
#         for timestamp, buf in pcap:
#             eth = dpkt.ethernet.Ethernet(buf) #获得以太包，即数据链路层包
#             print("ip layer:"+eth.data.__class__.__name__) #以太包的数据既是网络层包
#             print("tcp layer:"+eth.data.data.__class__.__name__) #网络层包的数据既是传输层包
#             print("http layer:" + eth.data.data.data.__class__.__name__) #传输层包的数据既是应用层包
#             print('Timestamp: ',str(datetime.datetime.utcfromtimestamp(timestamp))) #打印出包的抓取时间
#             #if isinstance(eth.data, dpkt.ip.IP) is not None :
#             #    print('Non IP Packet type not supported %s' % eth.data.__class__.__name__)
#             continue
        
#     except:
#         pass

    

def main():
    ifs = get_netcard()
    print ("waiting for windows to listen to "+ ifs)
    dpkt1 = get_pcap(ifs)
    print dpkt1
    pcaps = rdpcap("demo.pcap")
    packet = pcaps[0]
    print('pcaket-show:',packet.show())
    #f =open('demo.pcap','rb')
    #pcap = dpkt.pcap.Reader(f)
    #printPcap(pcap)


if __name__ =='__main__':
    main()
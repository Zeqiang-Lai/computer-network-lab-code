#include "stdafx.h"
#define HAVE_REMOTE

#ifndef _XKEYCHECK_H
#define _XKEYCHECK_H
#endif
#pragma comment(lib, "Packet")
#pragma comment(lib, "wpcap")
#pragma comment(lib, "WS2_32")

#include <pcap.h>
#include <Packet32.h>
#include <ntddndis.h>
#include <remote-ext.h>
#include <iostream>
#include <iomanip> 
#include <cstdio>
#include <time.h>
#include <cstdlib>
using namespace std;
#define threshold 1024*1024
/* IP帧格式 20字节*/
typedef struct ip_header {
	u_char ver_ihl;				//Version (4 bits) + Internet header length (4 bits)
	u_char tos;					//Type of service
	u_short tlen;				//Total length
	u_short identification;		//Identification
	u_short flags_fo;			//Flags (3 bits) + Fragment offset (13 bits)
	u_char ttl;					//Time to live
	u_char proto;				//Protocol
	u_short crc;				//Header checksum
	u_char saddr[4];			//Source address
	u_char daddr[4];			//Destination address
} ip_header;

/* 数据链路层帧格式 14字节*/
typedef struct mac_header {
	u_char dest_addr[6];
	u_char src_addr[6];
	u_char type[2];
} mac_header;

/*传输层格式 8字节*/
typedef struct udp_header{
    u_short sport;          // 源端口(Source port)
    u_short dport;          // 目的端口(Destination port)
    u_short len;            // UDP数据包长度(Datagram length)
    u_short crc;            // 校验和(Checksum)
	//u_int applied_data;	// 应用数据部分，不能确定
}udp_header;

/*
* 用回调方法捕获数据包
* packet_handler指向一个可以接受数据包的函数
* 这个函数会在收到每个新的数据包并收到一个通用状态时被libpcap所调用
*/
/* prototype of the packet handler */
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);

int main() {
	pcap_if_t *alldevs;
	pcap_if_t *d;
	int inum;
	int i = 0;
	pcap_t *adhandle;
	char errbuf[PCAP_ERRBUF_SIZE];
	u_int netmask;
	struct bpf_program fcode;
	char packet_filter[] = "ip and udp";

	/* 获得设备列表 */
	//列出当前所有可用的网络设备（网卡），将设备信息存入pcap_if_t结构列表中
	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1) {
		//fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
		cout << "Error in pcap_findalldevs: " << errbuf << "\n" << endl;
		system("pause");
		exit(1);
	}
	/* 打印列表 */
	for (d = alldevs; d; d = d->next) {
		printf("%d. %s", ++i, d->name);
		if (d->description)
			printf(" (%s)\n", d->description);
		else
			printf(" (No description available)\n");
	}
	if (i == 0) {
		printf("\nNo interfaces found! Make sure WinPcap is installed.\n");
		return -1;
	}

	printf("Enter the interface number (1-%d):", i);
	cin >> inum;

	if (inum < 1 || inum > i) {
		printf("\nInterface number out of range.\n");
		/* 释放设备列表 */
		pcap_freealldevs(alldevs);
		system("pause");
		return -1;
	}

	/* 跳转到选中的适配器 */
	for (d = alldevs, i = 0; i < inum - 1; d = d->next, i++);

	/* 打开适配器 */
	if ((adhandle = pcap_open(
		d->name,					//设备名
		65536,						//捕获包,65536保证能捕获到不同数据链路层上的每个数据包的全部内容
		PCAP_OPENFLAG_PROMISCUOUS,  //混杂模式
		1000,						//读取超时时间
		NULL,						//远程机器验证
		errbuf						//错误缓冲池
	)) == NULL) {
		//fprintf(stderr, "\nUnable to open the adapter. %s is not supported by WinPcap.\n", d->name);
		cout << "Unable to open the adapter. " << d->name << " is not supported by WinPcap.\n" << endl;
		/* 释放设备列表 */
		pcap_freealldevs(alldevs);
		system("pause");
		return -1;
	}

	/* 预处理 */
	//检查数据链路层,只考虑以太网
	if (pcap_datalink(adhandle) != DLT_EN10MB) {
		//fprintf(stderr, "\nThis program works only on Ethernet networks.\n");
		cout << "This program works only on Ethernet networks." << endl;
		/* 释放设备列表 */
		pcap_freealldevs(alldevs);
		system("pause");
		return -1;
	}

	if (d->addresses != NULL)
		/* 检索接口第一个地址的掩码 */
		netmask = ((struct sockaddr_in *)(d->addresses->netmask))->sin_addr.S_un.S_addr;
	else
		/* 如果接口没有地址,假设一个C类的掩码 */
		netmask = 0xffffff;

	/* 编译和设置过滤器 */
	//compile the filter,该函数用于解析过滤规则串，填写bpf_program结构。str指向过滤规则串。
	if (pcap_compile(adhandle, &fcode, packet_filter, 1, netmask) < 0) {
		cout << "\nUnable to compile the packet filter.Check the syntax." << endl;
		pcap_freealldevs(alldevs);
		system("pause");
		return -1;
	}
	//set the filter,该函数用于设置pcap_compile()解析完毕的过滤规则，完全可以自己提供过滤规则，无须pcap_compile()介入
	if (pcap_setfilter(adhandle, &fcode)<0) {
		cout << "\nError setting the filter." << endl;
		pcap_freealldevs(alldevs);
		system("pause");
		return -1;
	}

	printf("\nListening on %s...\n", d->description);
	/* At this point, we don't need any more the device list. Free it */
	pcap_freealldevs(alldevs);

	/* 开始捕获 */
	//这里pcap_loop的作用是抓包，每抓到一个包之后就调用packet_handler函数来处理之
	pcap_loop(adhandle, 0, packet_handler, NULL);

	system("pause");
	return 0;

}

/* 通过libpcap的每一个传入的数据包调用回调函数 */
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data) {
	mac_header *mh;
	ip_header *ih;
	udp_header *uh;
	//14+20+8
	int length = sizeof(mac_header) + sizeof(ip_header) + sizeof(udp_header) ;

	//printf("%d\n", length); 38
	printf("Ethernet frame:\n");
	for (int i = 0; i < length; i++) {
		printf("%02X ", pkt_data[i]);		//按二进制输出数据
		if ((i & 0xF) == 0xF)
			printf("\n");
	}
	printf("\n");

	/* 捕获后的处理 */
	mh = (mac_header *)pkt_data;			//通过强制类型转换,将二进制数据值依次存入结构体中

	printf("mac_header(Datalink Layer):\n");
	printf("\tdest_addr: ");
	for(int i = 0; i < 6; i++){
		printf("%02x ", mh->dest_addr[i]);
	}
	printf("\n");
	printf("\tsrc_addr: ");
	for(int i = 0; i < 6; i++){
		printf("%02x ", mh->src_addr[i]);
	}
	printf("\n");
	printf("\ttype: %04x", ntohs((u_short)mh->type));
	printf("\n");

	/* retireve the position of the ip header */

	ih = (ip_header *)(pkt_data + sizeof(mac_header));	//length of ethernet header
														
	printf("ip_header(Network Layer):\n");
	printf("\t%-10s: %02X\n", "ver_ihl", ih->ver_ihl);
	printf("\t%-10s: %02X\n", "tos", ih->tos);
	printf("\t%-10s: %04X\n", "tlen", ntohs(ih->tlen));
	printf("\t%-10s: %04X\n", "identification", ntohs(ih->identification));
	printf("\t%-10s: %04X\n", "flags_fo", ih->flags_fo);
	printf("\t%-10s: %02X\n", "ttl", ih->ttl);
	printf("\t%-10s: %02X\n", "proto", ih->proto);
	printf("\t%-10s: %04X\n", "crc", ih->crc);
	printf("\t%-10s: ", "saddr");
	for(int i = 0; i < 4; i++){
		printf("%02X ",ih->saddr[i]);
	}
	printf(" ");
	for(int i = 0; i < 4; i++){
		printf("%d.",ih->saddr[i]);
	}
	printf("\n");
	printf("\t%-10s: ", "daddr");
	for(int i = 0; i < 4; i++){
		printf("%02X ",ih->daddr[i]);
	}
	printf(" ");
	for(int i = 0; i < 4; i++){
		printf("%d.",ih->daddr[i]);
	}
	printf("\n");

	//输出udp_header
	uh = (udp_header *)(pkt_data + sizeof(mac_header)+sizeof(ip_header));
	//printf("%d,%d\n", sizeof(mac_header), sizeof(ip_header));
	printf("udp_header(Transport layer Layer):\n");
	printf("\t%-10s: %04x\n", "sport", ntohs(uh->sport));
	printf("\t%-10s: %04x\n", "dport", ntohs(uh->dport));
	printf("\t%-10s: %04x\n", "len",ntohs(uh->len));
	printf("\t%-10s: %04x\n", "crc", uh->crc);

	printf("applied_data(Application Layer):\n");
	printf("\t%-10s","The data is not sure\n");
	printf("\n");
}


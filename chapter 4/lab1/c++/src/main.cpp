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
/* IP֡��ʽ 20�ֽ�*/
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

/* ������·��֡��ʽ 14�ֽ�*/
typedef struct mac_header {
	u_char dest_addr[6];
	u_char src_addr[6];
	u_char type[2];
} mac_header;

/*������ʽ 8�ֽ�*/
typedef struct udp_header{
    u_short sport;          // Դ�˿�(Source port)
    u_short dport;          // Ŀ�Ķ˿�(Destination port)
    u_short len;            // UDP���ݰ�����(Datagram length)
    u_short crc;            // У���(Checksum)
	//u_int applied_data;	// Ӧ�����ݲ��֣�����ȷ��
}udp_header;

/*
* �ûص������������ݰ�
* packet_handlerָ��һ�����Խ������ݰ��ĺ���
* ������������յ�ÿ���µ����ݰ����յ�һ��ͨ��״̬ʱ��libpcap������
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

	/* ����豸�б� */
	//�г���ǰ���п��õ������豸�������������豸��Ϣ����pcap_if_t�ṹ�б���
	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1) {
		//fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
		cout << "Error in pcap_findalldevs: " << errbuf << "\n" << endl;
		system("pause");
		exit(1);
	}
	/* ��ӡ�б� */
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
		/* �ͷ��豸�б� */
		pcap_freealldevs(alldevs);
		system("pause");
		return -1;
	}

	/* ��ת��ѡ�е������� */
	for (d = alldevs, i = 0; i < inum - 1; d = d->next, i++);

	/* �������� */
	if ((adhandle = pcap_open(
		d->name,					//�豸��
		65536,						//�����,65536��֤�ܲ��񵽲�ͬ������·���ϵ�ÿ�����ݰ���ȫ������
		PCAP_OPENFLAG_PROMISCUOUS,  //����ģʽ
		1000,						//��ȡ��ʱʱ��
		NULL,						//Զ�̻�����֤
		errbuf						//���󻺳��
	)) == NULL) {
		//fprintf(stderr, "\nUnable to open the adapter. %s is not supported by WinPcap.\n", d->name);
		cout << "Unable to open the adapter. " << d->name << " is not supported by WinPcap.\n" << endl;
		/* �ͷ��豸�б� */
		pcap_freealldevs(alldevs);
		system("pause");
		return -1;
	}

	/* Ԥ���� */
	//���������·��,ֻ������̫��
	if (pcap_datalink(adhandle) != DLT_EN10MB) {
		//fprintf(stderr, "\nThis program works only on Ethernet networks.\n");
		cout << "This program works only on Ethernet networks." << endl;
		/* �ͷ��豸�б� */
		pcap_freealldevs(alldevs);
		system("pause");
		return -1;
	}

	if (d->addresses != NULL)
		/* �����ӿڵ�һ����ַ������ */
		netmask = ((struct sockaddr_in *)(d->addresses->netmask))->sin_addr.S_un.S_addr;
	else
		/* ����ӿ�û�е�ַ,����һ��C������� */
		netmask = 0xffffff;

	/* ��������ù����� */
	//compile the filter,�ú������ڽ������˹��򴮣���дbpf_program�ṹ��strָ����˹��򴮡�
	if (pcap_compile(adhandle, &fcode, packet_filter, 1, netmask) < 0) {
		cout << "\nUnable to compile the packet filter.Check the syntax." << endl;
		pcap_freealldevs(alldevs);
		system("pause");
		return -1;
	}
	//set the filter,�ú�����������pcap_compile()������ϵĹ��˹�����ȫ�����Լ��ṩ���˹�������pcap_compile()����
	if (pcap_setfilter(adhandle, &fcode)<0) {
		cout << "\nError setting the filter." << endl;
		pcap_freealldevs(alldevs);
		system("pause");
		return -1;
	}

	printf("\nListening on %s...\n", d->description);
	/* At this point, we don't need any more the device list. Free it */
	pcap_freealldevs(alldevs);

	/* ��ʼ���� */
	//����pcap_loop��������ץ����ÿץ��һ����֮��͵���packet_handler����������֮
	pcap_loop(adhandle, 0, packet_handler, NULL);

	system("pause");
	return 0;

}

/* ͨ��libpcap��ÿһ����������ݰ����ûص����� */
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data) {
	mac_header *mh;
	ip_header *ih;
	udp_header *uh;
	//14+20+8
	int length = sizeof(mac_header) + sizeof(ip_header) + sizeof(udp_header) ;

	//printf("%d\n", length); 38
	printf("Ethernet frame:\n");
	for (int i = 0; i < length; i++) {
		printf("%02X ", pkt_data[i]);		//���������������
		if ((i & 0xF) == 0xF)
			printf("\n");
	}
	printf("\n");

	/* �����Ĵ��� */
	mh = (mac_header *)pkt_data;			//ͨ��ǿ������ת��,������������ֵ���δ���ṹ����

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

	//���udp_header
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


import java.io.IOException;
import java.net.InetAddress;
import java.util.Date;
import java.util.Scanner;

import jpcap.JpcapCaptor;
import jpcap.NetworkInterface;
import jpcap.NetworkInterfaceAddress;
import jpcap.packet.ARPPacket;
import jpcap.packet.EthernetPacket;
import jpcap.packet.IPPacket;
import jpcap.packet.Packet;
import jpcap.packet.TCPPacket;
 
/**
 * 网络抓包并对内容进行解析
 * @author Wll
 * @time 2015-6-14
 *
 */
public class ARPCaptor {
	public static void main(String[] args) throws Exception {
		long start = System.currentTimeMillis();
		System.out.println("■■开始时间：" + new Date());
		
		//openDevice();
		showAdapterInfo();
//		setFilter(0);
//		setFilter(1);
//		setFilter(2);
		setFilter(3);
		
		System.out.println();
		System.out.println("======================== 数据包解析========================");
		for (int i = 0; i < 15; i++) { // 包的数量为 15
			ARPPacket arp = arpCap();
			EthernetPacket e = (EthernetPacket) arp.datalink;
			
			System.out.println("--------------包 " + (i + 1) + "--------------");
			
			// EthernetPacket
			String sourceAddress = e.getSourceAddress();
			String destinationAddress = e.getDestinationAddress();
			short frameType =  e.frametype;
			
			System.out.println("源MAC地址: " + sourceAddress);
			System.out.println("目的MAC地址: " + destinationAddress);
			System.out.println("帧类型: " + frameType);
			
			// arp 内容解析
			short protoType = arp.prototype;
			Object senderProtocolAddress =  arp.getSenderProtocolAddress();
			Object senderHardwareAddress =  arp.getSenderHardwareAddress();
			Object targetProtocolAddress =  arp.getTargetProtocolAddress();
			Object targetHardewareAddress = arp.getTargetHardwareAddress();
			int capturedLength = arp.caplen;
			int packetLength = arp.len;
			long timestamp_s = arp.sec;
			long timestamp_us = arp.usec;
			short hardType = arp.hardtype;
			short hardLength = arp.hlen;
			short operation = arp.operation;
			
			short arpRequest = ARPPacket.ARP_REQUEST;
			short arpReply = ARPPacket.ARP_REPLY;
			Packet eof = ARPPacket.EOF;
			short frameRelay = ARPPacket.HARDTYPE_FRAMERELAY;
			short tokenRing = ARPPacket.HARDTYPE_IEEE802;
			short protoIP = ARPPacket.PROTOTYPE_IP;
			short rarpRequest = ARPPacket.RARP_REQUEST;
			short rarpReply =  ARPPacket.RARP_REPLY;
			String data = new String(arp.data);
			
			
			System.out.println("协议类型: " + protoType);
			System.out.println("源协议地址和MAC地址: " + senderProtocolAddress + "--" + senderHardwareAddress);
			System.out.println("目的协议地址和MAC地址： " + targetProtocolAddress + "--" + targetHardewareAddress);
			System.out.println("数据报长度: " + capturedLength);
			System.out.println("长度: " + packetLength);
			System.out.println("时间戳(秒): " + timestamp_s);
			System.out.println("时间戳(微妙): " + timestamp_us);
			System.out.println("硬件类型: " + hardType);
			System.out.println("硬件地址长度: " + hardLength);
			System.out.println("操作: " + operation);
			
			System.out.println("ARP请求: " + arpRequest);
			System.out.println("ARP应答: " + arpReply);
			System.out.println("EOF: " + eof);
			System.out.println("硬件类型-帧中继: " + frameRelay);
			System.out.println("硬件类型-IEEE802(令牌环)： " + tokenRing);
			System.out.println("协议类型-IP: " + protoIP);
			System.out.println("RARP请求: " + rarpRequest);
			System.out.println("RARP应答: " + rarpReply);
			System.out.println("数据: " + data);
			
//			System.out.print("数据头部: ");
//			for (byte b : arp.header)
//				System.out.print(Integer.toHexString(b & 0xff) + " ");
//			System.out.println("\n");
		}
		
		long end = System.currentTimeMillis();
		System.out.println("■■结束时间： " + new Date());
		System.out.println("■■耗时 " + (end - start) + " ms■■");
	}
 
	/**
	 * 开启网卡准备抓包
	 * @throws Exception
	 */
	public static void openDevice() throws Exception {
		// 获取网卡信息
		
	}
 
	/**
	 * 设置过滤器，设置为只抓取某种类型的数据报
	 * @param type 数据报类型  0-ip  1-tcp  2-udp  3-arp
	 * @throws IOException
	 */
	public static void setFilter(int type) throws IOException {
		switch (type) {
		case 0:
			captor.setFilter("ip", true);
			break;
		case 1:
			captor.setFilter("tcp", true);
			break;
		case 2:
			captor.setFilter("udp", true);
			break;
		case 3:
			captor.setFilter("arp", true);
			break;
		}
	}
	
	
	/**
	 * 进行 IP 数据包抓取
	 * @return
	 */
	public static IPPacket ipCap() {
		IPPacket ip = null;
		
		while (true) {
			ip = (IPPacket) captor.getPacket();
			if (ip != null)
				return ip;
		}
	}
	
	/**
	 * 进行 TCP 数据包抓取
	 * @return
	 */
	public static TCPPacket tcpCap() {
		TCPPacket tcp = null;
		
		while (true) {
			tcp = (TCPPacket) captor.getPacket();
			if (tcp != null)
				return tcp;
		}
	}
	
	
	/**
	 * 进行 ARP 数据包抓取
	 * @return
	 */
	public static ARPPacket arpCap() {
		ARPPacket arp = null;
		
		while (true) {
			arp = (ARPPacket) captor.getPacket();
			if (arp != null)
				return arp;
		}
	}
	
	/**
	 * 获取网卡信息
	 * @throws IOException 
	 */
	public static void showAdapterInfo() throws IOException {
		System.out.println("========================网卡信息========================");
		int i = 1;
		devices = JpcapCaptor.getDeviceList();
		 
		/*
		 *  开启网卡准备抓包  
		 *  第一个参数：用于捕获包的网络接口
		 *  第二个参数： 一次性捕获的最大的字节数
		 *  第三个参数：是否为混杂模式
		 *  第四个参数：超时
		 */
		
		
		for (NetworkInterface device : devices) {
			String netInterfaceName = device.name;
			String netInterDescription = device.description;
			String dataLinkName = device.datalink_name;
			String dataLinkDesc = device.datalink_description;
			
			System.out.println("网卡" + (i++) + ": " + netInterfaceName + "(" + netInterDescription + ")");
			System.out.println("数据链路层信息: " + dataLinkName + "(" + dataLinkDesc + ")");
			
			// 网络接口地址
			for (NetworkInterfaceAddress addr: device.addresses) {
				InetAddress netInterAddress = addr.address;
				InetAddress netInterSubnet = addr.subnet;
				InetAddress netInterBroadcast = addr.broadcast; 
				
				System.out.println("网络接口地址: " + netInterAddress + "  子网掩码: " + netInterSubnet  + "  广播地址: " + netInterBroadcast);
			}
			
			System.out.print("MAC地址: ");
			int length = device.mac_address.length;
			int count = 1;
			
			for (byte b : device.mac_address) {
				System.out.print(Integer.toHexString(b & 0xff));
				
				if(count++ != length)
					System.out.print(":");
			}
			
			System.out.println();
			System.out.println("-------------------------------------------");
			
		}
		Scanner s = new Scanner(System.in);
		System.out.println("请输入选择的网卡编号");
		int num = s.nextInt();
		captor = JpcapCaptor.openDevice(devices[num-1], 65535, true, 2000);
	}
 
	private static JpcapCaptor captor;
	private static NetworkInterface[] devices;

}
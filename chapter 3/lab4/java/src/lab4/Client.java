package lab4;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.util.Map;

import lab4.TransmitInfo.EventType;

public class Client extends TransmitInfo{

//	public static void main(String[] args) throws IOException {
//		// TODO Auto-generated method stub
//			DatagramSocket socket = new DatagramSocket(8800);
//			while(true) {
//				byte[] buff = new byte[1024];
//				
//				DatagramPacket packet = new DatagramPacket(buff,  1024);
//				socket.receive(packet);
//				
//				System.out.println(new String(packet.getData()));
//				
//				String str ="Are you ok";
//				DatagramPacket packet2 = new DatagramPacket(str.getBytes(), str.length(), packet.getAddress(),packet.getPort());
//				socket.send(packet2);
//				if(flag == 1)
//					break;
//			}
//			socket.close();
			
		
		
//	}
	
	
	static Packet buffer[] = new Packet[getMax_SEQ() + 1];
	private static int NextFrameToSend;
	private static int NextFrameExpected;
	private static int AckExpected;
	private static DatagramSocket socket;
	private static int port = 8800;
	private static int MaxSize = 1024;
	private static InetAddress ip = null;
	private static int Nbuffered = 0;
	private static String[] message = {"55555555", 
								"100011111000",
								"1010101010101"}; 
	
	
	
	public static void main(String[] args) throws IOException {
		Map<String, String> map = ReadIniFile.readFile();
		setPort(Integer.parseInt(map.get("UDPPort")));
		
		socket = new DatagramSocket(8800);
		
//		String str = "HEllo";
//		String address = "127.0.0.1";
		//TransmitInfo transinfo = new TransmitInfo();
		setNextFrameToSend(0);
		setNextFrameExpected(0);
		setAckExpected(0);
		for(int i = 0; i < message.length; i++) {
			//接收
			Frame f = new Frame();
			String tmp = receiveInfo();
			//System.out.println("the tmp is" + tmp);
			String[] info = tmp.split("-"); 
			
//			for (String line : info) {
//				System.out.println(line);
//			}
//			for(int z =0; z < info.length; i++) {
//				
//			}
			if(event == EventType.cksum_err) {
				System.out.println("数据串传输出现错误\n");
			}else {
				f.setSeq(Integer.parseInt(info[0]));
				f.setAck(Integer.parseInt(info[1]));
				f.setInfo(info[2]);
				if(f.getSeq() == getNextFrameExpected()) {
//					sendInfoToNetworkLayer(f.getInfo());
					incExpected();
					incAck();
				}
				
//				while(between(getAckExpected(), f.getAck(), getNextFrameToSend())) {
//					Nbuffered = Nbuffered - 1;
//					//this.stopTimer();
//					incExpected();
////				}
				
				printReceiveInfo(f);
				}
			getInfoFromNetworkLayer(message[i]);
			//sendInfo(getSendMessage());
			printSendInfo();
			//接收
			
			}
		socket.close();
		}
			
		
			
			
	
	
	private static void printSendInfo() {
		System.out.println("当前发送的帧号为 " +(getNextFrameToSend()-1) +",期待的确认序号为 " + (getAckExpected()) +", 期待的对方下一帧号为 " + getNextFrameToSend() + ", 期待帧号为 " + getNextFrameExpected() );
	}
	private static void printReceiveInfo(Frame f) {
    	System.out.println("接收帧的发送序号为 " + f.getSeq() + ", 确认序号为 " + f.getAck() +", 当前期待帧为 " + getNextFrameExpected() + ",当前接收信息为 " + f.getInfo());
    	
    }
	public static void sendInfo(Frame f) throws IOException {
		DatagramPacket packet;
		String s = "";
		StringBuffer strbuff = new StringBuffer();
		s +=  f.getSeq()  + f.getAck()   +f.getInfo();
		//System.out.println("String is " + s);
		strbuff.append(s);
		//System.out.println("string byte is " + s.toString());
		String remainder = Crc.crc_remainder(strbuff);
		//System.out.println("the remainder is " + remainder);
		String tmp = "" + f.getSeq() + "-" + f.getAck() + "-" + f.getInfo() + "-" + remainder;
		StringBuffer sb = new StringBuffer(tmp);
//		strbuff.append("-" + remainder);
		///System.out.println("the strbuff is "+ sb.toString());
		packet = new DatagramPacket(sb.toString().getBytes(), sb.length(), InetAddress.getLocalHost(), 9999);
//		}else{
//			 packet = new DatagramPacket(strbuff.toString().getBytes(), s.length(), ip, port);
//		}
		
		socket.send(packet);
		
	}
	
	public static String receiveInfo() throws IOException {
		
		byte[] buff = new byte[MaxSize];
		DatagramPacket packet = new DatagramPacket(buff, MaxSize);
		socket.receive(packet);
		ip = packet.getAddress();
		port = packet.getPort();
		
		StringBuffer tmp = new StringBuffer(new String(packet.getData()));				
		String check = Crc.crc_check(tmp);
		int pos = check.indexOf("1");
		if(pos == -1) {
			setEvent(EventType.cksum_err);
		}
		
		
		return new String(packet.getData());
	}
	public void startUDP() throws SocketException {
		socket = new DatagramSocket();
//		this.receiveInfo(flag)
	}
	public void closeUDP() {
		socket.close();
	}
	
	public int getMaxSize() {
		return MaxSize;
	}
	public void setMaxSize(int maxSize) {
		MaxSize = maxSize;
	}
	public int getPort() {
		return port;
	}
	public static void setPort(int port1) {
		port = port1;
	}
	private static void sendData(int NextSend, int NextExpected,Packet buffer[]) throws IOException {
		Frame s = new Frame();
		//System.out.println("nextSend " + NextSend + ", Expected " + NextExpected);
		s.setInfo(buffer[NextSend]);
		s.setSeq(NextSend);
		s.setAck((NextExpected + getMax_SEQ())%(getMax_SEQ()+1));
		sendInfoToPhysicalLayer(s);
		//this.startTimer(NextSend);
	}
	public static void sendInfoToPhysicalLayer(Frame f) throws IOException {
//		String info = "";
//		info +=  f.getSeq() + "-" + f.getAck()  +"-" +f.getInfo();
//		
//		System.out.println("send info is " + info);
		sendInfo(f);
		
	}
//	public void getInfoFromPhysicalLayer(Frame f) throws IOException {
//		Integer flag = new Integer(0);
//		String tmp = server.receiveInfo(flag);
//		if(flag == 0) {
//			
//		}else {
//			this.setEvent(EventType.cksum_err);
//		}
//		
//	}
	//暂时用不上
	public void transmitInfo() {
//		this.setNextFrameToSend(0);
//		this.setNextFrameExpected(0);
//		this.setAckExpected(0);
//		Frame f = new Frame();
		
//		int Nbuffered = 0;
//		int i;
		//网络层准备
		//this.enableNetworkLayer();
//		while(true) {
//			this.waitForEvent();
//			switch (this.getEvent()) {
//			case network_layer_ready:
//				
//				
//				
//				break;
//			case frame_arrival:
//				this.getInfoFromPhysicalLayer(f);
//				if(f.getSeq() == this.getNextFrameExpected()) {
//					this.sendInfoToNetworkLayer(f.getInfo());
//					this.incExpected();
//				}
//				while(this.between(this.getAckExpected(), f.getAck(), this.getNextFrameToSend())) {
//					Nbuffered = Nbuffered - 1;
//					//this.stopTimer();
//					this.incExpected();
////				}
//				break;
//			case cksum_err: break;
//			case timeout:
//				this.setNextFrameToSend(this.getAckExpected());
//				for(i = 1; i <= Nbuffered; i++) {
//					this.sendData(this.getNextFrameToSend(), this.getNextFrameExpected(), buffer);
//					this.incAck();
//				}
//				break;
//			}
//			if(Nbuffered < this.getMax_SEQ())
//				this.enableNetworkLayer();
//			else
//				this.disableNetworkLayer(); 
//		}
		
	}
	public void waitForEvent() {
		
	}
	public static void getInfoFromNetworkLayer(String s) throws IOException {
		
		//setEvent(EventType.network_layer_ready);
		buffer[getNextFrameToSend()] = new Packet();
		buffer[getNextFrameToSend()].setData(s);
//		this.getInfoFromNetworkLayer(buffer[this.getNextFrameToSend()]);
		Nbuffered = Nbuffered + 1;
		sendData(NextFrameToSend, NextFrameExpected, buffer);
		incSend();
	}
	
	
//	public void sendInfoToNetworkLayer(String info) {
//		
//	}
	
	public static int getAckExpected() {
		return AckExpected;
	}

	public static void setAckExpected(int ackExpected) {
		AckExpected = ackExpected;
	}

	public static int getNextFrameExpected() {
		return NextFrameExpected;
	}

	public static void setNextFrameExpected(int nextFrameExpected) {
		NextFrameExpected = nextFrameExpected;
	}

	public static int getNextFrameToSend() {
		return NextFrameToSend;
	}

	public static void setNextFrameToSend(int nextFrameToSend) {
		NextFrameToSend = nextFrameToSend;
	}
	public static void incSend() {
		if(getNextFrameToSend() < getMax_SEQ()) {
			setNextFrameToSend(getNextFrameToSend() + 1);
		}else {
			setNextFrameToSend(0);
		}
	}
	public static void incExpected() {
		if(getNextFrameExpected() < getMax_SEQ()) {
			setNextFrameExpected(getNextFrameExpected() + 1);
		}else {
			setNextFrameExpected(0);
		}
	}
	public static void incAck() {
		if(getAckExpected() < getMax_SEQ()) {
			setAckExpected(getAckExpected() + 1);
		}else {
			setAckExpected(0);
		}
	}

}

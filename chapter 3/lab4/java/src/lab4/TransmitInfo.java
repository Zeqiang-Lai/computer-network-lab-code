package lab4;

import java.io.IOException;
import java.net.SocketException;

public class TransmitInfo {
	private int MAX_PKT = 1024;
	private static int Max_SEQ = 7;
	
	protected static EventType event;

	private static Server server = new Server();
	private static String sendMessage;
	private static String receiveMessage;
	public static String getSendMessage() {
		return sendMessage;
	}
	public static void setSendMessage(String sendMessage) {
		TransmitInfo.sendMessage = sendMessage;
	}
	public static String getReceiveMessage() {
		return receiveMessage;
	}
	public static void setReceiveMessage(String receiveMessage) {
		TransmitInfo.receiveMessage = receiveMessage;
	}

	//frame kind definition
	public enum FrameKind{
		data, ack, nak;
	}
	//
	public enum EventType{
		frame_arrival, cksum_err, timeout,network_layer_ready;
	}
	public static boolean between(int a, int b, int c) {
		if(((a<=b)&&(b<c)) || ((c<a)&&(a<=b)) || ((b<c) && (c <a)))
			return true;
		else
			return false;
	}
	public int getMAX_PKT() {
		return MAX_PKT;
	}

	public void setMAX_PKT(int mAX_PKT) {
		MAX_PKT = mAX_PKT;
	}
	
	
	private void enableNetworkLayer() throws SocketException {
		server.startUDP();
	}
	private void disableNetworkLayer() {
		server.closeUDP();
	}
	
	
	public static int getMax_SEQ() {
		return Max_SEQ;
	}

	public void setMax_SEQ(int max_SEQ) {
		Max_SEQ = max_SEQ;
	}

	private void startTimer(Frame f) {
		
	}
	private void stopTimer(Frame f) {
		
	}
	
	
	
	
	public EventType getEvent() {
		return event;
	}

	public static void setEvent(EventType Event) {
		event = Event;
	}

	
	
}







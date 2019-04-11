

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.net.SocketException;
import java.net.UnknownHostException;

public class receiver {
    final private static String TAG = "SocketUdp: ";
    private static final int PORT= 8888;//监听和发送的端口

    public static void main(String args[]) {

        DatagramSocket socket = null;
        DatagramPacket datapacket_send = null;
        DatagramPacket datapacket_receive= null;
        InetSocketAddress address = null;
        while(true) {
            try {
                //定义接收的socket客户端
                address = new InetSocketAddress(InetAddress.getLocalHost(), PORT);
                socket = new DatagramSocket(address);


                byte buf_receive[] = new byte[1024];
                datapacket_receive = new DatagramPacket(buf_receive, buf_receive.length);
                System.out.println("等待接收消息...");
                socket.receive(datapacket_receive);
                InetAddress addr = datapacket_receive.getAddress();
                int port = datapacket_receive.getPort();
                System.out.println("消息内容: " + new String(buf_receive));
                System.out.println("消息发送方 " + addr + ":" + port);

                SocketAddress toAddress = datapacket_receive.getSocketAddress();
                //如果crc校验通过
                String sendStr = "true";
                byte [] buf_send = sendStr.getBytes();
                datapacket_send = new DatagramPacket(buf_send, buf_send.length);
                datapacket_send.setSocketAddress(toAddress);
                socket.send(datapacket_send);
                System.out.println("ack已经发送");


            } catch (UnknownHostException e) {
                System.out.println(TAG + e.getMessage());
                e.printStackTrace();
            } catch (SocketException e) {
                System.out.println(TAG + e.getMessage());
                e.printStackTrace();
            } catch (IOException e) {
                System.out.println(TAG + e.getMessage());
                e.printStackTrace();
            }
            socket.close();
        }
    }

}
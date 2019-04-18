

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.net.SocketException;
import java.net.UnknownHostException;

public class Receiver {
    final private static String TAG = "SocketUdp: ";//用来输出错误信息，不用管
    private static final int PORT= 8888;//监听和发送的端口

    


    public static void main(String args[]) {

        DatagramSocket socket = null;
        DatagramPacket datapacket_send = null;
        DatagramPacket datapacket_receive= null;
        InetSocketAddress address = null;
        int frame_expected=0;
        //死循环不会退出
        while(true) {

            try {
                //定义接收的socket客户端
                address = new InetSocketAddress(InetAddress.getLocalHost(), PORT);
                socket = new DatagramSocket(address);


                byte [] buf_receive = new byte[1024];
                datapacket_receive = new DatagramPacket(buf_receive, buf_receive.length);
                System.out.println("等待接收:第"+frame_expected+"帧");
                socket.receive(datapacket_receive);
                InetAddress addr = datapacket_receive.getAddress();
                int port = datapacket_receive.getPort();
                //这句太关键了，写尼玛傻逼博客的脑残非得用下面这句，搞得我调试了一天
                //printf(new String(buf_receive))
                //这才是正确的写法好吧
                System.out.println("消息内容: " + new String(datapacket_receive.getData(),0,datapacket_receive.getLength()));
                //System.out.println("消息发送方 " + addr + ":" + port);

                SocketAddress toAddress = datapacket_receive.getSocketAddress();
                //如果crc校验通过
                if(true) {
                    String sendStr;
                    if (frame_expected == 0)
                        sendStr = "0";
                    else
                        sendStr = "1";
                    byte[] buf_send = sendStr.getBytes();
                    datapacket_send = new DatagramPacket(buf_send, buf_send.length);
                    datapacket_send.setSocketAddress(toAddress);
                    socket.send(datapacket_send);
                    System.out.println("ack已经发送,确认已收到第" + frame_expected + "帧");
                    frame_expected = 1 - frame_expected;
                }
                else
                {
                       System.out.println("收到的数据出错");
                }


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

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.SocketException;
import java.net.UnknownHostException;

public class sender {
    final private static String TAG = "SocketUdp: ";
    private static final int TIMEOUT = 50000;  //设置接收数据的超时时间
    private static final int PORT= 8888;//监听和发送的端口
    private static final int FRAME_SIZE=10;//
    public static void main(String args[])
    {
        //从文件中读取一行
        String message_to_send="test message 1";
        //将读到的一行转化为二进制数字
        byte[] message_byte_stream=message_to_send.getBytes();
        //计算需要切分为多少帧
        int frame_amount=message_byte_stream.length/FRAME_SIZE;
        //定义一个计数器，判断是否计算完毕
        int sended_amout=0;
        while(true)
        {

            while(true)
            {
                try {
                    //socket 客户端
                    DatagramSocket getSocket = new DatagramSocket();
                    //设置接收数据时阻塞的最长时间
                    getSocket.setSoTimeout(TIMEOUT);
                    //定义发送和接收的socket包
                    DatagramPacket datapacket_send = null;
                    DatagramPacket datapacket_receive = null;
                    //需要发送的地址，这里是本地发本地
                    InetSocketAddress toAddress = new InetSocketAddress(InetAddress.getLocalHost(), PORT);


                    String sendStr = "I'm sender, this is the message for receiver.";
                    byte buf_send[] = sendStr.getBytes();
                    byte buf_receive[]=new byte[1024];
                    datapacket_send = new DatagramPacket(buf_send, buf_send.length);
                    datapacket_send.setSocketAddress(toAddress);
                    getSocket.send(datapacket_send);
                    System.out.println("消息已经发送");



                    System.out.println("等待接收ack...");
                    datapacket_receive = new DatagramPacket(buf_receive, buf_receive.length);
                    getSocket.receive(datapacket_receive);
                    String ack_receive=new String(buf_receive);
                    //如果接收ack成功
                    if(ack_receive.equals("true"))
                    {
                        System.out.println("ack的内容: " + ack_receive);
                        //更新一帧需要发送的信息
                        sendStr="next_frame";
                        datapacket_send.setData(sendStr.getBytes());
                        sended_amout++;
                        break;
                    }
                    //如果接收ack失败
                    else
                        {
                            System.out.println("ack与期望不符，重发当前帧");
                            System.out.println("ack的内容: " + ack_receive);

                    }
                    //关闭socket客户端
                    getSocket.close();
                } catch (SocketException e) {
                    System.out.println(TAG + e.getMessage());

                    e.printStackTrace();
                } catch (UnknownHostException e) {
                    System.out.println(TAG + e.getMessage());

                    e.printStackTrace();
                } catch (IOException e) {
                    //System.out.println(TAG + e.getMessage());
                    System.out.println("接收ack超时，重新发送一帧");
                    //e.printStackTrace();
                }

            }
            if(sended_amout==1)
            {
                System.out.println("传输完毕");
            }
        }
    }
}
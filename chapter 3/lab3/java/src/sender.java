import java.net.*;
import java.io.*;

public class sender {
    private static final int TIMEOUT = 5000;  //设置接收数据的超时时间
    private static final int PORT= 8888;//监听和发送的端口
    public static void Main(String args[])throws IOException{

        String str_send = "Hello UDPserver";
        byte[] buf = new byte[1024];
        //sender在8888端口监听接收到的数据
        DatagramSocket ds = new DatagramSocket(PORT);
        //定义发送的地址为自己
        InetAddress loc = InetAddress.getLocalHost();
        //定义用来发送数据的DatagramPacket实例
        DatagramPacket dp_send= new DatagramPacket(str_send.getBytes(),str_send.length(),loc,PORT);
        //定义用来接收数据的DatagramPacket实例
        DatagramPacket dp_receive = new DatagramPacket(buf, 1024);
        //数据发向本地8888端口
        ds.setSoTimeout(TIMEOUT);              //设置接收数据时阻塞的最长时间
        while(true)
        {
            //发送当前帧直到接收到正确的ack，或者重发次数达到预定值，则退出循环
            while (true)
            {
                //发送数据
                ds.send(dp_send);
                    //接收从服务端发送回来的数据
                    ds.receive(dp_receive);
                    //如果接收到的ack等于1则结束循环
                    if (dp_receive.getData().equals(1))
                    {
                        //更新一帧需要发送的信息
                        str_send="next_frame";
                        dp_send.setData(str_send.getBytes());
                        break;
                    }
                    //重置dp_receive
                    dp_receive.setLength(1024);
            }
            //如果发送的是最后一帧，则推出循环
        }
        //ds.close();
    }
}
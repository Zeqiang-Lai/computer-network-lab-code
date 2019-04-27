

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
    private static int PORT= 8888;//监听和发送的端口
    private static  String PRODUCTOR_STR="10001000000100001";
    public static void main (String args[]) throws IOException{
        String  configure_path="ReceiverConfigure.ini";
        if(args.length>1)
        {
            configure_path=args[0];
        }
        //使用配置文件初始化

        //PORT= Integer.valueOf(readconfigure.readConfigFile(configure_path,"PORT")).intValue();
        PRODUCTOR_STR=readconfigure.readConfigFile(configure_path,"PRODUCTOR_STR");
        String sendStr=readconfigure.readConfigFile(configure_path,"SEND_STR");
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



                //
                //Sending ACK[number]
                System.out.println("Frame expected: ["+frame_expected+"]");
                socket.receive(datapacket_receive);
                InetAddress addr = datapacket_receive.getAddress();
                int port = datapacket_receive.getPort();

                //去掉crc位和flag位
                byte[] buf_message=new byte[datapacket_receive.getLength()-3];
                System.arraycopy(buf_receive,1,buf_message,0,datapacket_receive.getLength()-3);
                byte[] buf_message_with_crc=new byte[datapacket_receive.getLength()-1];
                System.arraycopy(buf_receive,1,buf_message_with_crc,0,datapacket_receive.getLength()-1);
                byte[] buf_flag=new byte[1];
                System.arraycopy(buf_receive,0,buf_flag,0,1);
                String str_flag=new String(buf_flag);
                //校验crc
                System.out.println("Checking...");
                String str_poly=PRODUCTOR_STR;
                String crc_result=crc.crc_check(new StringBuffer(BinaryChange.conver2HexStr(buf_message_with_crc)),new StringBuffer(str_poly));
                //System.out.println("crc校验前："+BinaryChange.conver2HexStr(buf_message_with_crc) );
                //System.out.println("crc校验结果是："+crc_result);







                /*
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
                */

                SocketAddress toAddress = datapacket_receive.getSocketAddress();
                //如果收到的帧是想要的
                if( Integer.parseInt(str_flag)==frame_expected) {
                    int pos=crc_result.indexOf("1");
                    if(pos==-1)
                    {


                        byte[] buf_send = str_flag.getBytes();
                        datapacket_send = new DatagramPacket(buf_send, buf_send.length);
                        datapacket_send.setSocketAddress(toAddress);
                        socket.send(datapacket_send);
                        System.out.println("Valid: Frame["+frame_expected+"]");
                        frame_expected = 1 - frame_expected;
                    }
                    else
                        System.out.println("Invalid: CRC = ["+crc_result+"]");

                }
                else
                {
                    System.out.println("\"InValid: Frame[\"+frame_expected+\"]\"");
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
//package zyb.org.UDP;
import java.io.*;
import java.net.*;
public class receiver{
    //定义用于通讯的端口是888
private static final int PORT=8888;

public static void Main(String[] args)throws IOException
        {
            //定义要发送的语句

            //定义用于接受数据的缓冲区
            byte[] buff=new byte[1024];
            //定义socket客户端
            DatagramSocket ds = new DatagramSocket(PORT);
            //定义用于接受数据的socket包
            DatagramPacket dp_receive=new DatagramPacket(buff,1024);
            while(true)
            {
                //接受数据
                ds.receive(dp_receive);
                //根据crc检验检验结果发送ack
                /*
                if(crcisright)
                {
                    string str_send="1";
                    DatagramPacket dp_send= new DatagramPacket(str_send.getBytes(),str_send.length(),dp_receive.getAddress(),PORT);
                    ds.send(dp_send);
                }
                else
                {
                    string str_send="0";
                    DatagramPacket dp_send= new DatagramPacket(str_send.getBytes(),str_send.length(),dp_receive.getAddress(),PORT);
                    ds.send(dp_send);
                }
                */


                //输出接受的数据
                System.out.println("server received data from client：");
                String str_receive = new String(dp_receive.getData(),0,dp_receive.getLength()) +
                        " from " + dp_receive.getAddress().getHostAddress() + ":" + dp_receive.getPort();
                System.out.println(str_receive);
                dp_receive.setLength(1024);


            }
            //ds.close();

        }
        }

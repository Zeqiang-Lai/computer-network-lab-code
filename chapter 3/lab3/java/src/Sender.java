import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.SocketException;
import java.net.UnknownHostException;

public class Sender {
    final private static String TAG = "SocketUdp: ";//输出错误信息不用管
    private static final int TIMEOUT = 5000;  //设置接收数据的超时时间
    private static final int PORT= 8888;//监听和发送的端口
    private static final int FILTERERROR=10;//平均每十帧出错一次
    private static final int FILTERLOST=10;//平均没十帧丢失一帧
    public static int MyFilter()
    {

        double random_n1=Math.random();
        double random_n2=Math.random();
        //当随机数大于失败概率发送成功
        if(random_n1>(1.0/FILTERLOST))
        {
            if(random_n2>(1.0/FILTERERROR))
                return 1;
            else
                return 2;
        }
        else return 0;

    }
    public static void main(String args[])
    {
        //从文件中读取一行
        String sendStr="message 1";
        //成功发送次数的计数器
        int success_sended_amout=0;
        //发送完所有数据后退出，success_sended_amount控制
        while(true)
        {
            //发送成功后退出
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
                    //设置收发数据的存储区
                    byte buf_send[] = sendStr.getBytes();
                    byte buf_receive[]=new byte[1024];

                    //输出下一帧的编号

                    System.out.println("Next frame to send:    ["+(success_sended_amout+1)%2+"]");


                    //添加crc和flag位

                    String binary_str_send=BinaryChange.conver2HexStr(buf_send);
                    String productor_str="10001000000100001";
                    String crc_remained=crc.crc_remainder(new StringBuffer(binary_str_send),new StringBuffer(productor_str));
                    String flag_str;
                    if(success_sended_amout%2==0)
                        flag_str="0";
                    else flag_str="1";
                    byte[] buf_flag=flag_str.getBytes();
                    byte[] buf_crc_remained=BinaryChange.conver2HexToByte(crc_remained);
                    byte[] buf_send_include_crc=new byte[buf_send.length+buf_crc_remained.length+buf_flag.length];
                    System.arraycopy(buf_flag, 0,buf_send_include_crc, 0, buf_flag.length);
                    System.arraycopy(buf_send, 0,buf_send_include_crc, buf_flag.length, buf_send.length);
                    System.arraycopy(buf_crc_remained, 0,buf_send_include_crc, buf_flag.length+buf_send.length, buf_crc_remained.length);



                    ////////////////////////////////////////////////
                    //配置发送包的内容和地址
                    datapacket_send = new DatagramPacket(buf_send_include_crc, buf_send_include_crc.length);
                    datapacket_send.setSocketAddress(toAddress);
                    //运行过滤程序，并输出发送情况
                    int filter_result=MyFilter();
                    if(filter_result==1)
                    {

                        getSocket.send(datapacket_send);
                        System.out.println("Sending State:         Normal");

                    }
                    else if(filter_result==2)
                    {
                        //改变发送的数据后再发送
                        buf_send_include_crc[(int)(Math.random()*(buf_send_include_crc.length-1))]++;
                        getSocket.send(datapacket_send);
                        System.out.println("Sending State:         Error");

                    }
                    else
                    {
                        System.out.println("Sending State:         Lost");
                    }

                    //等待接收ack

                    System.out.println("Receiving...");
                    //配置接收包
                    datapacket_receive = new DatagramPacket(buf_receive, buf_receive.length);
                    //在时间TIMEOUT的范围内接收ack
                    getSocket.receive(datapacket_receive);
                    //将接收的ack从包中提取出来
                    //存储ack_received中
                    String ack_received=new String(datapacket_receive.getData(),0,datapacket_receive.getLength());
                    String ack_expected;
                    if(success_sended_amout%2==0)
                        ack_expected="0";
                    else
                        ack_expected="1";
                    //如果接收ack成功
                    if(ack_received.equals(ack_expected))
                    {
                        //System.out.println("ack的内容: " + ack_received);
                        //System.out.println("ack与期望一致，发送下一帧");
                        System.out.println("Receiving State:       Ack["+ack_received+"]");
                        System.out.println("---------------------------------");
                        //更新一帧需要发送的信息
                        sendStr="message "+(success_sended_amout+1);
                        //datapacket_send.setData(sendStr.getBytes());
                        //发送成功的计数器加一
                        success_sended_amout++;
                        break;
                    }
                    //如果接收ack失败
                    else
                    {
                        //System.out.println("ack的内容: " + ack_received);
                        //System.out.println("ack与期望不符，重发当前帧");
                        System.out.println("Receiving State:       Ack["+ack_received+"]");
                        System.out.println("---------------------------------");

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
                    System.out.println("Receiving State:       Timeout");
                    System.out.println("---------------------------------");
                    //e.printStackTrace();
                }


            }
            if(success_sended_amout==20)
            {
                System.out.println("transit finished !");
                break;

            }

        }
    }
}
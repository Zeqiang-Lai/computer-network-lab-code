import java.io.IOException;

public class bit_fill_Main {

    public static void main(String[] args) throws IOException {

        String ini_path="Bit_Fill_data.ini";
        if(args.length>=1)
        ini_path=args[0];
        String InfoString=readconfigure.readConfigFile(ini_path,new String("InfoString"));
        System.out.println("-------------读取配置文件中待发送的数据信息-----");
        System.out.println(InfoString);
        System.out.println("-------------比特填充之后的数据信息-------------");
        String Fill_string = bitfill.BitFill(InfoString);
        System.out.println(Fill_string);
        System.out.println("-------------帧首部的四部分---------------------");
        System.out.println("帧起始标志:01111110");
        System.out.println("A:11111111");
        System.out.println("C:00000011");
        System.out.println("协议：0000000000100001 或 1010000000100001 或 1000000000100001");
        System.out.println("-------------帧尾部的两部分---------------------");
        System.out.println("FCS:XXXXXXXX");
        System.out.println("帧结束标志：01111110");
        System.out.println("-------------比特填充后的发送帧为---------------");
        System.out.println("若为IP数据报：                "+"01111110 11111111 00000011 0000000000100001 "+ Fill_string +" XXXXXXXX"+" 01111110");
        System.out.println("若为ppp链路控制协议LCP的数据："+"01111110 11111111 00000011 1010000000100001 "+ Fill_string +" XXXXXXXX"+" 01111110");
        System.out.println("若为网络层的控制数据：        "+"01111110 11111111 00000011 1000000000100001 "+ Fill_string +" XXXXXXXX"+" 01111110");
        System.out.println("-------------比特删除后的接受帧为---------------");
        System.out.println("若为IP数据报：                "+"01111110 11111111 00000011 0000000000100001 "+ InfoString +" XXXXXXXX"+" 01111110");
        System.out.println("若为ppp链路控制协议LCP的数据："+"01111110 11111111 00000011 1010000000100001 "+ InfoString +" XXXXXXXX"+" 01111110");
        System.out.println("若为网络层的控制数据：        "+"01111110 11111111 00000011 1000000000100001 "+ InfoString +" XXXXXXXX"+" 01111110");

        System.out.println("-------------按任意键退出程序-------------------");

    }
}

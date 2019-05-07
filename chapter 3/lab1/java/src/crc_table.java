import java.util.ArrayList;
import java.lang.String;
import java.lang.*;
import java.io.*;
public class crc_bit {
    static final int WIDTH =16;
    static final int TOPBIT =1<<(WIDTH-1);
    static final int POLYNOMIAL =0x1021;
    static String bitstream[];
    static int crc_table[]=new int[256];
    public static int getUint16(int i){
        return i & 0x0000ffff;
    }

    public static void build_crc_table()
    {
        int remainder;
        for(int dividend=0;dividend<256;++dividend)
        {
            remainder=dividend<<(WIDTH-8);
            for(int bit=8;bit>0;--bit)
            {
                if((remainder&TOPBIT)!=0)
                {
                    remainder=(remainder<<1)^POLYNOMIAL;
                }
                else
                {
                    remainder=(remainder<<1);
                }
            }
            crc_table[dividend]=getUint16(remainder);
        }
    }

    public static int crc_remainder(int msg[],int n_bytes)
    {
        int  remainder=0;
        int index;
        for(int bytes=0;bytes<n_bytes;++bytes)
        {
            remainder^=((int)msg[bytes]<<(WIDTH-8));
            index=remainder>>(WIDTH-8);
            remainder=getUint16((remainder<<8)^crc_table[index]);
//            for(int bit=8;bit>0;--bit)
//            {
//                if((remainder&TOPBIT)!=0)
//                { remainder=getUint16(remainder<<1);
//                    remainder=getUint16(remainder^POLYNOMIAL);}
//                else
//                    remainder=(remainder<<1);
//            }
        }
        return remainder;
    }



//    public static boolean crc_check(char msg_with_crc[],int n_bytes)
//    {
//        return(crc_remainder(msg_with_crc,n_bytes)==0);
//    }

   /*public static int to_bytes_stream(ArrayList<Character> bitstring)
    {
        int length=bitstring.size();
        if(length%8!=0)
        {
            int local=(((int)(length/8)+1)*8-length);
            for(int i=local+length;i>length;i--)
            {
                bitstring.set(i,bitstring.get(i-local));
            }
            for(int j=0;j<local;j++)
            {
                bitstring.set(j,'0');
            }
        }
        String st=String.valueOf(bitstring);
        for(int k=0;k<length/8;k++)
        {
           bitstream[k]=Integer.toString(Integer.parseInt(st,2),16);
        }
        return 0;
    }*/

   public static StringBuffer to_bytes_stream(StringBuffer string)
   {
       int length=string.length();
       if(length%8!=0)
       {
           int local=((int)(length/8)+1)*8-length;
           for(int i=0;i<local;i++)
           {
               StringBuffer a=new StringBuffer("0");
               string = a.append(string);
           }
       }
       StringBuffer b=new StringBuffer();
        for(int j=0;j<string.length()/8;j++)
        {
//            String a=Integer.toHexString(Integer.parseInt(string.substring(8*j,8*j+8)));
//            if((Integer.toHexString(Integer.parseInt(string.substring(8*j,8*j+8),2))).length()<2)
//            {
//                StringBuffer a1=new StringBuffer(a);
//                StringBuffer a2=new StringBuffer("0");
//                a1=a2.append(a1);
//                a=a1.toString();
//            }
//            System.out.println(a);
            b.append((Integer.toHexString(Integer.parseInt(string.substring(8*j,8*j+8),2))));
        }
        return b;
   }

    public static StringBuffer read_configuration(String file_path)throws Exception//从ini文件中读取多项式
    {
        StringBuffer sb=new StringBuffer();
        try (FileReader reader = new FileReader(file_path); BufferedReader br = new BufferedReader(reader))
        {
            String line;
            while ((line = br.readLine()) != null)
            {
                sb.append(line);
                sb.append("\n");
            }

        }
        catch (IOException e) {
            System.out.println("Error: configuration file not found.");

        }
        return sb;
    }

    public static void show_send_string_result(StringBuffer send_string)
    {
        System.out.println("################# Send String #####################");
        System.out.print("Message:");
        System.out.println(send_string);
        StringBuffer sendstring2=to_bytes_stream(send_string);
        System.out.print("Message in Hex: ");
        StringBuffer sendstring=new StringBuffer();
        for(int i=0;i<sendstring2.length();i++)
        {
            System.out.print(sendstring2.substring(i,i+1));
        }
        System.out.println("");
        System.out.print("CRC Code: ");
        StringBuffer ans=new StringBuffer(sendstring2.length());
        int ans2[]=new int[sendstring2.length()/2];
        for(int k=0;k<sendstring2.length()/2;k++)
        {
            ans.append(Integer.toBinaryString(Integer.parseInt(sendstring2.substring(2*k,2*k+2),16)));
            ans2[k]=Integer.parseInt(sendstring2.substring(2*k,2*k+2),16);
        }
        int crc_code=crc_remainder(ans2,sendstring2.length()/2);
        System.out.println(Integer.toHexString(crc_code));
        StringBuffer msg_with_code=sendstring2.append(Integer.toHexString(crc_code));
        System.out.print("Message With Code in Hex:");
        for(int i=0;i<msg_with_code.length();i++)
        {
            System.out.print(msg_with_code.substring(i,i+1));
        }
        System.out.println("");
    }

    public static void show_receive_string_result(StringBuffer receive_string)
    {
        System.out.println("################# Receive String #####################");
        System.out.print("Message_Received:");
        System.out.println(receive_string);
        System.out.print("Message in Hex: ");
        StringBuffer receivestring2=to_bytes_stream(receive_string);
        StringBuffer receivestring=new StringBuffer();
        for(int i=0;i<receivestring2.length();i++)
        {
            System.out.print(receivestring2.substring(i,i+1));
        }
        StringBuffer ans=new StringBuffer(receivestring2.length());
        int ans2[]=new int[receivestring2.length()/2];
        for(int k=0;k<receivestring2.length()/2;k++)
        {
            ans.append(Integer.toBinaryString(Integer.parseInt(receivestring2.substring(2*k,2*k+2),16)));
            ans2[k]=Integer.parseInt(receivestring2.substring(2*k,2*k+2),16);
        }
        int crc_code=crc_remainder(ans2,receivestring2.length()/2);
        System.out.println(" ");
        if (crc_code==0)
        {
            System.out.println("Receive string is valid!");
        }
        else
        {
            System.out.print("Invalid Receive string. CRC code=");
            System.out.println(Integer.toHexString(crc_code));
        }
    }

    public static void main(String args[])throws Exception
    {
//        String a="hello";
//        char a2[]=a.toCharArray();
//        System.out.println(crc_remainder(a2,5));
        build_crc_table();
        String ini_path="crc.ini";
        String h[]=new String[100];//存储划分后的子串
        if(args.length==1)
        {
            ini_path=args[0];
        }
        int i=0;
        StringBuffer ans=read_configuration(ini_path);
        String[]lines=ans.toString().split("\\n");
        for(String line:lines)//按换行符对读出的字符串进行划分
        {
            h[i]=line;
            i++;
        }
        StringBuffer poly=new StringBuffer(h[3]);
        StringBuffer send_string =new StringBuffer(h[4]);
        StringBuffer recevied_string =new StringBuffer(h[5]);
        recevied_string=new StringBuffer(h[5]);
        send_string =new StringBuffer(h[4]);
        show_send_string_result(send_string);
        show_receive_string_result(recevied_string);
    }
}
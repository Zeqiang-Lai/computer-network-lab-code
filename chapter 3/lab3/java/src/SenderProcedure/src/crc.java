//Author: tian-huang

import java.lang.String;
import java.lang.*;
import java.io.*;
public class crc
{
    public static char XOR(char a,char b)
    {
        return a==b?'0':'1';
    }
    static String crc_remainder(StringBuffer input_bitstring,StringBuffer poly)//crc码的生成程序
    {
        int len_input = input_bitstring.length();
        int len_poly = poly.length();
        StringBuffer padding=new StringBuffer("0");
        for(int i=1;i<len_poly-1;i++)
        {
            padding=padding.append("0");
        }

        StringBuffer padded_input = input_bitstring.append(padding);

        for(int j=0;j<len_input;++j)
        {
            if(padded_input.charAt(j)=='0')
            {
                continue;
            }
            for(int k=j;k<j+len_poly;++k)
            {
                padded_input.setCharAt(k,XOR(padded_input.charAt(k),poly.charAt(k-j)));
            }

        }
        return padded_input.substring(len_input,len_input+len_poly-1);
    }
    static String crc_check(StringBuffer input_bitstring,StringBuffer poly)//crc码的生成程序
    {

        int len_poly = poly.length();
        int len_input = input_bitstring.length()-len_poly+1;
        StringBuffer padded_input = input_bitstring;

        for(int j=0;j<len_input;++j)
        {
            if(padded_input.charAt(j)=='0')
            {
                continue;
            }
            for(int k=j;k<j+len_poly;++k)
            {
                padded_input.setCharAt(k,XOR(padded_input.charAt(k),poly.charAt(k-j)));
            }

        }
        return padded_input.toString();
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
        return sb;//返回读出的所有字符
    }
    public static  void show_check_example(StringBuffer bit_string,StringBuffer poly)
    {
        StringBuffer bit_string2=new StringBuffer(bit_string);
        String check_remainder=crc_check(bit_string,poly);
        int pos1=check_remainder.indexOf("1");
        bit_string = new StringBuffer(bit_string2);
        System.out.println("Actual string received:            "+bit_string);
        System.out.println("Remainder:                         "+check_remainder);
        if(pos1==-1)
            System.out.println("Valid:                             "+"true");
        else
            System.out.println("Valid:                             "+"false");
    }
    public static void main(String args[])throws Exception
    {
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
        StringBuffer crc_code_send=new StringBuffer(crc_remainder(send_string,poly));
        send_string =new StringBuffer(h[4]);
        StringBuffer crc_code_receive=new StringBuffer(crc_remainder(send_string,poly));
        send_string =new StringBuffer(h[4]);
        StringBuffer Message_send=new StringBuffer(send_string.append(crc_code_send));
        send_string =new StringBuffer(h[4]);
        StringBuffer Message_receive=new StringBuffer(recevied_string.append(crc_code_receive));
        send_string =new StringBuffer(h[4]);
        recevied_string=new StringBuffer(h[5]);
        send_string =new StringBuffer(h[4]);
        System.out.println("Message to be sent:                "+send_string+"\n");
        System.out.println("CRC_Code:                          "+crc_code_send+"\n");
        System.out.println("Message with checksum(crc):        "+Message_send+"\n");
        System.out.println("----------------------------------------------------"+"\n");
        System.out.println("Message recevied:                  "+recevied_string+"\n");
        System.out.println("CRC_Code:                          "+crc_code_receive+"\n");
        System.out.println("Message with checksum(crc):        "+Message_receive+"\n");
        System.out.println("True example:");
        show_check_example(Message_receive,poly);
        Message_receive=new StringBuffer(recevied_string.append(crc_code_receive));
        System.out.println("False example:");
        Message_receive.setCharAt(Message_receive.length()-1,XOR(Message_receive.charAt(Message_receive.length()-1),'1'));
        show_check_example(Message_receive,poly);
    }
}

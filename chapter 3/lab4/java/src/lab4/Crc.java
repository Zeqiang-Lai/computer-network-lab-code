package lab4;

import java.lang.String;
import java.lang.*;
import java.io.*;
public class Crc
{
    public static String s = "10001000000100001";
	public static  StringBuffer key = new StringBuffer(s);
	public static char XOR(char a,char b)
       {
           return a==b?'0':'1';
       }
       static String crc_remainder(StringBuffer input_bitstring)//crc码的生成程序
       {
           int len_input = input_bitstring.length();
           int len_poly = key.length();
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
                    padded_input.setCharAt(k,XOR(padded_input.charAt(k),key.charAt(k-j)));
                }

            }
            return padded_input.substring(len_input,len_input+len_poly-1);
       }
    static String crc_check(StringBuffer input_bitstring)//crc校验
    {
        int len_input = input_bitstring.length();
        int len_poly = key.length();

        StringBuffer padded_input = input_bitstring;

        for(int j=0;j<len_input;++j)
        {
            if(padded_input.charAt(j)=='0')
            {
                continue;
            }
            for(int k=j;k<j+len_poly;++k)
            {
                padded_input.setCharAt(k,XOR(padded_input.charAt(k),key.charAt(k-j)));
            }

        }
        return padded_input.toString();
    }

       
//       public static void main(String args[])throws Exception
//       {
//           String ini_path="D://crc.ini";
//           String h[]=new String[8];//存储划分后的子串
//           if(args.length>1)
//           {
//               ini_path=args[1];
//           }
//           int i=0;
//           StringBuffer ans=read_configuration(ini_path);
//           String[]lines=ans.toString().split("\\n");
//           for(String line:lines)//按换行符对读出的字符串进行划分
//           {
//               h[i]=line;
//               i++;
//           }
//           StringBuffer poly=new StringBuffer(h[3]);
//           StringBuffer send_string =new StringBuffer(h[4]);
//           StringBuffer recevied_string =new StringBuffer(h[5]);
//           StringBuffer crc_code_send=new StringBuffer(crc_remainder(send_string,poly));
//           send_string =new StringBuffer(h[4]);
//           StringBuffer crc_code_receive=new StringBuffer(crc_remainder(send_string,poly));
//           send_string =new StringBuffer(h[4]);
//           StringBuffer Message_send=new StringBuffer(send_string.append(crc_code_send));
//           send_string =new StringBuffer(h[4]);
//           StringBuffer Message_receive=new StringBuffer(send_string.append(crc_code_receive));
//           send_string =new StringBuffer(h[4]);
//           String check_remainder=crc_check(Message_receive,poly);
//           send_string =new StringBuffer(h[4]);
//           int pos1=check_remainder.indexOf("1");
//           System.out.println("Message to be sent:                "+send_string+"\n");
//           System.out.println("CRC_Code:                          "+crc_code_send+"\n");
//           System.out.println("Message with checksum(crc):        "+Message_send+"\n");
//           System.out.println("----------------------------------------------------"+"\n");
//           System.out.println("Message recevied:                  "+recevied_string+"\n");
//           System.out.println("CRC_Code:                          "+crc_code_receive+"\n");
//           System.out.println("Message with checksum(crc):        "+Message_receive+"\n");
//
//           System.out.println("Remainder:                         "+check_remainder+"\n");
//
//           if(pos1==-1)
//               System.out.println("Valid:                             "+"true");
//           else
//              System.out.println("Valid:                              "+"false");
//       }
}
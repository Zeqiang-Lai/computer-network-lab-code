import java.io.IOException;

public class byte_fill_Main {

    
    //ASCII码中将字符转换成对应的十进制数
    static int char2int( char input )
    {
        return input>64?(input-55):(input-48);
    }

    //ASCII码中将十进制数转换成对应的字符
    static int int2char( char input )
    {
        return input>9?(input+55):(input+48);
    }

    //将十六进制字符串HexStr1和HexStr2异或得到HexStr
     static String hexstrxor(String Str1, String Str2)
    {
        StringBuffer HexStr;
        String HexStr1 = Str1;
        String  HexStr2 = Str2;

        int i, iHexStrLenLow, iHexStrLenGap;

        int iHexStr1Len = HexStr1.length();
        int iHexStr2Len = HexStr2.length();

        //获取最小的长度
        iHexStrLenLow = iHexStr1Len<iHexStr2Len?iHexStr1Len:iHexStr2Len;
        if( iHexStr1Len>iHexStr2Len )
           HexStr=new StringBuffer(Str1) ;
        else
            HexStr=new StringBuffer(Str2);
        //获取长度差值
        iHexStrLenGap = Math.abs( iHexStr1Len-iHexStr2Len );

        //两个十六进制的字符串进行异或
        for( i=0; i<iHexStrLenLow; i++ )
        {
            int a=(int)HexStr1.charAt(i);
            int b=(int)HexStr2.charAt(i);
            int c=a^b;
            char d=(char)c;
            HexStr.setCharAt(i, d);
                    // HexStr.setCharAt(i,(int)HexStr1.charAt(i) ^ (int)HexStr2.charAt(i));
        //HexStr.setCharAt(i, int2char( HexStr.charAt(i) );
        }
        /*
        if( iHexStr1Len>iHexStr2Len )
            memcpy( HexStr+i, HexStr1+i, iHexStrLenGap );
        else if( iHexStr1Len<iHexStr2Len )
            memcpy( HexStr+i, HexStr2+i, iHexStrLenGap );
            */
        HexStr.setCharAt(iHexStrLenLow+iHexStrLenGap,(char)0x00);
    //*( HexStr+iHexStrLenLow+iHexStrLenGap ) = 0x00;
    return HexStr.toString();
    }

//字节填充部分

    //'7D'替换为'7D5D'
    static String convert_7D(String str){
        StringBuffer tmp = new StringBuffer(str);
        int count = 0;
        int begin = -1;
        //找到所有包含"7D"的起始位置，并在后面插入"5D"
        while((begin=str.indexOf("7D",begin+1))>0)
        {
            count++;
            //每次插入2个字符之后，下一次插入向后移动两个位置
            tmp = tmp.insert(begin+2*count,"5D");
        }
        return tmp.toString();
    }

    //'7E'替换为'7D5E'
    static String convert_7E(String str){
        StringBuffer tmp = new StringBuffer(str);
        int count = 0;
        int begin = -1;
        //找到所有包含"7D"的起始位置，将"7D"代替为"7D5E"
        while((begin=str.indexOf("7E",begin+1))>0)
        {
            //将begin开始的两个字符用"7D5E"代替

            tmp=tmp.replace(begin+2*count,begin+2*count+2,"7D5E");
            count++;
        }
        return tmp.toString();
    }


//小于20的字符，1X,0X都需要与20作异或运算，得出结果XX，前面再加上7D

    //'1X'替换为'7DXX',XX为'1X'与'20'异或的结果
    static String convert_1X(String str){
        StringBuffer tmp = new StringBuffer(str);
        int count = 0;
        int begin = -1;
        //找到所有包含"1"的起始位置，将"1X"与"20"作异或运算代
        while((begin=str.indexOf("1",begin+1))>0){
            //每两个字符进行一次提取，例如1110，只需提取11和10
            if(begin%2==0){

                //提取begin位置的两个字符与20作异或运算得到结果XX
                StringBuffer result=new StringBuffer(hexstrxor( str.substring(begin,2), "20"));
                // 结果为7DXX
                StringBuffer replace=new StringBuffer("7D");
                replace = replace.append(result);
                //替换原来的字符
                tmp=tmp.replace(begin+2*count,2,replace.toString());
                count++;
            }
        }
        return tmp.toString();
    }

    //'0X'替换为'7DXX',XX为'0X'与'20'异或的结果，同上
    static String convert_0X(String str){
        StringBuffer tmp = new StringBuffer(str);
        int count = 0;
        int begin = -1;

        while((begin=str.indexOf("0",begin+1))>0){
            if(begin%2==0){

                StringBuffer result=new StringBuffer(hexstrxor( str.substring(begin,2), "20"));
                StringBuffer replace=new StringBuffer("7D");
                replace = replace.append(result);
                tmp=tmp.replace(begin+2*count,2,replace.toString());
                count++;
            }
        }
        return tmp.toString();
    }

    static String Byte_Fill(String str){
        //注意先转换7D
        String str1=convert_7D(str);
        String str2=convert_7E(str1);
        String str3=convert_1X(str2);
        String str4=convert_0X(str3);
        return str4;
    }

    public static void  main(String args[])
    throws IOException {
        
        String ini_path="Byte_Fill_data.ini";
        if(args.length>=1)
        {
            ini_path=args[1];
        }
        String InfoString=readconfigure.readConfigFile(ini_path,"InfoString");
        System.out.println("-------------读取配置文件中待发送的数据信息-----");
        System.out.println(InfoString);
        //字节填充
        System.out.println("-------------字节填充之后的数据信息-------------");
        String Fill_String = Byte_Fill(InfoString);
        System.out.println(Fill_String);
        System.out.println("-------------帧首部的四部分---------------------");
        System.out.println("帧起始标志:7E  "+"A:FF  "+"C:03  "+"协议：0021 或 C021 或 8021");
        System.out.println("-------------帧尾部的两部分---------------------");
        System.out.println("FCS:XX  "+"帧结束标志：7E");
        System.out.println("-------------字节填充后的发送帧为---------------");
        System.out.println("若为IP数据报：                "+"7E FF 03 0021 "+Fill_String+" XX"+" 7E");
        System.out.println("若为ppp链路控制协议LCP的数据："+"7E FF 03 C021 "+Fill_String+" XX"+" 7E");
        System.out.println("若为网络层的控制数据：        "+"7E FF 03 8021 "+Fill_String+" XX"+" 7E");
        System.out.println("-------------字节删除后的接收帧为---------------");
        System.out.println("若为IP数据报：                "+"7E FF 03 0021 "+InfoString+" XX"+" 7E");
        System.out.println("若为ppp链路控制协议LCP的数据："+"7E FF 03 C021 "+InfoString+" XX"+" 7E");
        System.out.println("若为网络层的控制数据：        "+"7E FF 03 8021 "+InfoString+" XX"+" 7E");

        System.out.println("-------------按任意键退出程序-------------------");
      
    }

}

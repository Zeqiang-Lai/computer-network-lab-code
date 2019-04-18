
public class BinaryChange {
    /**
     * byte数组转换为二进制字符串,每个字节以","隔开
     * **/


    public static String conver2HexStr(byte []b)
    {
        StringBuffer result = new StringBuffer();
        for (int i = 0; i < b.length; i++) {
            result.append(String.format("%8s", Integer.toBinaryString(b[i] & 0xFF)).replace(' ', '0'));
        }
        return result.toString().substring(0, result.length());
    }

    /**
     * 二进制字符串转换为byte数组,每个字节以","隔开
     * **/
    public static byte[] conver2HexToByte(String str)
    {
        byte[] byteArray = new byte[str.length() / 8];
        for (int i = 0; i < byteArray.length; i++){
            String subStr = str.substring(8 * i, 8 * i + 8);
            byteArray[i] = ((byte)Integer.parseInt(subStr, 2));
        }
        return byteArray;
    }
}

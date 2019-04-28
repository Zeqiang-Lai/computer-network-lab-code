
public class bitfill {
    static String BitFill(String str)
    {
        StringBuilder tmp = new StringBuilder (str);
        int count = 0;//记录11111出现的次数
        int begin = -1;
        while((begin=str.indexOf("11111",begin+1))!=-1)
        {
            //插入0的位置为，起始位置+5+count
            tmp = tmp.insert(begin+5+count,"0");
            count++;
            begin = begin+4;
        }
        return tmp.toString();
    }

}

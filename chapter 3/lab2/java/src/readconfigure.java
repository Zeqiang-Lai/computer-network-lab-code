import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

public class readconfigure {
    static String readConfigFile(String cfgfilepath, String key)
            throws IOException {
        String strline, value = "";
        BufferedReader mybuf_reader = new BufferedReader(new FileReader(cfgfilepath));
        while((strline=mybuf_reader.readLine())!=null)
        {
            int index = strline.indexOf("=");
            if (index > -1) {
                String tmpkey = strline.substring(0, index);
                if (tmpkey.equals(key))
                return strline.substring(index + 1);
            }
        }
        mybuf_reader.close();
        return null;
    }
}


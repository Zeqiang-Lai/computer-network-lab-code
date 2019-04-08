package lab4;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.HashMap;
import java.util.Map;
import java.util.Properties;

public class ReadIniFile {
//	public static void main(String[] args) throws IOException {
//		// TODO Auto-generated method stub
//		ReadIniFile rf = new ReadIniFile();
//		Map<String, String> map = rf.readFile();
//		String port = map.get("UDPPort");
//		String FilterError = map.get("FilterError");
//		String FilterLost = map.get("FilterLost");
//		String GenXString = map.get("GenXString");
//		System.out.println("UDPPort " +port);
//		System.out.println("FilterError " + FilterError);
//		System.out.println("FilterLost " + FilterLost);
//		System.out.println("GenxString " + GenXString);
//	}
	public static Map<String, String> readFile() throws IOException {
		InputStream in = new FileInputStream(new File("D:\\lab4\\bin\\test.ini"));
		BufferedReader br = new BufferedReader(new InputStreamReader(in));
		Properties props = new Properties();
		props.load(br);
		Map<String, String> map = new HashMap<String, String>();
		for(Object s : props.keySet()) {
			map.put(s.toString(), props.getProperty(s.toString()));
		}
		return map;
		
		
	}

}

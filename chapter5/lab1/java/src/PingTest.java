package test;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.Scanner;

public class PingTest {

	public static void main(String[] args) throws Exception {
		// TODO Auto-generated method stub
		InetAddress address = null;
		
		Scanner s = new Scanner(System.in);
		String raw = s.nextLine();
		String[] tmp = raw.trim().split(" ");
		if(tmp[0].contentEquals("ping") && tmp.length <3 ){
			ping(tmp[1]);
		}else {
			System.out.println("Usage: ping <host>");
		}
		
	}
	
	 public static void ping(String ipAddress) throws Exception {
	        String line = null;
	        try {
	            Process pro = Runtime.getRuntime().exec("ping " + ipAddress);
	            BufferedReader buf = new BufferedReader(new InputStreamReader(
	                    pro.getInputStream()));
	            while ((line = buf.readLine()) != null)
	                System.out.println(line);
	        } catch (Exception ex) {
	            System.out.println(ex.getMessage());
	        }
	    }


}

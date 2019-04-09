#include<iostream>
#include<winsock2.h>
#include<string>
#include<thread>
#include"CRC.h"
#pragma comment(lib, "ws2_32.lib")
using namespace std;

class Receiver
{
public:
	Receiver() {
		buffer = "";
		frame_expected = 0;
	}
	string from_physical_layer(char *);
	string to_physical_layer();
	void inc() {
		frame_expected = 1 - frame_expected;
	}
private:
	string buffer;
	int frame_expected;
};

string Receiver::from_physical_layer(char *bitstring)
{
	CRC c;
	return c.crc_check(string(bitstring));
}

string Receiver::to_physical_layer()
{
	string frame = "";
	int ack = 1 - frame_expected;
	frame += frame_expected + '0';
	frame += ack + '0';
	return frame;
}

int main()
{
	Receiver r;
	char c[50] = "0011011110110110100101010101011011101100101101100";
	cout << r.from_physical_layer(c) << endl;
	cout << r.to_physical_layer();
}
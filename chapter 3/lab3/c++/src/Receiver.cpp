#include <string>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include<iostream>
#include<fstream>
using namespace std;

const int FRAME_SIZE = 1024;
int filelost = 0;
int frameerror = 0;

#pragma comment(lib, "Ws2_32.lib")

inline char XOR(char a, char b)
{
	return a == b ? '0' : '1';
}

class CRC
{
public:
	CRC() {
		poly = "10001000000100001";
	}
	void Init_CRC(string p) {
		poly = p;
	}
	string crc_remainder(string input_bitstring)
	{
		int len_input = input_bitstring.length();
		int len_poly = poly.length();
		string padding(len_poly - 1, '0');
		string padded_input = input_bitstring.append(padding);

		for (int i = 0; i < len_input; ++i) {
			if (padded_input[i] == '0')
				continue;
			for (int j = i; j < i + len_poly; ++j)
				padded_input[j] = XOR(padded_input[j], poly[j - i]);
		}
		return padded_input.substr(len_input, len_poly - 1);
	}
	string crc_check(string input_bitstring)
	{
		int len_poly = poly.length();
		int len_input = input_bitstring.length() - len_poly + 1;
		string padded_input = input_bitstring;

		for (int i = 0; i < len_input; ++i) {
			if (padded_input[i] == '0')
				continue;
			for (int j = i; j < i + len_poly; ++j)
				padded_input[j] = XOR(padded_input[j], poly[j - i]);
		}
		return padded_input;
	}
private:
	string poly;
};

class UDPSocket
{
public:
	UDPSocket() { isBind = 0; }
	int initialize(unsigned short port_, const char* ipAddr_);
	int send(const char* buff, int len);
	int recv(char* buff, int len);
	int close();
private:
	int recv_bind();
	int isBind;
	unsigned short port;
	std::string ipAddr;
	WSADATA wsaData;
	SOCKET msocket;
	sockaddr_in recvAddr;
	sockaddr_in senderAddr;
};


int UDPSocket::initialize(unsigned short port_, const char* ipAddr_)
{
	int iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}
	msocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (msocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	recvAddr.sin_family = AF_INET;
	recvAddr.sin_port = htons(port_);
	recvAddr.sin_addr.s_addr = inet_addr(ipAddr_);

	return 0;
}

int UDPSocket::send(const char* buff, int len)
{
	int iResult = sendto(msocket,
		buff, len, 0, (SOCKADDR *)& recvAddr, sizeof(recvAddr));
	if (iResult == SOCKET_ERROR) {
		printf("sendto failed with error: %d\n", WSAGetLastError());
		return 1;
	}
	return 0;
}

int UDPSocket::recv(char* buff, int len)
{
	if (isBind == 0) {
		if (recv_bind() == 1)
			return 1;
		isBind = 1;
	}
	int SenderAddrSize = sizeof(senderAddr);
	int iResult;
	iResult = recvfrom(msocket,
		buff, len, 0, (SOCKADDR *)& senderAddr, &SenderAddrSize);
	if (iResult == SOCKET_ERROR) {
		printf("recvfrom failed with error %d\n", WSAGetLastError());
		return 1;
	}
	return 0;
}

int UDPSocket::close()
{
	int iResult = closesocket(msocket);
	if (iResult == SOCKET_ERROR) {
		printf("closesocket failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}
	WSACleanup();
	return 0;
}

int UDPSocket::recv_bind()
{
	bind(msocket, (SOCKADDR *)& recvAddr, sizeof(recvAddr));
	return 0;
}

class Receiver
{
public:
	Receiver() {
		frame_expected = 0;
	}
	bool from_physical_layer(string bitstring,int len)
	{
		if (bitstring[0] == frame_expected+'0')
		{
			CRC c;
			string temp = c.crc_check(bitstring.substr(1, len - 1));
			cout << "CRC check: " << temp << endl;
			for (int i = 0; i < temp.length(); i++)
				if (temp[i] == '1')
				{
					cout << "frame error" << endl;
					return false;
				}
			cout << "valid frame";
			cout << "      sending ACK: " << frame_expected << endl;
			return true;
		}
		else
			return false;
	}
	string to_physical_layer()
	{
		string frame = "";
		frame += (frame_expected + '0');
		inc();
		return frame;
	}
	void inc() {
		frame_expected = 1 - frame_expected;
	}
	int frame_expected;
};

void pack_frame(int* num, const char* buff, int len, char* frame)
{
	memcpy(frame, num, sizeof(int));
	memcpy(frame + sizeof(int), buff, len + 1);
}

void unpack_frame(int* num, char* buff, char* frame, int size)
{
	memcpy(num, frame, sizeof(int));
	memcpy(buff, frame + sizeof(int), size - sizeof(int));
}

void init(CRC& c, UDPSocket& so_send, UDPSocket& so_recv)
{
	ifstream infile("init.txt");
	if (!infile)
		cout << "open file failed!" << endl;
	string temp[7];
	int t = 0;
	while (infile >> temp[t])
	{
		temp[t] = temp[t].substr(5, temp[t].length() - 5);
		t++;
	}
	c.Init_CRC(temp[0]);
	int send_port = 0, recv_port = 0;
	for (int i = 0; i < temp[3].length(); i++)
	{
		int sum = 1;
		for (int j = temp[3].length() - i - 1; j > 0; j--)
			sum *= 10;
		send_port += (temp[3][i] - '0')*sum;
	}
	for (int i = 0; i < temp[4].length(); i++)
	{
		int sum = 1;
		for (int j = temp[4].length() - i - 1; j > 0; j--)
			sum *= 10;
		recv_port += (temp[4][i] - '0')*sum;
	}
	so_send.initialize(send_port, temp[2].c_str());
	so_recv.initialize(recv_port, temp[2].c_str());
	filelost = (temp[5][0] - '0') * 10 + (temp[5][1] - '0');
	frameerror == (temp[6][0] - '0') * 10 + (temp[6][1] - '0');
	infile.close();
}

int main()
{
	UDPSocket so_send, so_recv;
	CRC c;
	Receiver r;

	char frame[FRAME_SIZE];
	init(c, so_send, so_recv);
	
	int count = 0;
	while (true)
	{
		count++;
		char recv_buf[FRAME_SIZE], recv_frame[FRAME_SIZE], send_frame[FRAME_SIZE];
		so_recv.recv(recv_frame, FRAME_SIZE);
		int len;
		unpack_frame(&len, recv_buf, recv_frame, FRAME_SIZE);
		if (count % frameerror != 2)
		{
			cout << "frame recv:";
			for (int i = 0; i < len; i++)
				cout << recv_buf[i];
			cout << endl;
			if (r.from_physical_layer(recv_buf, len))
			{
				string temp = r.to_physical_layer();
				int l = temp.length();
				pack_frame(&l, temp.c_str(), l, send_frame);
				so_send.send(send_frame, FRAME_SIZE);
				cout << "frame send:" << temp << endl;
			}
			else
			{
				cout << "frame error" << endl;
				Sleep(3000);
			}
		}
		else 
		{
			cout << "frame lost!" << endl;
			Sleep(3000);
		}
		cout << endl;
		Sleep(1000);
	}

	so_send.close();
	so_recv.close();
}


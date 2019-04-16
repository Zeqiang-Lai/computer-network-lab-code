#include "UDPSocket.h"
#include <stdio.h>

int UDPSocket::initialize(unsigned short port_, const char* ipAddr_)
{
	int iResult;
	port = port_;
	//----------------------
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	//---------------------------------------------
	// Create a socket for sending data
	msocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (msocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	// Set up the RecvAddr structure with the IP address of
	// the receiver (in this example case "192.168.1.1")
	// and the specified port number.
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
	int iResult = bind(msocket, (SOCKADDR *)& recvAddr, sizeof(recvAddr));
	if (iResult != 0) {
		printf("bind failed with error: %d  | port=%d\n", WSAGetLastError(), port);
		return 1;
	}
	return 0;
}

void pack_frame(int* num, const char* buff, int len, char* frame)
{
	std::memcpy(frame, num, sizeof(int));
	std::memcpy(frame + sizeof(int), buff, len + 1);
}

void unpack_frame(int* num, char* buff, char* frame, int size)
{
	std::memcpy(num, frame, sizeof(int));
	std::memcpy(buff, frame + sizeof(int), size - sizeof(int));
}
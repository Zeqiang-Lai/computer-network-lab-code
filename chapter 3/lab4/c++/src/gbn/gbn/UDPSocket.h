#pragma once
#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <string>
#include <winsock2.h>
#include <Ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

class UDPSocket
{
public:
	UDPSocket() { isBind = 0; }
	~UDPSocket() { close(); }
	int initialize(unsigned short port_, const char* ipAddr_);

	int send(const char* buff, int len);

	int recv(char* buff, int len);

	int close();

private:
	int recv_bind();

	int isBind;
	unsigned short port;

	WSADATA wsaData;
	SOCKET msocket;

	sockaddr_in recvAddr;
	sockaddr_in senderAddr;
};

/**
 * Pack a number and a string into a frame.
 *
 * @param num: the address of the number you want to pack.
 * @param buff: the address of the string you want to pack.
 * @param len: length of the string.
 * @param frame: the address of the frame.
 */
void pack_frame(int* num, const char* buff, int len, char* frame);

/**
 * Unpack frame into a number and a string.
 *
 * @param num: unpacked number will be stored in this address.
 * @param buff: unpacked string will be stored in this address.
 * @param frame: the address of the frame you want to unpack.
 * @param size: the size of the frame.
 */
void unpack_frame(int* num, char* buff, char* frame, int size);
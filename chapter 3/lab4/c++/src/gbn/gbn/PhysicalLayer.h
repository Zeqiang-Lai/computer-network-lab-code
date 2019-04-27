#pragma once
#include <array>
#include <queue>

#include "UDPSocket.h"
#include "Frame.h"

class DataLinkLayer;

class PhysicalLayer
{
public:

	PhysicalLayer(int localport, int remoteport, DataLinkLayer* data_link_layer);

	void send(BytesFrame& data);
	int receive(BytesFrame& data);

	void stopMonitor();

	static DWORD WINAPI startMonitorRemotePort(LPVOID lpParam);

	DWORD monitorRemotePort();

private:
	bool running = true;

	int m_localport;
	int m_remoteport;
	UDPSocket m_send_socket;
	UDPSocket m_recv_socket;

	std::queue<BytesFrame> m_recv_buff;//收到数据缓冲区
	DataLinkLayer* m_data_link_layer;
};
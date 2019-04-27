#include "PhysicalLayer.h"
#include "DataLinkLayer.h"


PhysicalLayer::PhysicalLayer(int localport, int remoteport, DataLinkLayer * data_link_layer)
{
	m_localport = localport;
	m_remoteport = remoteport;
	m_data_link_layer = data_link_layer;

	m_send_socket.initialize(m_remoteport, "127.0.0.1");
	m_recv_socket.initialize(m_localport, "127.0.0.1");

	DWORD thread_id;
	CloseHandle(CreateThread(NULL, 0, startMonitorRemotePort, (void*)this, 0, &thread_id));
}

void PhysicalLayer::send(BytesFrame& data)
{
	// simulate loss
	// send
	//printf("Sending frame...\n");
	char bytes[MAX_FRAME_SIZE];
	for (int i = 0; i < MAX_FRAME_SIZE; ++i)
		bytes[i] = data[i];
	int status = m_send_socket.send(bytes, MAX_FRAME_SIZE);
	//printf("Finish sending: %d\n\n", status);
	printf("--------------------·¢ËÍ³É¹¦---------------------------------------\n\n");
}

int PhysicalLayer::receive(BytesFrame& data)
{
	if (m_recv_buff.size() > 0) {
		data = m_recv_buff.front();
		m_recv_buff.pop();
		return data.size();
	}
	return 0;
}

void PhysicalLayer::stopMonitor()
{
	running = false;
}

DWORD __stdcall PhysicalLayer::startMonitorRemotePort(LPVOID lpParam)
{
	PhysicalLayer* This = (PhysicalLayer*)lpParam;
	return This->monitorRemotePort();
}

DWORD PhysicalLayer::monitorRemotePort()
{
	//printf("Start monitoring remote port...\n");
	while (running)
	{
		if (m_recv_buff.size() > 0) {
			if (m_data_link_layer != NULL)
				m_data_link_layer->onFrameArrival(m_data_link_layer);
			continue;
		}
		BytesFrame buff;
		//printf("Block recving...\n");

		char bytes[MAX_FRAME_SIZE];
		int status = m_recv_socket.recv(bytes, MAX_FRAME_SIZE);
		for (int i = 0; i < MAX_FRAME_SIZE; ++i)
			buff[i] = bytes[i];

		//printf("Recv status = %d\n", status);
		m_recv_buff.push(buff);
	}
	return 0;
}

#pragma once
#include <string>
#include <queue>
#include <winsock2.h>
#include "PhysicalLayer.h"
#include "Frame.h"
#include "CRC.h"

#include <Windows.h>

#define MAX_SEQ 9
#define MAX_EVENT_BUF_SIZE 20

enum EventType { network_layer_ready, frame_arrival, cksum_err, timeout };

class DataLinkLayer
{
public:
	//friend class PhyiscalLayer;
	DataLinkLayer(int localport, int remoteport, std::string host_name);

	int send(std::string data);
	std::string recv();

	void start();
	void stop();

	// TODO: make them private.
	void onFrameArrival(DataLinkLayer* dl_layer);
	void onTimeout(DataLinkLayer* dl_layer);

private:
	bool running = true;

	HANDLE sem_lock;
	HANDLE sem_not_empty;

	PhysicalLayer m_phsical_layer;

	std::queue<std::string> m_cur_data;//ÔÝ´æÍøÂç²ãÊý¾Ý
	std::queue<std::string> m_recv_buff;

	std::vector<EventType> m_event_buff;
	std::array<Frame, MAX_SEQ + 1> m_frames_to_send;//´°¿Ú

	std::string m_sem_lock_name;
	std::string m_sem_not_empty_name;

	bool isBuffFull;

	void enableNetworkLayer() { isBuffFull = false; }
	void disableNetworkLayer() { isBuffFull = true; }

	static DWORD startMainthread(LPVOID lpParam);
	DWORD mainThread();

	void increment(int& seq);
	bool between(int a, int b, int c);

	void toPhysicalLayer(int frame_nr, int frame_expected);
	Frame fromPhysicalLayer();

	Frame fromNetworkLayer();
	void toNetworkLayer(Frame frame);

	EventType waitForEvent();
};
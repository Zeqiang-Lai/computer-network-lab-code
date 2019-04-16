#pragma once
#include <winsock2.h>

class DataLinkLayer;

class Timer {
	bool clear = false;
	
public:
	int m_id;

	Timer(){}
	Timer(int id) :m_id(id) {}

	void setTimeout(DataLinkLayer* dl_layer, int delay);

	void stop();

	static DWORD WINAPI startTimerThread(LPVOID lpParam);

	DWORD timerThread();

private:
	DataLinkLayer * m_data_link_layer;
	int m_delay;
};

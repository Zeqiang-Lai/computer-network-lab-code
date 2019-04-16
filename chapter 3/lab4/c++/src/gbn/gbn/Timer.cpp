#include "Timer.h"
#include "DataLinkLayer.h"
#include <time.h>
#include <thread>
#include <chrono>
void Timer::stop() {
	this->clear = true;
}

DWORD __stdcall Timer::startTimerThread(LPVOID lpParam)
{
	Timer* This = (Timer*)lpParam;
	return This->timerThread();
}

void print_current_time1()
{
	time_t timer;
	char buffer[26];
	struct tm tm_info;

	time(&timer);
	localtime_s(&tm_info, &timer);

	strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", &tm_info);
	printf("%s\n", buffer);
}

DWORD Timer::timerThread()
{
	//printf("Begin Sleep: %d ", m_id);
	//print_current_time1();
	//printf("m_delay:%d\n", m_delay);
	std::this_thread::sleep_for(5s);
	//Sleep(5000);

	//printf("End Sleep: %d ", m_id);
	//print_current_time1();
	if (this->clear)
		return 0;
	//printf("ontime out\n");
	m_data_link_layer->onTimeout(m_data_link_layer);
	return 0;
}

void Timer::setTimeout(DataLinkLayer* dl_layer, int delay) {

	m_data_link_layer = dl_layer;
	m_delay = delay;
	this->clear = false;

	DWORD thread_id;
	CloseHandle(CreateThread(NULL, 0, startTimerThread, (void*)this, 0, &thread_id));
}
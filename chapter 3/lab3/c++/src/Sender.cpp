#include<iostream>
#include<winsock2.h>
#include<string>
#include<thread>
#include"Timer.h"
#include"CRC.h"
#pragma comment(lib, "ws2_32.lib")
using namespace std;

enum Event_type{frame_arrival,cksum_err,timeout,waiting}event;
const int BUF_SIZE = 1024;
const int FilterLost = 10;

class Sender
{
public:
	Sender() {
		event = waiting;
		timer;
		buffer = "01101111011011010010101010101101";
		next_frame_to_send = 0;
	}
	~Sender() {};
	//启动定时器,定时器运行在线程，超时时将event置为timeout
	void SetTimer() {
		thread t1(&Timer::StartTimer, &timer);
		t1.join();
		if (timer.timeout)
		{
			cout << "timeout" << endl;
			event = timeout;
		}
	}
	//等待事件，在ACK到达后将event置为frame_arrival
	void wait_for_event();
	//返回将要发送的帧
	string to_physical_layer();
	//传入接收的ACK，判断帧信息，是否发送下一帧
	void from_physical_layer(char *);
	//转换帧序号
	void inc() {
		next_frame_to_send = 1 - next_frame_to_send;
	}
	int next_frame_to_send;
private:
	Event_type event;
	Timer timer;
	string buffer;
};

void Sender::wait_for_event()
{
	if (event == waiting)
	{
		event = frame_arrival;
	}
}

string Sender::to_physical_layer()
{
	CRC crc;
	string frame = "";
	frame += next_frame_to_send+'0';
	frame += buffer;
	frame += crc.crc_remainder(buffer);
	return frame;
}

void Sender::from_physical_layer(char *rec)
{
	if (rec[0] == (next_frame_to_send+'0') && rec[1] == ((1- next_frame_to_send)+'0'))
	{
		timer.CancleTimer();
		inc();
	}
}

int main()
{
	Sender s;
	s.SetTimer();
	while (true)
	{
		cout << s.to_physical_layer() << endl;
		cout << s.next_frame_to_send << endl;
		char t[3] = { '1','1' };
		s.from_physical_layer(t);
		cout << s.next_frame_to_send << endl;
		Sleep(1000);
	}
}

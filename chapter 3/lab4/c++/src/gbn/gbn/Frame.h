#pragma once

#define MAX_FRAME_SIZE 1024
#define MAX_FRAME_PAYLOAD 1000
#define TIMEOUT_DELAY 10000

#include <winsock2.h>
#include <array>
#include <iostream>
#include "Timer.h"
#include <Windows.h>

typedef std::array<char, MAX_FRAME_SIZE> BytesFrame;

class DataLinkLayer;

class Frame
{
public:
	int seq;//序号
	int ack;//ack
	int length;//长度
	Timer t;

	//HANDLE sem_lock;
	//HANDLE sem_not_empty;

	std::array<char, MAX_FRAME_PAYLOAD> payload;//数据

	Frame();

	Frame(BytesFrame data);//物理层数据转换为帧

	BytesFrame toBytes();//帧转换为物理层数据

	void startTimer(DataLinkLayer* data_link_layer);

	void stopTimer();

private:

	DataLinkLayer * m_data_link_layer;
};
#include "Frame.h"
#include "DataLinkLayer.h"
#include "Timer.h"
#include <stdio.h>

Frame::Frame(std::array<char, MAX_FRAME_SIZE> data)
{
	char* tmp = data.data();
	memcpy(&seq, tmp, sizeof(int)); tmp += sizeof(int);
	memcpy(&ack, tmp, sizeof(int)); tmp += sizeof(int);
	memcpy(&length, tmp, sizeof(int)); tmp += sizeof(int);
	for (int i = 0; i < length; ++i) {
		payload[i] = *(tmp + i);
	}
	t = Timer(seq);
	//sem_lock = CreateSemaphore(NULL, 1, 1, TEXT("sem_lock"));
	//sem_not_empty = CreateSemaphore(NULL, 0, MAX_EVENT_BUF_SIZE, TEXT("sem_not_empty"));
}

Frame::Frame() : seq(-1), ack(-1), length(0)
{
	t = Timer(seq);
	//sem_lock = CreateSemaphore(NULL, 1, 1, TEXT("sem_lock"));
	//sem_not_empty = CreateSemaphore(NULL, 0, MAX_EVENT_BUF_SIZE, TEXT("sem_not_empty"));
}

std::array<char, MAX_FRAME_SIZE> Frame::toBytes()
{
	char tmp[MAX_FRAME_SIZE];
	char* ptmp = tmp;
	memcpy(ptmp, &seq, sizeof(int)); ptmp += sizeof(int);
	memcpy(ptmp, &ack, sizeof(int)); ptmp += sizeof(int);
	memcpy(ptmp, &length, sizeof(int)); ptmp += sizeof(int);
	for (int i = 0; i < length; ++i) {
		*(ptmp + i) = payload[i];
	}
	BytesFrame array_tmp;
	for (int i = 0; i < MAX_FRAME_SIZE; ++i)
		array_tmp[i] = tmp[i];
	return array_tmp;
}


void Frame::startTimer(DataLinkLayer* data_link_layer)
{
	//cout << "| StartTimer" << endl;
	//创建线程计时
	//如果超时   m_data_link_layer->onTimeout();
	t.m_id = seq;
	t.setTimeout(data_link_layer, 5000);
}

void Frame::stopTimer()
{
	t.stop();
	//cout << "| stopTimer" << endl;
}

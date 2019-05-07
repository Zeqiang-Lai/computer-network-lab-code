#include "DataLinkLayer.h"
#include <time.h>

int error_num = 4;
int loss_num = 7;

//向物理层发送数据
void DataLinkLayer::toPhysicalLayer(int frame_nr, int frame_expected)
{
	//构造帧
	Frame& frame = m_frames_to_send[frame_nr];//下一个要传的数据
	frame.seq = frame_nr;
	frame.ack = (frame_expected + MAX_SEQ) % (MAX_SEQ + 1);

	printf("| 待发送帧: [Num=%d] [Ack=%d] [Data-Crc='%s']\n", frame.seq, frame.ack, frame.payload);

	if (frame.seq != loss_num) {
		//将帧转换为物理层数据
		BytesFrame data = frame.toBytes();

		//开始计时并发送数据，每一个帧对应一个计时
		//frame.stopTimer();
		//Sleep(50);
		frame.startTimer(this);
		m_phsical_layer.send(data);
	}
	else {
		printf("--------------------模拟丢失---------------------------------------\n\n");
		if (loss_num - 1 < 0) {
			loss_num = MAX_SEQ;
		}
		else {
			loss_num -= 1;
		}
	}

}

//从物理层收到数据，转换为帧
Frame DataLinkLayer::fromPhysicalLayer()
{
	BytesFrame data;
	// 一定不会失败,因为该函数是有数据才会被调用.
	while (m_phsical_layer.receive(data) == 0)
		printf("Receive error\n");

	Frame frame(data);
	//printf("From physical: seq=%d ack=%d length=%d\n", frame.seq, frame.ack, frame.length);
	printf("--------------------接收-------------------------------------------\n");
	printf("| 收到帧: [Num=%d] [Ack=%d] [Data-Crc='%s']\n", frame.seq, frame.ack, frame.payload);

	//收到数据后校验Crc
	char data_crc[MAX_FRAME_PAYLOAD + 1];
	//NUM==n时模拟出错，-1为不出错

	if (frame.seq == error_num) {
		printf("| 模拟出错\n");
		for (int i = 0; i < frame.length; ++i)
			data_crc[i] = frame.payload[i];
		data_crc[0] = '2';
		data_crc[frame.length] = '\0';
		if (error_num - 1 < 0)
			error_num = MAX_SEQ;
		else
			error_num -= 1;
	}
	else {
		//转换为string类型
		for (int i = 0; i < frame.length; ++i)
			data_crc[i] = frame.payload[i];
		data_crc[frame.length] = '\0';
	}
	std::string msg(data_crc);
	//删去'-'
	int pos = msg.find('-');
	msg = msg.substr(0, pos) + msg.substr(pos + 1, msg.length());
	//校验
	CRC crc;
	string check = crc.crc_check(msg);
	cout << "| Crc_check:" << check;
	int pos1 = check.find("1");
	if (pos1 == -1) {
		cout << "   CRC校验成功" << endl;
		return Frame(data);
	}
	else {
		cout << "   CRC校验失败" << endl;
		return Frame();
	}
}

//从发送缓冲区获取数据转换为帧
Frame DataLinkLayer::fromNetworkLayer()
{
	std::string data = m_cur_data.back();
	m_cur_data.pop();

	//计算CRC,data = 数据+CRC
	CRC crc;
	string remainder = '-' + crc.crc_remainder(data);
	data.append(remainder);

	Frame frame;
	for (int i = 0; i < data.size(); ++i) {
		frame.payload[i] = data[i];
	}
	frame.length = data.size();
	return frame;
}

//帧转换为网络层数据，加入接收缓冲区
void DataLinkLayer::toNetworkLayer(Frame frame)
{
	//printf("frame[%d]: to network layer\n", frame.seq);
	char data[MAX_FRAME_PAYLOAD + 1];
	for (int i = 0; i < frame.length; ++i)
		data[i] = frame.payload[i];
	data[frame.length] = '\0';
	std::string msg(data);
	m_recv_buff.push(msg);
}

//取出事件缓冲区的第一个事件
EventType DataLinkLayer::waitForEvent()
{
	//while (m_event_buff.empty());
	//printf("wait for event: sem_not_empty\n");
	WaitForSingleObject(sem_not_empty, INFINITE);
	//printf("wait for event: sem_lock\n");
	WaitForSingleObject(sem_lock, INFINITE);
	EventType event = m_event_buff.front();
	//printf("eventbuff: ");
	//for (int i = 0; i < m_event_buff.size();++i)
		//printf("%d ", m_event_buff[i]);
	//printf("\n");
	//printf("Get event: %d\n", event);
	ReleaseSemaphore(sem_lock, 1, NULL);
	//m_event_buff.shrink_to_fit();
	return event;
}

//帧到达
void DataLinkLayer::onFrameArrival(DataLinkLayer* dl_layer)
{
	//HANDLE sem_lock = OpenSemaphore(SEMAPHORE_MODIFY_STATE, TRUE, m_sem_lock_name.c_str());
	//HANDLE sem_not_empty = OpenSemaphore(SEMAPHORE_MODIFY_STATE, TRUE, m_sem_not_empty_name.c_str());

	WaitForSingleObject(dl_layer->sem_lock, INFINITE);
	//printf("on frame arrival: %d\n", m_event_buff.size());
	for (int i = 0; i < m_event_buff.size(); ++i) {
		//printf("loop: %d\n", i);
		if (m_event_buff[i] == frame_arrival) {
			ReleaseSemaphore(dl_layer->sem_lock, 1, NULL);
			//CloseHandle(sem_lock);
			//CloseHandle(sem_not_empty);
			return;
		}
	}
	//printf("Push event: frame arrival\n");
	m_event_buff.push_back(frame_arrival);//push_back尾部加入
	ReleaseSemaphore(dl_layer->sem_not_empty, 1, NULL);
	ReleaseSemaphore(dl_layer->sem_lock, 1, NULL);
	//CloseHandle(dl_layer->sem_lock);
	//CloseHandle(dl_layer->sem_not_empty);
}

//超时
void DataLinkLayer::onTimeout(DataLinkLayer* dl_layer)
{
	//HANDLE sem_lock = OpenSemaphore(SEMAPHORE_MODIFY_STATE, TRUE, m_sem_lock_name.c_str());
	//HANDLE sem_not_empty = OpenSemaphore(SEMAPHORE_MODIFY_STATE, TRUE, m_sem_not_empty_name.c_str());

	WaitForSingleObject(dl_layer->sem_lock, INFINITE);
	for (int i = 0; i < dl_layer->m_event_buff.size(); ++i) {
		if (dl_layer->m_event_buff[i] == timeout) {
			ReleaseSemaphore(dl_layer->sem_lock, 1, NULL);
			//CloseHandle(dl_layer->sem_lock);
			//CloseHandle(dl_layer->sem_not_empty);
			return;
		}
	}
	dl_layer->m_event_buff.push_back(timeout);
	ReleaseSemaphore(dl_layer->sem_not_empty, 1, NULL);
	ReleaseSemaphore(dl_layer->sem_lock, 1, NULL);
	//CloseHandle(dl_layer->sem_lock);
	//CloseHandle(dl_layer->sem_not_empty);
}

DataLinkLayer::DataLinkLayer(int localport, int remoteport, std::string host_name) : m_phsical_layer(PhysicalLayer(localport, remoteport, this))
{
	m_sem_lock_name = host_name.append("_sem_lock");
	m_sem_not_empty_name = host_name.append("_sem_not_empty");
}

//数据链路层发送数据
int DataLinkLayer::send(std::string data)
{
	//判断缓冲区是否已满
	if (isBuffFull)
		return -1;
	//TODO: check msg length

	m_cur_data.push(data);//加入发送的缓冲区

						  //HANDLE sem_lock = OpenSemaphore(SEMAPHORE_MODIFY_STATE, TRUE, m_sem_lock_name.c_str());
						  //HANDLE sem_not_empty = OpenSemaphore(SEMAPHORE_MODIFY_STATE, TRUE, m_sem_not_empty_name.c_str());

	if (sem_lock == NULL || sem_not_empty == NULL) {
		printf("open semaphore fail: %d\n", GetLastError());
	}
	//else {
	//printf("create semaphore Sucess\n");
	//}

	WaitForSingleObject(sem_lock, INFINITE);
	//printf("send event push\n");
	m_event_buff.push_back(network_layer_ready);//发送事件加入事件缓冲区

	if (!ReleaseSemaphore(sem_not_empty, 1, NULL))
		printf("relase sem not empty fail:%d\n", GetLastError());
	ReleaseSemaphore(sem_lock, 1, NULL);

	//CloseHandle(sem_lock);
	//CloseHandle(sem_not_empty);
	return data.length();
}

//数据链路层接收数据
std::string DataLinkLayer::recv()
{
	while (m_recv_buff.empty());
	std::string msg = m_recv_buff.back();//从接收缓冲区拿到数据
	m_recv_buff.pop();
	return msg;
}

//主线程
void DataLinkLayer::start()
{
	//create semaphores
	sem_lock = CreateSemaphore(NULL, 1, 1, m_sem_lock_name.c_str());
	sem_not_empty = CreateSemaphore(NULL, 0, MAX_EVENT_BUF_SIZE, m_sem_not_empty_name.c_str());

	DWORD thread_id;
	CloseHandle(CreateThread(NULL, 0, startMainthread, (void*)this, 0, &thread_id));
}

void DataLinkLayer::stop()
{
	m_phsical_layer.stopMonitor();
	running = false;
}

DWORD DataLinkLayer::startMainthread(LPVOID lpParam)
{
	DataLinkLayer* This = (DataLinkLayer*)lpParam;
	return This->mainThread();
}

void print_current_time()
{
	time_t timer;
	char buffer[26];
	struct tm tm_info;

	time(&timer);
	localtime_s(&tm_info, &timer);

	strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", &tm_info);
	printf("%s", buffer);
}


//因为是双工协议，不再有发方收方的界限
DWORD DataLinkLayer::mainThread()
{
	int nextFrameToSend = 0;//发送方第一帧要传0号帧
	int ackExpected = 0;    //发送方发送了0号帧，所以期待接收Ack0来得到确认
	int frameExpected = 0;  //接收方期待接收的是对方的0号帧
	Frame frame;			//初始化帧
	int nbuffered = 0;		//发送窗口初始数据个数为0
	int i;
	EventType event;		//初始化事件

	enableNetworkLayer();//网络层可以用，缓冲区没满

	while (running)
	{
		event = waitForEvent();//监听事件缓冲区
							   //printf("Event: %d\n", event);

							   //print_current_time();

		switch (event)
		{
			//发送方发送帧
		case network_layer_ready:
			frame = fromNetworkLayer();//从发送缓冲区获取数据

			printf("| Frame_expected=%d, Ack_expected=%d, Next_frame_to_send=%d\n", frameExpected, ackExpected, nextFrameToSend);

			//printf("network_layer_ready: %d %d %d\n", frame.length, frame.ack, frame.seq);//这时ack和seq为初始值-1
			;			m_frames_to_send[nextFrameToSend] = frame;//将要发送的帧加入发送窗口
			nbuffered += 1;//窗口数据个数加1

						   //printf("%d", nbuffered);
						   //到物理层发送数据：
			toPhysicalLayer(nextFrameToSend, frameExpected);
			increment(nextFrameToSend);
			break;

			//接收方接收帧	
		case frame_arrival:
			//若CRC校验失败此处返回的是空帧
			frame = fromPhysicalLayer();

			printf("| Frame_expected=%d, Ack_expected=%d\n", frameExpected, ackExpected);

			//如果CRC校验成功
			if (frame.length != 0) {
				//接收方收到帧后，首先核对是否为预期帧号(frameExpected)，
				//如果是的，则接收并frameExpected + 1，移动接收窗口

				if (frame.seq == frameExpected) {//保证同时运行
					toNetworkLayer(frame);//将数据加入接收缓冲区
					increment(frameExpected);
				}

				//判断 ack 是否落在发送窗口内
				while (between(ackExpected, frame.ack, nextFrameToSend)) {
					nbuffered -= 1;
					frame.stopTimer();
					increment(ackExpected);
					//printf("%d\n", ackExpected);
				}
				printf("--------------------接收完成---------------------------------------\n\n");
			}
			//如果CRC校验失败
			else {
				printf("--------------------接收失败---------------------------------------\n\n");
			}
			break;
			//超时重传
		case timeout:
			nextFrameToSend = ackExpected;
			for (int i = 1; i <= nbuffered; ++i) {
				printf("--------------------重传-------------------------------------------\n");
				printf("[Timeout]: retransmit(%d)\n", nextFrameToSend);
				toPhysicalLayer(nextFrameToSend, frameExpected);
				increment(nextFrameToSend);
			}
			break;
		}
		WaitForSingleObject(sem_lock, INFINITE);
		m_event_buff.erase(m_event_buff.begin());//删除第一个事件
		ReleaseSemaphore(sem_lock, 1, NULL);
		if (nbuffered < MAX_SEQ)
			enableNetworkLayer();
		else {
			printf("$$$$$ Disable\n");
			disableNetworkLayer();
		}
	}
	return 0;
}

void DataLinkLayer::increment(int& seq)
{
	if (seq < MAX_SEQ)
		seq = seq + 1;
	else
		seq = 0;
}

//用于判断收到的ACK序号是否在发送窗口内。a为下限，b为ack序号，c为上限
bool DataLinkLayer::between(int a, int b, int c)
{
	if (((a <= b) && (b < c)) || ((c < a) && (a <= b)) || ((b < c) && (c < a)))
		return true;
	else
		return false;
}

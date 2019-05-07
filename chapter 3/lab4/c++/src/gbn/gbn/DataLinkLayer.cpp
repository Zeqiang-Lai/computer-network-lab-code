#include "DataLinkLayer.h"
#include <time.h>

int error_num = 4;
int loss_num = 7;

//������㷢������
void DataLinkLayer::toPhysicalLayer(int frame_nr, int frame_expected)
{
	//����֡
	Frame& frame = m_frames_to_send[frame_nr];//��һ��Ҫ��������
	frame.seq = frame_nr;
	frame.ack = (frame_expected + MAX_SEQ) % (MAX_SEQ + 1);

	printf("| ������֡: [Num=%d] [Ack=%d] [Data-Crc='%s']\n", frame.seq, frame.ack, frame.payload);

	if (frame.seq != loss_num) {
		//��֡ת��Ϊ���������
		BytesFrame data = frame.toBytes();

		//��ʼ��ʱ���������ݣ�ÿһ��֡��Ӧһ����ʱ
		//frame.stopTimer();
		//Sleep(50);
		frame.startTimer(this);
		m_phsical_layer.send(data);
	}
	else {
		printf("--------------------ģ�ⶪʧ---------------------------------------\n\n");
		if (loss_num - 1 < 0) {
			loss_num = MAX_SEQ;
		}
		else {
			loss_num -= 1;
		}
	}

}

//��������յ����ݣ�ת��Ϊ֡
Frame DataLinkLayer::fromPhysicalLayer()
{
	BytesFrame data;
	// һ������ʧ��,��Ϊ�ú����������ݲŻᱻ����.
	while (m_phsical_layer.receive(data) == 0)
		printf("Receive error\n");

	Frame frame(data);
	//printf("From physical: seq=%d ack=%d length=%d\n", frame.seq, frame.ack, frame.length);
	printf("--------------------����-------------------------------------------\n");
	printf("| �յ�֡: [Num=%d] [Ack=%d] [Data-Crc='%s']\n", frame.seq, frame.ack, frame.payload);

	//�յ����ݺ�У��Crc
	char data_crc[MAX_FRAME_PAYLOAD + 1];
	//NUM==nʱģ�����-1Ϊ������

	if (frame.seq == error_num) {
		printf("| ģ�����\n");
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
		//ת��Ϊstring����
		for (int i = 0; i < frame.length; ++i)
			data_crc[i] = frame.payload[i];
		data_crc[frame.length] = '\0';
	}
	std::string msg(data_crc);
	//ɾȥ'-'
	int pos = msg.find('-');
	msg = msg.substr(0, pos) + msg.substr(pos + 1, msg.length());
	//У��
	CRC crc;
	string check = crc.crc_check(msg);
	cout << "| Crc_check:" << check;
	int pos1 = check.find("1");
	if (pos1 == -1) {
		cout << "   CRCУ��ɹ�" << endl;
		return Frame(data);
	}
	else {
		cout << "   CRCУ��ʧ��" << endl;
		return Frame();
	}
}

//�ӷ��ͻ�������ȡ����ת��Ϊ֡
Frame DataLinkLayer::fromNetworkLayer()
{
	std::string data = m_cur_data.back();
	m_cur_data.pop();

	//����CRC,data = ����+CRC
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

//֡ת��Ϊ��������ݣ�������ջ�����
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

//ȡ���¼��������ĵ�һ���¼�
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

//֡����
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
	m_event_buff.push_back(frame_arrival);//push_backβ������
	ReleaseSemaphore(dl_layer->sem_not_empty, 1, NULL);
	ReleaseSemaphore(dl_layer->sem_lock, 1, NULL);
	//CloseHandle(dl_layer->sem_lock);
	//CloseHandle(dl_layer->sem_not_empty);
}

//��ʱ
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

//������·�㷢������
int DataLinkLayer::send(std::string data)
{
	//�жϻ������Ƿ�����
	if (isBuffFull)
		return -1;
	//TODO: check msg length

	m_cur_data.push(data);//���뷢�͵Ļ�����

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
	m_event_buff.push_back(network_layer_ready);//�����¼������¼�������

	if (!ReleaseSemaphore(sem_not_empty, 1, NULL))
		printf("relase sem not empty fail:%d\n", GetLastError());
	ReleaseSemaphore(sem_lock, 1, NULL);

	//CloseHandle(sem_lock);
	//CloseHandle(sem_not_empty);
	return data.length();
}

//������·���������
std::string DataLinkLayer::recv()
{
	while (m_recv_buff.empty());
	std::string msg = m_recv_buff.back();//�ӽ��ջ������õ�����
	m_recv_buff.pop();
	return msg;
}

//���߳�
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


//��Ϊ��˫��Э�飬�����з����շ��Ľ���
DWORD DataLinkLayer::mainThread()
{
	int nextFrameToSend = 0;//���ͷ���һ֡Ҫ��0��֡
	int ackExpected = 0;    //���ͷ�������0��֡�������ڴ�����Ack0���õ�ȷ��
	int frameExpected = 0;  //���շ��ڴ����յ��ǶԷ���0��֡
	Frame frame;			//��ʼ��֡
	int nbuffered = 0;		//���ʹ��ڳ�ʼ���ݸ���Ϊ0
	int i;
	EventType event;		//��ʼ���¼�

	enableNetworkLayer();//���������ã�������û��

	while (running)
	{
		event = waitForEvent();//�����¼�������
							   //printf("Event: %d\n", event);

							   //print_current_time();

		switch (event)
		{
			//���ͷ�����֡
		case network_layer_ready:
			frame = fromNetworkLayer();//�ӷ��ͻ�������ȡ����

			printf("| Frame_expected=%d, Ack_expected=%d, Next_frame_to_send=%d\n", frameExpected, ackExpected, nextFrameToSend);

			//printf("network_layer_ready: %d %d %d\n", frame.length, frame.ack, frame.seq);//��ʱack��seqΪ��ʼֵ-1
			;			m_frames_to_send[nextFrameToSend] = frame;//��Ҫ���͵�֡���뷢�ʹ���
			nbuffered += 1;//�������ݸ�����1

						   //printf("%d", nbuffered);
						   //������㷢�����ݣ�
			toPhysicalLayer(nextFrameToSend, frameExpected);
			increment(nextFrameToSend);
			break;

			//���շ�����֡	
		case frame_arrival:
			//��CRCУ��ʧ�ܴ˴����ص��ǿ�֡
			frame = fromPhysicalLayer();

			printf("| Frame_expected=%d, Ack_expected=%d\n", frameExpected, ackExpected);

			//���CRCУ��ɹ�
			if (frame.length != 0) {
				//���շ��յ�֡�����Ⱥ˶��Ƿ�ΪԤ��֡��(frameExpected)��
				//����ǵģ�����ղ�frameExpected + 1���ƶ����մ���

				if (frame.seq == frameExpected) {//��֤ͬʱ����
					toNetworkLayer(frame);//�����ݼ�����ջ�����
					increment(frameExpected);
				}

				//�ж� ack �Ƿ����ڷ��ʹ�����
				while (between(ackExpected, frame.ack, nextFrameToSend)) {
					nbuffered -= 1;
					frame.stopTimer();
					increment(ackExpected);
					//printf("%d\n", ackExpected);
				}
				printf("--------------------�������---------------------------------------\n\n");
			}
			//���CRCУ��ʧ��
			else {
				printf("--------------------����ʧ��---------------------------------------\n\n");
			}
			break;
			//��ʱ�ش�
		case timeout:
			nextFrameToSend = ackExpected;
			for (int i = 1; i <= nbuffered; ++i) {
				printf("--------------------�ش�-------------------------------------------\n");
				printf("[Timeout]: retransmit(%d)\n", nextFrameToSend);
				toPhysicalLayer(nextFrameToSend, frameExpected);
				increment(nextFrameToSend);
			}
			break;
		}
		WaitForSingleObject(sem_lock, INFINITE);
		m_event_buff.erase(m_event_buff.begin());//ɾ����һ���¼�
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

//�����ж��յ���ACK����Ƿ��ڷ��ʹ����ڡ�aΪ���ޣ�bΪack��ţ�cΪ����
bool DataLinkLayer::between(int a, int b, int c)
{
	if (((a <= b) && (b < c)) || ((c < a) && (a <= b)) || ((b < c) && (c < a)))
		return true;
	else
		return false;
}

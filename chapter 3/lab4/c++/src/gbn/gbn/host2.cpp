#include <iostream>
#include <string>
#include <bitset>

#include "DataLinkLayer.h"
#include <string>

#define LOCAL_PORT 9456
#define REMOTE_PORT 9457

using namespace std;

int main()
{
	DataLinkLayer dll(LOCAL_PORT, REMOTE_PORT, "host2");

	std::vector<std::string> recv_strs;
	std::vector<std::string> send_strs;

	int count = 0;

	while (count < 9) {
		std::string msg = "1" + bitset<8>(count).to_string();
		send_strs.push_back(msg);
		count += 1;
	}
	cout << "Host2��Host1���͵�����Ϊ: " << endl;
	for (int i = 0; i < send_strs.size(); ++i)
		cout << send_strs[i] << endl;
	printf("Press any key to continue.\n");
	getchar();

	dll.start();//�����¼�������

	count = 0;
	while (count < 9) {
		//Sleep(1000);
		//1��������2
		std::string msg = "1" + bitset<8>(count).to_string();
		cout << "--------------------����-------------------------------------------" << endl;
		while (true) {
			int status = dll.send(msg);
			if (status > 0) {
				break;
			}
			else {
				printf("send fail:%d\n", status);
			}

		}
		count++;
		recv_strs.push_back(dll.recv());

		cout << endl;

	}
	dll.stop();
	Sleep(3000);
	cout << "Host2��Host1�յ�������Ϊ: " << endl;
	for (int i = 0; i < recv_strs.size(); ++i)
		cout << recv_strs[i].substr(0,9) << endl;//ȥ��CRC

	printf("Press any key to exit.\n");
	getchar();

}
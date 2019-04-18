#include <iostream>
#include <string>
#include <bitset>

#include "DataLinkLayer.h"
#include <string>
#include <vector>

#define LOCAL_PORT 9457
#define REMOTE_PORT 9456

using namespace std;

int main()
{
	DataLinkLayer dll(LOCAL_PORT, REMOTE_PORT, "host1");
	std::vector<std::string> recv_strs;
	std::vector<std::string> send_strs;
	int count = 0;

	while (count < 9) {
		std::string msg = "0" + bitset<8>(count).to_string();
		send_strs.push_back(msg);
		count += 1;
	}
	cout << "Host1向Host2发送的内容为: " << endl;
	for (int i = 0; i < send_strs.size(); ++i)
		cout << send_strs[i] << endl;
	printf("Press any key to continue.\n");
	getchar();

	dll.start();//监听事件缓冲区

	count = 0;
	while (count < 9) {
		Sleep(600);
		//0代表主机1
		std::string msg = "0" + bitset<8>(count).to_string();
		cout << "--------------------发送-------------------------------------------" << endl;
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
	cout << "Host1从Host2收到的内容为: " << endl;
	for (int i = 0; i < recv_strs.size(); ++i)
		cout << recv_strs[i].substr(0,9) << endl;

	printf("Press any key to exit.\n");
	getchar();

}
#include <iostream>
#include "event.h"
using namespace std;

int main()
{
	start_listening();

	event_type event;
	while (true)
	{
		wait_for_event(event);
		switch (event)
		{
		case network_layer_ready:
			cout << "network_layer_ready" << endl;
			break;
		case frame_arrival:
			cout << "frame_arrival" << endl;
			break;
		case cksum_err:
			cout << "cksum_err" << endl;
			break;
		case timeout:
			cout << "time_out" << endl;
			break;
		}
	}
	getchar();
}
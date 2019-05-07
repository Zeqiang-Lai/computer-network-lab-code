#include "event.h"
event_type current_event;

HANDLE lock;
HANDLE pyhsical_layer_thread;
HANDLE network_layer_thread;

void wait_for_event(event_type &event)
{
	WaitForSingleObject(lock, INFINITE);
	event = current_event;
}

void start_listening()
{
	lock = CreateSemaphore(NULL, 1, 1, TEXT("sem_lock"));

	DWORD thread_id;
	pyhsical_layer_thread = CreateThread(NULL, 0, listening_physical, NULL, 0, &thread_id);
	network_layer_thread = CreateThread(NULL, 0, listening_network, NULL, 0, &thread_id);
}

void stop_listening()
{

}

DWORD WINAPI listening_physical(LPVOID lpParam)
{
	Sleep(2000);
	current_event = frame_arrival;
	ReleaseSemaphore(lock, 1, NULL);
	return 1;
}

DWORD WINAPI listening_network(LPVOID lpParam)
{
	Sleep(1000);
	current_event = network_layer_ready;
	ReleaseSemaphore(lock, 1, NULL);
	return 1;
}
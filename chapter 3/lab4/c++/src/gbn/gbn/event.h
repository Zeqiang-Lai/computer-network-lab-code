#pragma once
#include <Windows.h>

enum event_type { network_layer_ready, frame_arrival, cksum_err, timeout };

void wait_for_event(event_type &event);

void start_listening();

DWORD WINAPI listening_physical(LPVOID lpParam);

DWORD WINAPI listening_network(LPVOID lpParam);
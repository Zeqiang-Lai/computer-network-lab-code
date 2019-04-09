#pragma once
#include<time.h>

class Timer
{
public:
	Timer() {
		start = 0;
		end = 0;
		limit = 5000;
		timeout = false;
		flag = false;
	}
	void StartTimer() {
		flag = false;
		start = clock();
		while (!flag)
		{
			end = clock();
			if (end - start >= limit)
			{
				timeout = true;
				break;
			}
		}
	}
	void CancleTimer() {
		flag = true;
	}
	bool timeout;
	bool flag;
private:
	clock_t start;
	clock_t end;
	int limit;
};


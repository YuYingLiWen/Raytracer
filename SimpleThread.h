
#ifndef SIMPLETHREAD_H 
#define SIMPLETHREAD_H

#include <thread>
#include <mutex>
#include <queue>
#include "Color.h"

struct ThreadData;

std::queue<ThreadData*> data_queue;
std::mutex push_token;

struct ThreadData
{
	uint32_t pixel_id;
	Color final_pixel_color;
};


void Producer()
{

}

void Consumer()
{
	while (!data_queue.empty()) 
	{
		//ThreadData* data = data_queue.pop();
		
	}
}

void Push(ThreadData& new_data)
{
	data_queue.push(&new_data);
}

#endif // !SIMPLETHREAD_H 







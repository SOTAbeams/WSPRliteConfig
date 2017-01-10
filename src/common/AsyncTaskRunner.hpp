#ifndef common_AsyncTaskRunner_h
#define common_AsyncTaskRunner_h

#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>

#include "AsyncTask.hpp"

class AsyncTaskRunner
{
protected:
	std::queue< std::shared_ptr<AsyncTask> > q;
	std::mutex qMutex;
	std::condition_variable qCV;
	std::thread workThread;
	std::atomic_bool workThreadShouldStop;
	void threadStart();
	void threadStop();
	void doWork();
public:
	AsyncTaskRunner();
	virtual ~AsyncTaskRunner();
	void enqueue(std::shared_ptr<AsyncTask> newTask);
};

#endif

#include "AsyncTaskRunner.hpp"

void AsyncTaskRunner::threadStart()
{
	threadStop();

	{
		std::lock_guard<std::mutex> lk(qMutex);
		workThreadShouldStop = false;
	}
	workThread = std::thread([this](){
		doWork();
	});
}

void AsyncTaskRunner::threadStop()
{
	if (workThread.joinable())
	{
		{
			std::lock_guard<std::mutex> lk(qMutex);
			workThreadShouldStop = true;
		}
		qCV.notify_all();
		workThread.join();
	}
}

void AsyncTaskRunner::doWork()
{
	while (!workThreadShouldStop)
	{
		std::shared_ptr<AsyncTask> t;
		bool taskWaiting = false;

		{
			std::lock_guard<std::mutex> lk(qMutex);
			if (workThreadShouldStop)
			{
				break;
			}
			if (!q.empty())
			{
				t = q.front();
				q.pop();
				taskWaiting = true;
			}
		}

		if (taskWaiting)
		{
			t->runSync();
		}
		else
		{
			std::unique_lock<std::mutex> lk(qMutex);
			qCV.wait(lk, [this](){
				return (workThreadShouldStop || !q.empty());
			});
		}
	}
}

AsyncTaskRunner::AsyncTaskRunner()
{
	threadStart();
}

AsyncTaskRunner::~AsyncTaskRunner()
{
	threadStop();
}

void AsyncTaskRunner::enqueue(std::shared_ptr<AsyncTask> newTask)
{
	{
		std::lock_guard<std::mutex> lk(qMutex);
		q.push(newTask);
	}
	qCV.notify_all();
}

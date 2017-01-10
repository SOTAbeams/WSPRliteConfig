#include "AsyncTask.hpp"

void AsyncTask::runSync()
{
	try
	{
		task();
	}
	catch (const std::exception& e)
	{
		success = false;
		errorMsg = e.what();
		done = true;
		onError(e.what());
		return;
	}
	success = true;
	done = true;
	onSuccess();
}

AsyncTask::~AsyncTask() {}

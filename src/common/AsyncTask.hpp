#ifndef common_AsyncTask_h
#define common_AsyncTask_h

#include <functional>
#include <string>
#include <vector>

template<class... Args>
class ObserverSubject
{
public:
	using Func = std::function<void(Args...)>;
protected:
	std::vector<Func> callbacks;
public:
	void add(Func f)
	{
		callbacks.push_back(f);
	}
	void operator()(Args&&... args)
	{
		for (Func& f : callbacks)
		{
			f(args...);
		}
	}
};

class AsyncTask
{
public:
	bool done = false;
	bool success = false;
	std::string errorMsg;

	ObserverSubject<std::string> onError;
	ObserverSubject<float, std::string> onProgress;
	ObserverSubject<> onSuccess;
	virtual void task() = 0;
	void runSync();
	virtual ~AsyncTask();
};

#endif

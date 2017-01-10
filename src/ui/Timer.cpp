#include "Timer.hpp"

void TimerFn::Notify()
{
	if (callback)
	{
		callback();
	}
}

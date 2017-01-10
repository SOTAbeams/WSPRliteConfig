#ifndef ui_Timer_h
#define ui_Timer_h

#include <wx/timer.h>
#include <functional>

// Wrap wxTimer to use a neater (C++11) callback method
class TimerFn : public wxTimer
{
public:
	std::function<void()> callback;
	void Notify() override;
};

#endif

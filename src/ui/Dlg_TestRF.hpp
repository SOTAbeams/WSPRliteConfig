#ifndef ui_Dlg_TestRF_h
#define ui_Dlg_TestRF_h

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif
#include <wx/spinctrl.h>
#include "common/Device.hpp"
#include "common/AsyncTaskRunner.hpp"
#include "common/DeviceCommTask.hpp"

class Dlg_TestRF : public wxDialog
{
protected:
	std::shared_ptr<DeviceModel> deviceModel;

	wxSpinCtrlDouble *ctl_freq;
	wxSpinCtrl *ctl_paBias;
	AsyncTaskRunner taskRunner;
public:
	void handle_spinEvent(wxSpinEvent& event);
	void handle_spinCmdEvent(wxCommandEvent& event);
	void writeToDevice();
	Dlg_TestRF(wxWindow *parent, std::shared_ptr<DeviceModel> deviceModel_);
	virtual ~Dlg_TestRF();
	wxDECLARE_EVENT_TABLE();

	static uint64_t lastFreq;
	static uint16_t lastPaBias;
};

class Task_TestRF : public DeviceCommTask
{
public:
	using DeviceCommTask::DeviceCommTask;
	uint64_t freq;
	uint16_t paBias;
	void task() override;
	virtual ~Task_TestRF();
};


#endif

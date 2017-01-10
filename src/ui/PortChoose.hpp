#ifndef ui_PortChoose_h
#define ui_PortChoose_h

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif
#include "common/Serial.hpp"
#include "Timer.hpp"

class PortChooseList : public wxChoice
{
protected:
	SerialPortsList portsList;
	void updateListItems();
	std::vector<wxString> getNewPortsText();
public:
	std::vector<wxString> portsText;
	std::string preferredDesc;
	PortChooseList(wxWindow *parent);
	void UpdatePorts();
	virtual ~PortChooseList();
	SerialPort GetChosenPort();
};

class PortChooseDlg : public wxDialog
{
protected:
	TimerFn timer;
	PortChooseList *plist;
public:
	PortChooseDlg(wxWindow *parent);
	virtual ~PortChooseDlg();
	SerialPort GetChosenPort();
};


#endif

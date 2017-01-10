#include "PortChoose.hpp"
#include <sstream>

PortChooseList::PortChooseList(wxWindow *parent) :
	wxChoice(parent, wxID_ANY),
	portsList()
{
}

PortChooseList::~PortChooseList()
{}

SerialPort PortChooseList::GetChosenPort()
{
	if (GetSelection()==wxNOT_FOUND)
		throw std::logic_error("No port selected");
	if (GetSelection()<0 || GetSelection()>=portsList.ports.size())
		throw std::logic_error("Invalid port selections");
	return portsList.ports[GetSelection()];
}

void PortChooseList::updateListItems()
{
	portsText = getNewPortsText();

	if (portsText.size())
	{
		Set(portsText.size(), portsText.data());
	}
	else
	{
		SetSelection(wxNOT_FOUND);
		Clear();
	}
}

std::vector<wxString> PortChooseList::getNewPortsText()
{
	size_t n = portsList.ports.size();
	std::vector<wxString> items;
	items.reserve(n);

	for (SerialPort const &p: portsList.ports)
	{
		std::stringstream ss;
		ss << p.GetName() << " (" << p.GetDesc() << ")";
		items.push_back(ss.str());
	}
	return items;
}

void PortChooseList::UpdatePorts()
{
	std::string selectedItem = "";
	// Get currently selected port name+desc
	// Need to check that something is selected first, otherwise GetString prints "IA__gtk_tree_model_iter_nth_child: assertion 'n >= 0' failed" to stderr
	if (GetSelection()!=wxNOT_FOUND)
	{
		selectedItem = GetString(GetSelection());
	}

	// Update list of ports
	portsList.Update();
	std::vector<wxString> newPortsText = getNewPortsText();

	// Check whether any of the text in the dropdown has changed
	bool changed = false;
	if (newPortsText.size()!=portsText.size())
		changed = true;
	else
	{
		for (size_t i=0; i<newPortsText.size(); i++)
		{
			if (newPortsText[i]!=portsText[i])
			{
				changed = true;
				break;
			}
		}
	}
	// If text has not changed, don't SetSelection, otherwise it makes it difficult to select an item on Windows
	if (!changed)
		return;

	updateListItems();

	// Select the previously selected item if it's still there
	if (portsList.ports.size() && selectedItem!="")
	{
		SetStringSelection(selectedItem);
	}
	else
	{
		SetSelection(wxNOT_FOUND);
	}

	if (GetSelection()==wxNOT_FOUND && preferredDesc!="")
	{
		for (size_t i=0; i<newPortsText.size(); i++)
		{
			if (newPortsText[i].find(preferredDesc)!=std::string::npos)
			{
				SetSelection(i);
				break;
			}
		}
	}
}




PortChooseDlg::PortChooseDlg(wxWindow *parent)
	: wxDialog(parent, wxID_ANY, "Choose serial port")
{
	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

	std::string message = "Select serial port to use:";
	sizer->Add(CreateTextSizer(message), wxSizerFlags().Expand().TripleBorder());

	plist = new PortChooseList(this);
	plist->SetMinClientSize(wxSize(300,-1));
	sizer->Add(plist, wxSizerFlags().Expand().Proportion(0).TripleBorder(wxLEFT | wxRIGHT));
	sizer->AddStretchSpacer(1);
	sizer->Add(CreateSeparatedButtonSizer(wxOK | wxCANCEL), wxSizerFlags().Expand().DoubleBorder());

	SetSizerAndFit(sizer);
	plist->SetFocus();

	timer.callback = [this](){
		plist->UpdatePorts();
	};
	timer.Start(2000);
}

PortChooseDlg::~PortChooseDlg()
{}

SerialPort PortChooseDlg::GetChosenPort()
{
	return plist->GetChosenPort();
}


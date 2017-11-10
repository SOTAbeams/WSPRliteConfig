#include "Ctl_BiasSelect.hpp"
#include "common/StrUtil.hpp"

Ctl_BiasSelect::Ctl_BiasSelect(wxWindow *parent, wxWindowID id, std::shared_ptr<DeviceModel> deviceModel_) :
wxComboBox(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_READONLY | wxCB_DROPDOWN),
  deviceModel(deviceModel_)
{
}

void Ctl_BiasSelect::initChoices()
{
	choices.clear();
	if (deviceModel->info.deviceVersion.has_20mFilter())
	{
		// No filter bias data currently available. When it has been measured, it will default to LPF kit for bands below 14 MHz and direct for 14 MHz.
		choices.push_back(PaBiasSource::Default);
	}
	else
	{
		if (deviceModel->info.firmwareVersion.supports_varId(DeviceComm::VarId::PaBiasSource))
		{
			choices.push_back(PaBiasSource::Direct);
			choices.push_back(PaBiasSource::SbLpfKit);
		}
		else
		{
			choices.push_back(PaBiasSource::SbLpfKit);
		}
	}
	Clear();
	for (PaBiasSource &s : choices)
	{
		wxString txt = _("Unknown");
		if (deviceModel->info.deviceVersion.has_20mFilter())
		{
			txt = _("None");
		}
		else
		{
			if (s==PaBiasSource::Direct)
				txt = _("None");
			else if (s==PaBiasSource::SbLpfKit)
				txt = _("SOTABEAMS LPF kit");
		}
		Append(txt, (void*)&s);
	}
}

void Ctl_BiasSelect::set(PaBiasSource newChoice)
{
	if (deviceModel->info.deviceVersion.has_20mFilter())
	{
		// Classic
		newChoice = PaBiasSource::Default;
	}
	else
	{
		// Flexi - always needs a LPF attached, so default to SOTABEAMS LPF kit
		if (newChoice==PaBiasSource::Default)
		{
			newChoice = PaBiasSource::SbLpfKit;
		}
	}

	for (size_t i=0; i<choices.size(); i++)
	{
		if (choices[i]==newChoice)
		{
			SetSelection(i);
			return;
		}
	}
	SetSelection(wxNOT_FOUND);
}

PaBiasSource Ctl_BiasSelect::get()
{
	int i = GetSelection();
	if (i==wxNOT_FOUND)
	{
		if (choices.size())
			return choices[0];
		else
			return PaBiasSource::Default;
	}
	return *(PaBiasSource*)GetClientData(i);
}

#include "Ctl_OutputPowerSelect.hpp"
#include "common/StrUtil.hpp"

Ctl_OutputPowerSelect::Ctl_OutputPowerSelect(wxWindow *parent, wxWindowID id) :
wxComboBox(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_READONLY | wxCB_DROPDOWN)
{
	initChoices(7, 23);
}

void Ctl_OutputPowerSelect::initChoices(double min_dBm, double max_dBm)
{
	int8_t currentPowerLevel = getdBm();
	int8_t allLevels[] = {7,10,13,17,20,23};
	Clear();
	powerLevels.clear();
	for (int8_t p: allLevels)
	{
		if (p<min_dBm || p>max_dBm)
			continue;
		powerLevels.push_back(p);
		Append(StrUtil::power_dBmToRoundedString(p));
	}
	setdBm(currentPowerLevel);
	if (GetSelection()==wxNOT_FOUND && powerLevels.size()>0)
		SetSelection(powerLevels.size()-1);
}

void Ctl_OutputPowerSelect::setdBm(int8_t dBm)
{
	for (size_t i=0; i<powerLevels.size(); i++)
	{
		if (powerLevels[i] == dBm)
		{
			SetSelection(i);
			return;
		}
	}
	SetSelection(wxNOT_FOUND);
}

int8_t Ctl_OutputPowerSelect::getdBm()
{
	int i = GetSelection();
	if (i==wxNOT_FOUND)
	{
		if (powerLevels.size()>0)
			return powerLevels[powerLevels.size()-1];
		else
			return 23;
	}
	return powerLevels[i];
}


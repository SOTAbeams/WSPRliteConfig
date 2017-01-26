#include "Ctl_OutputPowerSelect.hpp"
#include "common/StrUtil.hpp"
#include "common/PaBias.hpp"

Ctl_OutputPowerSelect::Ctl_OutputPowerSelect(wxWindow *parent, wxWindowID id) :
wxComboBox(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_READONLY | wxCB_DROPDOWN)
{
	powerLevels.push_back(7);
	powerLevels.push_back(10);
	powerLevels.push_back(13);
	powerLevels.push_back(17);
	powerLevels.push_back(20);
	powerLevels.push_back(23);
	for (int8_t p: powerLevels)
	{
		Append(StrUtil::power_dBmToRoundedString(p));
	}
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
		return powerLevels[powerLevels.size()-1];
	return powerLevels[i];
}

uint16_t Ctl_OutputPowerSelect::getpaBias(uint64_t freq)
{
	return PaBias::get(freq, getdBm());
}


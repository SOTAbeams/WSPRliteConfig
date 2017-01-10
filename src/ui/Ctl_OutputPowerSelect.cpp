#include "Ctl_OutputPowerSelect.hpp"
#include "common/StrUtil.hpp"

Ctl_OutputPowerSelect::Ctl_OutputPowerSelect(wxWindow *parent, wxWindowID id) :
wxComboBox(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_READONLY | wxCB_DROPDOWN)
{
	powerLevels.push_back(PowerLevel(7, 30, 29, 32));
	powerLevels.push_back(PowerLevel(10, 58, 57, 59));
	powerLevels.push_back(PowerLevel(13, 92, 90, 92));
	powerLevels.push_back(PowerLevel(17, 157, 148, 150));
	powerLevels.push_back(PowerLevel(20, 242, 214, 218));
	powerLevels.push_back(PowerLevel(23, 411, 313, 325));
	for (PowerLevel &p: powerLevels)
	{
		Append(p.txt, (void*)&p);
	}
}

void Ctl_OutputPowerSelect::setdBm(int8_t dBm)
{
	for (size_t i=0; i<powerLevels.size(); i++)
	{
		if (powerLevels[i].dBm == dBm)
		{
			SetSelection(i);
			return;
		}
	}
	SetSelection(wxNOT_FOUND);
}

Ctl_OutputPowerSelect::PowerLevel* Ctl_OutputPowerSelect::getSelectedPowerInfo()
{
	int i = GetSelection();
	if (i==wxNOT_FOUND)
		return &powerLevels[powerLevels.size()-1];
	return (PowerLevel*)GetClientData(i);
}

int8_t Ctl_OutputPowerSelect::getdBm()
{
	return getSelectedPowerInfo()->dBm;
}

uint16_t Ctl_OutputPowerSelect::getpaBias(WsprBand bandId)
{
	if (bandId==WsprBand::Band_20m)
		return getSelectedPowerInfo()->paBias_14MHz;
	if (bandId==WsprBand::Band_30m)
		return getSelectedPowerInfo()->paBias_10MHz;
	return getSelectedPowerInfo()->paBias_other;
}


Ctl_OutputPowerSelect::PowerLevel::PowerLevel(int8_t dBm_, uint16_t paBias_14MHz_, uint16_t paBias_10MHz_, uint16_t paBias_other_) :
dBm(dBm_), paBias_14MHz(paBias_14MHz_), paBias_10MHz(paBias_10MHz_), paBias_other(paBias_other_)
{
	txt = StrUtil::power_dBmToRoundedString(dBm);
}

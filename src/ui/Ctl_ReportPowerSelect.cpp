#include "Ctl_ReportPowerSelect.hpp"
#include "common/StrUtil.hpp"

Ctl_ReportPowerSelect::Ctl_ReportPowerSelect(wxWindow *parent, wxWindowID id) :
wxComboBox(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_READONLY | wxCB_DROPDOWN)
{
	powerLevels.push_back(PowerLevel(-1, _("WSPRlite output power (no external amplifier)")));

	for (int8_t dBm_ten=0; dBm_ten<6; dBm_ten++)
	{
		for (auto dBm_one : {0,3,7})
		{
			powerLevels.push_back(PowerLevel(dBm_ten*10+dBm_one));
		}
	}
	for (PowerLevel &p: powerLevels)
	{
		Append(p.txt, (void*)&p);
	}
}

void Ctl_ReportPowerSelect::setdBm(int8_t dBm)
{
	for (size_t i=0; i<powerLevels.size(); i++)
	{
		if (powerLevels[i].dBm == dBm)
		{
			SetSelection(i);
			return;
		}
	}
	SetSelection(0);
}

Ctl_ReportPowerSelect::PowerLevel* Ctl_ReportPowerSelect::getSelectedPowerInfo()
{
	int i = GetSelection();
	if (i==wxNOT_FOUND)
		return &powerLevels[0];
	return (PowerLevel*)GetClientData(i);
}

int8_t Ctl_ReportPowerSelect::getdBm()
{
	return getSelectedPowerInfo()->dBm;
}

Ctl_ReportPowerSelect::PowerLevel::PowerLevel(int8_t dBm_) :
dBm(dBm_)
{
	txt = StrUtil::power_dBmToRoundedString(dBm);
}

Ctl_ReportPowerSelect::PowerLevel::PowerLevel(int8_t dBm_, wxString txt_) :
dBm(dBm_), txt(txt_)
{}

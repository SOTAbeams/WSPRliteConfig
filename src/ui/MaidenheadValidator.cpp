#include "MaidenheadValidator.hpp"

#include <cctype>

MaidenheadValidator::MaidenheadValidator() :
wxTextValidator(wxFILTER_INCLUDE_CHAR_LIST)
{
	SetCharIncludes("ABCDEFGHIJKLMNOPQR0123456789abcdefghijklmnopqr");//stuvwx
}

MaidenheadValidator::MaidenheadValidator(const MaidenheadValidator &val)
{
	wxTextValidator::Copy(val);
}

bool MaidenheadValidator::Validate(wxWindow *parent)
{
	if (!wxTextValidator::Validate(parent))
		return false;
	
	wxString loc = GetTextEntry()->GetValue().Upper();
	
	/*if (loc.Length()!=4 && loc.Length()!=6)
	{
		wxMessageBox(_("Locator should be 4 or 6 characters long"), _("Validation error"), wxICON_EXCLAMATION | wxOK, parent);
		return false;
	}*/
	
	if (loc.Length()!=4)
	{
		wxMessageBox(_("Locator should be 4 characters long"), _("Validation error"), wxICON_EXCLAMATION | wxOK, parent);
		return false;
	}
	
	for (int i=0; i<2; i++)
	{
		if (loc[i]<'A' || loc[i]>'R')
		{
			wxMessageBox(_("First 2 characters of locator should be uppercase, and be between A and R"), _("Validation error"), wxICON_EXCLAMATION | wxOK, parent);
			return false;
		}
	}
	for (int i=2; i<4; i++)
	{
		if (!std::isdigit(loc[i]))
		{
			wxMessageBox(_("Second 2 characters of locator should be digits"), _("Validation error"), wxICON_EXCLAMATION | wxOK, parent);
			return false;
		}
	}
	return true;
}

wxObject *MaidenheadValidator::Clone() const
{
	return new MaidenheadValidator(*this);
}

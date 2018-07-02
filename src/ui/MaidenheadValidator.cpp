#include "MaidenheadValidator.hpp"

#include <cctype>

MaidenheadValidator::MaidenheadValidator() :
wxTextValidator(wxFILTER_INCLUDE_CHAR_LIST)
{
	SetCharIncludes("ABCDEFGHIJKLMNOPQRSTUVWX0123456789abcdefghijklmnopqrstuvwx");
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
	
	if (loc.Length()!=4 && loc.Length()!=6)
	{
		wxMessageBox(_("Locator should be 4 or 6 characters long"), _("Validation error"), wxICON_EXCLAMATION | wxOK, parent);
		return false;
	}

	for (int i=0; i<2; i++)
	{
		if (loc[i]<'A' || loc[i]>'R')
		{
			wxMessageBox(_("1st and 2nd characters of locator should be between A and R"), _("Validation error"), wxICON_EXCLAMATION | wxOK, parent);
			return false;
		}
	}
	for (int i=2; i<4; i++)
	{
		if (!std::isdigit(loc[i]))
		{
			wxMessageBox(_("3rd and 4th characters of locator should be digits"), _("Validation error"), wxICON_EXCLAMATION | wxOK, parent);
			return false;
		}
	}
	if (loc.Length()==4)
	{
		return true;
	}

	for (int i=4; i<6; i++)
	{
		if (loc[i]<'A' || loc[i]>'X')
		{
			wxMessageBox(_("5th and 6th characters of locator should be between a and x"), _("Validation error"), wxICON_EXCLAMATION | wxOK, parent);
			return false;
		}
	}
	return true;
}

wxObject *MaidenheadValidator::Clone() const
{
	return new MaidenheadValidator(*this);
}

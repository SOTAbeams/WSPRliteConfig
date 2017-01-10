#ifndef ui_MaidenheadValidator_h
#define ui_MaidenheadValidator_h

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

class MaidenheadValidator : public wxTextValidator
{
public:
    MaidenheadValidator();
    MaidenheadValidator(const MaidenheadValidator& val);
    bool Validate(wxWindow *parent) override;
    virtual wxObject *Clone() const override;

};


#endif

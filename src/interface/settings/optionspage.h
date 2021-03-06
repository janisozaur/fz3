#ifndef FILEZILLA_INTERFACE_OPTIONSPAGE_HEADER
#define FILEZILLA_INTERFACE_OPTIONSPAGE_HEADER

#include <wx/panel.h>

#include "../Options.h"

class CSettingsDialog;
class COptionsPage : public wxPanel
{
public:
	virtual bool CreatePage(COptions* pOptions, CSettingsDialog* pOwner, wxWindow* parent, wxSize& maxSize);

	void UpdateMaxPageSize(wxSize& maxSize);

	virtual bool LoadPage() = 0;
	virtual bool SavePage() = 0;
	virtual bool Validate() { return true; }

	void SetCheck(int id, bool checked, bool& failure);
	void SetCheckFromOption(int control_id, interfaceOptions option_id, bool& failure);
	void SetRCheck(int id, bool checked, bool& failure);
	void SetTextFromOption(int ctrlId, interfaceOptions optionId, bool& failure);
	void SetStaticText(int id, wxString const& text, bool& failure);
	void SetChoice(int id, int selection, bool& failure);
	bool SetText(int id, std::wstring const& text, bool& failure);

	// The GetXXX functions do never return an error since the controls were
	// checked to exist while loading the dialog.
	bool GetCheck(int id) const;
	bool GetRCheck(int id) const;
	std::wstring GetText(int id) const;
	int GetChoice(int id) const;

	void SetOptionFromText(int ctrlId, interfaceOptions optionId);
	void SetIntOptionFromText(int ctrlId, interfaceOptions optionId); // There's no corresponding GetTextFromIntOption as COptions::GetOption is smart enough to convert
	void SetOptionFromCheck(int control_id, interfaceOptions option_id);

	void ReloadSettings();

	// Always returns false
	bool DisplayError(wxString const& controlToFocus, wxString const& error);
	bool DisplayError(wxWindow* pWnd, wxString const& error);

	bool Display();

	virtual bool OnDisplayedFirstTime();

protected:
	virtual wxString GetResourceName() const {
		return wxString();
	}

	virtual bool CreateControls(wxWindow* parent);

	COptions* m_pOptions{};
	CSettingsDialog* m_pOwner{};

	bool m_was_selected{};
};

#endif

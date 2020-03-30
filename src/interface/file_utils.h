#ifndef FILEZILLA_FILE_UTILS_HEADER
#define FILEZILLA_FILE_UTILS_HEADER

#include <libfilezilla/recursive_remove.hpp>

// Quotation rules:
// - Args containing spaces double-quotes need to be quotes by enclosing in double-quotes.
// - If an arg is quoted, contained double-quotes are doubled
//
// - Example: "foo""bar" is the quoted representation of foo"bar
std::wstring QuoteCommand(std::vector<std::wstring> const& cmd_with_args);
std::vector<std::wstring> UnquoteCommand(std::wstring_view const& command);

// Returns the association for a file based on its extension
std::vector<std::wstring> GetSystemAssociation(std::wstring const& file);

void AssociationToCommand(std::vector<std::wstring> & association, std::wstring_view const& file);

bool ProgramExists(std::wstring const& editor); // TODO: Still needed?
bool PathExpand(std::wstring & cmd); // TODO: Still needed?

// Returns a file:// URL
std::wstring GetAsURL(std::wstring const& dir);

// Opens specified directory in local file manager, e.g. Explorer on Windows
bool OpenInFileManager(std::wstring const& dir);

bool RenameFile(wxWindow* pWnd, wxString dir, wxString from, wxString to);

CLocalPath GetDownloadDir();

std::wstring GetExtension(std::wstring_view file);

class gui_recursive_remove final : public fz::recursive_remove
{
public:
	gui_recursive_remove(wxWindow* parent)
		: parent_(parent)
	{}

#ifdef FZ_WINDOWS
	virtual void adjust_shfileop(SHFILEOPSTRUCT & op)
	{
		op.hwnd = parent_ ? (HWND)parent_->GetHandle() : 0;
		if (parent_) {
			// Move to trash if shift is not pressed, else delete
			op.fFlags = wxGetKeyState(WXK_SHIFT) ? 0 : FOF_ALLOWUNDO;
		}
		else {
			recursive_remove::adjust_shfileop(op);
		}
	}
#endif

	virtual bool confirm() const {
		if (parent_) {
			if (wxMessageBoxEx(_("Really delete all selected files and/or directories from your computer?"), _("Confirmation needed"), wxICON_QUESTION | wxYES_NO, parent_) != wxYES)
				return false;
		}
		return true;
	}

private:
	wxWindow* parent_;
};

bool IsInvalidChar(wchar_t c, bool includeQuotes = false);

#endif

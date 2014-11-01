#include <filezilla.h>
#include "drop_target_ex.h"

#include "listctrlex.h"
#include "treectrlex.h"

template<class Control>
CScrollableDropTarget<Control>::CScrollableDropTarget(Control* pCtrl)
	: m_pCtrl(pCtrl)
	, m_count()
{
	m_timer.SetOwner(this);
}


template<class Control>
bool CScrollableDropTarget<Control>::OnDrop(wxCoord, wxCoord)
{
	m_timer.Stop();
	return true;
}

template<class Control>
wxDragResult CScrollableDropTarget<Control>::OnDragOver(wxCoord x, wxCoord y, wxDragResult def)
{
	def = FixupDragResult(def);
	if (!m_timer.IsRunning() && IsScroll(wxPoint(x, y))) {
		m_timer.Start(100, true);
		m_count = 0;
	}
	return def;
}

template<class Control>
void CScrollableDropTarget<Control>::OnLeave()
{
	m_timer.Stop();
}

template<class Control>
wxDragResult CScrollableDropTarget<Control>::OnEnter(wxCoord x, wxCoord y, wxDragResult def)
{
	def = FixupDragResult(def);
	if (!m_timer.IsRunning() && IsScroll(wxPoint(x, y))) {
		m_timer.Start(100, true);
		m_count = 0;
	}
	return def;
}

template<class Control>
bool CScrollableDropTarget<Control>::IsScroll(wxPoint p) const
{
	return IsTopScroll(p) || IsBottomScroll(p);
}

template<class Control>
bool CScrollableDropTarget<Control>::IsTopScroll(wxPoint p) const
{
	if (!m_pCtrl->GetItemCount()) {
		return false;
	}

	wxRect itemRect;
	if (!m_pCtrl->GetItemRect(m_pCtrl->GetTopItem(), itemRect))
		return false;

	wxRect windowRect = m_pCtrl->GetActualClientRect();

	if (itemRect.GetHeight() > windowRect.GetHeight() / 4) {
		itemRect.SetHeight(wxMax(windowRect.GetHeight() / 4, 8));
	}

	if (p.y < 0 || p.y >= itemRect.GetBottom())
		return false;

	if (p.x < 0 || p.x > windowRect.GetWidth())
		return false;

	auto top = m_pCtrl->GetTopItem();
	if (top <= 0) {
		return false;
	}

	return true;
}

template<class Control>
bool CScrollableDropTarget<Control>::IsBottomScroll(wxPoint p) const
{
	if (!m_pCtrl->GetItemCount()) {
		return false;
	}

	wxRect itemRect;
	if (!m_pCtrl->GetItemRect(m_pCtrl->GetFirstItem(), itemRect))
		return false;

	wxRect const windowRect = m_pCtrl->GetActualClientRect();

	int scrollHeight = itemRect.GetHeight();
	if (scrollHeight > windowRect.GetHeight() / 4) {
		scrollHeight = wxMax(windowRect.GetHeight() / 4, 8);
	}

	if (p.y > windowRect.GetBottom() || p.y < windowRect.GetBottom() - scrollHeight)
		return false;

	if (p.x < 0 || p.x > windowRect.GetWidth())
		return false;

	auto bottom = m_pCtrl->GetBottomItem();
	if (!m_pCtrl->Valid(bottom) || bottom == m_pCtrl->GetLastItem()) {
		return false;
	}

	return true;
}

template<class Control>
void CScrollableDropTarget<Control>::OnTimer(wxTimerEvent& /*event*/)
{
	if (!m_pCtrl->GetItemCount()) {
		return;
	}

	wxPoint p = wxGetMousePosition();
	wxWindow* ctrl = m_pCtrl->GetMainWindow();
	p = ctrl->ScreenToClient(p);

	if (IsTopScroll(p)) {
		auto top = m_pCtrl->GetTopItem();
		m_pCtrl->EnsureVisible(m_pCtrl->GetPrevItemSimple(top));
	}
	else if (IsBottomScroll(p)) {
		auto bottom = m_pCtrl->GetBottomItem();
		m_pCtrl->EnsureVisible(m_pCtrl->GetNextItemSimple(bottom));
	}
	else {
		return;
	}

	DisplayDropHighlight(p);

	if (m_count < 90)
		++m_count;
	m_timer.Start(100 - m_count, true);
}

template<class Control>
wxDragResult CScrollableDropTarget<Control>::FixupDragResult(wxDragResult res)
{
#ifdef __WXMAC__
	if (res == wxDragNone && wxGetKeyState(WXK_CONTROL)) {
		res = wxDragCopy;
	}
#endif

	if (res == wxDragLink) {
		res = wxGetKeyState(WXK_CONTROL) ? wxDragCopy : wxDragMove;
	}

	return res;
}


template class CScrollableDropTarget<wxTreeCtrlEx>;
template class CScrollableDropTarget<wxListCtrlEx>;

BEGIN_EVENT_TABLE(CScrollableDropTarget<wxTreeCtrlEx>, wxEvtHandler)
EVT_TIMER(wxID_ANY, CScrollableDropTarget<wxTreeCtrlEx>::OnTimer)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(CScrollableDropTarget<wxListCtrlEx>, wxEvtHandler)
EVT_TIMER(wxID_ANY, CScrollableDropTarget<wxListCtrlEx>::OnTimer)
END_EVENT_TABLE()

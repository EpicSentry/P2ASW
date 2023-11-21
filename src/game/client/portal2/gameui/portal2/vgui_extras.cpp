// ==== VGUI Extras - theKlaxon ====
// This file contains any extra vgui 
// functionalities that may be needed
// for the new portal2 gameui
// ===================================

#include "cbase.h"
#include "vgui_controls/SectionedListPanel.h"
#include "vgui_controls/Label.h"

//-----------------------------------------------------------------------------
// Purpose: gets the local coordinates of a cell
//-----------------------------------------------------------------------------
bool vgui::SectionedListPanel::GetItemBounds(int itemID, int& x, int& y, int& wide, int& tall)
{
	x = y = wide = tall = 0;
	if (!IsItemIDValid(itemID))
		return false;

	// get the item
#if 0
	CItemButton* item = m_Items[itemID];
#else
	Label* item = (Label*)(m_Items[itemID]);
#endif
	if (!item->IsVisible())
		return false;

	//!! ignores column for now
	item->GetBounds(x, y, wide, tall);
	return true;
}
//========= Copyright (c) 1996-2008, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=======================================================================================//

#ifndef __VADVANCEDVIDEO_H__
#define __VADVANCEDVIDEO_H__

#include "basemodui.h"
#include "VFlyoutMenu.h"
#include "OptionsSubVideo.h"

#define MAX_DYNAMIC_AA_MODES 10

namespace BaseModUI {

class DropDownMenu;
class SliderControl;

struct AAMode_t
{
	int m_nNumSamples;
	int m_nQualityLevel;
};

class CAdvancedVideo : public CBaseModFrame
{
	DECLARE_CLASS_SIMPLE( CAdvancedVideo, CBaseModFrame );

public:
	CAdvancedVideo(vgui::Panel *parent, const char *panelName);
	~CAdvancedVideo();

	void SetDefaults();
	void DiscardChangesAndClose();

protected:
	virtual void	Activate();
	virtual void	ApplySchemeSettings( vgui::IScheme* pScheme );
	virtual void	OnKeyCodePressed( vgui::KeyCode code );
	virtual void	OnCommand( const char *pCommand );
	virtual void	OnThink();

	MESSAGE_FUNC_HANDLE( OnHybridButtonNavigatedTo, "OnHybridButtonNavigatedTo", button );

private:
	void	GetCurrentSettings( void );
	bool	GetRecommendedSettings( void );
	void	UpdateFooter();
	void	SetupState( bool bRecommendedSettings );
	void	ProcessAAList();
	int		FindMSAAMode( int nAASamples, int nAAQuality );
	void	ApplyChanges();
	void	SetPagedPoolState();
	void	SetAntiAliasingState();
	void	SetFilteringState();
	void	SetVSyncState();
	void	SetQueuedModeState();
	void	SetShaderDetailState();
	void	SetCPUDetailState();
	void	SetModelDetailState();

private:
	int					m_nNumAAModes;
	AAMode_t			m_nAAModes[ MAX_DYNAMIC_AA_MODES ];

	BaseModHybridButton		*m_drpModelDetail;
	BaseModHybridButton		*m_drpPagedPoolMem;
	BaseModHybridButton		*m_drpAntialias;
	BaseModHybridButton		*m_drpFiltering;
	BaseModHybridButton		*m_drpVSync;
	BaseModHybridButton		*m_drpQueuedMode;
	BaseModHybridButton		*m_drpShaderDetail;
	BaseModHybridButton		*m_drpCPUDetail;

	vgui::Label* m_lblDescriptionTitle;
	vgui::Label* m_lblDescription;

	bool	m_bDirtyValues;
	bool	m_bEnableApply;
	int		m_iModelTextureDetail;
	int		m_iPagedPoolMem;
	int		m_iAntiAlias;
	int		m_iFiltering;
	int		m_nAASamples;
	int		m_nAAQuality;
	bool	m_bVSync;
	bool	m_bTripleBuffered;
	int		m_iQueuedMode;
	int		m_iGPUDetail;
	int		m_iCPUDetail;
};

};

#endif // __VADVANCEDVIDEO_H__

//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef C_PROP_PORTAL_H
#define C_PROP_PORTAL_H

#ifdef _WIN32
#pragma once
#endif

#include "portalrenderable_flatbasic.h"
#include "iviewrender.h"
#include "view_shared.h"
#include "viewrender.h"
#include "PortalSimulation.h"
#include "C_PortalGhostRenderable.h" 
#include "portal_shareddefs.h"

struct dlight_t;
class C_DynamicLight;

class C_Prop_Portal : public CPortalRenderable_FlatBasic, public CPortalSimulatorEventCallbacks
{
public:
	DECLARE_CLASS( C_Prop_Portal, CPortalRenderable_FlatBasic );
	DECLARE_CLIENTCLASS();
	DECLARE_PREDICTABLE();

							C_Prop_Portal( void );
	virtual					~C_Prop_Portal( void );


	// Handle recording for the SFM
	virtual void GetToolRecordingState( KeyValues *msg );

	CHandle<C_Prop_Portal>	m_hLinkedPortal; //the portal this portal is linked to
	bool					m_bActivated; //a portal can exist and not be active
	
	bool					m_bSharedEnvironmentConfiguration; //this will be set by an instance of CPortal_Environment when two environments are in close proximity
	
	cplane_t				m_plane_Origin;	// The plane on which this portal is placed, normal facing outward (matching model forward vec)

	virtual void			Spawn( void );
	virtual void			Activate( void );
	virtual void			ClientThink( void );

	virtual bool			Simulate();

	virtual void			UpdateOnRemove( void );

	virtual void			OnNewParticleEffect( const char *pszParticleName, CNewParticleEffect *pNewParticleEffect );

	struct Portal_PreDataChanged
	{
		bool					m_bActivated;
		bool					m_bIsPortal2;
		Vector					m_vOrigin;
		QAngle					m_qAngles;
		EHANDLE					m_hLinkedTo;
	} PreDataChanged;

	struct TransformedLightingData_t
	{
		ClientShadowHandle_t	m_LightShadowHandle;
		dlight_t				*m_pEntityLight;
	} TransformedLighting;

	virtual void			OnPreDataChanged( DataUpdateType_t updateType );
	virtual void			OnDataChanged( DataUpdateType_t updateType );
	virtual int				DrawModel( int flags, const RenderableInstance_t& instance );
	void					UpdateOriginPlane( void );
	void					UpdateGhostRenderables( void );

	void					SetIsPortal2( bool bValue );

	bool					IsActivedAndLinked( void ) const;
	
	bool					IsFloorPortal( float fThreshold = 0.8f ) const;
	bool					IsCeilingPortal( float fThreshold = -0.8f ) const;

	virtual void			PreTeleportTouchingEntity( CBaseEntity *pOther ) {};
	virtual void			PostTeleportTouchingEntity( CBaseEntity *pOther ) {};

	virtual void			PortalSimulator_TookOwnershipOfEntity( CBaseEntity *pEntity );
	virtual void			PortalSimulator_ReleasedOwnershipOfEntity( CBaseEntity *pEntity );

	CPortalSimulator		m_PortalSimulator;

	virtual C_BaseEntity *	PortalRenderable_GetPairedEntity( void ) { return this; };
	
	void					DoFizzleEffect( int iEffect, bool bDelayedPos = true ); //display cool visual effect	
	void					CreateFizzleEffect( C_BaseEntity *pOwner, int iEffect, Vector vecOrigin, QAngle qAngles, int nTeam, int nPortalNum );
	
	CProp_Portal			*GetLinkedPortal( void ) { return m_hLinkedPortal; }
	
	float	GetHalfWidth( void ) { return PORTAL_HALF_WIDTH; }
	float	GetHalfHeight( void ) { return PORTAL_HALF_HEIGHT; }
	
	bool	IsActive( void )	{ return m_bActivated; }
	
	//FIXME:
	//{
		bool IsMobile( void ) { return false; }
	//}
		
	//it shouldn't matter, but the convention should be that we query the exit portal for these values
	virtual float			GetMinimumExitSpeed( bool bPlayer, bool bEntranceOnFloor, bool bExitOnFloor, const Vector &vEntityCenterAtExit, CBaseEntity *pEntity ); //return -FLT_MAX for no minimum
	virtual float			GetMaximumExitSpeed( bool bPlayer, bool bEntranceOnFloor, bool bExitOnFloor, const Vector &vEntityCenterAtExit, CBaseEntity *pEntity ); //return FLT_MAX for no maximum

	//does all the gruntwork of figuring out flooriness and calling the two above
	static void				GetExitSpeedRange( CProp_Portal *pEntrancePortal, bool bPlayer, float &fExitMinimum, float &fExitMaximum, const Vector &vEntityCenterAtExit, CBaseEntity *pEntity );


private:

	CUtlVector<EHANDLE>		m_hGhostingEntities;
	CUtlVector<C_PortalGhostRenderable *>		m_GhostRenderables;
	float					m_fGhostRenderablesClip[4];


	friend void __MsgFunc_EntityPortalled(bf_read &msg);

};

typedef C_Prop_Portal CProp_Portal;

#endif //#ifndef C_PROP_PORTAL_H

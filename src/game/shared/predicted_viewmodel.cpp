//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//
#include "cbase.h"
#include "predicted_viewmodel.h"

#ifdef CLIENT_DLL
#include "prediction.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

IMPLEMENT_NETWORKCLASS_ALIASED( PredictedViewModel, DT_PredictedViewModel )
LINK_ENTITY_TO_CLASS_ALIASED( predicted_viewmodel, PredictedViewModel );

BEGIN_NETWORK_TABLE( CPredictedViewModel, DT_PredictedViewModel )
END_NETWORK_TABLE()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
#ifdef CLIENT_DLL
CPredictedViewModel::CPredictedViewModel() : m_LagAnglesHistory("CPredictedViewModel::m_LagAnglesHistory")
{
	m_vLagAngles.Init();
	m_LagAnglesHistory.Setup( &m_vLagAngles, INTERPOLATE_LINEAR_ONLY );
	m_vPredictedOffset.Init();
}
#else
CPredictedViewModel::CPredictedViewModel()
{
}
#endif


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CPredictedViewModel::~CPredictedViewModel()
{
}

#ifdef CLIENT_DLL
ConVar cl_wpn_sway_interp( "cl_wpn_sway_interp", "0.1", FCVAR_CLIENTDLL );
ConVar cl_wpn_sway_scale( "cl_wpn_sway_scale", "1.0", FCVAR_CLIENTDLL|FCVAR_CHEAT );
#endif

void CPredictedViewModel::CalcViewModelLag( Vector& origin, QAngle& angles, QAngle& /*original_angles*/ )
{
#ifdef CLIENT_DLL
	float interp = cl_wpn_sway_interp.GetFloat();
	if ( !interp /*|| m_bShouldIgnoreOffsetAndAccuracy */ )
		return;

	if ( prediction->InPrediction() && !prediction->IsFirstTimePredicted() )
	{
		origin += m_vPredictedOffset;
		return;
	}

	// Calculate our drift
	Vector	forward, right, up;
	AngleVectors( angles, &forward, &right, &up );
	
	// Add an entry to the history.
	m_vLagAngles = angles;
	m_LagAnglesHistory.NoteChanged( gpGlobals->curtime, gpGlobals->curtime, interp, false );
	
	// Interpolate back 100ms.
	m_LagAnglesHistory.Interpolate( gpGlobals->curtime, interp );
	
	// Now take the 100ms angle difference and figure out how far the forward vector moved in local space.
	Vector vLaggedForward;
	QAngle angleDiff = m_vLagAngles - angles;
	AngleVectors( -angleDiff, &vLaggedForward, 0, 0 );
	Vector vForwardDiff = Vector(1,0,0) - vLaggedForward;

	//if ( ShouldFlipViewModel() )
	//	right = -right;

	// Now offset the origin using that.
	vForwardDiff *= cl_wpn_sway_scale.GetFloat();
	m_vPredictedOffset = forward*vForwardDiff.x + right*-vForwardDiff.y + up*vForwardDiff.z;

	// reduce offset as viewmodel angle approaches nearly vertical
	float flMult = clamp( abs(DotProduct(up, Vector(0,0,1))) - 0.02f, 0, 1 );

	origin += (m_vPredictedOffset * flMult);
#endif
}
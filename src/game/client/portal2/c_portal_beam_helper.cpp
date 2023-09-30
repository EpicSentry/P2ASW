#include "cbase.h"
#include "c_portal_base2d.h"
#include "c_portal_beam_helper.h"
#include "vgui_controls/TreeView.h"

C_PortalBeamHelper::C_PortalBeamHelper( void )
{
  m_pBeamTemplate = NULL;
  m_beams.Purge();
}

C_PortalBeamHelper::~C_PortalBeamHelper( void )
{
	for ( int i = 0 ; i < m_beams.Count(); ++i )
	{
		m_beams[i]->Remove();
		m_beams[i] = NULL;
	}
}

void C_PortalBeamHelper::Init( C_Beam *pBeamTemplate )
{
	m_pBeamTemplate = pBeamTemplate;
	m_beams.AddToTail( pBeamTemplate );
}

C_Beam *C_PortalBeamHelper::CreateBeam( void )
{
	C_Beam *pNewBeam = C_Beam::BeamCreate( m_pBeamTemplate->GetModelName(), m_pBeamTemplate->GetWidth() );
   
	pNewBeam->SetType( m_pBeamTemplate->GetType() );
	pNewBeam->SetBeamFlags( m_pBeamTemplate->GetBeamFlags() );  
	pNewBeam->SetHaloScale( m_pBeamTemplate->GetHaloScale() );
	pNewBeam->SetWidth( m_pBeamTemplate->GetWidth() );
	pNewBeam->SetEndWidth( m_pBeamTemplate->GetEndWidth() );
	pNewBeam->SetFadeLength( m_pBeamTemplate->GetFadeLength() );
	pNewBeam->SetNoise( m_pBeamTemplate->GetNoise() );
	pNewBeam->SetRenderColorR( m_pBeamTemplate->GetRenderColorR() );
	pNewBeam->SetRenderColorG( m_pBeamTemplate->GetRenderColorG() );
	pNewBeam->SetRenderColorB( m_pBeamTemplate->GetRenderColorB() );
	pNewBeam->SetRenderAlpha( m_pBeamTemplate->GetRenderAlpha() );
	pNewBeam->SetFrame( m_pBeamTemplate->GetFrame() );
	pNewBeam->SetScrollRate( m_pBeamTemplate->GetScrollRate() );
  
	pNewBeam->SetHDRColorScale( m_pBeamTemplate->GetHDRColorScale() );

  return pNewBeam;
}

void C_PortalBeamHelper::UpdatePointDirection( const Vector &vStartPoint, const Vector &vDirection, unsigned int fMask, ITraceFilter *pTraceFilter, trace_t &tr )
{
	Vector vEndPoint = vStartPoint + ( vDirection * MAX_TRACE_LENGTH );

	C_PortalBeamHelper::UpdatePoints( vStartPoint, vEndPoint, fMask, pTraceFilter, tr );
}

void C_PortalBeamHelper::UpdatePoints( const Vector &vStartPoint, const Vector &vEndPoint, unsigned int fMask, ITraceFilter *pTraceFilter, trace_t &tr )
{
	Vector vStart = vStartPoint;
	Vector vDir = vEndPoint - vStartPoint;
	VectorNormalize( vDir );

	bool bHitReflectiveSurface = false;
	bool bFirePass = true;	

	trace_t tempTrace;

	int nLastRemovedBeam = 0;
	while ( (bHitReflectiveSurface || bFirePass) && nLastRemovedBeam != 10 )
	{
		if ( nLastRemovedBeam == m_beams.Count() )
		{
			m_beams.AddToTail( CreateBeam() );
		}

		if ( nLastRemovedBeam )
		{
			m_beams[nLastRemovedBeam]->SetEndWidth( m_pBeamTemplate->GetEndWidth() );
		}

		Ray_t ray;
		ray.Init( vStart, (vDir * MAX_TRACE_LENGTH) + vStart - vStart );
    
		UTIL_ClearTrace( tempTrace );

		enginetrace->TraceRay( ray, fMask, pTraceFilter, &tempTrace );

		if ( r_visualizetraces.GetBool() )
		  DebugDrawLine( tempTrace.startpos, tempTrace.endpos, 255, 0, 0, 1, -1.0 );
		
		C_Portal_Base2D *pPortal; // [esp+14Ch] [ebp-1Ch] BYREF

		if ( UTIL_DidTraceTouchPortals( ray, tempTrace, &pPortal, 0 )
		  && pPortal
		  && pPortal->IsActivedAndLinked() )
		{

			m_beams[nLastRemovedBeam]->PointsInit( vStart, tempTrace.endpos);
			m_beams[nLastRemovedBeam]->SetAbsOrigin( vStart );
			m_beams[nLastRemovedBeam]->TurnOff();

			Ray_t rayTransformed;
			UTIL_Portal_RayTransform( pPortal->m_matrixThisToLinked, ray, rayTransformed );

			vDir = rayTransformed.m_Delta;
			VectorNormalize( vDir );
			UTIL_Portal_PointTransform( pPortal->m_matrixThisToLinked, tempTrace.endpos, vStart );
			bFirePass = true;
			bHitReflectiveSurface = false;
		}
		else
		{	
			m_beams[nLastRemovedBeam]->PointsInit( vStart, tempTrace.endpos );
			m_beams[nLastRemovedBeam]->SetAbsOrigin( vStart );
			m_beams[nLastRemovedBeam]->TurnOff();
			bHitReflectiveSurface = UTIL_Paint_Reflect( tempTrace, vStart, vDir, REFLECT_POWER );
			bFirePass = true;
		}
		++nLastRemovedBeam;
	}

	int iLastBeam = m_beams.Count() - 1;

	bool bRemovedAll = m_beams.Count() == nLastRemovedBeam;
	
	int nNumRemoveda = m_beams.Count() - nLastRemovedBeam;

	if ( nNumRemoveda >= 0 && !bRemovedAll && iLastBeam >= 0 )
	{
		int iLastBeamIndex = m_beams.Count() - 1;

		while ( nNumRemoveda > 0 && iLastBeamIndex >= 0 );
		{
			--nNumRemoveda;
			--iLastBeamIndex;
			GetLastBeam()->Remove();
			m_beams.FindAndRemove( GetLastBeam() );

		}
	}

	tr = tempTrace;

}

void C_PortalBeamHelper::TurnOff( void )
{
	for ( int i = 0; i < m_beams.Count(); ++i )
		m_beams[i]->TurnOff();
}

void C_PortalBeamHelper::TurnOn( void )
{
	for ( int i = 0; i < m_beams.Count(); ++i )
	{
		m_beams[i]->TurnOn();
		m_beams[i]->UpdateVisibility();
		//m_beams[i]->OnDisableShadowDepthRenderingChanged();
		//m_beams[i]->OnShadowDepthRenderingCacheableStateChanged();
	}
}

int C_PortalBeamHelper::BeamCount( void )
{
	return m_beams.Count();
}

C_Beam *C_PortalBeamHelper::GetBeam( int i )
{
	return m_beams[i];
}

C_Beam *C_PortalBeamHelper::GetFirstBeam( void )
{
	return m_beams[0];
}

C_Beam *C_PortalBeamHelper::GetLastBeam( void )
{
	return m_beams[m_beams.Count() - 1];
}
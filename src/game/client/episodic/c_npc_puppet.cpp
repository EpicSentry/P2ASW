//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "c_ai_basenpc.h"
#include "bone_setup.h"

// Must be the last file included
#include "memdbgon.h"


extern ConVar r_sequence_debug;

class C_NPC_Puppet : public C_AI_BaseNPC
{
	DECLARE_CLASS( C_NPC_Puppet, C_AI_BaseNPC );
public:

	virtual void	ClientThink( void );
	virtual void	OnDataChanged( DataUpdateType_t updateType );
	virtual void	BuildTransformations( CStudioHdr *pStudioHdr, Vector *pos, Quaternion q[], const matrix3x4_t& cameraTransform, int boneMask, CBoneBitList &boneComputed );
	
	EHANDLE		m_hAnimationTarget;
	int			m_nTargetAttachment;

	DECLARE_CLIENTCLASS();
};

IMPLEMENT_CLIENTCLASS_DT( C_NPC_Puppet, DT_NPC_Puppet, CNPC_Puppet )
	RecvPropEHandle( RECVINFO(m_hAnimationTarget) ),
	RecvPropInt( RECVINFO(m_nTargetAttachment) ),
END_RECV_TABLE()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_NPC_Puppet::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );

	if ( updateType == DATA_UPDATE_CREATED )
	{
		SetNextClientThink( CLIENT_THINK_ALWAYS );
	}
}

//-----------------------------------------------------------------------------
// Purpose: We need to slam our position!
//-----------------------------------------------------------------------------
void C_NPC_Puppet::BuildTransformations( CStudioHdr *pStudioHdr, Vector *pos, Quaternion q[], const matrix3x4_t& cameraTransform, int boneMask, CBoneBitList &boneComputed )
{
	if ( m_hAnimationTarget && m_nTargetAttachment != -1 )
	{
		C_BaseAnimating *pTarget = m_hAnimationTarget->GetBaseAnimating();
		if ( pTarget )
		{
			matrix3x4_t matTarget;
			pTarget->GetAttachment( m_nTargetAttachment, matTarget );

			MatrixCopy( matTarget, GetBoneForWrite( 0 ) );
			boneComputed.ClearAll(); // FIXME: Why is this calculated already?
			boneComputed.MarkBone( 0 );
		}
	}

	// Call the baseclass
	BaseClass::BuildTransformations( pStudioHdr, pos, q, cameraTransform, boneMask, boneComputed );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_NPC_Puppet::ClientThink( void )
{
	if ( m_hAnimationTarget == NULL )
		return;

	C_BaseAnimating *pTarget = m_hAnimationTarget->GetBaseAnimating();
	if ( pTarget == NULL )
		return;

	int nTargetSequence = pTarget->GetSequence();
	const char *pSequenceName = pTarget->GetSequenceName( nTargetSequence );

	int nSequence = LookupSequence( pSequenceName );
	if ( nSequence >= 0 )
	{
		if ( nSequence != GetSequence() )
		{
			SetSequence( nSequence );
			UpdateVisibility();
		}

		SetCycle( pTarget->GetCycle() );
		SetPlaybackRate( pTarget->GetPlaybackRate() );	
	}
}

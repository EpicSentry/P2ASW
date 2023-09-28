//===== Copyright � 1996-2005, Valve Corporation, All rights reserved. ======//
//
//  Purpose: (IDA Pro + 2014 dev leak PDB = this file made from scratch)
//
//============================================================================//

#include "cbase.h"

#include "ai_basenpc.h"
#include "ai_baseactor.h"
#include "physics_bone_follower.h"

#include "shareddefs.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"



//-----------------------------------------------------------------------------
// Purpose: wetly
//-----------------------------------------------------------------------------
// idk if any of these are static, however, they are all correctly placed
class CNPC_Wheatley_Boss : public CAI_BaseActor
{
public:
    DECLARE_CLASS( CNPC_Wheatley_Boss, CAI_BaseActor );
    DECLARE_SERVERCLASS();

    virtual void Spawn();
    virtual void Precache();
    virtual void UpdateOnRemove();

    virtual float MaxYawSpeed() { return 0.0; } // Completely restored. (Line 34)
    virtual Class_T Classify() { return CLASS_NONE; } // Linux (L35 BP)
    virtual int GetSoundInterests() { return NULL; } // Linux (L36 BP)
    virtual void NPCThink();
private:
    void CreateBoneFollowers( void );
    CBoneFollowerManager m_BoneFollowerManager;

    DECLARE_DATADESC();
};

LINK_ENTITY_TO_CLASS( npc_wheatley_boss, CNPC_Wheatley_Boss ); // (L45 BP)

BEGIN_DATADESC( CNPC_Wheatley_Boss ) // Completely restored (Line 47)
    DEFINE_EMBEDDED( m_BoneFollowerManager )
END_DATADESC()

IMPLEMENT_SERVERCLASS_ST( CNPC_Wheatley_Boss, DT_NPC_Wheatley_Boss ) // Completely restored (Line 51)
END_SEND_TABLE() // There is nothing defined in this

//-----------------------------------------------------------------------------
// Purpose: Completely restored
//-----------------------------------------------------------------------------
void CNPC_Wheatley_Boss::Spawn()
{ // Line 58
    Precache(); // Line 59

    SetModel( STRING( GetModelName() ) ); // Line 61

    UTIL_SetSize( this, VEC_HULL_MIN, VEC_HULL_MAX ); // Line 63
    SetSolid( SOLID_NONE ); // Line 64

    SetMoveType( MOVETYPE_NONE ); // Line 66
    m_bloodColor		= DONT_BLEED;// No blood in Portal 2 (Line 67)
    m_iHealth			= 8000;// no wheatley boss should die (Line 68)
	m_flFieldOfView		= 0.5;// indicates the width of this NPC's forward view cone ( as a dotproduct result )

	m_NPCState			= NPC_STATE_NONE;
    m_takedamage        = DAMAGE_NO;// Doesn't take damage directly
    CapabilitiesClear(); // Line 73
    CapabilitiesAdd( bits_CAP_TURN_HEAD | bits_CAP_ANIMATEDFACE ); // Line 74

    SetHullType( HULL_HUMAN );
    SetHullSizeNormal( false ); // Line 77

    m_spawnflags       |= SF_NPC_FALL_TO_GROUND; // NOT AddSpawnFlags! It is manually written (Line 79)

    NPCInit(); // Line 81



    CreateVPhysics(); // Line 85
    CreateBoneFollowers(); // Line 86
} // Line 87

//-----------------------------------------------------------------------------
// Purpose: Completely restored. (Points to CCycler::Precache)
//-----------------------------------------------------------------------------
void CNPC_Wheatley_Boss::Precache()
{ // (L93 BP)
    PrecacheModel( (const char *)STRING( GetModelName() ) ); // (L94 BP)
}

void CNPC_Wheatley_Boss::CreateBoneFollowers( void ) // Purpose: Exactly the same as CPropLinkedPortalDoor::CreateBoneFollowers on Windows
{ // Line 98
	// already created bone followers?  Don't do so again.
	if ( m_BoneFollowerManager.GetNumBoneFollowers() )
		return;

	KeyValues *modelKeyValues = new KeyValues("");
	if ( modelKeyValues->LoadFromBuffer( modelinfo->GetModelName( GetModel() ), modelinfo->GetModelKeyValueText( GetModel() ) ) )
	{
		// Do we have a bone follower section?
		KeyValues *pkvBoneFollowers = modelKeyValues->FindKey("bone_followers");
		if ( pkvBoneFollowers )
		{
			// Loop through the list and create the bone followers
			KeyValues *pBone = pkvBoneFollowers->GetFirstSubKey();
			while ( pBone )
			{
				// Add it to the list
				const char *pBoneName = pBone->GetString();
				m_BoneFollowerManager.AddBoneFollower( this, pBoneName );

				pBone = pBone->GetNextKey();
			}
		}

		modelKeyValues->deleteThis();
	}

	// if we got here, we don't have a bone follower section, but if we have a ragdoll
	// go ahead and create default bone followers for it
	if ( m_BoneFollowerManager.GetNumBoneFollowers() == 0 )
	{
		vcollide_t *pCollide = modelinfo->GetVCollide( GetModelIndex() );
		if ( pCollide && pCollide->solidCount > 1 )
		{
			CreateBoneFollowersFromRagdoll( this, &m_BoneFollowerManager, pCollide );
		}
	}
}

void CNPC_Wheatley_Boss::NPCThink() // Purpose: Completely restored. Linux
{ // Line 138
	BaseClass::NPCThink(); // Line 139
	m_BoneFollowerManager.UpdateBoneFollowers( this ); // Line 140
}

void CNPC_Wheatley_Boss::UpdateOnRemove() // Purpose: Completely restored. Linux
{ // Line 144
	m_BoneFollowerManager.DestroyBoneFollowers(); // Line 145
    BaseClass::UpdateOnRemove(); // Line 146
}

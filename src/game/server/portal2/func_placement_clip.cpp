//========= Copyright 1996-2009, Valve Corporation, All rights reserved. ============//
//  Purpose: (IDA Pro + 2014 dev leak PDB = this file made from scratch)
//=============================================================================//

#include "cbase.h"
#include "baseentity.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// Purpose: All this does is prevent portal shots from being allowed within a certain volume
class CFuncPlacementClip : public CBaseEntity // This class is a nasty copy-paste of CFuncVehicleClip
{
public:
    DECLARE_CLASS( CFuncPlacementClip, CBaseEntity );
    DECLARE_DATADESC();

    virtual void Spawn();                   // IDA says this is virtual, and it is. Why does CFuncVehicleClip say otherwise?
    virtual bool CreateVPhysics( void );    // IDA says this is virtual, and it is. Why does CFuncVehicleClip say otherwise?

    void InputEnable( inputdata_t &data );
    void InputDisable( inputdata_t &data );
};

BEGIN_DATADESC( CFuncPlacementClip ) // Completely restored (Line 25)
	DEFINE_INPUTFUNC( FIELD_VOID, "Enable", InputEnable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Disable", InputDisable ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( func_placement_clip, CFuncPlacementClip );

void CFuncPlacementClip::Spawn() // Purpose: Completely restored
{ // Line 33

    SetLocalAngles( vec3_angle ); // Line 35
    SetMoveType( MOVETYPE_PUSH );  // so it doesn't get pushed by anything (Line 36)
    SetModel( STRING( GetModelName() ) ); // Line 37


    AddFlag( FL_BASEVELOCITY ); // Line 40

    CreateVPhysics(); // Line 42

    AddEffects( EF_NODRAW );		// make entity invisible (Line 44)

    SetCollisionGroup( COLLISION_GROUP_PLACEMENT_SOLID );   // Special Portal 2 flag (Line 46)
} // Line 47

bool CFuncPlacementClip::CreateVPhysics( void ) // Purpose: Completely restored
{ // Line 50
    SetSolid( SOLID_BSP ); // Line 51
    VPhysicsInitStatic(); // Line 52

    return true; // Line 54
} // Line 55

void CFuncPlacementClip::InputEnable( inputdata_t &data ) // Purpose: Completely restored
{ // Line 58
	IPhysicsObject *pPhys = VPhysicsGetObject(); // Line 59
	if ( pPhys ) // Line 60
	{
		pPhys->EnableCollisions( true );
	}
    RemoveSolidFlags( FSOLID_NOT_SOLID ); // Line 64
} // Line 65

void CFuncPlacementClip::InputDisable( inputdata_t &data ) // Purpose: Completely restored
{ // Line 68
	IPhysicsObject *pPhys = VPhysicsGetObject(); // Line 69
	if ( pPhys ) // Line 70
	{
		pPhys->EnableCollisions( false );
	}
    AddSolidFlags( FSOLID_NOT_SOLID ); // Line 74
} // Line 75

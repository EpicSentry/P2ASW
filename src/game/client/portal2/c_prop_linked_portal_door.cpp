#include "cbase.h"
#include "portal_player_shared.h"

class C_PropLinkedPortalDoor : public C_BaseAnimating, public CSignifierTarget
{
public:
	DECLARE_CLASS( C_PropLinkedPortalDoor, C_BaseAnimating );
	DECLARE_CLIENTCLASS();
	
	C_PropLinkedPortalDoor( void );
	~C_PropLinkedPortalDoor( void );

	bool OverrideSignifierPosition( void );
	bool GetSignifierPosition( Vector &vSource, Vector &vPositionOut, Vector &vNormalOut );
};

IMPLEMENT_CLIENTCLASS_DT( C_PropLinkedPortalDoor, DT_PropLinkedPortalDoor, CPropLinkedPortalDoor )
END_RECV_TABLE()

C_PropLinkedPortalDoor::C_PropLinkedPortalDoor( void )
{

}

C_PropLinkedPortalDoor::~C_PropLinkedPortalDoor( void )
{

}

bool C_PropLinkedPortalDoor::OverrideSignifierPosition( void )
{
	return true;
}

bool C_PropLinkedPortalDoor::GetSignifierPosition( Vector &vSource, Vector &vPositionOut, Vector &vNormalOut )
{
	vSource = GetAbsOrigin();
	vPositionOut = GetAbsOrigin();
	AngleVectors( GetAbsAngles(), &vNormalOut );

	return true;
}
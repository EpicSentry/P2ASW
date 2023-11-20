#include "cbase.h"
#include "c_baseprojector.h"

#undef CBaseProjector

IMPLEMENT_CLIENTCLASS_DT( C_BaseProjector, DT_BaseProjector, CBaseProjector )

	RecvPropEHandle( RECVINFO( m_hFirstChild ) ),
	RecvPropBool( RECVINFO( m_bEnabled ) ),

END_RECV_TABLE()

C_BaseProjector::C_BaseProjector()
{

}

C_BaseProjector::~C_BaseProjector()
{

}
//========= Copyright © 1996-2008, Valve Corporation, All rights reserved. ============//
//
// Purpose:		Client handler implementations for instruction players how to play
//
//=============================================================================//

#include "cbase.h"

#include "c_gameinstructor.h"
#include "c_baselesson.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


void CScriptedIconLesson::Mod_PreReadLessonsFromFile( void )
{

}


bool CScriptedIconLesson::Mod_ProcessElementAction( int iAction, bool bNot, const char *pchVarName, EHANDLE &hVar, const CGameInstructorSymbol *pchParamName, float fParam, C_BaseEntity *pParam, const char *pchParam, bool &bModHandled )
{
	return false;
}

bool C_GameInstructor::Mod_HiddenByOtherElements( void )
{

	return false;
}

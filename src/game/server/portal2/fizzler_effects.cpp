//========= Copyright � 1996-2009, Valve Corporation, All rights reserved. ============//
//
// Purpose: (IDA Pro + 2014 dev leak PDB = this file made from scratch)
//
//=====================================================================================//
#include "cbase.h"
#include "fizzler_effects.h"
#include "baseanimating.h"

// NOTE: This has to be the last file included!
#include "tier0/memdbgon.h"

ConVar fizzler_fx_model_end_scale( "fizzler_fx_model_end_scale", "0.001f", FCVAR_DEVELOPMENTONLY | FCVAR_CHEAT );                           // The target scale of the model to achieve (L13 BP)
ConVar fizzler_fx_model_scale_end_time_offset( "fizzler_fx_model_scale_end_time_offset", "0.5f", FCVAR_DEVELOPMENTONLY | FCVAR_CHEAT );     // This sets how much earlier to end the fizzle animation during the lifetime of the sequence (L14 BP)
ConVar fizzler_fx_model_scale_start_delay_time( "fizzler_fx_model_scale_start_delay_time", "1.0f", FCVAR_DEVELOPMENTONLY | FCVAR_CHEAT );   // This sets how late to scale the model once dissolving starts (L15 BP)

// Link it
LINK_ENTITY_TO_CLASS( env_fizzler_effects, FizzlerEffects ); // (L18 BP)

BEGIN_DATADESC( FizzlerEffects ) // Completely restored (Line 20)
    DEFINE_THINKFUNC( FizzleThink ) // (L21 BP)
END_DATADESC() // (L22 BP)

IMPLEMENT_SERVERCLASS_ST( FizzlerEffects, DT_FizzlerEffects ) // Completely restored (Line 24)
END_SEND_TABLE() // There is nothing defined in this (L25 BP)

FizzlerEffects::FizzlerEffects() // Purpose: Completely restored
{ // Unknown line (probably goes here...)
    m_ElapsedAnimationTime = 0.0;
    m_ScaleStarted = false;
}

FizzlerEffects* FizzlerEffects::Create( CBaseEntity* pAttachTo ) // Purpose: Completely restored
{ // Line 34
    FizzlerEffects* pFizzlerEffects = (FizzlerEffects *)CreateEntityByName( "env_fizzler_effects" );
    if ( pFizzlerEffects )
    {
        pFizzlerEffects->AttachToEntity( pAttachTo );
        pFizzlerEffects->Spawn();

        pAttachTo->DispatchResponse( "TLK_DISSOLVESCREAM" );

        CBaseAnimating* pDissolvingAnimating = pAttachTo->GetBaseAnimating();
        if ( pDissolvingAnimating )
        {
            pDissolvingAnimating->AddFlag( FL_DISSOLVING );
            pDissolvingAnimating->SetEffectEntity( pFizzlerEffects );
            pDissolvingAnimating->ResetSequence( 1 );
        }
    }

    return pFizzlerEffects;
}

int FizzlerEffects::UpdateTransmitState() // Purpose: Completely restored (Linux)
{
    return SetTransmitState( FL_EDICT_ALWAYS ); // (L57 BP)
}

void FizzlerEffects::Spawn() // Purpose: Completely restored
{ // Line 61
    BaseClass::Spawn(); // Line 62

    SetThink( &FizzlerEffects::FizzleThink ); // Line 64
    SetNextThink( gpGlobals->curtime ); // Line 65
} // Line 66

void FizzlerEffects::AttachToEntity( CBaseEntity* pAttachTo ) // Purpose: Completely restored (Linux)
{ // Unknown line, prob just goes here
    SetParent( pAttachTo );

    SetLocalOrigin( vec3_origin );
    SetLocalAngles( vec3_angle );
}

void FizzlerEffects::FizzleThink() // Purpose: Completely restored with line placement
{ // Line 77
    CBaseAnimating *pDissolving = (CBaseAnimating* )GetMoveParent();
    if ( !pDissolving )
    {
        UTIL_Remove( this ); // Fizzle effect is done OR the fizzled entity is gone
    }
    else
    {
        m_ElapsedAnimationTime += pDissolving->GetAnimTimeInterval(); // Increase the elapsed time by however long we took from the last interval

        float scaleDelayTime = fizzler_fx_model_scale_start_delay_time.GetFloat(); // Only set the model scale one time during the dissolve sequence
        if ( !m_ScaleStarted && m_ElapsedAnimationTime >= scaleDelayTime )
        {
            // Do scaling stuff
            pDissolving->SetModelScale( fizzler_fx_model_end_scale.GetFloat(), pDissolving->SequenceDuration( pDissolving->GetSequence() ) - fizzler_fx_model_scale_end_time_offset.GetFloat() - scaleDelayTime );
            m_ScaleStarted = true; // Mark as scaling
        }

        pDissolving->SetCollisionGroup( COLLISION_GROUP_DISSOLVING );
        pDissolving->StudioFrameAdvance(); // Continue animation
        if ( pDissolving->IsSequenceFinished() ) // Check if the fizzle effect is done
        {
            UTIL_Remove( pDissolving );
            UTIL_Remove( this );
        }
        else
        {
            SetNextThink( gpGlobals->curtime ); // Continue looping if not done animating
        }
    }
} // Line 107

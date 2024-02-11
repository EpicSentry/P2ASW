//===== Copyright � 1996-2005, Valve Corporation, All rights reserved. ======//
//
//  Purpose: Laser beam targets for Portal 2, which include both
//           the catcher and relays
//          (IDA Pro + 2014 dev leak PDB = this file made from scratch)
//
//===========================================================================//

#include "cbase.h"
#include "point_laser_target.h"
#include "mathlib/vector.h"
#include "particle_parse.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// NOTE: fsqrt in IDA is the same as sqrt in the code repo!


// Purpose: Completely restored. 7 in here
BEGIN_DATADESC( CPortalLaserTarget ) // Line 21
    DEFINE_THINKFUNC( DisableThink ),

    DEFINE_FIELD( m_pCatcher, FIELD_CLASSPTR ),
    DEFINE_FIELD( m_bPowered, FIELD_BOOLEAN ),

    DEFINE_KEYFIELD( m_ModelName, FIELD_MODELNAME, "model" ),
    DEFINE_KEYFIELD( m_bTerminalPoint, FIELD_BOOLEAN, "terminalpoint" ),

    DEFINE_OUTPUT( m_OnPowered, "OnPowered" ),
    DEFINE_OUTPUT( m_OnUnpowered, "OnUnpowered" ),
END_DATADESC()

LINK_ENTITY_TO_CLASS( point_laser_target, CPortalLaserTarget );

// Custom defines
#define LASER_CATCHER_NAME "models/props/laser_catcher.mdl"
#define LASER_CATCHER_CENTER_NAME "models/props/laser_catcher_center.mdl"
#define LASER_RELAY_NAME "models/props/laser_receptacle.mdl"

#if 1
Vector vLaserCatcherExtents( 11.0, 11.0, 11.0 );    // CLaserCatcher default extent
#else
Vector vLaserCatcherExtents( 16.0, 16.0, 16.0 );	// CLaserCatcher default extent
#endif
Vector vRelayExtents( 10.0, 10.0, 17.0 );           // CLaserRelay default extent

//-----------------------------------------------------------------------------
// Purpose: Functionally completely restored
//-----------------------------------------------------------------------------
void CPortalLaserTarget::Spawn()
{
    BaseClass::Spawn();
    AddFlag( FL_OBJECT );
    m_bPowered = false;
    Vector vExtents;

    if ( IsTerminalPoint() ) // Replaced with optimized function "IsTerminalPoint" (Line 54)
    {
#if 1
		CLaserCatcher *pLaserCatcher = (CLaserCatcher *)GetParent(); // Line 56

        if ( pLaserCatcher && !Q_strcmp( pLaserCatcher->GetCatcherModelName(), LASER_CATCHER_CENTER_NAME ) ) // Line 58
        {
            Vector vForward, vRight, vUp;
            pLaserCatcher->GetVectors( &vForward, &vRight, &vUp ); // Line 61
			
			vExtents = ( ( vUp + vRight ) * 20.0 ) + ( vForward * 15.0 ); // Note: This is the correct logic that runs from lines 62-65, but there is no way to know how it is written

			vExtents.x = fabs( vExtents.x );
			vExtents.y = fabs( vExtents.y );
			vExtents.z = fabs( vExtents.z );
        }
        else
#endif
        {
            vExtents = vLaserCatcherExtents; // Line 69
        }
    }
    else
    {
        vExtents = vRelayExtents; // Line 74
    }
    UTIL_SetSize( this, -vExtents, vExtents ); // Line 76
} // Line 77

//-----------------------------------------------------------------------------
// Purpose: Completely restored. env_portal_laser "attacks" the target, and fires an output
//-----------------------------------------------------------------------------
int CPortalLaserTarget::OnTakeDamage( const CTakeDamageInfo &info )
{
    if ( IsPowered() ) // Replaced with optimized function "IsPowered" (Line 84)
    {
        // Continue thinking
        SetNextThink( gpGlobals->curtime + 0.1 ); // Note: optimized to jump to line 103... (Line 87)
        return 0; // Line 88
    }

    if ( info.GetAttacker() && FClassnameIs( info.GetAttacker(), "env_portal_laser" ) ) // Both checks are on line 91
    {
        m_bPowered = true;
        m_OnPowered.FireOutput( this, this );

        // Fire catcher logic
        if ( m_pCatcher ) // Line 97
        {
            m_pCatcher->OnPowered(); // CLaserCatcher::OnPowered() (Line 99)
        }

        SetThink( &CPortalLaserTarget::DisableThink ); // Line 102
        SetNextThink( gpGlobals->curtime + 0.1 ); // Line 103
    }

    return 0; // Line 106
} // Line 107




//-----------------------------------------------------------------------------
// Purpose: Completely restored. Turns off the laser catcher
//-----------------------------------------------------------------------------
void CPortalLaserTarget::DisableThink( void )
{ // Line 116
    m_bPowered = false;
    m_OnUnpowered.FireOutput( this, this );
    if ( m_pCatcher ) // Line 119
    {
        m_pCatcher->OnUnPowered(); // CLaserCatcher::OnUnPowered() (Line 121)
    }

    SetThink( NULL ); // Line 124
} // Line 125
















LINK_ENTITY_TO_CLASS( prop_laser_catcher, CLaserCatcher );

BEGIN_DATADESC( CLaserCatcher ) // Completely restored (Line 144)
    DEFINE_FIELD( m_pCatcherLaserTarget, FIELD_CLASSPTR ), // TODO: Confirm if this is the right macro

    DEFINE_THINKFUNC( AnimateThink ),

    DEFINE_FIELD( m_IdleSequence, FIELD_INTEGER ),
    DEFINE_FIELD( m_PowerOnSequence, FIELD_INTEGER ),
    DEFINE_FIELD( m_iTargetAttachment, FIELD_INTEGER ),

    DEFINE_FIELD( m_iPowerState, FIELD_INTEGER ),

    DEFINE_KEYFIELD( m_bRusted, FIELD_BOOLEAN, "SkinType" ),

    DEFINE_OUTPUT( m_OnPowered, "OnPowered" ),
    DEFINE_OUTPUT( m_OnUnpowered, "OnUnpowered" ),
END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: Not done yet
//-----------------------------------------------------------------------------
CLaserCatcher::CLaserCatcher()
{ // Line 165
    AddEffects( EF_MARKED_FOR_FAST_REFLECTION );
}



//-----------------------------------------------------------------------------
// Purpose: Completely restored
//-----------------------------------------------------------------------------
void CLaserCatcher::Precache()
{ // Line 175

    PrecacheModel( GetCatcherModelName() ); // Line 177
    PrecacheParticleSystem( "laser_relay_powered" ); // Line 178

    PrecacheScriptSound( "prop_laser_catcher.poweron" ); // Line 180
    PrecacheScriptSound( "prop_laser_catcher.poweroff" ); // Line 181
    PrecacheScriptSound( "prop_laser_catcher.powerloop" ); // Line 182

    BaseClass::Precache(); // Line 184
}


//-----------------------------------------------------------------------------
// Purpose: Completely restored
//-----------------------------------------------------------------------------
void CLaserCatcher::Spawn()
{ // Line 192
    Precache(); // Line 193
    BaseClass::Spawn(); // Line 194

    // Check if there is a custom model
    SetModel( GetCatcherModelName() ); // Line 197

    AddEffects( EF_NOSHADOW | EF_NORECEIVESHADOW ); // Line 199
    SetSolid( SOLID_VPHYSICS ); // Line 200
    SetMoveType( MOVETYPE_VPHYSICS ); // Line 201

    m_IdleSequence = LookupSequence( "idle" ); // Line 203
    m_PowerOnSequence = LookupSequence( "spin" ); // Line 204
    m_iTargetAttachment = LookupAttachment( "laser_target" ); // Line 205


    // Start off
    ResetSequence( m_PowerOnSequence ); // Line 209
    SetPlaybackRate( 0.0 ); // Line 210

    // Mark the model skin to use later
    if ( m_bRusted ) // Line 213
    {
        m_iOnSkin = 3; // Line 215  (Rusted on)
        m_iOffSkin = 2; // Line 216 (Rusted off)
    }
    else
    {
        m_iOnSkin = 1; // Line 220  (Clean on)
        m_iOffSkin = 0; // Line 221 (Clean off)
    }

    // Default the skin to "off"
    m_nSkin = m_iOffSkin; // Line 225

    CreateHelperEntities(); // Line 227
    CreateVPhysics(); // Line 228

	SetFadeDistance( -1.0f, 0.0f ); // Line 230
	SetGlobalFadeScale( 0.0f ); // Line 231

} // Line 232

//-----------------------------------------------------------------------------
// Purpose: Completely restored (Unknown line placement, likely goes here...)
//-----------------------------------------------------------------------------
bool CLaserCatcher::CreateVPhysics()
{
    VPhysicsInitStatic();
    return true;
}

//-----------------------------------------------------------------------------
// Purpose: Completely restored
//-----------------------------------------------------------------------------
void CLaserCatcher::Activate()
{ // Line 247
    BaseClass::Activate(); // Line 248

    SetThink( &CLaserCatcher::AnimateThink ); // Line 250
    SetNextThink( gpGlobals->curtime + 0.1 ); // Line 251
} // Line 252

//-----------------------------------------------------------------------------
// Purpose: Completely restored
//-----------------------------------------------------------------------------
void CLaserCatcher::AnimateThink()
{ // Line 258
    if ( PoweringDown() ) // Line 259
    {
        SetPlaybackRate( SpinDown( GetPlaybackRate() ) ); // Line 261
    }
    if ( PoweringUp() ) // Line 263
    {
        SetPlaybackRate( SpinUp( GetPlaybackRate() ) ); // Line 265
    }

    // Continue the animation
    StudioFrameAdvance(); // Line 269
    DispatchAnimEvents( this ); // Line 270

    SetNextThink( gpGlobals->curtime + 0.1 ); // Line 272
} // Line 273

//-----------------------------------------------------------------------------
// Purpose: Completely restored
//-----------------------------------------------------------------------------
void CLaserCatcher::UpdateOnRemove()
{ // Line 279
    if ( m_pCatcherLaserTarget ) // Line 280
    {
        UTIL_Remove( m_pCatcherLaserTarget ); // Line 282
    }


    StopSound( "prop_laser_catcher.powerloop" ); // Line 286

    StopParticleEffect( this, "laser_relay_powered" ); // Line 288

    BaseClass::UpdateOnRemove(); // Line 290
}


//-----------------------------------------------------------------------------
// Purpose: Completely restored
//-----------------------------------------------------------------------------
void CLaserCatcher::OnPowered()
{ // Line 298
    // Fire output
    m_OnPowered.FireOutput( this, this ); // Line 300

    DispatchParticleEffect( "laser_relay_powered", PATTACH_POINT_FOLLOW, this, "particle_emitter" ); // Line 302
    EmitSound( "prop_laser_catcher.poweron" ); // Line 303
    EmitSound( "prop_laser_catcher.powerloop" ); // Line 304

    StopSound( "prop_laser_catcher.poweroff" ); m_iPowerState = POWERING_UP; // Line 306

    m_nSkin = m_iOnSkin; // Line 308
} // Line 309


//-----------------------------------------------------------------------------
// Purpose: Completely restored
//-----------------------------------------------------------------------------
void CLaserCatcher::OnUnPowered()
{ // Line 316
    // Fire output
    m_OnUnpowered.FireOutput( this, this ); // Line 318

    StopParticleEffect( this, "laser_relay_powered" ); // Line 320
    EmitSound( "prop_laser_catcher.poweroff" ); // Line 321

    StopSound( "prop_laser_catcher.powerloop" ); m_iPowerState = POWERING_DOWN; // Line 323

    m_nSkin = m_iOffSkin; // Line 325
} // Line 326

//-----------------------------------------------------------------------------
// Purpose: Unfinished
//-----------------------------------------------------------------------------
void CLaserCatcher::CreateHelperEntities()
{ // Line 332
    // Only do so if we don't have an existing target pair
    if ( !m_pCatcherLaserTarget ) // Line 334
    {
        Vector vecOrigin;
        GetAttachment( m_iTargetAttachment, vecOrigin, NULL, NULL, NULL ); // Line 337

        m_pCatcherLaserTarget = (CPortalLaserTarget *)CreateEntityByName( "point_laser_target" ); // Line 339

        m_pCatcherLaserTarget->SetAbsOrigin( vecOrigin ); // Line 341
        m_pCatcherLaserTarget->SetAbsAngles( GetAbsAngles() ); // Line 342
        m_pCatcherLaserTarget->KeyValue( "terminalpoint", IsTerminalPoint() ); // Replaced with optimized function "IsTerminalPoint" (Line 343)
        m_pCatcherLaserTarget->SetParent( this ); // Line 344

        // Ready to spawn
        DispatchSpawn( m_pCatcherLaserTarget ); // Line 347
		m_pCatcherLaserTarget->SetCatcher((CCatcher *)this);
    }
} // Line 350


//-----------------------------------------------------------------------------
// Purpose: Functionally completely restored
//-----------------------------------------------------------------------------
const char *CLaserCatcher::GetCatcherModelName()
{ // Line 357
    if ( !MAKE_STRING( m_ModelName.ToCStr() ) )
		return LASER_CATCHER_NAME;
    return m_ModelName.ToCStr();
}





//-----------------------------------------------------------------------------
// Purpose: Completely restored
//-----------------------------------------------------------------------------
float CLaserCatcher::SpinDown( float flSpinRate )
{ // Line 373



    float flNewSpinRate = fabs( flSpinRate ) - 0.1; // Line 377
    if ( flNewSpinRate <= 0.0 ) // Line 378
    {
        flNewSpinRate = 0.0; // Line 380
        m_iPowerState = UNPOWERED; // Line 381
    }

    return flNewSpinRate; // Line 384
} // Line 385

//-----------------------------------------------------------------------------
// Purpose: Completely restored
//-----------------------------------------------------------------------------
float CLaserCatcher::SpinUp( float flSpinRate )
{ // Line 391



    float flNewSpinRate = fabs( flSpinRate ) + 0.1; // Line 395
    if ( flNewSpinRate >= 1.0 ) // Line 396
    {
        flNewSpinRate = 1.0f; // Line 398
        m_iPowerState = POWERING_UP; // Line 399
    }

    return flNewSpinRate; // Line 402
} // Line 403

//-----------------------------------------------------------------------------
// Purpose: Completely restored. Linux
//-----------------------------------------------------------------------------
bool CLaserCatcher::PoweringDown()
{
    return ( m_iPowerState == POWERING_DOWN ); // Line 410
} // Line 411

//-----------------------------------------------------------------------------
// Purpose: Completely restored. Linux
//-----------------------------------------------------------------------------
bool CLaserCatcher::PoweringUp()
{
    return ( m_iPowerState == POWERING_UP ); // Line 418
} // Line 419


//-----------------------------------------------------------------------------
// Purpose: Not done
//-----------------------------------------------------------------------------
void CLaserCatcher::OnRestore()
{ // Line 426
    BaseClass::OnRestore();

    if ( m_pCatcherLaserTarget ) // Line 429
    {
        // Assign new catcher
        m_pCatcherLaserTarget->SetCatcher( this ); // Line 432

        // Check power state
        if ( m_pCatcherLaserTarget->IsPowered() ) // Replaced with optimized function "IsPowered"
        {
            EmitSound( "prop_laser_catcher.powerloop" );
            DispatchParticleEffect( "laser_relay_powered", PATTACH_POINT_FOLLOW, this, "particle_emitter", false, -1, NULL ); // Line 438
        }
    }

} // Line 442


LINK_ENTITY_TO_CLASS( prop_laser_relay, CLaserRelay );

BEGIN_DATADESC( CLaserRelay ) // Completely restored (Line 447)

    DEFINE_FIELD( m_pCatcherLaserTarget, FIELD_CLASSPTR ),

END_DATADESC()

const char *CLaserRelay::GetCatcherModelName() // Purpose: Functionally completely restored
{
    if ( !MAKE_STRING( m_ModelName.ToCStr() ) )
		return LASER_RELAY_NAME;
    return m_ModelName.ToCStr();
}



// The following have unknown line placements...

//-----------------------------------------------------------------------------
// Purpose: Completely restored. Retrieved from Linux dbg.
//          There are no XREF's to this...
//-----------------------------------------------------------------------------
bool CPortalLaserTarget::IsPowered()
{
    return m_bPowered;
}

//-----------------------------------------------------------------------------
// Purpose: Completely restored. Optimized out and Spawn() func implies this
//          Uses whatever the catcher entities set this to
//-----------------------------------------------------------------------------
bool CPortalLaserTarget::IsTerminalPoint()
{
    return m_bTerminalPoint;
}

//-----------------------------------------------------------------------------
// Purpose: Nonexistent in every decompile, but reimplemented to do what is done on
//          lines 347 & 432, as that is the only conceivable thing it can do (m_pCatcher is directly inaccessible)
//-----------------------------------------------------------------------------
void CPortalLaserTarget::SetCatcher( CCatcher *pCatcher ) // Custom var name
{
    m_pCatcher = pCatcher;
}

//-----------------------------------------------------------------------------
// Purpose: Completely restored. Laser beam ends when it is hit
//-----------------------------------------------------------------------------
bool CLaserCatcher::IsTerminalPoint()
{
    return true;
}

//-----------------------------------------------------------------------------
// Purpose: Completely restored. Laser beam does not end when it is hit
//-----------------------------------------------------------------------------
bool CLaserRelay::IsTerminalPoint()
{
    return false;
}
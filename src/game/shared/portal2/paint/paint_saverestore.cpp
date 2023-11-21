//========= Copyright © Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//==========================================================================//
#include "cbase.h"
#include "isaverestore.h"
#include "paint_stream_manager.h"

#ifdef GAME_DLL

#include "projectedwallentity.h"
#include "paint/paint_database.h"
#else

#include "c_world.h"

#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CPaintSaveRestoreBlockHandler : public CDefSaveRestoreBlockHandler
{
	virtual const char *GetBlockName() { return "PaintDatabase"; };

	virtual void PreSave( CSaveRestoreData * ) {}

	virtual void Save( ISave *pSave )
	{
		if ( !HASPAINTMAP )
		{
			return;
		}

#ifdef GAME_DLL
		// save paintmap data
		PaintDatabase.SavePaintmapData( pSave );

#ifndef NO_PROJECTED_WALL
		// save painted projected walls
		const ProjectedWallVector_t* pProjectedWalls = PaintDatabase.GetPaintedProjectedWalls();
		if ( pProjectedWalls )
		{
			int count = pProjectedWalls->Count();
			pSave->WriteInt( &count );

			for ( int i=0; i<count; ++i )
			{
				CProjectedWallEntity* pWall = pProjectedWalls->Element( i );
				if ( pWall )
				{
					pSave->StartBlock();
					{
						// save the source wall or source portal of the wall to make each wall unique
						bool bHasSourcePortal = !!pWall->GetSourcePortal();
						pSave->WriteBool( &bHasSourcePortal );
						if ( bHasSourcePortal )
						{
							EHANDLE hPortal = pWall->GetSourcePortal();
							pSave->WriteEHandle( &hPortal );
						}
						else
						{
							EHANDLE hProjectedWall = pWall;
							pSave->WriteEHandle( &hProjectedWall );
						}

						int nSegments = pWall->GetNumSegments();
						
						int nPaintedSegments = 0;
						for ( int s=0; s<nSegments; ++s )
						{
							PaintPowerType type = pWall->GetPaintPowerAtSegment( s );
							if ( type != NO_POWER )
							{
								nPaintedSegments++;
							}
						}
						pSave->WriteInt( &nPaintedSegments );

						float flSegmentLength = pWall->GetSegmentLength();
						Vector vDirection = pWall->GetEndPoint() - pWall->GetStartPoint();
						vDirection.NormalizeInPlace();
						for ( int s=0; s<nSegments; ++s )
						{
							int type = (int)pWall->GetPaintPowerAtSegment( s );
							if ( type != NO_POWER )
							{
								Vector vSegmentCenter = pWall->GetStartPoint() + ( ( 0.5f + s ) * flSegmentLength ) * vDirection;
								
								pSave->WriteInt( &type );
								pSave->WriteVector( vSegmentCenter );
							}
						}
					}
					pSave->EndBlock();
				}
			}
		} // pProjectedWalls
#endif

#endif
	}

	virtual void WriteSaveHeaders( ISave * ) {}

	virtual void PostSave() {}

	virtual void PreRestore()
	{
		if ( !HASPAINTMAP )
		{
			return;
		}

#ifdef GAME_DLL
		PaintDatabase.RemoveAllPaint();
#endif
	}

	virtual void ReadRestoreHeaders( IRestore * ) {}
	
	virtual void Restore( IRestore *pRestore, bool fCreatePlayers )
	{
		if ( !HASPAINTMAP )
		{
			return;
		}

#ifdef GAME_DLL
		// restore paintmap data
		PaintDatabase.RestorePaintmapData( pRestore );

#ifndef NO_PROJECTED_WALL
		// find all projected walls in the world
		ProjectedWallVector_t projectedWalls;
		CBaseEntity* pEnt = NULL;
		while ( ( pEnt = gEntList.FindEntityByClassname( pEnt, "projected_wall_entity" ) ) != NULL )
		{
			projectedWalls.AddToTail( dynamic_cast< CProjectedWallEntity* >( pEnt ) );
		}
		int nWalls = projectedWalls.Count();

		// restore painted projected wall
		int paintedWallCount = pRestore->ReadInt();
		for ( int i=0; i<paintedWallCount; ++i )
		{
			pRestore->StartBlock();
			{
				bool bHasSourcePortal;
				pRestore->ReadBool( &bHasSourcePortal );

				// find which wall to paint
				CProjectedWallEntity* pPaintedWall = NULL;
				if ( bHasSourcePortal )
				{
					EHANDLE hPortal;
					pRestore->ReadEHandle( &hPortal );
					CPortal_Base2D* pSourcePortal = dynamic_cast< CPortal_Base2D* >( hPortal.Get() );

					if ( pSourcePortal )
					{
						for ( int w=0; w<nWalls; ++w )
						{
							if ( projectedWalls[ w ]->GetSourcePortal() == pSourcePortal )
							{
								pPaintedWall = projectedWalls[ w ];
								break;
							}
						}
					}
				}
				else
				{
					EHANDLE hWall;
					pRestore->ReadEHandle( &hWall );
					pPaintedWall = dynamic_cast< CProjectedWallEntity* >( hWall.Get() );
				}

				Assert( pPaintedWall );

				int nPaintedSegments = pRestore->ReadInt();
				PaintPowerType type = NO_POWER;
				trace_t tr;
				tr.m_pEnt = pPaintedWall;
				for ( int s=0; s<nPaintedSegments; ++s )
				{
					type = (PaintPowerType)pRestore->ReadInt();
					pRestore->ReadVector( &tr.endpos, 1 );
#if 0 // Swarm Consistency, ugh. We need to change the AddPaint function eventually.
					PaintDatabase.AddPaint( tr, type );
#else
					const float flPaintRadius = ( type == NO_POWER ) ? sv_erase_surface_sphere_radius.GetFloat() : sv_paint_surface_sphere_radius.GetFloat();
					const float flAlphaPercent = sv_paint_alpha_coat.GetFloat();

					PaintDatabase.AddPaint( tr.m_pEnt, tr.endpos, tr.plane.normal, type, flPaintRadius, flAlphaPercent );
#endif
				}
			}
			pRestore->EndBlock();
		}

		projectedWalls.Purge();
#endif

#endif
	}

	virtual void PostRestore()
	{
	}
};

CPaintSaveRestoreBlockHandler g_PaintSaveRestoreBlockHandler;

ISaveRestoreBlockHandler *GetPaintSaveRestoreBlockHandler()
{
	return &g_PaintSaveRestoreBlockHandler;
}

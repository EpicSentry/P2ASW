//============ Copyright (c) Valve Corporation, All rights reserved. ============
//
// Purpose: Contains everything needed to create different gamestats tracking
//			systems.
//
//===============================================================================
#if !defined( PORTAL_GAMESTATS_SHARED_H ) && !defined( _GAMECONSOLE ) && !defined( NO_STEAM )
#define PORTAL_GAMESTATS_SHARED_H
#ifdef _WIN32
#pragma once
#endif
#include "cbase.h"
#include "tier1/utlvector.h"
#include "tier1/utldict.h"
#include "shareddefs.h"
#include "fmtstr.h"
#include "steamworks_gamestats.h"

//=============================================================================
//
// Helper functions for creating key values
//
void AddDataToKV( KeyValues* pKV, const char* name, int data );
void AddDataToKV( KeyValues* pKV, const char* name, uint64 data );
void AddDataToKV( KeyValues* pKV, const char* name, float data );
void AddDataToKV( KeyValues* pKV, const char* name, bool data );
void AddDataToKV( KeyValues* pKV, const char* name, const char* data );
void AddDataToKV( KeyValues* pKV, const char* name, const Color& data );
void AddDataToKV( KeyValues* pKV, const char* name, short data );
void AddDataToKV( KeyValues* pKV, const char* name, unsigned data );
void AddDataToKV( KeyValues* pKV, const char* name, const Vector& data );
void AddPositionDataToKV( KeyValues* pKV, const char* name, const Vector &data );
//=============================================================================

//=============================================================================
//
// Helper functions for creating key values from arrays
//
void AddArrayDataToKV( KeyValues* pKV, const char* name, const short *data, unsigned size );
void AddArrayDataToKV( KeyValues* pKV, const char* name, const byte *data, unsigned size );
void AddArrayDataToKV( KeyValues* pKV, const char* name, const unsigned *data, unsigned size );
void AddStringDataToKV( KeyValues* pKV, const char* name, const char *data );

//=============================================================================

// Macros to ease the creation of SendData method for stats structs/classes
#define BEGIN_STAT_TABLE( tableName ) \
	static const char* GetStatTableName( void ) { return tableName; } \
	void BuildGamestatDataTable( KeyValues* pKV ) \
{ \
	pKV->SetName( GetStatTableName() ); 

#define REGISTER_STAT( varName ) \
	AddDataToKV(pKV, #varName, varName);

#define REGISTER_STAT_NAMED( varName, dbName ) \
	AddDataToKV(pKV, dbName, varName);

#define REGISTER_STAT_POSITION( varName ) \
	AddPositionDataToKV(pKV, #varName, varName);

#define REGISTER_STAT_POSITION_NAMED( varName, dbName ) \
	AddPositionDataToKV(pKV, dbName, varName);

#define REGISTER_STAT_ARRAY( varName ) \
	AddArrayDataToKV( pKV, #varName, varName, ARRAYSIZE( varName ) );

#define REGISTER_STAT_ARRAY_NAMED( varName, dbName ) \
	AddArrayDataToKV( pKV, dbName, varName, ARRAYSIZE( varName ) );

#define REGISTER_STAT_STRING( varName ) \
	AddStringDataToKV( pKV, #varName, varName );

#define REGISTER_STAT_STRING_NAMED( varName, dbName ) \
	AddStringDataToKV( pKV, dbName, varName );

#define AUTO_STAT_TABLE_KEY() \
	pKV->SetInt( "TimeSubmitted", GetUniqueIDForStatTable( *this ) );

#define END_STAT_TABLE() \
	pKV->SetUint64( "TimeSubmitted", ::BaseStatData::TimeSubmitted ); \
	GetSteamWorksSGameStatsUploader().AddStatsForUpload( pKV ); \
}


//-----------------------------------------------------------------------------
// Purpose: Templatized class for getting unique ID's for stat tables that need
//			to be submitted multiple times per-session.
//-----------------------------------------------------------------------------
template < typename T >
class UniqueStatID_t
{
public:
	static unsigned GetNext( void )
	{
		return ++s_nLastID;
	}

	static void Reset( void )
	{
		s_nLastID = 0;
	}

private:
	static unsigned s_nLastID;
};

template < typename T >
unsigned UniqueStatID_t< T >::s_nLastID = 0;

template < typename T >
unsigned GetUniqueIDForStatTable( const T &table )
{
	return UniqueStatID_t< T >::GetNext();
}

//=============================================================================
//
// An interface for tracking gamestats.
//
class IGameStatTracker
{
public:

	//-----------------------------------------------------------------------------
	// Templatized methods to track a per-mission stat.
	// The stat is copied, then deleted after it's sent to the SQL server.
	//-----------------------------------------------------------------------------
	template < typename T >
	void SubmitStat( T& stat )
	{
		// Make a copy of the stat. All of the stat lists require pointers,
		// so we need to protect against a stat allocated on the stack
		T* pT = new T();
		if( !pT )
			return;

		*pT = stat;
		SubmitStat( pT );
	}

	//-----------------------------------------------------------------------------
	// Templatized methods to track a per-mission stat (by pointer)
	// The stat is deleted after it's sent to the SQL server
	//-----------------------------------------------------------------------------
	template < typename T >
	void SubmitStat( T* pStat )
	{
		// Get the static stat table for this type and add the stat to it
		GetStatTable<T>()->AddToTail( pStat );
	}

	//-----------------------------------------------------------------------------
	// Add all stats to an existing key value file for submit.
	//-----------------------------------------------------------------------------
	virtual void SubmitGameStats( KeyValues *pKV ) = 0;

	//-----------------------------------------------------------------------------
	// Prints the memory usage of all of the stats being tracked
	//-----------------------------------------------------------------------------
	void PrintGamestatMemoryUsage( void );

protected:
	//=============================================================================
	//
	// Used as a base interface to store a list of all templatized stat containers
	//
	class IStatContainer
	{
	public:
		virtual void SendData( KeyValues *pKV ) = 0;
		virtual void Clear( void ) = 0;
		virtual void PrintMemoryUsage( void ) = 0;
	};

	// Defines a list of stat containers.
	typedef CUtlVector< IStatContainer* > StatContainerList_t;

	//-----------------------------------------------------------------------------
	// Used to get a list of all stats containers being tracked by the deriving class
	//-----------------------------------------------------------------------------
	virtual StatContainerList_t* GetStatContainerList( void ) = 0;

private:

	//=============================================================================
	//
	// Templatized list of stats submitted
	//
	template < typename T >
	class CGameStatList : public IStatContainer, public CUtlVector< T* >
	{
	public:
		//-----------------------------------------------------------------------------
		// Get data ready to send to the SQL server
		//-----------------------------------------------------------------------------
		virtual void SendData( KeyValues *pKV )
		{
			//ASSERT( pKV != NULL );

			// Duplicate the master KeyValue for each stat instance
			for( int i=0; i < this->m_Size; ++i )
			{
				// Make a copy of the master key value and build the stat table
				KeyValues *pKVCopy = this->operator [](i)->m_bUseGlobalData ? pKV->MakeCopy() : new KeyValues( "" );
				this->operator [](i)->BuildGamestatDataTable( pKVCopy );
			}

			// Reset unique ID counter for the stat type
			UniqueStatID_t< T >::Reset();
		}

		//-----------------------------------------------------------------------------
		// Clear and delete every stat in this list
		//-----------------------------------------------------------------------------
		virtual void Clear( void )
		{
			this->PurgeAndDeleteElements();
		}

		//-----------------------------------------------------------------------------
		// Print out details about this lists memory usage
		//-----------------------------------------------------------------------------
		virtual void PrintMemoryUsage( void )
		{
			if( this->m_Size == 0 )
				return;

			// Compute the memory used as the size of type times the list count
			unsigned uMemoryUsed = this->m_Size * ( sizeof( T ) );

			Msg( CFmtStr( "	%d\tbytes used by %s table\n", uMemoryUsed, T::GetStatTableName() ) );
		}
	};

	//-----------------------------------------------------------------------------
	// Templatized method to get a single instance of a stat list per data type.
	//-----------------------------------------------------------------------------
	template < typename T >
	CGameStatList< T >* GetStatTable( void )
	{
		static CGameStatList< T > *s_vecOfType = 0;
		if( s_vecOfType == 0 )
		{
			s_vecOfType = new CGameStatList< T >();
			GetStatContainerList()->AddToTail( s_vecOfType );
		}
		return s_vecOfType;
	}

};

struct BaseStatData
{
	BaseStatData()
	{
		TimeSubmitted = 0; //GetSteamWorksSGameStatsUploader().GetTimeSinceEpoch();
		m_bUseGlobalData = true;
	}
	
	bool	m_bUseGlobalData;
	uint64	TimeSubmitted;

};
//=============================================================================
#endif // PORTAL_GAMESTATS_SHARED_H

//====== Copyright © 1996-2008, Valve Corporation, All rights reserved. =======
//
// Purpose: public interface to user remote file storage in Steam
//
//=============================================================================

#ifndef ISTEAMREMOTESTORAGE_H
#define ISTEAMREMOTESTORAGE_H
#ifdef _WIN32
#pragma once
#endif

#include "isteamclient.h"

#ifdef PORTAL2
// A handle to a piece of user generated content
typedef uint64 UGCHandle_t;
typedef uint64 PublishedFileUpdateHandle_t;
typedef uint64 PublishedFileId_t;
const PublishedFileId_t k_PublishedFileIdInvalid = 0;
const UGCHandle_t k_UGCHandleInvalid = 0xffffffffffffffffull;
const PublishedFileUpdateHandle_t k_PublishedFileUpdateHandleInvalid = 0xffffffffffffffffull;

// Handle for writing to Steam Cloud
typedef uint64 UGCFileWriteStreamHandle_t;
const UGCFileWriteStreamHandle_t k_UGCFileStreamHandleInvalid = 0xffffffffffffffffull;

const uint32 k_cchPublishedDocumentTitleMax = 128 + 1;
const uint32 k_cchPublishedDocumentDescriptionMax = 8000;
const uint32 k_cchPublishedDocumentChangeDescriptionMax = 8000;
const uint32 k_unEnumeratePublishedFilesMaxResults = 50;
const uint32 k_cchTagListMax = 1024 + 1;
const uint32 k_cchFilenameMax = 260;
const uint32 k_cchPublishedFileURLMax = 256;

enum ERemoteStoragePlatform
{
	k_ERemoteStoragePlatformNone = 0,
	k_ERemoteStoragePlatformWindows = (1 << 0),
	k_ERemoteStoragePlatformOSX = (1 << 1),
	k_ERemoteStoragePlatformPS3 = (1 << 2),
	k_ERemoteStoragePlatformLinux = (1 << 3),
	k_ERemoteStoragePlatformSwitch = (1 << 4),
	k_ERemoteStoragePlatformAndroid = (1 << 5),
	k_ERemoteStoragePlatformIOS = (1 << 6),
	// NB we get one more before we need to widen some things

	k_ERemoteStoragePlatformAll = 0xffffffff
};

enum ERemoteStoragePublishedFileVisibility
{
	k_ERemoteStoragePublishedFileVisibilityPublic = 0,
	k_ERemoteStoragePublishedFileVisibilityFriendsOnly = 1,
	k_ERemoteStoragePublishedFileVisibilityPrivate = 2,
	k_ERemoteStoragePublishedFileVisibilityUnlisted = 3,
};
#endif

//-----------------------------------------------------------------------------
// Purpose: Functions for accessing, reading and writing files stored remotely 
//			and cached locally
//-----------------------------------------------------------------------------
class ISteamRemoteStorage
{
	public:
		// NOTE
		//
		// Filenames are case-insensitive, and will be converted to lowercase automatically.
		// So "foo.bar" and "Foo.bar" are the same file, and if you write "Foo.bar" then
		// iterate the files, the filename returned will be "foo.bar".
		//

		// file operations
		virtual bool	FileWrite( const char *pchFile, const void *pvData, int32 cubData ) = 0;
		virtual int32	GetFileSize( const char *pchFile ) = 0;
		virtual int32	FileRead( const char *pchFile, void *pvData, int32 cubDataToRead ) = 0;
		virtual bool	FileExists( const char *pchFile ) = 0;

		// iteration
		virtual int32 GetFileCount() = 0;
		virtual const char *GetFileNameAndSize( int iFile, int32 *pnFileSizeInBytes ) = 0;

		// quota management
		virtual bool GetQuota( int32 *pnTotalBytes, int32 *puAvailableBytes ) = 0;
};

#define STEAMREMOTESTORAGE_INTERFACE_VERSION "STEAMREMOTESTORAGE_INTERFACE_VERSION002"

#endif // ISTEAMREMOTESTORAGE_H

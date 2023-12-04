//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//===========================================================================//
#include "cbase.h"
#include "vpklib/packedstore.h"
#include "packedstore_internal.h"
#include "tier1/utlintrusivelist.h"
#include "tier1/generichash.h"
#include "tier1/checksum_crc.h"
#include "tier1/checksum_md5.h"
#include "tier1/utldict.h"
#include "tier2/fileutils.h"
#include "tier1/utlbuffer.h"

#ifdef VPK_ENABLE_SIGNING
#include "crypto.h"
#endif


#ifdef IS_WINDOWS_PC
#include <windows.h>
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


typedef uint16 PackFileIndex_t;
#define PACKFILEINDEX_END 0xffff


#pragma pack(1)
struct CFilePartDescr
{
	PackFileIndex_t m_nFileNumber;
	uint32 m_nFileDataOffset;
	uint32 m_nFileDataSize;
};


struct CFileHeaderFixedData
{
	uint32 m_nFileCRC;
	uint16 m_nMetaDataSize;
	CFilePartDescr m_PartDescriptors[1];					// variable length

	FORCEINLINE const void *MetaData(void) const;

	FORCEINLINE const CFilePartDescr *FileData(int nPart = 0) const;

	uint32 TotalDataSize(void) const
	{
		return m_nMetaDataSize + m_PartDescriptors[0].m_nFileDataSize;
	}

	size_t HeaderSizeIncludingMetaData(void) const
	{
		size_t nRet = sizeof(*this) - sizeof(m_PartDescriptors)+m_nMetaDataSize;
		// see how many parts we have and count the size of their descriptors
		CFilePartDescr const *pPart = m_PartDescriptors;
		while (pPart->m_nFileNumber != PACKFILEINDEX_END)
		{
			nRet += sizeof(CFilePartDescr);
			pPart++;
		}
		nRet += sizeof(PackFileIndex_t);					// count terminator
		return nRet;
	}

};
#pragma pack()

#define PACKEDFILE_DIR_HASH_SIZE 43

static int s_FileHeaderSize(char const *pName, int nNumDataParts, int nNumMetaDataBytes)
{
	return 1 + strlen(pName) + 							// name plus nul
		sizeof(uint32)+									// file crc
		sizeof(uint16)+ 									// meta data size
		nNumMetaDataBytes +									// metadata
		nNumDataParts * sizeof(CFilePartDescr)+			// part data
		sizeof(PackFileIndex_t);							// part data 0xff end marker
}

class CFileDirectoryData
{
public:
	CFileDirectoryData *m_pNext;
	char const *m_Name;

};



// hash chain for accelerating file lookups. We can find an extension by hash, and find the
// directories containing files with this extension by another hash
class CFileExtensionData
{
public:
	CFileExtensionData *m_pNext;							// next one that has the same hash
	char const *m_Name;						// points at extension string within the directory data
	// nodes for each directory containing a file of this type
	CUtlIntrusiveList<CFileDirectoryData> m_pDirectoryHashTable[PACKEDFILE_DIR_HASH_SIZE];

	~CFileExtensionData(void)
	{
		for (int i = 0; i < ARRAYSIZE(m_pDirectoryHashTable); i++)
		{
			m_pDirectoryHashTable[i].Purge();
		}
	}

};


static int SkipFile(char const * &pData)					// returns highest file index
{
	int nHighestChunkIndex = -1;
	pData += 1 + V_strlen(pData);
	pData += sizeof(uint32);
	int nMetaDataSize = *(reinterpret_cast<uint16 const *>(pData));
	pData += sizeof(uint16);
	while (*((PackFileIndex_t const *)pData) != PACKFILEINDEX_END)
	{
		int nIdx = reinterpret_cast<CFilePartDescr const *>(pData)->m_nFileNumber;

		if (nIdx != VPKFILENUMBER_EMBEDDED_IN_DIR_FILE)
			nHighestChunkIndex = MAX(nHighestChunkIndex, nIdx);
		pData += sizeof(CFilePartDescr);
	}
	pData += sizeof(PackFileIndex_t);
	pData += nMetaDataSize;
	return nHighestChunkIndex;
}

static inline int SkipAllFilesInDir(char const * & pData)
{
	int nHighestChunkIndex = -1;
	pData += 1 + strlen(pData);					// skip dir name
	// now, march through all the files
	while (*pData)									// until we're out of files to look at
	{
		int nSkipIndex = SkipFile(pData);
		nHighestChunkIndex = MAX(nHighestChunkIndex, nSkipIndex);
	}
	pData++;												// skip end marker
	return nHighestChunkIndex;
}


CFileHeaderFixedData *CPackedStore::FindFileEntry(char const *pDirname, char const *pBaseName, char const *pExtension, uint8 **pExtBaseOut, uint8 **pNameBaseOut)
{
	if (pExtBaseOut)
		*pExtBaseOut = NULL;
	if (pNameBaseOut)
		*pNameBaseOut = NULL;

	int nExtensionHash = HashString(pExtension) % PACKEDFILE_EXT_HASH_SIZE;
	CFileExtensionData const *pExt = m_pExtensionData[nExtensionHash].FindNamedNodeCaseSensitive(pExtension);
	if (pExt)
	{
		int nDirHash = HashString(pDirname) % PACKEDFILE_DIR_HASH_SIZE;
		CFileDirectoryData const *pDir = pExt->m_pDirectoryHashTable[nDirHash].FindNamedNodeCaseSensitive(pDirname);
		if (pDir)
		{
			if (pExtBaseOut)
				*pExtBaseOut = (uint8 *)pDir;
			// we found the right directory. now, sequential search. data is heavily packed, so
			// this is a little awkward. See fileformat.txt
			char const *pData = pDir->m_Name;
			pData += 1 + strlen(pData);					// skip dir name
			// now, march through all the files
			while (*pData)									// until we're out of files to look at
			{
				if (!V_strcmp(pData, pBaseName))		// found it?
				{
					if (pNameBaseOut)
						*pNameBaseOut = (uint8 *)pData;

					return (CFileHeaderFixedData *)(pData + 1 + V_strlen(pData)); // return header
				}
				// this isn't it - skip over it
				SkipFile(pData);
			}
		}
	}
	return NULL;
}


const void *CFileHeaderFixedData::MetaData(void) const
{
	if (!m_nMetaDataSize)
		return NULL;
	const CFilePartDescr *ret = &(m_PartDescriptors[0]);
	while (ret->m_nFileNumber != PACKFILEINDEX_END)
		ret++;
	return reinterpret_cast<uint8 const *>(ret)+sizeof(PackFileIndex_t);
}

CFilePartDescr const *CFileHeaderFixedData::FileData(int nPart) const
{
	return m_PartDescriptors + nPart;
}

void CPackedStore::Init(void)
{
	m_nHighestChunkFileIndex = -1;
	m_bUseDirFile = false;
	m_pszFileBaseName[0] = 0;
	m_pszFullPathName[0] = 0;
	memset(m_pExtensionData, 0, sizeof(m_pExtensionData));
	m_nDirectoryDataSize = 0;
	m_nWriteChunkSize = k_nVPKDefaultChunkSize;

	m_nSizeOfSignedData = 0;
	m_Signature.Purge();
	m_SignaturePrivateKey.Purge();
	m_SignaturePublicKey.Purge();
}

void CPackedStore::BuildHashTables(void)
{
	m_nHighestChunkFileIndex = -1;
	for (int i = 0; i < ARRAYSIZE(m_pExtensionData); i++)
	{
		m_pExtensionData[i].Purge();
	}
	char const *pData = reinterpret_cast< char const *>(DirectoryData());
	while (*pData)
	{
		// for each extension
		int nExtensionHash = HashString(pData) % PACKEDFILE_EXT_HASH_SIZE;
		CFileExtensionData *pNewExt = new CFileExtensionData;
		pNewExt->m_Name = pData;
		m_pExtensionData[nExtensionHash].AddToHead(pNewExt);
		// now, iterate over all directories associated with this extension
		pData += 1 + strlen(pData);
		while (*pData)
		{
			int nDirHash = HashString(pData) % PACKEDFILE_DIR_HASH_SIZE;
			CFileDirectoryData *pNewDir = new CFileDirectoryData;
			pNewDir->m_Name = pData;
			pNewExt->m_pDirectoryHashTable[nDirHash].AddToHead(pNewDir);
			int nDirChunk = SkipAllFilesInDir(pData);
			m_nHighestChunkFileIndex = MAX(m_nHighestChunkFileIndex, nDirChunk);
		}
		// step past \0
		pData++;
	}
}


bool CPackedStore::IsEmpty(void) const
{
	return (m_DirectoryData.Count() <= 1);
}
#define INOUT_Z_CAP(x) _Inout_z_cap_(x)

#define COPY_ALL_CHARACTERS -1
template <size_t cchDest> char* V_strcat_safe(_Deref_prepost_z_ char(&pDest)[cchDest], const char* pSrc, int nMaxCharsToCopy = COPY_ALL_CHARACTERS)
{
	return V_strncat(pDest, pSrc, (int)cchDest, nMaxCharsToCopy);
};
static void StripTrailingString(char *pszBuf, const char *pszStrip)
{
	int lBuf = V_strlen(pszBuf);
	int lStrip = V_strlen(pszStrip);
	if (lBuf < lStrip)
		return;
	char *pExpectedPos = pszBuf + lBuf - lStrip;
	if (V_stricmp(pExpectedPos, pszStrip) == 0)
		*pExpectedPos = '\0';
}

CPackedStore::CPackedStore(char const* pFileBasename, IBaseFileSystem* pFS, bool bOpenForWrite) :m_PackedStoreReadCache(pFS)
{
	Init();
	m_pFileSystem = pFS;
	m_PackedStoreReadCache.m_pPackedStore = this;
	m_DirectoryData.AddToTail(0);
	char pszFName[260];
	if (pFileBasename)
	{
		V_strcpy(m_pszFileBaseName, pFileBasename);
		StripTrailingString(m_pszFileBaseName, ".vpk");
		StripTrailingString(m_pszFileBaseName, "_dir");
		sprintf(pszFName, "%s_dir.vpk", m_pszFileBaseName);
#ifdef _WIN32
		Q_strlower(pszFName);
#endif
		CInputFile dirFile(pszFName);

		// Try to load the VPK as a standalone (probably an addon) even if the standard _dir name is not present
		if (dirFile.IsOk())
		{
			m_bUseDirFile = true;
		}
		else
		{
			m_bUseDirFile = false;
			sprintf(pszFName, "%s.vpk", m_pszFileBaseName);
			dirFile.Open(pszFName);
		}

		bool bNewFileFormat = false;
		if (dirFile.IsOk())
		{
			// first, check if it is the new versioned variant
			VPKDirHeader_t dirHeader;
			// try to read the header.
			if (
				(dirFile.Read(&dirHeader, sizeof(dirHeader)) == sizeof(dirHeader)) &&
				(dirHeader.m_nHeaderMarker == VPK_HEADER_MARKER))
			{
				if (dirHeader.m_nVersion == VPK_PREVIOUS_VERSION)
				{
					// fill in the fields of the new header.
					dirHeader.m_nEmbeddedChunkSize = dirFile.Size() - dirHeader.m_nDirectorySize - sizeof(VPKDirHeaderOld_t);
					dirHeader.m_nChunkHashesSize = 0;
					dirHeader.m_nSelfHashesSize = 0;
					dirHeader.m_nSignatureSize = 0;
					// pretend we didnt read the extra header
					dirFile.Seek(sizeof(VPKDirHeaderOld_t));
				}
				else if (dirHeader.m_nVersion != VPK_CURRENT_VERSION)
				{
					Error("Unknown version %d for vpk %s", dirHeader.m_nVersion, pFileBasename);
				}
				bNewFileFormat = true;
			}
			else											// its an old file
			{
				dirFile.Seek(0);
				// fill in a fake header, zero out garbage we read
				dirHeader.m_nDirectorySize = dirFile.Size();
				dirHeader.m_nEmbeddedChunkSize = 0;
				dirHeader.m_nChunkHashesSize = 0;
				dirHeader.m_nSelfHashesSize = 0;
				dirHeader.m_nSignatureSize = 0;
			}
			uint32 nSizeOfHeader = dirFile.Tell();
			int nSize = dirHeader.m_nDirectorySize;
			m_nDirectoryDataSize = dirHeader.m_nDirectorySize;
			m_DirectoryData.SetCount(nSize);
			dirFile.MustRead(DirectoryData(), nSize);
			// now, if we are opening for write, read the entire contents of the embedded data chunk in the dir into ram
			if (bOpenForWrite && bNewFileFormat)
			{
				if (dirHeader.m_nEmbeddedChunkSize)
				{
					CUtlVector<uint8> readBuffer;
					int nRemainingSize = dirHeader.m_nEmbeddedChunkSize;

					m_EmbeddedChunkData.EnsureCapacity(dirHeader.m_nEmbeddedChunkSize);

					// We'll allocate around half a meg of contiguous memory for the read. Any more and the SDK's VPK 
					// utility has a higher chance of choking on low-end machines.
					readBuffer.SetCount(524288);

					while (nRemainingSize > 0)
					{
						int nReadSize = MIN(nRemainingSize, 524288);

						dirFile.MustRead(readBuffer.Base(), nReadSize);

						for (int i = 0; i < nReadSize; i++)
						{
							m_EmbeddedChunkData.AddToTail(readBuffer[i]);
						}
						nRemainingSize -= nReadSize;
					}
				}
			}
			int cbVecHashes = dirHeader.m_nChunkHashesSize;
			int ctHashes = cbVecHashes / sizeof(m_vecChunkHashFraction[0]);
			m_vecChunkHashFraction.EnsureCount(ctHashes);
			dirFile.MustRead(m_vecChunkHashFraction.Base(), cbVecHashes);
			FOR_EACH_VEC(m_vecChunkHashFraction, i)
			{
				int idxFound = m_vecChunkHashFraction.Find(m_vecChunkHashFraction[i]);
				Assert(idxFound == i); idxFound;
			}

			// now read the self hashes
			V_memset(m_DirectoryMD5.bits, 0, sizeof(m_DirectoryMD5.bits));
			V_memset(m_ChunkHashesMD5.bits, 0, sizeof(m_ChunkHashesMD5.bits));
			V_memset(m_TotalFileMD5.bits, 0, sizeof(m_TotalFileMD5.bits));
			if (dirHeader.m_nSelfHashesSize == 3 * sizeof(m_DirectoryMD5.bits))
			{
				// first is an MD5 of directory data
				dirFile.MustRead(m_DirectoryMD5.bits, sizeof(m_DirectoryMD5.bits));
				// next is an MD5 of 
				dirFile.MustRead(m_ChunkHashesMD5.bits, sizeof(m_ChunkHashesMD5.bits));
				// at this point the filesystem has calculated an MD5 of everything in the file up to this point.
				// we could ask it for a snapshot of that MD5 value and then be able to compare it to m_TotalFileMD5
				// but we would have to do it *before* we read it
				dirFile.MustRead(m_TotalFileMD5.bits, sizeof(m_TotalFileMD5.bits));
			}

			// Is there a signature?
			m_nSizeOfSignedData = 0;
			if (dirHeader.m_nSignatureSize != 0)
			{

				// Everything immediately proceeding it should have been signed.
				m_nSizeOfSignedData = dirFile.Tell();
				uint32 nExpectedSignedSize = nSizeOfHeader + dirHeader.ComputeSizeofSignedDataAfterHeader();
				if (m_nSizeOfSignedData != nExpectedSignedSize)
				{
					Error("Size mismatch determining size of signed data block (%d vs %d)", m_nSizeOfSignedData, nExpectedSignedSize);
				}

				// Read the public key
				uint32 cubPublicKey = 0;
				dirFile.MustRead(&cubPublicKey, sizeof(cubPublicKey));
				m_SignaturePublicKey.SetCount(cubPublicKey);
				dirFile.MustRead(m_SignaturePublicKey.Base(), cubPublicKey);

				// Read the private key
				uint32 cubSignature = 0;
				dirFile.MustRead(&cubSignature, sizeof(cubSignature));
				m_Signature.SetCount(cubSignature);
				dirFile.MustRead(m_Signature.Base(), cubSignature);
			}
		}
		Q_MakeAbsolutePath(m_pszFullPathName, sizeof(m_pszFullPathName), m_pszFileBaseName);
		V_strcat_safe(m_pszFullPathName, ".vpk");
		//Q_strlower( m_pszFullPathName ); // NO!  this screws up linux.
		Q_FixSlashes(m_pszFullPathName);
	}
	BuildHashTables();
}




void CPackedStore::DPackedStore(void)
{
	for (int i = 0; i < ARRAYSIZE(m_pExtensionData); i++)
	{
		m_pExtensionData[i].Purge();
	}

	for (int i = 0; i < ARRAYSIZE(m_FileHandles); i++)
	{
		if (m_FileHandles[i].m_nFileNumber != -1)
		{
#ifdef IS_WINDOWS_PC
			CloseHandle(m_FileHandles[i].m_hFileHandle);
#else
			m_pFileSystem->Close(m_FileHandles[i].m_hFileHandle);
#endif

		}
	}

	// Free the FindFirst cache data
	m_directoryList.PurgeAndDeleteElements();

	FOR_EACH_MAP(m_dirContents, i)
	{
		m_dirContents[i]->PurgeAndDeleteElements();
		delete m_dirContents[i];
	}
}
CPackedStore::~CPackedStore(void)
{
	for (int i = 0; i < ARRAYSIZE(m_pExtensionData); i++)
	{
		m_pExtensionData[i].Purge();
	}

	for (int i = 0; i < ARRAYSIZE(m_FileHandles); i++)
	{
		if (m_FileHandles[i].m_nFileNumber != -1)
		{
#ifdef IS_WINDOWS_PC
			CloseHandle(m_FileHandles[i].m_hFileHandle);
#else
			m_pFileSystem->Close(m_FileHandles[i].m_hFileHandle);
#endif

		}
	}

	// Free the FindFirst cache data
	m_directoryList.PurgeAndDeleteElements();

	FOR_EACH_MAP(m_dirContents, i)
	{
		m_dirContents[i]->PurgeAndDeleteElements();
		delete m_dirContents[i];
	}
}

void SplitFileComponents(char const *pFileName, char *pDirOut, char *pBaseOut, char *pExtOut)
{
	char pTmpDirOut[MAX_PATH];
	V_ExtractFilePath(pFileName, pTmpDirOut, MAX_PATH);
	// now, pTmpDirOut to pDirOut, except when we find more then one '\' in a row, only output one
	char *pOutDirPtr = pDirOut;
	for (char *pDirInPtr = pTmpDirOut; *pDirInPtr; pDirInPtr++)
	{
		char c = *(pDirInPtr);
		*(pOutDirPtr++) = c;
		// if we copied a \, skip all subsequent slashes
		while ((c == '\\') && (pDirInPtr[1] == c))
		{
			pDirInPtr++;
		}
	}
	*(pOutDirPtr) = 0;									// null terminate

	if (!pDirOut[0])
		strcpy(pDirOut, " ");								// blank dir name
	V_strcpy(pBaseOut, V_UnqualifiedFileName(pFileName));
	char *pDot = strrchr(pBaseOut, '.');
	if (pDot)
	{
		*pDot = 0;
		V_strncpy(pExtOut, pDot + 1, MAX_PATH);
	}
	else
	{
		pExtOut[0] = ' ';
		pExtOut[1] = 0;
	}

	V_FixSlashes(pDirOut, '/');
	V_strlower(pDirOut);

	// the game sometimes asks for paths like dir1/../dir2/ we will replace this with dir2/. This
	// one line of perl code sucks in c++.
	for (;;)
	{
		char *pDotDot = V_strstr(pDirOut + 1, "/../");		// start at second char. we don't want a beginning /
		if (!pDotDot)
		{
			break;
		}
		// search backwards from the /.. for the previous directory part
		char *pPrevSlash = pDotDot - 1;
		while ((pPrevSlash > pDirOut) && (pPrevSlash[0] != '/'))
		{
			pPrevSlash--;
		}
		// if our path was dir0/dir1/../dir2, we are now pointing at "/dir1".
		// is strmove in all compilers? that would be better than this loop
		char *pStrIn = pDotDot + 3;
		for (;;)
		{
			*pPrevSlash = *pStrIn;
			if (pStrIn[0])
			{
				++pPrevSlash;
				++pStrIn;
			}
			else
			{
				break;
			}
		}
	}


	char *pLastDirChar = pDirOut + strlen(pDirOut) - 1;
	if ((pLastDirChar[0] == '/') || (pLastDirChar[0] == '\\'))
		*pLastDirChar = 0;									// kill trailing slash
	V_strlower(pBaseOut);
	V_strlower(pExtOut);
}
void V_RemoveFirstExtension(char* path)
{
	int len = strlen(path);
	char* src = path + len - 1;

	// scan backwards for the first dot
	while (src != path && *src != '.')
	{
		src--;
	}

	// if we didn't find a dot, then there's no extension to remove
	if (src == path)
	{
		return;
	}

	// scan backwards for the second dot
	char* src2 = src - 1;
	while (src2 != path && *src2 != '.')
	{
		src2--;
	}

	// if we didn't find a second dot, then there's only one extension, return NULL
	if (src2 == path)
	{
		return;
	}

	// we found two dots, so remove the first extension by moving the second extension over it
	memmove(src2, src, strlen(src) + 1);
}


bool recursive = false;


CPackedStoreFileHandle CPackedStore::OpenFile(char const *pFileName)
{
	char dirName[MAX_PATH];
	char baseName[MAX_PATH];
	char extName[MAX_PATH];

	// Fix up the filename first
	char tempFileName[MAX_PATH];

	V_strncpy(tempFileName, pFileName, sizeof(tempFileName));
	V_FixSlashes(tempFileName, CORRECT_PATH_SEPARATOR);
	//	V_RemoveDotSlashes( tempFileName, CORRECT_PATH_SEPARATOR, true );
	V_FixDoubleSlashes(tempFileName);
	if (!V_IsAbsolutePath(tempFileName))
	{
		V_strlower(tempFileName);
	}

	SplitFileComponents(tempFileName, dirName, baseName, extName);

	CPackedStoreFileHandle ret;

	CFileHeaderFixedData *pHeader = FindFileEntry(dirName, baseName, extName, NULL, &(ret.m_pDirFileNamePtr));

	if (pHeader)
	{
		ret.m_nFileNumber = pHeader->m_PartDescriptors[0].m_nFileNumber;
		ret.m_nFileOffset = pHeader->m_PartDescriptors[0].m_nFileDataOffset;
		ret.m_nFileSize = pHeader->m_PartDescriptors[0].m_nFileDataSize + pHeader->m_nMetaDataSize;
		ret.m_nCurrentFileOffset = 0;
		ret.m_pMetaData = pHeader->MetaData();
		ret.m_nMetaDataSize = pHeader->m_nMetaDataSize;
		ret.m_pHeaderData = pHeader;
		ret.m_pOwner = this;
		//	Msg("CPackedStore::OpenFile: %s found, dirName %s, baseName %s, extName %s\n", pFileName, dirName, baseName, extName);
	}
	else
	{
		ret.m_nFileNumber = -1;
		ret.m_pOwner = NULL;
		//Warning("CPackedStore::OpenFile: %s NOT found, dirName %s, baseName %s, extName %s\n", pFileName, dirName, baseName, extName);
		if (!recursive) {
			recursive = true;
			if (V_strlen(pFileName) > 0)
				V_RemoveFirstExtension((char*)pFileName);
			OpenFile(pFileName);
		}
	}
	recursive = false;
	return ret;

}


void CPackedStore::Write(void)
{
	// !KLUDGE!
	//// Write the whole header into a buffer in memory.
	//// We do this so we can easily sign it.
	//CUtlBuffer bufDirFile;
	//
	//VPKDirHeader_t headerOut;
	//headerOut.m_nDirectorySize = m_DirectoryData.Count();
	//headerOut.m_nEmbeddedChunkSize = m_EmbeddedChunkData.Count();
	//headerOut.m_nChunkHashesSize = m_vecChunkHashFraction.Count()*sizeof(m_vecChunkHashFraction[0]);
	//headerOut.m_nSelfHashesSize = 3*sizeof(m_DirectoryMD5.bits);
	//headerOut.m_nSignatureSize = 0;
	//
	//// Do we plan on signing this thing and writing a signature?
	//m_Signature.Purge();
	//uint32 nExpectedSignatureSize = 0;
	//if ( m_SignaturePrivateKey.Count() > 0 && m_SignaturePublicKey.Count() > 0 )
	//{
	//	#ifdef VPK_ENABLE_SIGNING
	//		nExpectedSignatureSize = k_cubRSASignature;
	//		headerOut.m_nSignatureSize = sizeof(uint32) + m_SignaturePublicKey.Count() + sizeof(uint32) + nExpectedSignatureSize;
	//	#else
	//		Error( "VPK signing not implemented" );
	//	#endif
	//}
	//
	//bufDirFile.Put( &headerOut, sizeof( headerOut ) );
	//bufDirFile.Put( DirectoryData(), m_DirectoryData.Count() );
	//
	//if ( m_EmbeddedChunkData.Count() )
	//{
	//	int nRemainingSize = m_EmbeddedChunkData.Count();
	//	CUtlVector<uint8> writeBuffer;
	//	
	//	writeBuffer.SetCount( 524288 );
	//	int nChunkOffset = 0;
	//
	//	while ( nRemainingSize > 0 )
	//	{
	//		// We'll write around half a meg of contiguous memory at once. Any more and the SDK's VPK 
	//		// utility has a higher chance of choking on low-end machines.
	//		int nWriteSize = MIN( nRemainingSize, 524288 );
	//		
	//		for ( int i = 0; i < nWriteSize; i++ )
	//		{
	//			writeBuffer[i] = m_EmbeddedChunkData[nChunkOffset++];
	//		}
	//
	//		bufDirFile.Put( writeBuffer.Base(), nWriteSize );
	//		nRemainingSize -= nWriteSize;
	//	}
	//}
	//
	//// write the chunk hashes out
	//bufDirFile.Put( m_vecChunkHashFraction.Base(), m_vecChunkHashFraction.Count()*sizeof(m_vecChunkHashFraction[0]) );
	//
	//// write out the MD5s of the 2 main pieces of data
	//bufDirFile.Put( m_DirectoryMD5.bits, sizeof( m_DirectoryMD5.bits ) );
	//bufDirFile.Put( m_ChunkHashesMD5.bits, sizeof( m_ChunkHashesMD5.bits ) );
	//
	//// compute the final MD5 ( of everything in the file up to this point )
	//MD5_ProcessSingleBuffer( bufDirFile.Base(), bufDirFile.TellPut(), m_TotalFileMD5 );
	//bufDirFile.Put( m_TotalFileMD5.bits, sizeof( m_TotalFileMD5.bits ) );
	//
	//// Should we sign all this stuff?
	//m_nSizeOfSignedData = 0;
	//#ifdef VPK_ENABLE_SIGNING
	//	if ( headerOut.m_nSignatureSize > 0 )
	//	{
	//
	//		m_nSizeOfSignedData = bufDirFile.TellPut();
	//		uint32 nExpectedSignedSize = sizeof(headerOut) + headerOut.ComputeSizeofSignedDataAfterHeader();
	//		if ( m_nSizeOfSignedData != nExpectedSignedSize )
	//		{
	//			Error( "Size mismatch determining size of signed data block (%d vs %d)", m_nSizeOfSignedData, nExpectedSignedSize );
	//		}
	//
	//		// Allocate more than enough space to hold the signature
	//		m_Signature.SetCount( nExpectedSignatureSize + 1024 );
	//
	//		// Calcuate the signature
	//		uint32 cubSignature = m_Signature.Count();
	//		if ( !CCrypto::RSASignSHA256( (const uint8 *)bufDirFile.Base(), bufDirFile.TellPut(), 
	//			(uint8 *)m_Signature.Base(), &cubSignature, 
	//			(const uint8 *)m_SignaturePrivateKey.Base(), m_SignaturePrivateKey.Count() ) )
	//		{
	//			Error( "VPK signing failed.  Private key may be corrupt or invalid" );
	//		}
	//
	//		// Confirm that the size was what we expected
	//		if ( cubSignature != nExpectedSignatureSize )
	//		{
	//			Error( "VPK signing produced %d byte signature.  Expected size was %d bytes", cubSignature, nExpectedSignatureSize );
	//		}
	//
	//		// Shrink signature to fit
	//		m_Signature.SetCountNonDestructively( cubSignature );
	//
	//		// Now re-check the signature, using the public key that we are about
	//		// to burn into the file, to make sure there's no mismatch.
	//		if ( !CCrypto::RSAVerifySignatureSHA256( (const uint8 *)bufDirFile.Base(), bufDirFile.TellPut(), 
	//			(const uint8 *)m_Signature.Base(), cubSignature, 
	//			(const uint8 *)m_SignaturePublicKey.Base(), m_SignaturePublicKey.Count() ) )
	//		{
	//			Error( "VPK signature verification failed immediately after signing.  The public key might be invalid, or might not match the private key used to generate the signature." );
	//		}
	//
	//		// Write public key which should be used
	//		uint32 cubPublicKey = m_SignaturePublicKey.Count();
	//		bufDirFile.Put( &cubPublicKey, sizeof(cubPublicKey) );
	//		bufDirFile.Put( m_SignaturePublicKey.Base(), cubPublicKey );
	//
	//		// Write signature
	//		bufDirFile.Put( &cubSignature, sizeof(cubSignature) );
	//		bufDirFile.Put( m_Signature.Base(), cubSignature );
	//	}
	//#endif
	//
	//char szOutFileName[MAX_PATH];
	//
	//// Delete any existing header file, either the standalone kind,
	//// or the _dir kind.
	//V_sprintf_safe( szOutFileName, "%s.vpk", m_pszFileBaseName );
	//if ( g_pFullFileSystem->FileExists( szOutFileName ) )
	//	g_pFullFileSystem->RemoveFile( szOutFileName );
	//V_sprintf_safe( szOutFileName, "%s_dir.vpk", m_pszFileBaseName );
	//if ( g_pFullFileSystem->FileExists( szOutFileName ) )
	//	g_pFullFileSystem->RemoveFile( szOutFileName );
	//
	//// Force on multi-chunk mode if we have any files in a chunk
	//if ( m_nHighestChunkFileIndex >= 0 )
	//	m_bUseDirFile = true;
	//
	//
	//// Fetch actual name to write
	//GetDataFileName( szOutFileName, sizeof(szOutFileName), VPKFILENUMBER_EMBEDDED_IN_DIR_FILE );
	//
	//// Now actually write the data to disk
	//COutputFile dirFile( szOutFileName );
	//dirFile.Write( bufDirFile.Base(), bufDirFile.TellPut() );
	//dirFile.Close();
}

#ifdef VPK_ENABLE_SIGNING

void CPackedStore::SetKeysForSigning(int nPrivateKeySize, const void *pPrivateKeyData, int nPublicKeySize, const void *pPublicKeyData)
{
	m_SignaturePrivateKey.SetSize(nPrivateKeySize);
	V_memcpy(m_SignaturePrivateKey.Base(), pPrivateKeyData, nPrivateKeySize);

	m_SignaturePublicKey.SetSize(nPublicKeySize);
	V_memcpy(m_SignaturePublicKey.Base(), pPublicKeyData, nPublicKeySize);

	// Discard any existing signature
	m_Signature.Purge();
}

CPackedStore::ESignatureCheckResult CPackedStore::CheckSignature(int nSignatureSize, const void *pSignature) const
{
	if (m_Signature.Count() == 0)
		return eSignatureCheckResult_NotSigned;

	Assert(m_nSizeOfSignedData > 0);

	// Confirm correct public key, if they specified one.
	if (nSignatureSize > 0 && pSignature != NULL)
	{
		if (m_SignaturePublicKey.Count() != nSignatureSize || V_memcmp(pSignature, m_SignaturePublicKey.Base(), nSignatureSize) != 0)
		{
			return eSignatureCheckResult_WrongKey;
		}
	}

	char szFilename[MAX_PATH];
	GetDataFileName(szFilename, sizeof(szFilename), VPKFILENUMBER_EMBEDDED_IN_DIR_FILE);

	// Read the data
	CUtlBuffer bufSignedData;
	if (!g_pFullFileSystem->ReadFile(szFilename, NULL, bufSignedData, m_nSizeOfSignedData))
		return eSignatureCheckResult_Failed;
	if (bufSignedData.TellPut() < (int)m_nSizeOfSignedData)
	{
		Assert(false); // ?
		return eSignatureCheckResult_Failed;
	}

	// Check the signature
	if (!CCrypto::RSAVerifySignatureSHA256((const uint8 *)bufSignedData.Base(), m_nSizeOfSignedData,
		(const uint8 *)m_Signature.Base(), m_Signature.Count(),
		(const uint8 *)m_SignaturePublicKey.Base(), m_SignaturePublicKey.Count()))
	{
		return eSignatureCheckResult_InvalidSignature;
	}

	return eSignatureCheckResult_ValidSignature;
}

#endif

CPackedStoreReadCache::CPackedStoreReadCache(IBaseFileSystem *pFS) :m_treeCachedVPKRead(CachedVPKRead_t::Less)
{
	m_pPackedStore = NULL;
	m_cItemsInCache = 0;
	m_pFileSystem = pFS;
	m_cubReadFromCache = 0;
	m_cReadFromCache = 0;
	m_cDiscardsFromCache = 0;
	m_cAddedToCache = 0;
	m_cCacheMiss = 0;
	m_cubCacheMiss = 0;
	m_cFileErrors = 0;
	m_cFileErrorsCorrected = 0;
	m_cFileResultsDifferent = 0;
	m_pFileTracker = NULL;
}

// check if the read request can be satisfied from the read cache we have in 1MB chunks
bool CPackedStoreReadCache::BCanSatisfyFromReadCache(uint8 *pOutData, CPackedStoreFileHandle &handle, FileHandleTracker_t &fHandle, int nDesiredPos, int nNumBytes, int &nRead)
{
	return false;
	nRead = 0;
	int nFileFraction = nDesiredPos & k_nCacheBufferMask;
	int nOffset = nDesiredPos - nFileFraction;
	int cubReadChunk = nOffset + nNumBytes;
	if (cubReadChunk > k_cubCacheBufferSize)
		cubReadChunk = (k_nCacheBufferMask - nOffset) & (k_cubCacheBufferSize - 1);
	else
		cubReadChunk = nNumBytes;
	// the request might straddle multiple chunks - we make sure we have all of the data, if we are missing any, we fail
	while (nNumBytes)
	{
		int nReadChunk = 0;
		if (!BCanSatisfyFromReadCacheInternal(pOutData, handle, fHandle, nDesiredPos, cubReadChunk, nReadChunk))
		{
			return false;
		}
		nNumBytes -= cubReadChunk;
		pOutData += cubReadChunk;
		nDesiredPos += cubReadChunk;
		nRead += nReadChunk;
		nFileFraction += k_cubCacheBufferSize;
		cubReadChunk = nNumBytes;
		if (cubReadChunk > k_cubCacheBufferSize)
			cubReadChunk = k_cubCacheBufferSize;
	}
	return true;
}


// read a single line into the cache
bool CPackedStoreReadCache::ReadCacheLine(FileHandleTracker_t &fHandle, CachedVPKRead_t &cachedVPKRead)
{
	cachedVPKRead.m_cubBuffer = 0;
#ifdef IS_WINDOWS_PC
	if (cachedVPKRead.m_nFileFraction != fHandle.m_nCurOfs)
		SetFilePointer(fHandle.m_hFileHandle, cachedVPKRead.m_nFileFraction, NULL, FILE_BEGIN);
	ReadFile(fHandle.m_hFileHandle, cachedVPKRead.m_pubBuffer, k_cubCacheBufferSize, (LPDWORD)&cachedVPKRead.m_cubBuffer, NULL);
	SetFilePointer(fHandle.m_hFileHandle, fHandle.m_nCurOfs, NULL, FILE_BEGIN);
#else
	m_pFileSystem->Seek(fHandle.m_hFileHandle, cachedVPKRead.m_nFileFraction, FILESYSTEM_SEEK_HEAD);
	cachedVPKRead.m_cubBuffer = m_pFileSystem->Read(cachedVPKRead.m_pubBuffer, k_cubCacheBufferSize, fHandle.m_hFileHandle);
	m_pFileSystem->Seek(fHandle.m_hFileHandle, fHandle.m_nCurOfs, FILESYSTEM_SEEK_HEAD);
#endif
	Assert(cachedVPKRead.m_hMD5RequestHandle == 0);
	//if ( m_pFileTracker ) // file tracker doesn't exist in the VPK command line tool
	//{
	//	cachedVPKRead.m_hMD5RequestHandle = m_pFileTracker->SubmitThreadedMD5Request( cachedVPKRead.m_pubBuffer, cachedVPKRead.m_cubBuffer, m_pPackedStore->m_PackFileID, cachedVPKRead.m_nPackFileNumber, cachedVPKRead.m_nFileFraction );
	//}
	return cachedVPKRead.m_cubBuffer > 0;
}


// check if the MD5 matches
bool CPackedStoreReadCache::CheckMd5Result(CachedVPKRead_t &cachedVPKRead)
{
	return true;

}


int CPackedStoreReadCache::FindBufferToUse()
{
	int idxLRU = 0;
	int idxToRemove = m_treeCachedVPKRead.InvalidIndex();
	uint32 uTimeLowest = (uint32)~0; // MAXINT
	// find the oldest item, reuse its buffer
	for (int i = 0; i < m_cItemsInCache; i++)
	{
		if (m_rgLastUsedTime[i] < uTimeLowest)
		{
			uTimeLowest = m_rgLastUsedTime[i];
			idxToRemove = m_rgCurrentCacheIndex[i];
			idxLRU = i;
		}
		int idxCurrent = m_rgCurrentCacheIndex[i];
		// while we are here check if the MD5 is done
		if (m_treeCachedVPKRead[idxCurrent].m_hMD5RequestHandle)
		{
			CachedVPKRead_t &cachedVPKRead = m_treeCachedVPKRead[idxCurrent];
			if (m_pFileTracker->IsMD5RequestComplete(cachedVPKRead.m_hMD5RequestHandle, &cachedVPKRead.m_md5Value))
			{
				// if it is done, check the results
				cachedVPKRead.m_hMD5RequestHandle = 0;
				// if we got bad data - stop looking, just use this one
				if (!CheckMd5Result(cachedVPKRead))
					return i;
			}
		}
	}

	// if we submitted its MD5 for processing, then wait until that is done
	if (m_treeCachedVPKRead[idxToRemove].m_hMD5RequestHandle)
	{
		CachedVPKRead_t &cachedVPKRead = m_treeCachedVPKRead[idxToRemove];
		m_pFileTracker->BlockUntilMD5RequestComplete(cachedVPKRead.m_hMD5RequestHandle, &cachedVPKRead.m_md5Value);
		m_treeCachedVPKRead[idxToRemove].m_hMD5RequestHandle = 0;
		// make sure it matches what it is supposed to match
		CheckMd5Result(cachedVPKRead);
	}
	return idxLRU;
}


// manage the cache
bool CPackedStoreReadCache::BCanSatisfyFromReadCacheInternal(uint8 *pOutData, CPackedStoreFileHandle &handle, FileHandleTracker_t &fHandle, int nDesiredPos, int nNumBytes, int &nRead)
{

	m_rwlock.LockForRead();
	bool bLockedForWrite = false;

	CachedVPKRead_t key;
	key.m_nPackFileNumber = handle.m_nFileNumber;
	key.m_nFileFraction = nDesiredPos & k_nCacheBufferMask;
	int idxTrackedVPKFile = m_treeCachedVPKRead.Find(key);
	if (idxTrackedVPKFile == m_treeCachedVPKRead.InvalidIndex() || m_treeCachedVPKRead[idxTrackedVPKFile].m_pubBuffer == NULL)
	{
		m_rwlock.UnlockRead();
		m_rwlock.LockForWrite();
		bLockedForWrite = true;
		// if we didnt find it, we had to grab the write lock, it may have been added while we waited
		idxTrackedVPKFile = m_treeCachedVPKRead.Find(key);
	}
	if (idxTrackedVPKFile == m_treeCachedVPKRead.InvalidIndex())
	{
		idxTrackedVPKFile = m_treeCachedVPKRead.Insert(key);
	}

	CachedVPKRead_t &cachedVPKRead = m_treeCachedVPKRead[idxTrackedVPKFile];

	// Cache hit?
	if (cachedVPKRead.m_pubBuffer == NULL)
	{

		// We need to have it locked for write, because we're about to muck with these structures.
		if (!bLockedForWrite)
		{
			Assert(bLockedForWrite);
			return false;
		}

		Assert(cachedVPKRead.m_idxLRU < 0);

		// Can we add another line to the cache, or should we reuse an existing one?
		int idxLRU = -1;
		if (m_cItemsInCache >= k_nCacheBuffersToKeep)
		{
			// Need to kick out the LRU.
			idxLRU = FindBufferToUse();
			int idxToRemove = m_rgCurrentCacheIndex[idxLRU];
			Assert(m_treeCachedVPKRead[idxToRemove].m_idxLRU == idxLRU);
			Assert(m_treeCachedVPKRead[idxToRemove].m_pubBuffer != NULL);

			// Transfer ownership of the buffer
			cachedVPKRead.m_pubBuffer = m_treeCachedVPKRead[idxToRemove].m_pubBuffer;
			m_treeCachedVPKRead[idxToRemove].m_pubBuffer = NULL;
			m_treeCachedVPKRead[idxToRemove].m_cubBuffer = 0;
			m_treeCachedVPKRead[idxToRemove].m_idxLRU = -1;
			m_cDiscardsFromCache++;
		}
		else
		{

			// We can add a new one
			idxLRU = m_cItemsInCache;
			m_cItemsInCache++;
			Assert(cachedVPKRead.m_pubBuffer == NULL);
		}
		m_rgCurrentCacheIndex[idxLRU] = idxTrackedVPKFile;
		cachedVPKRead.m_idxLRU = idxLRU;

		if (cachedVPKRead.m_pubBuffer == NULL)
		{
			cachedVPKRead.m_pubBuffer = (uint8 *)malloc(k_cubCacheBufferSize);
			if (cachedVPKRead.m_pubBuffer == NULL)
				Error("Out of memory");
		}
		ReadCacheLine(fHandle, cachedVPKRead);
		m_cAddedToCache++;
	}
	else
	{
		Assert(cachedVPKRead.m_idxLRU >= 0);
		Assert(m_rgCurrentCacheIndex[cachedVPKRead.m_idxLRU] == idxTrackedVPKFile);
	}

	// Assume no bytes will be satisfied from cache
	bool bSuccess = false;
	nRead = 0;

	// Can we read at least one byte?
	Assert(cachedVPKRead.m_nFileFraction <= nDesiredPos);
	int nBufferEnd = cachedVPKRead.m_cubBuffer + cachedVPKRead.m_nFileFraction;
	if (cachedVPKRead.m_pubBuffer != NULL && nBufferEnd > nDesiredPos)
	{
		nRead = min(nBufferEnd - nDesiredPos, nNumBytes);
		int nOffset = nDesiredPos - cachedVPKRead.m_nFileFraction;
		Assert(nOffset >= 0);
		memcpy(pOutData, (uint8 *)&cachedVPKRead.m_pubBuffer[nOffset], nRead);
		m_cubReadFromCache += nRead;
		m_cReadFromCache++;
		bSuccess = true;
		m_rgLastUsedTime[m_treeCachedVPKRead[idxTrackedVPKFile].m_idxLRU] = Plat_MSTime();
	}
	if (bLockedForWrite)
		m_rwlock.UnlockWrite();
	else
		m_rwlock.UnlockRead();

	return bSuccess;
}


void CPackedStoreReadCache::RetryBadCacheLine(CachedVPKRead_t &cachedVPKRead)
{
	//ChunkHashFraction_t chunkHashFraction;
	//m_pPackedStore->FindFileHashFraction( cachedVPKRead.m_nPackFileNumber, cachedVPKRead.m_nFileFraction, chunkHashFraction );
	//
	//cachedVPKRead.m_pubBuffer = (uint8 *)malloc( k_cubCacheBufferSize );
	//FileHandleTracker_t &fHandle = m_pPackedStore->GetFileHandle( cachedVPKRead.m_nPackFileNumber );
	//fHandle.m_Mutex.Lock();
	//ReadCacheLine( fHandle, cachedVPKRead );
	//fHandle.m_Mutex.Unlock();
	//m_pFileTracker->BlockUntilMD5RequestComplete( cachedVPKRead.m_hMD5RequestHandle, &cachedVPKRead.m_md5Value );
	//cachedVPKRead.m_hMD5RequestHandle = 0;
	//CheckMd5Result( cachedVPKRead );
	//cachedVPKRead.m_pubBuffer = NULL;
}


// try reloading anything that failed its md5 check
// this is currently only for gathering information, doesnt do anything to repair the cache
void CPackedStoreReadCache::RetryAllBadCacheLines()
{
	//	while( m_queueCachedVPKReadsRetry.Count() )
	//	{
	//		CachedVPKRead_t cachedVPKRead;
	//		m_rwlock.LockForWrite();
	//		if ( m_queueCachedVPKReadsRetry.PopItem( &cachedVPKRead ) )
	//		{
	//			// retry anything that didnt match one time
	//			RetryBadCacheLine( cachedVPKRead );
	//			m_listCachedVPKReadsFailed.AddToTail( cachedVPKRead );
	//			// m_listCachedVPKReadsFailed contains all the data about failed reads - for error or OGS reporting
	//		}
	//		m_rwlock.UnlockWrite();
	//	}
}


int CPackedStore::ReadData(CPackedStoreFileHandle &handle, void *pOutData, int nNumBytes)
{
	int nRet = 0;

	// clamp read size to file size
	nNumBytes = MIN(nNumBytes, handle.m_nFileSize - handle.m_nCurrentFileOffset);
	if (nNumBytes > 0)
	{
		// first satisfy from the metadata, if we can
		int nNumMetaDataBytes = MIN(nNumBytes, handle.m_nMetaDataSize - handle.m_nCurrentFileOffset);
		if (nNumMetaDataBytes > 0)
		{
			memcpy(pOutData, reinterpret_cast<uint8 const *>(handle.m_pMetaData)
				+ handle.m_nCurrentFileOffset, nNumMetaDataBytes);
			nRet += nNumMetaDataBytes;
			pOutData = reinterpret_cast<uint8 *>(pOutData)+nNumMetaDataBytes;
			handle.m_nCurrentFileOffset += nNumMetaDataBytes;
			nNumBytes -= nNumMetaDataBytes;
		}
		// satisfy remaining bytes from file
		if (nNumBytes > 0)
		{
			FileHandleTracker_t &fHandle = GetFileHandle(handle.m_nFileNumber);
			int nDesiredPos = handle.m_nFileOffset + handle.m_nCurrentFileOffset - handle.m_nMetaDataSize;
			int nRead = 0;
			fHandle.m_Mutex.Lock();
			if (handle.m_nFileNumber == VPKFILENUMBER_EMBEDDED_IN_DIR_FILE)
			{
				// for file data in the directory header, all offsets are relative to the size of the dir header.
				nDesiredPos += m_nDirectoryDataSize + sizeof(VPKDirHeader_t);
			}

			if (m_PackedStoreReadCache.BCanSatisfyFromReadCache((uint8 *)pOutData, handle, fHandle, nDesiredPos, nNumBytes, nRead))
			{
				handle.m_nCurrentFileOffset += nRead;
			}
			else
			{
#ifdef IS_WINDOWS_PC
				if (nDesiredPos != fHandle.m_nCurOfs)
					SetFilePointer(fHandle.m_hFileHandle, nDesiredPos, NULL, FILE_BEGIN);
				ReadFile(fHandle.m_hFileHandle, pOutData, nNumBytes, (LPDWORD)&nRead, NULL);
#else
				m_pFileSystem->Seek(fHandle.m_hFileHandle, nDesiredPos, FILESYSTEM_SEEK_HEAD);
				nRead = m_pFileSystem->Read(pOutData, nNumBytes, fHandle.m_hFileHandle);
#endif
				handle.m_nCurrentFileOffset += nRead;
				fHandle.m_nCurOfs = nRead + nDesiredPos;
			}
			Assert(nRead == nNumBytes);
			nRet += nRead;
			fHandle.m_Mutex.Unlock();
		}
	}
	m_PackedStoreReadCache.RetryAllBadCacheLines();
	return nRet;
}


void CPackedStore::GetDataFileName(char* pchFileNameOut, int cchFileNameOut, int nFileNumber) const
{
	if (nFileNumber == VPKFILENUMBER_EMBEDDED_IN_DIR_FILE)
	{
		if (m_bUseDirFile)
		{
			V_snprintf(pchFileNameOut, cchFileNameOut, "%s_dir.vpk", m_pszFileBaseName);
		}
		else
		{
			V_snprintf(pchFileNameOut, cchFileNameOut, "%s.vpk", m_pszFileBaseName);
		}
	}
	else
	{
		V_snprintf(pchFileNameOut, cchFileNameOut, "%s_%03d.vpk", m_pszFileBaseName, nFileNumber);
	}

}

FileHandleTracker_t & CPackedStore::GetFileHandle(int nFileNumber)
{
	AUTO_LOCK(m_Mutex);
	int nFileHandleIdx = nFileNumber % ARRAYSIZE(m_FileHandles);

	if (m_FileHandles[nFileHandleIdx].m_nFileNumber == nFileNumber)
	{
		return m_FileHandles[nFileHandleIdx];
	}
	else if (m_FileHandles[nFileHandleIdx].m_nFileNumber == -1)
	{
		// no luck finding the handle - need a new one
		char pszDataFileName[MAX_PATH];
		GetDataFileName(pszDataFileName, sizeof(pszDataFileName), nFileNumber);
		m_FileHandles[nFileHandleIdx].m_nCurOfs = 0;
#ifdef IS_WINDOWS_PC
		m_FileHandles[nFileHandleIdx].m_hFileHandle =
			CreateFile(pszDataFileName,               // file to open
			GENERIC_READ,          // open for reading
			FILE_SHARE_READ,       // share for reading
			NULL,                  // default security
			OPEN_EXISTING,         // existing file only
			FILE_ATTRIBUTE_NORMAL, // normal file
			NULL);                 // no attr. template

		if (m_FileHandles[nFileHandleIdx].m_hFileHandle != INVALID_HANDLE_VALUE)
		{
			m_FileHandles[nFileHandleIdx].m_nFileNumber = nFileNumber;
		}
#else
		m_FileHandles[nFileHandleIdx].m_hFileHandle = m_pFileSystem->Open(pszDataFileName, "rb");
		if (m_FileHandles[nFileHandleIdx].m_hFileHandle != FILESYSTEM_INVALID_HANDLE)
		{
			m_FileHandles[nFileHandleIdx].m_nFileNumber = nFileNumber;
		}
#endif
		return m_FileHandles[nFileHandleIdx];
	}
	Error("Exceeded limit of number of vpk files supported (%d)!\n", MAX_ARCHIVE_FILES_TO_KEEP_OPEN_AT_ONCE);
	static FileHandleTracker_t invalid;
#ifdef IS_WINDOWS_PC
	invalid.m_hFileHandle = INVALID_HANDLE_VALUE;
#else
	invalid.m_hFileHandle = FILESYSTEM_INVALID_HANDLE;
#endif
	return invalid;
}


bool CPackedStore::InternalRemoveFileFromDirectory(const char *pszName)
{
	CPackedStoreFileHandle pData = OpenFile(pszName);
	if (!pData)
		return false;

	CFileHeaderFixedData *pHeader = pData.m_pHeaderData;
	// delete the old header so we can insert a new one with updated contents
	int nBytesToRemove = (int)(V_strlen((char *)pData.m_pDirFileNamePtr) + 1 + pHeader->HeaderSizeIncludingMetaData());
	m_DirectoryData.RemoveMultiple(pData.m_pDirFileNamePtr - m_DirectoryData.Base(), nBytesToRemove);
	return true;
}

int CPackedStore::GetFileList(CUtlStringList &outFilenames, bool bFormattedOutput, bool bSortedOutput)
{
	return GetFileList(NULL, outFilenames, bFormattedOutput, bSortedOutput);
}

int CPackedStore::GetFileList(const char *pWildCard, CUtlStringList &outFilenames, bool bFormattedOutput, bool bSortedOutput)
{
	// Separate the wildcard base from the extension
	char szWildCardPath[MAX_PATH];
	char szWildCardBase[64];
	char szWildCardExt[20];
	bool bNoBaseWildcard = false;
	bool bNoExtWildcard = false;

	szWildCardPath[0] = szWildCardExt[0] = szWildCardBase[0] = NULL;

	// Parse the wildcard string into a base and extension used for string comparisons
	if (pWildCard)
	{
		V_ExtractFilePath(pWildCard, szWildCardPath, sizeof(szWildCardPath));
		V_FixSlashes(szWildCardPath, '/');

		V_FileBase(pWildCard, szWildCardBase, sizeof(szWildCardBase));
		V_ExtractFileExtension(pWildCard, szWildCardExt, sizeof(szWildCardExt));

		// Remove '*' from the base and extension strings so that the string comparison calls will match
		char *pcStar = strchr(szWildCardBase, '*');
		pcStar ? *pcStar = NULL : bNoBaseWildcard = true;
		pcStar = strchr(szWildCardExt, '*');
		pcStar ? *pcStar = NULL : bNoExtWildcard = true;
	}

	char const *pData = reinterpret_cast< char const *>(DirectoryData());
	while (*pData)
	{
		// for each extension
		char pszCurExtension[MAX_PATH];
		if (pData[0] != ' ')
			sprintf(pszCurExtension, ".%s", pData);
		else
			pszCurExtension[0] = 0;
		// now, iterate over all directories associated with this extension
		pData += 1 + strlen(pData);
		while (*pData)
		{
			char pszCurDir[MAX_PATH];
			if (pData[0] != ' ')
				sprintf(pszCurDir, "%s/", pData);
			else
				pszCurDir[0] = 0;
			pData += 1 + strlen(pData);					// skip dir name
			// now, march through all the files
			while (*pData)									// until we're out of files to look at
			{
				char pszFNameOut[MAX_PATH * 2];
				if (bFormattedOutput)
				{
					CFileHeaderFixedData const *pHeader = reinterpret_cast< CFileHeaderFixedData const *>(pData + 1 + strlen(pData));
					sprintf(pszFNameOut, "%s%s%s crc=0x%x metadatasz=%d", pszCurDir, pData, pszCurExtension, pHeader->m_nFileCRC, pHeader->m_nMetaDataSize);
					CFilePartDescr const *pPart = &(pHeader->m_PartDescriptors[0]);
					while (pPart->m_nFileNumber != PACKFILEINDEX_END)
					{
						sprintf(pszFNameOut + strlen(pszFNameOut), " fnumber=%d ofs=0x%x sz=%d",
							pPart->m_nFileNumber, pPart->m_nFileDataOffset, pPart->m_nFileDataSize);
						pPart++;
					}
				}
				else
				{
					V_strncpy(pszFNameOut, pszCurDir, sizeof(pszFNameOut));
					V_strncat(pszFNameOut, pData, sizeof(pszFNameOut));
					V_strncat(pszFNameOut, pszCurExtension, sizeof(pszFNameOut));
				}
				SkipFile(pData);

				bool matches = true;

				if (pWildCard)
				{
					// See if the filename matches the wildcards
					char szFNameOutPath[MAX_PATH];
					char szFNameOutBase[64];
					char szFNameOutExt[20];

					V_ExtractFilePath(pszFNameOut, szFNameOutPath, sizeof(szFNameOutPath));
					V_FileBase(pszFNameOut, szFNameOutBase, sizeof(szFNameOutBase));
					V_ExtractFileExtension(pszFNameOut, szFNameOutExt, sizeof(szFNameOutExt));

					matches = !V_strnicmp(szFNameOutPath, szWildCardPath, sizeof(szWildCardPath));
					matches = matches && (!V_strlen(szWildCardExt) || bNoExtWildcard ? 0 == V_strnicmp(szFNameOutExt, szWildCardExt, strlen(szWildCardExt)) : 0 != V_stristr(szFNameOutExt, szWildCardExt));
					matches = matches && (!V_strlen(szWildCardBase) || bNoBaseWildcard ? 0 == V_strnicmp(szFNameOutBase, szWildCardBase, strlen(szWildCardBase)) : 0 != V_stristr(szFNameOutBase, szWildCardBase));
				}

				// Add the file to the output list
				if (matches)
				{
					char *pFName = new char[1 + strlen(pszFNameOut)];
					strcpy(pFName, pszFNameOut);
					outFilenames.AddToTail(pFName);
				}
			}
			pData++;												// skip end marker
		}
		pData++;												// skip end marker
	}

	if (bSortedOutput)
	{
		outFilenames.Sort(&CUtlStringList::SortFunc);
	}

	return outFilenames.Count();
}


int CPackedStore::GetFileAndDirLists(CUtlStringList &outDirnames, CUtlStringList &outFilenames, bool bSortedOutput)
{
	return GetFileAndDirLists(NULL, outDirnames, outFilenames, bSortedOutput);
}

void CPackedStore::BuildFindFirstCache()
{
	CUtlStringList allVPKFiles;
	char szLastDirFound[MAX_PATH];

	// Init
	V_strncpy(szLastDirFound, "$$$$$$$HighlyUnlikelyPathForInitializationPurposes#######", sizeof(szLastDirFound));
	m_dirContents.RemoveAll();
	m_dirContents.SetLessFunc(DefLessFunc(int));

	// Get all files in the VPK
	GetFileList(allVPKFiles, false, true);

	// Add directories to directory list and files into map
	FOR_EACH_VEC(allVPKFiles, i)
	{
		char szFilePath[MAX_PATH];

		V_ExtractFilePath(allVPKFiles[i], szFilePath, sizeof(szFilePath));
		Q_StripTrailingSlash(szFilePath);

		// New directory
		if (V_strnicmp(szFilePath, szLastDirFound, sizeof(szLastDirFound)))
		{
			// Mark the new one as the last one encountered
			V_strncpy(szLastDirFound, szFilePath, sizeof(szFilePath));

			// Add it
			m_directoryList.CopyAndAddToTail(szFilePath);
			m_dirContents.Insert(m_directoryList.Count(), new CUtlStringList()); // Freed in destructor
		}

		unsigned short nIndex = m_dirContents.Find(m_directoryList.Count());
		CUtlStringList *pList = m_dirContents.Element(nIndex);

		pList->CopyAndAddToTail(V_UnqualifiedFileName(allVPKFiles[i]));
	}
}

int CPackedStore::GetFileAndDirLists(const char *pWildCard, CUtlStringList &outDirnames, CUtlStringList &outFilenames, bool bSortedOutput)
{


	// If this is the first time we've called FindFirst on this CPackedStore then let's build the caches
	if (!m_directoryList.Count())
	{
		BuildFindFirstCache();
		//printf("CPackedStore::GetFileAndDirLists - list of directories in VPK files\n");
		//FOR_EACH_VEC( m_directoryList, i )
		//{
		//	printf("\t%d : %s\n", i, m_directoryList[i] );
		//}
	}

	// printf("CPackedStore::GetFileAndDirLists - Searching for %s\n", pWildCard? pWildCard: "NULL");

	if (pWildCard)
	{
		CUtlDict<int, int> AddedDirectories; // Used to remove duplicate paths

		char szWildCardPath[MAX_PATH];
		char szWildCardBase[64];
		char szWildCardExt[20];

		int nLenWildcardPath = 0;
		int nLenWildcardBase = 0;
		int nLenWildcardExt = 0;

		bool bBaseWildcard = true;
		bool bExtWildcard = true;

		szWildCardPath[0] = szWildCardExt[0] = szWildCardBase[0] = '\0';
		//
		// Parse the wildcard string into a base and extension used for string comparisons
		//
		V_ExtractFilePath(pWildCard, szWildCardPath, sizeof(szWildCardPath));
		V_FixSlashes(szWildCardPath, '/');

		V_FileBase(pWildCard, szWildCardBase, sizeof(szWildCardBase));
		V_ExtractFileExtension(pWildCard, szWildCardExt, sizeof(szWildCardExt));

		// From the pattern, we now have the directory path up to the file pattern, the filename base, and the filename 
		// extension.

		// Remove '*' from the base and extension strings so that the string comparison calls will match
		char *pcStar = strchr(szWildCardBase, '*');
		pcStar ? *pcStar = NULL : bBaseWildcard = false;

		pcStar = strchr(szWildCardExt, '*');
		pcStar ? *pcStar = NULL : bExtWildcard = false;

		nLenWildcardPath = V_strlen(szWildCardPath);
		nLenWildcardBase = V_strlen(szWildCardBase);
		nLenWildcardExt = V_strlen(szWildCardExt);

		// Generate the list of directories and files that match the wildcard
		//

		//
		// Directories first
		//
		FOR_EACH_VEC(m_directoryList, i)
		{
			// Does this file's path match the wildcard path?
			if ((nLenWildcardPath && (0 == V_strnicmp(m_directoryList[i], szWildCardPath, nLenWildcardPath)))
				|| (!nLenWildcardPath && (0 == V_strlen(m_directoryList[i]))))
			{
				// Extract the sub-directory name if there is one
				char szSubDir[64];
				char *szSubDirExtension = NULL; // this is anything after a '.' in szSubDir
				bool bBaseMatch = false;
				bool bExtMatch = false;

				// Copy everything to the right of the root directory
				V_strncpy(szSubDir, &m_directoryList[i][nLenWildcardPath], sizeof(szSubDir));

				// Set the next / to NULL and we have our subdirectory
				char *pSlash = strchr(szSubDir, '/');
				pSlash ? *pSlash = NULL : NULL;

				szSubDirExtension = strchr(szSubDir, '.');
				if (szSubDirExtension)
				{
					// Null out the . and move the szSubDirExtension to point to the extension
					*szSubDirExtension = '\0';
					szSubDirExtension++;
				}

				// If we have a base dir name, and we have a szWildCardBase to match against
				if (bBaseWildcard)
					bBaseMatch = true;  // The base is the wildCard ("*"), so whatever we have as the base matches
				else
					bBaseMatch = (0 == V_strnicmp(szSubDir, szWildCardBase, nLenWildcardBase));

				// If we have an extension and we have a szWildCardExtension to mach against
				if (bExtWildcard)
					bExtMatch = true; // The extension is the wildcard ("*"), so whatever we have as the extension matches
				else
					bExtMatch = (NULL == szSubDirExtension && '\0' == *szWildCardExt) || ((NULL != szSubDirExtension) && (0 == V_strnicmp(szSubDirExtension, szWildCardExt, nLenWildcardExt)));

				// If both parts match, then add it to the list of directories that match
				if (bBaseMatch && bExtMatch)
				{
					char szFullPathToDir[MAX_PATH];

					V_strncpy(szFullPathToDir, szWildCardPath, nLenWildcardPath);
					V_strcat_safe(szFullPathToDir, "/");
					V_strcat_safe(szFullPathToDir, szSubDir);

					// Add the subdirectory to the list if it isn't already there
					if (-1 == AddedDirectories.Find(szFullPathToDir))
					{
						char *pDName = new char[1 + strlen(szFullPathToDir)];
						V_strncpy(pDName, szFullPathToDir, 1 + strlen(szFullPathToDir));
						outDirnames.AddToTail(pDName);
						AddedDirectories.Insert(pDName, 0);
					}
				}
			}
		}

		//
		// Files
		//
		FOR_EACH_VEC(m_directoryList, i)
		{
			// We no longer want the trailing slash
			Q_StripTrailingSlash(szWildCardPath);

			// Find the directory that matches the wildcard path
			if (!V_strnicmp(szWildCardPath, m_directoryList[i], sizeof(szWildCardPath)))
			{
				CUtlStringList &filesInDirectory = *(m_dirContents.Element(i));

				// Use the cached list of files in this directory
				FOR_EACH_VEC(filesInDirectory, iFile)
				{
					bool matches = true;


					// See if the filename matches the wildcards
					char szFNameOutBase[64];
					char szFNameOutExt[20];

					V_FileBase(filesInDirectory[iFile], szFNameOutBase, sizeof(szFNameOutBase));
					V_ExtractFileExtension(filesInDirectory[iFile], szFNameOutExt, sizeof(szFNameOutExt));

					// Since we have a sorted list we can optimize using the return code of the compare
					int c = V_strnicmp(szWildCardBase, szFNameOutBase, nLenWildcardBase);
					if (c < 0)
						break;
					if (c > 0)
						continue;
					matches = ((nLenWildcardExt <= 0) || bBaseWildcard ? 0 == V_strnicmp(szFNameOutExt, szWildCardExt, nLenWildcardExt) : V_stristr(szFNameOutExt, szWildCardExt) != NULL);

					// Add the file to the output list
					if (matches)
					{
						bool bFound = false;

						FOR_EACH_VEC(outFilenames, j)
						{
							if (!V_strncmp(outFilenames[j], filesInDirectory[iFile], V_strlen(filesInDirectory[iFile])))
							{
								bFound = true;
								break;
							}
						}

						if (!bFound)
						{
							outFilenames.CopyAndAddToTail(filesInDirectory[iFile]);
						}
					}
				}
			}
		}
	}
	else // Otherwise, simply return the base data
	{
		// Add all the files as well
		FOR_EACH_VEC(m_directoryList, i)
		{
			// Add all directories
			outDirnames.CopyAndAddToTail(m_directoryList[i]);

			// Now add all files
			CUtlStringList &filesInDirectory = *(m_dirContents.Element(i));
			FOR_EACH_VEC(filesInDirectory, j)
			{
				outFilenames.CopyAndAddToTail(filesInDirectory[j]);
			}
		}
	}

	// Sort the output if requested
	if (bSortedOutput)
	{
		outDirnames.Sort(&CUtlStringList::SortFunc);
		outFilenames.Sort(&CUtlStringList::SortFunc);
	}

	return outDirnames.Count();
}


#pragma once

#ifndef USE_PRECOMPILED_HEADERS
#include "memaddr.h"
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include <psapi.h>
#include <intrin.h>
#include <algorithm>
#else
#pragma message("ADD PRECOMPILED HEADERS TO SILVER-BUN.")
#endif // !USE_PRECOMPILED_HEADERS

class CModule
{
public:
	struct ModuleSections_t
	{
		ModuleSections_t(void) = default;
		ModuleSections_t(const char* sectionName, uintptr_t pSectionBase, size_t nSectionSize) :
			m_SectionName(sectionName), m_pSectionBase(pSectionBase), m_nSectionSize(nSectionSize) {}

		inline bool IsSectionValid(void) const { return m_nSectionSize != 0; }

		std::string m_SectionName;           // Name of section.
		uintptr_t   m_pSectionBase;          // Start address of section.
		size_t      m_nSectionSize;          // Size of section.
	};

	CModule(void) = default;
	CModule(const char* szModuleName);
	CModule(const char* szModuleName, const uintptr_t nModuleBase);

	void Init();
	void LoadSections();

	CMemory FindPatternSIMD(const char* szPattern, const ModuleSections_t* moduleSection = nullptr) const;
	CMemory FindPatternSIMD(const uint8_t* pPattern, const char* szMask,
		const ModuleSections_t* moduleSection = nullptr, const size_t nOccurrence = 0) const;

	CMemory FindString(const char* szString, const ptrdiff_t occurrence = 1, bool nullTerminator = false) const;
	CMemory FindStringReadOnly(const char* szString, bool nullTerminator) const;
	CMemory FindStringData(const char* szString, bool bNullTerminator) const;
	CMemory FindFreeDataPage(const size_t nSize) const;

	std::vector<CMemory> GetXrefsTo(const uintptr_t address, const ModuleSections_t* moduleSection) const;

	CMemory          GetVirtualMethodTable(const char* szTableName, const size_t nRefIndex = 0);
	CMemory          GetImportedFunction(const char* szModuleName, const char* szFunctionName, const bool bGetFunctionReference) const;
	CMemory          GetExportedFunction(const char* szFunctionName) const;
	ModuleSections_t GetSectionByName(const char* szSectionName) const;

	inline const std::vector<CModule::ModuleSections_t>& GetSections() const { return m_ModuleSections; }
	inline uintptr_t     GetModuleBase(void) const { return m_pModuleBase; }
	inline DWORD         GetModuleSize(void) const { return m_nModuleSize; }
	inline const std::string& GetModuleName(void) const { return m_ModuleName; }
	inline uintptr_t     GetRVA(const uintptr_t nAddress) const { return (nAddress - GetModuleBase()); }

#if _WIN64 
	void             UnlinkFromPEB(void) const;
#endif // #if _WIN64 

	IMAGE_NT_HEADERS64*      m_pNTHeaders;
	IMAGE_DOS_HEADER*        m_pDOSHeader;

	ModuleSections_t         m_ExecutableCode;
	ModuleSections_t         m_ExceptionTable;
	ModuleSections_t         m_RunTimeData;
	ModuleSections_t         m_ReadOnlyData;

private:

	std::string                   m_ModuleName;
	uintptr_t                     m_pModuleBase;
	DWORD                         m_nModuleSize;
	std::vector<ModuleSections_t> m_ModuleSections;
};

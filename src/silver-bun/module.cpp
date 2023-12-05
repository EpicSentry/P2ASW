//===========================================================================//
//
// Purpose: Implementation of the CModule class.
//
//===========================================================================//
#include "cbase.h"
#include "module.h"
#include "utils.h"


#if _WIN64 
#include "tebpeb64.h"
#else
#include "Windows.h"
#endif // #if _WIN64 

//-----------------------------------------------------------------------------
// Purpose: constructor
// Input  : *szModuleName - 
//-----------------------------------------------------------------------------
CModule::CModule(const char* szModuleName)
	: m_ModuleName(szModuleName)
{
	m_pModuleBase = reinterpret_cast<uintptr_t>(GetModuleHandleA(szModuleName));

	Init();
	LoadSections();
}

//-----------------------------------------------------------------------------
// Purpose: constructor
// Input  : *szModuleName - 
//			nModuleBase   - 
//-----------------------------------------------------------------------------
CModule::CModule(const char* szModuleName, const uintptr_t nModuleBase)
	: m_ModuleName(szModuleName)
	, m_pModuleBase(nModuleBase)
{
	Init();
	LoadSections();
}

//-----------------------------------------------------------------------------
// Purpose: initializes module descriptors
//-----------------------------------------------------------------------------
void CModule::Init()
{
	m_pDOSHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(m_pModuleBase);
	m_pNTHeaders = reinterpret_cast<IMAGE_NT_HEADERS64*>(m_pModuleBase + m_pDOSHeader->e_lfanew);
	m_nModuleSize = static_cast<size_t>(m_pNTHeaders->OptionalHeader.SizeOfImage);

	const IMAGE_SECTION_HEADER* hSection = IMAGE_FIRST_SECTION(m_pNTHeaders); // Get first image section.

	for (WORD i = 0; i < m_pNTHeaders->FileHeader.NumberOfSections; i++) // Loop through the sections.
	{
		const IMAGE_SECTION_HEADER& hCurrentSection = hSection[i]; // Get current section.
		m_ModuleSections.push_back(ModuleSections_t(reinterpret_cast<const char*>(hCurrentSection.Name),
			static_cast<uintptr_t>(m_pModuleBase + hCurrentSection.VirtualAddress), hCurrentSection.SizeOfRawData)); // Push back a struct with the section data.
	}
}

//-----------------------------------------------------------------------------
// Purpose: initializes the default executable segments
//-----------------------------------------------------------------------------
void CModule::LoadSections()
{
	m_ExecutableCode = GetSectionByName(".text");
	m_ExceptionTable = GetSectionByName(".pdata");
	m_RunTimeData    = GetSectionByName(".data");
	m_ReadOnlyData   = GetSectionByName(".rdata");
}

//-----------------------------------------------------------------------------
// Purpose: find array of bytes in process memory using SIMD instructions
// Input  : *pPattern      - 
//          *szMask        - 
//          *moduleSection - 
//          nOccurrence    - 
// Output : CMemory
//-----------------------------------------------------------------------------
CMemory CModule::FindPatternSIMD(const uint8* pPattern, const char* szMask,
	const ModuleSections_t* moduleSection, const size_t nOccurrence) const
{
	if (!m_ExecutableCode.IsSectionValid())
		return CMemory();

	const bool bSectionValid = moduleSection ? moduleSection->IsSectionValid() : false;

	const uintptr_t nBase = bSectionValid ? moduleSection->m_pSectionBase : m_ExecutableCode.m_pSectionBase;
	const uintptr_t nSize = bSectionValid ? moduleSection->m_nSectionSize : m_ExecutableCode.m_nSectionSize;

	const size_t nMaskLen = strlen(szMask);
	const uint8* pData = reinterpret_cast<uint8*>(nBase);
	const uint8* pEnd = pData + nSize - nMaskLen;

	size_t nOccurrenceCount = 0;
	int nMasks[64]; // 64*16 = enough masks for 1024 bytes.
	const int iNumMasks = static_cast<int>(ceil(static_cast<float>(nMaskLen) / 16.f));

	memset(nMasks, '\0', iNumMasks * sizeof(int));
	for (intptr_t i = 0; i < iNumMasks; ++i)
	{
		for (intptr_t j = strnlen(szMask + i * 16, 16) - 1; j >= 0; --j)
		{
			if (szMask[i * 16 + j] == 'x')
			{
				_bittestandset(reinterpret_cast<LONG*>(&nMasks[i]), static_cast<LONG>(j));
			}
		}
	}
	const __m128i xmm1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(pPattern));
	__m128i xmm2, xmm3, msks;
	for (; pData != pEnd; _mm_prefetch(reinterpret_cast<const char*>(++pData + 64), _MM_HINT_NTA))
	{
		if (pPattern[0] == pData[0])
		{
			xmm2 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(pData));
			msks = _mm_cmpeq_epi8(xmm1, xmm2);
			if ((_mm_movemask_epi8(msks) & nMasks[0]) == nMasks[0])
			{
				for (uintptr_t i = 1; i < static_cast<uintptr_t>(iNumMasks); ++i)
				{
					xmm2 = _mm_loadu_si128(reinterpret_cast<const __m128i*>((pData + i * 16)));
					xmm3 = _mm_loadu_si128(reinterpret_cast<const __m128i*>((pPattern + i * 16)));
					msks = _mm_cmpeq_epi8(xmm2, xmm3);
					if ((_mm_movemask_epi8(msks) & nMasks[i]) == nMasks[i])
					{
						if ((i + 1) == static_cast<uintptr_t>(iNumMasks))
						{
							if (nOccurrenceCount == nOccurrence)
							{
								return static_cast<CMemory>(const_cast<uint8*>(pData));
							}
							nOccurrenceCount++;
						}
					}
					else
					{
						goto cont;
					}
				}
				if (nOccurrenceCount == nOccurrence)
				{
					return static_cast<CMemory>((&*(const_cast<uint8*>(pData))));
				}
				nOccurrenceCount++;
			}
		}cont:;
	}
	return CMemory();
}

//-----------------------------------------------------------------------------
// Purpose: find a string pattern in process memory using SIMD instructions
// Input  : *szPattern     - 
//			*moduleSection - 
// Output : CMemory
//-----------------------------------------------------------------------------
CMemory CModule::FindPatternSIMD(const char* szPattern, const ModuleSections_t* moduleSection) const
{
	const std::pair<std::vector<uint8>, std::string> patternInfo = Utils::PatternToMaskedBytes(szPattern);
	return FindPatternSIMD(patternInfo.first.data(), patternInfo.second.c_str(), moduleSection);
}

//-----------------------------------------------------------------------------
// Purpose: find address of reference to string constant in executable memory
// Input  : *szString       - 
//          bNullTerminator - 
// Output : CMemory
//-----------------------------------------------------------------------------
CMemory CModule::FindString(const char* szString, const ptrdiff_t nOccurrence, bool bNullTerminator) const
{
	if (!m_ExecutableCode.IsSectionValid())
		return CMemory();

	const CMemory stringAddress = FindStringReadOnly(szString, bNullTerminator); // Get Address for the string in the .rdata section.

	if (!stringAddress)
		return CMemory();

	uint8* pLatestOccurrence = nullptr;
	uint8* pTextStart = reinterpret_cast<uint8*>(m_ExecutableCode.m_pSectionBase); // Get the start of the .text section.
	ptrdiff_t dOccurrencesFound = 0;
	CMemory resultAddress;

	for (size_t i = 0ull; i < m_ExecutableCode.m_nSectionSize - 0x5; i++)
	{
		byte byte = pTextStart[i];
		if (byte == 0x8D) // 0x8D = LEA
		{
			const CMemory skipOpCode = CMemory(reinterpret_cast<uintptr_t>(&pTextStart[i])).OffsetSelf(0x2); // Skip next 2 opcodes, those being the instruction and the register.
			const int32 relativeAddress = skipOpCode.GetValue<int32>();                                  // Get 4-byte long string relative Address
			const uintptr_t nextInstruction = skipOpCode.Offset(0x4).GetPtr();                               // Get location of next instruction.
			const CMemory potentialLocation = CMemory(nextInstruction + relativeAddress);                    // Get potential string location.

			if (potentialLocation == stringAddress)
			{
				dOccurrencesFound++;
				if (nOccurrence == dOccurrencesFound)
				{
					return CMemory(&pTextStart[i]);
				}

				pLatestOccurrence = &pTextStart[i]; // Stash latest occurrence.
			}
		}
	}

	return CMemory(pLatestOccurrence);
}

//-----------------------------------------------------------------------------
// Purpose: find address of input string constant in read only memory
// Input  : *szString       - 
//          bNullTerminator - 
// Output : CMemory
//-----------------------------------------------------------------------------
CMemory CModule::FindStringReadOnly(const char* szString, bool bNullTerminator) const
{
	if (!m_ReadOnlyData.IsSectionValid())
		return CMemory();

	const std::vector<int> vBytes = Utils::StringToBytes(szString, bNullTerminator); // Convert our string to a byte array.
	const std::pair<size_t, const int*> bytesInfo = std::make_pair<size_t, const int*>(vBytes.size(), vBytes.data()); // Get the size and data of our bytes.

	const uint8* pBase = reinterpret_cast<uint8*>(m_ReadOnlyData.m_pSectionBase); // Get start of .rdata section.

	for (size_t i = 0ull; i < m_ReadOnlyData.m_nSectionSize - bytesInfo.first; i++)
	{
		bool bFound = true;

		// If either the current byte equals to the byte in our pattern or our current byte in the pattern is a wildcard
		// our if clause will be false.
		for (size_t j = 0ull; j < bytesInfo.first; j++)
		{
			if (pBase[i + j] != bytesInfo.second[j] && bytesInfo.second[j] != -1)
			{
				bFound = false;
				break;
			}
		}

		if (bFound)
		{
			return CMemory(&pBase[i]);
		}
	}

	return CMemory();
}

CMemory CModule::FindStringData(const char* szString, bool bNullTerminator) const
{
	if (!m_RunTimeData.IsSectionValid())
		return CMemory();

	const std::vector<int> vBytes = Utils::StringToBytes(szString, bNullTerminator); // Convert our string to a byte array.
	const std::pair<size_t, const int*> bytesInfo = std::make_pair<size_t, const int*>(vBytes.size(), vBytes.data()); // Get the size and data of our bytes.

	const uint8* pBase = reinterpret_cast<uint8*>(m_RunTimeData.m_pSectionBase); // Get start of .rdata section.

	for (size_t i = 0ull; i < m_RunTimeData.m_nSectionSize - bytesInfo.first; i++)
	{
		bool bFound = true;

		// If either the current byte equals to the byte in our pattern or our current byte in the pattern is a wildcard
		// our if clause will be false.
		for (size_t j = 0ull; j < bytesInfo.first; j++)
		{
			if (pBase[i + j] != bytesInfo.second[j] && bytesInfo.second[j] != -1)
			{
				bFound = false;
				break;
			}
		}

		if (bFound)
		{
			return CMemory(&pBase[i]);
		}
	}

	return CMemory();
}

//-----------------------------------------------------------------------------
// Purpose: find 'free' page in r/w/x sections
// Input  : nSize - 
// Output : CMemory
//-----------------------------------------------------------------------------
CMemory CModule::FindFreeDataPage(const size_t nSize) const
{
	auto checkDataSection = [](const void* address, const std::size_t size)
	{
		MEMORY_BASIC_INFORMATION membInfo = { 0 };

		VirtualQuery(address, &membInfo, sizeof(membInfo));

		if (membInfo.AllocationBase && membInfo.BaseAddress && membInfo.State == MEM_COMMIT && !(membInfo.Protect & PAGE_GUARD) && membInfo.Protect != PAGE_NOACCESS)
		{
			if ((membInfo.Protect & (PAGE_EXECUTE_READWRITE | PAGE_READWRITE)) && membInfo.RegionSize >= size)
			{
				return ((membInfo.Protect & (PAGE_EXECUTE_READWRITE | PAGE_READWRITE)) && membInfo.RegionSize >= size) ? true : false;
			}
		}
		return false;
	};

	// This is very unstable, this doesn't check for the actual 'page' sizes.
	// Also can be optimized to search per 'section'.
	const uintptr_t endOfModule = m_pModuleBase + m_pNTHeaders->OptionalHeader.SizeOfImage - sizeof(uintptr_t);
	for (uintptr_t currAddr = endOfModule; m_pModuleBase < currAddr; currAddr -= sizeof(uintptr_t))
	{
		if (*reinterpret_cast<uintptr_t*>(currAddr) == 0 && checkDataSection(reinterpret_cast<void*>(currAddr), nSize))
		{
			bool bIsGoodPage = true;
			uint32 nPageCount = 0;

			for (; nPageCount < nSize && bIsGoodPage; nPageCount += sizeof(uintptr_t))
			{
				const uintptr_t pageData = *reinterpret_cast<uintptr_t*>(currAddr + nPageCount);
				if (pageData != 0)
					bIsGoodPage = false;
			}

			if (bIsGoodPage && nPageCount >= nSize)
				return currAddr;
		}
	}

	return CMemory();
}
// Get all xrefs to an address within a module section
std::vector<CMemory> CModule::GetXrefsTo(const uintptr_t address, const ModuleSections_t* moduleSection) const
{
	std::vector<CMemory> xrefs;

	if (!moduleSection || !moduleSection->IsSectionValid())
		return xrefs;

	// Convert the address to a byte array and a mask
	const uint8* pPattern = reinterpret_cast<const uint8*>(&address);
	const char* szMask = "xxxx"; // 4 bytes for x86, 8 bytes for x64

	// Get the start and end of the section
	const uintptr_t start = moduleSection->m_pSectionBase;
	const uintptr_t end = start + moduleSection->m_nSectionSize;
	uintptr_t current = start;
	int occurrence = 0;
	while (current && current < end)
	{
		// Find the next xref using SIMD pattern scan
		uintptr_t xref = FindPatternSIMD(pPattern, szMask, moduleSection, occurrence).GetPtr();

		// If the xref is 0, it means there are no more xrefs or we reached the end of the section
		if (!xref)
			break;

		// Add the xref to the vector and update the current position
		xrefs.emplace_back(xref);
		current = xref + sizeof(address);
		occurrence++;
	}

	return xrefs;
}

//-----------------------------------------------------------------------------
// Purpose: get address of a virtual method table by rtti type descriptor name
// Input  : *szTableName - 
//			nRefIndex    - 
// Output : CMemory
//-----------------------------------------------------------------------------
CMemory CModule::GetVirtualMethodTable(const char* szTableName, const size_t nRefIndex)
{
	// Type descriptor names look like this: .?AVC_CSPlayer@@ (so: ".?AV" + table_name + "@@")
	const std::string type_descriptor_name = ".?AV" + std::string(szTableName) + "@@";

	// Find the address of the type descriptor name in the read-only section
	const CMemory type_descriptor_name_address = FindStringData(type_descriptor_name.c_str(), true);

	// If not found, return an invalid address
	if (!type_descriptor_name_address)
		return CMemory();

	// The type descriptor is 8 bytes before the name
	const CMemory type_descriptor_address = type_descriptor_name_address.Offset(-8);

	// Get all xrefs to the type descriptor in the read-only section
	const std::vector<CMemory> xrefs = GetXrefsTo(type_descriptor_address, &m_ReadOnlyData);

	// If there are no xrefs or the reference index is out of bounds, return an invalid address
	if (xrefs.empty() || nRefIndex >= xrefs.size())
		return CMemory();
	for (const CMemory xref : xrefs) {
		// The offset of the vtable in the complete class is 8 bytes before the xref
		const int32 offset_from_class = xref.Offset(-8).GetValue<int32>();

		// If the offset is not 0, it means the vtable belongs to a base class, not the class we want
		if (offset_from_class != 0)
			continue;

		const CMemory object_locator = xref.Offset(-12);
		
		std::vector<CMemory> vtable_address = GetXrefsTo(object_locator, &m_ReadOnlyData);
		if (vtable_address[0])
			return vtable_address[0].Offset(0x4);
	}
	return CMemory();
}
//-----------------------------------------------------------------------------
// Purpose: get address of imported function in this module
// Input  : *szModuleName         - 
//          *szFunctionName       - 
//          bGetFunctionReference - 
// Output : CMemory
//-----------------------------------------------------------------------------
CMemory CModule::GetImportedFunction(const char* szModuleName, const char* szFunctionName, const bool bGetFunctionReference) const
{
	if (!m_pDOSHeader || m_pDOSHeader->e_magic != IMAGE_DOS_SIGNATURE) // Is dosHeader valid?
		return CMemory();

	if (!m_pNTHeaders || m_pNTHeaders->Signature != IMAGE_NT_SIGNATURE) // Is ntHeader valid?
		return CMemory();

	// Get the location of IMAGE_IMPORT_DESCRIPTOR for this module by adding the IMAGE_DIRECTORY_ENTRY_IMPORT relative virtual address onto our module base address.
	IMAGE_IMPORT_DESCRIPTOR* pImageImportDescriptors = reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(m_pModuleBase + m_pNTHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
	if (!pImageImportDescriptors)
		return CMemory();

	for (IMAGE_IMPORT_DESCRIPTOR* pIID = pImageImportDescriptors; pIID->Name != 0; pIID++)
	{
		// Get virtual relative Address of the imported module name. Then add module base Address to get the actual location.
		const char* szImportedModuleName = reinterpret_cast<char*>(reinterpret_cast<DWORD*>(m_pModuleBase + pIID->Name));

		if (_stricmp(szImportedModuleName, szModuleName) == 0) // Is this our wanted imported module?.
		{
			// Original First Thunk to get function name.
			IMAGE_THUNK_DATA* pOgFirstThunk = reinterpret_cast<IMAGE_THUNK_DATA*>(m_pModuleBase + pIID->OriginalFirstThunk);

			// To get actual function address.
			IMAGE_THUNK_DATA* pFirstThunk = reinterpret_cast<IMAGE_THUNK_DATA*>(m_pModuleBase + pIID->FirstThunk);
			for (; pOgFirstThunk->u1.AddressOfData; ++pOgFirstThunk, ++pFirstThunk)
			{
				// Get image import by name.
				const IMAGE_IMPORT_BY_NAME* pImageImportByName = reinterpret_cast<IMAGE_IMPORT_BY_NAME*>(m_pModuleBase + pOgFirstThunk->u1.AddressOfData);

				if (memcmp(pImageImportByName->Name, szFunctionName, strlen(szFunctionName) + 1) == 0)
				{
					// Grab function address from firstThunk.
#if _WIN64
					uintptr_t* pFunctionAddress = &pFirstThunk->u1.Function;
#else
					uintptr_t* pFunctionAddress = reinterpret_cast<uintptr_t*>(&pFirstThunk->u1.Function);
#endif // #if _WIN64

					// Reference or address?
					return bGetFunctionReference ? CMemory(pFunctionAddress) : CMemory(*pFunctionAddress); // Return as CMemory class.
				}
			}

		}
	}
	return CMemory();
}

//-----------------------------------------------------------------------------
// Purpose: get address of exported function in this module
// Input  : *szFunctionName - 
//          bNullTerminator - 
// Output : CMemory
//-----------------------------------------------------------------------------
CMemory CModule::GetExportedFunction(const char* szFunctionName) const
{
	if (!m_pDOSHeader || m_pDOSHeader->e_magic != IMAGE_DOS_SIGNATURE) // Is dosHeader valid?
		return CMemory();

	if (!m_pNTHeaders || m_pNTHeaders->Signature != IMAGE_NT_SIGNATURE) // Is ntHeader valid?
		return CMemory();

	// Get the location of IMAGE_EXPORT_DIRECTORY for this module by adding the IMAGE_DIRECTORY_ENTRY_EXPORT relative virtual address onto our module base address.
	const IMAGE_EXPORT_DIRECTORY* pImageExportDirectory = reinterpret_cast<IMAGE_EXPORT_DIRECTORY*>(m_pModuleBase + m_pNTHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
	if (!pImageExportDirectory)
		return CMemory();

	// Are there any exported functions?
	if (!pImageExportDirectory->NumberOfFunctions)
		return CMemory();

	// Get the location of the functions via adding the relative virtual address from the struct into our module base address.
	const DWORD* pAddressOfFunctions = reinterpret_cast<DWORD*>(m_pModuleBase + pImageExportDirectory->AddressOfFunctions);
	if (!pAddressOfFunctions)
		return CMemory();

	// Get the names of the functions via adding the relative virtual address from the struct into our module base Address.
	const DWORD* pAddressOfName = reinterpret_cast<DWORD*>(m_pModuleBase + pImageExportDirectory->AddressOfNames);
	if (!pAddressOfName)
		return CMemory();

	// Get the ordinals of the functions via adding the relative virtual Address from the struct into our module base address.
	DWORD* pAddressOfOrdinals = reinterpret_cast<DWORD*>(m_pModuleBase + pImageExportDirectory->AddressOfNameOrdinals);
	if (!pAddressOfOrdinals)
		return CMemory();

	for (DWORD i = 0; i < pImageExportDirectory->NumberOfFunctions; i++) // Iterate through all the functions.
	{
		// Get virtual relative Address of the function name. Then add module base Address to get the actual location.
		const char* ExportFunctionName = reinterpret_cast<char*>(reinterpret_cast<DWORD*>(m_pModuleBase + pAddressOfName[i]));

		if (strcmp(ExportFunctionName, szFunctionName) == 0) // Is this our wanted exported function?
		{
			// Get the function ordinal. Then grab the relative virtual address of our wanted function. Then add module base address so we get the actual location.
			return CMemory(m_pModuleBase + pAddressOfFunctions[reinterpret_cast<WORD*>(pAddressOfOrdinals)[i]]); // Return as CMemory class.
		}
	}
	return CMemory();
}

//-----------------------------------------------------------------------------
// Purpose: get the module section by name (example: '.rdata', '.text')
// Input  : *szSectionName - 
// Output : ModuleSections_t
//-----------------------------------------------------------------------------
CModule::ModuleSections_t CModule::GetSectionByName(const char* szSectionName) const
{
	for (const ModuleSections_t& section : m_ModuleSections)
	{
		if (section.m_SectionName.compare(szSectionName) == 0)
			return section;
	}

	return ModuleSections_t();
}

#if _WIN64
//-----------------------------------------------------------------------------
// Purpose: unlink module from peb
//-----------------------------------------------------------------------------
void CModule::UnlinkFromPEB() const // Disclaimer: This does not bypass GetMappedFileName. That function calls NtQueryVirtualMemory which does a syscall to ntoskrnl for getting info on a section.
{
#define UNLINK_FROM_PEB(entry)    \
	(entry).Flink->Blink = (entry).Blink; \
	(entry).Blink->Flink = (entry).Flink;

	const PEB64* processEnvBlock = reinterpret_cast<PEB64*>(__readgsqword(0x60)); // https://en.wikipedia.org/wiki/Win32_Thread_Information_Block
	const LIST_ENTRY* inLoadOrderList = &processEnvBlock->Ldr->InLoadOrderModuleList;

	for (LIST_ENTRY* entry = inLoadOrderList->Flink; entry != inLoadOrderList; entry = entry->Flink)
	{
		const PLDR_DATA_TABLE_ENTRY pldrEntry = reinterpret_cast<PLDR_DATA_TABLE_ENTRY>(entry->Flink);
		const std::uintptr_t baseAddr = reinterpret_cast<std::uintptr_t>(pldrEntry->DllBase);

		if (baseAddr != m_pModuleBase)
			continue;

		UNLINK_FROM_PEB(pldrEntry->InInitializationOrderLinks);
		UNLINK_FROM_PEB(pldrEntry->InMemoryOrderLinks);
		UNLINK_FROM_PEB(pldrEntry->InLoadOrderLinks);
		break;
	}
#undef UNLINK_FROM_PEB
}
#endif // #if _WIN64 
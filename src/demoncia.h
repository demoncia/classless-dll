#pragma once

#include "MQ2Main.h"

char __fastcall DisableMap_Trampoline(char* pThis);
char __fastcall DisableMap_Detour(char* pThis) { return 0;  }

DETOUR_TRAMPOLINE_EMPTY(char __fastcall DisableMap_Trampoline(char* pThis));

// Hooks to CMapViewWnd::AboutToShow
void DisableMap() { EzDetour((((DWORD)0x006CF4A0 - 0x400000) + baseAddress), DisableMap_Detour, DisableMap_Trampoline); };


char* __fastcall InjectCustomZones_Trampoline(char* pThis, char* pPtr, unsigned __int32 zoneType, unsigned __int32 zoneID, char* zoneShortName, char* zoneLongName, unsigned __int32 eqStrID, __int32 zoneFlags2, __int32 x, __int32 y, __int32 z);
char* __fastcall InjectCustomZones_Detour(char* pThis, char* pPtr, unsigned __int32 zoneType, unsigned __int32 zoneID, char* zoneShortName, char* zoneLongName, unsigned __int32 eqStrID, __int32 zoneFlags2, __int32 x, __int32 y, __int32 z)
{
	if (!strcmp(zoneShortName, "interiorwalltest")) {
		DebugSpew("injecting zone hollows id 787");
		InjectCustomZones_Trampoline(pThis, pPtr, 0, 787, "hollows", "Darkened Hollows", 35154, 4, 0, 0, 0);
		DebugSpew("injecting zone delves id 788");
		InjectCustomZones_Trampoline(pThis, pPtr, 0, 788, "delves", "Demonic Delves", 35155, 4, 0, 0, 0);
	}
	DebugSpew("loaded zone %s id %d", zoneShortName, zoneID);
	return InjectCustomZones_Trampoline(pThis, pPtr, zoneType, zoneID, zoneShortName, zoneLongName, eqStrID, zoneFlags2, x, y, z);
}

DETOUR_TRAMPOLINE_EMPTY(char* __fastcall InjectCustomZones_Trampoline(char* pThis, char* pPtr, unsigned __int32 zoneType, unsigned __int32 zoneID, char* zoneShortName, char* zoneLongName, unsigned __int32 eqStrID, __int32 zoneFlags2, __int32 x, __int32 y, __int32 z));

// Hooks to EQWorldData::AddZone
void InjectCustomZones() { EzDetour((((DWORD)0x007DC430 - 0x400000) + baseAddress), InjectCustomZones_Detour, InjectCustomZones_Trampoline); };



unsigned int __cdecl DisableLuclinModels_Trampoline(char* lpAppName, char* lpKeyName, char* lpDefault, char* lpReturnedString, size_t nSize, char* lpFileName);
unsigned int __cdecl DisableLuclinModels_Detour(char* lpAppName, char* lpKeyName, char* lpDefault, char* lpReturnedString, size_t nSize, char* lpFileName)
{
	// lpAppName should be "Defaults"
	if (strcmp(lpAppName, "Defaults")) return DisableLuclinModels_Trampoline(lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, lpFileName);

	// compare Name and find luclin in it
	if (strstr(lpKeyName, "UseLuclin")) {
		strncpy(lpReturnedString, "FALSE", 5);
		return strlen(lpReturnedString);
	}

	return DisableLuclinModels_Trampoline(lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, lpFileName);
}

DETOUR_TRAMPOLINE_EMPTY(unsigned int __cdecl DisableLuclinModels_Trampoline(char* lpAppName, char* lpKeyName, char* lpDefault, char* lpReturnedString, size_t nSize, char* lpFileName));

// Hooks to GetINIFile
void DisableLuclinModels() { EzDetour((((DWORD)0x00860EF0 - 0x400000) + baseAddress), DisableLuclinModels_Detour, DisableLuclinModels_Trampoline); }

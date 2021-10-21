#pragma once

#include "MQ2Main.h"

char __fastcall DisableCMapViewWnd_Trampoline(char* pThis);
char __fastcall DisableCMapViewWnd_Detour(char* pThis) { return 0;  }
DETOUR_TRAMPOLINE_EMPTY(char __fastcall DisableCMapViewWnd_Trampoline(char* pThis));
// Hooks to CMapViewWnd::AboutToShow
void DisableCMapViewWnd() { EzDetour((((DWORD)0x006CF4A0 - 0x400000) + baseAddress), DisableCMapViewWnd_Detour, DisableCMapViewWnd_Trampoline); };

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
//bool __thiscall sub_7DC430(_DWORD *this, int a2, int a3, char *a4, char *a5, int a6, int a7, int a8, int a9, int a10)

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


char __fastcall DisableCBazaarSearchWnd_Trampoline(char* pThis);
char __fastcall DisableCBazaarSearchWnd_Detour(char* pThis) { return 0; }
DETOUR_TRAMPOLINE_EMPTY(char __fastcall DisableCBazaarSearchWnd_Trampoline(char* pThis));
// Hooks to CBazaarSearchWnd::AboutToShow
void DisableCBazaarSearchWnd() { EzDetour((((DWORD)0x00636670 - 0x400000) + baseAddress), DisableCBazaarSearchWnd_Detour, DisableCBazaarSearchWnd_Trampoline); };


char* __fastcall InjectCustomRaces_Trampoline(char* pThis, char* pPtr, int raceID, int genderID, const char* modelName, int a5, int isEnabled);
char* __fastcall InjectCustomRaces_Detour(char*pThis, char* pPtr, int raceID, int genderID, const char* modelName, int a5, int isEnabled)
{
	if (!raceID) return InjectCustomRaces_Trampoline(pThis, pPtr, raceID, genderID, modelName, a5, isEnabled);
	if (raceID == 723) {
		struct NPC {
			explicit NPC(int raceID, const char* modelName,  int genderID) : raceID(raceID), modelName(modelName), genderID(genderID) {};
			int raceID;
			const char* modelName;			
			int genderID;
		};

		static NPC NPCs[] = {
			NPC(728, "GBM", 2),
			NPC(771, "TRX", 2),
			NPC(774, "ANK", 2),
			NPC(810, "DFM", 2),
			NPC(812, "KGB", 2),
			NPC(827, "QUE", 2),
			NPC(829, "VLK", 2),			
			NPC(839, "TKA", 2),
			NPC(848, "VS2", 2),
			NPC(848, "VS2", 2),
			NPC(856, "AE3", 2),
			NPC(860, "DMF", 0),
			NPC(860, "DMM", 1),
			NPC(866, "RZ3", 2),
			NPC(876, "TSR", 2),
			NPC(877, "TSF", 2),
			NPC(880, "DGZ", 2),
			NPC(888, "PVP", 2),			
			NPC(889, "BBT", 2),
			NPC(890, "TKL", 2),
			NPC(891, "TG2", 2),
			NPC(892, "NT2", 2),
			NPC(893, "ZL2", 2),
			NPC(894, "ZLM", 2),
			NPC(895, "SE2", 2),
			NPC(896, "GOM", 0),
			NPC(896, "GOF", 1),
			NPC(897, "OW2", 2),
			NPC(898, "OWM", 2),
			NPC(899, "TCK", 2),
			NPC(900, "BLB", 2),
			NPC(901, "SG2", 2),
		};
		for (auto&& npc : NPCs) {
			DebugSpew("injecting race %s gender %d id %d", npc.modelName , npc.genderID, npc.raceID);
			InjectCustomRaces_Trampoline(pThis, pPtr, npc.raceID, npc.genderID, npc.modelName, a5, isEnabled);
		}
		
	}
	DebugSpew("loaded race %s gender %d as id %d", modelName, genderID, raceID);
	return InjectCustomRaces_Trampoline(pThis, pPtr, raceID, genderID, modelName, a5, isEnabled);
}
DETOUR_TRAMPOLINE_EMPTY(char* __fastcall InjectCustomRaces_Trampoline(char* pThis, char* pPtr, int raceID, int genderID, const char* modelName, int a5, int isEnabled));
// Hooks to CRaceGenderInfoManager::AddRaceGender
void InjectCustomRaces() { EzDetour((((DWORD)0x0050A440 - 0x400000) + baseAddress), InjectCustomRaces_Detour, InjectCustomRaces_Trampoline); };
//signed int __thiscall CRaceGenderInfoManager::AddRaceGender(signed int* this, int a2, int a3, const char* a4, int a5, int a6)

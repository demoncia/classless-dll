//  This File Has Came From MMOBugs.com
//  Please Do Not Distribute Without Authors or MMOBugs Consent. 

// MQ2MMODPS.cpp : Defines the entry point for the DLL application.
//

// PLUGIN_API is only to be used for callbacks.  All existing callbacks at this time
// are shown below. Remove the ones your plugin does not use.  Always use Initialize
// and Shutdown for setup and cleanup, do NOT do it in DllMain.


#include "MQ2Main.h"
#include "FloatingTextManager.h"
#include <chrono>  // for high_resolution_clock
#include <sstream>
extern CDPSAdvWnd *DPSWnd;

//#define DPSDEBUG

#define STRINGSIZE MAX_STRING
#define CC_NONMELEE 			283
#define	CC_SPELLS   			264

#define SpawnMe()           	((PSPAWNINFO)pCharSpawn)
#define PetID()             	((PSPAWNINFO)pCharSpawn)->PetID
#define GetSpawnID(spawnid) 	(PSPAWNINFO)GetSpawnByID(spawnid)
#define Target()            	((PSPAWNINFO)pTarget)

#define sONOFF                  "\agON":"\arOFF"

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

LONG          Report            = true;		// Module DPS Report?
LONG          PopUps            = true;		// Module DPS PopUps?
std::map<uint32_t, EdgeDPSEntry> DamageEntries;
extern FloatingTextManager* g_pFtm;

std::chrono::time_point<std::chrono::steady_clock> lastTimeStamp = std::chrono::high_resolution_clock::now();


CHAR          dpsWRITE[STRINGSIZE];             // DPS Output String!

#define       pDamage ((pCombatDamage_Struct)Packet)
#define       pAction ((pAction_Struct)Packet)
#define       pCorpse ((pDeath_Struct)Packet)
#define       pFormat ((pFormattedMessage_Struct)Packet)
#define       pDeleteSpawn ((pDeleteSpawn_Struct)Packet)
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//

static inline LONG Activated()
{
	return true;
}

LONG WriteINIbool(PCHAR ini, PCHAR sec, PCHAR key, PCHAR val, LONG cur) 
{
	long result;
	if(!_stricmp("false",val) || !_stricmp("off",val) || !_stricmp("0",val))             result=0;
	else if(!_stricmp("true",val) || !_stricmp("on",val) || !_stricmp("1",val))          result=1;
	else if(!_stricmp("auto",val) || !_stricmp("toggle",val) || !_stricmp("switch",val)) result=!cur;
	else                                                                              result=cur;
	result&=1;
	if(ini && ini[0] && sec && sec[0] && key && key[0]) WritePrivateProfileString(sec,key,result?"1":"0",ini);
	return result;
}

EdgeDPSEntry GetEdgeDPSEntryByID(DWORD id, bool bAdd)
{
	auto timestamp = std::chrono::high_resolution_clock::now();
	EdgeDPSEntry entry;
	memset(&entry, 0, sizeof(EdgeDPSEntry));

	auto found = DamageEntries.find(id);

	if (found != DamageEntries.end())
	{
		entry = found->second;
	}
	else
	{
		if (bAdd && id > 0)
		{
			entry.SpawnID = id;
			entry.BeginningTimestamp = timestamp;
			entry.weDamaged = true;
			SetEdgeDPSEntryByID(id, entry);
		}
	}
	return entry;
}

void SetEdgeDPSEntryByID(DWORD id, EdgeDPSEntry entry)
{
	auto timestamp = std::chrono::high_resolution_clock::now();
	entry.LastUpdateTimestamp = timestamp;
	lastTimeStamp = timestamp;
	DamageEntries[id] = entry;
}

PSTR GetCustomArg(PSTR szDest, PCSTR szSrc, DWORD dwNumber, DWORD Size=MAX_STRING, BOOL LeaveQuotes=0, BOOL ToParen=0, BOOL CSV=0, CHAR Separator=0, BOOL AnyNonAlphaNum=0);

PSTR GetCustomArg(PSTR szDest, PCSTR szSrc, DWORD dwNumber, DWORD Size, BOOL LeaveQuotes, BOOL ToParen, BOOL CSV, CHAR Separator, BOOL AnyNonAlphaNum)
{
	DWORD i=0;
	DWORD j=0;
	BOOL CustomSep = FALSE;
	BOOL InQuotes = FALSE;
	PCSTR szTemp = szSrc;
	ZeroMemory(szDest,Size);

	if (Separator!=0) CustomSep=TRUE;

	szTemp = GetNextArg(szTemp,dwNumber-1, CSV, Separator);

	while ((
		((CustomSep) || (szTemp[i] != ' '))
		&& ((CustomSep) || (szTemp[i] != '\t'))
		&& ((CustomSep) || (!CSV) || (szTemp[i]!=','))
		&& ((!CustomSep) || (szTemp[i]!=Separator))
		&& ((!AnyNonAlphaNum) || (
		(szTemp[i]>='0' && szTemp[i]<='9') ||
		(szTemp[i]>='a' && szTemp[i]<='z') ||
		(szTemp[i]>='A' && szTemp[i]<='Z') ||
		(szTemp[i]=='_')
		))
		&& (szTemp[i] != 0)
		&& ((!ToParen) || (szTemp[i] !=')'))
		)
		|| (InQuotes)
		) {
			if ((szTemp[i] == 0) && (InQuotes)) {
				return szDest;
			}
			if (szTemp[i] == '"') {
				InQuotes = !InQuotes;
				if (LeaveQuotes) {
					szDest[j] = szTemp[i];
					j++;
				}
			} else {
				szDest[j] = szTemp[i];
				j++;
			}
			i++;
	}
	if ((ToParen) && (szTemp[i]==')')) szDest[j]=')';

	return szDest;
}

extern bool ClearAfterCombat;

BOOL OnRecvEdgeDPSPacket(DWORD Type, PVOID Packet, DWORD Size)
{
	if (gGameState == GAMESTATE_INGAME && pCharSpawn && Activated())
	{
		if (Type == OP_Damage)
		{
#ifdef DPSDEBUG
			WriteChatf("OP_CombatAction");
#endif
			bool verified = false;
			if (SpawnMe()->SpawnID == pAction->source)         verified = true;
			else if (PetID() > 0 && PetID() == pAction->source)  verified = true;
			else if (PSPAWNINFO SourceID = GetSpawnID(pAction->source))
			{
				if (SourceID->MasterID > 0)
				{
					PSPAWNINFO MasterMob = GetSpawnID(SourceID->MasterID);
					if (SpawnMe() == MasterMob)
						verified = true;
					else if (IsMySwarmPet(MasterMob))
						verified = true;
					else if (IsGroupMember(MasterMob))
						verified = true;
					else if (IsGroupMemberSwarmPet(MasterMob))
						verified = true;
					else if (IsRaidMember(MasterMob))
						verified = true;
					else if (IsRaidMemberSwarmPet(MasterMob))
						verified = true;
				}

				if (SpawnMe() == SourceID)
					verified = true;
				else if (IsMySwarmPet(SourceID))
					verified = true;
				else if (IsGroupMember(SourceID))
					verified = true;
				else if (IsGroupMemberSwarmPet(SourceID))
					verified = true;
				else if (IsRaidMember(SourceID))
					verified = true;
				else if (IsRaidMemberSwarmPet(SourceID))
					verified = true;
			}
			if (SpawnMe()->SpawnID == pAction->target)         verified = true;
			else if (PetID() > 0 && PetID() == pAction->target)  verified = true;
			else if (PSPAWNINFO SourceID = GetSpawnID(pAction->target))
			{
				if (SourceID->MasterID > 0)
				{
					PSPAWNINFO MasterMob = GetSpawnID(SourceID->MasterID);
					if (SpawnMe()->SpawnID == MasterMob->SpawnID)
						verified = true;
					else if (IsMySwarmPet(MasterMob))
						verified = true;
					else if (IsGroupMember(MasterMob))
						verified = true;
					else if (IsGroupMemberSwarmPet(MasterMob))
						verified = true;
					else if (IsRaidMember(MasterMob))
						verified = true;
					else if (IsRaidMemberSwarmPet(MasterMob))
						verified = true;
				}

				if (SpawnMe()->SpawnID == SourceID->MasterID)
					verified = true;
				else if (IsMySwarmPet(SourceID))
					verified = true;
				else if (IsGroupMember(SourceID))
					verified = true;
				else if (IsGroupMemberSwarmPet(SourceID))
					verified = true;
				else if (IsRaidMember(SourceID))
					verified = true;
				else if (IsRaidMemberSwarmPet(SourceID))
					verified = true;
			}

			if (verified)
			{

				EdgeDPSEntry entry;
				memset(&entry, 0, sizeof(EdgeDPSEntry));

				EdgeDPSEntry killerentry;
				memset(&killerentry, 0, sizeof(EdgeDPSEntry));

				auto HaveTarget = GetSpawnID(pDamage->target);
				auto killerMob = GetSpawnID(pDamage->source);
				if (HaveTarget)
				{
					int ibreak = 0;

					while (HaveTarget && HaveTarget->MasterID != 0)
					{
						if (ibreak > 5)
							break;

						ibreak++;
						HaveTarget = GetSpawnID(HaveTarget->MasterID);
					}

					ibreak = 0;
					while (killerMob && killerMob->MasterID != 0)
					{
						if (ibreak > 5)
							break;

						ibreak++;
						killerMob = GetSpawnID(killerMob->MasterID);


					}

					if (HaveTarget)
					{
						entry = GetEdgeDPSEntryByID(HaveTarget->SpawnID);
						killerentry = GetEdgeDPSEntryByID(killerMob ? killerMob->SpawnID : HaveTarget->SpawnID);
						if (verified)
						{
							int nType = 0;
							if (!entry.InitialSpawnName[0])
								strncpy_s(entry.InitialSpawnName, HaveTarget->DisplayedName, 64);
							if (!killerentry.InitialSpawnName[0])
								strncpy_s(entry.InitialSpawnName, killerMob ? killerMob->DisplayedName : HaveTarget->DisplayedName, 64);
							SetEdgeDPSEntryByID(entry.SpawnID, entry);
							SetEdgeDPSEntryByID(killerentry.SpawnID, killerentry);
							int32_t nDamage = pDamage->damage;
							if (nDamage > 0)
								g_pFtm->AddDamageText(HaveTarget, killerMob ? killerMob : HaveTarget, nDamage, pDamage->spellid, nType);
						}
					}
				}
			}
		}
		else if (Type == OP_CombatAction)
		{

		if (pAction->effect_flag == 0x04)
			return true;
#ifdef DPSDEBUG
			WriteChatf("OP_CombatAction");
#endif
			bool verified = false;
			if (SpawnMe()->SpawnID == pAction->source)         verified = true;
			else if (PetID() > 0 && PetID() == pAction->source)  verified = true;
			else if (PSPAWNINFO SourceID = GetSpawnID(pAction->source))
			{
				if (SourceID->MasterID > 0)
				{
					PSPAWNINFO MasterMob = GetSpawnID(SourceID->MasterID);
					if (SpawnMe() == MasterMob)
						verified = true;
					else if (IsMySwarmPet(MasterMob))
						verified = true;
					else if (IsGroupMember(MasterMob))
						verified = true;
					else if (IsGroupMemberSwarmPet(MasterMob))
						verified = true;
					else if (IsRaidMember(MasterMob))
						verified = true;
					else if (IsRaidMemberSwarmPet(MasterMob))
						verified = true;
				}

				if (SpawnMe() == SourceID)
					verified = true;
				else if (IsMySwarmPet(SourceID))
					verified = true;
				else if (IsGroupMember(SourceID))
					verified = true;
				else if (IsGroupMemberSwarmPet(SourceID))
					verified = true;
				else if (IsRaidMember(SourceID))
					verified = true;
				else if (IsRaidMemberSwarmPet(SourceID))
					verified = true;
			}
			if (SpawnMe()->SpawnID == pAction->target)         verified = true;
			else if (PetID() > 0 && PetID() == pAction->target)  verified = true;
			else if (PSPAWNINFO SourceID = GetSpawnID(pAction->target))
			{
				if (SourceID->MasterID > 0)
				{
					PSPAWNINFO MasterMob = GetSpawnID(SourceID->MasterID);
					if (SpawnMe()->SpawnID == MasterMob->SpawnID)
						verified = true;
					else if (IsMySwarmPet(MasterMob))
						verified = true;
					else if (IsGroupMember(MasterMob))
						verified = true;
					else if (IsGroupMemberSwarmPet(MasterMob))
						verified = true;
					else if (IsRaidMember(MasterMob))
						verified = true;
					else if (IsRaidMemberSwarmPet(MasterMob))
						verified = true;
				}

				if (SpawnMe()->SpawnID == SourceID->MasterID)
					verified = true;
				else if (IsMySwarmPet(SourceID))
					verified = true;
				else if (IsGroupMember(SourceID))
					verified = true;
				else if (IsGroupMemberSwarmPet(SourceID))
					verified = true;
				else if (IsRaidMember(SourceID))
					verified = true;
				else if (IsRaidMemberSwarmPet(SourceID))
					verified = true;
			}

			if (verified)
			{

				EdgeDPSEntry entry;
				memset(&entry, 0, sizeof(EdgeDPSEntry));

				EdgeDPSEntry killerentry;
				memset(&killerentry, 0, sizeof(EdgeDPSEntry));

				auto HaveTarget = GetSpawnID(pAction->target);
				auto killerMob = GetSpawnID(pAction->source);
				if (HaveTarget)
				{
					int ibreak = 0;

					while (HaveTarget && HaveTarget->MasterID != 0)
					{
						if (ibreak > 5)
							break;

						ibreak++;
						HaveTarget = GetSpawnID(HaveTarget->MasterID);
					}

					ibreak = 0;
					while (killerMob && killerMob->MasterID != 0)
					{
						if (ibreak > 5)
							break;

						ibreak++;
						killerMob = GetSpawnID(killerMob->MasterID);


					}

					if (HaveTarget)
					{
						entry = GetEdgeDPSEntryByID(HaveTarget->SpawnID);
						killerentry = GetEdgeDPSEntryByID(killerMob ? killerMob->SpawnID : HaveTarget->SpawnID);
						if (verified)
						{

							int nType = 0;

							int32_t nDamage = 0;

							auto spellAction = GetSpellByID(pAction->spell);
							if (spellAction)
							{

								for (int i = 0; i <= 11; i++)
								{
									if (spellAction->Attrib[i] == 0)
									{
										nDamage += ReturnValueCalculate(spellAction, i, pAction->spell_level, 1);
									}
								}
								if (spellAction->SpellType > 0)
									nType = 1;
								else
								{
									if (!entry.InitialSpawnName[0])
										strncpy_s(entry.InitialSpawnName, HaveTarget->DisplayedName, 64);
									if (!killerentry.InitialSpawnName[0])
										strncpy_s(entry.InitialSpawnName, killerMob ? killerMob->DisplayedName : HaveTarget->DisplayedName, 64);
									SetEdgeDPSEntryByID(entry.SpawnID, entry);
									SetEdgeDPSEntryByID(killerentry.SpawnID, killerentry);
								}

								if(nDamage > 0)
								{
									g_pFtm->AddDamageText(HaveTarget, killerMob ? killerMob : HaveTarget, nDamage, pAction->spell, nType);
								}
							}
						}
					}

				}
			}
		}
	}
	return true;
}

#include "MQ2AdvDps.h"

VOID DPSCommand(PSPAWNINFO pChar, PCHAR zLine)
{
	char Tmp[STRINGSIZE]; char Var[STRINGSIZE]; char Set[STRINGSIZE]; int Parm=1; bool needhelp=true;
	do 
	{
		GetCustomArg(Tmp,zLine,Parm++,STRINGSIZE);
		GetCustomArg(Var,Tmp,1,STRINGSIZE,FALSE,FALSE,FALSE,'=');
		GetCustomArg(Set,Tmp,2,STRINGSIZE,FALSE,FALSE,FALSE,'=');
		if (!_stricmp("reset", Var))
		{
			needhelp = false;
			DamageEntries.clear();
		}
		else if (!_stricmp("toggle", Var))  if (DPSWnd)
		{
			((CXWnd*)DPSWnd)->Show(1, 1);
			CheckActive();
			needhelp = false;
		}
	} while (Tmp[0]);

	if(needhelp)
	{
		WriteChatf("USAGE: /dps reset | toggle");
	}
}

PLUGIN_API VOID InitializeEdgeDPSPlugin(VOID)
{
	AddCommand("/dps",DPSCommand);
}

PLUGIN_API VOID SetEdgeDPSGameState(DWORD GameState)
{
	DamageEntries.clear();

}

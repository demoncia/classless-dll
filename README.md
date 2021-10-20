# DLL

This is the dinput.dll included in client. Visit https://github.com/demoncia/client for the deployed code


## TODO
- [ ] DisableCAAwnd
- [ ] DisableAchievement (not needed, it can't be sent without a proper packet reply from server anyways)
- [ ] DisableAdventure
- [ ] DisableAggroMeter
- [ ] DisableCAlarmWnd
- [ ] DisableAlerts
- [ ] DisableAudioTriggers
- [ ] DisableCAuraWnd
- [ ] DisableCBandolierWnd
- [ ] DisableCBlockedBuffWnd
- [ ] DisableCBodyTintWnd
- [x] DisableCBazaarSearchWnd
- [ ] DisableCBazaarWnd
- [ ] DisableCBarterMerchantWnd
- [ ] DisableCBarterSearchWnd
- [ ] DisableCGroupMentorWnd
- [ ] DisableCGuildBankWnd
- [ ] DisableCGuildMgmtWnd
- [ ] DisableCTributeMasterWnd
- [ ] DisableCBarterWnd
- [ ] DisableCDynamicZoneWnd
- [ ] DisableClaimWindow
- [ ] DisableCCombatAbilityWnd
- [ ] DisableCTaskWnd
- [ ] DisableCTributeBenefitWnd
- [ ] DisableCTributeMasterWnd
- [ ] DisableExpeditions
- [ ] DisableExtendedTarget (/)
- [ ] DisableFellowship
- [ ] DisableCFindLocationWnd
- [ ] DisableLeadership
- [ ] DisableLeaveRealEstate (no ui, 0x1da9 4 bytes)
- [ ] DisableLookingForGroup
- [ ] DisableLookingForGuild
- [ ] DisableMail
- [ ] DisableMarketplace
- [ ] DisableMarketplace
- [ ] DisableMusicPlayer (/mp3)
- [ ] DisableNPCJournal
- [ ] DisablePolls
- [ ] DisableQuestJournal
- [ ] DisableRaid
- [ ] DisableRealEstateItems (0x2fa0 336 bytes
- [ ] DisableRealEstateTrophies
- [ ] DisableReward
- [ ] DisableSocialSharing
- [ ] DisableCStoryWnd
- [ ] DisableTGCStartupWnd (LegendofNorrath)
- [ ] DisableUpdateEQPlayers (no UI, sends 0x6f8d 4 bytes)
- [ ] DisableCVoiceMacroWnd
- [ ] DisableZoneGuide
- [ ] Hijack PlayerProfilePage link: http://eqplayers.station.sony.com/character_profile.vm?characterId=0
- [x] DisableCMapViewWnd


## Disable ASLR for eqgame.exe


This has already been done for the eqgame.exe in the demoncia patcher
- Download https://blog.didierstevens.com/2010/10/17/setdllcharacteristics/
- setdllcharacteristics -d eqgame.exe



## zone data
- zone shortname list does not include clz, wall
offset: 0xAC7B80
airplane x0 then 22 x0, 0x5, 3 x0, 0x5, 3 0x0, 0x8, 3 0x0

- zone full name list (does include clz, wall)
offset: 0xA0AB90
interiorwalltest 3 x0, Wall Wall Wall, x0 x0
burnedwoods x0, The Burned Woods x0 x0 x0 x0
pomischief x0, The Plane of Mischief x0 x0 x0
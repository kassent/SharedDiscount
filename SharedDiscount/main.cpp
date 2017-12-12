#include "f4se_common\Relocation.h"
#include "f4se_common\BranchTrampoline.h"
#include "xbyak\xbyak.h"
#include <shlobj.h>
#include <math.h>
#include "Definations.h"

/*
>>>>> NOTE
41 B0 01 49 8B 86 ? ? ? ? 48 03 88 ? ? ? ? 48 8B 09 48 8B 01 FF 50 40 45 33 E4

*/

//GetAttributeDataByID_1704DA0(v38, 25i64);

using _GetDiscount = void(*)(Actor * pCustomer, Actor * pMerchant, float[]);
RelocAddr<_GetDiscount>		GetDiscount(0x1092210);

using _CalcWareValue = UInt32(*)(void *, Actor * pCustomer, Actor * pMerchant, bool isSell, UInt32 count, bool unk);
RelocAddr<_CalcWareValue>	CalcWareValue(0xE81400);
_CalcWareValue				CalcWareValue_Original = nullptr;

using _CalcGoodValue = UInt32(*)(void *, Character * pCustomer, Character * pMerchant, bool isSell, UInt32 count);
RelocAddr<_CalcGoodValue>	CalcGoodValue(0x12BC590);
_CalcGoodValue				CalcGoodValue_Original = nullptr;

uintptr_t	* pGameDataHandler = nullptr;
uintptr_t	* pGameDataOffset = nullptr;


Actor * LookUpFormByID(UInt32 formID)
{
	//	void * pDataHandler = *(void **)(*(uintptr_t *)(*(uintptr_t *)((*RelocPtr<uintptr_t>(0x1FBF5B8)) + 0x108) + 0x90) + 0x70 * (*RelocPtr<uintptr_t>(0x1EF0870)));
	void * pDataHandler = *(void **)(*(uintptr_t *)(*(uintptr_t *)((*pGameDataHandler) + 0x108) + 0x90) + 0x70 * (*pGameDataOffset));
	using _LookUpFormByID = Actor *(*)(void *, UInt32 &, bool);
	_LookUpFormByID * LookUpFormByID = reinterpret_cast<_LookUpFormByID*>(*(uintptr_t*)pDataHandler + 0x40);
	void * pObj = (*LookUpFormByID)(pDataHandler, formID, true);
	if (!pObj || (uintptr_t)pObj >> 0x3E & 1) return nullptr;
	return reinterpret_cast<Actor *>((uintptr_t)pObj - 8);
}

UInt32 CalcWareValue_Hook(void * pObj, Actor * pCustomer, Actor * pMerchant, bool isSell, UInt32 count, bool unk)
{
	if (pCustomer && pMerchant && (*g_characterManager) != nullptr)
	{
		SInt32 maxDiscount = -100;
		Actor * pMaster = nullptr;
		for (size_t i = 0; i < (*g_characterManager)->numCompanions; ++i)
		{
			auto * pCharacter = (*g_characterManager)->companions[i];
			auto * pActor = LookUpFormByID(pCharacter->formID);
			//_MESSAGE("CU=%016I64X | VTBL=%016I64X", pCustomer, *(uintptr_t*)pCustomer);
			if (pActor != nullptr && pActor != pMerchant)
			{
				float params[4]{};
				GetDiscount(pActor, pMerchant, params);
				float arg1 = roundf(params[2] * params[3] * 100.0f);
				float arg2 = roundf((params[2] - params[1] - params[0]) * params[3] * 100.0f);
				SInt32 arg3 = 100 - static_cast<SInt32>(roundf((arg2 / arg1) * 100.0f));
				if (arg1 < arg2) arg3 = -arg3;
				maxDiscount = (arg3 > maxDiscount) ? (pMaster = pActor, arg3) : maxDiscount;
				//_MESSAGE("%d | %d", i, arg3);
			}
		}
		if (pMaster != nullptr)
		{
			return CalcWareValue_Original(pObj, pMaster, pMerchant, isSell, count, unk);
		}
	}
	return CalcWareValue_Original(pObj, pCustomer, pMerchant, isSell, count, unk);
}

UInt32 CalcGoodValue_Hook(void * pObj, Character * pCustomer, Character * pMerchant, bool isSell, UInt32 count)
{
	//_MESSAGE("%016I64X | %016I64X", pCustomer, pMerchant);
	if (pCustomer && pMerchant && (*g_characterManager) && (*g_characterManager)->numCompanions)
	{
		if (isSell)
		{
			UInt32 maxGolds = 0;
			for (size_t i = 0; i < (*g_characterManager)->numCompanions; ++i)
			{
				auto * pCharacter = (*g_characterManager)->companions[i];
				if (pCharacter != nullptr && pCharacter != pMerchant)
				{
					UInt32 golds = CalcGoodValue_Original(pObj, pCharacter, pMerchant, isSell, count);
					//_MESSAGE("%016I64X | %d | %d", pCharacter, golds, isSell);
					maxGolds = (maxGolds < golds) ? golds : maxGolds;
				}
			}
			return maxGolds;
		}
		else
		{
			UInt32 minGolds = 0xFFFFFFFF;
			for (size_t i = 0; i < (*g_characterManager)->numCompanions; ++i)
			{
				auto * pCharacter = (*g_characterManager)->companions[i];
				if (pCharacter != nullptr && pCharacter != pMerchant)
				{
					UInt32 golds = CalcGoodValue_Original(pObj, pCharacter, pMerchant, isSell, count);
					//_MESSAGE("%016I64X | %d | %d", pCharacter, golds, isSell);
					minGolds = (minGolds > golds) ? golds : minGolds;
				}
			}
			return minGolds;
		}
	}
	return CalcGoodValue_Original(pObj, pCustomer, pMerchant, isSell, count);
}

void GetDiscount_Hook(Actor * customer, Actor * merchant, float result[4])
{
	//_MESSAGE("%s | %016I64X | %016I64X | %016I64X | %016I64X",__FUNCTION__, customer, *(uintptr_t*)customer, merchant, *(uintptr_t*)merchant);
	if ((*g_characterManager) != nullptr)
	{
		Actor * pMaster = nullptr;
		SInt32 maxDiscount = -100;
		for (size_t i = 0; i < (*g_characterManager)->numCompanions; ++i)
		{
			auto * pCharacter = (*g_characterManager)->companions[i];
			auto * pActor= LookUpFormByID(pCharacter->formID);
			//_MESSAGE("CU=%016I64X | VTBL=%016I64X", pCustomer, *(uintptr_t*)pCustomer);
			if (pActor != nullptr && pActor != merchant)
			{
				float params[4]{};
				GetDiscount(pActor, merchant, params);
				float arg1 = roundf(params[2] * params[3] * 100.0f);
				float arg2 = roundf((params[2] - params[1] - params[0]) * params[3] * 100.0f);
				SInt32 arg3 = 100 - static_cast<SInt32>(roundf((arg2 / arg1) * 100.0f));
				if (arg1 < arg2) arg3 = -arg3;
				maxDiscount = (arg3 > maxDiscount) ? (pMaster = pActor, arg3) : maxDiscount;
			}
		}
		if (pMaster != nullptr)
		{
			return GetDiscount(pMaster, merchant, result);
		}
	}
	return GetDiscount(customer, merchant, result);
}

//using _RegisterScriptFunction = UInt32(*)(void * pObj, ScriptFunctor * pFunctor, void * arg2, void * arg3);
//RelocAddr<_RegisterScriptFunction>	RegisterScriptFunction(0x1854CB0);
//_RegisterScriptFunction				RegisterScriptFunction_Original = nullptr;
//bool RegisterScriptFunction_Hook(void * pObj, ScriptFunctor * pFunctor, void * arg2, void * arg3)
//{
//	static FILE * pFile = nullptr;// = fopen_s("ScriptDump.txt", "w");
//	if (!pFile)
//	{
//		errno_t err;
//		err = fopen_s(&pFile, "ScriptDump.txt", "w+");
//	}
//	if (pFile != nullptr)
//	{
//		//_MESSAGE("LLLLLLLL");
//		pFunctor->WriteDump(pFile);	
//		fwrite("\n", sizeof(char), 1, pFile);
//	}
//	_MESSAGE("(%08X) ==> %s", static_cast<uint32_t>((uintptr_t)(pFunctor->functor) - RelocationManager::s_baseAddr), pFunctor->fnName);
//	if (!strcmp("StartDefaultDialog", pFunctor->fnName) && pFile != nullptr)
//	{
//		fclose(pFile);
//	}
//	return RegisterScriptFunction_Original(pObj, pFunctor, arg2, arg3);
//}

void Hooks_Commit(void)
{

	//135A801
	//g_branchTrampoline.Write5Call(RelocAddr<uintptr_t>(0x135A801), (uintptr_t)CalcGoodValue_Hook);
	//g_branchTrampoline.Write5Call(RelocAddr<uintptr_t>(0x135A89E), (uintptr_t)CalcGoodValue_Hook);
	//ScriptFunctor::InitHook();

	{
		struct CalcWareValue_Code : Xbyak::CodeGenerator {
			CalcWareValue_Code(void * buf) : Xbyak::CodeGenerator(4096, buf)
			{
				Xbyak::Label retnLabel;
				//mov     [rsp+20h], r9b
				mov(ptr[rsp + 0x20], r9b);

				jmp(ptr[rip + retnLabel]);

				L(retnLabel);
				dq(CalcWareValue.GetUIntPtr() + 5);
			}
		};

		void * codeBuf = g_localTrampoline.StartAlloc();
		CalcWareValue_Code code(codeBuf);
		g_localTrampoline.EndAlloc(code.getCurr());

		CalcWareValue_Original = (_CalcWareValue)codeBuf;

		g_branchTrampoline.Write5Branch(CalcWareValue.GetUIntPtr(), (uintptr_t)CalcWareValue_Hook);
	}

	{
		struct CalcGoodValue_Code : Xbyak::CodeGenerator {
			CalcGoodValue_Code(void * buf) : Xbyak::CodeGenerator(4096, buf)
			{
				Xbyak::Label retnLabel;
				//mov     [rsp+20h], r9b
				mov(ptr[rsp + 0x20], r9b);

				jmp(ptr[rip + retnLabel]);

				L(retnLabel);
				dq(CalcGoodValue.GetUIntPtr() + 5);
			}
		};

		void * codeBuf = g_localTrampoline.StartAlloc();
		CalcGoodValue_Code code(codeBuf);
		g_localTrampoline.EndAlloc(code.getCurr());

		CalcGoodValue_Original = (_CalcGoodValue)codeBuf;

		g_branchTrampoline.Write5Branch(CalcGoodValue.GetUIntPtr(), (uintptr_t)CalcGoodValue_Hook);
	}

	//{
	//	struct RegisterScriptFunction_Code : Xbyak::CodeGenerator {
	//		RegisterScriptFunction_Code(void * buf) : Xbyak::CodeGenerator(4096, buf)
	//		{
	//			Xbyak::Label retnLabel;
	//			//mov     [rsp+20h], r9b
	//			mov(ptr[rsp + 0x8], rbx);

	//			jmp(ptr[rip + retnLabel]);

	//			L(retnLabel);
	//			dq(RegisterScriptFunction.GetUIntPtr() + 5);
	//		}
	//	};

	//	void * codeBuf = g_localTrampoline.StartAlloc();
	//	RegisterScriptFunction_Code code(codeBuf);
	//	g_localTrampoline.EndAlloc(code.getCurr());

	//	RegisterScriptFunction_Original = (_RegisterScriptFunction)codeBuf;

	//	g_branchTrampoline.Write5Branch(RegisterScriptFunction.GetUIntPtr(), (uintptr_t)RegisterScriptFunction_Hook);
	//}
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\Larian Studios\\Divinity Original Sin 2\\NativeModLogs\\SharedDiscount.log");
		if (!g_branchTrampoline.Create(1024 * 64))
		{
			_MESSAGE("couldn't create branch trampoline. this is fatal. skipping remainder of init process.");
			return FALSE;
		}
		if (!g_localTrampoline.Create(1024 * 64, hModule))
		{
			_MESSAGE("couldn't create codegen buffer. this is fatal. skipping remainder of init process.");
			return FALSE;
		}
		try
		{
			auto ReadOffsetData = [](uintptr_t location, SInt32 relOffset, UInt32 len)->uintptr_t
			{
				SInt32 rel32 = 0;
				RVAUtils::ReadMemory(location + relOffset, &rel32, sizeof(SInt32));
				_MESSAGE("(%016I64X) patched sucessfully...", location + len + rel32 - RelocationManager::s_baseAddr);
				return location + len + rel32;
			};

			uintptr_t location = RVAScan<uintptr_t>("8B D9 C1 EB 10 83 EA 3D 0F 84 0F 06 00 00", -0x36).GetUIntPtr();
			g_characterManager = ReadOffsetData(location + 0x495, 3, 7) - RelocationManager::s_baseAddr;


			try //before v3.0.158.595
			{
				location = RVAScan<uintptr_t>("48 6B 0D ? ? ? ? ? 4C 8B 35 ? ? ? ? 48 8B FA").GetUIntPtr(); //48 6B 0D ? ? ? ? ? 48 8B 35 ? ? ? ?
			}
			catch (const no_result_exception & exceptioin)
			{
				location = RVAScan<uintptr_t>("48 6B 0D ? ? ? ? ? 48 8B 35 ? ? ? ?").GetUIntPtr();
			}

			_MESSAGE("(%016I64X) patched sucessfully...", location - RelocationManager::s_baseAddr);

			pGameDataOffset = reinterpret_cast<uintptr_t*>(ReadOffsetData(location, 3, 8));
			pGameDataHandler = reinterpret_cast<uintptr_t*>(ReadOffsetData(location + 8, 3, 7));

			GetDiscount = RVAScan<uintptr_t>("E8 ? ? ? ? F3 0F 10 44 24 ? 48 8D 4D F0", 0, 1, 5).GetUIntPtr() - RelocationManager::s_baseAddr;
			_MESSAGE("(%016I64X) patched sucessfully...", GetDiscount.GetUIntPtr() - RelocationManager::s_baseAddr);

			CalcWareValue = RVAScan<uintptr_t>("E8 ? ? ? ? 44 03 E0 48 8B 4C 24 ?", 0, 1, 5).GetUIntPtr() - RelocationManager::s_baseAddr;
			_MESSAGE("(%016I64X) patched sucessfully...", CalcWareValue.GetUIntPtr() - RelocationManager::s_baseAddr);

			CalcGoodValue = RVAScan<uintptr_t>("E8 ? ? ? ? 48 8B CB 44 8B E8 E8 ? ? ? ? 49 8B CC", 0, 1, 5).GetUIntPtr() - RelocationManager::s_baseAddr;
			_MESSAGE("(%016I64X) patched sucessfully...", CalcGoodValue.GetUIntPtr() - RelocationManager::s_baseAddr);


			location = RVAScan<uintptr_t>("E8 ? ? ? ? F3 0F 10 44 24 ? 48 8D 4D F0").GetUIntPtr();
			_MESSAGE("(%016I64X) patched sucessfully...", location - RelocationManager::s_baseAddr);
			//g_branchTrampoline.Write5Call(RelocAddr<uintptr_t>(0xFA21BA), (uintptr_t)GetDiscount_Hook); //FA2859
			g_branchTrampoline.Write5Call(location, (uintptr_t)GetDiscount_Hook); //FA2859

			location = RVAScan<uintptr_t>("E8 ? ? ? ? F3 0F 10 44 24 ? 48 8D 8D ? ? ? ?").GetUIntPtr();
			_MESSAGE("(%016I64X) patched sucessfully...", location - RelocationManager::s_baseAddr);
			//g_branchTrampoline.Write5Call(RelocAddr<uintptr_t>(0xFA2859), (uintptr_t)GetDiscount_Hook);
			g_branchTrampoline.Write5Call(location, (uintptr_t)GetDiscount_Hook);

			Hooks_Commit();

		}
		catch (const no_result_exception & exceptioin)
		{
			_MESSAGE(exceptioin.what());
			return FALSE;
		}
		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

//GetDiscount_1092210(v5, v4, &v63);

/*
.text:000000000159FB1F                 lea     r8, aCancombineitem ; "CanCombineItem"
.text:000000000159FB26                 mov     rdx, rbx
.text:000000000159FB29                 call    sub_1854BA0
.text:000000000159FB2E                 mov     r9d, 1
.text:000000000159FB34                 lea     rcx, [rbp+3FD0h+var_260]
.text:000000000159FB3B                 mov     rdx, r12
.text:000000000159FB3E                 mov     r10, rax
.text:000000000159FB41                 lea     r8d, [r9+5]
.text:000000000159FB45                 call    sub_595AA0
.text:000000000159FB4A                 lea     r8d, [r9+6]
.text:000000000159FB4E                 lea     rdx, aItema     ; "ItemA"
.text:000000000159FB55                 lea     rcx, [rbp+3FD0h+var_250]
.text:000000000159FB5C                 call    sub_595AA0
.text:000000000159FB61                 lea     rdx, aItemb     ; "ItemB"
.text:000000000159FB68                 lea     rcx, [rbp+3FD0h+var_240]
.text:000000000159FB6F                 call    sub_595AA0
.text:000000000159FB74                 lea     rdx, aItemc     ; "ItemC"
.text:000000000159FB7B                 lea     rcx, [rbp+3FD0h+var_230]
*/

/*

GetCharacterUnk10ByUUID_1457650(&v10, a2);



(015A9EE0) ==> CompleteTutorial
(015A9D10) ==> UnlockAchievement
(015A9E20) ==> ProgressAchievement
(015A9D60) ==> SetAchievementProgress
(015AA920) ==> SetHomesteadKeyState
(015AA900) ==> EnableSendToHomestead
(015A9970) ==> KillCombatFor
(015A98F0) ==> HasKickstarterDialogReward
(015A98C0) ==> IsHardcoreMode
(015A9810) ==> CharacterHasDLC
(015A97F0) ==> GetUserCount
(015AA270) ==> GenerateTreasure
(015A94D0) ==> PlayAnimation
(015A90A0) ==> GetVarObject
(015A8D60) ==> SetVarObject
(015A8B70) ==> ClearVarObject
(015A7910) ==> SetScriptframe
(015A7860) ==> ClearScriptframe
(015A8AB0) ==> SetTag
(015A89F0) ==> ClearTag
(015A88E0) ==> GetVarInteger
(015A87D0) ==> GetVarFloat
(015A84B0) ==> GetVarString
(015A81B0) ==> GetVarFixedString
(015A80D0) ==> SetVarInteger
(015A7FF0) ==> SetVarFloat
(015A7F10) ==> SetVarString
(015A7DF0) ==> SetVarFixedString
(015A7CF0) ==> SetVarFloat3
(015A7A20) ==> GetVarFloat3
(015A77A0) ==> IsTagged
(015A7680) ==> RemoveStatus
(015A75C0) ==> RemoveHarmfulStatuses
(015A74D0) ==> HasActiveStatus
(015A73D0) ==> HasAppliedStatus
(015A7380) ==> GetUUID
(015A7290) ==> GetPosition
(015A6BD0) ==> GetRotation
(015A6A60) ==> GetClosestPlayer
(015A64A0) ==> GetClosestPlayerWithTalent
(015A68C0) ==> GetClosestAlivePlayer
(015A67F0) ==> GetClosestAliveUserPlayer
(015A66A0) ==> GetClosestPlayerToPosition
(015A5EE0) ==> Transform
(015A5DB0) ==> ApplyStatus
(015A5CC0) ==> GetRegion
(015B2020) ==> SetStoryEvent
(015B1FA0) ==> RequestProcessed
(015A5A90) ==> IterateParty
(015A59E0) ==> IterateParties
(015A5990) ==> IterateUsers
(015A5820) ==> GetUserName
(015A5750) ==> GetUserProfileID
(015A5B90) ==> LoadGame
(015A5720) ==> LeaveParty
(015A56E0) ==> AddToParty
(015A5600) ==> LoadPartyPreset
(015A4490) ==> CrimeGetType
(015A4430) ==> CrimeGetDetectionRange
(015A29F0) ==> CrimeGetTension
(015A29B0) ==> CrimeGetNewID
(015A43E0) ==> CrimeIsTensionOverWarningTreshold
(015A48E0) ==> CrimeClearAll
(015A46F0) ==> CrimeFindEvidence
(015A4530) ==> CrimeInterrogationDone
(015A44F0) ==> CrimeConfrontationDone
(015A3AF0) ==> CrimeGetLeadInvestigator
(015A5440) ==> SetCanFight
(015A51F0) ==> SetCanJoinCombat
(015A4900) ==> InventoryLaunchIterator
(015A4ED0) ==> InventoryLaunchTagIterator
(015A4B00) ==> InventoryLaunchTemplateIterator
(015A31D0) ==> SetIsBoss
(015A37E0) ==> IsBoss
(015AE6C0) ==> GetDistanceTo
(015AE5E0) ==> GetDistanceToPosition
(015AE590) ==> GetAngleTo
(015A3410) ==> SetCombatGroupID
(015A53B0) ==> EndTurn
(015A2C50) ==> CrimeAreaSetTensionModifier
(015A2BF0) ==> CrimeAreaGetTensionModifier
(015A2BA0) ==> CrimeAreaResetTensionModifier
(015A4330) ==> CrimeGetVictim
(015A4240) ==> CrimeTransferEvidenceTo
(015A40F0) ==> CrimeGetEvidence
(015A40A0) ==> CrimeGetNumberOfEvidence
(015A4040) ==> CrimeIsContinuous
(015A4020) ==> ShutdownCrimeSystem
(015A3EF0) ==> CrimeGetCriminals
(015A3D60) ==> CrimeIsAnyNPCGoingToReact
(015A3CE0) ==> CrimeTransferLeadershipTo
(015A3CA0) ==> CrimeEnableInterrogation
(015A3BA0) ==> CrimeIgnoreAllCrimesForCriminal
(015A38C0) ==> CrimeResetInterrogationForCriminals
(015A3750) ==> JumpToTurn
(015A2B00) ==> CrimeIgnoreCrime
(015A2A40) ==> CrimeAddCriminal
(015A3660) ==> NotifyCharacterCreationFinished
(015B2380) ==> SetHasDialog
(015AA190) ==> ShowArenaResult
(015A30F0) ==> EnterCombat
(015A2F60) ==> LeaveCombat
(015A2880) ==> SetFaction
(015A27F0) ==> GetFaction
(015B1D80) ==> GetTemplate
(015A2750) ==> SetInvulnerable_UseProcSetInvulnerable
(015A2550) ==> UserRest
(015A2720) ==> FireOsirisEvents
(015A2440) ==> IsSkillActive
(015A21E0) ==> StartCameraSpline
(015A1FB0) ==> StopCameraSpline
(015A1E10) ==> ApplyDamage
(015B1CF0) ==> IsSourceSkill
(015A1CA0) ==> MakePeace
(015A1B30) ==> MakeWar
(015A16B0) ==> ActivatePersistentLevelTemplateWithCombat
(015A18F0) ==> ActivatePersistentLevelTemplate
(015A15D0) ==> GetStatusType
(015A1410) ==> GetHealStat
(015A1240) ==> ReadyCheckStart
(015A1190) ==> IsGameLevel
(015A10F0) ==> IsCharacterCreationLevel
(015AA9A0) ==> HasRecipeUnlockedWithIngredient

*/
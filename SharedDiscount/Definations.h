#pragma once
#include "f4se_common\Relocation.h"

#define FORCE_INLINE  __forceinline
#define DEFINE_MEMBER_FUNCTION(fnName, retnType, addr, ...)						\
	template <class... Params>													\
	FORCE_INLINE retnType fnName(Params&&... params) {							\
		struct empty_struct {};													\
		typedef retnType(empty_struct::*_##fnName##_type)(__VA_ARGS__);			\
		const static uintptr_t address = addr + RelocationManager::s_baseAddr;	\
		_##fnName##_type fn = *(_##fnName##_type*)&address;						\
		return (reinterpret_cast<empty_struct*>(this)->*fn)(params...);			\
	}

struct ActorValueOwner
{
	// 
	struct BaseAttributes
	{
		UInt64			unk00[0x10 >> 3];
		UInt32			unk10;
		UInt32			baseStrength;				// 014 028
		UInt32			baseMemory;					// 018
		UInt32			baseIntelligence;			// 01C
		UInt64			unk20;
		UInt32			baseFinesse;				// 028
		UInt32			baseWits;
		UInt32			baseConstitution;			// 030
	};

	void				** vtbl; //01BE5060
	UInt32				unk08;
	UInt32				level;							// 00C
	UInt64				unk10[(0x30 - 0x10) >> 3];		// 010
	const char			* name;							// 030
	UInt64				unk00[(0x1D8  - 0x038) >> 3];
	UInt32				health;							// 1D8
	UInt32				physicalArmor;					// 1DC
	UInt32				magicalArmor;					// 1E0
	UInt64				unk1E0[(0x1F0 - 0x1E8) >> 3];	// 1E8
	UInt32				actionPoints;					// 1F0
	UInt32				unk1F4;
	UInt32				exp;							// 1F8
	UInt64				unk200[(0x248 - 0x200) >> 3];
	BaseAttributes		** baseAttributes;				// 248
};

//0x590
//.text:00000000015728A8                 call    sub_1854CB0
class Character
{
public:
	virtual ~Character();
	virtual void		Unk_01(void);
	virtual void		Unk_02(void); //return 5;
	virtual void		SetHandle(UInt64 handle);
	virtual UInt64 *	GetHandle(UInt64 * handle);
	virtual void		Unk_05();
	virtual void		Unk_06();
	virtual void		SetFormID(UInt32 formID);
	virtual UInt32 *	GetFormID(UInt32 * pFormID);
	virtual void		Unk_09();
	virtual void		Unk_0A();
	virtual void		Unk_0B();
	virtual void		Unk_0C();
	virtual void		Unk_0D();
	virtual UInt64 *	GetUnk10(UInt64 *); //get obj vtbl 01BD7530 //Endturn

	enum
	{
		kFlag_Selected = 1 << 13
	};

	enum
	{
		kFlag_CanControl = 1
	};

	//void				** vtbl; //01B7EF68
	UInt64				unk08;
	UInt64				unk10; //?handle?
	UInt64				unk18;
	UInt64				handle; // uesd in persuation function. handle to character==>sub_13437A0(g_characterHandler_1FBF7F0 + 0x100, handle))
	UInt64				unk28;
	UInt32				formID;
	UInt64				unk38[(0x48 - 0x38) >> 3];
	UInt64				flags; //2017 //0010 0000 0001 0111
	UInt64				unk50[(0x120 - 0x50) >> 3];
	ActorValueOwner		* actorValueOwner;
	UInt64				unk128[(0x1E8 - 0x128) >> 3];
	UInt32				groupID;
	UInt64				unk1F0;
	UInt64				controlFlags;
};
STATIC_ASSERT(sizeof(Character) == 0x200);
STATIC_ASSERT(offsetof(Character, formID) == 0x30);

class Actor
{
public:

};


class ChainGroup
{
public:
	struct Chain
	{
		struct Group
		{
			UInt64		unk00;
			Character	** characters;
			UInt32		unk10;
			UInt8		numCharacters;
		};
		UInt64			unk00[0x68 >> 3];	// 000
		Group			** groups;			// 068
		UInt32			unk70;				// 070
		UInt8			numGroups;			// 074

		DEFINE_MEMBER_FUNCTION(CalcChainIndex, SInt32, 0x12F8800, SInt32 chainIndex);
		DEFINE_MEMBER_FUNCTION(LinkCharacter, bool, 0x12F8120, Character * pCharacter, SInt32 chainIndex, SInt32 insertIndex);
	};

	ChainGroup			* pNext;
	UInt32				groupID;
	Chain				* pChain;
};
STATIC_ASSERT(offsetof(ChainGroup::Chain, groups) == 0x68);
STATIC_ASSERT(offsetof(ChainGroup::Chain, numGroups) == 0x74);
STATIC_ASSERT(offsetof(ChainGroup::Chain::Group, characters) == 0x8);
STATIC_ASSERT(offsetof(ChainGroup::Chain::Group, numCharacters) == 0x14);

class DataInfo
{
public:

	void				** vtbl; //01BAA0F8
	UInt16				unk08;
	UInt16				unk0A;
	UInt32				unk0C;
};


struct Message
{
#pragma pack(push, 4)
	struct Data
	{
		UInt32			isChain; //1 unchain 2 chain.
#ifdef _DEBUG
		UInt16			unk04;
		UInt16			unk06;
		UInt16			unk08;
		UInt16			unk0A;
#else
		UInt32			groupID;
		UInt32			targetID;
#endif
		UInt32			targetChainIndex; //chainIndex
		UInt32			insertIndex; //unchainIndex   0 top -1 bottom
	};
#pragma pack(pop)
	UInt64				unk00;
	UInt32				type;
	UInt64				unk10[(0x30 - 0x10) >> 3];
	Data				* unk30;
	UInt32				unk38;
	UInt32				unk3C;
};
STATIC_ASSERT(sizeof(Message) == 0x40);
STATIC_ASSERT(sizeof(Message::Data) == 0x14);



class CharacterManager
{
public:
	UInt64				unk00[0xA0 >> 3];
	UInt32				numGroupChains; // array length;
	ChainGroup			** groupChains;
	UInt64				unkB0[(0x190 - 0xB0) >> 3];
	Character			** companions;
	UInt32				unk198;
	UInt32				numCompanions;

};
extern RelocPtr<CharacterManager *>	g_characterManager;
STATIC_ASSERT(offsetof(CharacterManager, numCompanions) == 0x19C);//companions
STATIC_ASSERT(offsetof(CharacterManager, companions) == 0x190);
STATIC_ASSERT(sizeof(CharacterManager) == 0x1A0);


class DialogueManager
{
public:
	void				** vtbl;//1BC3650
};

//#include<stdio.h>
//0x50
class ScriptFunctor
{
public:
	//void				** vtbl;	//0x1BB8A10
	virtual ~ScriptFunctor();
	virtual void		Unk_01(void);
	virtual void		Unk_02(void); //return 5;
	virtual void		WriteDump(FILE * pFile);

	const char			* fnName;
	UInt64				unk10;		//ini'd to 0
	UInt32				unk18;		//ini'd to 0
	UInt64				unk20;		//ini'd to 0
	UInt32				unk28;		//ini'd to 0
	void				* unk30;	//ini'd to arg1
	UInt32				unk38;		//ini'd to 1
	void				* unk40;	//ini'd to arg2
	void				* functor;	//ini'd to functor

	//static ScriptFunctor * Init(ScriptFunctor * pObj, void * arg0, void * arg1, const char * name, void * arg2, void * fn)
	//{
	//	pObj->vtbl = RelocAddr<void**>(0x1BB8A10);
	//	pObj->fnName = name;
	//	pObj->unk10 = pObj->unk20 = 0;
	//	pObj->unk18 = pObj->unk28 = 0;
	//	pObj->unk30 = arg1;
	//	pObj->unk38 = 1;
	//	pObj->unk40 = arg2;
	//	pObj->functor = fn;
	//	_MESSAGE("(%08X) ==> %s", static_cast<uint32_t>((uintptr_t)(pObj->functor) - RelocationManager::s_baseAddr), pObj->fnName);
	//	return pObj;
	//}

	//static void InitHook()
	//{
	//	g_branchTrampoline.Write6Branch(RelocAddr<uintptr_t>(0x672CC0), (uintptr_t)Init);
	//}

	DEFINE_MEMBER_FUNCTION(CreateFunctor, ScriptFunctor *, 0x672CC0, void * arg0, void * arg1, const char * fnName, void * arg2, void * functor);
};
STATIC_ASSERT(sizeof(ScriptFunctor) == 0x50);

/*
(015636B0) ==> CharacterHasTalent
(015635E0) ==> CharacterAddTalent
(01563570) ==> CharacterRemoveTalent
(01563530) ==> CharacterGetLevel
(015634C0) ==> CharacterCanFight
(01563460) ==> CharacterFreeze
(01563410) ==> CharacterUnfreeze
(0156C2D0) ==> CharacterCreateAtTrigger
(0156C2C0) ==> TemporaryCharacterCreateAtTrigger
(0156BFE0) ==> CharacterCreateAtPosition
(0156BFD0) ==> TemporaryCharacterCreateAtPosition
(0156B4D0) ==> CharacterCreateAtPositionOutOfSightTo
(0156B4C0) ==> TemporaryCharacterCreateAtPositionOutOfSightTo
(0156B9F0) ==> CharacterCreateOutOfSightToObject
(0156B9E0) ==> TemporaryCharacterCreateOutOfSightToObject
(01563280) ==> OpenMessageBox
(01563230) ==> ShowCredits
(0156ADA0) ==> TeleportToPosition
(0156A8B0) ==> TeleportTo
(0156A750) ==> CharacterMoveToPosition
(0156A320) ==> CharacterMoveTo
(01569CE0) ==> CharacterLookFromTrigger
(01569FD0) ==> CharacterEquipItem
(0156A260) ==> TransferItemsToCharacter
(0156A1C0) ==> TransferItemsToParty
(0156A070) ==> TransferItemsToUser
(01569F30) ==> CharacterUnequipItem
(01569BE0) ==> CharacterFollowCharacter
(015697B0) ==> CharacterStopFollow
(01569280) ==> CharacterTeleportPartiesToTriggerWithMovie
(01569220) ==> CharacterTeleportPartiesToTriggerWithMovieRequestCallback
(01569130) ==> CharacterSetTeleportMovie
(015690C0) ==> CharacterTeleportPartiesToTrigger
(015693C0) ==> CharacterClearTradeGeneratedItems
(01569670) ==> CharacterSetCustomTradeTreasure
(01569360) ==> GenerateItems
(01569540) ==> CharacterGiveReward
(01569430) ==> CharacterGiveQuestReward
(01567FF0) ==> CharacterDie
(01567F00) ==> CharacterDieImmediate
(01568D00) ==> CharacterAddSkill
(01568C50) ==> CharacterRemoveSkill
(01568B60) ==> CharacterHasSkill
(015688A0) ==> CharacterAddAttributePoint
(01568970) ==> CharacterGetAttributePoints
(015687D0) ==> CharacterAddAbilityPoint
(01568700) ==> CharacterAddCivilAbilityPoint
(01568660) ==> CharacterAddActionPoints
(015685D0) ==> CharacterGetAbilityPoints
(01568540) ==> CharacterGetCivilAbilityPoints
(01568A90) ==> CharacterAddTalentPoint
(01568A00) ==> CharacterGetTalentPoints
(015684D0) ==> CharacterGetBaseSourcePoints
(01568460) ==> CharacterGetSourcePoints
(015683F0) ==> CharacterGetMaxSourcePoints
(01567E40) ==> CharacterResurrect
(01567D80) ==> CharacterResurrectAndResetXPReward
(01567C10) ==> CharacterResurrectCustom
(01567B30) ==> CharacterGetReservedUserID
(01567A40) ==> GetCurrentCharacter
(015679B0) ==> CharacterIsControlled
(01567B70) ==> CharacterGetGold
(01567930) ==> CharacterAddGold
(01567890) ==> PartyAddGold
(01567810) ==> PartyGetGold
(01567760) ==> UserAddGold
(015676F0) ==> UserGetGold
(01567430) ==> CharacterIncreaseSocialStat
(01567170) ==> CharacterDecreaseSocialStat
(01567110) ==> CharacterSetSpectating
(015670D0) ==> CharacterIsSpectating
(01566FF0) ==> CharacterCanSee
(01566F60) ==> CharacterSetCustomName
(01566F40) ==> CharacterAppear
(01566F20) ==> CharacterAppearCustom
(01566F00) ==> CharacterAppearAt
(01566EE0) ==> CharacterAppearAtCustom
(01566EC0) ==> CharacterAppearAtPosition
(01566EA0) ==> CharacterAppearAtPositionCustom
(01566E80) ==> CharacterAppearOutOfSightTo
(01566E60) ==> CharacterAppearOutOfSightToCustom
(01566E40) ==> CharacterAppearOutOfSightToObject
(01566E20) ==> CharacterAppearOutOfSightToObjectCustom
(01566E00) ==> CharacterAppearOnTrailOutOfSightTo
(01566DE0) ==> CharacterAppearOnTrailOutOfSightToCustom
(01566DC0) ==> CharacterAppearOnTrailOutOfSightToObject
(01566DA0) ==> CharacterAppearOnTrailOutOfSightToObjectCustom
(01566D80) ==> CharacterAppearAtPositionOutOfSightTo
(01566D60) ==> CharacterAppearAtPositionOutOfSightToCustom
(01566D40) ==> CharacterAppearAtPositionOutOfSightToObject
(01566D20) ==> CharacterAppearAtPositionOutOfSightToObjectCustom
(01566AE0) ==> CharacterDisappearOutOfSight
(015668F0) ==> CharacterDisappearOutOfSightToObject
(01566C20) ==> CharacterFleeOutOfSight
(01566770) ==> CharacterAttack
(015664F0) ==> CharacterAddToCharacterCreation
(015663C0) ==> GameMasterAddToCharacterCreation
(015666B0) ==> CharacterMakePlayer
(01566640) ==> CharacterRecruitCharacter
(01566600) ==> CharacterAssign
(015665A0) ==> CharacterAssignToUser
(01566320) ==> CharacterMakeCompanion
(015662A0) ==> CharacterMakeNPC
(01566200) ==> CharacterAddToParty
(01566180) ==> CharacterRemoveFromParty
(01565FB0) ==> CharacterAddToPlayerCharacter
(01565F10) ==> CharacterRemoveFromPlayerCharacter
(01566130) ==> CharacterIsPartyMember
(01566050) ==> CharacterIsInPartyWith
(01565800) ==> CharacterGetRelationToCharacter
(01565E60) ==> CharacterSetRelationIndivFactionToIndivFaction
(01565CF0) ==> CharacterSetRelationIndivFactionToFaction
(01565B90) ==> CharacterSetRelationFactionToIndivFaction
(01565A20) ==> CharacterSetRelationFactionToFaction
(01565990) ==> CharacterSetTemporaryHostileRelation
(015658A0) ==> CharacterSetReactionPriority
(015657A0) ==> CharacterGetAttitudeTowardsPlayer
(01565730) ==> CharacterGetHitpointsPercentage
(015656E0) ==> CharacterSetHitpointsPercentage
(01565670) ==> CharacterGetArmorPercentage
(01565540) ==> CharacterSetArmorPercentage
(015654D0) ==> CharacterGetMagicArmorPercentage
(015653A0) ==> CharacterSetMagicArmorPercentage
(015643F0) ==> CharacterLookAt
(01565120) ==> CharacterLevelUp
(01565060) ==> CharacterLevelUpTo
(01564EA0) ==> PartyAddActualExperience
(01564E10) ==> PartyAddExperience
(01564D80) ==> PartyAddExplorationExperience
(01564D20) ==> CharacterAddExplorationExperience
(01564C90) ==> PartyAddCharismaExperience
(01564AA0) ==> CharacterStatusText
(01564900) ==> CharacterEnteredSubRegion
(01564540) ==> CharacterDisplayTextWithParam
(01564C40) ==> CharacterSetImmortal
(01564F20) ==> CharacterGetHostCharacter
(01564710) ==> CharacterGetDisplayName
(015643B0) ==> CharacterFlushQueue
(01564370) ==> CharacterPurgeQueue
(01564030) ==> CharacterLaunchIterator
(01564150) ==> CharacterLaunchIteratorAroundCharacter
(01563FE0) ==> CharacterSetCanTrade
(01563F70) ==> CharacterCanTrade
(01563C60) ==> CharacterSetStill
(01563BC0) ==> CharacterIsInCombat
(01563B50) ==> CharacterIsMoving
(01563B00) ==> CharacterIsInFightMode
(01563970) ==> CharacterSetFightMode
(01563930) ==> CharacterMakeStoryNpc
(015637A0) ==> CharacterStopAllEffectsWithName
(01563120) ==> CharacterPickupItem
(01563060) ==> CharacterItemSetEvent
(01562FA0) ==> CharacterCharacterSetEvent
(01568FD0) ==> CharacterUseItem
(01568E90) ==> CharacterMoveItemToTrigger
(015681A0) ==> CharacterConsume
(015680D0) ==> CharacterUnconsume
(01562770) ==> CharacterAddAttribute
(01562720) ==> CharacterRemoveAttribute
(01562BB0) ==> CharacterGetBaseAttribute
(015627C0) ==> CharacterGetAttribute
(015625F0) ==> CharacterAddAbility
(015625A0) ==> CharacterRemoveAbility
(01562540) ==> CharacterIsIncapacitated
(015626B0) ==> CharacterGetAbility
(01562640) ==> CharacterGetBaseAbility
(015624D0) ==> CharacterGetOwner
(01562180) ==> ActivateTrade
(01561C90) ==> StartPickpocket
(01562100) ==> ExecuteDeal
(01565330) ==> CharacterGetEquippedWeapon
(015652C0) ==> CharacterGetEquippedShield
(01565180) ==> CharacterGetEquippedItem
(01569840) ==> CharacterSetFollowCharacter
(01569B00) ==> CharacterAttachToGroup
(015699E0) ==> CharacterDetachFromGroup
(01569960) ==> CharactersAreGrouped
(01562080) ==> CharacterGetInventoryGoldValue
(01561E00) ==> CharacterGetItemTemplateCount
(01561D90) ==> CharacterAddAttitudeTowardsPlayer
(01561D50) ==> CharacterIsFemale
(01561BE0) ==> CharacterSetCanSpotSneakers
(01561C40) ==> CharacterCanSpotSneakers
(015618E0) ==> CharacterMoveWeaponsToContainer
(01561880) ==> CharacterLockAbility
(01561820) ==> CharacterUnlockAbility
(01561700) ==> CharacterUnlockRecipe
(015616B0) ==> CharacterIsDead
(01561620) ==> CharacterIsDeadOrFeign
(01561530) ==> CharacterSetAnimationOverride
(01561450) ==> CharacterSetAnimationSetOverride
(01560DF0) ==> PartySetIdentifyPriceModifier
(01560CC0) ==> PartySetRepairPriceModifier
(01560AC0) ==> PartySetShopPriceModifier
(01560BF0) ==> SetTagPriceModifier
(01560A50) ==> CharacterResetCooldowns
(01561380) ==> CharacterShowStoryElementUI
(015612B0) ==> CharacterCloseStoryElementUI
(01560F80) ==> CharacterSendGlobalCombatCounter
(01560F20) ==> CharacterPlayHUDSound
(01560540) ==> CharacterRegisterCrime
(015607A0) ==> CharacterRegisterCrimeWithPosition
(015603D0) ==> CharacterStopCrime
(015604D0) ==> CharacterStopCrimeWithID
(01560360) ==> CharacterIgnoreCharacterActiveCrimes
(01560A10) ==> CharacterRemoveSummons
(0155FDD0) ==> CharacterLinkGhost
(0155FD20) ==> CharacterUnlinkGhost
(0155FB90) ==> DestroyGhost
(0155FA00) ==> CharacterUseSkill
(0155F840) ==> CharacterUseSkillAtPosition
(015600D0) ==> CharacterDisableCrime
(015601B0) ==> CharacterIsCrimeEnabled
(01560280) ==> CharacterGetCrimeRegion
(0155FFC0) ==> CharacterEnableCrime
(01560090) ==> CharacterDisableAllCrimes
(0155FF90) ==> CharacterEnableAllCrimes
(0155FF50) ==> CharacterEnableCrimeWarnings
(0155FED0) ==> CharacterGetCrimeDialog
(0155FE90) ==> CharacterRemoveTension
(0155F710) ==> CharacterIsEnemy
(0155F510) ==> CharacterIsAlly
(0155F4B0) ==> CharacterIsNeutral
(01568370) ==> CharacterAddSourcePoints
(01568300) ==> CharacterOverrideMaxSourcePoints
(015682C0) ==> CharacterRemoveMaxSourcePointsOverride
(0155F3C0) ==> CharacterApplyPreset
(0155F2C0) ==> CharacterApplyHenchmanPreset
(0155F1B0) ==> CharacterApplyRacePreset
(0155EF80) ==> CharacterMoveToAndTalk
(0155EE90) ==> CharacterMoveToAndTalkRequestDialogFailed
(0155EE30) ==> CharacterEnableWaypointUsage
(0155EDE0) ==> CharacterCanUseWaypoints
(0155EC80) ==> CharacterCanSeeGhost
(0155EC30) ==> CharacterIsSummon
(0155EBE0) ==> CharacterIsPartyFollower
(0155EAD0) ==> CharacterIsPolymorphedInto
(0155EA80) ==> CharacterIsPolymorphInteractionDisabled
(0155EA40) ==> CharacterGameMaster
(0155E9F0) ==> CharacterIsPlayer
(0155E970) ==> CharacterHasLinkedGhost
(0155E8D0) ==> CharacterReservePolymorphShape
(0155E880) ==> CharacterSetForceSynch
(0155E830) ==> CharacterSetForceUpdate
(0155E580) ==> CharacterTransform
(0155E4B0) ==> CharacterTransformAppearanceTo
(0155E3A0) ==> CharacterTransformAppearanceToWithEquipmentSet
(0155E000) ==> CharacterCanSpotCrimes
(0155E0C0) ==> CharacterReceivedTag
(0155DF20) ==> CharacterAddPreferredAiTargetTag
(0155DE40) ==> CharacterRemovePreferredAiTargetTag
(0155DDB0) ==> CharacterGetRace
(0155DD20) ==> CharacterGetOrigin
(0155C250) ==> CharacterGetInstrument
(0155DB20) ==> CharacterGetHenchmanPresetPrice
(0155D9B0) ==> CharacterOriginIntroStopped
(0155C1F0) ==> CharacterSetDoNotFaceFlag
(0155C1B0) ==> CharacterSetReadyCheckBlocked
(0155C150) ==> CharacterSetCorpseLootable
(01582770) ==> ItemDrop
(01582070) ==> ItemRemove
(0157AE10) ==> MoveAllItemsTo
(0157F520) ==> ContainerIdentifyAll
(01581FA0) ==> ItemIsInCharacterInventory
(01581E00) ==> ItemIsInPartyInventory
(01581C70) ==> ItemIsInUserInventory
(01581BD0) ==> ItemIsInInventory
(015818C0) ==> ItemTemplateIsInCharacterInventory
(01581700) ==> ItemTagIsInCharacterInventory
(01581210) ==> ItemTemplateTagIsInCharacterInventory
(015802D0) ==> ItemTemplateIsInPartyInventory
(0157F860) ==> ItemTemplateIsInUserInventory
(0157FE60) ==> ItemTemplateGetDisplayString
(0157F5A0) ==> ItemTemplateIsInContainer
(0157D320) ==> ItemLock
(0157D2E0) ==> ItemUnLock
(0157D250) ==> ItemIsOpened
(0157D200) ==> DoorIsOpening
(0157D170) ==> ItemIsClosed
(0157D120) ==> DoorIsClosing
(0157CEF0) ==> ItemIsLocked
(0157CE70) ==> ItemIsContainer
(0157CD10) ==> ItemHasOnUse
(0157CE20) ==> ItemLockUnEquip
(0157CDD0) ==> ItemIsPoisoned
(0157CD70) ==> ItemIsTorch
(0157CCD0) ==> ItemIsEquipable
(0157CAB0) ==> ItemOpen
(0157C890) ==> ItemClose
(0157CFA0) ==> ItemDestroy
(0157CF50) ==> ItemIsDestroyed
(0157C7D0) ==> ItemClearOwner
(0157C4A0) ==> ItemGetOwner
(0157C420) ==> ItemGetOriginalOwner
(0157C350) ==> GetInventoryOwner
(0157C780) ==> ItemSetCanInteract
(0157C6E0) ==> ItemSetCanPickUp
(0157C730) ==> ItemSetCanMove
(0157C520) ==> ItemSetOwner
(0157C580) ==> ItemSetOriginalOwner
(0157C690) ==> ItemSetOnlyOwnerCanUse
(0157C640) ==> ItemSetStoryItem
(0157C240) ==> ItemLaunchIterator
(0157C5F0) ==> ItemIsStoryItem
(0157BD80) ==> ItemCreateAtTrigger
(0157BD00) ==> CreateKickstarterMessageInABottleItemAtTrigger
(0157BB70) ==> ItemRotateY
(0157B740) ==> ItemRotateToAngleY
(0157B700) ==> ItemAddCharges
(0157B6B0) ==> ItemGetCharges
(0157B670) ==> ItemResetChargesToMax
(0157B620) ==> ItemGetMaxCharges
(0157B500) ==> ItemSetDurability
(0157B5E0) ==> ItemGetDurability
(0157B5A0) ==> ItemGetAmount
(0157B4C0) ==> ItemGetHealthPoints
(0157B430) ==> ItemCanSitOn
(0157B3E0) ==> ItemIsLadder
(0157B0E0) ==> ItemTemplateCanSitOn
(0157E6F0) ==> ContainerGetGoldValue
(0157E6B0) ==> ItemGetGoldValue
(0157E820) ==> ItemSetForceSynch
(0157C810) ==> ItemSetKnown
(0157EC50) ==> GetItemForItemTemplateInInventory
(0157E880) ==> GetItemForItemTemplateInPartyInventory
(01581080) ==> CharacterFindTaggedItem
(01580CE0) ==> PartyFindTaggedItem
(01580920) ==> UserFindTaggedItem
(0157F420) ==> CharacterRemoveTaggedLocalItems
(0157F2B0) ==> PartyRemoveTaggedLocalItems
(0157F140) ==> UserRemoveTaggedLocalItems
(0157EFD0) ==> UserTransferTaggedLocalItems
(0157EF80) ==> ItemIsPublicDomain
(015B24E0) ==> ObjectIsCharacter
(015B23D0) ==> GetStatString
(015B25C0) ==> ObjectExists
(015B2580) ==> ObjectIsGlobal
(015B2530) ==> ObjectIsItem
(015B2230) ==> ObjectIsInTrigger
(015B2170) ==> PositionIsInTrigger
(015B20E0) ==> ObjectIsOnStage
(015B33A0) ==> PlayEffect
(015B3120) ==> PlayLoopEffect
(015B2E30) ==> PlayBeamEffect
(015B2B00) ==> PlayLoopBeamEffect
(015B2A30) ==> PlaySound
(015B2A20) ==> DebugText
(015B2860) ==> DisplayText
(015B27A0) ==> SetOnStage
(015B2090) ==> SetVisible
(015AA720) ==> ShowMapMarker
(015AA980) ==> SetCameraDistanceOverride
(015AA960) ==> GetMaxCameraDistance
(015B1F00) ==> TimerLaunch
(015B1C10) ==> TimerCancel
(015B1B50) ==> TimerPause
(015B1A90) ==> TimerUnpause
(015AE540) ==> MusicPlayForPeer
(015AE3B0) ==> MusicPlayForPeerWithInstrument
(015AE360) ==> MusicPlayOnCharacter
(015AE330) ==> MusicPlayGeneral
(015ADFB0) ==> MoviePlay
(015AE150) ==> PlayMovieForDialog
(015ADE70) ==> CameraActivate
(015AC3A0) ==> QuestUpdate
(015AC5B0) ==> QuestReceiveSharedUpdate
(015AC200) ==> QuestUpdateExists
(015ABFE0) ==> QuestAdd
(015AB880) ==> QuestAccepted
(015AB660) ==> QuestClose
(015AB5C0) ==> QuestCloseAll
(015AB060) ==> QuestIsShared
(015A2CB0) ==> SetInArena
(015A2EF0) ==> IsInArena
(015AB470) ==> QuestIsClosed
(015ABEA0) ==> QuestGetBroadcastLevel
(015ABC00) ==> QuestSetCategory
(015ABA90) ==> QuestArchive
(015AB1B0) ==> QuestHasUpdate
(015AB9C0) ==> QuestArchiveCategory
(015AAE40) ==> AddSecret
(015AADA0) ==> UnlockJournalRecipe
(015B1A50) ==> GameEnd
(015B19B0) ==> GameEndWithMovie
(015B1980) ==> GameEndWithMovieRequestCallback
(015B15F0) ==> EnqueueGameEndMovie
(015B1110) ==> EnqueueGameEndDialogMovie
(015B10A0) ==> FinalizeGameEndMovieQueue
(015B0E80) ==> SetGameEndMovie
(015B0E00) ==> ShroudRender
(015ADE60) ==> DebugBreak
(015B0BC0) ==> FadeToBlack
(015B0980) ==> FadeToWhite
(015B06B0) ==> OpenCustomBookUI
(015B05D0) ==> AddEntryToCustomBook
(015B0490) ==> RemoveEntryFromCustomBook
(015B0260) ==> OpenWaypointUI
(015B0180) ==> OpenWaypointUIForFlee
(015AFFF0) ==> CloseUI
(015AFF10) ==> OpenCraftUI
(015AFE20) ==> UnlockWaypoint
(015AFD50) ==> LockWaypoint
(015AFC80) ==> RegisterWaypoint
(015AFC10) ==> UnlockSecretRegion
(015AFBA0) ==> LockSecretRegion
(015AF7B0) ==> CreateSurface
(015AF8E0) ==> CreateSurfaceAtPosition
(015AF550) ==> CreatePuddle
(015AF2C0) ==> TransformSurface
(015AF050) ==> TransformSurfaceAtPosition
(015AEFF0) ==> GetSurfaceGroundAt
(015AEF90) ==> GetSurfaceCloudAt
(015AEE60) ==> GetSurfaceGroundOwnerAt
(015AED30) ==> GetSurfaceCloudOwnerAt
(015AECB0) ==> GetSurfaceTypeIndex
(015AEC70) ==> GetSurfaceNameByTypeIndex
(015AEA00) ==> DrawSurfaceOnPath
(015AE9A0) ==> StopDrawSurfaceOnPath
(015ADCA0) ==> PlayEffectAtPosition
(015ADAE0) ==> PlayEffectAtPositionAndRotation
(015AD8A0) ==> PlayScaledEffectAtPosition
(015AD840) ==> EndCombat
(015AD810) ==> StopLoopEffect
(015AD730) ==> MakePlayerActive
(015AD5E0) ==> CombatGetNumberOfInvolvedPlayers
(015AD490) ==> CombatGetNumberOfInvolvedPartyMembers
(015AD080) ==> CombatGetInvolvedPlayer
(015ACEF0) ==> CombatGetInvolvedPartyMember
(015AD410) ==> CombatGetIDForCharacter
(015AD360) ==> IsCombatActive
(015AD210) ==> CombatGetActiveEntity
(015ACB40) ==> CreateExplosion
(015AC7C0) ==> CreateExplosionAtPosition
(015AA410) ==> GetMultiplayerCharacter
(015AA180) ==> AutoSave
(015AA1C0) ==> ShowGameOverMenu
(015AA150) ==> OnCompanionDismissed
(015AA5A0) ==> NotifyGameProgress
(015AA660) ==> ShowNotification
(015A9F80) ==> ShowTutorial
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
(01591E60) ==> HasDefaultDialog
(01591BD0) ==> StartDefaultDialog

*/
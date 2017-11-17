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
	UInt64				handle;
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

//0x50
class ScriptFunctor
{
public:
	void				** vtbl;	//0x1BB8A10
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
#ifndef _INVNTORY_HXX
#define _INVNTORY_HXX
/* ========================================================================
   Copyright (c) 1990,1996   Synergistic Software
   All Rights Reserved.
   =======================================================================
   Filename: Invntory.hxx  -Header for the Inventory handling module
   Author:   Wes Cumberland
   ========================================================================
   Contains the following class declarations:
   ObjectList   -This manages each avatars personal list o' stuff. It uses
                 one central(static) array to store all the avatars things
                 in a linked-list/linked-array format.
   
   InventoryNode -This represents a slot in each character's inventory list.
                  It points to a certain type of object and says how many
                  the character is currently carrying.


   Contains the following general inline functions:
   (arguments only specified in overloaded cases)

   ObjectList::
    ObjectList     -Constructor
    mfInit         -Actually does the constructing of the object
    mfEndOfList    -returns whether or not you're at the end of the list
    mfReset()      -resets the cursor to the first node of the list
    mfReset(Category) -resets to the first node of the list in that category
    mfisEmpty      -returns whether or not the list is empty
    mfAdvance      -Advances the cursor to the next node/item
    mfData()       -returns the current item
    mfData(InvenIndex) -returns the specified item
    mfLength       -returns the length of the list (not tested)



   InventoryNode::
    InventoryNode  -Constructor
    mfInit         -Initializes the structure and points it to a node
    mfActivate     -Runs the effect function of the pointed to Item
    mfValidate     -makes the node valid so it won't be used by someone else
    mfisValid      -returns wether or not the node is valid
    mfInvalidate   -makes the node invalid so it can be reused
    mfGainOne      -increments the quantity
    mfLoseOne      -decrements the quantity
    mfHowMany      -returns the quantity
    mfType         -returns the type
    mfNext         -returns the index of the next node
    mfSetNext      -sets the next node link to a specified index


   Contains the following private inline functions:
   ObjectList::
    mfNotAlloced   -returns whether or not a Head node has been allocated
    mfCatsMatch    -tests to see if two categories match.

   ======================================================================== */
#include <stdio.h>
#if !defined(_TYPEDEFS_H)
#include "typedefs.h"
#endif

#if !defined(_GAMETYPE_HXX)
#include "gametype.hxx"   
#endif

#if !defined(_ITEMTYPE_HXX)
#include "itemtype.hxx"
#endif

#if !defined(_WEAPON_HXX)
#include "weapon.hxx"
#endif

#if !defined(_ARMOR_HXX)
#include "armor.hxx"
#endif

#if !defined(_TMSGBOX_HXX)
#include "tmsgbox.hxx"
#endif

#if !defined(_VECTOR_HXX)
#include "vector.hxx"
#endif


/* ------------------------------------------------------------------------
   Defines
   ------------------------------------------------------------------------ */
#define MAX_INVENTORY_ITEMS 3000 
#define MAX_SPELL_BOXES	6

#define NOT_ADDED		0x00000000
#define ADD_OK			0x00000001
#define EFFECT_ONLY		0x00000002
#define LIST_FULL		0x00000004
#define LIST_OVERFLOW	0x00000008
#define ADD_DUP			0x00000010
#define EFFECT_FAILED	0x00000020
#define ALLITEMS_FULL	0x00000040

typedef ULONG	AddItemStatus;
typedef SHORT  InvenIndex;
//typedef UBYTE  Category;



/* ------------------------------------------------------------------------
   Classes
   ------------------------------------------------------------------------ */
class InventoryNode;
class ItemType;
class InventoryGUI;
extern BOOL fDomainTurn;




extern "C" BOOL DoesPlayerHaveItem(THINGTYPE);

extern void CombatCastSpellBox(LONG hAvatar , LONG SpellBoxNumber );

extern void MultiAddItem(LONG iRegentIndex, LONG iItemtype, LONG amount);
extern void MultiDropItem(LONG iRegentIndex, LONG iItemtype);

/* ========================================================================
   Class       - InventoryNode
   Description - This represents a slot in each character's inventory list.
                 It points to a certain type of object and says how many
                 the character is currently carrying.
   Size        - 8 bytes
   ======================================================================== */
class InventoryNode
{
	private:
		
		ITEMTYPE Itemtype;
		SHORT Quantity;		
		InvenIndex NextItem;
		UBYTE Charges;
		UBYTE Attribs;		

		//attrib flags
		static UBYTE const fValid;		//valid
		static UBYTE const fWorn;		//currently equipped
		static UBYTE const fIgnorable;	//can ignore me in total counts, if you need to
		static UBYTE const fHidden;		//mfFind can't find me

	public:



		inline InventoryNode();

		inline void mfValidate();
		inline BOOL const mfisValid() const;
		inline void mfInvalidate();

		inline void mfGainOne();
		inline void mfLoseOne();
		inline SHORT const mfHowMany() const;

		inline void mfInit(ITEMTYPE const,
							SHORT const,
							SHORT const,
							InvenIndex const,
							SHORT const);

		inline ITEMTYPE const mfType() const;
		inline InvenIndex const mfNext() const;
		inline void mfSetNext(InvenIndex const);
		
		// Ease of data access fn.
		inline ItemCategory const mfCategory() const;
		
		inline BOOL mfWorn() const;
		inline void mfWear();
		inline void mfRemove();

		inline void mfDecrementCharges();
		inline void mfSetCharges(SHORT const);
		inline SHORT const mfGetCharges() const ;
		
		inline BOOL mfCanIgnore() const;
		inline void mfIgnore();
		void mfUnIgnore();
		
		inline BOOL mfHidden() const;
		
		
		void mfChangeTo(ITEMTYPE);
};


/* ========================================================================
   Class       - ObjectList
   Description - This manages each avatars personal list o' stuff. It uses
                 one central(static) array to store all the avatars things
                 in a linked-list/linked-array format.
   Size        - 19 bytes
   ======================================================================== */
class ObjectList
{
		friend class InventoryGUI;
		friend class StaticInventoryGUI;
		friend class InventoryItor;

	private:
		//static data
		static DECL_VECTOR_CLASS(InventoryNode,AllItems);
		static BOOL AllItemsInitted;
		static void mfInitAllItems();
		static BOOL MouseCallbackInitted;
		static UBYTE IDs;
		
		//member data
		UBYTE		OLID;
		SHORT		hAvatarStats;
		LONG		AvatarID;
		InvenIndex	SpellBoxes[MAX_SPELL_BOXES+1];
		InvenIndex	HeadIndex;


		//member functions
		inline void mfMakeHead(ITEMTYPE const,SHORT const, SHORT const);
		BOOL const mfCatsMatch(ItemCategory const,ItemCategory const);
		inline SHORT const mfGetAvatarID();
		inline void mfSetAvatarID(SHORT const);

	public:
		//static functions
		static void mfMouseCallback(LONG,LONG,LONG);

		static ITEMTYPE const mfGameTypetoItemType(THINGTYPE const GameType);

		//AllItems Stuff
		static InvenIndex mfFirstBlank(); //retrieves first blank in AllItems
		inline static InventoryNode& mfData(InvenIndex const);

		//iterator stuff
		inline InvenIndex const begin() const;			
		inline InvenIndex const end() const;		
		
		//List management stuff
		AddItemStatus const mfAddItem(ITEMTYPE const,SHORT const, SHORT const, BOOL fSend = TRUE);
		AddItemStatus const mfAddItem(ITEMTYPE const);
		inline BOOL const mfisEmpty();

		//these all have subtle differences...
		LONG const mfTotalItems();			//counts number of items
		LONG const mfTotalDifferentItems();	//counts number of different items
		LONG const mfLength();		  		//counts number of different items
		LONG const mfLength(ItemCategory const);//counts number of instances of a certain category
		LONG const mfCountItems();			//counts different items, ignoring items with charges


		LONG const mfNonEmptyLength(ItemCategory const);
		BOOL const mfDoesContain(ITEMTYPE const);
		BOOL const mfDoIHaveItem(THINGTYPE const);
		BOOL const mfCanICastSpell(ITEMTYPE const);
		LONG mfSellMundane(void);

		inline LONG const mfActivateSpell(ITEMTYPE const);

		void ClearSpells(SHORT AvatarStatsHdl);
		BOOL PrepBattleSpell(
			ITEMTYPE const Index,
			SHORT const AvatarStatsHdl
		);
		
		BOOL mfIsBattleSpell(LONG);
		
		//Initialization stuff
		void mfInit(SHORT,LONG);
		inline void mfDeinit();
		inline ObjectList();
		~ObjectList();

		//debug func.
		void mfPrint();

		//Save stuff	
		void mfSaveTo(FILE *fp);
		void mfLoad();
		void mfLoadFrom(FILE *fp);
		void ReadINV(SHORT hPlayerStats, LONG ID);
		void AddPriestSpells(SHORT hPlayerStats);

		inline UBYTE GetID() const;

		//spell box management...
		void mfAddToSpellBoxes(LONG const to,ItemCategory const cat);
		void mfClearSpellBox(LONG const a);
		InvenIndex const mfGetSpellBox(LONG const a) const;
		void mfActivateSpellBox(LONG const a);
		BOOL mfFindInSpellBoxes(InvenIndex ii,LONG* outIdx);
		BOOL mfFindInSpellBoxes(ITEMTYPE it,LONG* outIdx);
		void mfChangeSpellBox(LONG a, InvenIndex newI);
};

	 
// Class to go through the inventory.
class InventoryItor 
{
	public:
		inline InventoryItor(ObjectList & const);
		inline InventoryItor(InventoryItor & const X);
		inline InventoryItor(InventoryItor const & const X);
		inline ~InventoryItor();
	
		// for setting the iterator to the begining of the list.
		inline InventoryItor const &operator=(InvenIndex const Index);
	
		// for testing whether you are at the end of the list or not.
		inline BOOL const operator!=(InvenIndex const & rhs);
	
		inline BOOL const operator==(InvenIndex const &rhs);
		
		// Advance the Iterator to the next InventoryNode
		inline InventoryItor const operator++(int);
		inline InventoryItor const &operator++();
		inline InventoryNode * const operator*();
		inline InventoryNode * const mfData() const;
		inline InventoryNode * const mfPrevious() const;
		inline InventoryItor &mfAdvance();
	
		// Advance to the first item matching this category.
		inline InventoryItor &mfAdvance(ItemCategory const cat);
		
		// ignore slots with counts of zero
		inline InventoryItor &mfAdvanceNonEmpty(ItemCategory const cat);
		
		// Advance to the next item matching this category.
		inline InventoryItor &mfAdvanceToNext(ItemCategory const cat);
		
		// Advance to the next nonempty item matching this category.
		inline InventoryItor &mfAdvanceToNextNonEmpty(ItemCategory const cat);
		
		// Advance to this inventory index
		inline InventoryItor &mfAdvanceTo(SHORT const to);
		
		// Advance to this inventory index of this category
		inline InventoryItor &mfAdvanceTo(SHORT const to, ItemCategory const cat);
		
		// Advance to this inventory index of this category
		inline InventoryItor &mfAdvanceToNonEmpty(SHORT const to, ItemCategory const cat);
		
		LONG const mfActivateCurrent();
		
		void mfDeleteItem();
		
		void mfDropItem(BOOL fSend = TRUE);
		void mfDropItem(LONG const /* x */,LONG const /* y */);
		
		BOOL const mfFind(ITEMTYPE const Type);
		BOOL const mfFind(ITEMTYPE const Type, ItemCategory const);
		void mfDropItem(LONG const /* x */,LONG const /* y */, BOOL);
		
		inline void mfLast();
		
		inline InvenIndex const mfGetCurrentInvIndex();

		void mfChangeTo(ITEMTYPE);
		
		BOOL mfIgnoreCurrent();
		
		
	protected:
	private:
		ObjectList & const fInventory;
		InvenIndex 		   fCurrentIndex;
		InvenIndex 		   fPrevIndex;
};


/* ------------------------------------------------------------------------
   Inline Member Functions
   ------------------------------------------------------------------------ */


inline ObjectList::ObjectList()
{
	mfInit(fERROR,fERROR);
}


inline void ObjectList::mfDeinit()
{
	this->~ObjectList();
}	
	

inline SHORT const ObjectList::mfGetAvatarID()      
{
	//printf("Ol%i: Getting AvatarID=%li from AvSt %li\n",OLID,AvatarID,hAvatarStats);
	return AvatarID;
}

inline void ObjectList::mfSetAvatarID(SHORT const a)
{
	//printf("Ol%i: Setting AvatarID=%li from AvSt %li\n",OLID,a,hAvatarStats);
	AvatarID=a;
}



inline BOOL const ObjectList::mfisEmpty()
{
	return (begin() == end());
}

inline InventoryNode& ObjectList::mfData(InvenIndex const idx)
{
	return AllItems[idx];
}

inline InvenIndex const ObjectList::begin() const 
{
	return HeadIndex; 
}
	
inline InvenIndex const ObjectList::end() const 
{
	return fERROR;
}



inline LONG const ObjectList::mfActivateSpell(ITEMTYPE const itype)
{
	LONG MakeMe=BUILD_LONG(itype,hAvatarStats);
	return ItemTypes[itype].mfEffect(MakeMe,ACTIVATE);
}

inline UBYTE ObjectList::GetID() const
{
	return OLID;
}


inline InventoryNode::InventoryNode()
:Itemtype(ITEM_NO_THING),Quantity(0),NextItem(-1),Attribs(0),Charges(0)
{
}

inline BOOL InventoryNode::mfWorn() const
{
	return Attribs&fWorn;
}
	
inline void InventoryNode::mfWear()  
{
	Attribs|=fWorn;
}

inline void InventoryNode::mfRemove()
{
	Attribs&=(~fWorn);
}

inline BOOL InventoryNode::mfHidden() const
{	
	return Attribs&fHidden;
}

inline void InventoryNode::mfDecrementCharges()
{
	if (Charges > 0)
		--Charges;
}

inline void InventoryNode::mfSetCharges(SHORT const val)
{
#if defined(_DEBUG)
	if (val > UCHAR_MAX)
	{
		fatal_error("INVENTORY: Overflow on number of charges.\n");
	}
#endif
	Charges = val;
}

inline SHORT const InventoryNode::mfGetCharges() const
{
	return (SHORT) Charges;
}

inline BOOL InventoryNode::mfCanIgnore() const
{
	return Attribs&fIgnorable;
}
inline void InventoryNode::mfIgnore()
{
 	Attribs|=fIgnorable;
}
inline void InventoryNode::mfUnIgnore()
{
	Attribs&=(~fIgnorable);
}


inline void InventoryNode::mfInit(
	ITEMTYPE const it,
	SHORT const quan,
	SHORT const NumCharges,
	InvenIndex const Next,
	SHORT const valid)
{
	Itemtype=it;
	Quantity=quan;
	NextItem=Next;
	Attribs=0;
	Charges=NumCharges;
	if (valid)
		mfValidate();
	else
		mfInvalidate();
}


inline void InventoryNode::mfValidate()
{
	Attribs|=fValid;
}

inline BOOL const InventoryNode::mfisValid() const
{ 
	return Attribs&fValid;
}

inline void InventoryNode::mfInvalidate()
{
	Attribs&=(~fValid);
}

inline ItemCategory const InventoryNode::mfCategory() const
{
	return (ItemTypes[Itemtype].mfCategory());
}
			

inline void InventoryNode::mfGainOne()
{
	if (Quantity < SHRT_MAX)
		++Quantity;
}

inline void InventoryNode::mfLoseOne()
{
	--Quantity;
	if (Quantity <0)
	{
		Quantity = 0;
	}
}

inline SHORT const InventoryNode::mfHowMany() const
{
	return Quantity;
}

inline ITEMTYPE const InventoryNode::mfType() const
{
	return Itemtype;
}

inline InvenIndex const InventoryNode::mfNext() const
{
	return NextItem;
}

inline void InventoryNode::mfSetNext(InvenIndex const n)
{
	NextItem=n;
}


inline InventoryItor::InventoryItor(ObjectList & const Inventory) :
	fInventory(Inventory),
	fCurrentIndex(Inventory.begin()),
	fPrevIndex(Inventory.end())
{
}

inline InventoryItor::InventoryItor(InventoryItor & const X) :
	fInventory(X.fInventory),
	fCurrentIndex(X.fCurrentIndex),
	fPrevIndex(X.fPrevIndex)
{
}

inline InventoryItor::InventoryItor(InventoryItor const & const X) :
	fInventory(X.fInventory),
	fCurrentIndex(X.fCurrentIndex),
	fPrevIndex(X.fPrevIndex)
{
}

inline InventoryItor::~InventoryItor()
{
}

// for setting the iterator to the begining of the list.
inline InventoryItor const & InventoryItor::operator=(InvenIndex const Index) 
{
	fCurrentIndex = Index;
	return *this;
}

// for testing whether you are at the end of the list or not.
inline BOOL const InventoryItor::operator!=(InvenIndex const & rhs)
{
	return (fCurrentIndex != rhs);
}

inline BOOL const InventoryItor::operator==(InvenIndex const &rhs)
{
	return (fCurrentIndex == rhs);
}

// Advance the Iterator to the next InventoryNode
inline InventoryItor const InventoryItor::operator++(int) 
{
	InventoryItor const OldVal(*this);
	
	fPrevIndex = fCurrentIndex;
	if (fCurrentIndex >= 0 || fCurrentIndex < MAX_INVENTORY_ITEMS)
	{
		fCurrentIndex=ObjectList::AllItems[fCurrentIndex].mfNext();
	}
	return OldVal;
}
// Advance the Iterator to the next InventoryNode
inline InventoryItor const & InventoryItor::operator++() 
{
	fPrevIndex = fCurrentIndex;
	if (fCurrentIndex >= 0 || fCurrentIndex < MAX_INVENTORY_ITEMS)
	{
		fCurrentIndex=ObjectList::AllItems[fCurrentIndex].mfNext();
	}
	return *this;
}

inline InventoryNode * const InventoryItor::operator*()
{
	if (fCurrentIndex < 0 || fCurrentIndex >= MAX_INVENTORY_ITEMS)
	{
		return 0;
	}
	
	return (&ObjectList::AllItems[fCurrentIndex]);
}

inline InventoryNode * const InventoryItor::mfData() const
{
	if (fCurrentIndex < 0 || fCurrentIndex >= MAX_INVENTORY_ITEMS)
	{
		return 0;
	}
	
	return (&ObjectList::AllItems[fCurrentIndex]);
}

inline InventoryNode * const InventoryItor::mfPrevious() const
{
	if (fPrevIndex < 0 || fPrevIndex >= MAX_INVENTORY_ITEMS)
	{
		return 0;
	}
	
	return (&ObjectList::AllItems[fPrevIndex]);
}

inline InventoryItor & InventoryItor::mfAdvance()
{
	++(*this);
	return *this;
}

// Advance to the first item matching this category.
inline InventoryItor & InventoryItor::mfAdvance(ItemCategory const cat)
{
	while(fCurrentIndex != fInventory.end())
	{
		ItemCategory const DataCat = mfData()->mfCategory();
		if (fInventory.mfCatsMatch(DataCat, cat))
			break;
		mfAdvance();
	}
	
	return *this;
}

// ignore slots with counts of zero
inline InventoryItor & InventoryItor::mfAdvanceNonEmpty(ItemCategory const cat)
{
	while(fCurrentIndex != fInventory.end())
	{
		ItemCategory const DataCat = mfData()->mfCategory();
		if (fInventory.mfCatsMatch(DataCat, cat) &&
			mfData()->mfHowMany() > 0)
			break;
		mfAdvance();
	}
	return *this;
}

// Advance to the next item matching this category.
inline InventoryItor & InventoryItor::mfAdvanceToNext(ItemCategory const cat)
{
	if (fCurrentIndex !=fInventory.end())
	{
		mfAdvance();
		mfAdvance(cat);
	}
	return *this;
}

// Advance to the next nonempty item matching this category.
inline InventoryItor & InventoryItor::mfAdvanceToNextNonEmpty(ItemCategory const cat)
{
	if (fCurrentIndex !=fInventory.end())
	{
		mfAdvance();
		mfAdvanceNonEmpty(cat);
	}
	return *this;
}

// Advance to this inventory index
inline InventoryItor & InventoryItor::mfAdvanceTo(SHORT const to)
{
	LONG i;
	for (fPrevIndex = fCurrentIndex = fInventory.begin(), i = 0;
	     fCurrentIndex != fInventory.end() && i < to;
	     ++i, mfAdvance())
	{ }
	return *this;
}

// Advance to this inventory index of this category
inline InventoryItor & InventoryItor::mfAdvanceTo(SHORT const to, ItemCategory const cat)
{
	LONG i;
	
	fPrevIndex = fCurrentIndex = fInventory.begin();
	for (mfAdvance(cat), i=0;
	     fCurrentIndex != fInventory.end() && i < to;
	     ++i, mfAdvanceToNext(cat))
	{
	}
	
	return *this;
}

// Advance to this inventory index of this category
inline InventoryItor & InventoryItor::mfAdvanceToNonEmpty(SHORT const to, ItemCategory const cat)
{
	LONG i;
	
	fPrevIndex = fCurrentIndex = fInventory.begin();
	for (mfAdvanceNonEmpty(cat), i=0;
	     fCurrentIndex != fInventory.end() && i < to;
	     ++i, mfAdvanceToNextNonEmpty(cat))
	{
	}
	
	return *this;
}

inline void InventoryItor::mfLast()
{
	InvenIndex PriorPrevious = fPrevIndex;
	while(fCurrentIndex != fInventory.end())
	{
		PriorPrevious = fPrevIndex;
		mfAdvance();
	}
	fCurrentIndex = fPrevIndex;
	fPrevIndex = PriorPrevious;
}

inline InvenIndex const InventoryItor::mfGetCurrentInvIndex()
{	
	return fCurrentIndex; 
}


/* ========================================================================
   Function    - ObjectList::mfMakeHead()
   Description - Finds an invalid Item entry and makes a head for the list
   Returns     - void
   ======================================================================== */
inline void ObjectList::mfMakeHead(ITEMTYPE const it,
                            SHORT const quan,
                            SHORT const charges)
{
	InvenIndex NewHead=mfFirstBlank();

	AllItems[NewHead].mfInit(it,quan, charges, fERROR, TRUE); //circular list

	HeadIndex=NewHead;
}

#endif

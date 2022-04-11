#ifndef ITEMEQP_HXX
#define ITEMEQP_HXX


#if !defined(_VECTOR_HXX)
#include "vector.hxx"
#endif

typedef enum
{
	CantBox=-1,
	NoEquip=0,
	Helm,
	Necklace,
	Ring,
	Gauntlets,
	Bracers,
	Armor, 
	Boots,
	Unlimited,
} ItemEquipType;



extern DECL_VECTOR_CLASS(ItemEquipType,EquipTypes);
extern LONG EquipMax[];
		 
#endif

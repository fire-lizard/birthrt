//effsepar.hxx 
//class to seperate the Old values that sometimes need to be stored in 
//the effects of Items.
//only way to do this before was a static variable, which meant that only 1 
//item of this type could be in use at a time.

#if !defined(_SYSTEM_H)
#include "system.h"
#endif


//max of the same item running concurrently in the world
#define MAX_DIFFERENT_EFFECTS 10


template <class ValType>
class EffectDiffer
{
	public:
		SHORT hAvatarStats;
		ValType Val;
		
		EffectDiffer(SHORT,ValType);
		EffectDiffer();
};

template <class ValType>
class EffectSeparator
{
	private:
		
		EffectDiffer<ValType> Effects[MAX_DIFFERENT_EFFECTS];
		
	public:

		EffectSeparator();		
 		const ValType* mfGetVal(SHORT);
		BOOL mfAddEffect(SHORT,ValType);
		BOOL mfRemoveEffect(SHORT);
		BOOL mfChangeEffectValue(SHORT,ValType);
		
};



template <class ValType>
EffectDiffer<ValType>::EffectDiffer(SHORT hAS,ValType v)
:hAvatarStats(hAS), Val(v)
{
}

template <class ValType>
EffectDiffer<ValType>::EffectDiffer()
:hAvatarStats(fERROR)
{
}


template <class ValType>
inline EffectSeparator<ValType>::EffectSeparator()
{
	for (LONG i=0;i<MAX_DIFFERENT_EFFECTS;i++)
		Effects[i].hAvatarStats=fERROR;
}


template <class ValType>
const ValType* EffectSeparator<ValType>::mfGetVal(SHORT hAvStats)
{
	if (hAvStats==fERROR)
	{
#ifdef DEBUG
		fatal_error("fERROR passed to mfGetVal %s(%li)\n",__FILE__,__LINE__);
#endif
		return NULL;
	}

	for (LONG i=0;i<MAX_DIFFERENT_EFFECTS;i++)
		if (hAvStats==Effects[i].hAvatarStats)
			return &Effects[i].Val;

#ifdef DEBUG
	fatal_error("couldn't find avatar stats in Effect list %s(%li)\n",__FILE__,__LINE__);
#endif
	return NULL;
}

template <class ValType>
BOOL EffectSeparator<ValType>::mfAddEffect(SHORT hAS,ValType v)
{
	LONG i=0;

	for (i=0;i<MAX_DIFFERENT_EFFECTS;i++)
		if (Effects[i].hAvatarStats==fERROR)
		{
			//check the rest of the list, to see if he's already got an
			//effect there.
			for (LONG j=i;j<MAX_DIFFERENT_EFFECTS;j++)
				if (Effects[j].hAvatarStats==hAS)
					return FALSE;

			Effects[i].hAvatarStats=hAS;
			Effects[i].Val=v;
//			printf("added value at %li\n",i);
			return TRUE;
		}
	return FALSE;
}

template <class ValType>
BOOL EffectSeparator<ValType>::mfRemoveEffect(SHORT hAS)
{
	for (LONG i=0;i<MAX_DIFFERENT_EFFECTS;i++)
		if (Effects[i].hAvatarStats==hAS)
		{
			Effects[i].hAvatarStats=fERROR;
//			printf("removed value from %li\n",i);
			return TRUE;
		}
	return FALSE;
}

template <class ValType>
BOOL EffectSeparator<ValType>::mfChangeEffectValue(SHORT hAS,ValType NewVal)
{
	ValType* pVal=(ValType*)mfGetVal(hAS);
	
	if (pVal)
	{
		*pVal=NewVal;
		return TRUE;
	}
	else
		return FALSE;
}
	

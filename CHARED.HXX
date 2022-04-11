// ===========================================================================
//
// CHARED.HXX
//
// ===========================================================================

#if !defined (_CHARED_HXX_)
#define _CHARED_HXX_

#include "itemtype.hxx"

// We have our own enums here to omit the magician.
enum CHARED_CLASSES {
	CLASS_FIGHTER,
	CLASS_PALADIN,
	CLASS_RANGER,
	CLASS_PRIEST,
	CLASS_WIZARD,
	CLASS_THIEF,
	CLASS_BARD
};


BOOL IsBloodabAvail (ITEMTYPE iItem);
LONG GetBloodlineGrade ();
void CharedDoEditorDialog ();

#endif

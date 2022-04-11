
/* ========================================================================
   Copyright (c) 1996  Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: DOORS.H
   Author: John Conley
   ======================================================================== */

#if !defined(_DOORS_H)
#define _DOORS_H

#if defined (__cplusplus)
extern "C" {
#endif

// There are 3 switch textures in the game; when user activates one,
// change the texture to the other one.
#define cSWITCH_TEXTURE_PAIRS 3
extern LONG gSwitchTexture[cSWITCH_TEXTURE_PAIRS][2];

void init_doors (void);
void stop_door (LONG s);
void handle_doors (LONG arg);
void open_door (LONG seg, LONG spcl);
void HandleLifts (LONG arg);
void HandleFloors (LONG arg);
void HandleCeilings (LONG arg);
void DoorActivate (LONG iLinedef, LONG tag, LONG special);
void LiftActivate (LONG iLinedef, LONG tag, LONG special);
BOOL FloorIsActive (LONG iSector);
void FloorActivate (LONG iLinedef, LONG tag, LONG special);
BOOL CeilingIsActive (LONG iSector);
void CeilingActivate (LONG iLinedef, LONG tag, LONG special);

BOOL LinedefIsSwingingDoor(LONG Ldidx);

void DoorInfo (LONG special);

#if defined (__cplusplus)
}
#endif


#endif

/* ========================================================================
   Copyright (c) 1996   Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: SECTORS.H
   ======================================================================== */
#if !defined(_SECTORS_H)
#define _SECTORS_H	

#if defined (__cplusplus)
extern "C" {
#endif

void point_to_sector_info( LONG x, LONG y, LONG *sec, LONG *f, LONG *c, LONG *s, LONG *t);
void ssector_to_sector_info( LONG x, LONG y, ULONG PointSsector, LONG *sector, LONG *floor, LONG *ceiling, LONG *special, LONG *tag );
void sector_info( LONG sector, LONG *floor, LONG *ceiling, LONG *special, LONG *tag );
LONG raise_sector_floor(LONG s,LONG inc);
LONG raise_sector_ceiling(LONG s,LONG inc);
ULONG point_to_sector(LONG x,LONG y);
long point_to_sector_special(long x, long y);
LONG point_to_floor_height(LONG x,LONG y);
LONG point_to_ceiling_height(LONG x,LONG y);
ULONG find_ssector(LONG x,LONG y);
LONG tag_to_sector(LONG tag);
LONG TagToNextSector (LONG iStart, LONG tag);
LONG tag_to_line(LONG tag, LONG notline);
void activate_seg(LONG arg);
// [d11-14-96 JPC] Removed: BOOL GetSectorDamage (LONG iSector, LONG *damageNumber);
// [d11-29-96 JPC] Removed: BOOL GetCrusherDamage (LONG iSector, LONG *damageNumber, LONG avatarHeight, LONG avatarZ);
BOOL IsSplashSector (LONG iSector);
BOOL IsLavaSector (LONG iSector);

#if defined (__cplusplus)
}
#endif


#endif

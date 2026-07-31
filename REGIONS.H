/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   Author: G. Powell
   ======================================================================== */
#ifndef _REGIONS_H
#define _REGIONS_H

/* ------------------------------------------------------------------------
   Sub Includes
   ------------------------------------------------------------------------ */
#if !defined(_REGIONS_H)
#include "TYPEDEFS.H"
#endif

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Enums
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Typedefs
   ------------------------------------------------------------------------ */
typedef enum {
	REGION_EVENT_NOT_HANDLED,
	REGION_HANDLED_EVENT
} REGION_EVENT_TYPE;

// Structure to return the previous key state in add_key & del_key.
typedef struct _KeyStruct {
	REGARG	val;
	REGARG	val2;
	PFVLL	func;
} KEYSTRUCT, *PTR_KEYSTRUCT;

typedef enum {
		CHANGE_ALL_REGIONS = 6,
		CHANGE_TOP_STACK = -12
	} CHANGE_REGION_MODE;

/* ------------------------------------------------------------------------
   Macros   
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
#if defined (__cplusplus)
extern "C" {
#endif

/* regions.c */
void add_key(LONG /* key */,
				  PFVLL /* func */,
				  REGARG /* val */,
				  REGARG /* val2 */);
#define replace_key(key,func,val, pStruct) replace_key_vals((key), (func), (val), 0, (pStruct))
void replace_key_vals(LONG /* key */,
                 PFVLL /* pNewFunc */,
                 REGARG /* NewVal */,
                 REGARG /* NewVal2 */,
                 PTR_KEYSTRUCT /* pOldKeyStruct */);
void del_key(LONG /* key */, 
             PTR_KEYSTRUCT /* pKeyStruct */);

LONG add_region(LONG x,LONG y,LONG w,LONG h,LONG key,PFVLL func,REGARG val, REGARG val2,LONG id, int idToolTip);
void del_region(PFVLL func,LONG key);
void del_all_regions(void);
REGION_EVENT_TYPE check_regions();
void paint_tooltips(void);
void init_regions(void);
void push_regions(void);
void pop_regions(void);
void activate_region(LONG id, BOOL state_on);
void OutlineAllRegions(void);
BOOL change_tooltip(PFVLL func, LONG key, LONG x, LONG y, 
					LONG newTooltip,
					CHANGE_REGION_MODE crMode);
BOOL change_function(PFVLL Oldfunc, 
                     LONG Oldkey, 
                     LONG x, LONG y, 
                     PFVLL NewFunc,
					 LONG NewKey,
					 REGARG NewVal,
					 REGARG NewVal2,
					 CHANGE_REGION_MODE crMode);

BOOL del_region_xy(PFVLL Oldfunc,
				   LONG x, 
				   LONG y, 
				   LONG w, 
				   LONG h, 
				   CHANGE_REGION_MODE crMode);
#if defined (__cplusplus)
}

/* Overloads for the handful of callbacks that take a pointer and so are
   declared void(REGARG,REGARG) rather than void(LONG,LONG). Everything else
   still matches PFVLL above and is unaffected. Cast the function pointer
   directly - laundering it through void* is only conditionally supported. */
inline LONG add_region(LONG x,LONG y,LONG w,LONG h,LONG key,PFVPP func,REGARG val,REGARG val2,LONG id,int idToolTip)
{ return add_region(x,y,w,h,key,(PFVLL)func,val,val2,id,idToolTip); }
inline void add_key(LONG key,PFVPP func,REGARG val,REGARG val2)
{ add_key(key,(PFVLL)func,val,val2); }
inline void del_region(PFVPP func,LONG key)
{ del_region((PFVLL)func,key); }
#endif
#endif // _REGIONS_H

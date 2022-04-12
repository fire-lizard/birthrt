/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: combmenu.hxx
   Author:   Gary Powell
   
   ========================================================================
   
   Contains the following general functions:

   
   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */

#include "COMBATUI.HXX"
#include "COMBCNTL.HXX"
#include "COMBOPTS.HXX"
#include "GAME.H"
#include "HANDLE.HXX"
#include "ITEMTYPE.HXX"
#include "PANEL.H"
#include "PLAYHIT.HXX"
#include "STRMGR.H"
#include "SCENE.HXX"

/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
#define MAX_BITMAP_PATH_LEN	40
#define MAX_STATBAR_LEN	40

/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
   
/* ------------------------------------------------------------------------
   Classes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
DEFINE_VECTOR_MEMBER_DATA_S(CONTROL_PANELS::CURRENT_CONTROL,CONTROL_PANELS,fccActive,MAX_NUMBER_CONTROLS) = {
	CONTROL_PANELS::CURRENT_CONTROL(),
	CONTROL_PANELS::CURRENT_CONTROL(),
	CONTROL_PANELS::CURRENT_CONTROL(),
	CONTROL_PANELS::CURRENT_CONTROL()
	};
DEFINE_VECTOR_MEMBER_CLASS(CONTROL_PANELS::CURRENT_CONTROL,CONTROL_PANELS,fccActive);

SHORT CONTROL_PANELS::fsccIndex = 0; // start out with the leader as the active one.
SHORT CONTROL_PANELS::fPrevsccIndex = 0; // start out with the leader as the active one.
BOOL CONTROL_PANELS::fCurrentlyRPG_Mode	= FALSE;	// Start out in either mode 
                                                    // But if it changes update the menus.
/* ------------------------------------------------------------------------
   Local Variables
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
static void CombatSelectThisAdventurer(CThing *);
static void CombatSelectThisEnemy(CThing *);

/* ========================================================================
   Function    - CombatSelectThisAdventurer
   Description - Click on an adventurer and cause this one to become active.
   Returns     - 
   ======================================================================== */

static void CombatSelectThisAdventurer(CThing *pAvatar)
{
	CONTROL_PANELS::mfMatchAvatarHdlToCurrent(((CAvatar *)pAvatar)->hThis);
}

/* ========================================================================
   Function    - CombatSelectThisEnemy
   Description - When clicked on become the enmey of the selected avatar.
   Returns     - 
   ======================================================================== */

static void CombatSelectThisEnemy(CThing * pAvatar)
{
	SHORT const hControledAvatar = CONTROL_PANELS::mfGetCurrAvatarHdl();
	
	if (hControledAvatar != fERROR)
	{
		CAvatar * const pControlledAvatar = (CAvatar * const ) BLKPTR(hControledAvatar);
		pControlledAvatar->mfSethEnemy(((CAvatar *)pAvatar)->hThis);
		
		if (pControlledAvatar->Status == CAvatar::AI_DEFEND ||
		     (pControlledAvatar->Status == CAvatar::AI_PAUSED && 
		      pControlledAvatar->fPrev.Status == CAvatar::AI_DEFEND))
		{
			// Just default to the first combat unless they are already
			// using a different mode.
			pControlledAvatar->mfSetAttackMode(FIGHT_SEQUENCE::ATM_HIGH);
		}
	}
}

/* ========================================================================
   Function    - mfSetEndCombat
   Description - Disengage the avatar and stop combat.
   Returns     - 
   ======================================================================== */
void CONTROL_PANELS::CURRENT_CONTROL::mfSetEndCombat()
{
	if (fhAvatar != fERROR)
	{
		LONG i;
		CAvatar *const pThisAvatar = (CAvatar *) BLKPTR(fhAvatar);
		
		if (!pThisAvatar->mfAmIImmoblized())
		{
			// GWP Until I figure a way to reattach to the adventure team.
			// GWP don't let me leave them wandering in the Wad.
			// GWP pAvatar->Status = fOriginalStatus;
			pThisAvatar->Status = CAvatar::AI_RUNNING;
			pThisAvatar->mfSetAngle(fOriginalAngle);
		}
		
		pThisAvatar->mfSetLeftButtonFn(fOriginalLeftButtonFn);
		
		SCENE const * const pCurrentScene = (SCENE const * const) BLKPTR(SCENE_MGR::hCurrentScene);

		// disconnect all the avatars from attacking me.
		for (i = 0; i < MAX_AVATARS; i++)
		{
			if (pCurrentScene->Avatars[i] != fERROR)
			{
				CAvatar * const pAvatar = (CAvatar * ) BLKPTR(pCurrentScene->Avatars[i]);
				if(pAvatar->hEnemy == fhAvatar)
				{
					pAvatar->mfSethEnemy(fERROR);
				}
			}
		}
		
		pThisAvatar->Engaged = 0;
		
		// Disconnect from attacking anyone else.
		if(pThisAvatar->hEnemy != fERROR)
		{
			CAvatar * const myEnemy = (CAvatar * const) BLKPTR(pThisAvatar->hEnemy);
			myEnemy->mfDisEngage();
			pThisAvatar->mfSethEnemy(fERROR);
		}
	}
}
/* ========================================================================
   Function    - mfSetControl
   Description - Attach the avatar to the menu buttons & show the menu.
   Returns     - 
   ======================================================================== */
void CONTROL_PANELS::CURRENT_CONTROL::mfSetControl(
		SHORT const hMenu,
        SHORT const hAvatar,
        BOOL  const SpeakFlag,
        LONG  const MaxSpellBoxes)
{
	if (hMenu != fERROR)
	{
		fMaxSpellBoxes = MaxSpellBoxes;
		
		if (CONTROL_PANELS::CURRENT_CONTROL::mfIsFighterCombatPanel(hMenu))
		{
			SetButtonArt(hMenu, 0, COMBAT_FIGHTER_PANEL);
		}
		else
		{
			SetButtonArt(hMenu, 0, COMBAT_REGULAR_PANEL);
		}
	
		// Attach the avatar to the combat menu
		if (hAvatar != fERROR)
		{
			LONG i;
			
			CAvatar  const * const pAvatar = (CAvatar const * const) BLKPTR(hAvatar);
			
			if (COMBAT_MODE::mfIsModeRPG())
			{
				SetButtonProc(hMenu, BUTNO_DEF_FALL_BTN, CombatFallbackKey, 0, 0);
				SetButtonToolTip(hMenu, BUTNO_DEF_FALL_BTN, COMBAT_STR_FALLBACK);
			}
			else
			{
				SetButtonProc(hMenu, BUTNO_DEF_FALL_BTN, CombatDefendKey, 0, 0);
				SetButtonToolTip(hMenu, BUTNO_DEF_FALL_BTN, COMBAT_STR_DEFEND);
			}
			
			DumbHandlePtr<PLAYER_STATS const> const dhPlayerStats(pAvatar->hPlayerStats);
			for (i=0;i<MaxSpellBoxes;i++)
			{
				InvenIndex const sSpell = dhPlayerStats->Inventory.mfGetSpellBox(i);
				
				if (sSpell >-1)
				{
					ITEMTYPE const SpellType = ObjectList::mfData(sSpell).mfType();
					THINGTYPE SpellThing = ItemTypes[SpellType].mfGameType();
					SetButtonToolTip(hMenu, BUTNO_SPELL1 + i, GAME_TTYPE::mfGetSTRMgrId(SpellThing));
				}
				else
				{
					SetButtonToolTip(hMenu, BUTNO_SPELL1 + i, fERROR);
				}
			}
		}
		
		ShowPanel(hMenu);
	}
	fhMenu = hMenu;
	fhAvatar = hAvatar;
	fSpeakFlag = SpeakFlag;
}
/* ========================================================================
   Function    - mfRelease
   Description - Clean up as we leave combat ui
   Returns     - 
   ======================================================================== */
void CONTROL_PANELS::CURRENT_CONTROL::mfRelease() 
{
	if (fhMenu != fERROR)
	{
		HidePanel(fhMenu);
		fhMenu = fERROR;
	}
	
	if (fhAvatar != fERROR)
	{
		CAvatar * const pAvatar = (CAvatar * const) BLKPTR(fhAvatar);
		pAvatar->mfTurnHighlightOff();
		fhAvatar = fERROR;
	}
}

/* ========================================================================
   Function    - mfPaintPanel
   Description - Paint the panel data for this avatar.
   Returns     - 
   ======================================================================== */
void CONTROL_PANELS::CURRENT_CONTROL::mfPaintPanel()
{
	if (fhAvatar == fERROR)
		return;
		
	CAvatar const * const pAvatar = (CAvatar const * const )BLKPTR(fhAvatar);
	extern BOOL mundane_descriptions;
	
	if (pAvatar->hPlayerStats != fERROR)
	{
		LONG	i;
		LONG	x;
		LONG	y;
		LONG	height;
		LONG	width;
		SHORT	color;
		SHORT	hBitm;
		char	cpBuffer[256];
		char 	cpFormatBuffer[128];
		char	cpNameBuffer[80];
						
		
		GetButtonPosition ( fhMenu, BUTNO_AVATAR_HEALTH, &x, &y );
		
		GetButtonSize ( fhMenu, BUTNO_AVATAR_HEALTH, &width, &height );
		
		DumbHandlePtr<PLAYER_STATS> const dhPlayerStats(pAvatar->hPlayerStats);
		
		LONG const health = 
			(dhPlayerStats->mfGetCurHitPoints() * 100) / dhPlayerStats->mfGetMaxHitPoints()
			* height / 100;
		
		// check health percentage
		if ((health*100/height) > 50)
			color = GREEN;
		else
		if ((health*100/height) > 25)
			color = YELLOW;
		else
			color = RED;
			
		// show his health
		color_rect( x, y, width, height, 1); // Erase the previous bar.
		if (health > 0)
		{
			color_rect( x, y + (height - health), width, health, color);
		}
		
		GetButtonPosition(fhMenu, BUTNO_AVATAR_ICON, &x, &y);
		GetButtonSize ( fhMenu, BUTNO_AVATAR_ICON, &width, &height );
		
		// First Paint Hightlighted background or not.
		if (fhAvatar == CONTROL_PANELS::mfGetCurrAvatarHdl())
		{
			hBitm = GetResourceStd ("ui\\CMBFLD1B.PCX", FALSE);
		}
		else
		{
			hBitm = GetResourceStd ("ui\\CMBFLD1A.PCX", FALSE);
		}
		if (hBitm != fERROR)
		{
			ClearRemapTable();
			DrawBitmap ((SHORT)x, (SHORT)y, hBitm, 0, 0, (SHORT)width, (SHORT)height);
			SetPurge(hBitm);
		}
		
		sprintf(cpBuffer, "ui\\PORTS_S\\SM%s.PCX", GAME_TTYPE::mfGetArtFileName(pAvatar->mfType()));
		hBitm = GetResourceStd (cpBuffer, FALSE);
		if (hBitm != fERROR)
		{
			SetRemapTable((USHORT)pAvatar->Realm.HomeRealm);
			DrawBitmap ((SHORT)x, (SHORT)y, hBitm, 0, 0, (SHORT)width, (SHORT)height);
			ClearRemapTable();
			SetPurge(hBitm);
		}
		
		
		// Now paint the attack buttons.
		GetButtonPosition(fhMenu, BUTNO_HIGH_ATTACK, &x, &y);
		GetButtonSize ( fhMenu, BUTNO_HIGH_ATTACK, &width, &height );
		
		if ((pAvatar->Status == CAvatar::AI_ATTACK ||
		     (pAvatar->Status == CAvatar::AI_PAUSED && 
		      pAvatar->fPrev.Status == CAvatar::AI_ATTACK)) &&
		     pAvatar->mfGetAttackMode() == FIGHT_SEQUENCE::ATM_HIGH )
		{
			hBitm = GetResourceStd ("ui\\CMBATK1B.PCX", FALSE);
		}
		else
		{
			hBitm = GetResourceStd ("ui\\CMBATK1A.PCX", FALSE);
		}
		
		if (hBitm != fERROR)
		{
			ClearRemapTable();
			DrawBitmap ((SHORT)x, (SHORT)y, hBitm, 0, 0, (SHORT)width, (SHORT)height);
			SetPurge(hBitm);
		}
		
		GetButtonPosition(fhMenu, BUTNO_QHIGH_ATTACK, &x, &y);
		GetButtonSize ( fhMenu, BUTNO_QHIGH_ATTACK, &width, &height );
		
		if ((pAvatar->Status == CAvatar::AI_ATTACK ||
		     (pAvatar->Status == CAvatar::AI_PAUSED && 
		      pAvatar->fPrev.Status == CAvatar::AI_ATTACK)) &&
		     pAvatar->mfGetAttackMode() == FIGHT_SEQUENCE::ATM_QUICK_HIGH )
		{
			hBitm = GetResourceStd ("ui\\CMBATK3B.PCX", FALSE);
		}
		else
		{
			hBitm = GetResourceStd ("ui\\CMBATK3A.PCX", FALSE);
		}
		
		if (hBitm != fERROR)
		{
			ClearRemapTable();
			DrawBitmap ((SHORT)x, (SHORT)y, hBitm, 0, 0, (SHORT)width, (SHORT)height);
			SetPurge(hBitm);
		}
		
		// Now paint the attack2 buttons.
		if (GAME_TTYPE::mfHasAttack2Animation(pAvatar->mfType()))
		{
			GetButtonPosition(fhMenu, BUTNO_LOW_ATTACK, &x, &y);
			GetButtonSize ( fhMenu, BUTNO_LOW_ATTACK, &width, &height );
		
			if ((pAvatar->Status == CAvatar::AI_ATTACK ||
			     (pAvatar->Status == CAvatar::AI_PAUSED &&
			      pAvatar->fPrev.Status == CAvatar::AI_ATTACK)) &&
			     pAvatar->mfGetAttackMode() == FIGHT_SEQUENCE::ATM_LOW)
			{
				hBitm = GetResourceStd ("ui\\CMBATK2B.PCX", FALSE);
			}
			else
			{
				hBitm = GetResourceStd ("ui\\CMBATK2A.PCX", FALSE);
			}
		
			if (hBitm != fERROR)
			{
				ClearRemapTable();
				DrawBitmap ((SHORT)x, (SHORT)y, hBitm, 0, 0, (SHORT)width, (SHORT)height);
				SetPurge(hBitm);
			}
			
			GetButtonPosition(fhMenu, BUTNO_QLOW_ATTACK, &x, &y);
			GetButtonSize ( fhMenu, BUTNO_QLOW_ATTACK, &width, &height );
		
			if ((pAvatar->Status == CAvatar::AI_ATTACK ||
			     (pAvatar->Status == CAvatar::AI_PAUSED &&
			      pAvatar->fPrev.Status == CAvatar::AI_ATTACK)) &&
				pAvatar->mfGetAttackMode() == FIGHT_SEQUENCE::ATM_QUICK_LOW)
			{
				hBitm = GetResourceStd ("ui\\CMBATK4B.PCX", FALSE);
			}
			else
			{
				hBitm = GetResourceStd ("ui\\CMBATK4A.PCX", FALSE);
			}
		
			if (hBitm != fERROR)
			{
				ClearRemapTable();
				DrawBitmap ((SHORT)x, (SHORT)y, hBitm, 0, 0, (SHORT)width, (SHORT)height);
				SetPurge(hBitm);
			}
		}
		
		// Now paint Defend or Fallback key.
		GetButtonPosition(fhMenu, BUTNO_DEF_FALL_BTN, &x, &y);
		GetButtonSize ( fhMenu, BUTNO_DEF_FALL_BTN, &width, &height );
		
		if (COMBAT_MODE::mfIsModeRPG() == TRUE)
		{
			if (pAvatar->Status == CAvatar::AI_FALLBACK ||
			    (pAvatar->Status == CAvatar::AI_PAUSED &&
			     pAvatar->fPrev.Status == CAvatar::AI_FALLBACK))
			{
				hBitm = GetResourceStd ("ui\\CMBFALBB.PCX", FALSE);
			}
			else
			{
				hBitm = GetResourceStd ("ui\\CMBFALBA.PCX", FALSE);
			}
		}
		else
		{
			if (pAvatar->Status == CAvatar::AI_DEFEND ||
			    (pAvatar->Status == CAvatar::AI_PAUSED &&
			     pAvatar->fPrev.Status == CAvatar::AI_DEFEND))
			{
				hBitm = GetResourceStd ("ui\\CMBDFNDB.PCX", FALSE);
			}
			else
			{
				hBitm = GetResourceStd ("ui\\CMBDFNDA.PCX", FALSE);
			}
		}
		
		if (hBitm != fERROR)
		{
			ClearRemapTable();
			DrawBitmap ((SHORT)x, (SHORT)y, hBitm, 0, 0, (SHORT)width, (SHORT)height);
			SetPurge(hBitm);
		}
		
		
		switch (pAvatar->Status)
		{
		case CAvatar::AI_ATTACK:
			if (pAvatar->hEnemy != fERROR)
			{
				LONG ToHit = pAvatar->mfToHitEnemy(pAvatar->hEnemy);
				if (mundane_descriptions)
				{
					if (ToHit > 20)
					{
						ToHit = 20;
					}
					else if (ToHit < 2)
					{
						ToHit = 2;
					}
					ToHit = (21 - ToHit) * 5;
					strncpy(cpFormatBuffer, STRMGR_GetStr(COMBAT_STR_HIT_CHANCE), sizeof(cpFormatBuffer));
				}
				else
				{
					strncpy(cpFormatBuffer, STRMGR_GetStr(COMBAT_STR_RPG_HIT_CHANCE), sizeof(cpFormatBuffer));
				}
				
				if (pAvatar->mfGetAttackMode() == FIGHT_SEQUENCE::ATM_QUICK_HIGH ||
					pAvatar->mfGetAttackMode() == FIGHT_SEQUENCE::ATM_QUICK_LOW)
					
				{
					sprintf(cpBuffer, cpFormatBuffer, STRMGR_GetStr(COMBAT_STR_QUICK),ToHit);
				}
				else
				{
					sprintf(cpBuffer, cpFormatBuffer, STRMGR_GetStr(COMBAT_STR_SLOW),ToHit);
				}
			}
			else
			{
				strncpy(cpNameBuffer,dhPlayerStats->mfGetName(), sizeof(cpNameBuffer));
				sprintf(cpBuffer, STRMGR_GetStr(COMBAT_STR_HIT_PTS), 
							cpNameBuffer,
							dhPlayerStats->mfGetArmorClass(),
							dhPlayerStats->mfGetCurHitPoints() );
			}
			break;
		default:
			strncpy(cpNameBuffer,dhPlayerStats->mfGetName(), sizeof(cpNameBuffer));
			sprintf(cpBuffer, STRMGR_GetStr(COMBAT_STR_HIT_PTS), 
							cpNameBuffer,
							dhPlayerStats->mfGetArmorClass(),
							dhPlayerStats->mfGetCurHitPoints() );
			break;
		}
		GetButtonPosition(fhMenu, BUTNO_AVATAR_STATS, &x, &y);
		init_gfont(FONT_SANS_12PT);
		gprint_text( x, y, cpBuffer, WHITE);
		cpBuffer[0] = 0;
		
		switch (pAvatar->Status)
		{
		case CAvatar::AI_DEAD:
			strncpy(cpNameBuffer,dhPlayerStats->mfGetName(), sizeof(cpNameBuffer));
			sprintf(cpBuffer, STRMGR_GetStr(COMBAT_STR_IS_DEAD), cpNameBuffer);
			break;
		case CAvatar::AI_STONE:
			strncpy(cpNameBuffer,dhPlayerStats->mfGetName(), sizeof(cpNameBuffer));
			sprintf(cpBuffer, STRMGR_GetStr(COMBAT_STR_IS_STONE), cpNameBuffer);
			break;
		case CAvatar::AI_DEFEND:
			if (pAvatar->hEnemy != fERROR)
			{
				CAvatar const * const pEnemy = (CAvatar const * const ) BLKPTR(pAvatar->hEnemy);
				if (pEnemy->hPlayerStats != fERROR)
				{
					DumbHandlePtr <PLAYER_STATS const> const dhEnemyStats(pEnemy->hPlayerStats);
					strcpy( cpFormatBuffer, STRMGR_GetStr(COMBAT_STR_DEFENDING));
					sprintf(cpBuffer, cpFormatBuffer, 
										dhEnemyStats->mfGetName(),
										(SHORT)dhEnemyStats->mfGetArmorClass() );
				}
			}
			break;
		case CAvatar::AI_ATTACK:
			if (pAvatar->hEnemy != fERROR)
			{
				CAvatar const * const pEnemy = (CAvatar const * const) BLKPTR(pAvatar->hEnemy);
				if (pEnemy->hPlayerStats != fERROR)
				{
					DumbHandlePtr <PLAYER_STATS> const dhEnemyStats(pEnemy->hPlayerStats);
					// Damage calculations haven't occured yet
					LONG minDamage;
					LONG maxDamage;
					
					PLAYER_HIT const PlayerHitObj(dhPlayerStats, dhEnemyStats);
					PlayerHitObj.mfDamageRange(pEnemy->mfType(), &minDamage, &maxDamage);
					if (pAvatar->mfGetAttackMode() == FIGHT_SEQUENCE::ATM_QUICK_HIGH ||
						pAvatar->mfGetAttackMode() == FIGHT_SEQUENCE::ATM_QUICK_LOW)
					
					{
						maxDamage *= 2;
					}
					
					if (pAvatar->mfGetDamageDealt()< 0)
					{
						strncpy(cpFormatBuffer, STRMGR_GetStr(COMBAT_STR_ATTACKING),sizeof(cpFormatBuffer));
						sprintf(cpBuffer, cpFormatBuffer,
							        minDamage,
							        maxDamage);
					}
					else
					{
						if (pAvatar->mfGetDamageDealt() == 0)
						{
							// We missed.
								strncpy(cpFormatBuffer, STRMGR_GetStr(COMBAT_STR_SWING_MISS), sizeof(cpFormatBuffer));
								sprintf(cpBuffer, cpFormatBuffer,
											  minDamage,
											  maxDamage );
						}
						else
						{
							// We hit'em!
							strncpy(cpFormatBuffer, STRMGR_GetStr(COMBAT_STR_HIT_FOR), sizeof(cpFormatBuffer));
							sprintf(cpBuffer, cpFormatBuffer, 
			                                  minDamage,
			                                  maxDamage,
			                                  pAvatar->mfGetDamageDealt());
						}
					}
				}
				else
				{
					sprintf(cpBuffer, STRMGR_GetStr(COMBAT_STR_CANT_ENGAGE));
				}
			}
			else
			{
				sprintf(cpBuffer, STRMGR_GetStr(COMBAT_STR_CANT_ENGAGE));
			}
			break;
		case CAvatar::AI_FALLBACK:
			if (pAvatar->hEnemy != fERROR)
			{
				CAvatar const * const pEnemy = (CAvatar const * const) BLKPTR(pAvatar->hEnemy);
				if (pEnemy->hPlayerStats != fERROR)
				{
					DumbHandlePtr <PLAYER_STATS const> const dhEnemyStats(pEnemy->hPlayerStats);
					
					strncpy(cpFormatBuffer, STRMGR_GetStr(COMBAT_STR_FALLING_BACK), sizeof(cpFormatBuffer));
					sprintf(cpBuffer, cpFormatBuffer, 
									    dhEnemyStats->mfGetName(),
										(SHORT)dhEnemyStats->mfGetArmorClass() );
				}
			}
			break;
		case CAvatar::AI_MOVING:
			if (pAvatar->hEnemy != fERROR)
			{
				CAvatar *pEnemy = (CAvatar *) BLKPTR(pAvatar->hEnemy);
				if (pEnemy->hPlayerStats != fERROR)
				{
					DumbHandlePtr <PLAYER_STATS const> const dhEnemyStats(pEnemy->hPlayerStats);
					
					strncpy(cpFormatBuffer, STRMGR_GetStr(COMBAT_STR_CHASING), sizeof (cpFormatBuffer));
					sprintf(cpBuffer, cpFormatBuffer,
										dhEnemyStats->mfGetName(),
										(SHORT)dhEnemyStats->mfGetArmorClass() );
				}
			}
			break;
		case CAvatar::AI_CASTSPELL:
			{
			SHORT const sSpellBox = pAvatar->fFollowPlayer.fCurrentSpellBox;
			
			if (sSpellBox > -1)
			{
			    SHORT const sSpell = dhPlayerStats->Inventory.mfGetSpellBox(sSpellBox);
				if (sSpell > 0)
				{
					ITEMTYPE const SpellType = ObjectList::mfData(sSpell).mfType();
					
					strncpy(cpFormatBuffer, STRMGR_GetStr(COMBAT_STR_CASTING), sizeof (cpFormatBuffer));
					sprintf(cpBuffer, cpFormatBuffer,
						   ItemTypes[SpellType].mfGetName());
				}
			}
			else if (pAvatar->mfGetDamageDealt() > 0)
			{
				strncpy(cpFormatBuffer, STRMGR_GetStr(COMBAT_STR_14), sizeof (cpFormatBuffer));
				sprintf(cpBuffer, cpFormatBuffer, pAvatar->mfGetDamageDealt());
			}
			
			}
			break;
		default:
			sprintf(cpBuffer, STRMGR_GetStr(COMBAT_STR_AWAITING_CMD));
			break;
		}
			
		GetButtonPosition (fhMenu, BUTNO_AVATAR_INFO, &x, &y);
		init_gfont(FONT_SANS_12PT);
		gprint_text( x, y, cpBuffer, WHITE);
		cpBuffer[0] = 0;
		
		// WARNING!! This code relies on the spell boxes being in sequence.
		init_gfont(FONT_SANS_5PT);
		for (i=0;i<fMaxSpellBoxes;i++)
		{
			LONG SpellBoxX;
			LONG SpellBoxY;
			LONG SpellBoxW;
			LONG SpellBoxH;
			
			if(GetButtonPosition(fhMenu,BUTNO_SPELL1+i,&SpellBoxX,&SpellBoxY)==fERROR)
				continue;
			if(GetButtonSize(fhMenu,BUTNO_SPELL1+i,&SpellBoxW,&SpellBoxH)==fERROR)
				continue;
				
			InvenIndex const sSpell = dhPlayerStats->Inventory.mfGetSpellBox(i);
			
			if (sSpell >-1)
			{
				SHORT hBitmap;
				char cpBuffer[5];
				
				ITEMTYPE const SpellType = ObjectList::mfData(sSpell).mfType();
				if (pAvatar->fFollowPlayer.fCurrentSpellBox == i)
				{
					hBitmap = ItemTypes[SpellType].mfGetIconOn();
				}
				else
				{
					hBitmap = ItemTypes[SpellType].mfGetIconOff();
				}
				DrawBitmap((SHORT)SpellBoxX,
							(SHORT)SpellBoxY,
							hBitmap,
							0,0,
							(SHORT)SpellBoxW,
							(SHORT)SpellBoxH);
				SetPurge(hBitmap);
				
				// Draw the number on the button code here.
				if (fccActive[fsccIndex].fhMenu == fhMenu)
				{
					// Only draw the hot key numbers on the active menu.
					sprintf(cpBuffer, "%ld", i+1);
					gprint_text(SpellBoxX, SpellBoxY, cpBuffer, WHITE);
				}
				
			}
		}
		
	}
}

/* ========================================================================
   Function    - mfSetBeginCombat
   Description - Initialize the variables for begining the combat menu.
   Returns     - 
   ======================================================================== */
void CONTROL_PANELS::CURRENT_CONTROL::mfSetBeginCombat(BOOL const AutoAttack)
{
	if (fhAvatar != fERROR)
	{
		LONG i;
		CAvatar * const pAvatar = (CAvatar * const) BLKPTR(fhAvatar);
		
		fOriginalAngle = pAvatar->mfAngle();
		fOriginalStatus = pAvatar->Status;
		fOriginalLeftButtonFn = pAvatar->mfSetLeftButtonFn(CombatSelectThisAdventurer);
		fPrevStatus = fOriginalStatus;
		fPrevPauseStatus = pAvatar->fPrev.Status;
		fPrevDamage = pAvatar->mfGetDamageDealt();
		fPrevEngaged = pAvatar->Engaged;
		
		if (!pAvatar->mfAmIImmoblized())
		{
			if (pAvatar->hPlayerStats != fERROR)
			{
				PLAYER_STATS const * const pPlayerStats = (PLAYER_STATS const * const) BLKPTR(pAvatar->hPlayerStats);
				fPrevHitPts = pPlayerStats->mfGetCurHitPoints();
				
				for (i = 0; i < MAX_COMBAT_SPELL_BOXES; i++)
				{
					fPrevSpells[i] = pPlayerStats->Inventory.mfGetSpellBox(i);
				}
			}
			
			if (pAvatar->Status != CAvatar::AI_ATTACK &&
				pAvatar->Status != CAvatar::AI_CASTSPELL)
			{
				if (pAvatar->Engaged > 0)
				{
					// If you are being attacked, auto Defend.
					pAvatar->mfSetAttackMode(FIGHT_SEQUENCE::ATM_DEFEND);
				}
				else
				{
					if (pAvatar->hPlayerStats != fERROR)
					{
						PLAYER_STATS const * const pPlayerStats = (PLAYER_STATS const * const) BLKPTR(pAvatar->hPlayerStats);
						
						if (AutoAttack == TRUE &&
						    (pPlayerStats->mfIsType(PLAYER_CLASS_INFO::FIGHTER) ||
						     pPlayerStats->mfIsType(PLAYER_CLASS_INFO::PALADIN) ||
						     pPlayerStats->mfIsType(PLAYER_CLASS_INFO::RANGER) ))
						{
							// If you are not being attacked, help out.
							pAvatar->mfSetAttackMode(FIGHT_SEQUENCE::ATM_HIGH);
						}
						else
						{
							pAvatar->mfSetAttackMode(FIGHT_SEQUENCE::ATM_DEFEND);
						}
					}
				}
			}
		}
		else
		{
			for (i = 0; i < MAX_COMBAT_SPELL_BOXES; i++)
			{
				fPrevSpells[i] = -1;
			}
		}
	}
}
/* ========================================================================
   Function    - mfNeedToUpdatePanel
   Description - Test whether the avatar data has changed and we need to update the panel.
   Returns     - 
   ======================================================================== */
BOOL const CONTROL_PANELS::CURRENT_CONTROL::mfNeedToUpdatePanel()
{
	BOOL Result = FALSE;
	
	if (fhAvatar != fERROR)
	{
		CAvatar const * const pAvatar = (CAvatar *const) BLKPTR(fhAvatar);
		
		if (pAvatar->Status != fPrevStatus)
		{
			fPrevStatus = pAvatar->Status;
			Result = TRUE;
		}
		
		if (pAvatar->fPrev.Status != fPrevPauseStatus)
		{
			fPrevPauseStatus = pAvatar->fPrev.Status;
			Result = TRUE;
		}
		
		// Check if we need to update the current hit points as well
		// so the panel isn't painted twice.
		if (pAvatar->hPlayerStats != fERROR)
		{
			LONG i;
			
			PLAYER_STATS * const pPlayerStats = (PLAYER_STATS * const) BLKPTR(pAvatar->hPlayerStats);
			if (fPrevHitPts != pPlayerStats->mfGetCurHitPoints())
			{
				fPrevHitPts = pPlayerStats->mfGetCurHitPoints();
				Result = TRUE;
			}
			
			for (i = 0; i < fMaxSpellBoxes; i++)
			{
				SHORT const sSpell = pPlayerStats->Inventory.mfGetSpellBox(i);
				if ( fPrevSpells[i] != sSpell)
				{
					fPrevSpells[i] = sSpell;
					
					if (sSpell >-1)
					{
						ITEMTYPE const SpellType = ObjectList::mfData(sSpell).mfType();
						THINGTYPE SpellThing = ItemTypes[SpellType].mfGameType();
						ChangeButtonToolTip(fhMenu, BUTNO_SPELL1 + i, GAME_TTYPE::mfGetSTRMgrId(SpellThing));
					}
					else
					{
						ChangeButtonToolTip(fhMenu, BUTNO_SPELL1 + i, fERROR);
					}
					
					Result = TRUE;
				}
			}
		}
		
		if (pAvatar->mfGetDamageDealt() != fPrevDamage)
		{
			fPrevDamage = pAvatar->mfGetDamageDealt();
			Result = TRUE;
		}
		
		if (pAvatar->Engaged != fPrevEngaged)
		{
			fPrevEngaged = pAvatar->Engaged;
			Result = TRUE;
		}
		
		if (pAvatar->mfGetAttackMode() != fPrevAttackMode)
		{
			fPrevAttackMode = pAvatar->mfGetAttackMode();
			Result = TRUE;
		}
	}
	
	return Result;
}
/* ========================================================================
   Function    - mfMatchMenuToCurrent
   Description - Given a menuid, set the current value to that menu.
   Returns     - 
   ======================================================================== */
void CONTROL_PANELS::mfMatchMenuToCurrent(LONG const menuid)
{
	// a zero means this was called via a hot key instead of a button.
	if (menuid)
	{
		SHORT i = fsccIndex;
		LONG infiniteCounter = 0;
		
		do
		{
			if (fccActive[i].fhMenu == menuid)
			{
				mfSetNewCurrent(i);
				break;
			}
			
			i++;
			if (i >= MAX_NUMBER_CONTROLS)
			{
				i = 0;
			}
			infiniteCounter++;
		
		} while (infiniteCounter <= MAX_NUMBER_CONTROLS);
	}
}

/* ========================================================================
   Function    - mfinitAllPanels
   Description - Initialize all the panels for display.
   Returns     - 
   ======================================================================== */
void CONTROL_PANELS::mfInitAllPanels()
{
	ADVENTURER_TEAM_ITOR AdvItor;
	LONG const NumberOfFollowers = ADVENTURER::mfNumberOfAdventurers();
	LONG i;
	SHORT menuNumber;
	SHORT FighterMenuNumber;
	
	BOOL const SpeakFlag = (NumberOfFollowers == 1) ? FALSE : TRUE;
	// Relys on the combat menus having no intermediate menus between them.
	
	if (NumberOfFollowers > 2) 
	{
		menuNumber = D_COMBAT_CONTROL1;
		FighterMenuNumber = D_COMBAT_FIGHTER_CONTROL1;
	}
	else
	{
		menuNumber = D_COMBAT_CONTROL3, 
		FighterMenuNumber = D_COMBAT_FIGHTER_CONTROL3;
	}
	
	fCurrentlyRPG_Mode = COMBAT_MODE::mfIsModeRPG();
	
	for (i = 0, 
		 AdvItor = ADVENTURER::begin(); 
	     AdvItor != ADVENTURER::end() && i < MAX_NUMBER_CONTROLS; 
	     AdvItor++, i++, menuNumber++, FighterMenuNumber++)
	{
		CAvatar const * const pAvatar = (CAvatar const * const) BLKPTR(*AdvItor);
		
		if (GAME_TTYPE::mfHasAttack2Animation(pAvatar->mfType()))
		{
			fccActive[i].mfSetControl(FighterMenuNumber, 
									  *AdvItor, 
									  SpeakFlag, 
									  MAX_FIGHTER_COMBAT_SPELL_BOXES);
		}
		else
		{
			fccActive[i].mfSetControl(menuNumber, 
									  *AdvItor, 
									  SpeakFlag, 
									  MAX_COMBAT_SPELL_BOXES);
		}
	}
	
	// For the rest of the menus, initialize them to off.
	for (; i < MAX_NUMBER_CONTROLS; i++)
	{
		fccActive[i].mfSetControl(fERROR, fERROR, FALSE, 0);
	}
	mfSetNewCurrent(0);
}

/* ========================================================================
   Function    - mfCycleToNextPanel
   Description - Loop around to the next panel.
   Returns     - 
   ======================================================================== */
void CONTROL_PANELS::mfCycleToNextPanel()
{
	LONG InfiniteCounter = 0;
	LONG const NumberOfFollowers = ADVENTURER::mfNumberOfAdventurers();
	LONG i = fsccIndex;
	
	LONG const PrevIndex = fsccIndex;
	
	// find the next active menu.
	do
	{
		InfiniteCounter++;
		if (InfiniteCounter > NumberOfFollowers)
		{
			break;
		}
		
		i++;
		
		if (i >= NumberOfFollowers)
		{
			i = 0;
		}
		
		if (CONTROL_PANELS::mfCurrentInUse())
		{
			// Found the next one.
			mfSetNewCurrent(i);
			break;
		}
		
	} while (PrevIndex != i); // go until we've cycled around again.
}

/* ========================================================================
   Function    - mfUpdateOptions
   Description - When the options have changed this fn is called.
   Returns     - 
   ======================================================================== */

void CONTROL_PANELS::CURRENT_CONTROL::mfUpdateOptions()
{
	if (fhAvatar != fERROR)
	{
	}
}
/* ========================================================================
   Function    - mfUpdateOptions
   Description - When the options have changed this fn is called.
   Returns     - 
   ======================================================================== */
void CONTROL_PANELS::mfUpdateOptions()
{
	LONG i;
	
	if (fCurrentlyRPG_Mode != COMBAT_MODE::mfIsModeRPG())
	{
		// If you've switched modes I need to get rid of the defend buttons
		// and replace them with Fallback. This is the easiest way to do that.
		LONG const OldCurrent = fsccIndex;
		
		mfReleaseAll();
		mfInitAllPanels();
		mfSetNewCurrent(OldCurrent);
	}
	else
	{
		for (i = 0; i < MAX_NUMBER_CONTROLS; i++)
		{
			fccActive[i].mfUpdateOptions();
		}
	}
}		


/* ========================================================================
   Function    - mfSetNewCurrent
   Description - Cover fn to assign a new current avatar.
   Returns     - 
   ======================================================================== */

void CONTROL_PANELS::mfSetNewCurrent(SHORT const i)
{
	if (fccActive[fsccIndex].fhMenu > 0)
	{
		if (CONTROL_PANELS::CURRENT_CONTROL::mfIsFighterCombatPanel(fccActive[fsccIndex].fhMenu))
		{
			SetButtonArt(fccActive[fsccIndex].fhMenu, 0, COMBAT_FIGHTER_PANEL);
		}
		else
		{
			SetButtonArt(fccActive[fsccIndex].fhMenu, 0, COMBAT_REGULAR_PANEL);
		}
	}
	
	// reset the unselected avatar back to their regular color.
	if (fccActive[fsccIndex].fhAvatar != fERROR)
	{
		CAvatar * const pAvatar = (CAvatar * const) BLKPTR(fccActive[fsccIndex].fhAvatar);
		pAvatar->mfTurnHighlightOff();
		pAvatar->mfSetBeingControlled(FALSE);
		if (pAvatar->hEnemy != fERROR)
		{
			CAvatar * const pEnemy = (CAvatar * const) BLKPTR(pAvatar->hEnemy);
			// Unhighlight your previous targeted enemy.
			pEnemy->mfTurnHighlightOff();
		}
	}
	
	fsccIndex = i;
	if (fccActive[fsccIndex].fhMenu > 0)
	{
		if (CONTROL_PANELS::CURRENT_CONTROL::mfIsFighterCombatPanel(fccActive[fsccIndex].fhMenu))
		{
			SetButtonArt(fccActive[fsccIndex].fhMenu, 0, COMBAT_FIGHTER_HIGHLIGHT_PANEL);
		}
		else
		{
			SetButtonArt(fccActive[fsccIndex].fhMenu, 0, COMBAT_REGULAR_HIGHLIGHT_PANEL);
		}
	}
	
	
	if (fccActive[fsccIndex].fhAvatar != fERROR)
	{
		ADVENTURER::mfSetWhoGetsObjectsPickedUp(fccActive[fsccIndex].fhAvatar);
		CAvatar * const pAvatar = (CAvatar * const) BLKPTR(fccActive[fsccIndex].fhAvatar);
		pAvatar->mfSetHighlightAvatar();
		pAvatar->mfSetBeingControlled(TRUE);
		
		if (pAvatar->hEnemy != fERROR)
		{
			CAvatar * const pEnemy = (CAvatar * const) BLKPTR(pAvatar->hEnemy);
			// Highlight your targeted enemy.
			pEnemy->mfSetHighlightEnemy();
		}
	}
}

/* ========================================================================
   Function    - mfSwapPanels
   Description - Exchange places for two avatars
   Returns     - 
   ======================================================================== */

void CONTROL_PANELS::CURRENT_CONTROL::mfSwapPanels(CURRENT_CONTROL &OtherPanel)
{
	SHORT const ThisPanelId = fhMenu;
	CURRENT_CONTROL const temp = *this;
	
	// Class copy.
	*this = OtherPanel;
	OtherPanel = temp;
	
	// Get rid of the current menus.
	HidePanel(fhMenu);
	HidePanel(OtherPanel.fhMenu);
	
	if (CONTROL_PANELS::CURRENT_CONTROL::mfNeedToConvert(OtherPanel.fhMenu, fhMenu))
	{
		// Switch from fighter to regular combat panels.
		OtherPanel.fhMenu = CONTROL_PANELS::CURRENT_CONTROL::mfConvertPanel(fhMenu);
		fhMenu = CONTROL_PANELS::CURRENT_CONTROL::mfConvertPanel(ThisPanelId);
	}
	else
	{
		// Put back the menu id's.
		OtherPanel.fhMenu = fhMenu;
		fhMenu = ThisPanelId;
	}
	
	// Put up the new menus.
	mfSetControl(fhMenu, 
	             fhAvatar, 
	             fSpeakFlag,
	             fMaxSpellBoxes);
	OtherPanel.mfSetControl(OtherPanel.fhMenu, 
							OtherPanel.fhAvatar, 
							OtherPanel.fSpeakFlag,
							OtherPanel.fMaxSpellBoxes);
}

/* ========================================================================
   Function    - mfSetBeginCombatAll
   Description - Initialize the combat control for the adventure team.
   Returns     - 
   ======================================================================== */

void CONTROL_PANELS::mfSetBeginCombatAll() 
{
	BOOL AutoAttack = TRUE;
	LONG i;
	
	// If anyone is casting a spell, don't walk in front and get in the way.
	for (i = 0; i < MAX_NUMBER_CONTROLS; i++)
	{
		if (fccActive[i].fhAvatar != fERROR)
		{
			CAvatar const * const pAvatar = (CAvatar const * const) BLKPTR(fccActive[i].fhAvatar);
			// GWP Gad what a hack to see if we are casting a spell or not!
			if (pAvatar->fFollowPlayer.fCurrentSpellBox != -1)
			{
				AutoAttack = FALSE;
				break;
			}
		}
	}
	
	for (i = 0; i < MAX_NUMBER_CONTROLS; i++)
	{
		fccActive[i].mfSetBeginCombat(AutoAttack);
	}
	
	// Now setup the target'able avatars.
	if (SCENE_MGR::hCurrentScene != fERROR)
	{
		LONG i;
		
		DumbAutoLockPtr<SCENE> pScene(SCENE_MGR::hCurrentScene);
		
		for (i = 0; i < MAX_AVATARS; i++)
		{
			if (pScene->Avatars[i] != fERROR)
			{
				CAvatar *pAvatar = (CAvatar *) BLKPTR(pScene->Avatars[i]);
				if (pAvatar->mfAmIAlive() &&
					!pAvatar->mfIsLeftButtonFnSet() &&
					pAvatar->GetAIFuncIndex() != CAvatar::AI_FUNC_FIREBALL )
				{
					pAvatar->mfSetLeftButtonFn(CombatSelectThisEnemy);
				}
			}
		}
	}
}

/* ========================================================================
   Function    - mfReleaseAll
   Description - Unconnect the avatars from the control panel.
   Returns     - 
   ======================================================================== */

void CONTROL_PANELS::mfReleaseAll() 
{
	for (LONG i = 0; i < MAX_NUMBER_CONTROLS; i++)
	{
		fccActive[i].mfRelease();
	}
	
	mfSetNewCurrent(0);
	
	// Now disconnect the target'able avatars.
	if (SCENE_MGR::hCurrentScene != fERROR)
	{
		LONG i;
		
		DumbAutoLockPtr<SCENE> pScene(SCENE_MGR::hCurrentScene);
		
		for (i = 0; i < MAX_AVATARS; i++)
		{
			if (pScene->Avatars[i] != fERROR)
			{
				CAvatar *pAvatar = (CAvatar *) BLKPTR(pScene->Avatars[i]);
				pAvatar->mfSetLeftButtonFn(0);
			}
		}
	}
}

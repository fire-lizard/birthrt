/* ========================================================================
   Copyright (c) 1990,1996	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: DomText.cpp  -
   Author:   Alan Clark

   Contains the following general functions:

   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include "system.h"
#include "actions.hxx"
#include "strenum.h"
#include "context.hxx"
/* -----------------------------------------------------------------------
   Defines and Compile Flags
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Macros
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Prototypes
   ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------
   Global Variables
   ------------------------------------------------------------------------ */
// Adventure
//---------------------------------------------
#if 0

// these have been moved to strdat.txt

#define ACTION_TEXT0	\
"^W310Regents don't spend all of their time ruling their domain.  There are monsters to be fought, intrigues to be solved, castles to be stormed; in short, regents participate in the same activities as normal adventurers. If a PC regent wants to see what's over the next hill, he can ride off in search of fame and fortune.\n \n\
Adventures may reflect the activities of other regents or provocative random events.  An adventure uses one of the character's actions for the turn."

// Agitate
#define ACTION_TEXT1	\
"^W310A regent with a holding can use his influence to agitate or stabilize the province's loyalty toward its ruler.  A ruler can agitate in favor of himself.  A priest regent can do this once per domain turn as a free action.\n \n\
The success number is increased by the difference between the active regent's holding and the province rating (it's easier to agitate when you have a substantial influence in a region).  If the ruler supports the effort, the ruler's law holding is subtracted from the success number; if the ruler is opposed, add the law holding's number to the success number.  The success number also can be modified by spending Gold Bars, at a rate of one per point."

// Contest
#define ACTION_TEXT2	\
"^W310A regent can neutralize another regent's holding by contesting his influence.  If the attempt is successful, the contested holding does not generate Regency Points or Gold Bars for its owner.  A level 0 holding is destroyed by being contested.  The success number is modified by the difference between the attacker's holding (or province rating, if he's the ruler) and the contested holding.\n \n\
The holding remains contested until the defender succeeds in a rule action.\n \n\
If a contested holding is contested a second time, its owner loses the holding and its slots become uncontrolled."

// Create Holding
#define ACTION_TEXT3	\
"^W310A regent wishing to establish a holding in a province where he has no holdings may use create holding. This gives the regent a holding (0). He is then free to rule in order to increase the influence of his holding.\n \n\
Creation of a holding can be opposed or supported by any regent with a similar holding in the province, or by the province ruler. The interfering regent's holding or province rating is added to or subtracted from the success number without spending Regency Points. Any concerned party can spend Regency Points to influence the success roll.\n \n\
NOT AVAILABLE when there are so many holdings in Anuire that masons cannot be found."

// Declare War
#define ACTION_TEXT4	\
"^W310Any regent can declare war.  A regent must declare war before he can move military units into provinces that don't belong to him; if he doesn't declare war, he can make troop movements only within his own realm.\n \n\
Since no troop movement is allowed after the third realm action, you may only declare war as your first or second realm action.\n \n\
A declaration of war only lasts until the end of a domain turn, after which the parties return to neutral status.\n \n\
NOT AVAILABLE as a lieutenant action."

// Diplomacy
#define ACTION_TEXT5	\
"^W310Negotiations with other domains fall under the diplomacy action. Regents are assumed to maintain basic contact with their neighbors and pass routine messages back and forth. However, diplomacy as a domain action is a full-court affair designed to achieve a specific goal. You must, therefore, have a court to conduct diplomacy.\n \n\
Diplomacy may have the following effects:\n\
^t020Create or break an alliance.\n\
^t020Force concessions such as tribute or territory\n\
^t030from another regent.\n\
^t020Request gold or artifacts.\n \n\
Alliances, tribute, and vassalage, unlike declarations of war, are durable and remain in effect from turn to turn unless broken by another action."

// Espionage
#define ACTION_TEXT6	\
"^W310Espionage includes any kind of spying or covert actions.  A thief regent may perform this once per domain turn as a free action.\n \n\
Espionage can reveal troop strengths and hidden troops, or assassinate a character or destroy a unit.\n \n\
The success number of an espionage attempt is a base of 20 improved by the level of the target province and the level of any guild holdings there controlled by the spying regent."

// Forge Ley Line
#define ACTION_TEXT7	\
"^W310Wizards may use this action to create a magical link between two provinces.  Many realm spells require a minimum source level in the province to be affected, but a ley line acts as a magical conduit, allowing the wizard to treat the weaker province as if it had the source level of a higher connected province for purposes of casting realm spells.\n \n\
Ley lines cost 1 RP and 1 GB per province crossed; thus linking to a province three moves away costs 3 RP and 3 GB.\n \n\
NOT AVAILABLE to non-wizards.\n \n\
In order to create a ley line you must have two sources that do not already have a ley line between them."

// Fortify
#define ACTION_TEXT8	\
"^W310Building strongholds in a domain is a good way to deter attack and tie up enemy forces in a war.\n \n\
Fortification of a province creates a new holding for that area: a castle.  Before a province can be taken, the castle must be destroyed by siege.\n \n\
Castles cost 8 GB per level.  Work progresses at 1d6 GB per domain turn, but the regent may pay 10 GB per turn for work to progress at 2d6 GB per turn.  A castle can accommodate a garrison of one military unit per level."

// Investiture
#define ACTION_TEXT9	\
"^W310The ceremony of investiture allows a regent to gain control of part of another regent's domain.  In order to invest a province, the province must have been conquered.  The conquering regent must then succeed in an investiture action.  The base success number is 10, and the conqueror must pay Regency Points equal to the level of the province.  He may pay additional Regency Points to improve his success chance.  The defender can bid Regency Points to oppose the investiture.\n \n\
NOT AVAILABLE as a lieutenant action."

// Lieutenant
#define ACTION_TEXT10	\
"^W310Normally, a hireling becomes a henchman only when he demonstrates loyalty and respect for a particular player.  Regents are different; they're besieged by people who want to work for them.  By spending a character action, the regent finds a lieutenant and adds him to his list of henchmen.\n \n\
Lieutenants are excellent stand-ins when a regent is busy and can provide extra muscle for the regent's adventuring party.  The regent's Charisma score dictates the maximum number of henchmen who can work for him.\n \n\
For this purpose, a scion of a minor bloodline can add 1 to his effective Charisma score; a scion of a major bloodline may add 2; a scion of a great bloodline may add 3.\n \n\
NOT AVAILABLE as a lieutenant action."

// Lieutenant Action
#define ACTION_TEXT11	\
"^W310One of a regent's lieutenants can conduct one lieutenant action per domain round.  This action is in additon to the regent's three realm turns.  The actions available for lieutenants are: adventure, agitate, contest, create holding, diplomacy, espionage, fortify, realm spell (if a wizard or priest), rule, or trade route.\n \n\
A lieutenant cannot spend additional Regency Points to improve the roll on his action, so his chance of success is less than if the regent had performed the action himself.  Normal costs in Gold Bars and Regency Points are spent.\n \n\
NOT AVAILABLE as a lieutenant action."


// Muster Armies
#define ACTION_TEXT12   \
"^W310 A regent can create new army units by mustering troops, raising a \
levy, or hiring mercenaries in a province he rules. A province can raise \
non-mercenary military units equal to its level (never higher) in a single \
domain turn.\n \n\
NOT AVAILABLE if all available able-bodied fighters have already been recruited."
#if 0
 Some of the available troops to muster and their costs are:\n \n\
^F00\n\
^t135 MUSTER^t185  MAINT.^t228PROV. LEVEL\n\
^UUNIT TYPE          ^t135COST(GB)^t185COST (GB)^t228  REQUIRED ^U\n \n\
ARCHERS^t1502^t2001^t2502\n\
CAVALRY^t1504^t2002^t2503\n\
INFANTRY^t1502^t2001^t2501\n\
ELITE INFANTRY^t1504^t2002^t2503\n\
IRREGULARS^t1501^t2001^t2501\n\
KNIGHTS^t1506^t2002^t2504\n\
PIKEMEN^t1502^t2001^t2502\n\
SCOUTS^t1502^t2001^t2500\n\
MERC. CAVALRY^t1506^t2002^t250-\n\
MERC. INFANTRY^t1504^t2002^t250-\n\
MERC. IRREGULARS^t1503^t2001^t250-\n \n\
^USPECIAL UNITS           ^U\n\
DWARF GUARDS^t1504^t2002^t2504\n\
DWARF CROSSBOWS^t1504^t2002^t2504\n\
ELF ARCHERS^t1504^t2001^t2503\n\
ELF CAVALRY^t1508^t2002^t2504\n\
GOBLIN ARCHERS^t1501^t2001^t2502\n\
GOBLIN INFANTRY^t1501^t2001^t2502\n\
GOBLIN CAVALRY^t1504^t2002^t2503\n\
GNOLL IRREGULARS^t1502^t2001^t250-\n\
GNOLL INFANTRY^t1503^t2001^t250-"
#endif

// Realm Spell
#define ACTION_TEXT13	\
"^W310A priest or wizard character can spend a domain action to cast a mighty enchantment that affects the entire province.  Realm spells are a special type of magic that are available only to Blooded spellcasters.  Without the power provided by an ancient bloodline and a tie to a temple or source holding, a realm spell would fail completely.  A realm spell can be used only in a province where a priest has a temple or a wizard has a source.\n \n\
NOT AVAILABLE to non-priest/non-wizard characters."

// Rule
#define ACTION_TEXT14	\
"^W310With this action, a regent spends time and energy advancing the causes of his domain.  By ruling, he can increase a province or holding's level by 1.  A province can be ruled only once per domain turn. Note that for a regent to increase a holding, a level must be vacant.\n \n\
Ruling a holding costs 1 Gold Bar plus Regency Points equal to the target level.  The province ruler and any regent with a similar holding in the province can support or oppose the action by adding or subtracting the level of their own holdings.\n \n\
Ruling a province costs 1 Gold Bar and 1 Regency Point per target level.\n \n\
If someone has contested a holding, a successful rule action will return the holding to an uncontested status."

// Trade Route
#define ACTION_TEXT15	\
"^W310A regent who controls a guild holding can attempt to open a trade route, thereby increasing the money-making power of his domain.  A trade route must be established between two provinces of different terrain types.  Overland trade routes can't be created unless a road links the largest towns or centerpoints of the two provinces involved.  Furthermore, provinces of level 1 to 3 are limited to one trade route; level 4 to 6 may have no more than two trade routes; level 7 and above can support three.\n \n\
Each domain turn, a trade route generates GB's equal to the average of the two provinces it links.  In addition, thief regents who control trade routes collect RP equal to the extra money generated by trade route."

// Build Road
#define ACTION_TEXT16	\
"^W310Roads are required for overland trade routes, and also speed troop movement. A road can be crucial in mountainous or heavily wooded areas. A road costs the sum of the Terrain Movement Costs of the two provinces the road goes between; thus, building a road from a hilly province (2GB) to a province in the plains (1GB) would cost 3 Gold Bars.\n \n\
A road will go from province center to province center and will be considered to provide a link between any holdings, towns, or castles in those two provinces.\n \n\
Roads can be built into other regent's provinces that are adjacent to your realm if you have an alliance with that regent.\n \n\
NOT AVAILABLE as a lieutenant action."

// Hold Action
#define ACTION_TEXT17	\
"^W310A regent always has the option to delay his action until the end of the current action round.  The advantage of waiting is simple: the regent can react to anything that occurs.  If war is declared on the regent or a random event comes along, the regent can address it immediately.  However, the held action must be used to respond to some provocation or situation.  If no one provokes the regent, he loses his action.  If the regent doesn't use his held action at the end of that domain turn, it is forfeited.  Additional uses of Hold Action act as a Pass.  That action round is simply forfeited.\n \n\
NOT AVAILABLE as a lieutenant action."

//Taxation
#define ACTION_TEXT18 \
"^W310Ruling a domain is expensive.  Fortunately, regents can collect money from their subjects.  Rulers levy taxes, guilds generate income, and priests collect offerings.  Regents may have great responsibilities, but with care, they can muster the resources to meet them.  In this step of the domain turn, all active regents collect money from their domains.  Because of the potentially large sums involved, it's easiest to work with blocks of money called Gold Bars, or GB.  (Don't confuse these with gp, or gold pieces.)  One Gold Bar is worth about 2,000 gp.  First, province rulers can declare taxation. The regent decides for each province whether to impose light, moderate, or severe taxation, then rolls the dice for each province.  Of course, severe taxation doesn't win the regent much support among his subjects, but light taxes generally make a regent more popular."

// Domain Maintenance
#define ACTION_TEXT19 \
"^W310The strength of a kingdom is measured not only by the power of its ruler \
and armies, but also by its economic health.  Many regents have ruined their \
lands through careless fiscal policies.  Each domain turn, the regent must cover \
the expenses of his government.  Each castle, fortified holding, and occupied \
province (a foreign province held by the regent's troops) costs 1 GB; if the \
regent fails to pay, the castle or fortification's level drops by one or the \
province rebels.  The regent must also pay a maintenance cost for his \
provinces and holdings.  The computer determines the correct maintenance for \
you by counting the number of provinces and law, guild, and temple (but not \
source) holdings in your domain.\n \n\
If the regent can't meet expenses, he must disband army units or reduce court \
costs to meet his kingdom's demands."

#if 0
  Don't total all the levels; just add up the number of holdings and provinces and refer to this table of costs:\n \n\
^t401 - 2^t1500 Gold Bars\n\
^t403 - 6^t1501 Gold Bars\n\
^t407 - 12^t1502 Gold Bars\n\
^t4013 - 18^t1503 Gold Bars\n\
^t4019 - 24^t1504 Gold Bars\n\
^t4025 - 30^t1505 Gold Bars\n\
^t4031 - 40^t1507 Gold Bars\n\
^t4041 - 50^t15010 Gold Bars\n\
^t4051 - 75^t15020 Gold Bars\n\
^t4076 - 100^t15030 Gold Bars\n\
^t40101+*^t1501 Gold Bar per 3 provinces/holdings\n \n\
* not cumulative with previous line; 102 provinces/holdings cost 34 GB\n \n\"
#endif

// Payroll
#define ACTION_TEXT20 \
"^W310Regents must pay the maintenance costs of any military units under their command.  If a unit goes unpaid, it disbands unless the regent spends 5 Regency Points for each GB he owes it.  If mercenaries are unpaid, they desert and become brigands, who will then raid your holdings until driven off by your armies."

// Court value
#define ACTION_TEXT21 \
"^W310Regents don't live like paupers.  Each active regent must spend money to hold court and support his staff each domain turn.  The regent decides how much he wants to spend, but, generally, the more he spends, the more opulent and impressive his court is."

// Dice Roll
#define ACTION_TEXT22 \
"^W310Roll the dice to determine the actual taxes collected from your provinces and holdings. If you end up with a negative treasury, you must adjust your finances until you have a positive balance. You can try lowering your court expenditures (you will do worse in diplomacy), not maintaining some of your holdings (they go down in level), or not paying your troops (they will disband - mercenaries will become bandits). But, you must balance the books. Such is the burden of regency."

// Realm Actions
#define ACTION_TEXT23 \
"^W310Once you are done with the realm maintenance, click here to move on to domain actions.  Now that you have some money and regency to work with, it is time to spend your resources on your plans for ruling your kingdom and interacting with the other kingdoms and regents of Anuire."

// Occupation
#define ACTION_TEXT24 \
"^W310A regent may need to make difficult choices for the good of his realm.  Occupying a province with troops, whether one of your own province or that of a foreign foe can cause hardship for the civilians.\n \nYou may choose to declare martial law and occupy your own provinces so that you may remove undesirable holdings from your realm.  Likewise, newly occupied foreign provinces can be purged of foreign influences.  This measure, while seemingly desirable will cause friction with you neighbors.\n \nLess beneficient rulers may choose to pillage occupied provinces, either to weaken an enemy, to reduce the value of an undefendable province, or just to raise needed gold."
#endif


DEFINE_VECTOR_DATA(ACTIONS const,Action) = {
ACTIONS(STR_NULL,0,-1),
ACTIONS(STR_ACTION_ADVENTURE			,ACTION_TEXT0 , H_Adventure_Button),
ACTIONS(STR_ACTION_AGITATE				,ACTION_TEXT1 , H_Agitate_Button),
ACTIONS(STR_ACTION_BUILD_RD				,ACTION_TEXT16, H_Build_Road_Button),
ACTIONS(STR_ACTION_CONTEST				,ACTION_TEXT2 , H_Contest_Button),
ACTIONS(STR_ACTION_CREATE_HOLDING	   	,ACTION_TEXT3 , H_Create_Holding_Button),
ACTIONS(STR_ACTION_DECLARE_WAR			,ACTION_TEXT4 , H_Declare_War_Button ),
ACTIONS(STR_ACTION_DIPLOMACY			,ACTION_TEXT5 , H_Diplomacy_Button),
ACTIONS(STR_ACTION_ESPIONAGE			,ACTION_TEXT6 , H_Espionage_Button ),
ACTIONS(STR_ACTION_FORGE_LEY_LINE		,ACTION_TEXT7 , H_Forge_Ley_Line_Button ),
ACTIONS(STR_ACTION_FORTIFY				,ACTION_TEXT8 , H_Fortify_Button),
ACTIONS(STR_ACTION_HOLD_ACTION			,ACTION_TEXT17, H_Hold_Action_Button ),
ACTIONS(STR_ACTION_INVESTITURE			,ACTION_TEXT9 , H_Investiture_Button),
ACTIONS(STR_ACTION_LIEUTENANT			,ACTION_TEXT10, H_Lieutenant_Button ),
ACTIONS(STR_ACTION_LIEUTENANT_ACTION	,ACTION_TEXT11, H_Lieutenant_Action_Button ),
ACTIONS(STR_ACTION_MUSTER_ARMIES		,ACTION_TEXT12, H_Muster_Button ),
ACTIONS(STR_ACTION_REALM_SPELL			,ACTION_TEXT13, H_Realm_Spell_Button ),
ACTIONS(STR_ACTION_RULE					,ACTION_TEXT14, H_Rule_Button),
ACTIONS(STR_ACTION_TRADE_ROUTE			,ACTION_TEXT15, H_Trade_Routes),
ACTIONS(STR_ACTION_TAXATION				,ACTION_TEXT18, H_Taxation),
ACTIONS(STR_ACTION_DOMAIN_MAINT			,ACTION_TEXT19, H_Maintenance),
ACTIONS(STR_ACTION_PAYROLL				,ACTION_TEXT20, H_Payroll),
ACTIONS(STR_ACTION_COURT_VALUE			,ACTION_TEXT21, -1),
ACTIONS(STR_ACTION_DICE_ROLL			,ACTION_TEXT22,  H_Taxation_Roll),
ACTIONS(STR_ACTION_REALM_ACTIONS		,ACTION_TEXT23,  H_Domain_Actions),
ACTIONS(STR_ACTION_OCCUPATION			,ACTION_TEXT24, H_Occupation),
};
DEFINE_VECTOR_CLASS(ACTIONS const, Action);

/* ======================================================================== */



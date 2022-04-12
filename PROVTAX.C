/* =®RM250¯=======================================================================
   Copyright (c) 1990,1995	Synergistic Software
   All Rights Reserved
   ========================================================================
   Filename: provtax.c  - Province Tax tables and access fns.
   Author:   Gary Powell
   
   ========================================================================
   
   Contains the following general functions:

   
   ======================================================================== */

/* ------------------------------------------------------------------------
   Includes
   ------------------------------------------------------------------------ */
#include "PROVTAX.H"
#include "DICE.H"

/* ------------------------------------------------------------------------
   Notes
   ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------
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

typedef struct _PROVINCE_TAX_RATE {
	DICE	TaxRateDice[3];
} PROVINCE_TAX_RATE, *PTR_PROVINCE_TAX_RATE;

// This table came from the Birthright Player Handbook, p. 43 (Table 17)
static PROVINCE_TAX_RATE ProvinceTaxRateTable[] = {
	{{{0,  0,  0}, {0,  0,  0}, {1,  3, -2}}},	// Province Level 0
	{{{1,  3, -2}, {1,  3, -1}, {1,  3,  0}}},	//  1
	{{{1,  3, -1}, {1,  3,  0}, {1,  4,  0}}},	//  2
	{{{1,  3,  0}, {1,  4,  0}, {1,  4,  1}}},	//  3
	{{{1,  4,  0}, {1,  4,  1}, {1,  6,  1}}},	//  4
	{{{1,  4,  1}, {1,  6,  1}, {1,  8,  1}}},	//  5
	{{{1,  6,  1}, {1,  8,  1}, {1, 10,  1}}},	//  6
	{{{1,  8,  1}, {1, 10,  1}, {1, 12,  1}}},	//  7
	{{{1, 10,  1}, {1, 12,  1}, {2,  8,  0}}},	//  8
	{{{1, 12,  1}, {2,  8,  0}, {2,  8,  2}}},	//  9
	{{{2,  8,  0}, {2,  8,  2}, {2, 10,  2}}}	// 10
};

#define HOLDINGS_MAX_TAX	 5
typedef struct _GUILD_TAX_RATE {
	DICE	TaxRateDice[HOLDINGS_MAX_TAX];
} GUILD_TAX_RATE, *PTR_GUILD_TAX_RATE;

// This table came from the Birthright rulebook, p. 43 (Table 18)
static GUILD_TAX_RATE GuildTaxRateTable[] = {
// Holding level 1,    2        3          4 & 5         6+
//	{{{0,  0,  0}, {0,  0,  0}, {0,  0, 0}, {0,  0, 0}, {0,  0, 0}}},	// Province Level 0
	{{{1,  3, -2}, {0,  0,  0}, {0,  0, 0}, {0,  0, 0}, {0,  0, 0}}},	// 1
	{{{1,  2, -1}, {1,  2,  0}, {0,  0, 0}, {0,  0, 0}, {0,  0, 0}}},	// 2
	{{{1,  2, -1}, {1,  3,  0}, {1,  4, 0}, {0,  0, 0}, {0,  0, 0}}},	// 3
	{{{1,  2,  0}, {1,  3,  0}, {1,  4, 0}, {1,  6, 0}, {0,  0, 0}}},	// 4
	{{{1,  2,  0}, {1,  3,  0}, {1,  4, 0}, {1,  6, 0}, {0,  0, 0}}},	// 5
	{{{1,  2,  0}, {1,  3,  0}, {2,  2, 0}, {2,  3, 0}, {2,  4, 1}}},	// 6
	{{{1,  3,  0}, {1,  2,  1}, {1,  4, 1}, {1,  6, 1}, {2,  4, 2}}}	// 7+
};

/* =======================================================================
   Function    - RollProvinceTaxes
   Description - Roll for the amount to be collected from a Province
   Returns     - void
   ======================================================================== */
SHORT RollProvinceTaxes (
	SHORT sProvinceRating,				 // civlevel
	SHORT TaxRate,							 // taxlevel
	SHORT flag)	  //if flag==1, province is blessed, if flag == -1, province is blighted.
{
	
	SHORT sTaxReceived = 0;

	if (sProvinceRating >= (sizeof(ProvinceTaxRateTable)/ sizeof(PROVINCE_TAX_RATE)))
	{
		sProvinceRating = (sizeof(ProvinceTaxRateTable)/ sizeof(PROVINCE_TAX_RATE)) - 1;
	}

	if (sProvinceRating >= 0)  // Only you can prevent array access errors!
	{
		if(flag == 1) // province is blessed
		{
			sTaxReceived = ProvinceTaxRateTable[sProvinceRating].TaxRateDice[TaxRate].sbNumberOfDice * 	
								ProvinceTaxRateTable[sProvinceRating].TaxRateDice[TaxRate].sbNumberOfSides+
								ProvinceTaxRateTable[sProvinceRating].TaxRateDice[TaxRate].sbModifier;
		}
		else if(flag == -1)	// province is blessed
		{
			sTaxReceived = ProvinceTaxRateTable[sProvinceRating].TaxRateDice[TaxRate].sbNumberOfDice * 1+
								ProvinceTaxRateTable[sProvinceRating].TaxRateDice[TaxRate].sbModifier;
		}
		else
			sTaxReceived = RollDice(&ProvinceTaxRateTable[sProvinceRating].TaxRateDice[TaxRate]);

		if (sTaxReceived < 0)
		{
			// Some Taxes will generate negative incomes, they are to be rounded to Zero.
			sTaxReceived = 0;
		}
	}

	return sTaxReceived;
}

/* =======================================================================
   Function    - AveProvinceTaxes
   Description - Return average for the amount to be collected from a Province
   Returns     - 
   ======================================================================== */
SHORT AveProvinceTaxes (
	SHORT sProvinceRating,
	SHORT TaxRate)
{
	SHORT sTaxReceived = 0;

	if (sProvinceRating >= (sizeof(ProvinceTaxRateTable)/ sizeof(PROVINCE_TAX_RATE)))
	{
		sProvinceRating = (sizeof(ProvinceTaxRateTable)/ sizeof(PROVINCE_TAX_RATE)) - 1;
	}

	if (sProvinceRating >= 0)  // Only you can prevent array access errors!
	{
		sTaxReceived = AveDice(&ProvinceTaxRateTable[sProvinceRating].TaxRateDice[TaxRate]);

		if (sTaxReceived < 0)
		{
			// Some Taxes will generate negative incomes, they are to be rounded to Zero.
			sTaxReceived = 0;
		}
	}

	return sTaxReceived;
}

/* =======================================================================
   Function    - RollGuildTempleTaxes
   Description - Roll for the amount to be collected from a Province for guilds
   Returns     - The amount of taxes collected.
   ======================================================================== */
SHORT RollGuildTempleTaxes(
	SHORT sProvinceRating,
	SHORT sHoldingLevel,
	SHORT flag)
{
	SHORT sTaxReceived = 0;

	if (sHoldingLevel > 0)		// If you've got no holdings you can't tax'm
	{
		if (sHoldingLevel > HOLDINGS_MAX_TAX)
		{
			sHoldingLevel = HOLDINGS_MAX_TAX;
		}
		else if (sHoldingLevel == 5)
		{
			sHoldingLevel = 4;	// 4 & 5 share the same column of data.
		}

		if (sProvinceRating > (sizeof(GuildTaxRateTable) / sizeof(GUILD_TAX_RATE)))
		{
			sProvinceRating = (sizeof(GuildTaxRateTable) / sizeof(GUILD_TAX_RATE)) - 1;
		}
										
		if(flag == 1) // holding is blessed
		{
			sTaxReceived = GuildTaxRateTable[sProvinceRating-1].TaxRateDice[sHoldingLevel-1].sbNumberOfDice * 	
								GuildTaxRateTable[sProvinceRating-1].TaxRateDice[sHoldingLevel-1].sbNumberOfSides+
								GuildTaxRateTable[sProvinceRating-1].TaxRateDice[sHoldingLevel-1].sbModifier;
		}
		else if(flag == -1)  // holding is blighted
		{
			sTaxReceived = GuildTaxRateTable[sProvinceRating-1].TaxRateDice[sHoldingLevel-1].sbNumberOfDice * 1+
								GuildTaxRateTable[sProvinceRating-1].TaxRateDice[sHoldingLevel-1].sbModifier;
		}
		else
			sTaxReceived = RollDice(&GuildTaxRateTable[sProvinceRating - 1].TaxRateDice[sHoldingLevel - 1]);
		
		
		if (sTaxReceived < 0)
		{
			sTaxReceived = 0;	// No negative taxes.
		}
	}
	return sTaxReceived;
}
/* =======================================================================
   Function    - AveGuildTempleTaxes
   Description - Return the average for the amount to be collected from a Province for guilds
   Returns     - 
   ======================================================================== */
SHORT AveGuildTempleTaxes(
	SHORT sProvinceRating,
	SHORT sHoldingLevel)
{
	SHORT sTaxReceived = 0;

	if (sHoldingLevel > 0)		// If you've got no holdings you can't tax'm
	{
		if (sHoldingLevel > HOLDINGS_MAX_TAX)
		{
			sHoldingLevel = HOLDINGS_MAX_TAX;
		}
		else if (sHoldingLevel == 5)
		{
			sHoldingLevel = 4;	// 4 & 5 share the same column of data.
		}

		if (sProvinceRating > (sizeof(GuildTaxRateTable) / sizeof(GUILD_TAX_RATE)))
		{
			sProvinceRating = (sizeof(GuildTaxRateTable) / sizeof(GUILD_TAX_RATE)) - 1;
		}

		// printf("Civ:%d Holding:%d - ",sProvinceRating,sHoldingLevel);

		sTaxReceived = AveDice(&GuildTaxRateTable[sProvinceRating - 1].TaxRateDice[sHoldingLevel - 1]);
		if (sTaxReceived < 0)
		{
			sTaxReceived = 0;	// No negative taxes.
		}
	}
	return sTaxReceived;
}

/* ======================================================================= */




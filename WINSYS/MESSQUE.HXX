//---------------------------------------------------------------------------
//
//	Name: MessQue.hxx
//
//  Author: D. Jackson
//
//	Description: Message que procedures for multiplayer
//
//  Disclaimer: Done this way because it will work
//              @@@@@ rewrite this into list etc later 
//  Comments: 
//
//  Revision History
//
//---------------------------------------------------------------------------
// Copyright 1996 All rights reserved Synergistic Software Inc.
//---------------------------------------------------------------------------
//

#ifndef _MESSQUE_H
#define _MESSQUE_H


   #define MAX_MESSQUE  1000
   #define MAX_MESSDATA 4


   typedef struct
   {

       long  type;                  //---- is empty if zero 
       long  subtype;
       long  data[MAX_MESSDATA];

   }   MESSQUE, * PMESSQUE;



   //
   //---- Battle Message subtypes for type iMPT_BATTLE in mulplay.hxx
   //

   enum
   {
       iMPST_BATTLE_MOVE = 1,      //---- move 
       iMPST_BATTLE_ARRIVE,        //---- arrived

//       iMPST_BATTLE_ENGAGE,        //---- engaged ???

       iMPST_BATTLE_RESULTS,       //---- results 
       iMPST_BATTLE_ROUTE,         //---- route 
       iMPST_BATTLE_FALLBACK,      //---- fallback

       iMPST_BATTLE_MAGIC,         //---- Magic 

       iMPST_BATTLE_WIN,           //---- win
       iMPST_BATTLE_RESERVES,      //---- reserves
       iMPST_BATTLE_END            //---- end
   };


   //
   //---- Prototype
   //

   void Init_MessQue ( void );

   int Add_MessQue( PMESSQUE pMessBuf );

   // int Del_MessQue ( int index );

   int Get_MessQueTS( long type, long subtype, PMESSQUE pMessBuf );

   int DelAll_MessQue ( long type );    

   int Get_MessQueTS1( long type, long subtype, long data0, PMESSQUE pMessBuf );



#endif

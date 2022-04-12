//---------------------------------------------------------------------------
//
//	Name: MessQue.cpp
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


#include "string.h"

#include "MESSQUE.HXX"


//---- @@@@@ Do this for now it needs to be working ( Bad programmer )

MESSQUE MessQue[MAX_MESSQUE];

int Mess_Next = 0;
 

//---- Initialize the message que 

void Init_MessQue (void )
{
   int index;

   for ( index = 0; index < MAX_MESSQUE; index++ )
   {
       MessQue[index].type = 0;       //---- Message type 0 is invalid 
   }

   Mess_Next = 0;   

}


// always add to the bottom

int Add_MessQue( PMESSQUE pMessBuf )
{

   if ( Mess_Next == MAX_MESSQUE )
   {
       return 0;    //---- Que is full 
   }


   memcpy ( &MessQue[Mess_Next], pMessBuf, sizeof ( MESSQUE ) );

   Mess_Next++;

   return 1;

}




int Del_MessQue ( int index )
{
   int x;


   //---- if it is there 

   if ( index < Mess_Next )
   {

      if ( index == ( MAX_MESSQUE - 1 ) )
      {
         MessQue[MAX_MESSQUE-1].type = 0; // que full just blank type
      }
      else
      {
         for ( x = index; x < Mess_Next; x++ )
         {    
             memcpy ( &MessQue[x], &MessQue[x+1], sizeof (MESSQUE) ); 
         }  
      }

      Mess_Next--;
      return 1;

   }
   else
   {
       return 0;
   }

}


//---- remove a message from the que ( first one of this type and subtype )


int Get_MessQueTS( long type, long subtype, PMESSQUE pMessBuf )
{
   int x;

   for ( x=0; x < Mess_Next; x++ )
   {

       if ( MessQue[x].type == type &&
            MessQue[x].subtype == subtype )
       {
          memcpy ( pMessBuf, &MessQue[x], sizeof (MESSQUE) ); 
          Del_MessQue( x );
          return 1;
       }

   }


   return 0;  

}





//---- Delete all messages of this type 

int DelAll_MessQue ( long /* type */ )
{

   int x = 0;
   int y;

   while ( x < Mess_Next )
   {

      if ( x == MAX_MESSQUE - 1 )
      {
         MessQue[MAX_MESSQUE-1].type = 0; // que full just blank type
      }
      else
      {
         for ( y = x; y < Mess_Next; y++ )
         {    
             memcpy ( &MessQue[y], &MessQue[y+1], sizeof (MESSQUE) ); 
         }  
      }

      Mess_Next--;

   }

   return 1;

}


//
//---- Special to remove a message based on first three params
//

int Get_MessQueTS1( long type, long subtype, long data0, PMESSQUE pMessBuf )
{
   int x;

   for ( x=0; x < Mess_Next; x++ )
   {

       if ( MessQue[x].type == type &&
            MessQue[x].subtype == subtype &&
            MessQue[x].data[0] == data0 )
       {
          memcpy ( pMessBuf, &MessQue[x], sizeof (MESSQUE) ); 
          Del_MessQue( x );
          return 1;
       }

   }


   return 0;  

}

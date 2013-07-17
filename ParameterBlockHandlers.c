/**
  ******************************************************************************
 ********************************************************************************
  * @file    Parameter Block Handlers.c
  * @author  JUrban
 *  @version V1.0                                           7/4
  * @date    12-June-2013
  * @brief   This file contains all the functions for handling 
                the parameter blocks.
  ******************************************************************************
  ******************************************************************************
  */
#include "SNAP_Defines.h"
/* external routines*/
extern void FillBuffer (char bufr[],char filchr, int cntr);
extern void Int2ASCII(char i);
/* local Routine prototypes */
void Make_Website_Update_from_Processing_Buffer(void);
void makePNumberHeader(char numb);
void convertPNumber_to_ASCII(char numb);
void copyPHeaderToWebsite(void);
void Copy_ASCII_data_to_Website(void);
void clear_tempblock(void);
void Clear_PHeaderBuffer(void);
/* conversion routines/data*/
extern int Processing_Byte_Count;
extern char PNumber;
extern char PacketCount;
extern int ProcessPtr;
extern int Packet_Data_Pointer;
extern char B2ASCBuf[];
extern char tempblock[];
extern char PHeaderBuffer[];
extern char Packet_Data_Buffer[];
extern char Device_Processing_Buffer[];
extern char Device_Receiver_Buffer[];
extern char Device_Serial_number[];
/*******************************************************************************
*****        Make_Website_Update_from_Processing_Buffer                     ****
*****   taskes the raw device data, and converts to ASCII for website       ****
*****  makes a pnumber header in the following format:                      ****
*****     /P1/... /P15/ then converts binary if necessary                   ****
***** this creates a buffer with ONLY parameter data, no other headers are  ****
*****   included. ex. /P1/0000010123456789/P2/700/P3/800/P4/900.....        ****
***** the "httpsend = bwgroup.." is a stock block when ready to send the    ****
***** httpsend block is copied to the xmit buffer, then the parameters are added
*****                                                                       ****
*****  all data going into the Packet_Data_Buffer uses Website_Param_Pointer
*****  to put it into the buffer
*******************************************************************************/
void Make_Website_Update_from_Processing_Buffer(void){
char i;
PNumber = 1;
Packet_Data_Pointer = 0;
/* put in serial number first */
    makePNumberHeader(PNumber); /* make /Pxx/ header for data*/
    copyPHeaderToWebsite();
    for (i = 0;i<=15;i++){
      Packet_Data_Buffer[Packet_Data_Pointer] = Device_Serial_number[i];
      Packet_Data_Pointer++; }

/* position the process pointer*/
    if (Device_Processing_Buffer[3] == 'A')
      i= 21;
    
  else if (Device_Processing_Buffer[3] == 'B')
    i = 12;
 
for (ProcessPtr =i; ProcessPtr <=Processing_Byte_Count;ProcessPtr++){

 if (Device_Processing_Buffer[ProcessPtr] == 'A'){ /* copy an ASCII field*/
    makePNumberHeader(PNumber); /* make /Pxx/ header for data*/
     copyPHeaderToWebsite();
     ProcessPtr++;
    for (ProcessPtr=ProcessPtr ; ProcessPtr<BFRSIZE;ProcessPtr++){
      if (Device_Processing_Buffer[ProcessPtr] == ',') break;
      Packet_Data_Buffer[Packet_Data_Pointer] = Device_Processing_Buffer[ProcessPtr];
      Packet_Data_Pointer++;
    }
  }
  else if (Device_Processing_Buffer[ProcessPtr] == 'B'){ /* convert-copy an binary field*/
    makePNumberHeader(PNumber); /* make /Pxx/ header for data*/
    clear_tempblock();
    copyPHeaderToWebsite();
    ProcessPtr++;
    if (Device_Processing_Buffer[ProcessPtr] == ','){
    i = 0;
    tempblock[i] = Device_Processing_Buffer[ProcessPtr];
    ProcessPtr++;
    i++;
    }
    else i=0;
    while (Device_Processing_Buffer[ProcessPtr]!= ',') {
      tempblock[i] = Device_Processing_Buffer[ProcessPtr];
      ProcessPtr++;
      i++;
     }
      Int2ASCII(1);
    for (i=0;i<=9;i++){
      if (B2ASCBuf[i]!=' '){
       Packet_Data_Buffer[Packet_Data_Pointer] = B2ASCBuf[i];
       Packet_Data_Pointer++;
      }
      else if (i==9) Packet_Data_Buffer[Packet_Data_Pointer] = 0;
      }
     }
    else if (Device_Processing_Buffer[ProcessPtr] == 0x00){
      Packet_Data_Buffer[Packet_Data_Pointer] = CR;
      PacketCount = Packet_Data_Pointer;
      break;
    }
  }  
  FillBuffer (Device_Receiver_Buffer,0x00, BFRSIZE_HALF);

  } 
/* clear the tempblock*/
void clear_tempblock(void){
 char i;
 for (i=0; i <= 4; i++)
   tempblock[i]=0x00;
}
/* Clear_PHeaderBuffer*/
void Clear_PHeaderBuffer(void){
char i;
  for (i=0; i<=6;i++)
    PHeaderBuffer[i] = 0;
}
/* copyPHeaderToWebsite*/
void copyPHeaderToWebsite(void){
char pntr=0;
while (PHeaderBuffer[pntr] != 0x00){
  Packet_Data_Buffer[Packet_Data_Pointer] = PHeaderBuffer[pntr];
  Packet_Data_Pointer++;
  pntr++;
  }
}
/*****************************************************************************
 *****             makePNumberHeader                                      ****
 ****************************************************************************/
void makePNumberHeader(char numb){
  Clear_PHeaderBuffer();
   clear_tempblock();
  tempblock[0] = numb;
  Int2ASCII(1);
  PHeaderBuffer[0] = '/';
  PHeaderBuffer[1] = 'p';
    if (B2ASCBuf[8]!= ' '){
      PHeaderBuffer[2] = B2ASCBuf[8];
      PHeaderBuffer[3] = B2ASCBuf[9];
      PHeaderBuffer[4] = '/';
     }
     else{
      PHeaderBuffer[2] = B2ASCBuf[9];
      PHeaderBuffer[3] = '/';
     }
      PNumber++;
}


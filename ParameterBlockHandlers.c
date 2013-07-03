/**
  ******************************************************************************
 ********************************************************************************
  * @file    Parameter Block Handlers.c
  * @author  JUrban
  * @version V1.0
  * @date    12-June-2013
  * @brief   This file contains all the functions for handling 
                the parameter blocks.
  ******************************************************************************
  ******************************************************************************
  */
#include "SNAP_Defines.h"
/* external routines*/
extern void FillBuffer (char bufr[],char filchr, int cntr);
extern void Int2ASCII(void);
void ProcessB2ASCBuf(void);

/* local Routine prototypes */
void Make_Website_Update_from_Processing_Buffer(void);
void makePNumberHeader(char numb);
void convertPNumber_to_ASCII(char numb);
void copyPHeaderToWebsite(void);
void Copy_ASCII_data_to_Website(void);
void clear_tempblock(void);
/* conversion routines/data*/
extern char Packet_Data_Count;
extern char PNumber;
extern int PCount;
extern int ProcessPtr;
extern int Packet_Data_Pointer;
extern char B2ASCBuf[];
extern char tempblock[];
extern char PHeaderBuffer[];
extern char Packet_Data_Buffer[];
extern char Device_Processing_Buffer[];

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
for (ProcessPtr =3; ProcessPtr <BFRSIZE;ProcessPtr++){
  makePNumberHeader(PNumber); /* make /Pxx/ header for data*/
  if (Device_Processing_Buffer[ProcessPtr] == 'A'){
     copyPHeaderToWebsite();
     ProcessPtr++;
    for (ProcessPtr=ProcessPtr ; ProcessPtr<BFRSIZE;ProcessPtr++){
        if (Device_Processing_Buffer[ProcessPtr] == ',') break;
      Packet_Data_Buffer[Packet_Data_Pointer] = Device_Processing_Buffer[ProcessPtr];
      Packet_Data_Pointer++;
    }
  }
  else if (Device_Processing_Buffer[ProcessPtr] == 'B'){
    clear_tempblock();
    copyPHeaderToWebsite();
    ProcessPtr++;
    i = 0;
    while (Device_Processing_Buffer[ProcessPtr]!= ',') {
      tempblock[i] = Device_Processing_Buffer[ProcessPtr];
      i++;
      ProcessPtr++;
    }
      Int2ASCII();
      ProcessB2ASCBuf();
    for (i=0;i<=7;i++){
      if (B2ASCBuf[i] != ' '){
       Packet_Data_Buffer[Packet_Data_Pointer] = B2ASCBuf[i];
       Packet_Data_Pointer++;
       }
      }
    }
    else if (Device_Processing_Buffer[ProcessPtr] == 0x00){
      break;
    }
      
  } 
  Packet_Data_Count = Packet_Data_Pointer -1;
}
void clear_tempblock(void){
 char i;
 for (i=0; i <= 4; i++)
   tempblock[i]=0x00;
}
void copyPHeaderToWebsite(void){
char pntr=0;
while (PHeaderBuffer[pntr] != 0x00){
  Packet_Data_Buffer[Packet_Data_Pointer] = PHeaderBuffer[pntr];
  Packet_Data_Pointer++;
  pntr++;
}
 
}

void ProcessB2ASCBuf(void){
char i;
for (i=0; i<=7;i++){
  if ((B2ASCBuf[i]=='0') && (i != 7))
      B2ASCBuf[i]=' ';
  else if (B2ASCBuf[i]!='0') break;
}
}/*****************************************************************************
 *****             makePNumberHeader                                      ****
 ****************************************************************************/
void makePNumberHeader(char numb){
  FillBuffer(PHeaderBuffer,0x00, 6);
  clear_tempblock();
  tempblock[0] = numb;
  Int2ASCII();
  PHeaderBuffer[0] = '/';
  PHeaderBuffer[1] = 'P';
    if (B2ASCBuf[6]!= '0'){
      PHeaderBuffer[2] = B2ASCBuf[6];
      PHeaderBuffer[3] = B2ASCBuf[7];
      PHeaderBuffer[4] = '/';
     }
     else{
      PHeaderBuffer[2] = B2ASCBuf[7];
      PHeaderBuffer[3] = '/';
     }
      PNumber++;
}

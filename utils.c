#include "main.h"
extern char B2ASCBuf[];

char Indx;
extern char tempblock[4];
u32 LocalSum;
void BIN2ASC(u32 InNum,char leading);
void Int2ASCII(char leading);
void clearB2ASCBuf(void);
void InitB2ASCBuf(void);
/****************************************************************/
/*** clear the binary to ascii buffer                         ***/
/****************************************************************/
void clearB2ASCBuf(void){
 char i;
 for (i=0;i<=9;i++){
      if (B2ASCBuf[i] != '0') break;
      if (i == 9) break;
      B2ASCBuf[i] = ' ';
      }
}
void InitB2ASCBuf(void){
 char i;
 for (i=0;i<=9;i++)
      B2ASCBuf[i] = '0';
}
/****************************************************************/
/*** Int2ASCII                                                ***/
/*** convert binary to ascii                                  ***/
/*** templock = binary number  up to 4 bytes lsb - msb        ***/
/*** B2ASCBuf = ascii number with leading zero                ***/
/****************************************************************/
void Int2ASCII(char leading){
        LocalSum = tempblock[0];
	LocalSum = LocalSum + (tempblock[1] * 256);
 	LocalSum = LocalSum + (tempblock[2] * 65536);
 	LocalSum = LocalSum + (tempblock[3] * 16777216);
        InitB2ASCBuf();
        BIN2ASC(LocalSum,leading);
}
/*----------------------------------------------------------------------*/
/*up to 4 bytes binary to ascii output					*/
/*leading zero not supressed. to buffer B2ASCBuf[8] returns number count*/
/*STK =1								*/
/*----------------------------------------------------------------------*/
void BIN2ASC(u32 InNum,char leading){
char Bufptr;
char Res;
	Bufptr = 0;
        Res = 0;
        if (InNum >= 1000000000){
        Res = InNum / (u32)1000000000;
	B2ASCBuf[Bufptr] = Res | 0x30;
	InNum = InNum - (Res * (u32)1000000000);
        }
	Bufptr++;

        if (InNum >= 100000000){
	Res = InNum / (u32)100000000;
	B2ASCBuf[Bufptr] = Res | 0x30;
	InNum = InNum - (Res * (u32)100000000);
        }
	Bufptr++;
 
        if (InNum >= 10000000){
	Res = InNum / (u32)10000000;
	B2ASCBuf[Bufptr] = Res | 0x30;
	InNum = InNum - (Res * (u32)10000000);
        }
	Bufptr++;

        if (InNum >= 1000000){
	Res = InNum / (u32)1000000;
	B2ASCBuf[Bufptr] = Res | 0x30;
	InNum = InNum - (Res * (u32)1000000);
        }
	Bufptr++;

        if (InNum >= 100000){
	Res = InNum / (u32)100000;
	B2ASCBuf[Bufptr] = Res | 0x30;
	InNum = InNum - (Res * (u32)100000);
        }
	Bufptr++;

        if (InNum >= 10000){
	Res = InNum / (u32)10000;
	B2ASCBuf[Bufptr] = Res | 0x30;
	InNum = InNum - (Res * (u32)10000);
        }
	Bufptr++;

        if (InNum >= 1000){
	Res = InNum / (u32)1000;
	B2ASCBuf[Bufptr] = Res | 0x30;
	InNum = InNum - (Res * (u32)1000);
        }
	Bufptr++;

	Res = InNum / (u32)100;
	B2ASCBuf[Bufptr] = Res | 0x30;
	InNum = InNum - (Res * (u32)100);
	Bufptr++;

	Res = InNum / (u32)10;
	B2ASCBuf[Bufptr] = Res | 0x30;
	InNum = InNum - (Res * (u32)10);
	Bufptr++;

	B2ASCBuf[Bufptr] = InNum | 0x30;
        if (leading == 1)
        clearB2ASCBuf();
}

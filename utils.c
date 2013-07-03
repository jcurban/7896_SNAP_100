#include "main.h"
extern char B2ASCBuf[];
extern void FillBuffer (char bufr[],char filchr, int cntr);

char Indx;
char Temp;	
char Temp1;	
char Temp2;	
extern char tempblock[4];
char temphldf[5];
char testbuffer[30];
u32 LocalSum;
char UartSendBlock[32];
void StoreB2AscBuf( u32 BinNum);
void BIN2ASC(u32 BinNum);
void Int2ASCII(void);
void TESTHDR(void);



void Int2ASCII(void){
        LocalSum = tempblock[0];
	LocalSum = LocalSum + (tempblock[1] * 256);
 	LocalSum = LocalSum + (tempblock[2] * 65536);
	StoreB2AscBuf( LocalSum);	/*uses up 8 bytes in send block*/
}
/****************************************************************/
/*helper routine to take a binary number conver to ascii	*/
/*then put into B2ASCBuf					*/
/* input BinNum = number to convert to 8 digits			*/
/*       Indx = index to store ascii into buffer B2ASCBuf	*/ 
/****************************************************************/
void StoreB2AscBuf(u32 BinNum){
	BIN2ASC(BinNum);	/*counts*/
}
/*----------------------------------------------------------------------*/
/*up to 3 bytes binary to ascii output					*/
/*leading zero not supressed. to buffer B2ASCBuf[8] returns number count*/
/*STK =1								*/
/*----------------------------------------------------------------------*/
void BIN2ASC(u32 InNum){
char Bufptr;
char Res;
  FillBuffer(B2ASCBuf,' ', 8);
	Bufptr = 0;
	Res = InNum / (u32)10000000;
	B2ASCBuf[Bufptr] = Res | 0x30;
	InNum = InNum - (Res * (u32)10000000);
	Bufptr++;

	Res = InNum / (u32)1000000;
	B2ASCBuf[Bufptr] = Res | 0x30;
	InNum = InNum - (Res * (u32)1000000);
	Bufptr++;

	Res = InNum / (u32)100000;
	B2ASCBuf[Bufptr] = Res | 0x30;
	InNum = InNum - (Res * (u32)100000);
	Bufptr++;

	Res = InNum / (u32)10000;
	B2ASCBuf[Bufptr] = Res | 0x30;
	InNum = InNum - (Res * (u32)10000);
	Bufptr++;

	Res = InNum / (u32)1000;
	B2ASCBuf[Bufptr] = Res | 0x30;
	InNum = InNum - (Res * (u32)1000);
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
}


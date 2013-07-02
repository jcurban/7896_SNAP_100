#include "main.h"
char B2ASCBuf[8];
char Indx;
char Temp;	
char Temp1;	
char Temp2;	
char tempblock[4];
char temphldf[5];
char testbuffer[30];
u32 LocalSum;
char UartSendBlock[32];
void StoreB2AscBuf( u32 BinNum, u8 Indx);
void BIN2ASC(u32 BinNum);
void Int2ASCII(void);
void TESTHDR(void);

void Int2ASCII(void){
        tempblock[0] = 100;
        tempblock[1] = 36;
        tempblock[2] = 5;
        LocalSum = tempblock[0];
	LocalSum = LocalSum + (tempblock[1] * 256);
 	LocalSum = LocalSum + (tempblock[2] * 65536);
        /*LocalSum = (Temp2 * 65536);
	LocalSum = LocalSum + (Temp1 * 256);
	LocalSum = LocalSum + Temp;*/
	StoreB2AscBuf( LocalSum, 0);	/*uses up 8 bytes in send block*/
}
/****************************************************************/
/*helper routine to take a binary number conver to ascii	*/
/*then put into B2ASCBuf					*/
/* input BinNum = number to convert to 8 digits			*/
/*       Indx = index to store ascii into buffer B2ASCBuf	*/ 
/****************************************************************/
void StoreB2AscBuf( u32 BinNum, u8 Indx){
	BIN2ASC(BinNum);	/*counts*/
	UartSendBlock[Indx]= B2ASCBuf[0];
	UartSendBlock[Indx + 1]= B2ASCBuf[1];
	UartSendBlock[Indx + 2]= B2ASCBuf[2];
	UartSendBlock[Indx + 3]= B2ASCBuf[3];
	UartSendBlock[Indx + 4]= B2ASCBuf[4];
	UartSendBlock[Indx + 5]= B2ASCBuf[5];
	UartSendBlock[Indx + 6]= B2ASCBuf[6];
	UartSendBlock[Indx + 7]= B2ASCBuf[7];
}
/*----------------------------------------------------------------------*/
/*up to 3 bytes binary to ascii output					*/
/*leading zero not supressed. to buffer B2ASCBuf[8] returns number count*/
/*STK =1								*/
/*----------------------------------------------------------------------*/
void BIN2ASC(u32 InNum){
char Bufptr;
char Res;
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


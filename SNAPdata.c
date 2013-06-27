#include "stm8s.h"
const char DLECHR = 0X10;
const char ETXCHR = 0x03;
const int BFRSIZE = 255;
u8 checksum;
u8 checksum_Okay;   /* 0 = okay, 1 = not*/
int cntr,gtchr;
char Device_Serial_number[16];
char Device_Type_Number[6];
unsigned char   DEVICE_SOH; /*START OF TEXT 0X02*/
u16 DEVICE_MSGLENGTH; /*LENGTH OF MESSAGE*/
unsigned char DEVICE_COMMAND; /* COMMAND TYPE*/
unsigned char DEVICE_PACKETDATA[512]; 
unsigned char DEVICE_CHECKSUM;
unsigned char DEVICE_EOT; /*end of text*/

/* DEVICE DATA */
char Device_State;
/*device receiving data*/
u8 Device_RX_InPtr;
u8 Device_RX_OutPtr;
u16 Device_Rcvr_Char_Count;
u8 Device_Rcvr_Complete_flag;
u8 Device_Update_Ready_for_Website_flag;
u8 Device_Rcvr_State;
u8 Device_Rcvr_char;
char *Device_Rcvr_Pointer;
char *Device_Rcvr_Dest_Pointer;
unsigned char Device_Receiver_Buffer[255];
unsigned char Packet_Data_Buffer[256];

/*device xmiting data*/
u8 Device_Xmit_Complete_Flag;
u8 Device_Xmit_Char;
char Device_Xmit_Pointer;
u8 Device_Xmit_Char_Count;
char* Device_Processing_Pointer;
unsigned char Device_Processing_Buffer[256];
unsigned char Device_Xmit_Buffer[256];

/*GS1011 data*/
char GS1011_State;
char GS1011_State_State;
char GS1011_Rcvr_Char;
char GS1011_Received_Response_Flag;
u8 GS1011_String_Found;
u16 GS1011_Rcvr_InPtr;
u16 GS1011_Rcvr_OutPtr;
u16 GS1011_Rvcr_Count;
u16 GS1011_Rcvr_Pointer;
char GS1011_Receiver_Buffer[512];

/*GS1011 receiving data*/
u8 GS1011_Xmit_Pointer;
u8 GS1011_Xmit_Char_Count;
unsigned char GS1011_Xmit_Char;
unsigned char GS1011_Xmit_Buffer[256];

char CID_Value;

unsigned char PNumber;
unsigned int PCount;
unsigned char PBuffer[16];


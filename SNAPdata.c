#include "stm8s.h"
#include "SNAP_Defines.h"
;
char SNAP_State;
u8 checksum_Okay;   /* 0 = okay, 1 = not*/

char Device_Serial_number[16];
char website_IP_Address[15];
/* conversion variables*/
char PNumber;
char PacketCount;
int ProcessPtr;
int Packet_Data_Pointer;
char PHeaderBuffer[6];
char tempblock[4];
char B2ASCBuf[8];

/* DEVICE DATA */
char Device_State;
/*device receiving data*/
u8 Device_RX_InPtr;
u8 Device_RX_OutPtr;
u16 Device_Rcvr_Char_Count;
u8 Device_Rcvr_Complete_flag;
u8 Device_Update_Ready_for_Website_flag;
u8 Device_Rcvr_char;
char Device_Rcvr_Pointer;
char Device_Rcvr_Dest_Pointer;
char Device_Receiver_Buffer[255];
unsigned char Packet_Data_Buffer[255];

/*device xmiting data*/
char Device_Rcvr_EOM_Timer;
char Device_Rcvr_Timeout;
u8 Device_Xmit_Complete_Flag;
u8 Device_Xmit_Char;
char Device_Xmit_Pointer;
char Device_Xmit_Setup_Char_Pointer;
u8 Device_Xmit_Char_Count;
char* Device_Processing_Pointer;
int Processing_Byte_Count;
unsigned char Device_Processing_Buffer[255];
unsigned char Device_Xmit_Buffer[128];
/*GS1011 data*/
char GS1011_Rcvr_EOM_Timer;
char GS1011_Rcvr_Timeout;
char GS1011_State;
char GS1011_State_State;
char GS1011_Rcvr_Char;
char GS1011_Received_Response_Flag;
int Found_String_At_Byte;
u8 GS1011_String_Found;
u16 GS1011_Rcvr_InPtr;
u16 GS1011_Rcvr_OutPtr;
u16 GS1011_Rvcr_Count;
u16 GS1011_Rcvr_Pointer;
char GS1011_Receiver_Buffer[512];

char ACK_message_response_number;
/*GS1011 receiving data*/
u8 GS1011_Xmit_Pointer;
u8 GS1011_Xmit_Char_Count;
char GS1011_Xmit_Setup_Char_Pointer;
unsigned char GS1011_Xmit_Char;
unsigned char GS1011_Xmit_Buffer[256];

char CID_Value;



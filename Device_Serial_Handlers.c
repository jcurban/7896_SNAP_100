/**
  ******************************************************************************
 *******************************************************************************
  * @file    Device_Serial_handlers.c
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    11-June-2013
  * @brief   This file contains all the functions for the uart3 peripheral.
  ******************************************************************************
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "stm8s_uart3.h"
#include "stm8s_it.h"
#include "SNAP_Defines.h"
/* EXTERNAL ROUTINES */
extern void CopyBuffer (char *dest, char *srce);
extern void CopyBufferDevice(char srce[]);
extern void CopyBufferCounted (char *dest, char *srce, int cntr);
extern void FillBuffer (char bufr[],char filchr, int cntr);
extern void UART3_SendData8(u8);
extern int InitializeDeviceBuffer (char *bufr,char filchr, int cntr);
extern int Add_Char_to_Buffer (char *bufr, int ptr, char chr);
extern int Add_Integer_to_Buffer (char *bufr,int ptr, int vint);
extern int Add_String_to_Buffer (char *bufr, int ptr, char *srce);
void Add_Char_to_GS1011_Buffer (char chr);
extern void Initialize_GS011_Xmit_buffer(void);
extern void CopyBufferGS1011 (char dest[], char srce[]);

extern void Make_Website_Update_from_Processing_Buffer(void);
void Add_String_to_GS1011_Buffer ( char *srce);
void CopySerialNumber(void);
void Send_ACK_Message(void);
char Check_Checksum_Device_Buffer(void);
/* EXTERNAL DATA */
extern char Device_Serial_number[];
extern char SNAP_State;
extern char checksum_Okay;
extern char CID_Value;
extern char Device_State;
extern char checksum_this;

extern char Device_Processing_Buffer[];
extern char Website_Parameter_ASCII_Buffer[];
extern char Device_Xmit_Buffer[];
extern int cntr,gtchr;
extern u8 Device_RX_InPtr;
extern u8 Device_RX_OutPtr;
extern int Device_Rcvr_Char_Count;
extern u8 Device_Rcvr_Complete_flag;
extern char Device_Update_Ready_for_Website_flag;
extern u8 Device_Rcvr_char;
extern int Device_Processing_Pointer;
extern int Device_Rcvr_Pointer;
extern char  *Device_Rcvr_Dest_Pointer;
extern char Device_Receiver_Buffer[];
extern char GS1011_Xmit_Buffer[];
extern char ACKMessage[];
extern char Packet_Data_Buffer;
extern u8 Device_Xmit_Complete_Flag;
extern u8 Device_Xmit_Char;
extern char Device_Xmit_Pointer;
extern u8 Device_Xmit_Char_Count;
extern u8 GS1011_Xmit_Char_Count;
extern char GS1011_Xmit_Buffer[];
/* EXTERNAL DATA STOCK MESSAGES */
extern char   DEVICE_SOH; /*START OF TEXT 0X02*/
extern int DEVICE_MSGLENGTH; /*LENGTH OF MESSAGE*/
extern char DEVICE_COMMAND; /* COMMAND TYPE*/
extern char DEVICE_PACKETDATA; 
extern char DEVICE_CHECKSUM;
extern char DEVICE_EOT; /*end of text*/
extern char SNAPREADYHDR;
extern char SNAPREADY;
extern char SNAPREADYEND;
extern char Powered[];
extern char SendtoWebsiteHeader[];
extern char SendtoWebsiteHeader1[];

/* LOCAL DEFINITIONS */
void InitDeviceUART(void);
void Get_Device_Char(void);
void Send_Next_Char_to_Device(void);
/*void Make_Send_SNAP_Ready_Message(void);*/
void Initialize_Device_receiver_buffer(void);
void Handle_Device_State(void);
void Assemble_and_Checksum_device_message(void);
char Checksum_Device_Buffer(char bufr[]);
char Parse_Device_Rcvrd_Buffer(void);
void Start_Device_Xmit (void);
void Save_PValues(void);
void Send_powered_Message(void);

void Process_Receiver_Device_Message(void);
void Process_Xmit_Device_Message(char bufr[], char bufr2[]);

void Send_Powered(void);
void Wait_For_Update(void);
void Process_Received_Update(void);
void Send_Update(void);
void Send_Finished(void);
void Convert_Update_Parameters(void);
void Send_Powered_Wait_For_Update(void);
/*****************************************************************************/
/*****************************************************************************/
/*****       device State machine                                        *****/
/*****************************************************************************/
/*****************************************************************************/

/*****************************************************************************/
/* State 0 - Send_Powered                                                    */
/*      Lets the Device know we're ready for the update                      */
/*****************************************************************************/
void Send_Powered(void){
  FillBuffer (Device_Receiver_Buffer,0x00, BFRSIZE);
  Send_powered_Message();
  Device_Update_Ready_for_Website_flag =0;
  SNAP_State = 1;
}
/*****************************************************************************/
/* State 1 - Wait_For_Update                                                 */
/*      can't do much without the update data                                */
/*****************************************************************************/
void Wait_For_Update(void){
  if (Device_Rcvr_Complete_flag != 0){
      Device_Rcvr_Complete_flag = 0;
      SNAP_State = 2;}
}
/*****************************************************************************/
/* State 2 - Process_Update                                                  */
/*      convert the update data to ASCII for the website                     */
/*****************************************************************************/
void Process_Received_Update(void){
  Process_Receiver_Device_Message();
  checksum_Okay = Check_Checksum_Device_Buffer();
  if (checksum_Okay == 0x55){
    CopySerialNumber();
    Make_Website_Update_from_Processing_Buffer();
    SNAP_State = 3;
  }
  else{                                 /*if checksum wrong ask for it again*/
  }
}
/*****************************************************************************/
/* State 3 - Send_Update                                                     */
/*      Send the message up to the website                                   */
/*****************************************************************************/
void Send_Update(void){
  if (CID_Value != 0x00) {
    FillBuffer (GS1011_Xmit_Buffer,0x00, BFRSIZE);
    Convert_Update_Parameters();   
  } 
}
/*****************************************************************************/
/* State 4 - Send_Finished                                                 */
/*      got response from website                                 */
/*****************************************************************************/
void Send_Finished(void){
}

  /* Check configuration data in EE */
  
  
  /*SendYouThereMessageToGS1011();
  InitializeGS1011Buffer();
  SendYouThereMessageToGS1011();*/

/*****************************************************************************/
/**************************e wai***************************************************/
/****         Send messagt response routines                          ***/
/*****************************************************************************/
/*****************************************************************************/
void Convert_Update_Parameters(void){
 CID_Value = 0x30;
 Initialize_GS011_Xmit_buffer();
 CopyBufferGS1011(GS1011_Xmit_Buffer, SendtoWebsiteHeader);
 Add_Char_to_GS1011_Buffer (CID_Value);
 Add_String_to_GS1011_Buffer (SendtoWebsiteHeader1);
 }
/*****************************************************************************/
/*****        Send_powered_Message                                ****/
/*****        1st byte in Device processed buffer is the character count  ****/
/*****************************************************************************/
void Send_powered_Message(void){
  Initialize_Device_receiver_buffer();
  CopyBufferDevice(Powered);
  Device_Xmit_Char_Count = Device_Xmit_Char_Count;
  Start_Device_Xmit ();
}
/*****************************************************************************/
/*****        Send_powered_Message                                ****/
/*****        1st byte in Device processed buffer is the character count  ****/
/*****************************************************************************/
void Send_ACK_Message(void){
  Initialize_Device_receiver_buffer();
  CopyBufferDevice(ACKMessage);
  Device_Xmit_Char_Count = Device_Xmit_Char_Count;
  Start_Device_Xmit ();
}
/*****************************************************************************/
/*****        Make_Send_SNAP_Ready_Message                                ****/
/*****        1st byte in Device processed buffer is the character count  ****/
/*****************************************************************************
void Make_Send_SNAP_Ready_Message(void){
  FillBuffer (&Device_Xmit_Buffer,0x00, BFRSIZE);
  Device_Processing_Pointer = 0;
  Device_Processing_Pointer = Add_String_to_Buffer (&Device_Xmit_Buffer, Device_Processing_Pointer, &SNAPREADYHDR);
  Device_Processing_Pointer = Add_String_to_Buffer (&Device_Xmit_Buffer, Device_Processing_Pointer, &SNAPREADY);
  Device_Processing_Pointer = Add_String_to_Buffer (&Device_Xmit_Buffer, Device_Processing_Pointer, &SNAPREADYEND);
  Device_Xmit_Char_Count = CountChars(&Device_Xmit_Buffer);
  Start_Device_Xmit ();
} */
/*****************************************************************************/
/*****        Assemble_and_Checksum_device_message                        ****/
/*****        1st byte in Device processed buffer is the character count  ****/
/*****************************************************************************/
void Assemble_and_Checksum_device_message(void){
  /*Device_Processing_Pointer = InitializeDeviceBuffer (Device_Processing_Buffer, 0x00, BFRSIZE/2);
  Device_Processing_Pointer = Add_Char_to_Buffer (Device_Processing_Buffer, Device_Processing_Pointer, DEVICE_SOH);
  Device_Processing_Pointer = Add_Integer_to_Buffer (Device_Processing_Buffer, Device_Processing_Pointer, DEVICE_MSGLENGTH);
  Device_Processing_Pointer = Add_Char_to_Buffer (Device_Processing_Buffer, Device_Processing_Pointer, DEVICE_COMMAND);
  Device_Processing_Pointer = Add_String_to_Buffer (Device_Processing_Buffer, Device_Processing_Pointer, &DEVICE_PACKETDATA);
  Checksum_Device_Buffer(Device_Processing_Buffer);
  Device_Processing_Pointer = Add_Char_to_Buffer (Device_Processing_Buffer, Device_Processing_Pointer, DEVICE_CHECKSUM);
  Device_Processing_Pointer = Add_Char_to_Buffer (Device_Processing_Buffer, Device_Processing_Pointer, DEVICE_EOT);*/
  /*Process_Device_Message(&Device_Xmit_Buffer, &Device_Processing_Buffer);
  Start_Device_Xmit ();          save the start of the buffer*/
}
/*****************************************************************************/
/*****             Checksum_Device_Buffer                                 ****/
/*****        1st byte in Device processed buffer is the character count  ****/
/*****                                                                    ****/
/*****  bytes from 1st after count to checksum byte are checksumed        ****/
/*****************************************************************************/
char Checksum_Device_Buffer(char bufr[]){
  int cntr,gtchr;
  int checksum;
  checksum =0;
  cntr = bufr[1];
  cntr = (cntr + bufr[2] * 256)+ 1;
  if (cntr < BFRSIZE){
  for (gtchr = 1; gtchr < cntr; gtchr++){
      checksum += bufr[gtchr];  /*ADDIN THE BYTES OF THE PAYLOAD TO THE CHECKSUM*/
    }
    checksum = (0xFF55 - checksum);  /*Cksum to send*/
  }
  else {
     /* error code here*/
  }
return checksum;
}       
/*****************************************************************************/
/*** returns the checksum to the calling routine */
/*****************************************************************************/
char Check_Checksum_Device_Buffer(){
  char checksum;
  char gtchr;
  
  int cntr;
  checksum = 0;
  cntr = Device_Processing_Buffer[0];
  cntr = (cntr + Device_Processing_Buffer[1] * 256);  /* number of bytes to checksum */
                                      /* after stx and to etx */
 if (cntr < BFRSIZE){
  for (gtchr = 0; gtchr <= cntr; gtchr++){
      checksum += Device_Processing_Buffer[gtchr];  /*ADDIN THE BYTES OF THE PAYLOAD TO THE CHECKSUM*/
  }
  gtchr++;
  checksum += Device_Processing_Buffer[gtchr];
 }
 Device_Processing_Buffer[gtchr] = 00; /*clear out checksum*/ 
 gtchr++;
 Device_Processing_Buffer[gtchr] = 00; /*clear out etx*/ 
 return checksum;
}

/*****************************************************************************/
/*****             Process_Device_Message                                 ****/
/*****        takes the buffer it's passed and inserts DLEs where necess. ****/
/*****************************************************************************/
void Process_Xmit_Device_Message(char bufr[], char bufr2[]){
int bfrptr;
 u16 xmtptr;
 u8 i;
 for (bfrptr = 0; bfrptr <3; bfrptr++){      /* PUT SOH, PACKET COUNT INTO*/
  bufr2[bfrptr] = bufr[bfrptr];               /*XMIT BUFFER*/  
 }
xmtptr = bfrptr;
cntr = bufr[1];
cntr = cntr + ((bufr[2] * 256)+3);

for (bfrptr = bfrptr; bfrptr <cntr; bfrptr++){
  i = bufr[bfrptr];
  if (bufr[bfrptr]== STX || bufr[bfrptr] == ETX || bufr[bfrptr] == DLE|| bufr[bfrptr] == ','){
     i ^= 0xFF;
    bufr2[xmtptr] =  DLE;
    xmtptr++;
  }
  bufr2[xmtptr] =  i;
  xmtptr++;
 }
bufr2[xmtptr] =  ETX;
}
/*****************************************************************************/
/*****************************************************************************/
/*****  INITIALIZE RECEIVER BUFFER                                       *****/
/*****************************************************************************/
/*****************************************************************************/
void Initialize_Device_receiver_buffer(void){
  FillBuffer (Device_Receiver_Buffer,0x00, BFRSIZE);
  Device_RX_InPtr=0;
  Device_RX_OutPtr=0;
}
/*****************************************************************************/
/*****************************************************************************/
/****                        ISR routines                                  ***/
/*****************************************************************************/
/*****************************************************************************/

/*****************************************************************************/
/*****        Get_Device_Char  - UART3 RCV INTERRUPT                      ****/
/*****        1st byte in Device processed buffer is the character count  ****/
/*****************************************************************************/
void Get_Device_Char(void){
  Device_Rcvr_char = ((u8)UART3->DR);
  Device_Receiver_Buffer[Device_RX_InPtr] = Device_Rcvr_char;
  if (Device_Rcvr_char == 0x03)
      Device_Rcvr_Complete_flag = 1;
  Device_RX_InPtr++;
}

/*****************************************************************************/
/*****************************************************************************/
/*****        Device Receive message handlers                            *****/
/*****************************************************************************/
/*****************************************************************************/

/*****************************************************************************/
/*****             Process_Receiver_Device_Message                        ****/
/*****   takes the buffer it's passed and unescapes where necess.         ****/
/*****************************************************************************/
void Process_Receiver_Device_Message(void){
 u16 rcvrbfrptr;
 u16 prcsbufrptr;
 u8 i;
prcsbufrptr =0;
for (rcvrbfrptr = 1; rcvrbfrptr <BFRSIZE; rcvrbfrptr++){
  i = Device_Receiver_Buffer[rcvrbfrptr];
  if (i == 0x03) {
    Device_Processing_Buffer[prcsbufrptr] =  i;
    break;
    }
  if (Device_Receiver_Buffer[rcvrbfrptr] == 0x10){
    rcvrbfrptr++;
    i = Device_Receiver_Buffer[rcvrbfrptr];
     i ^= 0xFF;
  }
  Device_Processing_Buffer[prcsbufrptr] =  i;
  prcsbufrptr++;
 }
}
/*****************************************************************************/
/*****************************************************************************/
/*****        Device xmit message handlers                               *****/
/*****************************************************************************/
/*****************************************************************************/

/*****************************************************************************/
/*****        start Device xmit                                           ****/
/*****        calling routine MUST:                                       ****/
/*****           Device_Xmit_Pointer = Address of buffer to be sent       ****/
/*****           Device_Xmit_Char_Count = number of characters to send    ****/
/*****                                                                    ****/
/*****      puts 1st character in the data reg. and enables xmit and int  ****/
/*****************************************************************************/
void Start_Device_Xmit (void){
  Device_Xmit_Pointer = 0;
  Device_Xmit_Char = Device_Xmit_Buffer[Device_Xmit_Pointer];
  Device_Xmit_Pointer++;
  Device_Xmit_Char_Count--;
  Device_Xmit_Complete_Flag =1;
  UART3->DR = Device_Xmit_Char;
  UART3->CR2 |= (uint8_t)UART3_CR2_TCIEN | UART3_CR2_TIEN;
  while (Device_Xmit_Complete_Flag == 1)
  {};
}
/*****************************************************************************/
/*****        Device xmit - UART3 XMIT INTERRUPT HANDLER                  *****/
/*****        sends characters till the count expires, on the next int.   ****/
/*****             turnf off the txmit enable, once TXE is low            ****/
/*****************************************************************************/
void Send_Next_Char_to_Device(void){
  if (UART3->SR & UART3_FLAG_TXE){
    if (Device_Xmit_Char_Count != 0){
      Device_Xmit_Char = Device_Xmit_Buffer[Device_Xmit_Pointer];
      if (Device_Xmit_Char == 0x03){
        Device_Xmit_Char_Count = 0;
        UART3->DR = Device_Xmit_Char;
        }
        else{
          Device_Xmit_Pointer++;
          Device_Xmit_Char_Count--;
          UART3->DR = Device_Xmit_Char;
          }
    }
    else {
      UART3->CR2 &= (uint8_t)(~(UART3_CR2_TIEN | UART3_CR2_TCIEN ));  
      Device_Xmit_Complete_Flag = 0;
    }
  }
}
/*****************************************************************************/
/*****        Device Initialize/DeInit                                   *****/
/*****************************************************************************/
/*****************************************************************************/
void InitDeviceUART(void)
{
  UART3_Init(9600, UART3_WORDLENGTH_8D, 
                 UART3_STOPBITS_1,  UART3_PARITY_NO, 
                 UART3_MODE_TXRX_ENABLE);
  UART3->CR2 |= 0x20;  /*UART3 RX INTERRUPT ENABLE*/
  Device_Rcvr_Complete_flag = 0;
}

/*****************************************************************************/
/*****                          UART3_DeInit                              ****/
/***** Clear the Idle Line Detected bit in the status rerister by a read  ****/
/***** to the UART3_SR register followed by a Read to the UART3_DR register **/
/*****************************************************************************/
void UART3_DeInit(void)
{
    /* Clear the Idle Line Detected bit in the status rerister by a read
       to the UART3_SR register followed by a Read to the UART3_DR register */
    (void) UART3->SR;
    (void) UART3->DR;

    UART3->BRR2 = UART3_BRR2_RESET_VALUE; /*Set UART3_BRR2 to reset value 0x00 */
    UART3->BRR1 = UART3_BRR1_RESET_VALUE; /*Set UART3_BRR1 to reset value 0x00 */

    UART3->CR1 = UART3_CR1_RESET_VALUE;  /*Set UART3_CR1 to reset value 0x00  */
    UART3->CR2 = UART3_CR2_RESET_VALUE;  /*Set UART3_CR2 to reset value 0x00  */
    UART3->CR3 = UART3_CR3_RESET_VALUE;  /*Set UART3_CR3 to reset value 0x00  */
    UART3->CR4 = UART3_CR4_RESET_VALUE;  /*Set UART3_CR4 to reset value 0x00  */
    UART3->CR6 = UART3_CR6_RESET_VALUE;  /*Set UART3_CR6 to reset value 0x00  */

}

/*****************************************************************************/
/*****                          UART3_Init                                ****/
/*****************************************************************************/
/**
  * @brief  Initializes the UART3 according to the specified parameters.
  * @param  BaudRate: The baudrate.
  * @param  WordLength : This parameter can be any of 
  *         the @ref UART3_WordLength_TypeDef enumeration.
  * @param  StopBits: This parameter can be any of the 
  *         @ref UART3_StopBits_TypeDef enumeration.
  * @param  Parity: This parameter can be any of the 
  *         @ref UART3_Parity_TypeDef enumeration.
  * @param  Mode: This parameter can be any of the @ref UART3_Mode_TypeDef values
  * @retval None
  */
void UART3_Init(uint32_t BaudRate, UART3_WordLength_TypeDef WordLength, 
                UART3_StopBits_TypeDef StopBits, UART3_Parity_TypeDef Parity, 
                UART3_Mode_TypeDef Mode)
{
    uint8_t BRR2_1 = 0, BRR2_2 = 0;
    uint32_t BaudRate_Mantissa = 0, BaudRate_Mantissa100 = 0;

        /* Check the parameters */
    assert_param(IS_UART3_WORDLENGTH_OK(WordLength));
    assert_param(IS_UART3_STOPBITS_OK(StopBits));
    assert_param(IS_UART3_PARITY_OK(Parity));
    assert_param(IS_UART3_BAUDRATE_OK(BaudRate));
    assert_param(IS_UART3_MODE_OK((uint8_t)Mode));

    /* Clear the word length bit */
    UART3->CR1 &= (uint8_t)(~UART3_CR1_M);     
    /* Set the word length bit according to UART3_WordLength value */
    UART3->CR1 |= (uint8_t)WordLength; 

    /* Clear the STOP bits */
    UART3->CR3 &= (uint8_t)(~UART3_CR3_STOP);  
    /* Set the STOP bits number according to UART3_StopBits value  */
    UART3->CR3 |= (uint8_t)StopBits;  

    /* Clear the Parity Control bit */
    UART3->CR1 &= (uint8_t)(~(UART3_CR1_PCEN | UART3_CR1_PS));  
    /* Set the Parity Control bit to UART3_Parity value */
    UART3->CR1 |= (uint8_t)Parity;     

    /* Clear the LSB mantissa of UART3DIV  */
    UART3->BRR1 &= (uint8_t)(~UART3_BRR1_DIVM);  
    /* Clear the MSB mantissa of UART3DIV  */
    UART3->BRR2 &= (uint8_t)(~UART3_BRR2_DIVM);  
    /* Clear the Fraction bits of UART3DIV */
    UART3->BRR2 &= (uint8_t)(~UART3_BRR2_DIVF);  

    /* Set the UART3 BaudRates in BRR1 and BRR2 registers according to UART3_BaudRate value */
    BaudRate_Mantissa    = ((uint32_t)CLK_GetClockFreq() / (BaudRate << 4));
    BaudRate_Mantissa100 = (((uint32_t)CLK_GetClockFreq() * 100) / (BaudRate << 4));
    /* The fraction and MSB mantissa should be loaded in one step in the BRR2 register */
    /* Set the fraction of UART3DIV  */
    BRR2_1 = (uint8_t)((uint8_t)(((BaudRate_Mantissa100 - (BaudRate_Mantissa * 100))
                        << 4) / 100) & (uint8_t)0x0F); 
    BRR2_2 = (uint8_t)((BaudRate_Mantissa >> 4) & (uint8_t)0xF0);

    UART3->BRR2 = (uint8_t)(BRR2_1 | BRR2_2);
    /* Set the LSB mantissa of UART3DIV  */
    UART3->BRR1 = (uint8_t)BaudRate_Mantissa;           

    if ((uint8_t)(Mode & UART3_MODE_TX_ENABLE))
    {
        /* Set the Transmitter Enable bit */
        UART3->CR2 |= UART3_CR2_TEN;  
    }
    else
    {
        /* Clear the Transmitter Disable bit */
        UART3->CR2 &= (uint8_t)(~UART3_CR2_TEN);  
    }
    if ((uint8_t)(Mode & UART3_MODE_RX_ENABLE))
    {
        /* Set the Receiver Enable bit */
        UART3->CR2 |= UART3_CR2_REN;  
    }
    else
    {
        /* Clear the Receiver Disable bit */
        UART3->CR2 &= (uint8_t)(~UART3_CR2_REN);  
    }
}
/******************* (C) COPYRIGHT 2013 MARS INTERNATIONAL *****END OF FILE****/

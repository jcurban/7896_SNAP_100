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
extern void InitializeDeviceBuffer (void);
extern int Add_Char_to_Buffer (char *bufr, int ptr, char chr);
extern int Add_Integer_to_Buffer (char *bufr,int ptr, int vint);
extern int Add_String_to_Buffer (char *bufr, int ptr, char *srce);
extern void CopyBufferGS1011 (char dest[], char srce[]);
extern void CopySerialNumber(void);
extern void Make_Website_Update_from_Processing_Buffer(void);
extern void Add_String_to_GS1011_Buffer ( char *srce);
extern void Send_ACK_Message(void);
/* EXTERNAL DATA */
extern char Device_Rcvr_EOM_Timer;
extern char Device_Rcvr_Timeout;
extern char SNAP_State;
extern char checksum_Okay;
extern char CID_Value;
extern char Device_State;
extern char checksum_this;
extern char Processing_Byte_Count;
extern u8 Device_RX_InPtr;
extern u8 Device_RX_OutPtr;
extern int Device_Rcvr_Char_Count;
extern u8 Device_Rcvr_Complete_flag;
extern char Device_Update_Ready_for_Website_flag;
extern u8 Device_Rcvr_char;
extern int Device_Processing_Pointer;
extern int Device_Rcvr_Pointer;
extern char Device_Rcvr_Dest_Pointer;
extern char Packet_Data_Buffer;
extern u8 Device_Xmit_Complete_Flag;
extern u8 Device_Xmit_Char;
extern char Device_Xmit_Pointer;
extern u8 Device_Xmit_Char_Count;
extern u8 GS1011_Xmit_Char_Count;
extern char webptr;
extern char Device_Update_Data_count;
/* EXTERNAL DATA buffers and stock messages */
extern char Powered[];
extern char ConfiguringAdaptor[];
extern char AdaptorReady[];
extern char ACKMessage[];
extern char ResendMessage[];
extern char SendtoWebsiteHeader[];
extern char SendtoWebsiteHeader1[];

extern char Website_Update_Data_Buffer[];
extern char GS1011_Xmit_Buffer[];
extern char Device_Processing_Buffer[];
extern char Website_Parameter_ASCII_Buffer[];
extern char Device_Xmit_Buffer[];
extern char Device_Receiver_Buffer[];
extern char Device_Serial_number[];
/* LOCAL DEFINITIONS */
void InitDeviceUART(void);
void Get_Device_Char(void);
void Send_Next_Char_to_Device(void);
/*void Make_Send_SNAP_Ready_Message(void);*/
void InitializeDeviceBuffer(void);
void Handle_Device_State(void);
void Assemble_and_Checksum_device_message(void);
void Assemble_Process_Send_Device_Website_Update(void);
void Checksum_Device_Buffer(char bufr[]);
char Check_Checksum_Device_Buffer(void);
void InitializeDevice_Processing_Buffer(void);

char Parse_Device_Rcvrd_Buffer(void);
void Start_Device_Xmit (void);
void Save_PValues(void);
void Send_powered_Message(void);
void InitializeDevice_Processing_Buffer(void);
void Process_Receiver_Device_Message(void);
void Process_Xmit_Device_Message(char bufr[], char bufr2[]);

void Send_Powered(void);
void Wait_For_Update(void);
void Process_Received_Update(void);
void Send_Update(void);
void Send_Finished(void);
void Convert_Update_Parameters(void);
void Send_Powered_Wait_For_Update(void);
void Check_Device_Input (void);
void Clear_Device_Rcvr_Settings(void);
void Send_AdaptorReady_Message(void);
void Send_ConfiguringAdaptor_Message(void);
void Send_Resend_Message(void);
void Add_Char_to_GS1011_Buffer (char chr);
void copyStockSerialNumber(void);
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
  Send_powered_Message();
  Device_Update_Ready_for_Website_flag =0;
  SNAP_State = WAIT_FOR_UPDATE_STATE;
}
/*****************************************************************************/
/* State 1 - Wait_For_Update                                                 */
/*      can't do much without the update data                                */
/*****************************************************************************/
#define COMMAND_BYTE 0x03
#define STATUS_COMMAND_BYTE 0x04
void Wait_For_Update(void){
  if (Device_Rcvr_Complete_flag != 0){
      Check_Device_Input();
  }
  else {
    if (Device_Rcvr_Timeout == 1)
      InitializeDeviceBuffer();
   }     
}

/*****************************************************************************/
/* Check_Device_Input                                                  */
/*****************************************************************************/
void Check_Device_Input (void){
      if (Device_Receiver_Buffer[COMMAND_BYTE] == 'U')
        if (SNAP_State == WAIT_FOR_UPDATE_STATE){ /* only process update if waiting for update*/
            SNAP_State = PREOCESS_DEVICE_UPDATE_STATE;
        }
      else if (Device_Receiver_Buffer[COMMAND_BYTE] == 'S')
        /*if (Device_Receiver_Buffer[STATUS_COMMAND_BYTE] == 'S')    */
        InitializeDeviceBuffer();
}
/*****************************************************************************/
/* State 2 - Process_Update   WAIT_FOR_UPDATE_STATE                          */
/*      convert the update data to ASCII for the website                     */
/*****************************************************************************/
void Process_Received_Update(void){
  Process_Receiver_Device_Message();
  checksum_Okay = Check_Checksum_Device_Buffer();
  if (checksum_Okay == 0x55){
    CopySerialNumber();
    copyStockSerialNumber();   /* THIS IS ONLY SO WE DON'T INTERFERE WITH THE SHOW !!!!*/
    Make_Website_Update_from_Processing_Buffer();
    InitializeDeviceBuffer();
    SNAP_State = GET_GS1011_NETWORK_STATE;
  }
  else{                                 /*if checksum wrong ask for it again*/
   Send_Resend_Message();
    SNAP_State = WAIT_FOR_UPDATE_STATE;
  }
}
/*****************************************************************************/
/*****************************************************************************/
extern char TestingSerialNumber[];
void copyStockSerialNumber(void){
  char i;
  for (i = 0; i <= 19; i++){
    Device_Processing_Buffer[i] = TestingSerialNumber[i];
  }
}
/*****************************************************************************/
/*****        Send_powered_Message                                ****/
/*****        1st byte in Device processed buffer is the character count  ****/
/*****************************************************************************/
void Send_powered_Message(void){
  InitializeDeviceBuffer();
  CopyBufferDevice(Powered);
  Device_Xmit_Char_Count = Device_Xmit_Char_Count;
  Start_Device_Xmit ();
}
/*****************************************************************************/
/*****        Send_ConfiguringAdaptor_Message                                ****/
/*****        1st byte in Device processed buffer is the character count  ****/
/*****************************************************************************/
void Send_AdaptorReady_Message(void){
  InitializeDeviceBuffer();
  CopyBufferDevice(AdaptorReady);
  Device_Xmit_Char_Count = Device_Xmit_Char_Count;
  Start_Device_Xmit ();
}
/*****************************************************************************/
/*****        Send_ConfiguringAdaptor_Message                                ****/
/*****        1st byte in Device processed buffer is the character count  ****/
/*****************************************************************************/
void Send_ConfiguringAdaptor_Message(void){
  InitializeDeviceBuffer();
  CopyBufferDevice(ConfiguringAdaptor);
  Device_Xmit_Char_Count = Device_Xmit_Char_Count;
  Start_Device_Xmit ();
}
/*****************************************************************************/
/*****        Send_powered_Message                                ****/
/*****        1st byte in Device processed buffer is the character count  ****/
/*****************************************************************************/
void Send_ACK_Message(void){
  InitializeDeviceBuffer();
  CopyBufferDevice(ACKMessage);
  Device_Xmit_Char_Count = Device_Xmit_Char_Count;
  Start_Device_Xmit ();
}
/*****************************************************************************/
/*****        Send_Resend_Message                                ****/
/*****        1st byte in Device processed buffer is the character count  ****/
/*****************************************************************************/
void Send_Resend_Message(void){
  InitializeDeviceBuffer();
  CopyBufferDevice(ResendMessage);
  Device_Xmit_Char_Count = Device_Xmit_Char_Count;
  Start_Device_Xmit ();
}
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
/*****             Assemble_Process_Send_Device_website                   ****/
/*****************************************************************************/
void Assemble_Process_Send_Device_Website_Update(void){
char i;
  InitializeDeviceBuffer();
  InitializeDevice_Processing_Buffer();
  webptr = 0x00;
  Device_Processing_Buffer[webptr] = STX;
  webptr++;
  Device_Processing_Buffer[webptr] = Device_Update_Data_count + 1;
  webptr++;
  for (i=0;i<= Device_Update_Data_count; i++) {
    Device_Processing_Buffer[webptr] = Website_Update_Data_Buffer[i];
    webptr++;
    Checksum_Device_Buffer(Device_Processing_Buffer);
    Process_Xmit_Device_Message(Device_Processing_Buffer, Device_Xmit_Buffer);
  }
}
void InitializeDevice_Processing_Buffer(void){
char i;
  for (i=0; i< BFRSIZE; i++)
     Device_Processing_Buffer[i]=0x00;
}
/*****************************************************************************/
/*****             Checksum_Device_Buffer                                 ****/
/*****        1st byte in Device processed buffer is the character count  ****/
/*****                                                                    ****/
/*****  bytes from count to one before the checksum byte are checksumed    ****/
/*****************************************************************************/
void Checksum_Device_Buffer(char bufr[]){
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
    bufr[gtchr] = checksum;
    gtchr++;
    bufr[gtchr] = ETX;
  }
  else {
     /* error code here*/
  }
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
 int cntr;
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
  Processing_Byte_Count = xmtptr;
  xmtptr++;
 }
bufr2[xmtptr] =  ETX;
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
  (void)UART3->SR;
  Device_Rcvr_char = ((u8)UART3->DR);

  Device_Receiver_Buffer[Device_RX_InPtr] = Device_Rcvr_char;
  if (Device_Rcvr_char == 0x03){
     Device_Rcvr_EOM_Timer = 0;
     Device_Rcvr_Complete_flag = 1;
  }
  else {
  Device_Rcvr_EOM_Timer = 150;
  Device_RX_InPtr++;
}
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
for (rcvrbfrptr = 1; rcvrbfrptr <BFRSIZE_HALF; rcvrbfrptr++){
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
  Processing_Byte_Count = prcsbufrptr;
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

/**
  ******************************************************************************
  * UART 1 is the GS1011 UART
  * @file    stm8s_uart1.c
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    18-November-2011
  * @brief   This file contains all the functions for the UART1 peripheral.
  ******************************************************************************
    
  ******************************************************************************
  *                                                                            *
  *  This file contains the Initialization, and ISR routines for the GS1011    *
  *     The initialization sets the port to:                                   *
  *             9600 baud, 8 bits, no parity, 1 stop                           *
  *     The RX ISR gets the resposes from the GS1011                           *
  *     The TX ISR sends commands and device updates to the GS1011             *
  *                                                                            *
  *     The update data is formatted after it comes in from the device.        *
  *     The ACK message is sent if the status:good is returned from the GS1011 *
  *                                                                            *
  *                                                                            *
  *****************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm8s_uart1.h"

/* definitions ---------------------------------------------------------------*/
void InitGS1011UART(void);
void Handle_GS1011_State(void);
/*******************************   state machine handlers ************/
void GetNetworkStatusFromGS1011(void);
void Get_any_ResponseFromGS1011(char bufr[]);
void Reset_Network_Access(void);
void Send_Website_Update(void);
void Send_DNSLOOKUP_Message(void);
/*******************************   state machine handlers end ************/
void GS1011_Received_Data_Handler(void);
void Sending_GS1011_Data_Handler(void);
void Start_GS1011_Send(void);
void GetNetworkStatusFromGS1011 (void);
void SetupGS1011Association(void);
void SendUpdateToWebsite (void);
void GetResponseFromGS1011(void);
void GetNSTAT_ResponseFromGS1011(void);
void SendYouThereMessageToGS1011(void);
void Are_You_There_with_Response(void);
void InitializeGS1011Buffer(void);
void Initialize_GS011_Xmit_buffer(void);
/* external definitions ------------------------------------------------------*/
/* GS1011 DATA */
extern char SNAP_State;
extern char Send_Update_State;
extern int BFRSIZE;
extern int Found_String_At_Byte;
extern char EEWRTimout;
extern char GS1011_Rcvr_Timeout;
extern char GS1011_State;
extern char GS1011_State_State;
extern u8 GS1011_String_Found;
extern char GS1011_Received_Response_Flag;
extern unsigned char GS1011_Xmit_Char;
extern unsigned char GS1011_Xmit_Char_Count;
extern void Start_GS1011_Send(void);
extern char GS1011_Xmit_Pointer;
extern char GS1011_Xmit_Buffer[];
extern unsigned char GS1011_Xmit_Char_Count;
extern char CopyBuffer (char dest[], char srce[]);
extern void CopyBufferGS1011 (char srce[]);
extern void FillBuffer (char bufr[],char filchr, char cntr);
extern char GS1011_Rcvr_EOM_Timer;
extern char GS1011_Rcvr_Char;
extern u16 GS1011_Rcvr_InPtr;
extern u16 GS1011_Rcvr_OutPtr;
extern u16 GS1011_Rvcr_Count;
extern u16 GS1011_Rcvr_Pointer;
extern char website_IP_Address[];
extern char GS1011_Receiver_Buffer[];


/* external stock message definitions ----------------------------------------*/
extern char GetNSTATMessage[];
extern char DNSLOOKUP_Message[];
extern char WSTATECONNECTEDmsg[];
extern char OKAYmsg[];
extern char IPmsg[];
/*  Reset GS1011 messages */
extern char YouThereMessage[];
extern char ForceFactoryResetMessage;
extern char ClearW0Message;
extern char ClearW1Message;
extern char ClearY0Message;
extern char ResetAdaptorMessage;
extern char ShutOffEchoMessage;
extern char DisassociateMessage;
extern void FindGS1011Chars(char chrstrng[]);
/* setup GS1011 to connect to WIFI */
extern char SetInitialIPAddressMessage;
extern char SetWM2Message;
extern char SetNDHCPMessage;
extern char SetDHCPSRVRMessage;
extern char SetWEBPROVMessage;
extern char GetProfileMessage;

/*  sending update messages  */
extern char ConnectionType_CONF11_message[];
extern char SetConnectionType;
extern char SendKeepAliveMessage;
extern char OpenMarsConnectionHeader;   /*add ip Address*/
extern char SendtoWebsiteHeader;   /*add CID, and status*/

/* sending update state maching*/ 
void Get_Website_IP_address(void);
void Set_Mars_as_Website(void);
void Set_Keep_Alive(void);
void  HTTPOPEN_Get_CID(void);
void  Convert_update_and_Send(void);
void  Get_Website_Response_and_Respond(void);

void GetWebsite_ResponseFromGS1011(void);
/* external functions */
extern void CountGS1011Chars(void);
extern char CountChars(char s[]);
extern int Add_Char_to_Buffer (char *bufr,int ptr,char chr);
void copy_buffer_from_offset_to_terminator(char srcebufr[],char destbufr[], int ofst, char trm);
#define CR 0x0D
#define LF 0x0A
/*****************************************************************************/
/*****************************************************************************/
/*****        GS1011 State machine                                       *****/
/*****************************************************************************/
/*****************************************************************************/

/*****************************************************************************
 ****   GetNetworkStatusFromGS1011  -  subSTATE 0                         ****
 ****                                                                     ****
 ******************************************************************************/
void GetNetworkStatusFromGS1011 (void){
  
  if (GS1011_Received_Response_Flag == 0x00){
    InitializeGS1011Buffer();
    CopyBufferGS1011(GetNSTATMessage);  
    Start_GS1011_Send();                                /* kickstart the xmitter*/
    GS1011_Received_Response_Flag = 0x01;
    GetNSTAT_ResponseFromGS1011();
   }
  else {
    GetNSTAT_ResponseFromGS1011();
  }
  
}
void GetNSTAT_ResponseFromGS1011(void){
  if (GS1011_Rcvr_Timeout == 1) {
    GS1011_Received_Response_Flag = 0x00;
    FindGS1011Chars(OKAYmsg);
      
    if (GS1011_String_Found == 1){
        FindGS1011Chars(WSTATECONNECTEDmsg);
        if (GS1011_String_Found == 1){
          SNAP_State = 5;
          Send_Update_State = 0;
          }
        else 
           SNAP_State = 4;
    }
  }
}

/*****************************************************************************
 ****   Reset_Network_Access  -  STATE 4                                  ****
 ****                                                                     ****
 **** sends an AT\r\n and waits for an OK or ERROR                        ****
 ****                                                                     ****
 ******************************************************************************/
void Reset_Network_Access(void){
}

/*****************************************************************************
 ****   Send_Website_Update  -  STATE 5                                   ****
 ****                                                                     ****
 **** this has 10 substates associated with it                            ****
 ****  uses Send_Update_State                                             **** 
 ****  STATE0 DNSLOOKUP                                                   ****
 ****  STATE1 SET MARS WEBSITE                                            ****
 ****  STATE2 KEEP ALIVE                                                  ****
 ****  STATE3 OPEN HTTP get CID                                           ****
 ****  STATE4 CONVERT UPDATE and SEND                                     ****
 ****  STATE5 WAIT FOR RESPONSE (send status to device)        ****
 ******************************************************************************/

void Send_Website_Update(void){
  switch (Send_Update_State) {
  case 0:
      Get_Website_IP_address();
      break;
  case 1:
      Set_Mars_as_Website();
      break;
  case 2:
      Set_Keep_Alive();
      break;
  case 3:
      HTTPOPEN_Get_CID();
      break;
  case 4:
      Convert_update_and_Send();
      break;
  case 5:
      Get_Website_Response_and_Respond();
      break;
  }
 }
/*****************************************************************************
 ****   Get_Website_IP_address  -  subSTATE 0                             ****
 ****                                                                     ****
 ****  gets the website ip address                                        ****
 ******************************************************************************/
void Get_Website_IP_address(void){
  Send_DNSLOOKUP_Message();
while (GS1011_Received_Response_Flag == 0x01)
{
Get_any_ResponseFromGS1011(IPmsg);
}
if (GS1011_String_Found == 1){
  FillBuffer(website_IP_Address,0x00,15);
  Found_String_At_Byte += 3;
  copy_buffer_from_offset_to_terminator(GS1011_Receiver_Buffer, website_IP_Address, Found_String_At_Byte, 0x0d);
  }
  GS1011_Received_Response_Flag == 0x00;
  Send_Update_State = 1;
} 
void Send_DNSLOOKUP_Message(void){
  if (GS1011_Received_Response_Flag == 0x00){
    InitializeGS1011Buffer();
    CopyBufferGS1011(DNSLOOKUP_Message);  
    Start_GS1011_Send();                                /* kickstart the xmitter*/
    GS1011_Received_Response_Flag = 0x01;
    Get_any_ResponseFromGS1011(IPmsg);
   }
  else 
    Get_any_ResponseFromGS1011(IPmsg);
  }
  
  
/*****************************************************************************
 ****   Set_Mars_as_Website  -  subSTATE 1                             ****
 ****                                                                     ****
 ****  gets the website ip address                                        ****
 ******************************************************************************/
  void Set_Mars_as_Website(void){
   }  
/*****************************************************************************
 ****   Set_Keep_Alive  -  subSTATE 2                                ****
 ****                                                                     ****
 ****  GS1011 responds okay                                               ****
 *****************************************************************************/
  void Set_Keep_Alive(void){
  }
  
/*****************************************************************************
 ****   Set_Mars_as_Website  -  subSTATE 3                                ****
 ****                                                                     ****
 ****  GS1011 responds okay                                               ****
 ******************************************************************************/
  void  HTTPOPEN_Get_CID(void){
  }
  
/*****************************************************************************
 ****   Set_Mars_as_Website  -  subSTATE 4                             ****
 ****                                                                     ****
 ****  GS1011 responds okay                                               ****
 ******************************************************************************/
  void  Convert_update_and_Send(void){
  }
  
/*****************************************************************************
 ****   Set_Mars_as_Website  -  subSTATE 5                             ****
 ****                                                                     ****
 ****  gets the website ip address                                        ****
 ******************************************************************************/
  void  Get_Website_Response_and_Respond(void){
  }
 
/*****************************************************************************/
/*****************************************************************************/
/****                       normal routines                                ***/
/*****************************************************************************/
/*****************************************************************************/
void SendYouThereMessageToGS1011(void){
CopyBufferGS1011(YouThereMessage);                                 
Start_GS1011_Send();                                /* kickstart the xmitter*/
if (GS1011_Rcvr_Timeout == 1)
  GetResponseFromGS1011();
}
/*****************************************************************************/
/*****************************************************************************/
/****                        ISR routines                                  ***/
/****                 UART DATA HANDLING ROUTINES                          ***/
/*****************************************************************************/
/*****************************************************************************/

/*****************************************************************************
  *   GetResponseFromGS1011                                                  *
  *                                                                          *
  *   Checks the input from the GS1011 for OK or ERROR                       *                                                  *
  *       and returns the status to the calling routine                      *
  *****************************************************************************/
void GetResponseFromGS1011(void){
  if (GS1011_Rcvr_Timeout == 1){
    GS1011_Received_Response_Flag = 0x00;
      FindGS1011Chars(OKAYmsg);
   }
}
void Get_any_ResponseFromGS1011(char bufr[]){
  if (GS1011_Rcvr_Timeout == 1) {
    GS1011_Received_Response_Flag = 0x00;
    FindGS1011Chars(OKAYmsg);
      
    if (GS1011_String_Found == 1){
        FindGS1011Chars(bufr);
        if (GS1011_String_Found == 1)
          GS1011_String_Found == 1;
        else 
           GS1011_String_Found == 0;
    }
  }
}

/*****************************************************************************
  *   Handle_GS1011_Received_Data                                              *
  *                                                                            *
  *   Get a character from the data register.                                  *
  *         Set a timer for x seconds(timer value is dependent upon command.   *
  *                                                                            *
  *   If the timer expires check if the response matches the command.          *
  *                                                                            *
  *   If the command was an update, and the response was status:good then      *
  *             Send the stock ACK message to the device                       *
  *                                                                            *
  *****************************************************************************/
void GS1011_Received_Data_Handler(void){
 GS1011_Rcvr_Char = UART1->DR;
 GS1011_Receiver_Buffer[GS1011_Rcvr_InPtr] = GS1011_Rcvr_Char;
 GS1011_Rcvr_InPtr++;
 GS1011_Rvcr_Count++;
 GS1011_Rcvr_Pointer++;
 GS1011_Rcvr_EOM_Timer = 250;
}
 /******************************************************************************
  *   Start_GS1011_Send                                                        *
  *                                                                            *
  *   This routine is passed a pointer to a buffer to send to the GS1011       *
  *                                                                            *
  *   Gets the first character and sets the buffer count and pointer.          *
  *   It sends the first character then the interupts take over.               *
  *                                                                            *
  *****************************************************************************/
void Start_GS1011_Send(void){
  GS1011_Xmit_Pointer = 0;
  GS1011_Xmit_Char = GS1011_Xmit_Buffer[GS1011_Xmit_Pointer];
 GS1011_Rcvr_Timeout = 0;
   GS1011_Xmit_Pointer++;
  GS1011_Xmit_Char_Count--;
  UART1->DR = GS1011_Xmit_Char;
  UART1->CR2 |= ((uint8_t)UART1_CR2_TCIEN | UART1_CR2_TCIEN | UART1_CR2_TIEN);
  }
   
  /*****************************************************************************
  *   Handle_Sending_GS1011_Data                                               *
  *                                                                            *
  *   Send a character to the data register.                                   *
  *         Continue till count = 00                                           *
  *                                                                            *
  *****************************************************************************/
void Sending_GS1011_Data_Handler(void){
    if (UART1->SR &= UART3_FLAG_TXE){

  GS1011_Xmit_Char = GS1011_Xmit_Buffer[GS1011_Xmit_Pointer];
  if (GS1011_Xmit_Char_Count!=0){
      UART1->DR= GS1011_Xmit_Char;
      GS1011_Xmit_Pointer++;
      GS1011_Xmit_Char_Count--;}
  else UART1->CR2 &= (uint8_t)~(UART1_CR2_TCIEN | UART1_CR2_TIEN);     
 }
}
void Initialize_GS011_Xmit_buffer(void){
  FillBuffer (GS1011_Xmit_Buffer,0x00, BFRSIZE);
  GS1011_Xmit_Pointer = 0;
  GS1011_Xmit_Char_Count = 0;
}
/*****************************************************************************/
/*****************************************************************************/
/*****        GS1011 Initialize/DeInit                                   *****/
/*****************************************************************************/
/*****************************************************************************/
void InitGS1011UART(void){
  UART1_Init(9600,  UART1_WORDLENGTH_8D, UART1_STOPBITS_1,
  UART1_PARITY_NO, UART1_SYNCMODE_CLOCK_DISABLE,  UART1_MODE_TXRX_ENABLE);
}

/*****************************************************************************/
/*****                          UART1_DeInit                              ****/
/***** Clear the Idle Line Detected bit in the status rerister by a read  ****/
/***** to the UART3_SR register followed by a Read to the UART1_DR register **/
/*****************************************************************************/
void UART1_DeInit(void)
{
    /* Clear the Idle Line Detected bit in the status rerister by a read
       to the UART1_SR register followed by a Read to the UART1_DR register */
    (void)UART1->SR;
    (void)UART1->DR;

    UART1->BRR2 = UART1_BRR2_RESET_VALUE;  /* Set UART1_BRR2 to reset value 0x00 */
    UART1->BRR1 = UART1_BRR1_RESET_VALUE;  /* Set UART1_BRR1 to reset value 0x00 */

    UART1->CR1 = UART1_CR1_RESET_VALUE;  /* Set UART1_CR1 to reset value 0x00 */
    UART1->CR2 = UART1_CR2_RESET_VALUE;  /* Set UART1_CR2 to reset value 0x00 */
    UART1->CR3 = UART1_CR3_RESET_VALUE;  /* Set UART1_CR3 to reset value 0x00 */
    UART1->CR4 = UART1_CR4_RESET_VALUE;  /* Set UART1_CR4 to reset value 0x00 */
    UART1->CR5 = UART1_CR5_RESET_VALUE;  /* Set UART1_CR5 to reset value 0x00 */

    UART1->GTR = UART1_GTR_RESET_VALUE;
    UART1->PSCR = UART1_PSCR_RESET_VALUE;
}


/*****************************************************************************/
/*****                          UART1_Init                                ****/
/*****************************************************************************/
/**
  * @brief  Initializes the UART1 according to the specified parameters.
  * @note   Configure in Push Pull or Open Drain mode the Tx pin by setting the
  *         correct I/O Port register according the product package and line
  *         configuration
  * @param  BaudRate: The baudrate.
  * @param  WordLength : This parameter can be any of the 
  *         @ref UART1_WordLength_TypeDef enumeration.
  * @param  StopBits: This parameter can be any of the 
  *         @ref UART1_StopBits_TypeDef enumeration.
  * @param  Parity: This parameter can be any of the 
  *         @ref UART1_Parity_TypeDef enumeration.
  * @param  SyncMode: This parameter can be any of the 
  *         @ref UART1_SyncMode_TypeDef values.
  * @param  Mode: This parameter can be any of the @ref UART1_Mode_TypeDef values
  * @retval None
  */
void UART1_Init(uint32_t BaudRate, UART1_WordLength_TypeDef WordLength, 
                UART1_StopBits_TypeDef StopBits, UART1_Parity_TypeDef Parity, 
                UART1_SyncMode_TypeDef SyncMode, UART1_Mode_TypeDef Mode)
{
    uint32_t BaudRate_Mantissa = 0, BaudRate_Mantissa100 = 0;

    /* Check the parameters */
    assert_param(IS_UART1_BAUDRATE_OK(BaudRate));
    assert_param(IS_UART1_WORDLENGTH_OK(WordLength));
    assert_param(IS_UART1_STOPBITS_OK(StopBits));
    assert_param(IS_UART1_PARITY_OK(Parity));
    assert_param(IS_UART1_MODE_OK((uint8_t)Mode));
    assert_param(IS_UART1_SYNCMODE_OK((uint8_t)SyncMode));

    /* Clear the word length bit */
    UART1->CR1 &= (uint8_t)(~UART1_CR1_M);  
    
     /* Set the word length bit according to UART1_WordLength value */
    UART1->CR1 |= (uint8_t)WordLength;

    /* Clear the STOP bits */
    UART1->CR3 &= (uint8_t)(~UART1_CR3_STOP);  
    /* Set the STOP bits number according to UART1_StopBits value  */
    UART1->CR3 |= (uint8_t)StopBits;  

    /* Clear the Parity Control bit */
    UART1->CR1 &= (uint8_t)(~(UART1_CR1_PCEN | UART1_CR1_PS  ));  
    /* Set the Parity Control bit to UART1_Parity value */
    UART1->CR1 |= (uint8_t)Parity;  

    /* Clear the LSB mantissa of UART1DIV  */
    UART1->BRR1 &= (uint8_t)(~UART1_BRR1_DIVM);  
    /* Clear the MSB mantissa of UART1DIV  */
    UART1->BRR2 &= (uint8_t)(~UART1_BRR2_DIVM);  
    /* Clear the Fraction bits of UART1DIV */
    UART1->BRR2 &= (uint8_t)(~UART1_BRR2_DIVF);  

    /* Set the UART1 BaudRates in BRR1 and BRR2 registers according to UART1_BaudRate value */
    BaudRate_Mantissa    = ((uint32_t)CLK_GetClockFreq() / (BaudRate << 4));
    BaudRate_Mantissa100 = (((uint32_t)CLK_GetClockFreq() * 100) / (BaudRate << 4));
    /* Set the fraction of UART1DIV  */
    UART1->BRR2 |= (uint8_t)((uint8_t)(((BaudRate_Mantissa100 - (BaudRate_Mantissa * 100)) << 4) / 100) & (uint8_t)0x0F); 
    /* Set the MSB mantissa of UART1DIV  */
    UART1->BRR2 |= (uint8_t)((BaudRate_Mantissa >> 4) & (uint8_t)0xF0); 
    /* Set the LSB mantissa of UART1DIV  */
    UART1->BRR1 |= (uint8_t)BaudRate_Mantissa;           

    /* Disable the Transmitter and Receiver before seting the LBCL, CPOL and CPHA bits */
    UART1->CR2 &= (uint8_t)~(UART1_CR2_TEN | UART1_CR2_REN); 
    /* Clear the Clock Polarity, lock Phase, Last Bit Clock pulse */
    UART1->CR3 &= (uint8_t)~(UART1_CR3_CPOL | UART1_CR3_CPHA | UART1_CR3_LBCL); 
    /* Set the Clock Polarity, lock Phase, Last Bit Clock pulse */
    UART1->CR3 |= (uint8_t)((uint8_t)SyncMode & (uint8_t)(UART1_CR3_CPOL | 
                                              UART1_CR3_CPHA | UART1_CR3_LBCL));  

    if ((uint8_t)(Mode & UART1_MODE_TX_ENABLE))
    {
        /* Set the Transmitter Enable bit */
        UART1->CR2 |= (uint8_t)UART1_CR2_TEN;  
    }
    else
    {
        /* Clear the Transmitter Disable bit */
        UART1->CR2 &= (uint8_t)(~UART1_CR2_TEN);  
    }
    if ((uint8_t)(Mode & UART1_MODE_RX_ENABLE))
    {
        /* Set the Receiver Enable bit */
        GS1011_Rcvr_Char = UART1->DR;
        GS1011_Rcvr_Char = UART1->DR;
        UART1->CR2 |= ((uint8_t)UART1_CR2_REN | UART1_CR2_RIEN);
    }
    else
    {
        /* Clear the Receiver Disable bit */
        UART1->CR2 &= (uint8_t)(~UART1_CR2_REN);  
    }
    /* Set the Clock Enable bit, lock Polarity, lock Phase and Last Bit Clock 
           pulse bits according to UART1_Mode value */
    if ((uint8_t)(SyncMode & UART1_SYNCMODE_CLOCK_DISABLE))
    {
        /* Clear the Clock Enable bit */
        UART1->CR3 &= (uint8_t)(~UART1_CR3_CKEN); 
    }
    else
    {
        UART1->CR3 |= (uint8_t)((uint8_t)SyncMode & UART1_CR3_CKEN);
    }
}


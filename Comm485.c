
/* Includes ------------------------------------------------------------------*/
#include "Comm485.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SrcAddr 0x11    /*THIS IS MY ADDRESS (CONTROLLER)*/
/*timers before some communications must be done just to keep the different modules working*/
#define KeepAliveSG         DLY100_2_SEC
#define KeepAliveAroma      DLY100_2_SEC
#define KeepAliveSound      DLY100_4_SEC
#define KeepAliveChroma     DLY100_4_SEC
#define KeepAliveRemoteWL   DLY100_1_SEC
#define KeepAliveRemoteW    DLY100_2_SEC
#define KeepAliveRemoteINT  DLY100_2_SEC

const u8 PeriphFailLimit = 0x03;   /*FAIL LIMIT BEFORE REMOVING FROM ACTIVE LIST*/
/*these are the numbers to send the aroma for each of the 14 different settings on the screen (0-13)*/
const u8 AromaValues[14] = {0, 20, 18, 17, 15, 14, 12, 10, 9, 7, 6, 4, 3, 1};

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/*MasCommReqBits/MasComBsyBits: (see defines in "Comm485.h"                   */
/*B0= Find Modules        B1= SGs set temp          B2= SGs ON/OFF            */
/*B3= SGs Preheat         B4= SGs get status        B5= Aroma ON/OFF          */
/*B6= Aroma setup         B7= Aroma Status          B8= Chroma ON/OFF         */
/*B9= Chroma setup        B10= Chroma set Canned    B11=Chroma get info       */
/*B12= get Chroma Status  B13= Sound setup          B14= Sound set volume     */
/*B15= Sound skip song    B16= Sound ON/OFF         B17= Sound get status     */
/*B18= WLRemote status    B19= Wired remote status  B20= Internet Remote stat.*/
/*B31= clock Cal                                                              */
/*----------------------------------------------------------------------------*/
u32 MasCommReqBits;
u32 MasCommBsyBits;
vu8 RxBuffer1[RxBufferSize1 + 1];
u8  ChkClkFailCtr;
u8  SGTemperatureCmd; /*Commanded temperature in deg F for the steam generator*/
u8  RemoteButtonPressed;    /* 1=start user 2; 2=start user 1; 3=stop; 4=LEDWhiteOn toggle*/
u8  ReqClockCal;
u8  ChkClkState;
u8  FindModState;
u8  ReqModuleKA;    /* 1 to run keep alive routine 0 on exit */
u8  ReqFindModules; /*SET TO 1 TO REQUEST FIND. WHEN DONE WILL GO TO 0*/
u8  Aroma1Val;      /*Raw Value 0-12 value for user1 aroma*/
u8  Aroma2Val;      /*Raw Value 0-12 value for user2 aroma*/
u8  Aroma3Val;      /*Raw Value 0-12 value for user3 aroma*/
u8  Pump1Val;       /*Raw Value 0=off; 1=Pump1; 2=Pump2 for user 1*/
u8  Pump2Val;       /*Raw Value 0=off; 1=Pump1; 2=Pump2 for user 2*/
u8  Pump3Val;       /*Raw Value 0=off; 1=Pump1; 2=Pump2 for user 3*/
u8  Chroma1Color;   /*Raw Value 0-12 value for user1 Color Value*/
u8  Chroma2Color;   /*Raw Value 0-12 value for user2 Color Value*/
u8  Chroma3Color;   /*Raw Value 0-12 value for user3 Color Value*/
u8  Chroma1Int;     /*Raw Value 0-12 value for user1 Chroma intensity*/
u8  Chroma2Int;     /*Raw Value 0-12 value for user2 Chroma intensity*/
u8  Chroma3Int;     /*Raw Value 0-12 value for user3 Chroma intensity*/
u8  Vol1Val;        /*Raw Value 0-12 value for volume user 1*/
u8  Vol2Val;        /*Raw Value 0-12 value for volume user 2*/
u8  Vol3Val;        /*Raw Value 0-12 value for volume user 3*/
u8  Bass1Val;       /*Raw Value 0-12 value for Bass user 1*/
u8  Bass2Val;       /*Raw Value 0-12 value for Bass user 2*/
u8  Bass3Val;       /*Raw Value 0-12 value for Bass user 3*/
u8  Treb1Val;       /*Raw Value 0-12 value for Bass user 1*/
u8  Treb2Val;       /*Raw Value 0-12 value for Bass user 2*/
u8  Treb3Val;       /*Raw Value 0-12 value for Bass user 3*/
u8  InGain1Val;     /*Raw Value 0-12 value for Input Source user 1*/
u8  InGain2Val;     /*Raw Value 0-12 value for Input Source 2*/
u8  InGain3Val;     /*Raw Value 0-12 value for Input Source 3*/
u8  InSrc1Val;      /*Raw Value Sound Source user 1, Src1(A)=1, Scr2(B)=3, IPOD = 3, OFF=0*/
u8  InSrc2Val;      /*Raw Value Sound Source user 2, Src1=1, Scr2=3, IPOD = 3, OFF=0*/
u8  InSrc3Val;      /*Raw Value Sound Source user 3, Src1=1, Scr2=3, IPOD = 3, OFF=0*/
u8  Bal1Val;        /*Raw Value 0-12 value for Balance user 1*/
u8  Bal2Val;        /*Raw Value 0-12 value for Balance user 2*/
u8  Bal3Val;        /*Raw Value 0-12 value for Balance user 3*/
u8  Temp1Val;       /*Raw Value 0-12 value for Temp user 1*/
u8  Temp2Val;       /*Raw Value 0-12 value for Temp user 2*/
u8  Temp3Val;       /*Raw Value 0-12 value for Temp user 3*/
u8  Temp1Val_C;     /*SetTemp Deg F for Temp user 1 Local uses only not saved*/
u8  Temp2Val_C;     /*SetTemp Deg F for Temp user 2 Local uses only not saved*/
u8  Temp3Val_C;     /*SetTemp Deg F for Temp user 3 Local uses only not saved*/
u8  CycTime1Val;    /*Cycle Time value 0-60 for Cycle Time user 1*/
u8  CycTime2Val;    /*Cycle Time value 0-60 for Cycle Time user 2*/
u8  CycTime3Val;    /*Cycle Time value 0-60 for Cycle Time user 3*/
u8  RxChksumFail;   /*rx checksum failure if = 1*/
u8  SG_OnOff;       /*ON OFF FOR STEAM GENERATORS*/
u8  Sound_OnOff;    /*ON OFF FOR SOUND 0=off*/
u8  ColorBrightMax;
u8  PreHeatOnOff;   /*used during request for communications to destinguish beteen
                    on and off control. 1=on 0= off for all modules*/
u8  Payload485[8];  /*payload data buffer for sending the 485 packet,
                   1st byte is length*/
u8  Tx485Buf[24];   /*data to send*/
u8  CommBsyState;
u8  Tx485Flag;      /*3 to start TxRx. 2 when Tx done. 1 when Rx done.
                    0 when rx parse is done*/
/*these values are the ones to be sent to the peripherials. not the user based values*/
u8  Volume;         /*Sound volume 0-47*/
u8  Bass;           /*Sound Bass 0-15*/
u8  Treble;         /*Sound Treble 0-15*/
u8  Balance;        /*Sound Balance 0-47*/
u8  InputGain;      /*Sound Input gain 0-11*/
u8  InputPort;      /*Sound Input select 0=MiniJack, 1=RCA jacks, 2=IPOD*/
u8  ColorIndex;     /*COLOR INDEX TO SEND TO CHROMA CONTROLLER 0-8*/
u8  Chroma_OnOff;   /*ON OFF FOR CHROMA */
u8  RemoteSrc;      /*IS 1 IF FROM BUTTON PRESSED FROM WIRELESS OR EXT REMOTE PERIPHERIALS*/
/*************************************************************************************/
u8  AudAInLevel;    /*Input level for Audio A (MiniJack)*/
u8  AudBInLevel;    /*Input level for Audio B (RCA Jacks)*/
u8  IPODInLevel;    /*Input level for IPOD */

/*STEAM GENERATOR 1 STUFF*/
u8 SG1Status;
u8 SG1Error;
u8 SG1MajFirmNum;
u8 SG1MinFirmNum;
u8 SG1_FC;


/*STEAM GENERATOR 2 STUFF*/
u8 SG2Status;
u8 SG2Error;
u8 SG2MajFirmNum;
u8 SG2MinFirmNum;
u8 SG2_FC;

/*STEAM GENERATOR 3 STUFF*/
u8 SG3Status;
u8 SG3Error;
u8 SG3MajFirmNum;
u8 SG3MinFirmNum;
u8 SG3_FC;

/*STEAM GENERATOR 4 STUFF*/
u8 SG4Status;
u8 SG4Error;
u8 SG4MajFirmNum;
u8 SG4MinFirmNum;
u8 SG4_FC;

/*AROMA STUFF*/
u8 Aroma1Status;
u8 Aroma1MajFirmNum;
u8 Aroma1MinFirmNum;
u8 AROMA_FC;           /*FAIL COUNTER*/

/*CHROMA STUFF*/
u8 CHR1Status;
u8 CHR1NumColors;
u8 CHR1MajFirmNum;
u8 CHR1MinFirmNum;
u8 CHR1_FC;           /*FAIL COUNTER*/

/*SOUND STUFF*/
u8 SOUND1Status;
u8 SOUND1MajFirmNum;
u8 SOUND1MinFirmNum;
u8 SOUND_FC;           /*FAIL COUNTER*/

/*WIRELESS REMOTE STUFF*/
u8 REMOTE1Status;
u8 REMOTE1MajFirmNum;
u8 REMOTE1MinFirmNum;
u8 REMOTE1_FC;          /*FAIL COUNTER*/

/*WIRED REMOTE STUFF*/
u8 REMOTE2Status;
u8 REMOTE2MajFirmNum;
u8 REMOTE2MinFirmNum;
u8 REMOTE2_FC;          /*FAIL COUNTER*/

/*EXT REMOTE STUFF*/
u8 REMOTE3Status;
u8 REMOTE3MajFirmNum;
u8 REMOTE3MinFirmNum;
u8 REMOTE3_FC;          /*FAIL COUNTER*/

/*CLOCK CALIBRATION STUFF*/
u8 ClkCalStatus;    /*status*/
u8 ClkCalData;      /*cal register*/
u8 ClkCalDay;       /*day*/
u8 ClkCalMonth;     /*month*/
u8 ClkCalYear;      /*year*/

u16 BitSavVar;  /* Bit 0: IMOnOff(0 = off; 1 = on)*/
                    /* Bit 1: TextIconMode(0 = Text; 1 = ICONS)*/
                    /* Bit 2: TempScale( 0 = Deg F; 1 = Deg C)*/
                    /* Bit 3: ClockOn( 0 = Clock off; 1 = Clock On)*/
                    /* Bit 4: Clock display 00:00 =1*/

/******************************************************************************/
/* ActivePeriph:                                                              */
/*    B0 = Steam Gen #1       B1 = Steam Gen #2        B2 = Steam Gen #3      */
/*    B3 = Steam Gen #4       B4 = Sound module        B5 = Chroma module     */
/*    B6 = Aroma              B7 = Wireless Remote     B8 = Wired Remote(3B)  */
/*    B9 = Internet Remote    B10 = Wired Remote(4B)                          */
/******************************************************************************/
  u16 ActivePeriph;
  u8  ActiveSteamGens;  /*uses only bits 0-3 as above. to see what is active at start*/
  u16 ActivePeriphWork;
  
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
u8 Test1;
u8 Test2;
u8 Test3;
u8 Test4;

/******************************************************************************/
/*Function to Send data on the RS485 line.                                    */
/*INPUTS:                                                                     */
/*        Dest = byte for the destination address.                            */
/*        Command = byte for the command number to the particular address.    */
/*        Control = byte used only for on/off commands 1=on,k 0=off           */
/*        "Payload485 will be setup by this routine                           */
/*Output: Tx485Buf will be filled with the Data to be transmitted.            */
/*        Initially Tx485Flag = 3; when all is done, this                     */
/*        flag will go to 0.                                                  */
/******************************************************************************/
void Send485(u8 Dest, u8 Command, u8 Control){
u16 Cksum;
u8  PacketLen;
u8  LocalByte, i, j;

  Tx485Flag = 3;
  
  /*CLEAR THE RX BUFFER FROM PREVIOUS DATA*/
  for (i=0; i < RxBufferSize1; i++){
    RxBuffer1[i] = 0xFF;
  }
  SetPayload(Command,Control, Dest);    /*SETUP THE PAYLOAD BUFFER*/

  /*STEP 1 IS TO COMPUTE LENGTH AND THE CHECKSUM BYTE*/
  Cksum = Dest + Command + SrcAddr; /*get partial checksum on known parts*/
  PacketLen = 5;  /*this accounts for the Dest addr, src Addr, Cksum Command and Length*/
  LocalByte = Payload485[0];     /*GET THE LENGTH OF THE PAYLOAD*/
  if ((LocalByte > 0) && (LocalByte < sizeof Payload485)){  /*here the payload data is in bounds*/
    PacketLen += LocalByte;     /*ADDIN THE LENGTH OF THE PAYLOAD*/
    for (i = 0; i < LocalByte; i++){
      Cksum += Payload485[ i + 1];  /*ADDIN THE BYTES OF THE PAYLOAD TO THE CHECKSUM*/
    }
  }
  Cksum += PacketLen;   /*ADD IN LENGTH*/
  Cksum = (0xFF55 - Cksum) & 0x00FF;  /*Cksum to send*/
  /*STEP 2 IS TO SETUP THE SEND BUFFER *Tx485Buf*/
  i = 0;      /*INITIALIZE THE POINTER*/
  Tx485Buf[i] = STX;   /*STX BYTE*/
  i++;
  i = FillTxBuf(i, Dest);   /*Insert the dest address*/
  i = FillTxBuf(i,SrcAddr);   /*Insert the source address*/  
  i = FillTxBuf(i,PacketLen);   /*Insert the Packet length*/  
  i = FillTxBuf(i,Command);   /*Insert the Command number*/
  /*NOW PROCESS AND STORE THE PAYLOAD IF ANY*/
  if (Payload485[0]){       /*if the payload is not zero add the data*/
    for (j = 0; j < Payload485[0]; j++){
      i = FillTxBuf(i,Payload485[j +1]);   /*Insert the byte*/
    }
  }
  /*NOW ADD THE CHECKSUM*/
  i = FillTxBuf(i,Cksum);   /*Insert the Checksum*/
  /*FINSIH UP WITH THE ETX*/
  Tx485Buf[i] = ETX;   /*ETX BYTE*/
  Tx485Indx = 0;      /*START FROM BEGINNING OF BUFFER*/
  GPIOC->BSRR = 0x00000400; /*SET TRANSCEIVER DIRECTION TO TX*/
  USART1->CR1 |= 0x0040; /*TURN ON THE TRANSMIT COMPLETE AND TXE INTERRUPT*/
}

/******************************************************************************/
/*Function to do the ESC sequence checking and put data into the Tx485Buf     */
/*INPUTS:                                                                     */
/*        BufOffset = byte index into the Tx485Buf to fill.                   */
/*        Dat = Data byte to process                                          */
/*Output: Return the next offset into the buffer.                             */
/******************************************************************************/
u8 FillTxBuf( u8 BufOffset, u8 Dat){
  if ((BufOffset + 2) > (sizeof Tx485Buf)){
    return BufOffset;             /*DO NOT ALLOW BUFFER OVERRUN*/
  }
  if ((Dat == DLE) | (Dat == STX) | (Dat == ETX)){
    Dat ^= 0xFF;  /*COMPLIMENT THE DATA*/
    Tx485Buf[BufOffset] = DLE;   /*INSERT THE DLE CODE*/
    BufOffset++;
  }
  Tx485Buf[BufOffset] = Dat;    /*INSERT THE DATA IT MAY BE COMPLIMENTED*/
  BufOffset++;                  /*SET FOR NEXT*/
  return BufOffset;
}

/******************************************************************************/
/*Function to parse the received RS-485 data. if I get here the tx and rx     */
/*  processes have completed sucessfully, indicated by the Tx485Flag being = 1*/
/*INPUTS: RxBuffer1 has data. Tx485Flag is =1(Rx has been completed.          */
/*Output: Tx485Flag = 0 if sucessful. if not sucessful the Tx485Flag will     */
/*    not be updated but the RxIntTO will be set to zero. Fixed data put      */
/*    into RxBuffer1 without the STX AND ETX characters.                      */
/******************************************************************************/
void ParseRxData(void){
  u8 SrcIndx;
  u8 DestIndx;
  u8 ChksumVal;
  if (Tx485Flag == 1){        /*ONLY DO THIS IF EVERYTHING ELSE IS OK*/
    ChksumVal = 0;
    SrcIndx = 0;
    DestIndx = 0;             /*SETUP FOR READING AND WRITING TO BUFFERS*/
    if (RxBuffer1[SrcIndx++] == STX){   /*MAKE SURE THE 1ST CHAR IS A STX*/
      while (RxBuffer1[SrcIndx] != ETX){
       if(RxBuffer1[SrcIndx] == DLE){    /*IF ESCAPEING HAS HAPPENED*/
          SrcIndx++;
          RxBuffer1[DestIndx] = RxBuffer1[SrcIndx] ^ 0xFF; /*STORE THE LOGICAL NOT OF THE DATA*/
        }
        else{
          RxBuffer1[DestIndx] = RxBuffer1[SrcIndx]; /*JUST STORE REGULAR DATA*/
        }
        ChksumVal += RxBuffer1[DestIndx];
        SrcIndx++;
        DestIndx++;
        if (DestIndx >= RxBufferSize1){
          ChksumVal = 0;    /*force a bad checksum if I cant find the ETX*/
          break;
        }
      }
    }
    if (ChksumVal != 0x55){ /*this is a failure all chksums must be 0x55*/
      RxIntTO = 0;
      RxChksumFail = 1;
    }
    else{
      RxChksumFail = 0;   /*if OK*/
    }
   Tx485Flag = 0;  
  }
}


/******************************************************************************/
/*Function to set the payload buffer                                          */
/*INPUTS: Command number = command number from document                       */
/*        Dest = Destination address                                          */
/*        Control value used for on/off commands 1=On 0=Off                   */
/*        0 or 1.                                                             */
/*Output: fills the "Payload485" buffer with the correct values               */
/******************************************************************************/
void SetPayload(u8 Command, u8 Control, u8 Dest){ /*SETUP THE PAYLOAD BUFFER*/
  u16 Local;
  Payload485[0] = 0;    /*default value count of 0*/
  switch (Command){
    case (0x22):{       /*SET STEAM GEN PREHEAT ON/OFF*/
      Payload485[0] = 1;  /*count*/
      Payload485[1] = Control;  /*on or off command*/
      break;
    }
    case 0x21:{         /*SET THE POWER ON/OFF*/
      Payload485[0] = 1;  /*count*/
      Payload485[1] = Control;  /*on or off command*/
      break;
    }
    case 0x20:{           /*SET STEAM TEMPERATURE*/
      Payload485[0] = 7;  /*count*/
      Payload485[1] = SGTemperatureCmd;  /*Temp Command Deg F*/
      Payload485[2] = (u8)((TcountsCx16 >> 8) & 0x00FF); /*msb of read temp in binary * 16*/
      Payload485[3] = (u8)(TcountsCx16 & 0x00FF); /*lsb of read temp in binary * 16*/
      Payload485[4] = (u8)((TcountsCx16 >> 8) & 0x00FF); /*msb of read temp in binary * 16*/
      Payload485[5] = (u8)(TcountsCx16 & 0x00FF); /*lsb of read temp in binary * 16*/
      Payload485[6] = 0;
      Payload485[7] = 0;
      break;
    }
    case 0x40:{           /*SET AROMA*/
      Payload485[0] = 2;  /*count*/
      Local = GetUserSetting(UserAromaPump);
      Payload485[1] = (Local & 0x01);  /*Pump Number 0 or 1*/
      if ((Local == 2) || (Control == 0)){
        Payload485[2] = 0;          /*Pump is off or command is turn OFF so send zero to turn it off*/
      }
      else{
        Local = GetUserSetting(UserAromaVal); /*local is 0 to 13*/
        if (Local > 13){
          Local = 0;        /*in case of out of range*/
        }
        Payload485[2] = AromaValues[Local]; /*the aroma controller actually adds 2 to these numbers. dont know why*/
      }
      break;
    }
    case  0x51:{            /*CHROMA SETUP*/
      Payload485[0] = 3;  /*count*/
      Payload485[1] = ColorIndex;  /*Color Index*/
      Payload485[2] = GetUserSetting(UserChromaInt); /*brightness*/
      Payload485[3] = ColorBrightMax;  /*brightness max*/
      break;      
    }
    case 0x60:{           /*SET SOUND SETTINGS*/
      Payload485[0] = 6;  /*count*/
      Payload485[1] = Volume;       /*volume*/
      Payload485[2] = Bass;         /*Bass control*/
      Payload485[3] = Treble;       /*Treble*/
      Payload485[4] = Balance;      /*balance*/
      Payload485[5] = InputGain;    /*Input gain*/
      Payload485[6] = InputPort;    /*Input port*/
      break;
    } 
    case 0x61:{           /*SET SOUND Volume*/
      Payload485[0] = 1;  /*count*/
      Payload485[1] = Volume;       /*volume*/
      break;
    }
    case 0x62:{           /*Skip Song*/
      Payload485[0] = 1;  /*count*/
      Payload485[1] = IPODFlags;       /*command*/
      break;
    }
    case 0x78:{           /*External Remote only*/
      if (Dest == 0x1D){
        Payload485[0] = 1;  /*count*/
        Payload485[1] = (u8) (RndFloat2u16Int(CorrT_F));       /*Temp deg F*/
        break;
      }
    }
    case 0x80:{           /*ClockCal*/
      Payload485[0] = 5;  /*count*/
      Payload485[1] = ClkCalStatus;    /*status*/
      Payload485[2] = ClkCalData;      /*cal register*/
      Payload485[3] = ClkCalDay;       /*day*/
      Payload485[4] = ClkCalMonth;     /*month*/
      Payload485[5] = ClkCalYear;      /*year*/
      break;
    }
    case 0x83:{           /*EXTERNAL REMOTE AND WIRED REMOTE(4 BUTTON)*/
      Payload485[0] = 4;  /*count*/
      Payload485[1] = TmpInt;    /*CURRENT TEMPERATURE*/
      Payload485[2] = CycMinLeft;    /*Minutes left to run*/
      Payload485[3] = CycSecLeft;    /*Seconds left to run*/
      if (BitSavVar & 0x0004){  /*if deg C*/
        Local = 8;
      }
      else{
        Local = 0;  /* if F*/
      }
      if (UserMemory == 1){
        Local |= 1;
      }
      if (UserMemory == 2){
        Local |= 2;
      }
      if (RunSteamLoop == 0){
        Local &= 0xFFF8;    /* SET USER TO ZERO*/
        Payload485[2] = 0;    /*Minutes left to run is zero if not running*/
        Payload485[3] = 0;    /*Seconds left to run is zero if not running*/
      }
      if (BitSavVar & 0x00000001){  /*if IMOnOff is on */
        Local |= 0x10;              /* add bit for IM on */
      }
      if (Usr3PeriphWasOn){
        Local |= 0x20;              /*ADD BIT FOR GUEST MODE PERIPH WAS ACTIVATED*/
      }
      Payload485[4] = Local; /*F/C and User bits*/
      /*for testing only*/
      Test1 = Payload485[1];
      Test2 = Payload485[2];
      Test3 = Payload485[3];
      Test4 = Payload485[4];
      break;
    }
  }
}

/******************************************************************************/
/*Function to scan for Modules                                                */
/*INPUTS: ReqFindModules = 1 to start. when done =0                           */
/*Output: ActivePeriph is a bit string with 1 being an active peripherial.    */
/*    B0 = Steam Gen #1       B1 = Steam Gen #2        B2 = Steam Gen #3      */
/*    B3 = Steam Gen #4       B4 = Sound module        B5 = Chroma module     */
/*    B6 = Aroma              B7 = Wireless Remote     b8 = Wired Remote      */
/*    B9 = External Remote                                                    */
/* runs in spare time.                                                        */
/******************************************************************************/
void FindModules(void){

  if ((Tx485Flag == 0) && (ReqFindModules == 1)){    /*make sure transfer not in progress and REQUEST ACTIVE*/
    switch (FindModState){
      case 0:{
        Send485(0x13,0x23, 0);    /*START GET OF STATUS OF SG#1*/
        FindModState = 1;         /*COME BACK WHEN DONE*/
        break;
      }
      
      case 1:{
        if (RxIntTO == 0){
          ActivePeriph &= (~0x00000001);
          SG1Status = 0xFF;    /*NOT AVAILABLE*/
          SG1Error = 0x00;     /*DEFAULT ERROR*/
          SG1MajFirmNum = 0x00;
          SG1MinFirmNum = 0x00;
        }
        else{                   /*FOUND SG1*/
          ActivePeriph |= 0x00000001;   /*TURN ON SG1 ACTIVE BIT*/
          SG1Status = RxBuffer1[4];  /*KNOWN LOCATIONS FOR SG RETURN MESSAGES*/
          SG1Error = RxBuffer1[5];
          SG1MajFirmNum = RxBuffer1[6];
          SG1MinFirmNum = RxBuffer1[7];
        }
        FindModState = 2;
        break;
      }
      
      case 2:{
        Send485(0x14,0x23, 0);    /*START GET OF STATUS OF SG#2*/
        FindModState = 3;         /*COME BACK WHEN DONE*/
        break;
      }
      
      case 3:{
        if (RxIntTO == 0){
          ActivePeriph &= (~0x00000002);
          SG2Status = 0xFF;    /*NOT AVAILABLE*/
          SG2Error = 0x00;     /*DEFAULT ERROR*/
          SG2MajFirmNum = 0x00;
          SG2MinFirmNum = 0x00;
        }
        else{                   /*FOUND SG2*/
          ActivePeriph |= 0x00000002;   /*TURN ON SG2 ACTIVE BIT*/
          SG2Status = RxBuffer1[4];  /*KNOWN LOCATIONS FOR SG RETURN MESSAGES*/
          SG2Error = RxBuffer1[5];
          SG2MajFirmNum = RxBuffer1[6];
          SG2MinFirmNum = RxBuffer1[7];
        }
        FindModState = 4;
        break;
      }
      
      case 4:{
        Send485(0x15,0x23, 0);    /*START GET OF STATUS OF SG#3*/
        FindModState = 5;         /*COME BACK WHEN DONE*/
        break;
      }
      
      case 5:{
        if (RxIntTO == 0){
          ActivePeriph &= (~0x00000004);
          SG3Status = 0xFF;    /*NOT AVAILABLE*/
          SG3Error = 0x00;     /*DEFAULT ERROR*/
          SG3MajFirmNum = 0x00;
          SG3MinFirmNum = 0x00;
        }
        else{                   /*FOUND SG3*/
          ActivePeriph |= 0x00000004;   /*TURN ON SG3 ACTIVE BIT*/
          SG3Status = RxBuffer1[4];  /*KNOWN LOCATIONS FOR SG RETURN MESSAGES*/
          SG3Error = RxBuffer1[5];
          SG3MajFirmNum = RxBuffer1[6];
          SG3MinFirmNum = RxBuffer1[7];
        }
        FindModState = 6;
        break;
      }
      
      case 6:{
        Send485(0x16,0x23, 0);    /*START GET OF STATUS OF SG#4*/
        FindModState = 7;         /*COME BACK WHEN DONE*/
        break;
      }
      
      case 7:{
        if (RxIntTO == 0){
          ActivePeriph &= (~0x00000008);
          SG4Status = 0xFF;    /*NOT AVAILABLE*/
          SG4Error = 0x00;     /*DEFAULT ERROR*/
          SG4MajFirmNum = 0x00;
          SG4MinFirmNum = 0x00;
        }
        else{                   /*FOUND SG4*/
          ActivePeriph |= 0x00000008;   /*TURN ON SG4 ACTIVE BIT*/
          SG4Status = RxBuffer1[4];  /*KNOWN LOCATIONS FOR SG RETURN MESSAGES*/
          SG4Error = RxBuffer1[5];
          SG4MajFirmNum = RxBuffer1[6];
          SG4MinFirmNum = RxBuffer1[7];
        }
        FindModState = 8;
        break;
      }
      
      
      case 8:{
        Send485(0x19,0x23, 0);    /*START GET OF STATUS OF AROMA*/
        FindModState = 9;         /*COME BACK WHEN DONE*/
        break;
      }
      
      case 9:{
        if (RxIntTO == 0){
          ActivePeriph &= (~0x00000040);
          Aroma1Status = 0xFF;    /*NOT AVAILABLE*/
          Aroma1MajFirmNum = 0x00;
          Aroma1MinFirmNum = 0x00;
        }
        else{                   /*FOUND AROMA*/
          ActivePeriph |= 0x00000040;   /*TURN ON AROMA ACTIVE BIT*/
          Aroma1Status = RxBuffer1[4];  /*KNOWN LOCATIONS FOR AROMA RETURN MESSAGES*/
          Aroma1MajFirmNum = RxBuffer1[5];
          Aroma1MinFirmNum = RxBuffer1[6];
        }
        FindModState = 10;
        break;
      }
      
      case 10:{
        Send485(0x17,0x23, 0);    /*START GET OF STATUS OF CHROMA*/
        FindModState = 11;         /*COME BACK WHEN DONE*/
        break;
      }
      
      case 11:{
        if (RxIntTO == 0){
          ActivePeriph &= (~0x00000020);
          CHR1Status = 0xFF;    /*NOT AVAILABLE*/
          CHR1NumColors = 0x00;     /*DEFAULT ERROR*/
          CHR1MajFirmNum = 0x00;
          CHR1MinFirmNum = 0x00;
        }
        else{                   /*FOUND AROMA*/
          ActivePeriph |= 0x00000020;   /*TURN ON CHROMA ACTIVE BIT*/
          CHR1Status = RxBuffer1[4];  /*KNOWN LOCATIONS FOR CHROMA RETURN MESSAGES*/
          CHR1NumColors = RxBuffer1[5];
          CHR1MajFirmNum = RxBuffer1[6];
          CHR1MinFirmNum = RxBuffer1[7];
        }
        FindModState = 12;   /* DONE */
        break;
      }
      
      case 12:{
        Send485(0x18,0x23, 0);    /*START GET OF STATUS OF SOUND*/
        FindModState = 13;        /*COME BACK WHEN DONE*/
        break;
      }
      
      case 13:{
        if (RxIntTO == 0){
          ActivePeriph &= (~0x00000010);
          SOUND1Status = 0xFF;    /*NOT AVAILABLE*/
          SOUND1MajFirmNum = 0x00;
          SOUND1MinFirmNum = 0x00;
        }
        else{                           /*FOUND SOUND*/
          ActivePeriph |= 0x00000010;   /*TURN ON SOUND ACTIVE BIT*/
          SOUND1Status = RxBuffer1[4];  /*KNOWN LOCATIONS FOR SOUND RETURN MESSAGES*/
          SOUND1MajFirmNum = RxBuffer1[5];
          SOUND1MinFirmNum = RxBuffer1[6];
        }
        FindModState = 14;
        break;
      }
      
      case 14:{
        Send485(0x1A,0x23, 0);    /*START GET OF STATUS OF REMOTE*/
        FindModState = 15;         /*COME BACK WHEN DONE*/
        break;
      }
      
      case 15:{
        if (RxIntTO == 0){
          ActivePeriph &= (~0x00000080);
          REMOTE1Status = 0xFF;          /*NOT AVAILABLE*/
          REMOTE1MajFirmNum = 0x00;
          REMOTE1MinFirmNum = 0x00;
        }
        else{                           /*FOUND WIRELESS REMOTE*/
          ActivePeriph |= 0x00000080;   /*TURN ON REMOTE ACTIVE BIT*/
          REMOTE1Status = RxBuffer1[4]; /*KNOWN LOCATIONS FOR REMOTE RETURN MESSAGES*/
          REMOTE1MajFirmNum = 0;
          REMOTE1MinFirmNum = 0;
        }
        FindModState = 16;   
        break;
      }
      
      case 16:{
        Send485(0x1E,0x83, 0);    /*START GET OF STATUS OF WIRED REMOTE(4B)*/
        FindModState = 17;        /*COME BACK WHEN DONE*/
        break;
      }
      
      case 17:{
        if (RxIntTO == 0){
          ActivePeriph &= (~0x00000400);
          REMOTE2Status = 0xFF;          /*NOT AVAILABLE*/
          REMOTE2MajFirmNum = 0x00;
          REMOTE2MinFirmNum = 0x00;
        }
        else{                           /*FOUND WIRED REMOTE*/
          ActivePeriph |= 0x00000400;   /*TURN ON WIRED REMOTE ACTIVE BIT*/
          REMOTE2Status = RxBuffer1[4]; /*KNOWN LOCATIONS FOR REMOTE RETURN MESSAGES*/
          REMOTE2MajFirmNum = RxBuffer1[5];
          REMOTE2MinFirmNum = RxBuffer1[6];
        }
       FindModState = 18;
      }
      
      case 18:{
        if(ActivePeriph & 0x00000400){  /*ONLY LOOK IF OTHER WIRED REMOTE DOES NOT EXIST*/
          FindModState = 20;            /*dont look for this wired remote*/
          break;
        }
        Send485(0x1C,0x23, 0);    /*START GET OF STATUS OF WIRED REMOTE*/
        FindModState = 19;         /*COME BACK WHEN DONE*/
        break;
      }
      
      case 19:{
        if (RxIntTO == 0){
          ActivePeriph &= (~0x00000100);
          REMOTE2Status = 0xFF;          /*NOT AVAILABLE*/
          REMOTE2MajFirmNum = 0x00;
          REMOTE2MinFirmNum = 0x00;
        }
        else{                           /*FOUND WIRED REMOTE*/
          ActivePeriph |= 0x00000100;   /*TURN ON WIRED REMOTE ACTIVE BIT*/
          REMOTE2Status = RxBuffer1[4]; /*KNOWN LOCATIONS FOR REMOTE RETURN MESSAGES*/
          REMOTE2MajFirmNum = 0;
          REMOTE2MinFirmNum = 0;
        }
       FindModState = 20;
      }
      
      case 20:{
        Send485(0x1D,0x83, 0);    /*START GET OF STATUS OF EXT REMOTE*/
        FindModState = 21;         /*COME BACK WHEN DONE*/
        break;
      }
      
      case 21:{
        if (RxIntTO == 0){
          ActivePeriph &= (~0x00000200);
          REMOTE3Status = 0xFF;          /*NOT AVAILABLE*/
          REMOTE3MajFirmNum = 0x00;
          REMOTE3MinFirmNum = 0x00;
        }
        else{                           /*FOUND EXT REMOTE*/
          ActivePeriph |= 0x00000200;   /*TURN ON EXT REMOTE ACTIVE BIT*/
          REMOTE3Status = RxBuffer1[4]; /*KNOWN LOCATIONS FOR REMOTE RETURN MESSAGES*/
          REMOTE3MajFirmNum = RxBuffer1[5];
          REMOTE3MinFirmNum = RxBuffer1[6];
        }
        FindModState = 0;   /* DONE */
        ReqFindModules = 0;
        ActiveSteamGens = ActivePeriph & 0x0F;  /*uses only bits 0-3 to see which generators are active at start*/
      }
    }
  }
}


/******************************************************************************/
/*Function to make sure each active module is kept alive by periodic comm.    */
/*INPUTS: ReqModuleKA = 1 to start. when done =0                              */
/* runs in spare time.                                                        */
/*OUTPUT: flag is set in master comm requests to get status                   */
/******************************************************************************/
void ModuleKA(void){
  /*run checks for each module to see if its time to talk to it*/
  /*CHECK THE SG 1 MODULE*/
  if ((KA_SG1 == 0) && (ActivePeriph & 0x0001)){    /*IS TIME TO DO IT AND IT IS AVAILABLE*/
    MasCommReqBits |= SGStatus; /*SET REQUEST BIT*/
  }
  /*CHECK THE SG 2 MODULE*/
  if ((KA_SG2 == 0) && (ActivePeriph & 0x0002)){    /*IS TIME TO DO IT AND IT IS AVAILABLE*/
    MasCommReqBits |= SGStatus; /*SET REQUEST BIT*/
  }
  /*CHECK THE SG 3 MODULE*/
  if ((KA_SG3 == 0) && (ActivePeriph & 0x0004)){    /*IS TIME TO DO IT AND IT IS AVAILABLE*/
    MasCommReqBits |= SGStatus; /*SET REQUEST BIT*/
  }
  /*CHECK THE SG 4 MODULE*/
  if ((KA_SG4 == 0) && (ActivePeriph & 0x0008)){    /*IS TIME TO DO IT AND IT IS AVAILABLE*/
    MasCommReqBits |= SGStatus; /*SET REQUEST BIT*/
  }
  /*CHECK THE AROMA MODULE*/
  if ((KA_AROMA == 0) && (ActivePeriph & 0x0040)){    /*IS TIME TO DO IT AND IT IS AVAILABLE*/
    MasCommReqBits |= AromaStatus; /*SET REQUEST BIT*/
  }
  /*CHECK THE SOUND MODULE*/
  if ((KA_SOUND == 0) && (ActivePeriph & 0x0010)){    /*IS TIME TO DO IT AND IT IS AVAILABLE*/
    MasCommReqBits |= SoundStatus; /*SET REQUEST BIT*/
  }
  /*CHECK THE CHROMA MODULE*/
  if ((KA_CHROMA == 0) && (ActivePeriph & 0x0020)){    /*IS TIME TO DO IT AND IT IS AVAILABLE*/
    MasCommReqBits |= ChromaStatus; /*SET REQUEST BIT*/
  }
  /*CHECK THE WIRELESS REMOTE MODULE*/
  if ((KA_WIRELESSREMOTE == 0) && (ActivePeriph & 0x0080)){    /*IS TIME TO DO IT AND IT IS AVAILABLE*/
    MasCommReqBits |= WLRemoteStatus; /*SET REQUEST BIT*/
  }
  /*CHECK THE WIRED REMOTE MODULE( 3 BUTTON)*/
  if ((KA_WIREDREMOTE == 0) && (ActivePeriph & 0x0100)){    /*IS TIME TO DO IT AND IT IS AVAILABLE*/
    MasCommReqBits |= WRemoteStatus; /*SET REQUEST BIT*/
  }
  /*CHECK THE WIRED REMOTE MODULE( 4 BUTTON)*/
  if ((KA_WIREDREMOTE == 0) && (ActivePeriph & 0x0400)){    /*IS TIME TO DO IT AND IT IS AVAILABLE*/
    MasCommReqBits |= WRemoteStatus4B; /*SET REQUEST BIT*/
  }
  /*CHECK THE EXT REMOTE MODULE*/
  if ((KA_INTERNETREMOTE == 0) && (ActivePeriph & 0x0200)){    /*IS TIME TO DO IT AND IT IS AVAILABLE*/
    MasCommReqBits |= EXTRemoteStatus; /*SET REQUEST BIT*/
  }
}

/******************************************************************************/
/*Function to prioritize requests to the peripherials                         */
/*INPUTS: MasCommReqBits and individual on off bits must be setup.            */
/* runs in spare time.                                                        */
/*OUTPUT: Data either sent to or read from the peripherial                    */
/******************************************************************************/
void DoCommEvents(void){
  /*check for running events*/
  
  if (MasCommBsyBits != 0){
    ContinueEvents();     /*process any already running events*/
    RS485_Dly = RS485_Dly_Val;  /*refresh minimum delay values*/
    return;
  }
  if (RS485_Dly){
    return;               /*wait for minimum delay */
  }
  
  if (MasCommReqBits & ClockCal){    /*IF Req Clock Cal */
    MasCommBsyBits |= ClockCal;      /*activate this command*/
    return;
  }
  if (MasCommReqBits & FindMod){    /*IF Find Modules */
    MasCommBsyBits |= FindMod;      /*activate this command*/
    return;
  }
  if (MasCommReqBits & SGPreheat){    /* IF SG preheat*/
    MasCommBsyBits |= SGPreheat;
    return;
  }
  if (MasCommReqBits & SGOnOff){    /* IF SG On OFF command*/
    MasCommBsyBits |= SGOnOff;
    return;
  }
  if (MasCommReqBits & SGSetTemp){    /*IF SGSet temp command*/
    MasCommBsyBits |= SGSetTemp;      /*activate this command*/
    return;
  }
  if (MasCommReqBits & SGStatus){    /* IF SG status*/
    MasCommBsyBits |= SGStatus;
    return;
  }
  if (MasCommReqBits & AromaOff){    /* IF Aroma Off*/
    MasCommBsyBits |= AromaOff;
    return;
  }
  if (MasCommReqBits & AromaSetup){    /* IF Aroma Setup*/
    MasCommBsyBits |= AromaSetup;
    return;
  }
  if (MasCommReqBits & AromaStatus){    /* IF Aroma status*/
    MasCommBsyBits |= AromaStatus;
    return;
  }
  if (MasCommReqBits & ChromaSetup){    /* IF Chroma setup*/
    MasCommBsyBits |= ChromaSetup;
    return;
  }
  if (MasCommReqBits & ChromaCaned){    /* IF Chroma canned*/
    MasCommBsyBits |= ChromaCaned;
    return;
  }
  if (MasCommReqBits & ChromaInfo){    /* IF Chroma info*/
    MasCommBsyBits |= ChromaInfo;
    return;
  }
  if (MasCommReqBits & ChromaStatus){    /* IF Chroma status*/
    MasCommBsyBits |= ChromaStatus;
    return;
  }
  if (MasCommReqBits & ChromaOnOff){    /* IF ChromaOnOff*/
    MasCommBsyBits |= ChromaOnOff;
    return;
  }
  if (MasCommReqBits & SoundSetup){    /* IF Sound setup*/
    MasCommBsyBits |= SoundSetup;
    return;
  }
  if (MasCommReqBits & SoundVol){    /* IF Sound volume*/
    MasCommBsyBits |= SoundVol;
    return;
  }
  if (MasCommReqBits & SoundSkip){    /* IF Sound Skip*/
    MasCommBsyBits |= SoundSkip;
    return;
  }
  if (MasCommReqBits & SoundOnOff){    /* IF Sound on off*/
    MasCommBsyBits |= SoundOnOff;
    return;
  }
  if (MasCommReqBits & SoundStatus){    /* IF Sound Get Status*/
    MasCommBsyBits |= SoundStatus;
    return;
  }
  if (MasCommReqBits & SoundOnOff){    /* IF Sound on off*/
    MasCommBsyBits |= SoundOnOff;
    return;
  }
  if (MasCommReqBits & WLRemoteStatus){    /* IF WIRELESS Remote status*/
    MasCommBsyBits |= WLRemoteStatus;
    return;
  }
  if (MasCommReqBits & WRemoteStatus){    /* IF WIRED Remote status*/
    MasCommBsyBits |= WRemoteStatus;
    return;
  }
  if (MasCommReqBits & WRemoteStatus4B){    /* IF WIRED Remote status 4 BUTTON*/
    MasCommBsyBits |= WRemoteStatus4B;
    return;
  }
  if (MasCommReqBits & EXTRemoteStatus){    /* IF EXT Remote status*/
    MasCommBsyBits |= EXTRemoteStatus;
    return;
  }
}

/******************************************************************************/
/*Function to continue requests for communications to the peripherials        */
/*INPUTS: MasCommBsyBits                                                      */
/* runs on demand.                                                            */
/*OUTPUT: Data either sent to or read from the peripherial                    */
/******************************************************************************/
void ContinueEvents(void){
  if (Tx485Flag){
    return;           /*wait for previous command to complete or timeout*/
  }
  switch (MasCommBsyBits){
    case ClockCal:{
      switch (CommBsyState){
        case 00:{
          ReqClockCal = 1;    /*STARTUP THE CLOCK CAL ROUTINE*/
          CommBsyState = 1;
          break;
        }
        case 01:{
          if (ReqClockCal == 0){
            CommBsyState = 0;
            MasCommBsyBits = 0;
            MasCommReqBits &= (~ClockCal);   /*end request*/
          }
        }
      }
      break;
    }
    case FindMod:{
      switch (CommBsyState){
        case 00:{
          ReqFindModules = 1;   /*startup find module program*/
          CommBsyState = 1;
          break;
        }
        case 01:{
          if (ReqFindModules == 0){
            CommBsyState = 0;
            MasCommBsyBits = 0;
            MasCommReqBits &= (~FindMod);   /*end request*/
          }
        }
      }
      break;
    }
    case SGSetTemp:{  /*SET THE TEMP FOR ALL OF THE ACTIVE STEAM GENERATORS*/
      switch (CommBsyState){
        case 00:{
          if (ActivePeriph & 0x0001){ /*IF SG1 IS ACTIVE*/
            Send485(0x13, 0x20, 0x00);  /*set gen #1*/
            CommBsyState = 1;
            KA_SG1 = KeepAliveSG;       /*START TIMER SINCE I TALKED TO SG*/
          }
          else{
            CommBsyState = 2;           /*try gen #2*/
          }
          break;
        }
        case 01:{                 /*get info from Gen #1*/
          CommBsyState = 2;       /*go to try gen #2*/
          if ((RxIntTO) && (ActivePeriph & 0x0001)){
            SG1Status = RxBuffer1[4];  /*KNOWN LOCATIONS FOR SG1 RX RETURN MESSAGES*/
            SG1Error = RxBuffer1[5];
            SG1_FC = 0;
          }
          else{
            SG1_FC++;                   /*count error*/
            SG1Status = 0xFF;          /*FLAG BAD RX FOR SG1*/
            if(SG1_FC >= PeriphFailLimit){
              ActivePeriph &= (~0x001); /*shut off periph*/
              SG1_FC = 0;
            }
          }
          break;
        }
        case 0x02:{         /*TRY GEN #2*/
          if (ActivePeriph & 0x0002){ /*IF SG2 IS ACTIVE*/
            Send485(0x14, 0x20, 0x00);  /*set gen #2*/
            KA_SG2 = KeepAliveSG;       /*START TIMER SINCE I TALKED TO SG*/
            CommBsyState = 3;
          }
          else{
            CommBsyState = 4;           /*try GEN #3*/
          }
          break;
        }
        case 0x03:{
          CommBsyState = 4;
          if ((RxIntTO) && (ActivePeriph & 0x0002)){
            SG2Status = RxBuffer1[4];  /*KNOWN LOCATIONS FOR SG2 RX RETURN MESSAGES*/
            SG2Error = RxBuffer1[5];
            SG2_FC = 0;
          }
          else{
            SG2_FC++;                 /*flag error*/
            SG2Status = 0xFF;          /*FLAG BAD RX FOR SG2*/
            if (SG2_FC >= PeriphFailLimit){
              ActivePeriph &= (~0x002); /*shut off periph*/
              SG2_FC = 0;
            }
          }
          break;
        }
        case 0x04:{                   /*try gen #3*/
          if (ActivePeriph & 0x0004){ /*IF SG3 IS ACTIVE*/
            Send485(0x15, 0x20,0x00); /*set gen #3*/
            KA_SG3 = KeepAliveSG;     /*START TIMER SINCE I TALKED TO SG*/
            CommBsyState = 5;
          }
          else{
            CommBsyState = 6;         /*tey gen #4*/
          }
          break;
        }
        case 05:{
          CommBsyState = 6;
          if ((RxIntTO) && (ActivePeriph & 0x0004)){
            SG3Status = RxBuffer1[4];  /*KNOWN LOCATIONS FOR SG3 RX RETURN MESSAGES*/
            SG3Error = RxBuffer1[5];
            SG3_FC = 0;
          }
          else{
            SG3Status = 0xFF;          /*FLAG BAD RX FOR SG3*/
            SG3_FC++;                 /*count failure*/
            if(SG3_FC >= PeriphFailLimit){
              ActivePeriph &= ~0x0004; /*shut off periph*/
              SG3_FC = 0;
            }
          }
          break;
        }
        case 0x06:{                   /*try gen #4 */
          if (ActivePeriph & 0x0008){ /*IF SG4 IS ACTIVE*/
            Send485(0x16, 0x20, 0x00);  /*set gen #4*/
            KA_SG4 = KeepAliveSG;       /*START TIMER SINCE I TALKED TO SG*/
            CommBsyState = 7;
          }
          else{
            CommBsyState = 8;
          }
          break;
        }
        case 0x07:{
          CommBsyState = 8;
          if ((RxIntTO) && (ActivePeriph & 0x0008)){
            SG4Status = RxBuffer1[4];  /*KNOWN LOCATIONS FOR SG4 RX RETURN MESSAGES*/
            SG4Error = RxBuffer1[5];
            SG4_FC = 0;
          }
          else{
            SG4Status = 0xFF;          /*FLAG BAD RX FOR SG4*/
            SG4_FC++;             /*count error*/
            if (SG4_FC >= PeriphFailLimit){
              ActivePeriph &= ~0x0008;  /*shut off periph*/
              SG4_FC = 0;
            }
          }
          break;
        }
        case 0x08:{
          CommBsyState = 0;   /*done*/
          MasCommBsyBits = 0;
          MasCommReqBits &= (~SGSetTemp); /*END REQUEST*/
          break;
        }
      }
      break;
    }

    case SGOnOff:{    /*ON OFF OF ALL THE ACTIVE STEAM GENERATORS*/
      switch (CommBsyState){
        case 00:{
          if (ActivePeriph & 0x0001){ /*IF SG1 IS ACTIVE*/
            Send485(0x13, 0x21, SG_OnOff);  /*set gen #1*/
            KA_SG1 = KeepAliveSG;       /*START TIMER SINCE I TALKED TO SG1*/
          }
          CommBsyState = 1;   /*try gen #2*/
          break;
        }
        case 01:{
          if (ActivePeriph & 0x0002){ /*IF SG2 IS ACTIVE*/
            Send485(0x14, 0x21,SG_OnOff);  /*set gen #1*/
            KA_SG2 = KeepAliveSG;       /*START TIMER SINCE I TALKED TO SG2*/
          }
          CommBsyState = 2;   /*try gen #3*/
          break;
        }
        case 02:{
          if (ActivePeriph & 0x0004){ /*IF SG3 IS ACTIVE*/
            Send485(0x15, 0x21, SG_OnOff);  /*set gen #3*/
            KA_SG3 = KeepAliveSG;       /*START TIMER SINCE I TALKED TO SG3*/
          }
          CommBsyState = 3;   /*try gen #3*/
          break;
        }
        case 03:{
          if (ActivePeriph & 0x0008){ /*IF SG4 IS ACTIVE*/
            Send485(0x16, 0x21, SG_OnOff);  /*set gen #4*/
            KA_SG4 = KeepAliveSG;       /*START TIMER SINCE I TALKED TO SG4*/
          }
          CommBsyState = 4;
          break;
        }
        case 04:{
          CommBsyState = 0;   /*done*/
          MasCommBsyBits = 0;
          MasCommReqBits &= (~SGOnOff);   /*end request*/
          break;
        }
      }
      break;
    }

  case SGPreheat:{  /*SET PREHEAT OF ALL THE ACTIVE STEAM GENERATORS*/
      PreHeatOnOff = (BitSavVar & 0x0001);  /*Get the Preheat value 1 = on*/
      switch (CommBsyState){
        case 00:{
          if (ActivePeriph & 0x0001){ /*IF SG1 IS ACTIVE*/
            Send485(0x13, 0x22, PreHeatOnOff);  /*set gen #1*/
            KA_SG1 = KeepAliveSG;       /*START TIMER SINCE I TALKED TO SG1*/
          }
          CommBsyState = 1;   /*try gen #2*/
          break;
        }
        case 01:{
          if (ActivePeriph & 0x0002){ /*IF SG2 IS ACTIVE*/
            Send485(0x14, 0x22, PreHeatOnOff);  /*set gen #1*/
            KA_SG2 = KeepAliveSG;       /*START TIMER SINCE I TALKED TO SG*/
          }
          CommBsyState = 2;   /*try gen #3*/
          break;
        }
        case 02:{
          if (ActivePeriph & 0x0004){ /*IF SG3 IS ACTIVE*/
            Send485(0x15, 0x22, PreHeatOnOff);  /*set gen #3*/
            KA_SG3 = KeepAliveSG;       /*START TIMER SINCE I TALKED TO SG*/
          }
          CommBsyState = 3;   /*try gen #3*/
          break;
        }
        case 03:{
          if (ActivePeriph & 0x0008){ /*IF SG4 IS ACTIVE*/
            Send485(0x16, 0x22, PreHeatOnOff);  /*set gen #4*/
            KA_SG3 = KeepAliveSG;       /*START TIMER SINCE I TALKED TO SG*/
          }
          CommBsyState = 0;   /*done*/
          MasCommBsyBits = 0;
          MasCommReqBits &= (~SGPreheat);   /*end request*/
          break;
        }
      }
      break;
  }

  case SGStatus:{   /*HERE GET THE STATUS OF ALL OF THE ACTIVE STEAM GENERATORS*/
      switch (CommBsyState){
        case 00:{
          if (ActivePeriph & 0x0001){ /*IF SG1 IS ACTIVE*/
            Send485(0x13, 0x23, 0x00);  /*set gen #1*/
            CommBsyState = 1;
            KA_SG1 = KeepAliveSG;       /*START TIMER SINCE I TALKED TO SG*/
          }
          else{
            CommBsyState = 2;           /*try gen #2*/
          }
          break;
        }
        case 01:{                 /*get info from Gen #1*/
          CommBsyState = 2;       /*go to try gen #2*/
          if ((RxIntTO) && (ActivePeriph & 0x0001)){
            SG1Status = RxBuffer1[4];  /*KNOWN LOCATIONS FOR SG1 RX RETURN MESSAGES*/
            SG1Error = RxBuffer1[5];
            SG1_FC = 0;
          }
          else{
            SG1_FC++;                   /*count error*/
            SG1Status = 0xFF;          /*FLAG BAD RX FOR SG1*/
            if(SG1_FC >= PeriphFailLimit){
              ActivePeriph &= (~0x001); /*shut off periph*/
              SG1_FC = 0;
            }
          }
          break;
        }
        case 0x02:{         /*TRY GEN #2*/
          if (ActivePeriph & 0x0002){ /*IF SG2 IS ACTIVE*/
            Send485(0x14, 0x23, 0x00);  /*set gen #2*/
            KA_SG2 = KeepAliveSG;       /*START TIMER SINCE I TALKED TO SG*/
            CommBsyState = 3;
          }
          else{
            CommBsyState = 4;           /*try GEN #3*/
          }
          break;
        }
        case 0x03:{
          CommBsyState = 4;
          if ((RxIntTO) && (ActivePeriph & 0x0002)){
            SG2Status = RxBuffer1[4];  /*KNOWN LOCATIONS FOR SG2 RX RETURN MESSAGES*/
            SG2Error = RxBuffer1[5];
            SG2_FC = 0;
          }
          else{
            SG2_FC++;                 /*flag error*/
            SG2Status = 0xFF;          /*FLAG BAD RX FOR SG2*/
            if (SG2_FC >= PeriphFailLimit){
              ActivePeriph &= (~0x002); /*shut off periph*/
              SG2_FC = 0;
            }
          }
          break;
        }
        case 0x04:{                   /*try gen #3*/
          if (ActivePeriph & 0x0004){ /*IF SG3 IS ACTIVE*/
            Send485(0x15, 0x23,0x00); /*set gen #3*/
            KA_SG3 = KeepAliveSG;     /*START TIMER SINCE I TALKED TO SG*/
            CommBsyState = 5;
          }
          else{
            CommBsyState = 6;         /*tey gen #4*/
          }
          break;
        }
        case 05:{
          CommBsyState = 6;
          if ((RxIntTO) && (ActivePeriph & 0x0004)){
            SG3Status = RxBuffer1[4];  /*KNOWN LOCATIONS FOR SG3 RX RETURN MESSAGES*/
            SG3Error = RxBuffer1[5];
            SG3_FC = 0;
          }
          else{
            SG3Status = 0xFF;          /*FLAG BAD RX FOR SG3*/
            SG3_FC++;                 /*count failure*/
            if(SG3_FC >= PeriphFailLimit){
              ActivePeriph &= ~0x0004; /*shut off periph*/
              SG3_FC = 0;
            }
          }
          break;
        }
        case 0x06:{                   /*try gen #4 */
          if (ActivePeriph & 0x0008){ /*IF SG4 IS ACTIVE*/
            Send485(0x16, 0x23, 0x00);  /*set gen #4*/
            KA_SG4 = KeepAliveSG;       /*START TIMER SINCE I TALKED TO SG*/
            CommBsyState = 7;
          }
          else{
            CommBsyState = 0;   /*done*/
            MasCommBsyBits = 0;
            MasCommReqBits &= (~SGStatus); /*END REQUEST*/
          }
          break;
        }
        case 07:{
          if ((RxIntTO) && (ActivePeriph & 0x0008)){
            SG4Status = RxBuffer1[4];  /*KNOWN LOCATIONS FOR SG4 RX RETURN MESSAGES*/
            SG4Error = RxBuffer1[5];
            SG4_FC = 0;
          }
          else{
            SG4Status = 0xFF;          /*FLAG BAD RX FOR SG4*/
            SG4_FC++;             /*count error*/
            if (SG4_FC >= PeriphFailLimit){
              ActivePeriph &= ~0x0008;  /*shut off periph*/
              SG4_FC = 0;
            }
          }
          CommBsyState = 0;   /*done*/
          MasCommBsyBits = 0;
          MasCommReqBits &= (~SGStatus); /*END REQUEST*/
          break;
        }
      }
      break;
    }

    case AromaOff:
    case AromaSetup:{
      switch (CommBsyState){
        case 00:{
          if(ActivePeriph & 0x40){
            if (MasCommBsyBits & AromaOff){
            	Send485(0x19, 0x40, 0x00);  /*AROMA TURNS OFF AROMA*/
            }
            else{
            	Send485(0x19, 0x40, 0x01);  /*AROMA SETUP TURNS ON AROMA*/
            }
          KA_AROMA = KeepAliveAroma;  /*START TIMER SINCE I TALKED */
          CommBsyState = 1;
          }
          else{
            CommBsyState = 0;
            if(MasCommBsyBits & AromaOff){
              MasCommReqBits &= (~AromaOff);   /*end request*/
            }
            if(MasCommBsyBits & AromaSetup){
              MasCommReqBits &= (~AromaSetup);   /*end request*/
            }
            MasCommBsyBits = 0;
          }
          break;
        }
        case 01:{
          CommBsyState = 0;
          if(MasCommBsyBits & AromaOff){
            MasCommReqBits &= (~AromaOff);   /*end request*/
          }
          if(MasCommBsyBits & AromaSetup){
            MasCommReqBits &= (~AromaSetup);   /*end request*/
          }
          MasCommBsyBits = 0;
        }
      }
      break;
    }

    case AromaStatus:{    /*SPECIAL CASE AROMA CONTROL ALWAYS USES THE STATUS COMMAND BECAUSE*/
                          /*THE AROMA SOFTWARE HAS AN ERROR AND DOES NOT RETURN ANY DATA WHEN ON/OFF CMD IS USED */
      switch (CommBsyState){
        case 00:{
          if (ActivePeriph & 0x40){
            Send485(0x19, 0x23, 0x00);  /*AROMA STATUS*/
            CommBsyState = 1;
            KA_AROMA = KeepAliveAroma;  /*START TIMER SINCE I TALKED */
          }
          else{       /*just exit if not available*/
            CommBsyState = 0;
            MasCommBsyBits = 0;
            MasCommReqBits &= (~AromaStatus);   /*end request*/
          }
          break;
        }
        case 01:{
          if (RxIntTO == 0){    /*timed out*/
            AROMA_FC++;           /*COUNT FAILURE*/
            if(AROMA_FC >= PeriphFailLimit){
              AROMA_FC = 0;
              ActivePeriph &= (~0x40); /*SHUT OFF THE PERIPH*/
            }
          }
          else{
            AROMA_FC = 0;     /*GOT A GOOD PASS*/
          }
          CommBsyState = 0;
          MasCommBsyBits = 0;
          MasCommReqBits &= (~AromaStatus);   /*end request*/
        }
      }
      break;
    }

  case ChromaOnOff:{
      switch (CommBsyState){
        case 00:{
          if (ActivePeriph & 0x0020){ /*IF Chroma IS ACTIVE*/
            Send485(0x17, 0x21, Chroma_OnOff);  /*chroma on off*/
            KA_CHROMA = KeepAliveChroma;  /*START TIMER SINCE I TALKED */
            CommBsyState = 1;
          }
          else{       /*just exit if not available*/
            CommBsyState = 0;
            MasCommBsyBits = 0;
            MasCommReqBits &= (~Chroma_OnOff);   /*end request*/
          }
          break;
        }
        case 01:{
          CommBsyState = 0;
          MasCommBsyBits = 0;
          MasCommReqBits &= (~ChromaOnOff);   /*end request*/
        }
      }
      break;
    }

    case ChromaStatus:{
      switch (CommBsyState){
        case 00:{
          if (ActivePeriph & 0x0020){ /*IF Chroma IS ACTIVE*/
            Send485(0x17, 0x23, 0x00);  /**/
            KA_CHROMA = KeepAliveChroma;  /*START TIMER SINCE I TALKED TO SG*/
            CommBsyState = 1;
          }
          else{       /*just exit if not available*/
            CommBsyState = 0;
            MasCommBsyBits = 0;
            MasCommReqBits &= (~ChromaStatus);   /*end request*/
          }
          break;
        }
        case 01:{
          if (RxIntTO){
            CHR1Status = RxBuffer1[4];  /*KNOWN LOCATIONS FOR chroma RX RETURN MESSAGES*/
            CHR1NumColors = RxBuffer1[5];
            CHR1_FC = 0;
          }
          else{
            CHR1_FC++;          /*COUNT FAILURE*/
             if(CHR1_FC >= PeriphFailLimit){
              CHR1_FC = 0;
              ActivePeriph &= (~0x20); /*SHUT OFF THE PERIPH*/
            }
          }
          CommBsyState = 0;   /*done*/
          MasCommBsyBits = 0;
          MasCommReqBits &= (~ChromaStatus); /*END REQUEST*/
          break;
        }
      }
      break;
    }
    
    case ChromaCaned:{
      switch (CommBsyState){
        case 00:{
          if (ActivePeriph & 0x20){
            Send485(0x17, 0x51, 0x00);  /*chroma setup*/
            KA_CHROMA = KeepAliveChroma;  /*START TIMER SINCE I TALKED */
            CommBsyState = 1;
          }
          else{       /*just exit if not available*/
            CommBsyState = 0;
            MasCommBsyBits = 0;
            MasCommReqBits &= (~ChromaCaned);   /*end request*/
          }
         break;
        }
        case 01:{
          if (RxIntTO == 0){    /*timed out*/
            CHR1_FC++;           /*COUNT FAILURE*/
            if(CHR1_FC >= PeriphFailLimit){
              CHR1_FC = 0;
              ActivePeriph &= (~0x20); /*SHUT OFF THE PERIPH*/
            }
          }
          else{
            CHR1_FC = 0;     /*GOT A GOOD PASS*/
          }
          CommBsyState = 0;
          MasCommReqBits &= (~ChromaCaned);   /*end request*/
          MasCommBsyBits = 0;
        }
      }
      break;
    }

    case SoundStatus:{
      switch (CommBsyState){
        case 00:{
          if (ActivePeriph & 0x10){
            Send485(0x18, 0x23, 0x00);
            KA_SOUND = KeepAliveSound;  /*START TIMER SINCE I TALKED */
            CommBsyState = 1;
          }
          else{       /*just exit if not available*/
            CommBsyState = 0;
            MasCommBsyBits = 0;
            MasCommReqBits &= (~SoundStatus);   /*end request*/
          }
          break;
        }
        case 01:{
          if (RxIntTO == 0){    /*timed out*/
            SOUND_FC++;           /*COUNT FAILURE*/
            if(SOUND_FC >= PeriphFailLimit){
              SOUND_FC = 0;
              ActivePeriph &= (~0x10); /*SHUT OFF THE PERIPH*/
            }
          }
          else{
            SOUND_FC = 0;     /*GOT A GOOD PASS*/
          }
          CommBsyState = 0;
          MasCommReqBits &= (~SoundStatus);   /*end request*/
          MasCommBsyBits = 0;
        }
      }
      break;
    }
  

    case SoundSetup:{
      switch (CommBsyState){
        case 00:{
          if(ActivePeriph & 0x10){
            Send485(0x18, 0x60, 0x00);
            KA_SOUND = KeepAliveSound;  /*START TIMER SINCE I TALKED */
            CommBsyState = 1;
          }
          else{
            CommBsyState = 0;
            MasCommBsyBits = 0;
            MasCommReqBits &= (~SoundSetup);   /*end request*/
          }
          break;
        }
        case 01:{
          CommBsyState = 0;
          MasCommBsyBits = 0;
          MasCommReqBits &= (~SoundSetup);   /*end request*/
        }
      }
      break;
    }
  
    case SoundVol:{
      switch (CommBsyState){
        case 00:{
          if(ActivePeriph & 0x10){
            Send485(0x18, 0x61, 0x00);
            KA_SOUND = KeepAliveSound;  /*START TIMER SINCE I TALKED */
            CommBsyState = 1;
          }
          else{
            CommBsyState = 0;
            MasCommBsyBits = 0;
            MasCommReqBits &= (~SoundVol);   /*end request*/
          }
          break;
        }
        case 01:{
          CommBsyState = 0;
          MasCommBsyBits = 0;
          MasCommReqBits &= (~SoundVol);   /*end request*/
        }
      }
      break;
    }

    case SoundSkip:{
      switch (CommBsyState){
        case 00:{
          if(ActivePeriph & 0x10){
            Send485(0x18, 0x62, 0x00);
            KA_SOUND = KeepAliveSound;  /*START TIMER SINCE I TALKED */
            CommBsyState = 1;
          }
          else{
            CommBsyState = 0;
            MasCommBsyBits = 0;
            MasCommReqBits &= (~SoundSkip);   /*end request*/
          }
          break;
        }
        case 01:{
          CommBsyState = 0;
          MasCommBsyBits = 0;
          MasCommReqBits &= (~SoundSkip);   /*end request*/
        }
      }
      break;
    }
    
    case SoundOnOff:{
      switch (CommBsyState){
        case 00:{
          if(ActivePeriph & 0x10){
            Send485(0x18, 0x21, Sound_OnOff);  /*SOUND on off*/
            KA_SOUND = KeepAliveSound;  /*START TIMER SINCE I TALKED */
            CommBsyState = 1;
          }
          else{
            CommBsyState = 0;
            MasCommBsyBits = 0;
            MasCommReqBits &= (~SoundOnOff);   /*end request*/
          }
          break;
        }
        case 01:{
          CommBsyState = 0;
          MasCommBsyBits = 0;
          MasCommReqBits &= (~SoundOnOff);   /*end request*/
        }
      }
      break;
    }
    case WLRemoteStatus:{
      switch (CommBsyState){
        case 00:{
          if (ActivePeriph & 0x80){
            Send485(0x1A, 0x23, 0x00);  /*REMOTE STATUS*/
            KA_WIRELESSREMOTE = KeepAliveRemoteWL;       /*START TIMER SINCE I TALKED */
            CommBsyState = 1;
          }
          else{       /*just exit if not available*/
            CommBsyState = 0;
            MasCommBsyBits = 0;
            MasCommReqBits &= (~WLRemoteStatus);   /*end request*/
          }
          break;
        }
        case 01:{
          if ((RxIntTO) && (REMOTE1Status != 0xFF)){    /*not timed out and valid data*/
            REMOTE1Status = RxBuffer1[4]; /*KNOWN LOCATIONS FOR REMOTE RETURN MESSAGES*/
            REMOTE1_FC = 0;     /*CLR FAIL COUNTER*/
            if (((REMOTE1Status & 0x07) > 0) && (RemoteButtonPressed == 0)){
              RemoteButtonPressed = REMOTE1Status & 0x07;  /*IF A BUTTON WAS PRESSED FLAG IT*/
              RemoteSrc = 1;        /*flag it is from the Wireless Remote*/
            }
          }
          else{
            REMOTE1_FC++;           /*COUNT FAILURE*/
            if(REMOTE1_FC >= PeriphFailLimit){
              REMOTE1_FC = 0;
              ActivePeriph &= (~0x80); /*SHUT OFF THE PERIPH*/
            }
          }
          CommBsyState = 0;
          MasCommBsyBits = 0;
          MasCommReqBits &= (~WLRemoteStatus);   /*end request*/
        }
      }
      break;
    }
    case WRemoteStatus:{
      switch (CommBsyState){
        case 00:{
          if (ActivePeriph & 0x100){
            Send485(0x1C, 0x23, 0x00);  /*WIRED REMOTE STATUS*/
            KA_WIREDREMOTE = KeepAliveRemoteW;       /*START TIMER SINCE I TALKED */
            CommBsyState = 1;
          }
          else{       /*just exit if not available*/
            CommBsyState = 0;
            MasCommBsyBits = 0;
            MasCommReqBits &= (~WRemoteStatus);   /*end request*/
          }
          break;
        }
        case 01:{
          if ((RxIntTO) && (REMOTE2Status != 0xFF)){    /*not timed out and valid data*/
            REMOTE2_FC = 0;     /*CLR FAIL COUNTER*/
            REMOTE2Status = RxBuffer1[4]; /*KNOWN LOCATIONS FOR REMOTE RETURN MESSAGES*/
            if (((REMOTE2Status & 0x07) && (RemoteButtonPressed == 0)) > 0){
              RemoteButtonPressed = REMOTE2Status & 0x07;  /*IF A BUTTON WAS PRESSED FLAG IT*/
              RemoteSrc = 0;        /*flag it is from the Wired Remote*/
            }
          }
          else{
            REMOTE2_FC++;           /*COUNT FAILURE*/
            if(REMOTE2_FC >= PeriphFailLimit){
              REMOTE2_FC = 0;
              ActivePeriph &= (~0x100); /*SHUT OFF THE PERIPH*/
            }
          }
          CommBsyState = 0;
          MasCommBsyBits = 0;
          MasCommReqBits &= (~WRemoteStatus);   /*end request*/
        }
      }
      break;
    }
    case WRemoteStatus4B:{
      switch (CommBsyState){
        case 00:{
          if (ActivePeriph & 0x400){
            Send485(0x1E, 0x83, 0x00);  /*WIRED REMOTE STATUS*/
            KA_WIREDREMOTE = KeepAliveRemoteW;       /*START TIMER SINCE I TALKED */
            CommBsyState = 1;
          }
          else{       /*just exit if not available*/
            CommBsyState = 0;
            MasCommBsyBits = 0;
            MasCommReqBits &= (~WRemoteStatus4B);   /*end request*/
          }
          break;
        }
        case 01:{
          if ((RxIntTO) && (REMOTE2Status != 0xFF)){    /*not timed out and valid data*/
            REMOTE2_FC = 0;     /*CLR FAIL COUNTER*/
            REMOTE2Status = RxBuffer1[4]; /*KNOWN LOCATIONS FOR REMOTE RETURN MESSAGES*/
            REMOTE2MajFirmNum = RxBuffer1[5];
            REMOTE2MinFirmNum = RxBuffer1[6];
            if (((REMOTE2Status & 0x07) && (RemoteButtonPressed == 0)) > 0){
              RemoteButtonPressed = REMOTE2Status & 0x07;  /*IF A BUTTON WAS PRESSED FLAG IT*/
              RemoteSrc = 0;        /*flag it is from the Wired Remote*/
            }
          }
          else{
            REMOTE2_FC++;           /*COUNT FAILURE*/
            if(REMOTE2_FC >= PeriphFailLimit){
              REMOTE2_FC = 0;
              ActivePeriph &= (~0x400); /*SHUT OFF THE PERIPH*/
            }
          }
          CommBsyState = 0;
          MasCommBsyBits = 0;
          MasCommReqBits &= (~WRemoteStatus4B);   /*end request*/
        }
      }
      break;
    }
    case EXTRemoteStatus:{
      switch (CommBsyState){
        case 00:{
          if (ActivePeriph & 0x200){
            Send485(0x1D, 0x83, 0x00);  /*EXTERNAL REMOTE STATUS*/
            KA_INTERNETREMOTE = KeepAliveRemoteINT;       /*START TIMER SINCE I TALKED */
            CommBsyState = 1;
          }
          else{       /*just exit if not available*/
            CommBsyState = 0;
            MasCommBsyBits = 0;
            MasCommReqBits &= (~EXTRemoteStatus);   /*end request*/
          }
          break;
        }
        case 01:{
          if ((RxIntTO) && (REMOTE3Status != 0xFF)){    /*not timed out and valid data*/
            REMOTE3_FC = 0;     /*CLR FAIL COUNTER*/
            REMOTE3Status = RxBuffer1[4]; /*KNOWN LOCATIONS FOR REMOTE RETURN MESSAGES*/
            if ((REMOTE3Status & 0x07) && (RemoteButtonPressed == 0)){
              RemoteButtonPressed = (REMOTE3Status & 0x07);  /*IF A BUTTON WAS PRESSED FLAG IT*/
              RemoteSrc = 1;        /*flag it is from the Ext remote*/
            }
          }
          else{
            REMOTE3_FC++;           /*COUNT FAILURE*/
            if(REMOTE3_FC >= PeriphFailLimit){
              REMOTE3_FC = 0;
              ActivePeriph &= (~0x200); /*SHUT OFF THE PERIPH*/
            }
          }
          CommBsyState = 0;
          MasCommBsyBits = 0;
          MasCommReqBits &= (~EXTRemoteStatus);   /*end request*/
        }
      }
      break;
    }
  }
}

/******************************************************************************/
/*Function to perform Clock Calibration if external PC program is present     */
/* runs from Main.                                                            */
/******************************************************************************/
void CheckClockCal(void){
  if ((Tx485Flag == 0) && (ReqClockCal == 1) && (CalClkDly == 0)){    /*make sure transfer not in progress and REQUEST ACTIVE*/
    switch (ChkClkState){
      case 0x00:{
        Send485(0xFE, 0x80, 0x00);  /*Check for Clock Cal program*/
        ChkClkState = 1;
        break;
      }
      case 0x01:{
        if (RxIntTO == 0){
          if(ChkClkFailCtr > 2){
            ReqClockCal = 0;    /*not available just continue exit command**/
            ChkClkState = 0;
          }
          else{
            ChkClkFailCtr++;
            ChkClkState = 2;
          }
        }
        else{                 /*HERE I FOUND THE EXTERNAL PROGRAM*/
          ChkClkFailCtr = 0;
          ChkClkState = 2;
        }
        break;
      }
      case 0x02:{
        if(RxBuffer1[4] !=1){   /*if there was no cal data from the cal request*/
          Send485(0xFE, 0x80, 0x00);  /*ask again*/
          CalClkDly = DLY100_500_MS;
          ChkClkState = 1;
          break;
        }
        else{                   /*GOT CAL DATA TO IMPLEMENT*/
          ClkCalData = RxBuffer1[5]; /*save data locally*/
          ClkCalDay = RxBuffer1[7];
          ClkCalMonth = RxBuffer1[8];
          ClkCalYear = RxBuffer1[9];
          SetRTCCalData(ClkCalData);
          EEClkCalSetup();
          ClkCalStatus = 1;
          Send485(0xFE, 0x80, 0x00);  /*ask again*/
          ClkCalStatus = 0;
          CalClkDly = DLY100_500_MS;

          /*REMOVE THE REFERENCE NUMBER TO TO FORCE THE SETUP OF THE RTC ON NEXT STARTUP*/
          PWR_BackupAccessCmd(ENABLE);
          RTC_WaitForLastTask();
          BKP_WriteBackupRegister(BKP_DR1, 0x0000);
          RTC_WaitForLastTask();
          PWR_BackupAccessCmd(DISABLE);
          ChkClkState = 1;
          break;
        }
      }
    }
  }
}

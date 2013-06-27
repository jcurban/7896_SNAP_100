
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __COMM485_H
#define __COMM485_H

#define ETX 0x03
#define STX 0x02
#define DLE 0x10
/*FOR MasCommReqBits AND MasCommBsyBits DEFINITIONS*/
#define FindMod           0x00000001      /*bit for find modules*/
#define SGSetTemp         0x00000002      /*Com bit for steam gen set temp cmd*/
#define SGOnOff           0x00000004      /*Com bit for steam gen on off cmd*/
#define SGPreheat         0x00000008      /*Com bit for steam gen preheat cmd*/
#define SGStatus          0x00000010      /*Com bit for steam gen get status*/
#define AromaOff          0x00000020      /*Com bit for Aroma On Off cmd*/
#define AromaSetup        0x00000040      /*Com bit for Aroma Setup*/
#define AromaStatus       0x00000080      /*Com bit for Aroma status*/
#define ChromaOnOff       0x00000100      /*Com bit for Chroma On Off*/
#define ChromaSetup       0x00000200      /*Com bit for Chroma setup*/
#define ChromaCaned       0x00000400      /*Com bit for Chroma set canned program*/
#define ChromaInfo        0x00000800      /*Com bit for Chroma info*/
#define ChromaStatus      0x00001000      /*Com bit for Chroma status*/
#define SoundSetup        0x00002000      /*Com bit for Sound Setup*/
#define SoundVol          0x00004000      /*Com bit for Sound volume*/
#define SoundSkip         0x00008000      /*Com bit for Sound skip song*/
#define SoundOnOff        0x00010000      /*Com bit for Sound On Off*/
#define SoundStatus       0x00020000      /*Com bit for Sound Get status*/
#define WLRemoteStatus    0x00040000      /*Com bit for wireless remote status*/
#define WRemoteStatus     0x00080000      /*Com bit for wired remote status*/
#define EXTRemoteStatus   0x00100000      /*Com bit for external remote status*/
#define WRemoteStatus4B   0x00200000      /*Com bit for wired remote status (4 BUTTON)*/
#define ClockCal          0x80000000      /*Com bit for Clock calibration*/

/* Includes ------------------------------------------------------------------
#include "stm8f10x_lib.h"
#include "stm8f10x_it.h"
#include "Periph_Functions.h"

/* Exported constants --------------------------------------------------------*/
#define RxBufferSize1   25    /*USART1 BUFFER SIZE*/

/******************************************************************************/
/* Exported types ------------------------------------------------------------*/
extern u8  Tx485Buf[24];   /*data to send*/
extern u8  Tx485Flag;      /*2= start,1=Tx done, 0 = rx done*/
extern u8  Payload485[];   /*payload data to send with the 485 packet 1st byte is length*/
extern vu8 RxBuffer1[];
extern u8  ReqFindModules; /*SET TO 1 TO REQUEST FIND. WHEN DONE WILL GO TO 0*/
extern u8  Aroma1Val;      /*Raw Value 0-12 value for user1 aroma*/
extern u8  Aroma2Val;      /*Raw Value 0-12 value for user2 aroma*/
extern u8  Aroma3Val;      /*Raw Value 0-12 value for user3 aroma*/
extern u8  Pump1Val;       /*Raw Value 0=pump1, 1=Pump2, 2=off for user 1*/
extern u8  Pump2Val;       /*Raw Value 0=pump1, 1=Pump2, 2=off for user 2*/
extern u8  Pump3Val;       /*Raw Value 0=pump1, 1=Pump2, 2=off for user 3*/
extern u8  Chroma1Color;   /*Raw Value 0-12 value for user1 Color Value*/
extern u8  Chroma2Color;   /*Raw Value 0-12 value for user2 Color Value*/
extern u8  Chroma3Color;   /*Raw Value 0-12 value for user3 Color Value*/
extern u8  Chroma1Int;     /*Raw Value 0-12 value for user1 Chroma intensity*/
extern u8  Chroma2Int;     /*Raw Value 0-12 value for user2 Chroma intensity*/
extern u8  Chroma3Int;     /*Raw Value 0-12 value for user3 Chroma intensity*/
extern u8  Aroma1Val;      /*Raw Value 0-12 value for aroma 1 */
extern u8  Aroma2Val;      /*Raw Value 0-12 value for aroma 2 */
extern u8  Aroma3Val;      /*Raw Value 0-12 value for aroma 3 */
extern u8  Vol1Val;        /*Raw Value 0-12 value for volume user 1*/
extern u8  Vol2Val;        /*Raw Value 0-12 value for volume user 2*/
extern u8  Vol3Val;        /*Raw Value 0-12 value for volume user 3*/
extern u8  Bass1Val;       /*Raw Value 0-12 value for Bass user 1*/
extern u8  Bass2Val;       /*Raw Value 0-12 value for Bass user 2*/
extern u8  Bass3Val;       /*Raw Value 0-12 value for Bass user 3*/
extern u8  InSrc1Val;      /*Raw Value Sound Source user 1, Src1=1, Scr2=3, IPOD = 3*/
extern u8  InSrc2Val;      /*Raw Value Sound Source user 2, Src1=1, Scr2=3, IPOD = 3*/
extern u8  InSrc3Val;      /*Raw Value Sound Source user 3, Src1=1, Scr2=3, IPOD = 3*/
extern u8  Treb1Val;       /*Raw Value 0-12 value for Treblw user 1*/
extern u8  Treb2Val;       /*Raw Value 0-12 value for Treble user 2*/
extern u8  Treb3Val;       /*Raw Value 0-12 value for Treble user 3*/
extern u8  Temp1Val;       /*SetTemp Deg F for Temp user 1*/
extern u8  Temp2Val;       /*SetTemp Deg F for Temp user 2*/
extern u8  Temp3Val;       /*SetTemp Deg F for Temp user 3*/
extern u8  Temp1Val_C;     /*SetTemp Deg F for Temp user 1 Local uses only not saved*/
extern u8  Temp2Val_C;     /*SetTemp Deg F for Temp user 2 Local uses only not saved*/
extern u8  Temp3Val_C;     /*SetTemp Deg F for Temp user 3 Local uses only not saved*/
extern u8  CycTime1Val;    /*Cycle Time value 0-60 for Cycle Time user 1*/
extern u8  CycTime2Val;    /*Cycle Time value 0-60 for Cycle Time user 2*/
extern u8  CycTime3Val;    /*Cycle Time value 0-60 for Cycle Time user 3*/
extern u8  Bal1Val;        /*Raw Value 0-12 value for Balance user 1*/
extern u8  Bal2Val;        /*Raw Value 0-12 value for Balance user 2*/
extern u8  Bal3Val;        /*Raw Value 0-12 value for Balance user 3*/
extern u8  InGain1Val;     /*Raw Value 0-12 value for Input Source user 1*/
extern u8  InGain2Val;     /*Raw Value 0-12 value for Input Source 2*/
extern u8  InGain3Val;     /*Raw Value 0-12 value for Input Source 3*/
extern u32 MasCommReqBits;
extern u8  Chroma_OnOff;   /*ON OFF FOR CHROMA */
extern u8  ColorIndex;     /*COLOR INDEX TO SEND TO CHROMA CONTROLLER 0-8*/
extern u8  ColorBrightMax;
extern u8  Volume;        /*Sound volume 0-47*/
extern u8  Bass;          /*Sound Bass 0-15*/
extern u8  Treble;        /*Sound Treble 0-15*/
extern u8  Balance;       /*Sound Balance 0-47*/
extern u8  InputGain;     /*Sound Input gain 0-11*/
extern u8  InputPort;     /*Sound Input select 0=MiniJack, 1=RCA jacks,2=IPOD*/
extern u8  Sound_OnOff;    /*ON OFF FOR SOUND*/
extern u16 ActivePeriph;
extern u8 SG1Status;
extern u8 SG1Error;
extern u8 SG1MajFirmNum;
extern u8 SG1MinFirmNum;
extern u8 SG2Status;
extern u8 SG2Error;
extern u8 SG2MajFirmNum;
extern u8 SG2MinFirmNum;
extern u8 SG3Status;
extern u8 SG3Error;
extern u8 SG3MajFirmNum;
extern u8 SG3MinFirmNum;
extern u8 SG4Status;
extern u8 SG4Error;
extern u8 SG4MajFirmNum;
extern u8 SG4MinFirmNum;
extern u8 Aroma1Status;
extern u8 Aroma1MajFirmNum;
extern u8 Aroma1MinFirmNum;
extern u8 CHR1Status;
extern u8 CHR1NumColors;
extern u8 CHR1MajFirmNum;
extern u8 CHR1MinFirmNum;
extern u8 SOUND1Status;
extern u8 SOUND1MajFirmNum;
extern u8 SOUND1MinFirmNum;
extern u8 REMOTE1Status;
extern u8 REMOTE1MajFirmNum;
extern u8 REMOTE1MinFirmNum;
extern u8 REMOTE2Status;
extern u8 REMOTE2MajFirmNum;
extern u8 REMOTE2MinFirmNum;
extern u8 REMOTE3Status;
extern u8 REMOTE3MajFirmNum;
extern u8 REMOTE3MinFirmNum;
extern u8 SGTemperatureCmd; /*Commanded temperature in deg F FOR THE steam generator*/
extern u8 AudAInLevel;    /*Input level for Audio A (MiniJack)*/
extern u8 AudBInLevel;    /*Input level for Audio B (RCA Jacks)*/
extern u8 IPODInLevel;    /*Input level for IPOD */
extern u16 BitSavVar;
extern u8 RemoteButtonPressed;
extern u8 SG_OnOff;        /*ON OFF FOR STEAM GENERATORS*/
extern u8 ClkCalStatus;    /*status*/
extern u8 ClkCalData;      /*cal register*/
extern u8 ClkCalDay;       /*day*/
extern u8 ClkCalMonth;     /*month*/
extern u8 ClkCalYear;      /*year*/
extern u8 ActiveSteamGens;  /*uses only bits 0-3 as above. to see what is active at start*/
extern u8  RemoteSrc;      /*IS 1 IF FROM BUTTON PRESSED FROM WIRELESS OR EXT REMOTE PERIPHERIALS*/
extern u8 PreHeatOnOff;   /*used during request for communications to destinguish beteen
                    on and off control. 1=on 0= off for all modules*/

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Send485(u8 Dest, u8 Command, u8 Control);
void FindModules(void);
void ParseRxData(void);
void ModuleKA(void);
void DoCommEvents(void);
void ContinueEvents(void);
u8 FillTxBuf( u8 BufOffset, u8 Dat);
void SetPayload(u8 Command, u8 Control, u8 Dest);
void CheckClockCal(void);

#endif /* __COMM485_H */


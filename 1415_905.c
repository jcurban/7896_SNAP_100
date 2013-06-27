/*********************************************************/
/* Software for K-C Towel module			 */
/* JOC							 */
/* Schematic 	1415-53					 */
/* PIC16F914  @ 4MHZ  INTERNAL OSC			 */
/*********************************************************/
/********************************************************/
/*	 V 7.02 SNAP DEMO SPECIAL		 	*/
/*SCHEMATIC CUT AND JUMPER SEE SCHEMATIC MARKUP		*/
/********************************************************/

#include "int16CXX.h"

#pragma config  |= 0x11C4

/* DEFINES*/
#define	DEBUG		1		/*DEBUG=1 TO REMOVE ALL SLEEP FUNCTIONS*/

#define	 VER0		0		/*VERSION NUMBER VER0,VER1,VER2,VER3 */
#define	 VER1		9
#define  VER2		0
#define  VER3		5		/*THIS VERSION = 09.05*/

#define	 CR		0x0D
#define  LF		0x0A
/* PORT DEFINITIONS*/
#define	VLB 		PORTA.0		/*LOW BATTERY INPUT*/
#define	V_OC 		PORTA.1		/*OVERCURRENT VOLTAGE*/
#define	IREn 		PORTB.3		/*IR ENABLE FOR HAND SENSOR 0=EN*/
#define	IR 		PORTB.2		/*INPUT FROM IR RECEIVER (active low)*/
#define	RELAY 		PORTA.6		/*MOTOR RELAY OUT 1=ON*/
#define	LOW_IR	 	TRISE.0		/*ABOUT 1.7 MA IF = 0	*/
#define	MID_IR	 	TRISE.1		/*ABOUT 3.5MA IF = 0	*/
#define	ProxEn	 	PORTC.4		/*WHEEL SENSOR ENABLE*/
#define	ProxRd	 	PORTA.3		/*WHEEL SENSOR DATA*/
#define	Tear_Input	PORTB.0		/*TEAR INPUT LINE*/
#define	LOWBAT_R	PORTC.0		/*RED DRIVE ON LOWBAT LED*/
#define	LOWBAT_G	PORTC.1		/*GRN DRIVE ON LOWBAT LED*/
#define	LPAPER_R	PORTC.2		/*RED DRIVE ON LOW PAPER LED*/
#define	LPAPER_G	PORTC.3		/*GRN DRIVE ON LOW PAPER LED*/

#define SLEEP_T_CNT	369	/*359us per interrupt on timer * 369 = one sleep time(0.132 sec*/
#define TB_LEDDSP_CNT	20	/*NUMBER OF DISPENSES THE LED TB DISPLAY IS ACTIVE*/
#define JAM_FLAG_LIMIT	2	/*NUMBER OF TIMES JAM HAPPENS BEFORE PERMINATE SHUT DOWN*/
#define NTB_LIMIT_CNT2	2	/*NUMBER OF TEAR BAR ERRORS(MODE 2) ALLOWED BEFORE LED INDICATION OF ERROR*/
#define NTB_LIMIT_CNT0	2	/*NUMBER OF TEAR BAR ERRORS(MODE 0) ALLOWED BEFORE LED INDICATION OF ERROR*/

/*THESE DEFINES ARE APPROXIMATE VALUES TO BE USED WITH "delayms"*/
/*BASED ON 358usec PER PASS OF THE RT INTERRUPT			*/
/*COUNTS ARE COUNTED UP TO FIX COMPILER/PROCESSOR LIMITATIONS	*/

#define DLY_1_MS	65536 - 3
#define DLY_4_MS 	65536 - 11
#define DLY_10_MS	65536 - 28
#define DLY_30_MS	65536 - 84
#define DLY_40_MS	65536 - 112
#define DLY_50_MS	65536 - 140
#define DLY_90_MS	65536 - 251
#define DLY_100_MS	65536 - 279
#define DLY_200_MS	65536 - 559
#define DLY_250_MS	65536 - 698
#define DLY_400_MS	65536 - 1117
#define DLY_500_MS	65536 - 1397
#define	DLY_0P8_SEC	65536 - 2235
#define	DLY_1_SEC	65536 - 2793
#define DLY_1P2_SEC	65536 - 3352
#define DLY_1P5_SEC	65536 - 4190
#define	DLY_2_SEC	65536 - 5586
#define	DLY_2P8_SEC	65536 - 7821
#define DLY_3_SEC	65536 - 8379
#define DLY_3P8_SEC	65536 - 10615
#define	DLY_4_SEC	65536 - 11173
#define	DLY_5_SEC	65536 - 13966
#define DLY_6_SEC	65536 - 16759
#define DLY_9P8_SEC	65536 - 27374
#define DLY_10_SEC	65536 - 27933
#define DLY_15_SEC	65536 - 41899
#define DLY_20_SEC	65536 - 55856

#define UsartSendDlyVal DLY_20_SEC
#define UartRxDlyVal	DLY_4_SEC
#define SLEEP_T_VAL	65536 - SLEEP_T_CNT	/*COUNTS OF TIMER0 TIME TAKEN UP BY SLEEP TIME*/
#define BLCNT_DLY_OFF	DLY_200_MS;
#define BLCNT_DLY_ON	DLY_200_MS;
#define BLCNT_DLY_SPACE	DLY_400_MS;
#define PAPER_ADV_TIME	DLY_3_SEC 
#define JAM_DLY_TIME	DLY_2_SEC
		/*SPECIAL FOR  "L" COUNTERS THESE ARE 8 BIT CTRS COUNTED AT 1HZ*/
#define JAM_RESET_DLY	DLY_10_SEC

#define MODE2_PASSES4_CLR 3			/*MODE 2 CLRS IN A ROW TO DECLARE A CLEAR*/

#define SLEEP_T_VAL_24	0xFFFFFF - SLEEP_T_CNT	/*COUNTS OF TIMER0 TIME TAKEN UP BY SLEEP TIME*/
#define SleepExtendVal	0xFFFFFF - 0x9970B9	/*this is one hour of delay*/

/*SLEEP COUNTER DEFINITIONS NUMBER OF SLEEP PASSES TO DO AT SLEEP TIME*/
#define SleepM0_Ext_Dly	2		/*HIDDEN MODE*/
#define SleepM2_Ext_Dly	1		/*HANG MODE*/
#define SleepM0_Dly	1		/*HIDDEN MODE*/
#define SleepM2_Dly	1		/*HANG MODE*/

/*DEFINES FOR THE LED TRANSFER PROTOCOL*/
#define STX		0x02
#define ETX		0x03
#define DLE		0x10

/*----------------------------------------------------------------------*/
/*EEPROM LOCATION DEFINITIONS						*/
/*EEPROM IS PROGRAMMED FROM QUICKWRITER LSB 1st				*/
/*----------------------------------------------------------------------*/
#define	EE_TSB_CNT3	1		/*STORED IN EE TOTAL SINCE BORN COUNT MSB*/
#define EE_TSB_CNT2	2		/*STORED IN EE TOTAL SINCE BORN COUNT*/
#define EE_TSB_CNT1	3		/*STORED IN EE TOTAL SINCE BORN COUNT LSB*/
#define EE_TSB_CNT0	4		/*STORED IN EE TOTAL SINCE BORN COUNT LSB*/

#define	EE_TLBC_CNT2	5		/*STORED IN EE TOTAL LAST BATTERY CHANGE COUNT MSB*/
#define EE_TLBC_CNT1	6		/*STORED IN EE TOTAL LAST BATTERY CHANGE COUNT*/
#define EE_TLBC_CNT0	7		/*STORED IN EE TOTAL LAST BATTERY CHANGE COUNT LSB*/

#define	EE_TCBC_CNT2	8		/*STORED IN EE TOTAL CURRENT BATTERY CHANGE COUNT MSB*/
#define EE_TCBC_CNT1	9		/*STORED IN EE TOTAL CURRENT BATTERY CHANGE COUNT*/
#define EE_TCBC_CNT0	10		/*STORED IN EE TOTAL CURRENT BATTERY CHANGE COUNT LSB*/

#define	EE_TBSB_CNT1	11		/*STORED IN EE BATTERY CHANGES SINCE BORN COUNT MSB*/
#define EE_TBSB_CNT0	12		/*STORED IN EE BATTERY CHANGES SINCE BORN COUNT LSB*/

#define	EE_VLBC_CNT1	13		/*STORED IN EE BATTERY VOLTAGE AT LAST BATTERY CHANGE MSB*/
#define EE_VLBC_CNT0	14		/*STORED IN EE BATTERY VOLTAGE AT LAST BATTERY CHANGE LSB*/

#define EEAnValSav1	15		/*ANALOG VALUE MSB SAVE FOR CHECKING BATTERY CHANGE*/
#define EEAnValSav0	16		/*ANALOG VALUE LSB SAVE FOR CHECKING BATTERY CHANGE*/

#define	EE_VLBCM1_CNT1	17		/*STORED IN EE BATTERY VOLTAGE AT LAST BATTERY CHANGE-1 MSB*/
#define EE_VLBCM1_CNT0	18		/*STORED IN EE BATTERY VOLTAGE AT LAST BATTERY CHANGE-1 LSB*/

#define	EE_VLBCM2_CNT1	19		/*STORED IN EE BATTERY VOLTAGE AT LAST BATTERY CHANGE-2 MSB*/
#define EE_VLBCM2_CNT0	20		/*STORED IN EE BATTERY VOLTAGE AT LAST BATTERY CHANGE-2 LSB*/

#define	EE_VLBCM3_CNT1	21		/*STORED IN EE BATTERY VOLTAGE AT LAST BATTERY CHANGE-3 MSB*/
#define EE_VLBCM3_CNT0	22		/*STORED IN EE BATTERY VOLTAGE AT LAST BATTERY CHANGE-3 LSB*/

#define	EE_VLBCM4_CNT1	23		/*STORED IN EE BATTERY VOLTAGE AT LAST BATTERY CHANGE-4 MSB*/
#define EE_VLBCM4_CNT0	24		/*STORED IN EE BATTERY VOLTAGE AT LAST BATTERY CHANGE-4 LSB*/


#pragma cdata[0x2101]= 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,\
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  /* PROGRAM THE EEPROM LOCATIONS 1ST TIME*/

/*----------------------------------------------------------------------*/
/*RESISTOR DIVIDER IS 0.3125 AND FULL SCALE =  3.3 VOLTS TRIP IS 5.1 V	*/
/*THRESHOLD FOR BATT LOW (LT 1.275V PER CELL)				*/
/*SO CHECK VALUE I= ((0.3125 * 5.1)/3.3) * 1024	= 494			*/
/*----------------------------------------------------------------------*/
#define BATT_LOW_VAL	494
/*----------------------------------------------------------------------*/
/* With Vref=3.3V and Rsense=.15 Ohm					*/
/*		120mv	.8A	37 counts				*/
/*		240mv	1.60A	75 counts				*/
/*----------------------------------------------------------------------*/
#define MAXI_MOTOR   75		/* 1.6A */
#define SAFE_BELOW   37		/* 0.80A*/

/* varaiable definitions*/
char FLAGS,FLAGS1;
char KEYS,BITS;

bit LOW_PAPER		@ KEYS.0;	/*IF LOW_PAPER KEYS.0=1*/
bit PAPER_FEED		@ KEYS.1;	/*IF PAPER FEED KEYS.1=1*/
bit DOOR_OPEN		@ KEYS.2;	/*IF DOOR OPEN KEYS.2=1*/
bit EX_SHT_FLAG		@ FLAGS.0;	/*FLAG TO INDICATE A SHORT EXTRA SHEET DISPENSE*/
bit SPCL_FLAG		@ FLAGS.1;	/*FOR SPECIAL OPS*/
bit IR_MODE		@ FLAGS.2;	/*IR SENSOR MODE CONTROL*/
bit JAM_FLAG		@ FLAGS.3;	/*USED FOR JAM OVERCURRENT ERROR*/
bit BAT_CHANGED		@ FLAGS.4;	/*IF =  1 THEN BATTERY WAS CHANGED*/
bit IR_SENSED		@ FLAGS.5;	/*SENSED A HAND OR TOWEL IN THROAT*/
bit Stop_AD_Reads	@ FLAGS.6;	/*USED FOR LED DATA SEQUENCES*/
bit UartStartupFlag	@ FLAGS.7;	/*SET TO 1 WHEN THE STARTUP UART STUFF IS DONE*/
bit UartTransferFlag	@ FLAGS1.0;	/*SET TO 1 WHEN THE TRANSFER DATA STUFF IS DONE*/
bit UartNoAT		@ FLAGS1.1;	/*SET TO 1 TO NOT SEND AT+ AT START OF UART TRANS*/
bit UartNoLF		@ FLAGS1.2;	/*SET TO 1 TO NOT SEND CRLF AT END OF UART TRANS*/
bit WHEEL_TOG		@ BITS.0;	/*EDGE TRANSITION BIT FOR ROLLER SENSOR*/
bit DISP_VER_DONE	@ BITS.1;	/*SET TO 1 WHEN VERSION BLINKING IS DONE*/
bit TB_LED_FLG		@ BITS.2;	/*FLAG TO INDICATE TEAR BAR TRIGGERED*/
bit LED_LITERAL		@ BITS.3;	/*USED FOR LED LITERAL ESCAPE OVERIDE*/
bit CON_CLR_RESULT	@ BITS.4;	/*RESULT OF CONSTANT CLR DECISION*/
bit SLEEPTIME		@ BITS.5;	/*IF = 1 COMMAND TO ENTER SLEEP MODE*/ 
bit PAPER_FEED_LOCKOUT	@ BITS.6;	/*IF = 1 PAPER FEED IS LOCKED OUT*/
bit TEAR		@ BITS.7;	/*1=TEAR HAPPENED*/

char FSRSave;
char IRSEN_STATE;	/*STATE MACHINE POINTER FOR IR SENSOR PROGRAM*/
char LOGIC_STATE;	/*STATE MACHINE POINTER FOR LOGIC PROGRAM*/
char LEN_SW;		/* 1 = LENGTH LONG, 2 = LENGTH SHORT, 3 = LENGTH MED*/
char SENS_SW;		/* 1 = SENS MAX, 2 = SENS MIN, 3 = SENS MED*/
char DELAY_SW;		/* 1 = DLY LONG, 2 = DLY SHORT, 3 = LENGTH MED*/
char BATLO_STATE;	/*STATE MACHINE POINTER FOR BATTERY READ PROGRAM*/
char IRSEN_CTR;		/*COUNTER FOR HOW MANY DETECT PASSES OF THE IR SENSOR*/
char TB_LEDDSP_CTR;	/*ONLY THE 1ST 20 DISPENSES ACTIVATE THE TB LED DISPLAY*/
char LPAPER_STATE;	/*BLINK COUNTER STATE*/
char JAM_COUNTER;	/*COUNTER FOR NUMBER OF JAM EVENTS IN A ROW*/
char NTB_COUNT;		/*NO TEAR BAR COUNTER*/
char CON_CLR_STATE;	/*STATE FOR CONSTANT CLR FUNCTION*/
char IRSENCNT_TOTRIG; 	/*NUMBER OF SENSES BEFORE TRIGGER ACTIVATE*/
char NTB_LIMIT_CNT;	/*MASTER LIMIT COUNT DEPENDENT ON THE MODE*/
char M2_CLR_STATE;	/*CLEAR STATE FOR MODE 2*/
char CLR_TOTAL_PASSES;	/*MODE 2 NUMBER OF CLEARS IN A ROW TO DECLARE CLEAR*/
char M2_CLR_RESULT;
char NotClrPasses;
char SleepPassCtr;	/*number of sleep passes to do*/
char UartSendState;
char UartSendPtr;
char UartRxPtr;
char OKCK;
 
uns16 IRSTATE_DLY;
uns16 TEAR_FILTER_CNT;	/*USED FOR FILTERING THE TEAR SIGNAL*/
uns16 LPAPER_CNTR;	/*COUNTER FOR LOW PAPER BLINK ON LED'S*/
uns16 analogval;	/*GENERAL AD RESULT FOR BATTERY VOLTAGE READ*/
uns16 delay_ctr;	/*USED BY DELAYMS() TO KEEP TRACK OF DELAY IN 178USEC INCREMENTS.*/
uns16 SHEET_DLY;	/*SHEET TO NEXT SHEET DELAY COUNTER */
uns16 MAS_SHEET_DLY;	/*SHEET DELAY READ FROM SWITCH*/
uns16 BATLO_BLINK_TMR;	/*COUNTER FOR LOW BATTERY BLINK ON LED'S*/
uns16 BATLO_CTR;	/*TIME DELAY BEFORE BATTERY LOW INDICATION*/
//uns16 TEAR_LED_TMR;	/*USED FOR TEAR LED ON TIME*/
uns16 MOT_OVLD_CTR;	/*COUNTER FOR MOTOR JAM DETECTION*/
uns16 PAPER_ADV_DLY;	/*FIXED DELAY FOR HOW LONG THE PAPER ADV BUTTON CAN BE ACTIVE*/
uns16 CON_CLR_TOT_CTR;	/*WORKING COPY OF CON_CLR_TOTAL_TIME*/
uns16 CON_CLR_TOTAL_TIME; /*MASTER VALUE FOR CONSTANT CLR FUNCTION*/
uns16 CON_CLR_TIME;	/*MASTER SLIDING WINDOW FOR DETECTING CLEAR*/
uns16 CON_CLR_TIME_CTR;	/*WORING COPY OF CON_CLR_TIME*/
uns16 JAM_TMR;		/*COUNTER FOR DELAY BETWEEN JAM EVENTS*/
uns16 DISP_TMR;		/*MAX TIME FOR DISPENSE*/
uns16 PERIPH_CTR;
uns16 BatValNow;

#pragma rambank = 1
char BLCNT_STATE;
char BLCNT_INDX;
char UartSendCount;
char y;
char UartStartupState;
char UartTransferState;
char UartGotCR;
char BLCNT_VAL;		/*CURRENT COUNT VALUE*/
char BLCNT[4];		/*THESE ARE THE 4 VERSION NUMBER LOCATIONS*/
char DoorOpenSeq;
char DoorCloseSeq;
char Temp;		/*used for data transfer*/
char Temp1;		/* used for data transfer*/
char Temp2;		/* used for data transfer*/
char Tmp;		/* GP LOCATION */
char Tmp1;		/* GP LOCATION*/
char LOGIC_MODE;	/*LOGIC MODE 0 =  NORMAL, 1=NOT USED 2 = STADIUM MODE*/	
char INTDummy;
char CID;		/*control id number*/
uns24 SleepExtendCtr;	/*COUNTDOWN FOR ENTRY INTO THE EXTENDED SLEEP MODE*/
uns16 UsartSendDly;	/*DELAY BETWEEN SNAP SEND*/
uns16 Local2;
uns16 TBErr_Reset_Ctr;	/*COUNTER FOR THE TB ERROR AUTO RESET IF CLEAR FOR X TIME*/
uns16 SPCL_TIMER;	/*SPECIAL OPERATIONS TIMER*/
uns16 UartRxDLY;
uns16 LED_CKSUM;
uns16 DoorCloseTmr;
uns16 AnChkVal;
uns16 StartupDelay;
uns16 x;
uns16 CurBatVal;	/*AD RESULT FOR BATTERY VOLTAGE READ*/
uns16 M2_DLY_CTR;
union {
	char C[4];	/*TOTAL SINCE BORN MSB = 0...LSB=4*/
	uns24 L;
} ShtTSCCNT;

union {
	char C[3];	/*TOTAL SINCE LAST BATTERY CHANGE MSB = 0...LSB=3*/
	uns24 L;
} ShtTLSBCNT;

union {
	char C[3];	/*TOTAL SINCE CURRENT BATTERY CHANGE MSB = 0...LSB=3*/
	uns24 L;
} ShtTCBCNT;

union {
	char C[2];	/*TOTAL BATTERY CHANGES SINCE BORN MSB = 0...LSB=2*/
	uns16 L;
} ShtTBSBCNT;

union {
	char C[2];	/*VOLTAGE AT LAST BATTERY CHANGE*/
	uns16 L;
} ShtVLBCCNT;
char B2ASCBuf[8];
#pragma rambank = 2
char UartSendBlock[50];
char UartRxBlock[30];
#pragma rambank = 0

 /* function definitions */
void init(void);			/*INITAILIZATION OF THE PERIPHERIALS*/
void delayms(uns16 ms);			/*DELAY ROUTINE PARAMETER*/
void paper_dispense(void);		/*DISPENSE ONE SHEET OF PAPER*/
void paper_advance(void);		/*ADVANCE PAPER WHEN BUTTON PRESSED*/
void keyboard(void);			/*SCAN SLIDE SW CAB OPEN FUEL PAPER FEED AND LOGIC MODE*/
void Chk_Hand_Sensor(void);		/*ROUTINE TO CHECK IR HAND SENSOR*/
void Chk_Battery_Volts(void);		/*ROUTINE TO CHECK BATTERY VOLTAGE*/
void DO_LOGIC_0(void);			/*ROUTINE TO DO THE LOGIC (LOGIC_MODE=0)*/
void DO_LOGIC_2(void);			/*ROUTINE FOR ALTERNATE LOGIC (LOGIC_MODE=2)*/
void Disp_Low_Paper(void);		/*LOW PAPER FLASHER*/
void TEAR_LED(void);			/*LED INDICATOR FOR TEAR BAR */
void CHK_JAM_EVENTS(void);		/*LOGIC FOR RESETTING JAM EVENTS*/
void ARM_IR(bit SLP);			/*REARM THE IR ROUTINE*/
void CHK_CON_CLR(void);			/*CHECK FOR CONSTANT CLEAR*/
void M2_CON_CLR(void);
void SleepTimeCorrection(void);
void writeByteEE(char wraddr, char wrdata);	/*WRITE EE DATA TO addr*/
void ADV_TSB_SHT_CNT(void);
char readByteEE(char addr);		/*EEPROM READ PROGRAM*/
void ADV_TLSB_SHT_CNT(void);
void ADV_TBSB_SHT_CNT(void);
void ADV_TCBC_SHT_CNT(void);
void ChkDoorOpenOps(void);
void ChkDoorCloseOps(void);
void UartSendMachine(void);
void UartStartupData(void);
void SendStringCNF(void);
void SendStringDNS(void);
void SendStringKA(void);
void UartTransferData(void);
void SendStringOPEN(void);
void SendData1(void);
void SendData2(void);
void BIN2ASC(uns24 InNum);
void SendData3(void);
void StoreB2AscBuf( uns24 BinNum, uns8 Indx);
void SendData4(void);
bit CHK_SPECIAL_OPS(void);		/*CHECK FOR SPECIAL RESET OR LED DATA TRANSFER*/

/*------------------------------------------------------------------------------*/
/********************	INTERRUPT STK=2		*********************************/
/*------------------------------------------------------------------------------*/
#pragma origin = 4

interrupt hrt_int( void){
	int_save_registers    /* W, STATUS (and PCLATH if required)*/

/*EXTERNAL INTERRUPT THIS IS THE TEAR BAR INTERRUPT*/
	if(INTF && INTE){
		if(!TEAR_FILTER_CNT){
			if (!Tear_Input){	/*CHECK IN CASE OF FALSE INTERRUPT*/
				TEAR = 1;	/*ARM THE FLAG*/
//				TEAR_LED_TMR = DLY_250_MS;
//				TB_LED_FLG = 1;	/*FLAG TO TURN LED ON*/
//				TEAR_FILTER_CNT = DLY_500_MS;
			}
		}	
	INTF = 0;
	}


/*TIMER1 INTERRUPT */
	if (TMR1IF && TMR1IE){
		TMR1IF= 0;
		TMR1IE= 0;
	}


/*USART INTERRUPT */

	if (RCIF && RCIE){
		FSRSave = FSR;
		INTDummy = RCREG;
		if((INTDummy == LF) && (OKCK == 3)){
			UartGotCR = 1;		/*signal end of a message*/
			OKCK = 0;
		}
		if(INTDummy == 'O'){
			OKCK =1;
		}
		if((INTDummy == 'K') && (OKCK == 1)){
			OKCK = 3;
		}
		if(UartRxPtr < (sizeof(UartRxBlock))){
			if((UartGotCR == 0) && (INTDummy != CR) && (INTDummy != LF)){	/*No Cr or Lf*/
				UartRxBlock[UartRxPtr] = INTDummy;
				UartRxPtr++;
			}
		}
		if( OERR){
			CREN = 0;
			CREN = 1;
		}
	FSR = FSRSave;
	}


/*THIS IS THE TIMER0 INTERRUPT AT 2.7933KHZ 358 MICROSECONDS*/
	if(T0IF && T0IE){
		TMR0	= 90;		/*RE-SET FOR INTERRUPT OF 2.7933KHZ ADJ FOR EXTRA INST.*/
		T0IF	= 0;

		if (PORTA.3){
			PORTA.3=0;	/*TOGLE FOR TEST*/
			}
		else{
			PORTA.3=1;
			}

		if (UsartSendDly){
			UsartSendDly++;
		}
		if (SHEET_DLY){
			SHEET_DLY++;
		}
		if (BATLO_CTR){
			BATLO_CTR++;
		}
		if (BATLO_BLINK_TMR){
			BATLO_BLINK_TMR++;
		}
		if (TEAR_FILTER_CNT){
			TEAR_FILTER_CNT++;
		}
		if (delay_ctr){
			delay_ctr++;	/*THIS IS USED FOR MAIN ROUTINE DELAYS*/
		}
	
		if (LPAPER_CNTR){
			LPAPER_CNTR++;	/*FOR THE LOW PAPER BLINK*/
		}

//		if (TEAR_LED_TMR){
//			TEAR_LED_TMR++;
//		}

		if (MOT_OVLD_CTR){
			MOT_OVLD_CTR++;
		}
		
		if (PAPER_ADV_DLY){
			PAPER_ADV_DLY++;
		}
		
		if (CON_CLR_TOT_CTR){
			CON_CLR_TOT_CTR++;
		}

		if (CON_CLR_TIME_CTR){
			CON_CLR_TIME_CTR++;
		}
				
		if (TBErr_Reset_Ctr){
			TBErr_Reset_Ctr++;
		}

		if (JAM_TMR){
			JAM_TMR++;
		}

		if (PERIPH_CTR){
			PERIPH_CTR++;
		}
		
		if (DISP_TMR){
			DISP_TMR++;
		}

		if(M2_DLY_CTR){
			M2_DLY_CTR++;
		}

		if(SleepExtendCtr){
			SleepExtendCtr++;
		}
		if (SPCL_TIMER){
			SPCL_TIMER++;
		}
		if (UartRxDLY){
			UartRxDLY++;
		}
		if (StartupDelay){
			StartupDelay++;
		}
		if (DoorCloseTmr){
			DoorCloseTmr++;
		}
		Chk_Hand_Sensor();	/*CHECK FOR SHEET REQUEST*/
	}
	int_restore_registers	/* W, STATUS (and PCLATH if required)*/
}



/******************************************************************************************************/
/********************			MAIN ROUTINE			*******************************/
/******************************************************************************************************/

#include "math24.h"

void main(void){
start:
nop();
start1:	GIE=0;
	init();
	/* START INTERRUPTS */
	INTCON	= 0b1111.0000;	/*TIMER 0 INTS ENABLED, PERIF ENABLED AND GLOBAL ENABLE ON*/
	TEAR= 0;		
	IR_SENSED = 0;
/*START INFINITE LOOP */
	while (StartupDelay){
		nop();
	}
	GO = 1;		/*DUMMY READ*/
	keyboard();	/*DUMMY PASS*/
		
	while(1){
		if ((SLEEPTIME) && (IRSEN_STATE == 0) && (SPCL_FLAG == 0) && (Stop_AD_Reads == 0)){
			GIE=0;
			T0IE=0;		/*STOP TIMER 0*/
			IREn = 1;	/*TURN OFF RX*/
			SLEEPTIME=0;	/*RESET SLEEP FLAG*/
			/*SLEEP FOR 0.132 SECONDS*/
			SleepPassCtr = SleepM0_Dly;
//			GetSleepCtr();		/*SET UP SleepPassCtr*/
			while (SleepPassCtr){
				#if (DEBUG == 0)
				clrwdt();
				SWDTEN = 1;	/*WDT ON FOR WAKEUP*/
    				sleep();
				#endif
				SleepPassCtr--;
				/*----------------------*/
				/*COME OUT OF SLEEP HERE*/
				/*----------------------*/
				nop();
				SWDTEN = 0;	/*WDT OFF*/
				/*HERE ACCOUNT FOR LOST COUNTS WHILE SLEEP HAPPENED*/
				SleepTimeCorrection();
			}

			T0IE=1;
			GIE=1;

		}
		T0IE = 0;
		T0IE = 1;
		if(PERIPH_CTR == 0){	/*NO NEED TO DO THESE PROGRAMS MORE THAN 10X SEC*/
			PERIPH_CTR = DLY_100_MS;
			keyboard();		/*CHECK SWITCHES*/
			Chk_Battery_Volts();	/*CHECK LOW BATTERY VOLTAGE*/
		}
		if (SPCL_FLAG == 0){	/*do not run these programs if DATA LED PROGRAM IS ACTIVE*/
			DO_LOGIC_2();		/*LOGIC STATE MACHINE FOR NEW LOGIC_MODE = 2*/
			M2_CON_CLR();
			Disp_Low_Paper();	/*DISPLAY FOR LOW PAPER*/
//			TEAR_LED();
 			paper_advance();	/*CHECK PAPER ADVANCE FUNCTION*/
			if (DOOR_OPEN){
				ChkDoorOpenOps();	/*special door open operations for LED transfer stuff*/
				ARM_IR(0);	/*ARM IR WITH NO SLEEP AND CLR SENSED*/
				LOGIC_STATE = 0;	/*INIT DISPENSE LOGIC*/
				JAM_FLAG =0;	/*RELEASE JAM ERROR ALSO*/
				JAM_COUNTER = 0;
				NTB_COUNT = 0;	/*CLEAR SO 1 SEC DELAY AFTER NO TEAR*/
				TEAR = 0;
				if (!TB_LED_FLG){
					if(DISP_VER_DONE == 1){
						LOWBAT_R = 0;	/*ONLY IF THE VERSION IS DONE*/
					}
					LOWBAT_G = 0;
				}
			}
			else{
				ChkDoorCloseOps( );
			}	
		}

		DO_LOGIC_0();		/*LOGIC STATE MACHINE FOR OLD MODE WHEN LOGIC MODE = 0*/
		CHK_JAM_EVENTS();		
		CHK_CON_CLR();
		UartSendMachine();
		UartStartupData();
		UartTransferData();

		if((SPCL_FLAG) && (DoorCloseTmr == 0) && (DOOR_OPEN == 0)){
			SPCL_FLAG = 0;	/*after timeout turn off the LED reader program*/
		}
		if (CHK_SPECIAL_OPS()){	/*CHECK FOR MANUAL RESET OR LED DATA TRANSFER*/
			goto start1;	/*RE-BOOT*/
		}
	}
}
/* end of main*/


/*------------------------------------------------------------------------------*/
/********************	SUBROUTINES		*********************************/
/*------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------*/
/*	THIS IS THE MAIN SYSTEM LOGIC AND CONTROL PROGRAM stk=2			*/
/*THIS IS THE STANDARD MODE IN ALL VERSIONS UP TO VERSION 5			*/
/*------------------------------------------------------------------------------*/
void DO_LOGIC_0(void){
	if(SLEEPTIME || DOOR_OPEN || CON_CLR_STATE || IRSEN_STATE || (LOGIC_MODE != 0)){
		return;		
	}

	NTB_LIMIT_CNT = NTB_LIMIT_CNT0;
	if(SHEET_DLY){
		ARM_IR(1);
		return;
	}

	/*CHECK BOUNDS FOR "LOGIC_STATE" */
	if (LOGIC_STATE > 9){
		LOGIC_STATE=0;
	}
	
	switch (LOGIC_STATE){
		/*WAIT HERE FOR HAND TO TRIGGER */
		case 0:{
			if(!IRSEN_STATE){
				if (IR_SENSED){
					LOGIC_STATE = 1;	/*STARTUP DISPENSE LOGIC*/
				}
				else{
					IRSENCNT_TOTRIG = 2;
					ARM_IR(0);	/*ARM IR ALLOW SLEEP*/
				}
			}
		break;
		}
		
		/*DISPENSE*/
		case 1:{
			paper_dispense();
			T0IE = 0;
			T0IE = 1;
			SHEET_DLY = MAS_SHEET_DLY;
			LOGIC_STATE = 2;	/*NEXT STATE*/
			break;
		}
		

		/*DECIDE WHAT TO DO AFTER A DISPENSE*/
		case 2:{
			if (TEAR){
				LOGIC_STATE = 0;	/* GO TO CASE 0*/
				NTB_COUNT = 0;		
				SHEET_DLY = MAS_SHEET_DLY;
				break;
			}
			/*SET UP TIMING FOR AN INSTACLR CHECK*/
			CON_CLR_TOTAL_TIME = DLY_100_MS;
			CON_CLR_TIME = DLY_90_MS;
			CON_CLR_STATE = 1;	/*START UP THE CLEAR DECISION*/
			LOGIC_STATE = 3;
			break;
		}

		/*CHECK FOR CLEAR AND DECIDE WHAT TO DO NEXT*/
		case 3:{
			if (CON_CLR_RESULT){			/*IF INSTACLR WAS CLEAR*/
				LOGIC_STATE = 4;		/*BRANCH TO LOGIC STATE 4*/
			}
			else{
				LOGIC_STATE = 6;		/*ELSE BRANCH TO LOGIC STATE 6*/
			}
			break;
		}
			
			
			
		/*HERE RESULT OF INSTACLR WAS CLEAR AND NO TEAR BAR SENSED */
		case 4:{
			if (NTB_COUNT > NTB_LIMIT_CNT){
				LOGIC_STATE = 8;
			}
			else{
				CON_CLR_TOTAL_TIME = DLY_1_SEC;	/*IF 1ST TIME THEN 1 SEC*/
				CON_CLR_TIME = DLY_0P8_SEC;
				CON_CLR_STATE = 1;
				LOGIC_STATE = 5;
			}
			break;
		}

		/*HERE WE WAIT FOR A CLEAR OR TEAR BAR*/
		case 5:{
			if (CON_CLR_RESULT){	/*IF HAS BEEN CLR FOR SPECIFIED TIME OR TEAR*/
				LOGIC_STATE = 0;	/*GET OUT OF LOOP*/
			}
			else{
				CON_CLR_TOTAL_TIME = DLY_1_SEC;
				CON_CLR_TIME = DLY_0P8_SEC;
				CON_CLR_STATE = 1;
			}
			break;
		}



		/*HERE INSTACLR WAS NOT CLEAR*/
		case 6:{
			CON_CLR_TOTAL_TIME = DLY_4_SEC;
			CON_CLR_TIME = DLY_3P8_SEC;
			CON_CLR_STATE = 1;
			LOGIC_STATE = 7;
			break;
		}
			
		/*HERE INSTACLR WAS NOT CLR AND DELAY IS DONE DECIDE TO GIVE 5" SHEET*/
		case 7:{
			if (CON_CLR_RESULT){
				LOGIC_STATE = 0;	/*WAS CLR OR TEAR, CONTINUE LOGIC*/
			}
			else{
				EX_SHT_FLAG = 1;
				paper_dispense();	/*DO 5" SHEET*/
				EX_SHT_FLAG = 0;
				CON_CLR_TOTAL_TIME = DLY_4_SEC;
				CON_CLR_TIME = DLY_3P8_SEC;
				CON_CLR_STATE = 1;
				LOGIC_STATE = 5;	/*LOOP TILL CLR OR TEAR*/
			}
			break;
		}
		
		/*HERE THE AUTORESET TIMER WILL START IF TOO MANY MISSED TEAR BARS*/
		case 8:{
			TBErr_Reset_Ctr = DLY_3_SEC;
			CON_CLR_TOTAL_TIME = DLY_3_SEC;
			CON_CLR_TIME = DLY_2P8_SEC;
			CON_CLR_STATE = 1;
			LOGIC_STATE = 9;
			break;
		}
			
		case 9:{
			if (CON_CLR_RESULT){
				if (TBErr_Reset_Ctr == 0){
					NTB_COUNT = 0;	/*CLR THE ERROR*/
					LOGIC_STATE = 0;
					break;
				}
			}
			else{
				LOGIC_STATE = 8;	/*IF NOT CLEAR KEEP IN THIS LOOP*/
				break;
			}
		}
			
	}
}

/*------------------------------------------------------------------------------*/
/*	THIS IS THE MAIN SYSTEM LOGIC AND CONTROL PROGRAM #2 stk=2		*/
/*THIS MODE PRESENTS A FULL TOWEL LENGTH EACH TEAR OR THROAT CLEAR.		*/
/*------------------------------------------------------------------------------*/
void DO_LOGIC_2(void){
	if(SLEEPTIME || DOOR_OPEN || IRSEN_STATE || M2_CLR_STATE || (LOGIC_MODE != 0x02)){
		return;		
	}

	NTB_LIMIT_CNT = NTB_LIMIT_CNT2;
	if(SHEET_DLY){
		return;
	}

		
	if(M2_DLY_CTR == 0){
		M2_DLY_CTR = DLY_500_MS;
		if (M2_CLR_RESULT){		/*1 IF INSTACLR FOUND A CLEAR OR A TEAR HAPPENED*/
			M2_CLR_RESULT = 0;
			paper_dispense();	/*DO SHEET*/
			SHEET_DLY = MAS_SHEET_DLY;
		}
		else{
			M2_CLR_STATE = 1;	/*START UP THE CLEAR DECISION*/
		}
	}
	else{
		SLEEPTIME = 1;		/*ALLOW SLEEP MODE*/
	}
}


/*------------------------------------------------------------------------------*/
/*	ROUTINE TO CHECK FOR CONTINUOUS CLEAR FOR MODE 2			*/
/*INPUT:	CLR_TOTAL_PASSES = # OF CONTINUOUS PASSES TO DECLARE CLEAR CONDX*/
/*		M2_CLR_STATE	   = 1 TO START DECISION			*/
/*OUTPUT:	M2_CLR_STATE      = 0 INDICATE DECISION COMPLETE		*/
/*		M2_CLR_RESULT     = 1 IF CLR FOR AT LEAST CON_CLR_TIME		*/
/*IF TEAR HAPPENS THEN IMMEDIATLY RETURN WITH CLR_RESULT = 1			*/
/*------------------------------------------------------------------------------*/
void M2_CON_CLR(void){
	if(IRSEN_STATE){	/*WAIT FOR NEW IR DATA*/
		return;
	}
/*CHECK BOUNDS FOR "CON_CLR_STATE" */
	if (M2_CLR_STATE > 2){
		M2_CLR_STATE = 0;
	}
	
	IRSENCNT_TOTRIG = 4;		/*MOST SENSITIVE TO DETECT CLR*/
	switch (M2_CLR_STATE){
		case 0:{
			break;		/*IDLE STATE*/
		}
		
		/*HERE WE INITIALIZE THE DECISION VARIABLES*/
		case 1:{
			CLR_TOTAL_PASSES = MODE2_PASSES4_CLR;	/*START UP COUNTER*/
			NotClrPasses = 0;
			ARM_IR(1);	/*ARM IR NO SLEEP */
			M2_CLR_STATE = 2;	/*NEXT STATE*/
			break;
		}
		
		case 2:{
			if (TEAR){
				M2_CLR_RESULT = 1;
				SHEET_DLY = MAS_SHEET_DLY;
				NTB_COUNT = 0;
				M2_CLR_STATE = 0;
				break;
			}
			if (IR_SENSED){			/*IR SENSED SO NOT CLEAR*/
				NotClrPasses++;
			}
			if(CLR_TOTAL_PASSES){
				CLR_TOTAL_PASSES--;	/*COUNT TOTAL PASSES*/
				ARM_IR(1);		/*ARM IR WITH NO SLEEP */
			}
			else{	/*HERE TOTAL PASSES ARE DONE*/
				if (NotClrPasses == 0){
					M2_CLR_RESULT = 1; /*ALL WERE CLEAR*/
					if (NTB_COUNT <= NTB_LIMIT_CNT){
						NTB_COUNT++;
					}
				}
				else{
					M2_CLR_RESULT = 0; /*NOT ALL WERE CLEAR*/
				}
				M2_CLR_STATE = 0;
			}
		}
	}
}


/*------------------------------------------------------------------------------*/
/*	ROUTINE TO CHECK FOR CONTINUOUS CLEAR FOR MODE 1			*/
/*INPUT:	CON_CLR_TOTAL_TIME = TOTAL TIME TO MAKE DECISION		*/
/*		CON_CLR_TIME	   = TIME THAT CONTINUOUS CLEAR MUST EXIST	*/
/*		CON_CLR_STATE	   = 1 TO START DECISION			*/
/*OUTPUT:	CON_CLR_STATE      = 0 INDICATE DECISION COMPLETE		*/
/*		CON_CLR_RESULT     = 1 IF CLR FOR AT LEAST CON_CLR_TIME		*/
/*IF TEAR HAPPENS THEN IMMEDIATLY RETURN WITH CON_CLR_RESULT = 1		*/
/*------------------------------------------------------------------------------*/
void CHK_CON_CLR(void){
	if(IRSEN_STATE){	/*WAIT FOR NEW IR DATA*/
		return;
	}
/*CHECK BOUNDS FOR "CON_CLR_STATE" */
	if (CON_CLR_STATE > 2){
		CON_CLR_STATE = 0;
		}
	
	IRSENCNT_TOTRIG = 1;		/*MOST SENSITIVE TO DETECT CLR*/
	switch (CON_CLR_STATE){
		case 0:{
			break;		/*IDLE STATE*/
		}
		
		/*HERE WE INITIALIZE THE DECISION VARIABLES*/
		case 1:{
			CON_CLR_TOT_CTR = CON_CLR_TOTAL_TIME;	/*START UP TIMERS*/
			CON_CLR_TIME_CTR = CON_CLR_TIME;
			ARM_IR(1);	/*ARM IR NO SLEEP */
			CON_CLR_STATE = 2;	/*NEXT STATE*/
			break;
		}
		
		case 2:{
			if (TEAR){
				CON_CLR_RESULT = 1;
				SHEET_DLY = MAS_SHEET_DLY;
				NTB_COUNT = 0;
				CON_CLR_STATE = 0;
				break;
			}
			if (CON_CLR_TOT_CTR){			/*DECISION TIME NOT EXPIRED*/
				if (IR_SENSED){			/*IR SENSED SO NOT CLEAR*/
					if (CON_CLR_TIME_CTR){	/*IF THE CLEAR TIME NOT ALREADY BEEN SATISFIED*/
						CON_CLR_TIME_CTR = CON_CLR_TIME; /*RESET THE CLR TIME TO TRY AGAIN*/
					}
				}
			ARM_IR(1);				/*ARM IR NO SLEEP */
			}
			else{					/*DECISION TIME HAS EXPIRED*/
				if (CON_CLR_TIME_CTR){
					CON_CLR_RESULT = 0;	/* CONTINUOUS CLR TIME HAS NOT BEEN MET*/
				}
				else{
					CON_CLR_RESULT = 1;	/* CONTINUOUS CLR TIME HAS BEEN MET*/
					SHEET_DLY = MAS_SHEET_DLY;
					if (NTB_COUNT <= NTB_LIMIT_CNT){
						NTB_COUNT++;
					}
				}
			CON_CLR_STATE = 0;	/*FINISH*/
			}
			break;
		}
	}
}


/*------------------------------------------------------------------------------*/
/*	ROUTINE TO RE-ARM THE IR SENSE ROUTINE. IF INPUT =0 THEN ALLOW SLEEP.	*/
/*	IF INPUT = 1 THEN NO SLEEP.						*/
/*------------------------------------------------------------------------------*/
void ARM_IR(bit SLP){
	T0IE = 0;	/*TEMP TURN OFF TIMER 0 INT*/
	IRSEN_STATE=1;	/*RESET IRSEN_STATE IF IT WAS 0*/
	IR_SENSED = 0;
	IR_MODE = SLP;	/*SET SLEEP MODE*/
	T0IE = 1;	/*TURN ON TIMER 0 INT*/
}
	
/*------------------------------------------------------------------------------*/
/*	CHECK IR HAND SENSOR RUN FROM INTERRUPT STK=1				*/
/*ROUTINE SETS IR_SENSED = 1 IF HAND IS SENSED					*/
/*SPECIAL MODE IF "IR_MODE" <> 0 THEN DO NOT ALLOW SLEEP			*/
/*THEN LOGIC PROGRAM HAS CONTROL OF SENSOR READ.				*/
/*IRSEN_STATE IS 0 IF IDLE AND 1 TO RUN STATE MACHINE				*/
/*------------------------------------------------------------------------------*/

void Chk_Hand_Sensor(void){
char filt;
	if (DOOR_OPEN){	/*DO NOT RUN IF DOOR OPEN */
		return;
	}

/*CHECK BOUNDS FOR "IRSEN_STATE" */
	if (IRSEN_STATE > 3){
		IRSEN_STATE = 0;
		}

	switch (IRSEN_STATE){
		case 0:{
			IREn = 1;	/*TURN OFF RX*/
			TMR2ON = 0;	/*NOW TURN OFF THE 56KHZ IR WAVE*/
			LOW_IR = 1;
			MID_IR = 1;
			break;
		}
		case 1:{			/*TURN ON RX AND WAIT ABOUT 10MSEC TO STARTUP*/
			IREn=0;			/*TURN ON RX*/
			IRSTATE_DLY = DLY_30_MS; /*ABOUT 30 MSEC*/
			IRSEN_STATE = 2;		/*ADVANCE*/
			break;
		}
		case 2:{			/*AFTER RX STARTUP DELAY START 56 KHZ SIGNAL*/
			IRSTATE_DLY++;
			if (IRSTATE_DLY == 0){
				LOW_IR=1;MID_IR=0;	/*DEFAULT IF NOT 1 2 OR 3 SET FOR MID SENS*/
				if (SENS_SW == 2){
					LOW_IR=0;MID_IR=1;	/*SET FOR LOW SENS*/
				}
				if (SENS_SW == 3){
					LOW_IR=1;MID_IR=0;	/*SET FOR MID SENS*/
				}
				if (SENS_SW == 1){
					LOW_IR=0;MID_IR=0;	/*SET FOR HI SENS*/
				}
				PORTE.0 = 0;
				PORTE.1 = 0;	/*ACTIVATE DEPENDING ON THE TRIS BITS*/
				TMR2ON = 1;	/*STARTUP 56KHZ*/
				IRSEN_STATE = 3;
			}
		break;
		}
		case 3:{		/*CHECK FOR HAND IN DISPENSER*/
			filt.0 = !IR;	/*SMALL DISCRETE TIME FILTER TO KEEP OUT NOISE*/
			filt.1 = !IR;
			filt.2 = !IR;
			filt.3 = !IR;
			filt.4 = !IR;
			filt.5 = !IR;
			filt.6 = !IR;
			filt.7 = !IR;
	
	/*LOGIC MODE 0 IS LOOKING FOR A HAND EVENT, LOGIC_MODE 2 IS LOOKING FOR A CLEAR EVEMT*/ 
			if (LOGIC_MODE == 0){
				IR_SENSED = 0;
				if (filt == 0xFF){
					IRSEN_CTR++;	/*COUNT THIS READ*/
					if (IRSEN_CTR < IRSENCNT_TOTRIG){
						break;
					}
					/*set IR_SENSED*/
					IR_SENSED = 1;
				}
			}
			else{		/*MODE 2 LOOKING FOR A CLEAR*/
				IR_SENSED = 1;
				if (filt == 0x00){
					IRSEN_CTR++;	/*COUNT THIS READ*/
					if (IRSEN_CTR < IRSENCNT_TOTRIG){
						break;
					}
					/*set IR_SENSED*/
					IR_SENSED = 0;
				}
			}

			IRSEN_CTR=0;
			IREn = 1;	/*TURN OFF RX*/
			TMR2ON = 0;	/*NOW TURN OFF THE 56KHZ IR WAVE*/
			LOW_IR = 1;
			MID_IR = 1;
			IRSEN_STATE=0;
			if (IR_MODE){
				break;
			}
			SLEEPTIME = 1;		/*SLEEP FOR SLEEPTIME*/
			break;
		}
	}
}

/*----------------------------------------------------------------------*/
/* DISPENSE PAPER	STK=2						*/
/* DIA OF ROLLER=1.54",,C=PI*D= 4.84",,EACH TRANSITION = 4.84/8=.604"	*/
/* PLUS 1 BECAUSE OF THE --cntr LOOP STARTUP				*/
/*----------------------------------------------------------------------*/

void paper_dispense(){
char cntr;
	if((NTB_COUNT > NTB_LIMIT_CNT) || (JAM_FLAG) || (Stop_AD_Reads)){
		return;			/* DO NOT ENTER IF JAM DETECTED*/
	}

	if (EX_SHT_FLAG == 1){
		if (LOGIC_MODE == 0){
			cntr = 9;
		}
	}

	else{				/*ELSE DO NORMAL SHEET LENGTH*/
		if (LEN_SW==2)cntr=15;		/* 8 in / 0.604 = 14 +1 =15 (8.45")*/
		if (LEN_SW==3)cntr=19;		/* 12 in / 0.604 = 18 + 1 =19 (10.87")*/
		if (LEN_SW==1)cntr=24;		/* 14 in / 0.604 = 23 + 1 =24 (13.9")*/

	}
	ProxEn=0;			/*PROX EMITTER ON*/
	CHS0=1;				/*SETUP CHANNEL FOR CHAN1 = Voc*/
	CHS1=0;
	MOT_OVLD_CTR = JAM_DLY_TIME;	/*RESET TIMER FOR OVLD*/
	delayms (DLY_40_MS);		/*GIVE TIME FOR PROX SENSOR TO STARTUP*/
	TEAR = 0;
	DISP_TMR = DLY_3_SEC;
	SleepExtendCtr = SleepExtendVal;	/*reset sleep extend counter*/
		while (--cntr){
		if ((TEAR)){
			break;	/*TEAR DURING DISPENSE STOP DISPENSE*/
		}
		WHEEL_TOG = ProxRd;
		while((WHEEL_TOG == ProxRd) && (DISP_TMR)){
		 	/* LOOK FOR TRANSITION ON ROTATING PADDLE WHEEL.*/
			/* CNTR KEEPS TRACK OF NUMBER OF PADDLE TRANSITIONS.*/
			if(!GO){
				ADON=1;			/*AD ON*/
				delayms(DLY_1_MS);
				GO=1;			/*START THE A/D CONVERSION*/
				while (GO);{
				}		/*WAIT FOR CONVERSION DONE*/
				analogval = ADRESH * 256;
				analogval = analogval + ADRESL;	/*GET DATA */
				ADON=0;			/*AD OFF*/

				if (analogval < SAFE_BELOW){
					RELAY=1;
					delayms(DLY_4_MS);
				}
				else if (analogval > MAXI_MOTOR){	
					RELAY=0;	/*CURRENT GREATER THAN SETPOINT; SHUT OFF MOTOR*/
					if (!MOT_OVLD_CTR){
						JAM_FLAG = 1;	/*IF HI CURRENT FOR TOO LONG SET JAM FLAG*/
						ProxEn=1;	/*turn prox emitter OFF*/
						return;
					}
				}
			}
		}
	}
	RELAY=0;
	ProxEn=1;		/*turn prox emitter OFF*/

	ADV_TSB_SHT_CNT();	/*ADV ABSOLUTE TOTAL*/
	ADV_TCBC_SHT_CNT();	/*ADV COUNT CURRENT BATT CHANGE*/
//	if (TB_LEDDSP_CTR){
//		TB_LEDDSP_CTR--;	/*COUNT THIS DISPENSE*/
//	}
	JAM_COUNTER = 0;		/*THIS WAS A GOOD DISPENSE SO RESET JAM FLAG COUNTER*/
}


/*----------------------------------------------------------------------*/
/* ADVANCE THE PAPER	STK=1						*/
/*----------------------------------------------------------------------*/
void paper_advance(){
if (!PAPER_FEED){
	PAPER_FEED_LOCKOUT = 0;	/*RELEASE LOCKOUT*/
	return;
}
if ((PAPER_FEED_LOCKOUT) || (Stop_AD_Reads)){
	return;
}
	PAPER_ADV_DLY = PAPER_ADV_TIME;	/*SET MAX TIME TO RUN THIS LOOP*/
	while(PAPER_FEED && DOOR_OPEN){	/*DOOR IS OPEN AND FEED BUTTON PRESSED*/
		if(PAPER_ADV_DLY == 0){
			RELAY=0;
			PAPER_FEED_LOCKOUT = 1;	/*WHEN TIME IS UP LOCKOUT TILL BUTTON RELEASE*/
			return;
		}
		if(!GO){
			CHS1=0;
			CHS0=1;					/*SETUP CHANNEL (CHAN1 = Voc)*/
			ADON=1;					/*AD ON*/
			delayms(DLY_1_MS);                      /*DELAY 4MS FOR A/D SETTLE AFTER TURN ON*/
			GO=1;					/*START THE A/D CONVERSION*/
			while (GO);{
			}					/*WAIT FOR CONVERSION DONE*/
			analogval = ADRESH * 256;
			analogval += ADRESL;			/*GET DATA LOCAL*/
			ADON=0;					/*AD OFF*/

			if (analogval > MAXI_MOTOR){		/*CURRENT DANGEROUSLY HIGH*/
				RELAY=0;
			}
			else if (analogval < SAFE_BELOW){
				RELAY=1;
				delayms(DLY_4_MS);		/*AT LEAST 4ms BEFORE CHECK AGAIN*/
			}
		keyboard();				/*CHECK SWITCHES AGAIN*/
		}
	}
RELAY=0;						/*ADVANCE BUTTON RELEASED OR TIMEOUT MOTOR OFF*/
}

/*----------------------------------------------------------------------*/
/*CHECK BATTERY VOLTS BY USING CHANNEL 0 A/D				*/
/*CALLED FROM MAIN LOOP							*/
/*RESISTOR DIVIDER IS 0.3125 AND FULL SCALE =  3.3 VOLTS TRIP IS 5.2 V	*/
/*SO CHECK VALUE I= ((0.3125 * 5.2)/3.3) * 1024	== 504			*/
/*BEFORE ALARM WAIT 4 SEC STK=2						*/
/*----------------------------------------------------------------------*/
void Chk_Battery_Volts(void){
	if ((DOOR_OPEN == 0) && (!TB_LED_FLG) && (Stop_AD_Reads == 0)){

			if(!GO){				/*IF AD IS IN USE JUST EXIT*/
				PORTB.1 = 1;			/*TURN ON BATT DIVIDER*/
				CHS1=0;
				CHS0=0;				/*SETUP CHANNEL (CHAN 0 = VLB)*/
				ADON=1;				/*TURN ON AD*/
				delayms(DLY_1_MS);		/*LET INPUT SETTLE*/
				GO=1;				/*START CONVERSION*/
				while (GO);{
				}				/*WAIT FOR AD TO FINISH*/
				analogval = ADRESH * 256;
				analogval = analogval + ADRESL;	/*GET DATA LOCAL*/
				ADON=0;				/*AD OFF*/
				BatValNow = analogval;
				PORTB.1 = 0;			/*TURN OFF BATT DIVIDER*/
				if( analogval > BATT_LOW_VAL){	/*IF BATTERY IS OK*/
					BATLO_CTR = DLY_4_SEC;	/*DELAY BEFORE INDICATION OF LOW BATTERY*/
					LOWBAT_G=0;LOWBAT_R=0;
			 		BATLO_STATE=0;
			 	}
			if (BATLO_CTR == 0 && BATLO_STATE ==0){	/*HERE BATT IS LOW*/
					BATLO_STATE =1;	/*STARTUP BLINK */
				}
			if (BATLO_BLINK_TMR == 0){

				/*CHECK BOUNDS FOR "BATLO_STATE" */
				if (BATLO_STATE > 3){
					BATLO_STATE = 0;
				}
	
				switch (BATLO_STATE){
					case 0:{
						return;
					}
					case 1:{
						LOWBAT_G=1;LOWBAT_R=1;
						BATLO_STATE++;
						BATLO_BLINK_TMR=DLY_200_MS;
						break;
					}
		
					case 2:{
						LOWBAT_G=0;LOWBAT_R=0;
						BATLO_STATE = 1;;
						BATLO_BLINK_TMR=DLY_2_SEC;
						break;
					}
				}
			}	
		}
	}
}

/*----------------------------------------------------------------------*/
/*COUNT UP IS AT AT 2.7933KHZ RATE.INPUT 65536 - VALUE * 358USEC = DELAY*/
/*				STK=1					*/
/*THIS IS A BLOCKING DELAY ROUTINE.					*/
/*----------------------------------------------------------------------*/

void delayms(uns16 ms){
	delay_ctr = ms;
	while (delay_ctr);{	/*DELAY_CTR DECREMENTED IN INTERRUPT.*/
	}
}


/*----------------------------------------------------------------------*/
/*ROUTINE TO COLLECT THE KEYS AND SWITCHES				*/
/*MAPPED TO DESCRIPITON TEXT	STK=1					*/
/*RUN FROM MAIN LOOP OR ON DEMAND. DO NOT RUN FROM INTERRUPT		*/
/*									*/
/*					DESCRIPTION			*/
/*	DELAY_SW	= 1 IF LONG, 2 IF SHORT, 3 IF MEDIUM		*/
/*	SENS_SW		= 1 IF MAX,  2 IF MIN,   3 IF MEDIUM		*/
/*	LEN_SW		= 1 IF LONG, 2 IF SHORT, 3 IF MEDIUM		*/
/*      LOW_PAPER	= LOW=1						*/
/*	PAPER_FEED	= FEED=1					*/
/*      DOOR_OPEN	= OPEN=1					*/
/*	LOGIC MODE:	0 = ORIGINAL MODE NO HANGING TOWERL		*/
/*			2 = STADIUM MODE				*/
/*----------------------------------------------------------------------*/
void keyboard(void){
	char TEMP;
	PAPER_FEED = !PORTB.5;
	PORTA.4=0;	/*STROBE FOR SWITCHES*/
	nop2();
	TEMP = PORTD;
	PORTA.4 = 1;	/*STROBE OFF*/
	DELAY_SW = 2;	/*FORCE SHORT DELAY HERE TO MAKE LED READER STILL WORK */
	MAS_SHEET_DLY = DLY_500_MS;
	LEN_SW = (TEMP & 0x0C) >> 2;
	SENS_SW = (TEMP & 0x30) >> 4;
	LOW_PAPER = !TEMP.7;
	DOOR_OPEN = !TEMP.6;
	
	PORTE.2 = 0;	/*STROBE 2 FOR MODE*/
	nop2();
	TEMP = PORTA & 0x80;
	PORTE.2 = 1;	/*STROBE OFF*/
	
	if (TEMP){
		LOGIC_MODE = 0;	/*SELECT OPERATING MODE 0*/
	}
	else{
		LOGIC_MODE = 2;	/*SELECT OPERATING MODE 2*/
	}
}

/************************************************/
/*ROUTINE TO CHECK ON THE JAM FLAG AND RESET IT	*/
/*FOR "JAM_FLAG_LIMIT" TIMES TO HELP STOP	*/
/*FALSE SHUTDOWNS.				*/
/*	STK=1					*/
/************************************************/
void CHK_JAM_EVENTS(void){
	if (JAM_FLAG == 0){
		JAM_TMR = JAM_RESET_DLY;
		return;
	}
	if (JAM_COUNTER >= (JAM_FLAG_LIMIT - 1)){
		return;
	}
	if (JAM_TMR == 0){
		JAM_FLAG = 0;		/*GIVE IT ANOTHER CHANCE*/
		JAM_COUNTER++;
	}
}

/************************************************/
/*	DISPLAY LOW PAPER OR JAM ON LED'S	*/
/* PRIORITY IS JAM ERROR THEN TB ERROR THEN	*/
/* LOW PAPER					*/
/*	STK=1					*/
/************************************************/
void Disp_Low_Paper(void){
	if (!DOOR_OPEN ){
		if (JAM_FLAG){	/*RED 1/5 SEC ON 2 SEC OFF*/
			if (LPAPER_CNTR == 0){
				if (LPAPER_STATE == 0){
					LPAPER_G=0;
					LPAPER_R=1;
					LPAPER_CNTR=DLY_200_MS;
					LPAPER_STATE=1;
				}
				else if (LPAPER_STATE == 1){
					LPAPER_G=0;
					LPAPER_R=0;
					LPAPER_CNTR=DLY_2_SEC;
					LPAPER_STATE=0;
				}
			}
		return;
		}

		if (NTB_COUNT > NTB_LIMIT_CNT){	/*RED 1/5 SEC ON 2 SEC OFF*/
			if (LPAPER_CNTR == 0){
				if (LPAPER_STATE == 0){
					LPAPER_G=0;
					LPAPER_R=1;
					LPAPER_CNTR=DLY_200_MS;
					LPAPER_STATE=1;
				}
				else{
					LPAPER_G=0;
					LPAPER_R=0;
					LPAPER_CNTR=DLY_2_SEC;
					LPAPER_STATE=0;
				}
			}
		return;
		}
		
		if (LOW_PAPER){
			if (LPAPER_CNTR ==0){	/* ORANGE 1/5 SEC ON 2 SEC OFF*/
				if (LPAPER_STATE == 0){
					LPAPER_G=0;
					LPAPER_R=0;
					LPAPER_CNTR=DLY_2_SEC;
					LPAPER_STATE=1;
				}
					
				else{
					LPAPER_G=1;
					LPAPER_R=0;
					LPAPER_CNTR=DLY_200_MS;
					LPAPER_STATE=0;
				}
			}
		}
		else{
			LPAPER_G=0;
			LPAPER_R=0;
		}
	}
	else{
			LPAPER_G=0;
			LPAPER_R=0;
	}
}

/****************************************************************/
/*LED DISPLAY FOR TEST OF TEAR BAR. ONLY WHILE TB_LEDDSP_CTR IS	*/
/*NOT = 0							*/
/*STK=1 							*/
/****************************************************************/
//void TEAR_LED(void){
//	if (TB_LEDDSP_CTR){
//		if (TB_LED_FLG){
//			LOWBAT_R = 0;
//			LOWBAT_G = 1;
//		}
//	}
//	if (!TEAR_LED_TMR){
//		TB_LED_FLG = 0;	/*LED FLAG OFF AND LET BATTERY PROGRAM TAKE OVER*/
//	}
//}

/****************************************************************/
/*HERE CORRECT COUNTERS FOR ONE SLEEP TIME PASS			*/
/*STK=1 							*/
/****************************************************************/
void SleepTimeCorrection(void){
#if (DEBUG == 0)
	if(!TO){			/*ONLY IF FULL SLEEP TIME ELAPSED*/
		if (LPAPER_CNTR >= SLEEP_T_VAL){
			LPAPER_CNTR=0;
		}
		else if (LPAPER_CNTR){
			LPAPER_CNTR += SLEEP_T_CNT;
		}

		if (BATLO_CTR >= SLEEP_T_VAL){
			BATLO_CTR=0;
		}
		else if (BATLO_CTR){
			BATLO_CTR += SLEEP_T_CNT;
		}

		if (BATLO_BLINK_TMR >= SLEEP_T_VAL){
			BATLO_BLINK_TMR=0;
		}
		else if (BATLO_BLINK_TMR){
			BATLO_BLINK_TMR += SLEEP_T_CNT;			}
	
		if (SHEET_DLY >= SLEEP_T_VAL){
			SHEET_DLY = 0;
		}
		else if (SHEET_DLY){
			SHEET_DLY += SLEEP_T_CNT;
		}

		if (TEAR_FILTER_CNT >= SLEEP_T_VAL){
			TEAR_FILTER_CNT = 0;
		}
		else if (TEAR_FILTER_CNT){
			TEAR_FILTER_CNT += SLEEP_T_CNT;
		}
		
		if (delay_ctr >= SLEEP_T_VAL){
			delay_ctr = 0;
		}
		else if (delay_ctr){
			delay_ctr += SLEEP_T_CNT;
		}
			
//		if (TEAR_LED_TMR >= SLEEP_T_VAL){
//			TEAR_LED_TMR=0;
//		}
//		else if (TEAR_LED_TMR){
//			TEAR_LED_TMR += SLEEP_T_CNT;
//		}
			
		if (PAPER_ADV_DLY >= SLEEP_T_VAL){
			PAPER_ADV_DLY = 0;
		}
		else if (PAPER_ADV_DLY){
			PAPER_ADV_DLY += SLEEP_T_CNT;
		}
				
		if (CON_CLR_TOT_CTR >= SLEEP_T_VAL){
			CON_CLR_TOT_CTR = 0;
		}
		else if (CON_CLR_TOT_CTR){
			CON_CLR_TOT_CTR += SLEEP_T_CNT;
		}
				
		if (CON_CLR_TIME_CTR >= SLEEP_T_VAL){
			CON_CLR_TIME_CTR = 0;
		}
		else if (CON_CLR_TIME_CTR){
			CON_CLR_TIME_CTR += SLEEP_T_CNT;
		}

		if (M2_DLY_CTR >= SLEEP_T_VAL){
			M2_DLY_CTR = 0;
		}
		else if (M2_DLY_CTR){
			M2_DLY_CTR += SLEEP_T_CNT;
		}

		if (BLCNT_DLY_VAL >= SLEEP_T_VAL){
			BLCNT_DLY_VAL = 0;
		}
		else if (BLCNT_DLY_VAL){
			BLCNT_DLY_VAL += SLEEP_T_CNT;
		}

		if (SleepExtendCtr >= SLEEP_T_VAL_24){
			SleepExtendCtr = 0;
		}
		else if (SleepExtendCtr){
			SleepExtendCtr += SLEEP_T_CNT;
		}

		if (SPCL_TIMER >= SLEEP_T_VAL){
			SPCL_TIMER = 0;
		}
		else if (SPCL_TIMER){
			SPCL_TIMER += SLEEP_T_CNT;
		}
	}
#endif		
}

/****************************************************************/
/*ADVANCE THE TOTAL SINCE BORN SHEET COUNT IN EEPROM EACH COUNT	*/
/* IS 10 DISPENSES						*/
/****************************************************************/
void ADV_TSB_SHT_CNT(void){
		ShtTSCCNT.C[0] = readByteEE(EE_TSB_CNT0); 
		ShtTSCCNT.C[1] = readByteEE(EE_TSB_CNT1); 
		ShtTSCCNT.C[2] = readByteEE(EE_TSB_CNT2);
		ShtTSCCNT.C[3] = readByteEE(EE_TSB_CNT3);
		ShtTSCCNT.L++;
		if ((ShtTSCCNT.C[0] == 0) && (ShtTSCCNT.C[1] == 0) && (ShtTSCCNT.C[0] == 0)){
			ShtTSCCNT.C[3]++;		/*IF AFTER ADVANCE THE 1ST 3 BYTES OVERFLOW*/
		}
		writeByteEE(EE_TSB_CNT0, (ShtTSCCNT.C[0]));
		writeByteEE(EE_TSB_CNT1, (ShtTSCCNT.C[1]));
		writeByteEE(EE_TSB_CNT2, (ShtTSCCNT.C[2]));
		writeByteEE(EE_TSB_CNT3, (ShtTSCCNT.C[3]));
}

/****************************************************************/
/*ADVANCE THE TOTAL SINCE LAST BATTERY CHANGE SHEET COUNT	*/
/* IN EEPROM.							*/
/*								*/
/****************************************************************/
void ADV_TLSB_SHT_CNT(void){
	ShtTLSBCNT.C[0] = readByteEE(EE_TLBC_CNT0); 
	ShtTLSBCNT.C[1] = readByteEE(EE_TLBC_CNT1); 
	ShtTLSBCNT.C[2] = readByteEE(EE_TLBC_CNT2);
	ShtTLSBCNT.L++;
	writeByteEE(EE_TLBC_CNT0, (ShtTLSBCNT.C[0]));
	writeByteEE(EE_TLBC_CNT1, (ShtTLSBCNT.C[1]));
	writeByteEE(EE_TLBC_CNT2, (ShtTLSBCNT.C[2]));
}

/****************************************************************/
/*ADVANCE THE TOTAL SINCE CURRENT BATTERY CHANGE SHEET COUNT	*/
/* IN EEPROM.							*/
/*								*/
/****************************************************************/
void ADV_TCBC_SHT_CNT(void){
	ShtTCBCNT.C[0] = readByteEE(EE_TCBC_CNT0); 
	ShtTCBCNT.C[1] = readByteEE(EE_TCBC_CNT1); 
	ShtTCBCNT.C[2] = readByteEE(EE_TCBC_CNT2);
	ShtTCBCNT.L++;
	writeByteEE(EE_TCBC_CNT0, (ShtTCBCNT.C[0]));
	writeByteEE(EE_TCBC_CNT1, (ShtTCBCNT.C[1]));
	writeByteEE(EE_TCBC_CNT2, (ShtTCBCNT.C[2]));
}

/****************************************************************/
/*ADVANCE THE TOTAL BATTERY CHANGES SINCE BORN COUNT IN EEPROM  */
/* 								*/
/****************************************************************/
void ADV_TBSB_SHT_CNT(void){
	ShtTBSBCNT.C[0] = readByteEE(EE_TBSB_CNT0); 
	ShtTBSBCNT.C[1] = readByteEE(EE_TBSB_CNT1); 
	ShtTBSBCNT.L++;
	writeByteEE(EE_TBSB_CNT0, (ShtTBSBCNT.C[0]));
	writeByteEE(EE_TBSB_CNT1, (ShtTBSBCNT.C[1]));
}

/****************************************************************/
/*SPECIAL OPERATIONS: -IF DOOR CLOSED AND FEED SWITCH FOR > 1SEC*/
/*THEN RESET PROGRAM BY RETURNING A 1 FOR ALL OTHER CONDX RET 0	*/
/* -IF DOOR CLOSED AND FEED SWITCH FOR < 1SEC THEN DO DATA 	*/
/*  TRANSFER OVER THE LED'S					*/
/*STK=1 							*/
/****************************************************************/
bit CHK_SPECIAL_OPS(void){
bit RetVal = 0;
	if ((!DOOR_OPEN && PAPER_FEED) || (SPCL_FLAG)){
		SPCL_FLAG = 1;		/*NO EXIT WITHOUT RESET*/
		if (PAPER_FEED){
			if(SPCL_TIMER == 0){	/*more than 1 sec is a reset*/
				RetVal = 1;
			}
		}
		else{
			SPCL_TIMER = DLY_1_SEC;
	
		}
	}
	else{				/*NORMAL OPERATION*/
		SPCL_TIMER = DLY_1_SEC;
	}
	return (RetVal);
}

/*----------------------------------------------------------------------*/
/*RETURNS THE CHARACTER STORED IN EEPPROM AT THE ADDRESS (char addr)	*/
/*STK =1								*/
/*----------------------------------------------------------------------*/
char readByteEE(char addr){
	EEADR = addr;
	RD = 1;
	return (EEDATA);
}

/*----------------------------------------------------------------------*/
/*WRITES THE CHARACTER (char data) AT THE ADDRESS (char addr)		*/
/*STK =1								*/
/*----------------------------------------------------------------------*/
void writeByteEE(char wraddr, char wrdata){
	GIE = 0;		/*INTS OFF*/
 	Tmp = readByteEE(wraddr);
	if (Tmp == wrdata){
		GIE = 1;
		return;		/*data is the same so dont update*/
	}
	EEADR = wraddr;
	EEDATA = wrdata;
	WREN = 1;
	RP1 = 0;
	RP0 = 1;		/*BANK =1*/
	#asm
;---------------------------------------------
;SPECIAL WRITE SEQUENCE
	MOVLW 0x55 		;Write 55h to
	MOVWF EECON2 		;EECON2

	MOVLW 0xAA 		;Write AAh to
	MOVWF EECON2 		;EECON2

	BSF EECON1, WR 		;Start write operation
;---------------------------------------------
#endasm
	while (WR){
	}			/*WAIT FOR WRITE TO FINISH*/
	WREN = 0;		/*DISABLE WRITES*/
	GIE = 1;
}

/****************************************************************/
/*SEND DATA OUT OF UartSendBlock using UartSendCount		*/
/*ADD AT+ TO THE BEGINNING AND CR LF TO END			*/
/*STK=1 							*/
/****************************************************************/
void UartSendMachine(void){
	if((UartSendCount) && (TRMT)){	/*COUNT NOT ZERO AND TR IS EMPTY*/
		switch (UartSendState){
			case 00:{
				if(UartNoAT){	/*bypass sending AT+*/
					UartSendState = 3;
					UartSendPtr = 0;
					break;
				}
				TXREG = 'A';
				UartSendState = 1;
				break;
			}
			case 01:{
				TXREG = 'T';
				UartSendState = 2;
				break;
			}
			case 02:{
				TXREG = '+';
				UartSendState = 3;
				UartSendPtr = 0;
				break;
			}
			case 03:{
				TXREG = UartSendBlock[UartSendPtr];
				UartSendPtr++;
				UartSendCount--;
				if(UartSendCount == 0){
					UartSendCount = 2;
					UartSendState = 4;
				}
				break;
			}
			case 04:{
				if(UartNoLF){
					UartSendState = 0;
					UartSendCount = 0;	/*done if no cr lf to send*/
					break;
				}
				TXREG = CR;
				UartSendState = 5;
				break;
			}
			case 05:{
				TXREG = LF;
				UartSendState = 0;
				UartSendCount = 0;
				break;
			}
		}
	}
	
}

/****************************************************************/
/*helper routine to take a binary number conver to ascii	*/
/*then put into B2ASCBuf					*/
/* input BinNum = number to convert to 8 digits			*/
/*       Indx = index to store ascii into buffer B2ASCBuf	*/ 
/****************************************************************/
void StoreB2AscBuf( uns24 BinNum, uns8 Indx){
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

#pragma codepage 1

/****************************************************************/
/*SEND STRING HTTPCONF=3,keep-alive				*/
/*STK=1 							*/
/****************************************************************/
void SendStringKA(void){
	UartSendBlock[0]= 'H';
	UartSendBlock[1]= 'T';
	UartSendBlock[2]= 'T';
	UartSendBlock[3]= 'P';
	UartSendBlock[4]= 'C';
	UartSendBlock[5]= 'O';
	UartSendBlock[6]= 'N';
	UartSendBlock[7]= 'F';
	UartSendBlock[8]= '=';
	UartSendBlock[9]= '3';
	UartSendBlock[10]= ',';
	UartSendBlock[11]= 'k';
	UartSendBlock[12]= 'e';
	UartSendBlock[13]= 'e';
	UartSendBlock[14]= 'p';
	UartSendBlock[15]= '-';
	UartSendBlock[16]= 'a';
	UartSendBlock[17]= 'l';
	UartSendBlock[18]= 'i';
	UartSendBlock[19]= 'v';
	UartSendBlock[20]= 'e';
	UartSendCount = 21;
	UartGotCR = 0;
	UartNoAT = 0;	/*send AT+ AT START */
	UartNoLF = 0;	/*send CR LF at end*/
	UartRxPtr = 0;
}

/****************************************************************/
/*SEND STRING HTTPCONF=11,=mars.bwgrp.com			*/
/*STK=1 							*/
/****************************************************************/
void SendStringCNF(void){
	UartSendBlock[0]= 'H';
	UartSendBlock[1]= 'T';
	UartSendBlock[2]= 'T';
	UartSendBlock[3]= 'P';
	UartSendBlock[4]= 'C';
	UartSendBlock[5]= 'O';
	UartSendBlock[6]= 'N';
	UartSendBlock[7]= 'F';
	UartSendBlock[8]= '=';
	UartSendBlock[9]= '1';
	UartSendBlock[10]= '1';
	UartSendBlock[11]= ',';
	UartSendBlock[12]= 'm';
	UartSendBlock[13]= 'a';
	UartSendBlock[14]= 'r';
	UartSendBlock[15]= 's';
	UartSendBlock[16]= '.';
	UartSendBlock[17]= 'b';
	UartSendBlock[18]= 'w';
	UartSendBlock[19]= 'g';
	UartSendBlock[20]= 'r';
	UartSendBlock[21]= 'p';
	UartSendBlock[22]= '.';
	UartSendBlock[23]= 'c';
	UartSendBlock[24]= 'o';
	UartSendBlock[25]= 'm';
	UartSendCount = 26;
	UartGotCR = 0;
	UartNoAT = 0;	/*send AT+ AT START */
	UartNoLF = 0;	/*send CR LF at end*/
	UartRxPtr = 0;
}

/****************************************************************/
/*SEND STRING dnslookup=mars.bwgrp.com.				*/
/*STK=1 							*/
/****************************************************************/
void SendStringDNS(void){
	UartSendBlock[0]= 'd';
	UartSendBlock[1]= 'n';
	UartSendBlock[2]= 's';
	UartSendBlock[3]= 'l';
	UartSendBlock[4]= 'o';
	UartSendBlock[5]= 'o';
	UartSendBlock[6]= 'k';
	UartSendBlock[7]= 'u';
	UartSendBlock[8]= 'p';
	UartSendBlock[9]= '=';
	UartSendBlock[10]= 'm';
	UartSendBlock[11]= 'a';
	UartSendBlock[12]= 'r';
	UartSendBlock[13]= 's';
	UartSendBlock[14]= '.';
	UartSendBlock[15]= 'b';
	UartSendBlock[16]= 'w';
	UartSendBlock[17]= 'g';
	UartSendBlock[18]= 'r';
	UartSendBlock[19]= 'p';
	UartSendBlock[20]= '.';
	UartSendBlock[21]= 'c';
	UartSendBlock[22]= 'o';
	UartSendBlock[23]= 'm';
	UartSendCount = 24;
	UartGotCR = 0;
	UartNoAT = 0;	/*send AT+ AT START */
	UartNoLF = 0;	/*send CR LF at end*/
	UartRxPtr = 0;
}

/****************************************************************/
/*SEND STRING httpopen=54.255.101.110				*/
/*STK=1 							*/
/****************************************************************/
void SendStringOPEN(void){
	UartSendBlock[0]= 'h';
	UartSendBlock[1]= 't';
	UartSendBlock[2]= 't';
	UartSendBlock[3]= 'p';
	UartSendBlock[4]= 'o';
	UartSendBlock[5]= 'p';
	UartSendBlock[6]= 'e';
	UartSendBlock[7]= 'n';
	UartSendBlock[8]= '=';
	UartSendBlock[9]= '5';
	UartSendBlock[10]= '4';
	UartSendBlock[11]= '.';
	UartSendBlock[12]= '2';
	UartSendBlock[13]= '2';
	UartSendBlock[14]= '5';
	UartSendBlock[15]= '.';
	UartSendBlock[16]= '1';
	UartSendBlock[17]= '0';
	UartSendBlock[18]= '1';
	UartSendBlock[19]= '.';
	UartSendBlock[20]= '1';
	UartSendBlock[21]= '1';
	UartSendBlock[22]= '0';
	UartSendCount = 23;
	UartGotCR = 0;
	UartNoAT = 0;	/*send AT+ AT START */
	UartNoLF = 0;	/*send CR LF at end*/
	UartRxPtr = 0;
}

/****************************************************************/
/*SEND STRING DATA PART1					*/
/*STK=1 							*/
/****************************************************************/
void SendData1(void){
	UartSendBlock[0]= 'h';
	UartSendBlock[1]= 't';
	UartSendBlock[2]= 't';
	UartSendBlock[3]= 'p';
	UartSendBlock[4]= 's';
	UartSendBlock[5]= 'e';
	UartSendBlock[6]= 'n';
	UartSendBlock[7]= 'd';
	UartSendBlock[8]= '=';
	UartSendBlock[9]= CID;
	UartSendBlock[10]= ',';
	UartSendBlock[11]= '1';
	UartSendBlock[12]= ',';
	UartSendBlock[13]= '1';
	UartSendBlock[14]= '0';
	UartSendBlock[15]= '0';
	UartSendBlock[16]= ',';
	UartSendBlock[17]= '/';
	UartSendBlock[18]= 'a';
	UartSendBlock[19]= 'c';
	UartSendBlock[20]= 'c';
	UartSendBlock[21]= 'o';
	UartSendBlock[22]= 'u';
	UartSendBlock[23]= 'n';
	UartSendBlock[24]= 't';
	UartSendBlock[25]= '/';
	UartSendBlock[26]= 'a';
	UartSendBlock[27]= 'p';
	UartSendBlock[28]= 'i';
	UartSendBlock[29]= '/';
	UartSendBlock[30]= 's';
	UartSendBlock[31]= 'e';
	UartSendBlock[32]= 't';
	UartSendBlock[33]= 'd';
	UartSendBlock[34]= 'e';
	UartSendBlock[35]= 'v';
	UartSendBlock[36]= 'i';
	UartSendBlock[37]= 'c';
	UartSendBlock[38]= 'e';
	UartSendBlock[39]= 'l';
	UartSendBlock[40]= 'o';
	UartSendBlock[41]= 'g';
	UartSendBlock[42]= '/';
	UartSendCount = 43;
	UartGotCR = 0;
	UartNoAT = 0;	/*send AT+ AT START */
	UartNoLF = 1;	/*NO send CR LF at end*/
	UartRxPtr = 0;
}

/****************************************************************/
/*SEND STRING DATA PART2					*/
/*STK=1 							*/
/****************************************************************/
void SendData2(void){
	UartSendBlock[0]= 'p';	/*SER NUM*/
	UartSendBlock[1]= '1';
	UartSendBlock[2]= '/';
	UartSendBlock[3]= '0';
	UartSendBlock[4]= '0';
	UartSendBlock[5]= '0';
	UartSendBlock[6]= '0';
	UartSendBlock[7]= '0';
	UartSendBlock[8]= '0';
	UartSendBlock[9]= '1';
	UartSendBlock[10]= '2';
	UartSendBlock[11]= '3';
	UartSendBlock[12]= '4';
	UartSendBlock[13]= '5';
	UartSendBlock[14]= '1';
	UartSendBlock[15]= '2';
	UartSendBlock[16]= '3';
	UartSendBlock[17]= '9';
	UartSendBlock[18]= '2';
	UartSendBlock[19]= '/';
	
	UartSendBlock[20]= 'p';
	UartSendBlock[21]= '2';
	UartSendBlock[22]= '/';
	BIN2ASC(((VER0 * 10)) + (VER1));	/*ver*/
	UartSendBlock[23]= B2ASCBuf[6];
	UartSendBlock[24]= B2ASCBuf[7];
	UartSendBlock[25]= '.';
	BIN2ASC(((VER2 * 10)) + (VER3));
	UartSendBlock[26]= B2ASCBuf[6];
	UartSendBlock[27]= B2ASCBuf[7];
	UartSendBlock[28]= '/';
	
	UartSendBlock[29]= 'p';			/*sensitivity*/
	UartSendBlock[30]= '3';
	UartSendBlock[31]= '/';
	if (SENS_SW == 1) Temp = 'H';
	if (SENS_SW == 2) Temp = 'L';
	if (SENS_SW == 3) Temp = 'M';
	UartSendBlock[32]= Temp;
	UartSendBlock[33]= '/';
	
	UartSendBlock[34]= 'p';			/*length*/
	UartSendBlock[35]= '4';
	UartSendBlock[36]= '/';
	if (LEN_SW == 1) Temp = 'L';
	if (LEN_SW == 2) Temp = 'S';
	if (LEN_SW == 3) Temp = 'M';
	UartSendBlock[37]= Temp;
	UartSendBlock[38]= '/';
	
	UartSendBlock[39]= 'p';			/*delay*/
	UartSendBlock[40]= '5';
	UartSendBlock[41]= '/';
	if (DELAY_SW == 1) Temp = 'L';
	if (DELAY_SW == 2) Temp = 'S';
	if (DELAY_SW == 3) Temp = 'M';
	UartSendBlock[42]= Temp;
	UartSendBlock[43]= '/';
	
	UartSendBlock[44]= 'p';			/*mode*/
	UartSendBlock[45]= '6';
	UartSendBlock[46]= '/';
	if (LOGIC_MODE == 0) Temp = 'D';
	if (LOGIC_MODE == 2) Temp = 'H';
	UartSendBlock[47]= Temp;
	UartSendBlock[48]= '/';
		
	UartSendCount = 49;
	UartGotCR = 0;
	UartNoAT = 1;	/*send NO AT+ AT START */
	UartNoLF = 1;	/*send NO CR LF at end*/
	UartRxPtr = 0;
}

/****************************************************************/
/*SEND STRING DATA PART3					*/
/*STK=1 							*/
/****************************************************************/
void SendData3(void){
uns24 LocalSum;
	UartSendBlock[0]= 'p';	/*TOT DISP*/
	UartSendBlock[1]= '7';
	UartSendBlock[2]= '/';
	Temp = readByteEE(EE_TSB_CNT2);	/*MSB*/
	Temp1 = readByteEE(EE_TSB_CNT1);	/*MIDB*/
	Temp2 = readByteEE(EE_TSB_CNT0);	/*LSB*/
	LocalSum = ((uns8)100 * Temp);
	LocalSum += (Temp1 * 10);
	LocalSum += Temp2;
	StoreB2AscBuf( LocalSum, 3);	/*uses up 8 bytes in send block*/
	UartSendBlock[11]= '/';
	
	UartSendBlock[12]= 'p';		/*TOTAL SINCE LAST BAT CHANGE*/
	UartSendBlock[13]= '8';
	UartSendBlock[14]= '/';
	Temp = readByteEE(EE_TLBC_CNT2);	/*MSB*/
	Temp1 = readByteEE(EE_TLBC_CNT1);	/*MIDB*/
	Temp2 = readByteEE(EE_TLBC_CNT0);	/*LSB*/
	LocalSum = (Temp * (char)100);
	LocalSum = LocalSum + (Temp1 * 10);
	LocalSum = LocalSum + Temp2;
	StoreB2AscBuf( LocalSum, 15);	/*uses up 8 bytes in send block*/
	UartSendBlock[23]= '/';
	
	UartSendBlock[24]= 'p';		/*TOTAL SINCE LAST BAT CHANGE*/
	UartSendBlock[25]= '9';
	UartSendBlock[26]= '/';
	Temp = readByteEE(EE_TCBC_CNT2);	/*MSB*/
	Temp1 = readByteEE(EE_TCBC_CNT1);	/*MIDB*/
	Temp2 = readByteEE(EE_TCBC_CNT0);	/*LSB*/
	LocalSum = (Temp * (char)100);
	LocalSum = LocalSum + (Temp1 * 10);
	LocalSum = LocalSum + Temp2;
	StoreB2AscBuf( LocalSum, 27);	/*uses up 8 bytes in send block*/
	UartSendBlock[35]= '/';
	
	UartSendBlock[36]= 'p';		/*TOTAL BAT CHANGES*/
	UartSendBlock[37]= '1';
	UartSendBlock[38]= '0';
	UartSendBlock[39]= '/';
	Temp1 = readByteEE(EE_TBSB_CNT1);	/*MSB*/
	Temp2 = readByteEE(EE_TBSB_CNT0);	/*LSB*/
	LocalSum =  (Temp1 * 10);
	LocalSum = LocalSum + Temp2;
	StoreB2AscBuf( LocalSum, 40);	/*uses up 8 bytes in send block*/
	UartSendBlock[48]= '/';
		
	UartSendCount = 49;
	UartGotCR = 0;
	UartNoAT = 1;	/*send NO AT+ AT START */
	UartNoLF = 1;	/*send NO CR LF at end*/
	UartRxPtr = 0;
}

/****************************************************************/
/*SEND STRING DATA PART3					*/
/*STK=1 							*/
/****************************************************************/
void SendData4(void){
uns24 LocalSum;
	UartSendBlock[0]= 'p';	/*V at last bat change*/
	UartSendBlock[1]= '1';
	UartSendBlock[2]= '1';
	UartSendBlock[3]= '/';
	Temp1 = readByteEE(EE_VLBC_CNT1);	/*MIDB*/
	Temp2 = readByteEE(EE_VLBC_CNT0);	/*LSB*/
	LocalSum = (Temp1 * 256);
	LocalSum += Temp2;
	StoreB2AscBuf( LocalSum, 4);	/*uses up 8 bytes in send block*/
	UartSendBlock[12]= '/';
	
	UartSendBlock[13]= 'p';		/*CURR BAT VOLTS*/
	UartSendBlock[14]= '1';
	UartSendBlock[15]= '2';
	UartSendBlock[16]= '/';
	StoreB2AscBuf( BatValNow, 17);	/*uses up 8 bytes in send block*/
	UartSendBlock[25]= '/';
	
	UartSendBlock[26]= 'p';		/*low paper*/
	UartSendBlock[27]= '1';
	UartSendBlock[28]= '3';
	UartSendBlock[29]= '/';
	if(LOW_PAPER) {
		UartSendBlock[30]= '1';
	}
	else{
		UartSendBlock[30]= '0';
	}
	UartSendBlock[31]= '/';
	
	UartSendBlock[32]= 'p';		/*LOW BAT*/
	UartSendBlock[33]= '1';
	UartSendBlock[34]= '4';
	UartSendBlock[35]= '/';
	if(BATLO_STATE) {
		UartSendBlock[36]= '1';
	}
	else{
		UartSendBlock[36]= '0';
	}
	UartSendBlock[37]= '/';
	
	
	UartSendBlock[38]= 'p';		/*JAM*/
	UartSendBlock[39]= '1';
	UartSendBlock[40]= '5';
	UartSendBlock[41]= '/';
	if(JAM_FLAG) {
		UartSendBlock[42]= '1';
	}
	else{
		UartSendBlock[42]= '0';
	}
		
	UartSendCount = 43;
	UartGotCR = 0;
	UartNoAT = 1;	/*send NO AT+ AT START */
	UartNoLF = 0;	/*send CR LF at end*/
	UartRxPtr = 0;
}

/****************************************************************/
/*SEND DATA FOR SNAP AT STARTUP 				*/
/*STK=1 							*/
/****************************************************************/
void UartStartupData(void){
	if(UartStartupFlag == 0){
		switch (UartStartupState){
			case 0:{
				SendStringDNS();
				UartStartupState = 1;
				UartRxDLY = UartRxDlyVal;
				break;
			}
			case 1:{
				if(UartSendCount == 0){	/*wait for full transmit*/
					if(UartRxDLY){
						if(UartGotCR){
							UartStartupState = 2;	/*check for OK*/
						}
					}
					else{		/*NO SNAP avail*/
						UartStartupFlag = 1;
					}
				}
				break;
			}
			case 2:{		/*check for OK*/
				SendStringCNF();
				UartStartupState = 3;
				UartRxDLY = UartRxDlyVal;
				break;
			}
			case 3:{
				if(UartSendCount == 0){	/*wait for full transmit*/
					if(UartRxDLY){
						if(UartGotCR){
							UartStartupState = 4;	/*check for OK*/
						}
					}
					else{		/*NO SNAP avail*/
						UartStartupFlag = 1;
					}
				}
				break;
			}
			case 4:{		/*check for OK*/
				SendStringKA();
				UartRxDLY = UartRxDlyVal;
				UartStartupState = 5;
				break;
			}
			case 5:{
				if(UartSendCount == 0){	/*wait for full transmit*/
					if(UartRxDLY){
						if(UartGotCR){
							UartStartupFlag = 1;
							UartStartupState = 0;
						}
					}
					else{		/*NO SNAP avail*/
						UartStartupState = 0;
					}
				}
			}
			break;
		}
	}
}

/****************************************************************/
/*SEND DATA FOR SNAP DATA TRANSFER 				*/
/* TO ACTIVATE TRANSFER SET UartTransferFlag =1 when done =0	*/
/*STK=1 							*/
/****************************************************************/
void UartTransferData(void){
	if((UartTransferFlag == 1) && (UartStartupFlag)){
		switch (UartTransferState){
			case 0:{
				SendStringOPEN();
				UartTransferState = 1;
				UartRxDLY = UartRxDlyVal;
				break;
			}
			case 1:{
				if(UartSendCount == 0){	/*wait for full transmit*/
					if(UartRxDLY){
						if(UartGotCR){	/*check for OK*/
							UartTransferState = 2;
						}
					}
					else{		/*NO SNAP avail*/
						UartTransferState = 7;
					}
				}
				break;
			}
			case 2:{		/*NEXT*/
				CID = UartRxBlock[UartRxPtr - 3];	/*this is the CID for later transmission*/
				SendData1();			/*send the 1st block of data*/
				UartTransferState = 3;
				break;
			}
			case 3:{
				if(UartSendCount == 0){	/*wait for full transmit*/
					SendData2();			/*send data block 2*/
					UartTransferState = 4;
				}
				break;
			}
			case 4:{
				if(UartSendCount == 0){	/*wait for full transmit*/
					SendData3();			/*send data block 2*/
					UartTransferState = 5;
				}
				break;
			}
			case 5:{
				if(UartSendCount == 0){	/*wait for full transmit*/
					SendData4();
					UartRxDLY = UartRxDlyVal;
					UartTransferState = 6;
				}
				break;
			}
			case 6:{
				if(UartRxDLY){
					if(UartGotCR){	/*check for OK*/
						UartTransferState = 7;
						}
					}
					else{		/*NO SNAP avail*/
						UartTransferState = 7;
					}
				break;
			}
			case 7:{
				UartTransferState = 0;
				UartTransferFlag = 0;
			}
		}
	}
	else{
		if((UsartSendDly == 0) && (UartStartupFlag == 1)){
			UartTransferFlag = 1;
			UsartSendDly = UsartSendDlyVal;
		}
	}
}

/*----------------------------------------------------------------------*/
/*CHECK FOR DOOR OPEN OPERATIONS FOR THE LED TRANSFER DATA		*/
/*STK =1								*/
/*----------------------------------------------------------------------*/
void ChkDoorOpenOps(void){
	switch(DoorOpenSeq){
		case 00:{
			PCON |= 1;	/*SET BOR CHECK WHEN DOOR IS CLOSED TO SEE IF BATT CHANGED*/
			Stop_AD_Reads = 1;	/*I am using the AD now*/
			PORTB.1 = 1;			/*TURN ON BATT DIVIDER*/
			CHS1=0;
			CHS0=0;				/*SETUP CHANNEL (CHAN 0 = VLB)*/
			ADON=1;				/*TURN ON AD*/
			delayms(DLY_1_MS);		/*LET INPUT SETTLE*/
			GO=1;				/*START CONVERSION*/
			DoorOpenSeq = 1;
			break;
		}
		case 01:{
			while (GO);{
			}				/*WAIT FOR AD TO FINISH*/
			analogval = ADRESH * 256;
			analogval = analogval + ADRESL;	/*GET DATA LOCAL*/
			ADON=0;				/*AD OFF*/
			PORTB.1 = 0;			/*TURN OFF BATT DIVIDER*/
			GIE = 0;
			x = analogval;		/*make copy */
			GIE = 1;
			y = analogval.low8;
			writeByteEE(EEAnValSav0, y);	/*SAVE THE BATT VOLTAGE FOR REFERENCE USE WHEN DOOR IS CLOSED*/
			y = analogval.high8;
			writeByteEE(EEAnValSav1, y);
			DoorOpenSeq =  2;				/*I AM DONE NOW*/
			DoorCloseSeq = 0;
			Stop_AD_Reads = 0;	/*I am not using the AD now*/
			break;
		}
		default:{
		}
	}
}

/*----------------------------------------------------------------------*/
/*CHECK FOR DOOR OPEN OPERATIONS FOR THE LED TRANSFER DATA		*/
/*STK =1								*/
/*----------------------------------------------------------------------*/
void ChkDoorCloseOps(void){
	switch(DoorCloseSeq){
		case 00:{		/*first pass since door opened*/
			if((PCON & 0x01) == 1){
				BAT_CHANGED = 0;
			}
			else{
				BAT_CHANGED = 1;	/*IF ANY BOR THEN BAT HAS BEEN CHANGED*/
			}
			DoorCloseSeq = 1;
			break;
		}
		
		case 01:{
			AnChkVal.low8 = readByteEE(EEAnValSav0);
			AnChkVal.high8 = readByteEE(EEAnValSav1);
			if (BAT_CHANGED){	/*this ia a battery change*/
				ADV_TBSB_SHT_CNT();	/*count the battery change*/
				writeByteEE(EE_VLBC_CNT0, AnChkVal.low8);	/*SAVE THE BATT VOLTAGE AT LAST BATTERY CHANGE*/
				writeByteEE(EE_VLBC_CNT1, AnChkVal.high8);
				Tmp1 = readByteEE(EE_TCBC_CNT2);	/*move the current count to last battery change count*/
				writeByteEE(EE_TLBC_CNT2, Tmp1);
				Tmp1 = readByteEE(EE_TCBC_CNT1);
				writeByteEE(EE_TLBC_CNT1, Tmp1);
				Tmp1 = readByteEE(EE_TCBC_CNT0);
				writeByteEE(EE_TLBC_CNT0, Tmp1);
				writeByteEE(EE_TCBC_CNT0, 0);	/*clear the current count since battey change*/
				writeByteEE(EE_TCBC_CNT1, 0);
				writeByteEE(EE_TCBC_CNT2, 0);
			}
			DoorCloseSeq = 2;
			break;
		}
		case 02:{
			DoorCloseTmr = DLY_5_SEC;
			DoorOpenSeq = 0;	/*reset for next door open*/
			DoorCloseSeq = 3;
		}
		
		default:{
		}
	}
}

/*----------------------------------------------------------------------*/
/*up to 3 bytes binary to ascii output					*/
/*leading zero not supressed. to buffer B2ASCBuf[8] returns number count*/
/*STK =1								*/
/*----------------------------------------------------------------------*/
void BIN2ASC(uns24 InNum){
char Bufptr;
char Res;
	Bufptr = 0;
	Res = InNum / (int24)10000000;
	B2ASCBuf[Bufptr] = Res | 0x30;
	InNum = InNum - (Res * (int24)10000000);
	Bufptr++;

	Res = InNum / (int24)1000000;
	B2ASCBuf[Bufptr] = Res | 0x30;
	InNum = InNum - (Res * (int24)1000000);
	Bufptr++;

	Res = InNum / (int24)100000;
	B2ASCBuf[Bufptr] = Res | 0x30;
	InNum = InNum - (Res * (int24)100000);
	Bufptr++;

	Res = InNum / (int24)10000;
	B2ASCBuf[Bufptr] = Res | 0x30;
	InNum = InNum - (Res * (int24)10000);
	Bufptr++;

	Res = InNum / (int24)1000;
	B2ASCBuf[Bufptr] = Res | 0x30;
	InNum = InNum - (Res * (int24)1000);
	Bufptr++;

	Res = InNum / (int24)100;
	B2ASCBuf[Bufptr] = Res | 0x30;
	InNum = InNum - (Res * (int24)100);
	Bufptr++;

	Res = InNum / (int24)10;
	B2ASCBuf[Bufptr] = Res | 0x30;
	InNum = InNum - (Res * (int24)10);
	Bufptr++;

	B2ASCBuf[Bufptr] = InNum | 0x30;
}

#pragma codepage 0
/***************************************************************/
/******	INITIALIZE *********************************************/
/***************************************************************/

void init(void){
	OSCCON=0b0110.0001;	/*INTERNAL OSC AT 4 MHZ*/
	clearRAM();
	CMCON0	= 0b0000.0111;	/*COMPARITORS OFF*/
	PCON |= 0x10;		/*TURN ON BOR*/
	IRSEN_STATE = 1;	/*START IN ACTIVE MODE*/
	LCDCON = 0;		/*TURN OFF LCD MODES*/
	LCDSE1 = 0;
	ANSEL = 0x03;		/*RA0 TO 1 ARE ANALOG INPUTS*/
	ADCON0	= 0b1000.0000;	/*SEL CHAN #0*/
	ADCON1	= 0b0011.0000;	/*INTERNAL 500KHZ */
/*--------- ports---------------------------------------------------------------*/
/*----------PORTA:--------------------------------------------------------------*/
	TRISA	= 0b1010.1011;	/*RA0 AND RA1 ARE ANALOG INPUTS*/
	PORTA	= 0b1011.1011;	
/*----------PORTB:--------------------------------------------------------------*/
	TRISB	= 0b0010.0101; 	/*SETUP PORT B */
	PORTB	= 0b1100.1101;	
/*----------PORTC:--------------------------------------------------------------*/
	TRISC	= 0b1100.0000;	/*SETUP PORT C */
	PORTC	= 0b1101.0000;	
/*----------PORTD:--------------------------------------------------------------*/
	TRISD	= 0b1111.1111; 	/*SETUP PORT D */
	PORTD	= 0b1111.1111;	
/*----------PORTE:--------------------------------------------------------------*/
	TRISE	= 0b0000.1000;	/*SETUP PORT E*/
	PORTE	= 0b0000.1111;	

/* TIMER 0 USED FOR RT INT.							*/
/*	    ALSO USED FOR DETERMINING IF HANDS ARE CLEAR OF THE DISPENSER	*/
/* TIMER 1 NOT USED								*/
/* TIMER 2 USED TO GENERATE 56KHZ WITH THE PWM FEATURE				*/
	OPTION	= 0b1000.0000;	/*PRESCALER = DIV BY 2*/
	TMR0	= 90;		/* DIV BY 179(76) SET FOR INTERRUPT OF 2.7933KHZ*/
	/*CORRECTED IMPERICALLY(90) FOR OVERHEAD*/
	PR2	= 17;		/*SET PWM PERIOD TO 18usec. 55.55KHZ*/
	CCPR1L	= 0b0000.0001;	/*SETS PWM MODE AND 2 LSBS OF DUTY CYCLE 10%*/
	CCP1CON	= 0b0011.1100;	/*SETS MSB OF DUY CYCLE COUNT*/
	T2CON	= 0b0000.0000;	/*SETS PRESCALE TO 1:1 */
	OSCTUNE = 0x00;
	TB_LEDDSP_CTR = TB_LEDDSP_CNT;	/*INITIALIZE THE TB LED DISPLAY*/
	WDTCON = 0b0000.1110;	/*DIV 31KHZ BY 4096 to get 0.132 SEC*/
	SleepExtendCtr = SleepExtendVal;	/*reset sleep extend counter*/
/*USART*/
	SPBRG = 25;		/* FOR BR = 9600*/
	TXSTA = 0b0010.0100;
	RCSTA = 0b1001.0000;
	RCIE = 1;		/* EN INTERRUPTS*/
	INTDummy = RCREG;
	IRSENCNT_TOTRIG = 2;
	DoorOpenSeq = 2;
	StartupDelay = DLY_1_SEC;
}

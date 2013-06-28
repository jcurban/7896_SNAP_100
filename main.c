/*-------------extern -----------------------------------------------------------------------*/
/* SNAP JOB#7896 USES STM8007C8T6 PROCESSOR                                           */
/* - 32K FLASH 2KRAM  128 EEPROM  ver 1.00                                            */
/*------------------------------------------------------------------------------------*/
/* Includes ------------------------------------------------------------------*/
#include "main.h"
extern char CID_Value;
extern char SNAP_State;
extern void Send_Powered(void);
extern void Wait_For_Update(void);
extern void Process_Received_Update(void);
extern void GetNetworkStatusFromGS1011(void);
extern void Reset_Network_Access(void);
extern void Send_Website_Update(void);

extern u16 EEWRTimout;
extern u8 Device_Rcvr_Complete_flag;
extern int GS1011_Rvcr_Count;
extern char GS1011_Xmit_Buffer;
extern char *GS1011_Rcvr_Pointer;
extern char GS1011_Received_Response_Flag;
void SendYouThereMessageToGS1011(void);
void InitializeGS1011Buffer(void);
void InitializeDeviceBuffer (void);


/* Local Var------------------------------------------------------------------*/
u8  TEST;
u8  BLCNT_INDX;
u8  BLCNT_STATE;
u8  BLCNT_VAL;         /*VERSION BLINK VALUE*/
u8  SysError;          /* bit 0 = 1 if Temp reading ou   t of limits
                          bit 1 = 1 if command start and no response*/
/*--------------VERSION NUMBER OF THIS FIRMWARE-------------------------------*/
u8 VER[4] = {VER0,VER1,VER2,VER3};
/*----------------------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
void RCC_Configuration(void);
void GPIO_Configuration(void);
void TIMER1_Configuration (void);
void InitEEData(void);
void InitDeviceUART(void);
void InitGS1011UART(void);
/*external Functions ---------------------------------------------------------*/
/*void Make_Send_SNAP_Ready_Message(void);*/
void Send_powered_Message(void);
void Save_PValues(void);
/* Main powerup/main loop -----------------------------------------------*/
extern char * GS1011_Xmit_Pointer;
int main( void ){
  /* System Clocks Configuration */
  RCC_Configuration();
  /* Configure the GPIO ports */
  GPIO_Configuration();
  /*CONFIGURE TIMER1*/
  TIMER1_Configuration();
  /*Configure device UART*/
  InitDeviceUART();
  /*Configure GS101 UART*/
  InitGS1011UART();
  InitializeGS1011Buffer();
  InitializeDeviceBuffer();
  enableInterrupts();
  SNAP_State = 0;
  CID_Value = 0x00;
  GS1011_Received_Response_Flag = 0x00;
  SendYouThereMessageToGS1011();                /* sacrificial message*/

  while (1)
  {
    switch (SNAP_State){
    case 0:
      Send_Powered();
      break;
    case 1:
      Wait_For_Update();
      break;
    case 2:
      Process_Received_Update();
      break;
    case 3:
      GetNetworkStatusFromGS1011();
      break;
    case 4:
      Reset_Network_Access();
      break;
    case 5:
      Send_Website_Update();
      break;
    }
  }
 /*infinite loop*/
}
/* Get the EEData on startup-------------------------------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
void InitEEData(void){
}
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}
/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
*                 16MHZ INTERNAL OSCILLATOR HSI ENABLED BY DEFAULT
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Configuration(void){
  CLK->SWCR = 0x08;     /*SWIF SET TO AUTO*/
  CLK->CKDIVR = 0;      /*SET TO FULL SPEED 16MHZ*/
  CLK->PCKENR1 = 0x8D;  /*TIM1, I2C CLOCK ENABLED, UART1,3*/
  CLK->PCKENR2 = 0x08;  /*ADC CLOCK ENABLED*/
  CLK->CSSR = 0x00;     /*security off*/
}
/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration(void){
  GPIOA->DDR = 0xDF;  /*all outputs*/
  GPIOA->CR1 = 0x70; /*default inputs floating outputs OD*/
  GPIOA->CR2 = 0; /*default values no ints, 2mhz speed*/
  GPIOB->DDR = 0xF0;  /*4-7 outputs, 0-3 inputs*/
  GPIOB->CR1 = 0; /*default inputs floating outputs OD*/
  GPIOB->CR2 = 0; /*default values no ints, 2mhz speed*/
  GPIOC->DDR = 0xFF;  /*all outputs*/
  GPIOC->CR1 = 0xFF; /*outputs push pull*/
  GPIOC->CR2 = 0; /*default values no ints, 2mhz speed*/
  GPIOD->DDR = 0xBD;  /*all outputs except D1,D6 */
  GPIOD->CR1 = 0xB1; /*outputs push pull except 2,3,46 OD*/
  GPIOD->CR2 = 0; /*default values no ints, 2mhz speed*/
  GPIOE->DDR = 0xFF;  /*all outputs */
  GPIOE->CR1 = 0x09; /*default inputs floating outputs OD*/
  GPIOE->CR2 = 0; /*default values no ints, 2mhz speed*/
  GPIOG->DDR = 0xFE;  /*all outputs except PG0 */
  GPIOG->CR1 = 0xFE; /*outputs push pull input is floating */
  GPIOG->CR2 = 0; /*default values no ints, 2mhz speed*/
}

/* TIMER1 configuration -----------------------------------------------------*/
/*USED FOR GP TIMING RUN AT 200HZ                                            */
/*---------------------------------------------------------------------------*/
void TIMER1_Configuration (void){
  TIM1->PSCRH = 0x00; /*prescale div by 4  (16m/4 = 4mhz)*/
  TIM1->PSCRL = 0x03; /*prescale div by 4  (16m/4 = 4mhz)*/
  TIM1->ARRH = 0x4E; /*auto reload = 20000 (4m/20k = 200)*/
  TIM1->ARRL = 0x20;
  TIM1->CR1 = 0x91; /*enable counter with interrupts on overflow*/
  TIM1->IER = 0x01; /*enable interrupts*/
}


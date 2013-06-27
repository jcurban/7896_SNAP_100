/**
  ******************************************************************************
 ********************************************************************************
  * @file    Parameter Block Handlers.c
  * @author  JUrban
  * @version V1.0
  * @date    12-June-2013
  * @brief   This file contains all the functions for handling 
                the parameter blocks.
  ******************************************************************************
  ******************************************************************************
  */
/* local Routine prototypes */
void Save_PValues(void);
void get_Serial_Number(void);
void get_Device_type(void);
void SaveParameterInBlock(char PNumb, char *PBfr);
/* external routine prototypes */
extern void CopyBufferCounted (char *dest, char *srce, int cntr);
void CopyBufferCounted (char *dest, char *srce, int cntr);
extern void FillBuffer (char *bufr,char filchr, int cntr);
/* external data */
extern int PPointer;
extern char PNumber;
extern int PCount;
extern char PBuffer;
extern char Device_Serial_number;
extern char Device_Type_Number;
extern char Packet_Data_Buffer;
extern char Device_Receiver_Buffer;
extern int Device_Rcvr_Char_Count;
/*
******************************************************************************
****
****    Save_PValues
****
**** finds the serial number, uses it to determine class, save the parameters
****            into the proper block, in the correct format.
****
******************************************************************************
*/
void Save_PValues(void){
char *PSrceIndx;
char *PDestIndx;
int PCount;
    CopyBufferCounted (&Packet_Data_Buffer, &Device_Receiver_Buffer + 4, Device_Rcvr_Char_Count - 6);
    PNumber = 0;
    PSrceIndx = 0;
    PCount = Device_Rcvr_Char_Count - 6;
    get_Serial_Number();
    get_Device_type();
    while ((int)PSrceIndx <= PCount){  /* do this till pointer at end of buf*/
      FillBuffer (&PBuffer, 0x00, 16);
      PDestIndx = 0;
      while (Packet_Data_Buffer[PSrceIndx] != ',') { /* copy till ','*/
        PBuffer[PDestIndx++] = Packet_Data_Buffer[PSrceIndx++];
      }
      SaveParameterInBlock(PNumber, &PBuffer);
    }
}

/*
******************************************************************************
****
****    get_Serial_Number
****
**** finds the serial number, uses it to determine class, save the parameters
****            into the proper block, in the correct format.
****
******************************************************************************\
*/
void get_Serial_Number(){
  CopyBufferCounted (&Device_Serial_number, &Packet_Data_Buffer, 16);
}
/*
******************************************************************************
****
****    get_Device_Class_Number
****
**** finds the serial number, uses it to determine class, save the parameters
****            into the proper block, in the correct format.
****
******************************************************************************\
*/
void get_Device_type(){
  CopyBufferCounted (&Device_Type_Number, &Device_Serial_number, 6);
}
/******************************************************************************
****
****    Save_PValues
****
**** finds the serial number, uses it to determine class, save the parameters
****            into the proper block, in the correct format.
****
******************************************************************************
*/
void SaveParameterInBlock(char PNumb, char *PBfr){
}
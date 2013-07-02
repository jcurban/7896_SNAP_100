#include "stm8s.h"
#include "SNAP_Defines.h"

extern int Found_String_At_Byte;
extern u8 GS1011_String_Found;
extern char Device_Xmit_Pointer;
extern u8 Device_Xmit_Char_Count;
extern u8 GS1011_Xmit_Char_Count;
extern int GS1011_Xmit_Pointer;
extern char GS1011_Xmit_Setup_Char_Pointer;
extern char Device_Xmit_Setup_Char_Pointer;
extern int GS1011_Rvcr_Count;
extern char GS1011_Xmit_Buffer[];
extern char Device_Xmit_Buffer[];
extern char GS1011_Receiver_Buffer[];
extern char Device_Receiver_Buffer[];
extern u16 GS1011_Rcvr_InPtr;
extern u16 GS1011_Rcvr_OutPtr;
extern u8 Device_RX_InPtr;
extern u8 Device_RX_OutPtr;
extern char Device_Serial_number[];
extern char Device_Processing_Buffer[];

/*****************************************************************************/
/*****            STRING UTILITY ROUTINES                                 ****/
/*****************************************************************************/
int CountChars(char s[]);
void CopyBufferGS1011(char srce[]);
char CountGS1011Chars(void);
char CopyBuffer (char dest[], char srce[]);
void CopyBufferDevice( char srce[]);
void CopyBufferGS1011 (char srce[]);
void CopyBufferCounted (char dest[], char srce[], char cntr);
void FillBuffer (char bufr[],char filchr, char cntr);

void InitializeDeviceBuffer (void);
void InitializeGS1011Buffer (void);
void Add_Char_to_Buffer (char bufr[], int ptr,char chr);
void Add_Char_to_GS1011_Buffer (char chr);
void Add_Integer_to_Buffer (char bufr[],int ptr, int vint);
int Add_String_to_Buffer (char bufr[],int ptr, char srce[]);
void Add_String_to_GS1011_Buffer ( char srce[]);
void Add_String_to_GS1011_BufferCounted ( char srce[], char cnt);
void CopySerialNumber(void);
void FindGS1011Chars(char chrstrng[]);
void copy_buffer_from_offset_to_terminator(char srcebufr[],char destbufr[], int ofst, char trm);
/*****************************************************************************/
/*****              GENERAL BUFFER HANDLING ROUTINES                      ****/
/*****************************************************************************/

/*****************************************************************************/
/***** InitializeDeviceBuffer (buffer pointer)                            ****/
/*****                  source buffer must terminate with a 0x00.         ****/
/*****          Returns an integer for number of characters in the buffer ****/
/*****************************************************************************/
void InitializeDeviceBuffer (void){
int ptr;
  for (ptr = 0; ptr < 255; ptr++) 
   Device_Receiver_Buffer[ptr] = 0x00;
Device_RX_InPtr = 0;
Device_RX_OutPtr = 0;
}
/*****************************************************************************/
/***** InitializeDeviceBuffer (buffer pointer)                            ****/
/*****                  source buffer must terminate with a 0x00.         ****/
/*****          Returns an integer for number of characters in the buffer ****/
/*****************************************************************************/
void InitializeGS1011Buffer (void){
int ptr;
for (ptr = 0; ptr < BFRSIZEX2; ptr++) 
   GS1011_Receiver_Buffer[ptr] = 0x00;
for (ptr = 0; ptr < BFRSIZE; ptr++)
    GS1011_Xmit_Buffer[ptr] = 0x00;

GS1011_Rcvr_InPtr = 0;
GS1011_Rcvr_OutPtr = 0;
GS1011_Xmit_Char_Count=0;
GS1011_Xmit_Setup_Char_Pointer=0;
}
/******************************************************************************/
/***** Add_Char_to_Buffer (char s)                                         ****/
/*****                 Buffer pointer always contain the next space        ****/
/*****                    available for data storage                       ****/
/*****     CALLING ROUTINE IS RESPONSIBLE TO BE SURE THERE'S ROOM AVAILABLE****/
/*****************************************************************************/
void Add_Char_to_Buffer (char bufr[], int ptr, char chr){
  bufr[ptr] = chr;
  ptr++;
}

/******************************************************************************/
/***** Add_Char_to_GS1011_Buffer (char s)                                  ****/
/*****                 Buffer pointer always contain the next space        ****/
/*****                    available for data storage                       ****/
/*****     CALLING ROUTINE IS RESPONSIBLE TO BE SURE THERE'S ROOM AVAILABLE****/
/*****************************************************************************/
void Add_Char_to_GS1011_Buffer (char chr){
  GS1011_Xmit_Buffer[GS1011_Xmit_Setup_Char_Pointer] = chr;
  GS1011_Xmit_Setup_Char_Pointer++;
}

/*****************************************************************************/
/***** Add_Integer_to_Buffer (int s)                               ****/
/*****                 Device buffers always contain the next space       ****/
/*****                    available for data storage                      ****/
/*****                 as an int in first 2 bytes                         ****/
/*****************************************************************************/
void Add_Integer_to_Buffer (char bufr[], int ptr, int vint){
  int high, low;
  high = vint>>8;            /* calculate high byte and save */
  low = vint & 0Xff;             /* calculate low byte and save */
  bufr[ptr] = low;
  bufr[ptr+1] = high;
  ptr +=2;
}

/*****************************************************************************/
/***** Add_String_to_Buffer (buffer pointer, source pointer)       ****/
/*****                 buffer & source buffer must terminate with a 0x00. ****/
/*****    overwrites the receiving buffers terminating 0x00               ****/
/*****                  copies source terminating 0x00.                   ****/
/*****************************************************************************/
int Add_String_to_Buffer (char bufr[],int ptr, char srce[]){
 u16 i;
    
    for (i = 0; i<BFRSIZE; i++){
       bufr[ptr] = srce[i];      /* store string into buffer until */
       if (srce[i] == 0x00){
            break;
        }
     }
return ptr;
}

/*****************************************************************************/
/***** Add_String_to_GS1011_Buffer (buffer pointer, source pointer)       ****/
/*****                 buffer & source buffer must terminate with a 0x00. ****/
/*****    overwrites the receiving buffers terminating 0x00               ****/
/*****                  copies source terminating 0x00.                   ****/
/*****************************************************************************/
void Add_String_to_GS1011_Buffer ( char srce[]){
 u16 i;
     for (i = 0; i<BFRSIZE; i++){
     GS1011_Xmit_Buffer[GS1011_Xmit_Char_Count] = srce[i];      /* store string into buffer until */
        if (srce[i] == 0x00)break;
     GS1011_Xmit_Char_Count++;   
     }
}

/*****************************************************************************/
/***** Add_String_to_GS1011_Buffer (buffer pointer, source pointer)       ****/
/*****                 buffer & source buffer must terminate with a 0x00. ****/
/*****    overwrites the receiving buffers terminating 0x00               ****/
/*****                  copies source terminating 0x00.                   ****/
/*****************************************************************************/
void Add_String_to_GS1011_BufferCounted ( char srce[], char cnt){
 u16 i;
 if ((GS1011_Xmit_Char_Count + cnt) < BFRSIZE){
     for (i = 0; i<=cnt; i++)      
     {
              GS1011_Xmit_Buffer[GS1011_Xmit_Char_Count] = srce[i];      /* store string into buffer until */
              GS1011_Xmit_Char_Count++;                    /*   the strings terminating 0x00 */          
     }
  }
}
/*****************************************************************************/
/***** FillBuffer (buffer pointer)                                         ****/
/*****                  source buffer must terminate with a 0x00.         ****/
/*****          Returns an integer for number of characters in the buffer ****/
/*****************************************************************************/
void FillBuffer (char bufr[],char filchr, char cntr){
 u8 i;
 if (cntr < sizeof(bufr)){
 for (i = 0; i < cntr; i++) {
   bufr[i] = filchr;
 }
 }
}
/*****************************************************************************/
/***** copy buffer (pointer to destination, pointer to source buffer)     ****/
/*****                  source buffer must terminate with a 0x00.         ****/
/*****************************************************************************/
char CopyBuffer (char dest[], char srce[]){
 u8 i;
 for (i=0; i < BFRSIZE; i++){
   dest[i] = srce[i];
   if (srce[i] == NULL) break;
  }
  return i;
 }
/*****************************************************************************/
/***** copy buffer (pointer to destination, pointer to source buffer)     ****/
/*****                  source buffer must terminate with a 0x00.         ****/
/*****************************************************************************/
void CopyBufferDevice(char srce[]){
 u8 i;
 for (i=0; i < BFRSIZE; i++){
   Device_Xmit_Buffer[i] = srce[i];
   if (srce[i] == ETX){
    Device_Xmit_Setup_Char_Pointer =i;
    break;
   }
  }

 }

/*****************************************************************************/
/***** copy buffer (pointer to destination, pointer to source buffer)     ****/
/*****                  source buffer must terminate with a 0x00.         ****/
/*****************************************************************************/
void CopyBufferGS1011 (char srce[]){
 for (GS1011_Xmit_Char_Count=0; GS1011_Xmit_Char_Count < BFRSIZE; GS1011_Xmit_Char_Count++){
    GS1011_Xmit_Buffer[GS1011_Xmit_Char_Count] = srce[GS1011_Xmit_Char_Count];
    if (srce[GS1011_Xmit_Char_Count] == 0){
        break;
    }
  }
}
/*****************************************************************************/
/***** copy buffer from offset to terminator(pointer to destination, pointer to source buffer,****/
/*****                                  number of bytes to copy)          ****/
/*****************************************************************************/
void copy_buffer_from_offset_to_terminator(char srcebufr[],char destbufr[], int ofst, char trm){
int destoffset,chkchr; 
destoffset = 0;
for (destoffset =0; destoffset < 512; destoffset++){
  chkchr = srcebufr[ofst];
if (chkchr != trm){
    destbufr[destoffset] = srcebufr[ofst];
    ofst++;
  }
else
  break;
}
 }
/*****************************************************************************/
/***** copy buffer count(pointer to destination, pointer to source buffer,****/
/*****                                  number of bytes to copy)          ****/
/*****************************************************************************/
void CopyBufferCounted (char dest[], char srce[], char cntr){
u8 i;
 
for (i=0; i < cntr; i++){
    dest[i] = srce[i];
  }
 }
void CopySerialNumber(void){
u8 i;
u8 cntr=15; 
for (i=0;i<=16;i++){
  Device_Serial_number[i] = 0;
}
for (i=0; i <= cntr; i++){
    Device_Serial_number[i] = Device_Processing_Buffer[i+4];
  }
 }
/*****************************************************************************/
/***** CountChars (buffer pointer)                                         ****/
/*****                  source buffer must terminate with a 0x00.         ****/
/*****          Returns an integer for number of characters in the buffer ****/
/*****************************************************************************/
int CountChars(char s[]){
int cntr;
  for (cntr=0; cntr < BFRSIZE; cntr++){
    if  (s[cntr] == 0x00) break;
  }
  return cntr;
}
/*****************************************************************************/
/***** CountChars (buffer pointer)                                         ****/
/*****                  source buffer must terminate with a 0x00.         ****/
/*****          Returns an integer for number of characters in the buffer ****/
/*****************************************************************************/
char CountGS1011Chars(void){
char i;
  for (i=0; i < BFRSIZE; i++){
     if  (GS1011_Xmit_Buffer[i] == 0x00) break;
  } 
     return i;
}

/*****************************************************************************/
/***** Find string ( buffer pointer, string)                                         ****/
/*****                  source buffer must terminate with a 0x00.         ****/
/*****          Returns an integer for number of characters in the buffer ****/
/*****************************************************************************/
void FindGS1011Chars(char findstrng[]){
int lookinptr, lookforptr, restartptr, lookforcnt,i;
char lookinchr,lookforchr;
    GS1011_String_Found = 0;      /* expect not to find it*/
    lookforptr=0x00;                  /*point at first byte of compare string*/
    
for (i = 0; i< BFRSIZE; i++){          /* get the size of the string to find*/
  if (findstrng[i] == 0x00){
    lookforcnt = i;
    break;}
}
for (lookinptr=0; lookinptr < GS1011_Rvcr_Count; lookinptr++){
    if (GS1011_String_Found == 1)                       /* if found exit*/
      break;
    lookinchr = GS1011_Receiver_Buffer[lookinptr];
    lookforchr = findstrng[lookforptr];                    /*match first byte of string? */
    if (lookinchr == lookforchr){  
      restartptr = lookinptr;                   /* save next char pointer to look in buffer */
      restartptr++;                             /* in case of double character */
 /***                                            loop while bytes match */
     for (i=1; i <= lookforcnt-1; i++){
        lookinptr++;
        lookforptr++;
        lookinchr = GS1011_Receiver_Buffer[lookinptr];    /*keep checking till end*/
        lookforchr = findstrng[lookforptr];    
        if (lookinchr != lookforchr){
          lookforptr=0x00;                         /*no reset the string pointer*/
          lookinptr = restartptr;                  /* and the lookin pointer */
          break;
         }
        else 
          if (i == lookforcnt-1){
          Found_String_At_Byte = (lookinptr - (lookforcnt-1));  
          GS1011_String_Found = 1;           /* save start of string in bufr */
          break;
          }
      }
    }
  /***                                            loop while bytes match */
   
   }
}

/*****************************************************************************/
/***** CountChars (buffer pointer)                                         ****/
/*****                  source buffer must terminate with a 0x00.         ****/
/*****          Returns an integer for number of characters in the buffer ****/
/*****************************************************************************/
/*void CountDeviceChars(void){
char chr;
  for (Device_Xmit_Char_Count=0; Device_Xmit_Char_Count < BFRSIZE; Device_Xmit_Char_Count++){
    chr = Device_Xmit_Pointer;
    if  (chr == 0x03){
      Device_Xmit_Char_Count++;
      break;
      }
      Device_Xmit_Pointer++;
  }
}*/
/*****************************************************************************/
/*****              DEVICE BUFFER  HANDLING ROUTINES                      ****/
/*****  While the GS1011 buffers are purely ASCII, the Device_Buffer can  ****/
/*****    ANY type of data, not only in the packet data, but as counts,   ****/
/*****    etc. in the header of the message.                              ****/
/*****                                                                    ****/
/*****    where bufr is buffer containing parameters                      ****/
/*****          ptr is pointer at begining of parameter                   ****/
/*****          cntr is the parameter counter                             ****/
/*****************************************************************************/



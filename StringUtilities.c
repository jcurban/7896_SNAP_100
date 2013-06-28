#include "stm8s.h"
extern const int BFRSIZE;
extern int Found_String_At_Byte;
extern u8 GS1011_String_Found;
extern char Device_Xmit_Pointer;
extern u8 Device_Xmit_Char_Count;
extern u8 GS1011_Xmit_Char_Count;
extern int GS1011_Xmit_Pointer;
extern int GS1011_Rvcr_Count;
extern char GS1011_Xmit_Buffer[];
extern char Device_Xmit_Buffer[];
extern char GS1011_Receiver_Buffer[];
extern char Device_Receiver_Buffer[];
extern u16 GS1011_Rcvr_InPtr;
extern u16 GS1011_Rcvr_OutPtr;
extern u8 Device_RX_InPtr;
extern u8 Device_RX_OutPtr;

/*****************************************************************************/
/*****            STRING UTILITY ROUTINES                                 ****/
/*****************************************************************************/
int CountChars(char s[]);

void CountGS1011Chars(void);
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
int Add_String_to_Buffer (char bufr[],int ptr, char *srce);
void Add_String_to_GS1011_Buffer ( char *srce);

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
  for (ptr = 0; ptr < 512; ptr++) 
   GS1011_Receiver_Buffer[ptr] = 0x00;
GS1011_Rcvr_InPtr = 0;
GS1011_Rcvr_OutPtr = 0;
}
/*****************************************************************************/
/***** Add_Char_to_Buffer (char s)                                 ****/
/*****                 Buffer pointer always contain the next space       ****/
/*****                    available for data storage                      ****/
/*****************************************************************************/
void Add_Char_to_Buffer (char *bufr, int ptr, char chr){
  bufr[ptr] = chr;
  ptr = ptr + 0x01;
}
void Add_Char_to_GS1011_Buffer (char chr){
  GS1011_Xmit_Buffer[GS1011_Xmit_Char_Count] = chr;
  GS1011_Xmit_Char_Count++;
}

/*****************************************************************************/
/***** Add_Integer_to_Buffer (int s)                               ****/
/*****                 Device buffers always contain the next space       ****/
/*****                    available for data storage                      ****/
/*****                 as an int in first 2 bytes                         ****/
/*****************************************************************************/
void Add_Integer_to_Buffer (char *bufr, int ptr, int vint){
  int high, low;
  high = vint / 256;            /* calculate high byte and save */
  low = vint % 256;             /* calculate low byte and save */
  bufr[ptr] = low;
  bufr[ptr+1] = high;
  ptr = ptr + 2;
}

/*****************************************************************************/
/***** Add_String_to_Buffer (buffer pointer, source pointer)       ****/
/*****                 buffer & source buffer must terminate with a 0x00. ****/
/*****    overwrites the receiving buffers terminating 0x00               ****/
/*****                  copies source terminating 0x00.                   ****/
/*****************************************************************************/
int Add_String_to_Buffer (char *bufr,int ptr, char *srce){
 u16 i,j;
    i = ptr;
    for (j = 0; j<BFRSIZE; j++)      
     {
        if (srce[j] == 0x00){
            ptr = i;
            break;
        }
        else {
              bufr[i] = srce[j];      /* store string into buffer until */
              i++;                    /*   the strings terminating 0x00 */          
             }
     }
return i;
}
void Add_String_to_GS1011_Buffer ( char *srce){
 u16 j;
     for (j = 0; j<BFRSIZE; j++)      
     {
        if (srce[j] == 0x00){
            break;
        }
        else {
              GS1011_Xmit_Buffer[GS1011_Xmit_Char_Count] = srce[j];      /* store string into buffer until */
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
 for (i = 0; i < cntr; i++) {
   bufr[i] = filchr;
 }
}
/*****************************************************************************/
/***** copy buffer (pointer to destination, pointer to source buffer)     ****/
/*****                  source buffer must terminate with a 0x00.         ****/
/*****************************************************************************/
char CopyBuffer (char dest[], char srce[]){
 u8 i;
 for (i=0; i < BFRSIZE; i++){
   if (srce[i] != 0)
    dest[i] = srce[i];
   else{
    dest[i] = srce[i];
    break;}
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
   if (srce[i] != 0x03)
    Device_Xmit_Buffer[i] = srce[i];
   else{
    Device_Xmit_Buffer[i] = srce[i];
    i++;
    Device_Xmit_Char_Count =i;
    break;}
  }

 }
/*****************************************************************************/
/***** copy buffer (pointer to destination, pointer to source buffer)     ****/
/*****                  source buffer must terminate with a 0x00.         ****/
/*****************************************************************************/
void CopyBufferGS1011 (char srce[]){
 u8 i;
 for (i=0; i < BFRSIZE; i++){
   if (srce[i] != 0)
    GS1011_Xmit_Buffer[i] = srce[i];
   else{
    GS1011_Xmit_Buffer[i] = srce[i];
    GS1011_Xmit_Char_Count =i;
    break;}
  }
 }
/*****************************************************************************/
/***** copy buffer from offset to terminator(pointer to destination, pointer to source buffer,****/
/*****                                  number of bytes to copy)          ****/
/*****************************************************************************/
void copy_buffer_from_offset_to_terminator(char srcebufr[],char destbufr[], int ofst, char trm){
char destoffset,chkchr; 
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
/*****************************************************************************/
/***** CountChars (buffer pointer)                                         ****/
/*****                  source buffer must terminate with a 0x00.         ****/
/*****          Returns an integer for number of characters in the buffer ****/
/*****************************************************************************/
int CountChars(char *s){
int cntr;
  for (cntr=0; cntr < BFRSIZE; cntr++){
    if  (s[cntr] == 0x00)
      break;
  }
  
  return cntr;
}
/*****************************************************************************/
/***** CountChars (buffer pointer)                                         ****/
/*****                  source buffer must terminate with a 0x00.         ****/
/*****          Returns an integer for number of characters in the buffer ****/
/*****************************************************************************/
void CountGS1011Chars(void){
char chr;
  for (GS1011_Xmit_Char_Count=0; GS1011_Xmit_Char_Count < BFRSIZE; GS1011_Xmit_Char_Count++){
    chr = GS1011_Xmit_Buffer[GS1011_Xmit_Pointer];
    if  (chr == 0x00){
      GS1011_Xmit_Char_Count++;
      break;}
    GS1011_Xmit_Pointer++;
  }
}

/*****************************************************************************/
/***** Find string ( buffer pointer, sting)                                         ****/
/*****                  source buffer must terminate with a 0x00.         ****/
/*****          Returns an integer for number of characters in the buffer ****/
/*****************************************************************************/
void FindGS1011Chars(char chrstrng[]){
int bufptr, strptr,strcnt,i;
char chr,chr1;
    GS1011_String_Found = 0;      /* expect not to find it*/
    strptr=0x00;                  /*point at first byte of compare string*/
for (i = 0; i< 6; i++){
  if (chrstrng[i] == 0x00){
    strcnt = i;
    break;}
}
for (bufptr=0; bufptr < GS1011_Rvcr_Count; bufptr++){
    if (GS1011_String_Found == 1) 
      break;
    chr = GS1011_Receiver_Buffer[bufptr];
    chr1 = chrstrng[strptr];
    if (chr == chr1){                   /*match first byte of string? */
      for (i=1; i <= strcnt-1; i++){
        bufptr++;
        strptr++;
        chr = GS1011_Receiver_Buffer[bufptr];
        chr1 = chrstrng[strptr];    /*keep checking till end*/
        if (chr != chr1){
          strptr=0x00;                 /*no reset the string pointer*/
          break;
         }
        else 
          if (i == strcnt-1){
          Found_String_At_Byte = (bufptr - (strcnt-1));  
          GS1011_String_Found = 1;

          break;
          }
      }
    }
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



/*--------------- P r o g 1 . c ---------------*/

/*
by:	David Tyler
	  Embedded Real Time Systems
	  Electrical and Computer Engineering Dept.
	  UMASS Lowell

PURPOSE
Provide the main loop and handle the processing of packets and output

CHANGES
02-05-2015 dwt - File Created
*/

/* Include Micrium and STM headers. */
#include "includes.h"

/* Include PktParser and Error module headers. */
#include "PktParser.h"
#include "Error.h"

#pragma pack(1) // Don’t align on word boundaries

/*----- c o n s t a n t    d e f i n i t i o n s -----*/
//More in PktParser.h

#define BaudRate 9600
#define DEST_ADDR 1

#define SpeedSize 2
#define DepthSize 2
#define IdSize 10

// Wind Speed Packet
#define SpeedDecimalMask 0x0F

// Date/time Packet
#define DateSize 32

#define MiddleBytePosition 8
#define EndBytePosition 24
      
#define MonthLength 4
#define DayLength 5
#define YearLength 12
#define HourLength 5
#define MinuteLength 6
        
#define MonthPosition 5
#define DayPosition 0
#define YearPosition 9
#define HourPosition 27
#define MinutePosition 21

/*----- P a c k e t   S t r u c t u r e-----*/
typedef struct
{
  CPU_INT08U payloadLen;
  CPU_INT08U dstAddr;
  CPU_INT08U srcAddr;
  CPU_INT08U msgType;
  union
  {
    CPU_INT08S temp;
    CPU_INT16U pres;
  struct
  {
    CPU_INT08S dewPt;
    CPU_INT08U hum;
  } hum;
  struct
  {
    CPU_INT08U speed[SpeedSize];
    CPU_INT16U dir;
  } wind;
  CPU_INT16U rad;
  CPU_INT32U dateTime;
  CPU_INT08U depth[DepthSize];
  CPU_INT08U id[IdSize];
  } dataPart;
} Payload;

/*----- f u n c t i o n    p r o t o t y p e s -----*/

CPU_INT32S HandlePayload();
void DisplayDate(CPU_INT32U date);
void DisplayWind(CPU_INT08U *speed,CPU_INT16U dir);
void DisplayPrecip(CPU_INT08U *depth);

/*--------------- m a i n ( ) -----------------*/

CPU_INT32S main()
{
    CPU_INT32S	exitCode;       // Return this code on exit.
	
//  Initialize the STM32F107 eval. board.
    BSP_IntDisAll();            /* Disable all interrupts. */
    BSP_Init();                 /* Initialize BSP functions */
    BSP_Ser_Init(BaudRate);     /* Initialize the RS232 interface. */

//  Run the application.    
    exitCode = HandlePayload();
    
    return exitCode;
}

/*----- H a n d l e P a y l o a d ( ) -----*/

CPU_INT32S HandlePayload()
{
  CPU_CHAR id[IdSize+1];
  Payload data;

  while(1)
  {
    //Read in serial data and packetize it
    ParsePkt(&data);
    
    //Check that packet is addressed to us otherwise throw an error
    if(data.dstAddr != DEST_ADDR)
      AddressErr();
    else
    {
      switch(data.msgType)
      {
      case 0x01: //Temperature msg
        BSP_Ser_Printf("\n SOURCE NODE %d: TEMPERATURE MESSAGE\n",data.srcAddr);
        BSP_Ser_Printf("   Temperature = %d\n",data.dataPart.temp);
        break;
      case 0x02: //Barometric Pressure msg
        //swap bytes to fix endianness
        data.dataPart.pres = (data.dataPart.pres<<ByteSize) | 
                             (data.dataPart.pres>>ByteSize);
        
        BSP_Ser_Printf("\n SOURCE NODE %d: BAROMETRIC PRESSURE MESSAGE\n",data.srcAddr);
        BSP_Ser_Printf("   Pressure = %u\n",data.dataPart.pres);
        break;
      case 0x03: //Humidity msg
        BSP_Ser_Printf("\n SOURCE NODE %d: HUMIDITY MESSAGE\n",data.srcAddr);
        BSP_Ser_Printf("   Dew Point = %d Humidity = %d\n",
                       data.dataPart.hum.dewPt,data.dataPart.hum.hum);
        break;
      case 0x04: //Wind msg
        BSP_Ser_Printf("\n SOURCE NODE %d: WIND MESSAGE\n",data.srcAddr);
        DisplayWind(data.dataPart.wind.speed,data.dataPart.wind.dir);
        break;
      case 0x05: //Radiation msg
        //swap bytes to fix endianness
        data.dataPart.rad = (data.dataPart.rad<<ByteSize) | 
                            (data.dataPart.rad>>ByteSize);
        
        BSP_Ser_Printf("\n SOURCE NODE %d: SOLAR RADIATION MESSAGE\n",data.srcAddr);
        BSP_Ser_Printf("   Solar Radiation Intensity = %u\n",data.dataPart.rad);
        break;
      case 0x06: //Date/Time msg 
        BSP_Ser_Printf("\n SOURCE NODE %d: DATE/TIME STAMP MESSAGE\n",data.srcAddr);
        DisplayDate(data.dataPart.dateTime);
        break;
      case 0x07: //Precip msg
        BSP_Ser_Printf("\n SOURCE NODE %d: PRECIPITATION MESSAGE\n",data.srcAddr);
        DisplayPrecip(data.dataPart.depth);
        break;
      case 0x08: //ID msg
        //copy message into a string 1 char longer
        Str_Copy_N(id,(CPU_CHAR *)data.dataPart.id,data.payloadLen-HeaderLength);
        //null terminate string at last char
        id[data.payloadLen-HeaderLength]='\0';
        
        BSP_Ser_Printf("\n SOURCE NODE %d: SENSOR ID MESSAGE\n",data.srcAddr);
        BSP_Ser_Printf("   Node ID = %s\n",id);
        break;
      default: //unknown msg type
        TypeErr();
      }
    }
  }  
}

/*----- D i s p l a y P r e c i p ( ) -----*/

void DisplayPrecip(CPU_INT08U *depth)
{
  CPU_INT08U i;
  CPU_INT32U precipDepth; //Part of BCD depth preceding decimal
  
  //unroll precip depth array into bcd
  for(i=0;i<DepthSize-1;i++)
  {
    //Shift each depth element 4 bytes left times the element we are on
    //in order to unroll into the 32 bit precipDepth
    precipDepth |= depth[i]<<((DepthSize-i-1)*NibbleSize);
  }
  //We don't want last depth element in this var - 
  //it gets printed sep after decimal point
  precipDepth >>= NibbleSize;
  
  BSP_Ser_Printf("   Precipitation Depth = %x.%x\n",
    depth[0],
    depth[DepthSize-1]);
  
  return;
}

/*----- D i s p l a y W i n d ( ) -----*/

void DisplayWind(CPU_INT08U *speed,CPU_INT16U dir)
{
  CPU_INT08U i;
  CPU_INT32U windSpeed;
  
  //swap bytes for endianness
  dir = (dir<<ByteSize) |
        (dir>>ByteSize);
        
  //unroll wind speed array into bcd
  for(i=0;i<SpeedSize-1;i++)
  {
    //Shift each depth element 4 bytes left times the element we are on
    //in order to unroll into the 32 bit windSpeed
    windSpeed |= speed[i]<<((SpeedSize-i-1)*(ByteSize/2));
  }
  //Grab 1st half of last speed element into windSpeed
  //The last half gets printed sep after decimal point
  windSpeed |= speed[i]>>(ByteSize/2);
        
  BSP_Ser_Printf("   Speed = %x.%x Wind Direction = %d\n",
    windSpeed,
    speed[i]&SpeedDecimalMask,
    dir);
  
  return;
}

/*----- D i s p l a y D a t e ( ) -----*/

void DisplayDate(CPU_INT32U date)
{
  CPU_INT16U mask = ~0; //Make 0xFFFF
  
  // swap bytes for endianness
  // swap ends then middle two
  date = ((date<<EndBytePosition)&(CPU_INT32U)(ByteMask<<EndBytePosition)) | 
         ((date>>EndBytePosition)&ByteMask) |
         ((date>>MiddleBytePosition)&(ByteMask<<MiddleBytePosition)) |
         ((date<<MiddleBytePosition)&(ByteMask<<(EndBytePosition-MiddleBytePosition)));
  
  // mask out date parts: shift by position then 
  // generate a bitmask based on length of field
  BSP_Ser_Printf("   Time Stamp = %u/%u/%u %u:%u \n",
    date>>(MonthPosition)&(mask>>(sizeof(mask)*ByteSize-MonthLength)),
    date>>(DayPosition)&(mask>>(sizeof(mask)*ByteSize-DayLength)),
    date>>(YearPosition)&(mask>>(sizeof(mask)*ByteSize-YearLength)),
    date>>(HourPosition)&(mask>>(sizeof(mask)*ByteSize-HourLength)),
    date>>(MinutePosition)&(mask>>(sizeof(mask)*ByteSize-MinuteLength)));
  
  return;
}
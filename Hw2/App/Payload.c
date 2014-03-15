/*--------------- P a y l o a d . c ---------------*/

/*
by:	David Tyler
	  Embedded Real Time Systems
	  Electrical and Computer Engineering Dept.
	  UMASS Lowell

PURPOSE
Handles the processing of packets

CHANGES
02-05-2015 dwt - File Created
*/

#include "includes.h"
#include "BfrPair.h"
#include "Payload.h"
#include "Reply.h"
#include "Error.h"

//typedef enum {TempMsg,,P3,L,D,C,ER,ER2,ER3};
void PayloadInit(BfrPair **payloadPair,BfrPair **replyPair)
{
  // Create and initialize payload buffer pair.
  BfrPairInit(&payloadBfrPair,pBfr0Space,pBfr1Space,PayloadBfrSize);
  // Create and initialize reply buffer pair.
  BfrPairInit(&replyBfrPair,rBfr0Space,rBfr1Space,ReplyBfrSize);
  
  *payloadPair = &payloadBfrPair;
  *replyPair = &replyBfrPair;

}
void PayloadTask(void)
{
  CPU_INT08U msgBfr[160];
  CPU_CHAR id[IdSize+1];
  Payload *payload;
  
  //If possible, Swap the payload buffer pair
  if(BfrPairSwappable(&payloadBfrPair))
    BfrPairSwap(&payloadBfrPair);
  
  if(!GetBfrClosed(&payloadBfrPair) || PutBfrClosed(&replyBfrPair))
    return;
  
  payload = (Payload *) GetBfrAddr(&payloadBfrPair);
  
  PutBfrReset(&replyBfrPair);
  
  //check for errors
  if(payload->payloadLen<0)
    payload->msgType = ErrMsg;
    
  switch(payload->msgType)
  {
  case ErrMsg:
    HandleErr(msgBfr,payload->payloadLen);
    break;
  case TempMsg: //Temperature Message
    sprintf((CPU_CHAR *) msgBfr, "\n SOURCE NODE %d: TEMPERATURE MESSAGE\n"
                                 "   Temperature = %d\n",
                                     payload->srcAddr,
                                     payload->dataPart.temp);
    break;
  case BaroMsg: //Barometric Pressure msg
    //swap bytes to fix endianness
    payload->dataPart.pres = (payload->dataPart.pres<<ByteSize) | 
                         (payload->dataPart.pres>>ByteSize);
    
    sprintf((CPU_CHAR *) msgBfr, "\n SOURCE NODE %d: BAROMETRIC PRESSURE MESSAGE\n"
                                 "   Pressure = %u\n", payload->srcAddr,
                                                       payload->dataPart.pres);
    break;
  case HumMsg: //Humidity msg
    sprintf((CPU_CHAR *) msgBfr, "\n SOURCE NODE %d: HUMIDITY MESSAGE\n   "
                                 "Dew Point = %d Humidity = %d\n", 
                                  payload->srcAddr,
                                  payload->dataPart.hum.dewPt,
                                  payload->dataPart.hum.hum);
    break;
  case WindMsg: //Wind msg
    DisplayWind(msgBfr,payload->srcAddr,payload->dataPart.wind.speed,payload->dataPart.wind.dir);
    break;
  case RadMsg: //Radiation msg
    //swap bytes to fix endianness
    payload->dataPart.rad = (payload->dataPart.rad<<ByteSize) | 
                        (payload->dataPart.rad>>ByteSize);
    sprintf((CPU_CHAR *) msgBfr, "\n SOURCE NODE %d: SOLAR RADIATION MESSAGE\n"
                                 "   Solar Radiation Intensity = %u\n", 
                                     payload->srcAddr,
                                     payload->dataPart.rad);
    break;
  case TimeMsg: //Date/Time msg 
    DisplayDate(msgBfr,payload->srcAddr,payload->dataPart.dateTime);
    break;
  case PrecipMsg: //Precip msg
    DisplayPrecip(msgBfr,payload->srcAddr,payload->dataPart.depth);
    break;
  case IDMsg: //ID msg
    //copy message into a string 1 char longer
    Str_Copy_N(id,(CPU_CHAR *)payload->dataPart.id,payload->payloadLen-HeaderLength);
    //null terminate string at last char
    id[payload->payloadLen-HeaderLength]='\0';
    
    sprintf((CPU_CHAR *) msgBfr, "\n SOURCE NODE %d: SENSOR ID MESSAGE\n"
                                 "   Node ID = %s\n", 
                                     payload->srcAddr,
                                     id);
    break;
  default: //unknown msg type
    break;
  }
  
  PutReplyMsg(&replyBfrPair, msgBfr);
  
  ClosePutBfr(&replyBfrPair);
  OpenGetBfr(&payloadBfrPair);
}
/*----- D i s p l a y P r e c i p ( ) -----*/

void DisplayPrecip(CPU_INT08U *msgBfr,CPU_INT08U addr,CPU_INT08U *depth)
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

  sprintf((CPU_CHAR *) msgBfr, "\n SOURCE NODE %d: PRECIPITATION MESSAGE\n"
                               "   Precipitation Depth = %x.%x\n", 
                                   addr,
                                   depth[0],
                                   depth[DepthSize-1]);
  return;
}

/*----- D i s p l a y W i n d ( ) -----*/

void DisplayWind(CPU_INT08U *msgBfr,CPU_INT08U addr,CPU_INT08U *speed,CPU_INT16U dir)
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
        
  //BSP_Ser_Printf("   Speed = %x.%x Wind Direction = %d\n",
 //   windSpeed,
 //   speed[i]&SpeedDecimalMask,
 //   dir);
  sprintf((CPU_CHAR *) msgBfr, "\n SOURCE NODE %d: WIND MESSAGE\n   "
                               "Speed = %x.%x Wind Direction = %d\n", 
                                     addr,
                                     windSpeed,
                                     speed[i]&SpeedDecimalMask,
                                     dir);
  return;
}

/*----- D i s p l a y D a t e ( ) -----*/

void DisplayDate(CPU_INT08U *msgBfr,CPU_INT08U addr,CPU_INT32U date)
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
  sprintf((CPU_CHAR *) msgBfr, "\n SOURCE NODE %d: DATE/TIME STAMP MESSAGE\n"
                               "   Time Stamp = %u/%u/%u %u:%u \n", 
                                   addr,
    date>>(MonthPosition)&(mask>>(sizeof(mask)*ByteSize-MonthLength)),
    date>>(DayPosition)&(mask>>(sizeof(mask)*ByteSize-DayLength)),
    date>>(YearPosition)&(mask>>(sizeof(mask)*ByteSize-YearLength)),
    date>>(HourPosition)&(mask>>(sizeof(mask)*ByteSize-HourLength)),
    date>>(MinutePosition)&(mask>>(sizeof(mask)*ByteSize-MinuteLength)));  
  
  return;
}
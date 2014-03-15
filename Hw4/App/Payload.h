#ifndef __payload__
#define __payload__
/*--------------- P a y l o a d . h ---------------*/

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
#include <includes.h>
#include "BfrPair.h"

#pragma pack(1) // Don’t align on word boundaries

/*----- c o n s t a n t    d e f i n i t i o n s -----*/

#define ErrMsg 0
#define TempMsg 1
#define BaroMsg 2
#define HumMsg 3
#define WindMsg 4
#define RadMsg 5
#define TimeMsg 6
#define PrecipMsg 7
#define IDMsg 8

#define DEST_ADDR 1
#define HeaderLength 4

#define SpeedSize 2
#define DepthSize 2
#define IdSize 10

#define ByteSize 8
#define NibbleSize 4
#define ByteMask 0xFF

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
  CPU_INT08S payloadLen;
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

// Size of buffer data spaces
#define PayloadBfrSize 14

// Define the payload buffer pair.
static BfrPair payloadBfrPair;
static CPU_INT08U pBfr0Space[PayloadBfrSize];
static CPU_INT08U pBfr1Space[PayloadBfrSize];

//Semaphores
static OS_SEM openPayloadBfrs;
static OS_SEM closedPayloadBfrs;

/*----- f u n c t i o n    p r o t o t y p e s -----*/
void PayloadInit(void);
void PayloadTask(void *data);
void DisplayPrecip(CPU_INT08U *msgBfr,CPU_INT08U addr,CPU_INT08U *depth);
void DisplayWind(CPU_INT08U *msgBfr,CPU_INT08U addr,CPU_INT08U *speed,CPU_INT16U dir);
void DisplayDate(CPU_INT08U *msgBfr,CPU_INT08U addr,CPU_INT32U date);

#endif
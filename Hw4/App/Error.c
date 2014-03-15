/*--------------- E r r o r . c ---------------*/

/*
by:	David Tyler
	  Embedded Real Time Systems
	  Electrical and Computer Engineering Dept.
	  UMASS Lowell

PURPOSE
Provide error methods to other modules

CHANGES
02-05-2015 dwt - File Created
*/

#include "includes.h"
#include "Error.h"

void HandleErr(CPU_INT08U *msgBfr,CPU_INT08S len)
{
  switch(len)
  {
  case P1Err:
    sprintf((CPU_CHAR *) msgBfr, " \a*** ERROR: Bad Preamble Byte %d\n", -1*P1Err);
    return;
  case P2Err:
    sprintf((CPU_CHAR *) msgBfr, " \a*** ERROR: Bad Preamble Byte %d\n", -1*P2Err);
    return;
  case P3Err:
    sprintf((CPU_CHAR *) msgBfr, " \a*** ERROR: Bad Preamble Byte %d\n", -1*P3Err);
    return;
  case CheckErr:
    sprintf((CPU_CHAR *) msgBfr, " \a*** ERROR: Checksum error\n");
    return;
  case SizeErr:
    sprintf((CPU_CHAR *) msgBfr, " \a*** ERROR: Bad Packet Size\n");
    return;
  default:
    return;
  }
}
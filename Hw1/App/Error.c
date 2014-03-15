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

//Report Preamble error
void PreambleErr(CPU_INT08S preambleByte)
{
  BSP_Ser_Printf(" \a*** ERROR: Bad Preamble Byte %d\n",preambleByte+1);
  return;
}
//Report Checksum error
void CheckErr(void)
{
  BSP_Ser_Printf(" \a*** ERROR: Checksum error\n");
  return;
}
//Report Length error
void LengthErr(void)
{
  BSP_Ser_Printf(" \a*** ERROR: Bad Packet Size\n");
  return;
}
//Report Address error
void AddressErr(void)
{
  BSP_Ser_Printf(" \a*** INFO: Not My Address\n");
  return;
}
//Report Type error
void TypeErr(void)
{
  BSP_Ser_Printf(" \a*** ERROR: Unknown Message Type\n");
  return;
}
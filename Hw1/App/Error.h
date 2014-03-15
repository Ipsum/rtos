#ifndef __error__
#define __error__
/*--------------- E r r o r . c ---------------*/

/*
by:	David Tyler
	  Embedded Real Time Systems
	  Electrical and Computer Engineering Dept.
	  UMASS Lowell

PURPOSE
This header file defines the public names (functions and types)
exported from the module "Error.c"

CHANGES
02-05-2015 dwt - File Created
*/

/*----- f u n c t i o n    p r o t o t y p e s -----*/

void PreambleErr(CPU_INT08S preambleByte);
void CheckErr(void);
void LengthErr(void);
void AddressErr(void);
void TypeErr(void);

#endif
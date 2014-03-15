#ifndef SerIODriver_H
#define SerIODriver_H

/*=============== S e r I O D r i v e r . h ===============*/

/*
BY:	George Cheney
		16.472 / 16.572 Embedded Real Time Systems
		Electrical and Computer Engineering Dept.
		UMASS Lowell
*/

/*
PURPOSE
Interface to SerIODriver.c

CHANGES
03-10-2011  - Release to class.
*/

#include <includes.h>
#include <assert.h>

//----- f u n c t i o n    p r o t o t y p e s -----

void PutByte(CPU_INT16S c);
void ISR(void);
void InitSerIO(void);

#endif
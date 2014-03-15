#ifndef Consumer_H
#define Consumer_H

/*=============== C o n s u m e r . h ===============*/

/*
BY:	George Cheney
		16.472 / 16.572 Embedded Real Time Systems
		Electrical and Computer Engineering Dept.
		UMASS Lowell
*/

/*
PURPOSE
The interface to the module Consumer.c

CHANGES
03-10-2011  - Release to class.
*/

#include <includes.h>
#include <assert.h>
#include "SerIODriver.h"

//----- f u n c t i o n    p r o t o t y p e s -----

void CreateConsumerTask(void);

#endif
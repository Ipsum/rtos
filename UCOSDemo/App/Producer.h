#ifndef Producer_H
#define Producer_H
/*=============== P r o d u c e r . h ===============*/

/*
BY:	George Cheney
		16.472 / 16.572 Embedded Real Time Systems
		Electrical and Computer Engineering Dept.
		UMASS Lowell
*/

/*
PURPOSE
Interface to the module Producer.c

CHANGES
03-10-2011  - Release to class.
*/

#include <includes.h>
#include <assert.h>

#include "SerIODriver.h"

//----- c o n s t a n t    d e f i n i t i o n s -----

#define BfrSize	512             // Circular buffer size

//----- S e m a p h o r e s -----

extern OS_SEM	bytesAvail;	  /* Number of data bytes available in buffer. */
extern OS_SEM	spacesAvail;	/* Number of empty spaces available in buffer. */

//----- t y p e    d e f i n i t i o n s -----

typedef struct
{
	volatile    CPU_INT16U	tail;	/* -- Input index */
	volatile    CPU_INT16U	head;	/* -- output index */
	CPU_CHAR		bfr[BfrSize];	    /* -- Buffer space */
} CircBfr;

// Circular buffer to pass data from Producer to Consumer
extern CircBfr	cBfr;

//----- f u n c t i o n    p r o t o t y p e s -----

void CreateProducerTask(void);

#endif
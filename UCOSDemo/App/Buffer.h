#ifndef BUFFER_H
#define BUFFER_H

/*=============== B u f f e r . h ===============*/

/*
BY:	    George Cheney
		16.472 / 16.572 Embedded Real Time Systems
		Electrical and Computer Engineering Dept.
		UMASS Lowell
*/

/*
PURPOSE
Provide a set of operations on buffers (not circular)


CHANGES
03-27-2008 gpc - Create for S2008.
*/

/*----- c o n s t a n t    d e f i n i t i o n s -----*/

// Define the buffer capacity

#ifndef BfrSize
#define BfrSize 4
#endif



/*----- t y p e    d e f i n i t i o n s -----*/

// Define the buffer type

typedef struct
{
	CPU_INT08U  *in;             // Points to space to add next byte
	CPU_INT08U	*out;            // Points to next byte to remove
	CPU_INT08U	bfr[BfrSize];    // The buffer storage area
} Buffer;


/*----- f u n c t i o n    p r o t o t y p e s -----*/

CPU_BOOLEAN Empty(Buffer *theBfr);
CPU_BOOLEAN Full(Buffer *theBfr);
CPU_INT16S AddByte(Buffer *theBfr, CPU_INT16S theByte);
CPU_INT16S RemoveByte(Buffer *theBfr);
void InitBfr(Buffer *bfr);
#endif

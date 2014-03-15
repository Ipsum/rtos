#ifndef __buffer__
#define __buffer__
/*--------------- B u f f e r . h ---------------

by: David Tyler
    UMASS Lowell

PURPOSE
Provide buffers and functions for managing buffers for coop multitasking

CHANGES
02-25-2013 dwt -  Created
*/
#include "includes.h"

typedef struct
{
CPU_BOOLEAN closed; /* -- True if buffer has data ready to process */
CPU_INT16U size; /* -- The capacity of the buffer in bytes */
CPU_INT16U putIndex; /* -- The position where the next byte is added */
CPU_INT16U getIndex; /* -- The position of the next byte to remove */
CPU_INT08U *buffer; /* -- The address of the buffer data space */
} Buffer;

/*----- f u n c t i o n    p r o t o t y p e s -----*/
void BfrInit(Buffer *bfr,CPU_INT08U *bfrSpace,CPU_INT16U size);
void BfrReset(Buffer *bfr);
CPU_BOOLEAN BfrClosed(Buffer *bfr);
void BfrClose(Buffer *bfr);
void BfrOpen(Buffer *bfr);
CPU_BOOLEAN BfrFull(Buffer *bfr);
CPU_BOOLEAN BfrEmpty(Buffer *bfr);
CPU_INT16S BfrAddByte(Buffer *bfr, CPU_INT16S theByte);
CPU_INT16S BfrNextByte(Buffer *bfr);
CPU_INT16S BfrRemoveByte(Buffer *bfr);

#endif
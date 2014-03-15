#ifndef __bfrpair__
#define __bfrpair__
/*--------------- B f r P a i r . h ---------------

by: David Tyler
    UMASS Lowell

PURPOSE
Provide buffer pairs and functions for managing buffers for coop multitasking

CHANGES
02-25-2013 dwt -  Created
*/
#include "Buffer.h"

#ifndef NumBfrs
#define NumBfrs 2
#endif

/*----- c o n s t a n t   d e f i n a t i o n s -----*/
typedef struct
{
  CPU_INT08U putBfrNum; /* -- The index of the put buffer */
  Buffer buffers[NumBfrs]; /* -- The 2 buffers */
} BfrPair;

/*----- f u n c t i o n    p r o t o t y p e s -----*/
void BfrPairInit(BfrPair *bfrPair, CPU_INT08U *bfr0Space, 
                 CPU_INT08U *bfr1Space, CPU_INT16U size);
    
void PutBfrReset(BfrPair *bfrPair);
CPU_INT08U *PutBfrAddr(BfrPair *bfrPair);
CPU_INT08U *GetBfrAddr(BfrPair *bfrPair);
CPU_BOOLEAN PutBfrClosed(BfrPair *bfrPair);
CPU_BOOLEAN GetBfrClosed(BfrPair *bfrPair);
void ClosePutBfr(BfrPair *bfrPair);
void OpenGetBfr (BfrPair *bfrPair);
CPU_INT16S PutBfrAddByte(BfrPair *bfrPair, CPU_INT16S byte);
CPU_INT16S GetBfrNextByte(BfrPair *bfrPair);
CPU_INT16S GetBfrRemByte(BfrPair *bfrPair);
CPU_BOOLEAN BfrPairSwappable(BfrPair *bfrPair);
void BfrPairSwap(BfrPair *bfrPair);

#endif
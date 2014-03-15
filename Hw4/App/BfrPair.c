/*--------------- B f r P a i r . c ---------------

by: David Tyler

PURPOSE
Provide buffer pairs and functions for managing buffer pairs for coop 
multitasking.

CHANGES
02-24-2013 dwt -  Created
*/
#include "includes.h"
#include "Buffer.h"
#include "BfrPair.h"

/*--------------- B f r P a i r I n i t ( ) ---------------

PURPOSE
Initialize both buffers of the buffer pair.

INPUT PARAMETERS
bfrPair - buffer pair address
bfr0Space - address of buffer 0 space
bfrlSpace - address of buffer 1 space
size - buffer capacity in bytes
*/
void BfrPairInit( BfrPair *bfrPair, CPU_INT08U *bfr0Space, 
                 CPU_INT08U *bfr1Space, CPU_INT16U size)
{
  //Writing a pointless comment saying we are init'ing the buffers!
  BfrInit(&bfrPair->buffers[0],bfr0Space,size);
  BfrInit(&bfrPair->buffers[1],bfr1Space,size);
}

/*--------------- P u t B f r R e s e t ( ) ---------------

PURPOSE
Reset the put buffer.

INPUT PARAMETERS
bfrPair - buffer pair address
*/
void PutBfrReset(BfrPair *bfrPair)
{
  BfrReset(&bfrPair->buffers[bfrPair->putBfrNum]);
}

/*--------------- P u t B f r A d d r ( ) ---------------

PURPOSE
Obtain the address of the put buffer’s buffer data
space.

INPUT PARAMETERS
bfrPair - buffer pair address

RETURN VALUE
The address of the put buffer’s buffer data space.
*/
CPU_INT08U *PutBfrAddr(BfrPair *bfrPair)
{
  return bfrPair->buffers[bfrPair->putBfrNum].buffer;
}

/*--------------- G e t B f r A d d r ( ) ---------------

PURPOSE
Obtain the address of the get buffer’s buffer data space.

INPUT PARAMETERS
bfrPair - buffer pair address

RETURN VALUE
The address of the put buffer’s buffer data space.
*/
CPU_INT08U *GetBfrAddr(BfrPair *bfrPair)
{
  return bfrPair->buffers[(NumBfrs-1)-bfrPair->putBfrNum].buffer;
}

/*--------------- P u t B f r C l o s e d ( ) ---------------

PURPOSE
Test whether or not the put buffer is closed.

INPUT PARAMETERS
bfrPair - buffer pair address

RETURN VALUE
TRUE if the put buffer is closed, otherwise FALSE
*/
CPU_BOOLEAN PutBfrClosed(BfrPair *bfrPair)
{
  return BfrClosed(&bfrPair->buffers[bfrPair->putBfrNum]);
}

/*--------------- G e t B f r C l o s e d ( ) ---------------

PURPOSE
Test whether or not the get buffer is closed.

INPUT PARAMETERS
bfrPair - buffer pair address

RETURN VALUE
TRUE if the get buffer is closed, otherwise FALSE
*/
CPU_BOOLEAN GetBfrClosed(BfrPair *bfrPair)
{
  CPU_INT08U getBfrNum = (NumBfrs-1)-(bfrPair->putBfrNum);
  
  return BfrClosed(&bfrPair->buffers[getBfrNum]);
}

/*--------------- C l o s e P u t B f r ( ) ---------------

PURPOSE
Mark the put buffer closed.

INPUT PARAMETERS
bfrPair - buffer pair address
*/
void ClosePutBfr(BfrPair *bfrPair)
{
  BfrClose(&bfrPair->buffers[bfrPair->putBfrNum]);
}

/*--------------- O p e n G e t B f r ( ) ---------------

PURPOSE
Mark the get buffer open.

INPUT PARAMETERS
bfrPair - buffer pair address
*/
void OpenGetBfr(BfrPair *bfrPair)
{
  CPU_INT08U getBfrNum = (NumBfrs-1)-(bfrPair->putBfrNum);
  
  BfrOpen(&bfrPair->buffers[getBfrNum]);
}

/*--------------- P u t B f r A d d B y t e ( ) ---------------

PURPOSE
Add a byte to the put buffer at position “putIndex” and increment
“putIndex” by 1. If the buffer becomes full, mark it closed.

INPUT PARAMETERS
bfrPair - buffer pair address
byte - the byte to be added

RETURN VALUE
The byte added, unless the buffer was full. If the
buffer was full, return -1.
*/
CPU_INT16S PutBfrAddByte(BfrPair *bfrPair, CPU_INT16S byte)
{
  return BfrAddByte(&bfrPair->buffers[bfrPair->putBfrNum],byte);
}

/*--------------- G e t B f r N e x t B y t e ( ) ---------------

PURPOSE
Return the byte from position “getIndex” of the get buffer or return -1 if
the get buffer is empty.

INPUT PARAMETERS
bfrPair - buffer pair address

RETURN VALUE
The byte from position “getIndex” of the get buffer unless the get
buffer is empty. If the buffer is empty, return -1.
*/
CPU_INT16S GetBfrNextByte(BfrPair *bfrPair)
{
  return BfrNextByte(&bfrPair->buffers[!(bfrPair->putBfrNum)]);
}

/*--------------- G e t B f r R e m B y t e ( ) ---------------

PURPOSE
Return the byte from position “getIndex” in the get buffer 
and increment the get buffer’s “getIndex” by 1. 
If the buffer becomes empty, mark it open.

INPUT PARAMETERS
bfrPair - buffer pair address

RETURN VALUE
The byte from position “getIndex” of the get buffer unless 
the get buffer is empty. If the get buffer is empty, return -1.
*/
CPU_INT16S GetBfrRemByte(BfrPair *bfrPair)
{
  CPU_INT08U getBfrNum = !(bfrPair->putBfrNum);
  
  return BfrRemoveByte(&bfrPair->buffers[getBfrNum]);
}

/*--------------- B f r P a i r S w a p p a b l e ( ) ---------------

PURPOSE
Test whether or not a buffer pair is ready to be swapped. 
It is ready if the put buffer is closed and the get buffer is open.

INPUT PARAMETERS
bfrPair - buffer pair address

RETURN VALUE
TRUE if ready to swap, otherwise FALSE.
*/
CPU_BOOLEAN BfrPairSwappable(BfrPair *bfrPair)
{
  CPU_INT08U getBfrNum = !bfrPair->putBfrNum;
  
  if(BfrClosed(&bfrPair->buffers[bfrPair->putBfrNum]) &&
     !BfrClosed(&bfrPair->buffers[getBfrNum]))
  {
    //ready to swap, return true
    return 1;
  }
  return 0;
}

/*--------------- B f r P a i r S w a p ( ) ---------------

PURPOSE
Swap the put buffer and the get buffer, and reset the put buffer.

INPUT PARAMETERS
bfrPair - buffer pair address
*/
void BfrPairSwap(BfrPair *bfrPair)
{
  //swap buffers
  bfrPair->putBfrNum = !(bfrPair->putBfrNum);
  //reset put buffer
  BfrReset(&bfrPair->buffers[bfrPair->putBfrNum]);
  
  return;
}
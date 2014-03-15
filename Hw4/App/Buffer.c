/*--------------- B u f f e r . c ---------------

by: David Tyler

PURPOSE
Provide buffers and functions for managing buffers for coop multitasking

CHANGES
02-24-2013 dwt -  Created
*/
#include "includes.h"
#include "Buffer.h"

/*--------------- B f r I n i t ( ) ---------------

PURPOSE
Initialize a buffer: record the size, set putIndex and
getIndex to zero, and mark the buffer open.

INPUT PARAMETERS
bfr - buffer address
bfrSpace - address of the buffer data space
size - buffer capacity in bytes
*/
void BfrInit( Buffer *bfr,CPU_INT08U *bfrSpace,CPU_INT16U size)
{
  //Record the size
  bfr->size=size;
  //set put and get index to 0
  bfr->getIndex=0;
  bfr->putIndex=0;
  //set the address of the data space
  bfr->buffer=bfrSpace;
  //mark the buffer open
  BfrOpen(bfr);
  
  
  return;
}

/*--------------- B f r R e s e t ( ) ---------------

PURPOSE
Reset the buffer: set putIndex and getIndex to
zero, and mark the buffer open.

INPUT PARAMETERS
bfr - buffer address
*/
void BfrReset(Buffer *bfr)
{
  //set put and get index to 0
  bfr->getIndex=0;
  bfr->putIndex=0;
  //mark the buffer open
  BfrOpen(bfr);
  
  return;
}

/*--------------- B f r C l o s e d ( ) ---------------

PURPOSE
Test whether or not a buffer is closed.

INPUT PARAMETERS
bfr - buffer address

RETURN VALUE
TRUE if closed, otherwise FALSE
*/
CPU_BOOLEAN BfrClosed(Buffer *bfr)
{
  return bfr->closed;
}

/*--------------- B f r C l o s e ( ) ---------------

PURPOSE
Mark the buffer closed.

INPUT PARAMETERS
bfr - buffer address
*/
void BfrClose(Buffer *bfr)
{
  bfr->closed=1;
  
  return;
}

/*--------------- B f r O p e n ( ) ---------------

PURPOSE
Mark the buffer open.

INPUT PARAMETERS
bfr - buffer address
*/
void BfrOpen(Buffer *bfr)
{
  bfr->closed=0;
  
  return;
}

/*--------------- B f r F u l l ( ) ---------------

PURPOSE
Test whether or not a buffer is full.

INPUT PARAMETERS
bfr - buffer address

RETURN VALUE
TRUE if full, otherwise FALSE
*/
CPU_BOOLEAN BfrFull(Buffer *bfr)
{
  if(bfr->putIndex >= bfr->size)
    return TRUE;
  
  return FALSE;
}

/*--------------- B f r E m p t y ( ) ---------------

PURPOSE
Test whether or not a buffer is empty.

INPUT PARAMETERS
bfr - buffer address

RETURN VALUE
TRUE if empty, otherwise FALSE
*/
CPU_BOOLEAN BfrEmpty(Buffer *bfr)
{
  if(bfr->getIndex == bfr->putIndex)
    return TRUE;
  
  return FALSE;
}

/*--------------- B f r A d d B y t e ( ) ---------------

PURPOSE
Add a byte to a buffer at position “putIndex” and 
increment “putIndex” by 1. If the buffer becomes full,
mark it closed.

INPUT PARAMETERS
bfr - buffer address
theByte - byte to be added

RETURN VALUE
The byte added, unless the buffer was full. 
If the buffer was full, return -1.
*/
CPU_INT16S BfrAddByte(Buffer *bfr, CPU_INT16S theByte)
{
  //check for a full buffer
  if(BfrFull(bfr))
    return -1;
  
  //insert and increment index
  bfr->buffer[bfr->putIndex++] = theByte;
  
  //close buffer if it fills up
  if(BfrFull(bfr))
    BfrClose(bfr);
  
  return theByte;
}

/*--------------- B f r N e x t B y t e ( ) ---------------

PURPOSE
Return the byte from position “getIndex” or return -1 
if the buffer is empty.

INPUT PARAMETERS
bfr - buffer address

RETURN VALUE
The byte from position “getIndex” unless the buffer is
empty. If the buffer is empty, return -1.
*/
CPU_INT16S BfrNextByte(Buffer *bfr)
{
  if(BfrEmpty(bfr))
    return -1;
  
  return bfr->buffer[bfr->getIndex];
}

/*--------------- B f r R e m o v e B y t e ( ) ---------------

PURPOSE
Return the byte from position “getIndex” and
increment “getIndex” by 1. If the buffer becomes
empty, mark it open.

INPUT PARAMETERS
bfr - buffer address

RETURN VALUE
The byte from position “getIndex” unless the buffer is
empty. If the buffer is empty, return -1.
*/
CPU_INT16S BfrRemoveByte(Buffer *bfr)
{
  CPU_INT16S theByte;
  
  if(BfrEmpty(bfr))
    return -1;
  
  theByte = bfr->buffer[bfr->getIndex++];
  
  //mark open if fills
  if(BfrEmpty(bfr))
    BfrOpen(bfr);
  
  return theByte;
}


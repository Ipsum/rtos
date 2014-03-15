#ifndef __iodriver__
#define __iodriver__
/*--------------- S e r I O D r i v e r . h ---------------

by: David Tyler
    UMASS Lowell

PURPOSE
Provides input and output functions

CHANGES
02-25-2013 dwt -  Created
*/
#include "includes.h"
#include "BfrPair.h"

// If not already defined, use the default buffer size of 4.
#ifndef BfrSize
#define BfrSize 4
#endif

/*----- c o n s t a n t   d e f i n a t i o n s -----*/
#define USART_TXE 0x80
#define USART_RXNE 0x20

// Allocate the input buffer pair.
static BfrPair iBfrPair;
static CPU_INT08U iBfr0Space[BfrSize];
static CPU_INT08U iBfr1Space[BfrSize];

// Allocate the output buffer pair.
static BfrPair oBfrPair;
static CPU_INT08U oBfr0Space[BfrSize];
static CPU_INT08U oBfr1Space[BfrSize];

/*----- f u n c t i o n    p r o t o t y p e s -----*/
void InitSerIO(void);

CPU_INT16S PutByte(CPU_INT16S txChar);
CPU_INT16S GetByte(void);

void ServiceTx(void);
void ServiceRx(void);

#endif
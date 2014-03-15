/*=============== S e r I O D r i v e r . c ===============*/

/*
BY:	George Cheney
		16.472 / 16.572 Embedded Real Time Systems
		Electrical and Computer Engineering Dept.
		UMASS Lowell
*/

/*
PURPOSE
Provides USART2 Tx support

CHANGES
03-10-2011  - Release to class.
*/

#include "SerIODriver.h"

//----- c o n s t a n t    d e f i n i t i o n s -----

#define USART_TXEIE 0x80        // Unmask Tx Interrupts
#define USART_TXE   0x80        // Tx Empty Bit

// IRQ38 Definitions
#define SETENA1   (*((CPU_INT32U *) 0XE000E104))
#define USART2ENA 0x00000040   

#define SuspendTimeout 100	    // Timeout for semaphore wait

//----- g l o b a l    v a r i a b l e s -----

static OS_SEM	txDone;			  /* Binary semaphore: Consumer waits until transmit done */

/*--------------- I n i t S e r I O ( ) ---------------*/

/*
PURPOSE
Initialize the serial I/O driver module by ceating the txDone semaphore and
unmasking the Tx interrupt.
*/

void InitSerIO(void)
{
  OS_ERR  osErr;  // O/S Error Code
  
  // Create the TxDone semaphore.
  OSSemCreate(&txDone, "Tx Done", 1, &osErr);	/* -- Unblock the Tx. */
  assert(osErr == OS_ERR_NONE);
 
  // Unmask Tx interrupts. 
  USART2->CR1 |= USART_TXEIE;

  // Enable IRQ38.
  SETENA1 = USART2ENA;
}

/*--------------- P u t B y t e ( ) ---------------*/

/*
PURPOSE
Output 1 character to USART2.

INPUT PARAMETERS
c		-- the character to transmit
*/
void PutByte(CPU_INT16S c)
{
	OS_ERR		osErr;		/* -- Semaphore error code */

	/* Wait for xmit done and buffer not empty. */
	OSSemPend(&txDone, SuspendTimeout, OS_OPT_PEND_BLOCKING, NULL, &osErr);
		
	assert(osErr==OS_ERR_NONE);
	
	/* Write the character to the tx. */
	USART2->DR = c;
  
  // Unmask Tx interrupts.
  USART2->CR1 |= USART_TXEIE;
}

/*--------------- I S R ( ) ---------------*/

/*
PURPOSE
Interrupt handler. 
Signal on tx interrupt to indicate tx ready for data.
*/
void ISR(void)
{
	OS_ERR	osErr;	/* O/S Error code */
	
  /* Disable interrupts. */
  CPU_SR_ALLOC();
  OS_CRITICAL_ENTER();  
  
	/* Tell kernel we're in an ISR. */
	OSIntEnter();

  /* Enable interrupts. */
  OS_CRITICAL_EXIT();
	
	/* Check for a tx event */
	if (USART2->SR & USART_TXE)
		{
		/* Signal the Consumer that the tx is done. */
		OSSemPost(&txDone, OS_OPT_POST_1, &osErr);
		assert(osErr==OS_ERR_NONE);
    
    // Mask TX interrupts.
    USART2->CR1 &= ~USART_TXEIE;
    }
	   
	/* Give the O/S a chance to swap tasks. */
	OSIntExit ();
}

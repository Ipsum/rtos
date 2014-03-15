/*=============== C o n s u m e r . c ===============*/

/*
BY:	George Cheney
		16.472 / 16.572 Embedded Real Time Systems
		Electrical and Computer Engineering Dept.
		UMASS Lowell
*/

/*
PURPOSE
Consumer task: take data from the Producer task and output
it to the Tx.

CHANGES
03-10-2011  - Release to class.
*/

#include "Producer.h"
#include "Consumer.h"

//----- c o n s t a n t    d e f i n i t  i o n s -----

#define SuspendTimeout 100	    // Timeout for semaphore wait

#define CONSUMER_STK_SIZE 128  // Consumer task Priority
#define ConsumerPrio 3          // Consumer task Priority

//----- g l o b a l    v a r i a b l e s -----

static  OS_TCB   consumerTCB;                     // Consumer task TCB
static  CPU_STK  consumerStk[CONSUMER_STK_SIZE];  // Space for Consumer task stack

//----- f u n c t i o n    p r o t o t y p e s -----

void Consumer (void *data);

/*--------------- C r e a t e C o n s u m e r T a s k ( ) ---------------*/

/*
PURPOSE
Create the Consumer task.
*/

void CreateConsumerTask(void)
{
  /* O/S error code */
  OS_ERR  osErr;                            
  
  /* Create the consumer task */
  OSTaskCreate(&consumerTCB,            // Task Control Block                 
               "Consumer Task",         // Task name
               Consumer,                // Task entry point 
               NULL,                    // Address of optional task data block
               ConsumerPrio,            // Task priority
               &consumerStk[0],         // Base address of task stack space
               CONSUMER_STK_SIZE / 10,  // Stack water mark limit
               CONSUMER_STK_SIZE,       // Task stack size
               0,                       // This task has no task queue
               0,                       // Number of clock ticks (defaults to 10)
               (void      *)0,          // Pointer to TCB extension
               0,                       // Task options
               &osErr);                 // Address to return O/S error code
    
  /* Verify successful task creation. */
  assert(osErr == OS_ERR_NONE);
}

/*--------------- C o n s u m e r ( ) ---------------*/

/*
PURPOSE
Read characters from the circular buffer and output to the USART2
transmitter.

INPUT PARAMETERS
data 		--	task data pointer (not used)

GLOBALS
bytesAvail	-- Consumer waits on this. 
spacesAvail	-- Producer waits.
cBfr			-- the circular buffer.
*/
void Consumer (void *data)
{
	CPU_CHAR  c;				  /* -- The next character to output */
	OS_ERR		osErr;			/* -- Semaphore error code */
	
	/* Task runs forever, or until preempted or suspended. */
	for(;;)
	{
		/* Wait for buffer not empty. */
		OSSemPend(&bytesAvail, SuspendTimeout, OS_OPT_PEND_BLOCKING, NULL, &osErr);
		
		assert(osErr==OS_ERR_NONE);
		
		/* Get the next character from the buffer. */
		c = cBfr.bfr[cBfr.head];
		cBfr.head = (cBfr.head + 1) % BfrSize;

		/* Output character to tx. */
		PutByte(c);
				
 		/* Signal the Producer that the buffer is not full. */
		OSSemPost(&spacesAvail, OS_OPT_POST_1, &osErr);
		assert(osErr==OS_ERR_NONE);
	}
}

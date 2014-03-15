/*=============== P r o d u c e r . c ===============*/

/*
BY:	George Cheney
		16.472 / 16.572 Embedded Real Time Systems
		Electrical and Computer Engineering Dept.
		UMASS Lowell
*/

/*
PURPOSE
Producer Task: Generate data for the Consumer task.

CHANGES
03-10-2011  - Release to class.
*/

#include "Producer.h"

// -----c o n s t a n t    d e f i n i t i o n s -----

#define SuspendTimeout 100	    // Timeout for semaphore wait
#define PRODUCER_STK_SIZE 128  // Producer task Priority
#define ProducerPrio 4          // Producer task Priority

//----- g l o b a l    v a r i a b l e s -----

static  OS_TCB   producerTCB;                     // Producer task TCB 
static  CPU_STK  ProducerStk[PRODUCER_STK_SIZE];  // Space for Producer task stack

OS_SEM	bytesAvail;	  /* Number of data bytes available in buffer. */
OS_SEM	spacesAvail;	/* Number of empty spaces available in buffer. */

// Circular buffer to pass data from Producer to Consumer
CircBfr	cBfr;

//----- f u n c t i o n    p r o t o t y p e s -----

void Producer (void *data);

/*--------------- C r e a t e P r o d u c e r T a s k ( ) ---------------*/

/*
PURPOSE
Create the Producer task, initialize the circular buffer, and initialize 
semaphores.

GLOBALS
bytesAvail	-- Consumer waits on this. 
spacesAvail	-- Producer waits.
cBfr			-- the circular buffer.
*/

void CreateProducerTask(void)
{
  /* O/S error code */
  OS_ERR  osErr;
  
  /* Create the producer task */
  OSTaskCreate(&producerTCB,            // Task Control Block                 
               "Producer Task",         // Task name
               Producer,                // Task entry point 
               NULL,                    // Address of optional task data block
               ProducerPrio,            // Task priority
               &ProducerStk[0],         // Base address of task stack space
               PRODUCER_STK_SIZE / 10,  // Stack water mark limit
               PRODUCER_STK_SIZE,       // Task stack size
               0,                       // This task has no task queue
               0,                       // Number of clock ticks (defaults to 10)
               NULL,                    // Pointer to TCB extension
               (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),   // Task options
               &osErr);                 // Address to return O/S error code
  
  /* Verify successful task creation. */
  assert(osErr == OS_ERR_NONE);

  /* Initialize the circular buffer. */
  cBfr.tail = 0;
  cBfr.head = 0;
  
  /* Create and initialize semaphores. */
  OSSemCreate(&bytesAvail, "Bytes Avail", 0, &osErr);			/* -- Empty at start. */
  assert(osErr == OS_ERR_NONE);
  
  OSSemCreate(&spacesAvail, "Spaces Avail", BfrSize, &osErr);	/* -- BfrSize bytes available. */
  assert(osErr == OS_ERR_NONE);
}

/*--------------- P r o d u c e r ( ) ---------------*/

/*
PURPOSE
Generate alphabets and write them to the circular buffer.

INPUT PARAMETERS
data		-- Pointer to task data (not used )

GLOBALS
bytesAvail	-- Consumer waits on this. 
spacesAvail	-- Producer waits.
cBfr			-- the circular buffer.
*/


void Producer (void *data)
{
	CPU_CHAR		c;					/* -- Current character. */
	OS_ERR		  osErr;			/* -- Semaphore error code */
	CPU_CHAR		cNext='A';	/* -- Next character */
	
	/* Task runs forever, or until preempted or suspended. */
	for(;;)
	{
		/* Get next character to write. */
		c = cNext;
		
		/* Advance next character. */
		switch (cNext)
			{
			/* Send linefeed after carriage return. */
			case '\r':
				cNext = '\n';
				break;
			/* Start alphabet over after linefeed. */
			case '\n':
				cNext = 'A';
				break;
			/* Send carriage return after 'Z'. */
			case 'Z':
				cNext = '\r';
				break;
			/* Otherwise, advance to next character in the alphabet. */
			default:
				++cNext;
				break;
			}
		
		/* Wait for buffer space */
		OSSemPend(&spacesAvail, SuspendTimeout, OS_OPT_PEND_BLOCKING, NULL, &osErr);
		
		assert(osErr==OS_ERR_NONE);
		
		/* Write character to the buffer. */
		cBfr.bfr[cBfr.tail] = c;
		cBfr.tail = (cBfr.tail + 1) % BfrSize;
		
		/* Signal the Consumer that the buffer is not empty. */
		OSSemPost(&bytesAvail, OS_OPT_POST_1, &osErr);
		assert(osErr==OS_ERR_NONE);
	}
}
#ifndef Assert_H
#define Assert_H

/*=============== a s s e r t . h ===============*/

/*
BY:	George Cheney
		16.472 / 16.572 Embedded Real Time Systems
		Electrical and Computer Engineering Dept.
		UMASS Lowell

PURPOSE
Replace C library assert macro with one that will break
to the debugger.

CHANGES
03-10-2011  gpc - Release to class.
*/

/*--------------- a s s e r t ( ) ---------------
PURPOSE
On failed assertion, execute a BPKT instruction, trapping to the debugger
allowing inspection of variables at the point of failure.

MACRO PARAMETERS
cond  - a Boolean which if false indicates a failed assertion
*/
#define assert(cond) \
if(!(cond)) \
  asm(" BKPT 0xFF");

#endif
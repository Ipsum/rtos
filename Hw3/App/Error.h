#ifndef __error__
#define __error__
/*--------------- E r r o r . h ---------------*/

/*
by:	David Tyler
	  Embedded Real Time Systems
	  Electrical and Computer Engineering Dept.
	  UMASS Lowell

PURPOSE
This header file defines the public names (functions and types)
exported from the module "Error.c"

CHANGES
02-05-2015 dwt - File Created
*/

/*----- c o n s t a n t   d e f i n a t i o n s -----*/
#define P1Err -1
#define P2Err -2
#define P3Err -3
#define CheckErr -4
#define SizeErr -5

/*----- f u n c t i o n    p r o t o t y p e s -----*/
void HandleErr(CPU_INT08U *msgBfr,CPU_INT08S len);

#endif
#ifndef __pktparser__
#define __pktparser__
/*--------------- P k t P a r s e r . h ---------------*/

/*
by:	David Tyler
	  Embedded Real Time Systems
	  Electrical and Computer Engineering Dept.
	  UMASS Lowell

PURPOSE
This header file defines the public names (functions and types)
exported from the module "PktParser.c"

CHANGES
02-05-2015 dwt - File Created
*/

/*----- f u n c t i o n    p r o t o t y p e s -----*/
CPU_VOID ParsePkt(CPU_VOID *payloadBfrPair);

/*----- c o n s t a n t    d e f i n i t i o n s -----*/
// General Defines
#define ByteSize 8
#define ByteMask 0xFF

//-PktParser-//
#define HeaderLength 4

// Preamble Characters
#define P1Char 0x03
#define P2Char 0xEF
#define P3Char 0xAF

// Packet Length
#define PacketMinLength 0x08

/*----- t y p e    d e f i n i t i o n s -----*/

// Parser States //
typedef enum {P1,P2,P3,L,D,C,ER,ER2,ER3} ParserState;

// The Packet Buffer to return //
typedef struct
{
  CPU_INT08S payloadLen;
  CPU_INT08U data[1];
} PktBfr;

#endif
/*--------------- P k t P a r s e r . c ---------------*/

/*
by:	David Tyler
	  Embedded Real Time Systems
	  Electrical and Computer Engineering Dept.
	  UMASS Lowell

PURPOSE
Read in data from serial io and pass them into a packet structure

CHANGES
02-05-2015 dwt - File Created
*/

/* Include Micrium and STM headers. */
#include "includes.h"

/* Include PktParser and Error module headers. */
#include "PktParser.h"
#include "Payload.h"
#include "BfrPair.h"
#include "SerIODriver.h"
#include "Error.h"
#include "assert.h"

//----- c o n s t a n t    d e f i n i t  i o n s -----

//#define SuspendTimeout 0	    // Timeout for semaphore wait

#define PARSE_STK_SIZE 128  // Consumer task Priority
#define ParsePrio 1          // Consumer task Priority

//----- g l o b a l    v a r i a b l e s -----

static  OS_TCB   parseTCB;                     // Consumer task TCB
static  CPU_STK  parseStk[PARSE_STK_SIZE];  // Space for Parse task

/*--------------- C r e a t e P a r s e T a s k ( ) ---------------*/

/*
PURPOSE
Create the Parser task.
*/
void CreateParseTask(void)
{
   /* O/S error code */
  OS_ERR  osErr;                            
  
  /* Create the consumer task */
  OSTaskCreate(&parseTCB,            // Task Control Block                 
               "Parse Task",         // Task name
               ParsePkt,                // Task entry point 
               NULL,                    // Address of optional task data block
               ParsePrio,            // Task priority
               &parseStk[0],         // Base address of task stack space
               PARSE_STK_SIZE / 10,  // Stack water mark limit
               PARSE_STK_SIZE,       // Task stack size
               0,                       // This task has no task queue
               0,                       // Number of clock ticks (defaults to 10)
               NULL,          // Pointer to TCB extension
               0,                       // Task options
               &osErr);                 // Address to return O/S error code
    
  /* Verify successful task creation. */
  assert(osErr == OS_ERR_NONE);
}
            
// - - - P a r s e P k t - - - //
// Read one packet from RX and //
// extract payload             //
// Inputs:                     //
//  *pktBfr - ptr to payload   //
//  data from RX               //
// Outputs:                    //
//  Payload passed by ref      //
/////////////////////////////////
CPU_VOID ParsePkt(CPU_VOID *data)
{
  static ParserState state;
  static CPU_INT08U checksum;
  CPU_INT16S c;
  static int i;
  OS_ERR osErr;
  
  PktBfr *pktBfr = (PktBfr *) PutBfrAddr(&payloadBfrPair);
  
  while(1)
  {
    if(PutBfrClosed(&payloadBfrPair))
    {
      OSSemPend(&openPayloadBfrs,SuspendTimeout,OS_OPT_PEND_BLOCKING,NULL,&osErr);
      assert(osErr==OS_ERR_NONE);
      
      if(BfrPairSwappable(&payloadBfrPair))
        BfrPairSwap(&payloadBfrPair);
    }
    
    if(PutBfrClosed(&payloadBfrPair))
      return;
    
    //Receive a byte
    c = GetByte();
    BSP_Ser_Printf("%c",c);
    //Block waiting for next byte
    if(c<0)
      return; 
    //XOR byte with current checksum
    checksum ^= c;
    
    switch(state)
    {
    case P1: //Preamble 1
      if(c == P1Char)
        state = P2;
      else
      {
        //Error if wrong char
        pktBfr->payloadLen=P1Err;
        ClosePutBfr(&payloadBfrPair);
        
        OSSemPost(&closedPayloadBfrs,OS_OPT_POST_1,&osErr);
        assert(osErr==OS_ERR_NONE);
        
        state = ER;
      }
      break;
    case P2: //Preamble 2
      if(c == P2Char)
        state = P3;
      else
      {
        //Error if wrong char
        pktBfr->payloadLen=P2Err;
        ClosePutBfr(&payloadBfrPair);
        
        OSSemPost(&closedPayloadBfrs,OS_OPT_POST_1,&osErr);
        assert(osErr==OS_ERR_NONE);
        
        state = ER;
      }
      break;
    case P3: //Preamble 3
      if(c == P3Char)
        state = L;
      else
      {
        //Error if wrong char
        pktBfr->payloadLen=P3Err;
        ClosePutBfr(&payloadBfrPair);
        
        OSSemPost(&closedPayloadBfrs,OS_OPT_POST_1,&osErr);
        assert(osErr==OS_ERR_NONE);
        
        state = ER;
      }
      break;
    case L: //Length
      //Check that it is a valid length
      if(c<PacketMinLength)
      {
        //If length is invalid, error
        pktBfr->payloadLen=SizeErr;
        ClosePutBfr(&payloadBfrPair);
        
        OSSemPost(&closedPayloadBfrs,OS_OPT_POST_1,&osErr);
        assert(osErr==OS_ERR_NONE);
        
        state = ER;
      }
      else
      {
        //Read in length field
        pktBfr->payloadLen = c - HeaderLength;
        state = D;
        i = 0;
      }
      break;
    case D: //Data
      //Read in Data until payloadLen is reached
      pktBfr->data[i++] = c;
      if(i >= pktBfr->payloadLen-1)
        state = C;
      break;
    case C: //Checksum
      //Check that the final bitwise XOR of packet = 0
      if(!(checksum))
      {
        ClosePutBfr(&payloadBfrPair);
        
        OSSemPost(&closedPayloadBfrs,OS_OPT_POST_1,&osErr);
        assert(osErr==OS_ERR_NONE);
        
        state=P1;
        return;
      }
      //Otherwise, report a checksum error
      pktBfr->payloadLen=CheckErr;
      ClosePutBfr(&payloadBfrPair);
      
      OSSemPost(&closedPayloadBfrs,OS_OPT_POST_1,&osErr);
      assert(osErr==OS_ERR_NONE);
        
      state = ER;
      break;
    case ER: //Error State 1
      //Clear checksum
      checksum = 0;
      //If we have a valid preamble char, progress to next error state
      if(c==P1Char)
      {
        state = ER2;
        //Don't lose a checksum char!
        checksum ^= P1Char;
      }
      break;
    case ER2: //Error State 2
      //If we have a valid preamble char, progress to next error state
      if(c==P2Char)
        state = ER3;
      //Otherwise, return to first error state
      else
        state = ER;
      break;
    case ER3: //Error State 3
      //If we have a valid preamble char, we have a real message.
      //Progress to Length state.
      if(c==P3Char)
        state=L;
      //Otherwise, fall back to first error state
      else
        state=ER;
      break;
    }
  }     
}
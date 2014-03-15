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
#include "Error.h"

// - - - P a r s e P k t - - - //
// Read one packet from RX and //
// extract payload             //
// Inputs:                     //
//  *pktBfr - ptr to payload   //
//  data from RX               //
// Outputs:                    //
//  Payload passed by ref      //
/////////////////////////////////
void ParsePkt(void *payloadBfr)
{
  ParserState state = P1;
  CPU_INT16S c;
  CPU_INT08S checksum = 0;
  CPU_INT08U i = 0;
  
  PktBfr *pktBfr = payloadBfr;
  
  while(1)
  {
    //Receive a byte
    c = GetByte();
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
        PreambleErr(state);
        state = ER;
      }
      break;
    case P2: //Preamble 2
      if(c == P2Char)
        state = P3;
      else
      {
        //Error if wrong char
        PreambleErr(state);
        state = ER;
      }
      break;
    case P3: //Preamble 3
      if(c == P3Char)
        state = L;
      else
      {
        //Error if wrong char
        PreambleErr(state);
        state = ER;
      }
      break;
    case L: //Length
      //Check that it is a valid length
      if(c<PacketMinLength)
      {
        //If length is invalid, error
        LengthErr();
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
        return;
      //Otherwise, report an error
      CheckErr();
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
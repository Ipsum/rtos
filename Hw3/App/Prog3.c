/*--------------- P r o g 3 . c ---------------

by: George Cheney
    ECE Dept.
    UMASS Lowell

PURPOSE
Receive wireless sensor network packets from the RS232 port,
interpret and display the messages.

DEMONSTRATES
Cooperative multitasking
Concurrent, polled I/O

CHANGES
01-29-2013 gpc -  Created
*/

#include "includes.h"
#include "Intrpt.h"
#include "BfrPair.h"
#include "Reply.h"
#include "Payload.h"
#include "Error.h"
#include "PktParser.h"
#include "SerIODriver.h"

/*----- c o n s t a n t    d e f i n i t i o n s -----*/

// Define RS232 baud rate.
#define BaudRate 9600

/*----- f u n c t i o n    p r o t o t y p e s -----*/

void AppMain(void);

/*--------------- m a i n ( ) -----------------*/

CPU_INT32S main()
{
//  Initialize the STM32F107 eval. board.
    IntDis();            /* Disable all interrupts. */

    BSP_Init();                 /* Initialize BSP functions */

    BSP_Ser_Init(BaudRate);     /* Initialize the RS232 interface. */

//  Run the application.    
    AppMain();
    
    return 0;
}

/*--------------- A p p M a i n ( ) ---------------

PURPOSE
This is the application main program.

*/

void AppMain(void)
{
  BfrPair *payloadBfrPair; // Address of the Payload Buffer Pair
  BfrPair *replyBfrPair; // Address of the Reply Buffer Pair
  
  // Create and Initialize iBfrPair and oBfrPair.
  InitSerIO();
  
  // Enable interrupts
  IntEn();
  
  // Create and initialize the Payload Buffer Pair and the Reply Buffer
  // Pair.
  PayloadInit(&payloadBfrPair, &replyBfrPair);
  
  // Multitasking Executive Loop: Tasks are executed round robin.
  for (;;)
  {
    // Execute the ParsePkt task.
    ParsePkt(payloadBfrPair);
    
    // Execute the Payload task.
    PayloadTask();
    
    // Execute the Reply Task.
    Reply(replyBfrPair);
  }
} 
/* RX TEST
void AppMain(void)
{
  InitSerIO();
  IntEn();
  
  for(;;)
  {
    static CPU_INT16S c='A';
    
    while(1)
    {
      //BSP_Ser_Printf("222");
      if (PutByte(c)<0)
        break;
      
      if(c<'Z')
        c++;
      else
        c='A';
    }
    ServiceTx();
  }
}
*/
/* TX test
void AppMain(void)
{
  CPU_BOOLEAN putPending = FALSE;
  
  InitSerIO();
  
  IntEn();
  
  //echo byte
  while(TRUE)
  {
    CPU_INT16S c;
    
    ServiceRx();
    for(;;)
    {
      if(!putPending)
        c=GetByte();
      if(c<0)
        break;
      if(PutByte(c)<0)
      {
        putPending=TRUE;
        break;
      }
      else
        putPending = FALSE;
    }
    ServiceTx();
  }
}
*/
//--- S e r I O D r i v e r . c ---

#include "SerIODriver.h"
#include "BfrPair.h"
#include "Buffer.h"
#include "stm32f10x_map.h"

#define SETENA1 (*((CPU_INT32U *) 0xE000E104))
#define USART2ENA 0x00000040
#define USARTINIT 0x20AC
#define TXIEENA 0x80
#define RXIEENA 0x20
/*--------------- I n i t S e r I O ( ) ---------------

PURPOSE
Initialize the RS232 I/O driver by
initializing both iBfrPair and
oBfrPair.

INPUT PARAMETERS
none
*/
void InitSerIO(void)
{
  //init output and input buffer pairs
  BfrPairInit(&iBfrPair,iBfr0Space,iBfr1Space,BfrSize);
  BfrPairInit(&oBfrPair,oBfr0Space,oBfr1Space,BfrSize);
  
  //enable uart, tx, rx, tx interrupt, rx interrupt
  USART2->CR1 |= USARTINIT;
    
  //enable IRQ38
  SETENA1 = USART2ENA;
}

/*--------------- P u t B y t e ( ) ---------------

PURPOSE
If the oBfrPair put buffer is not full, write one byte into the buffer, and
return txChar as the return value; if, the buffer is full, return -1 indicating
failure.

INPUT PARAMETERS
txChar - the byte to be transmitted

RETURN VALUE
On success, PutByte() returns the character in "txChar” that was added to
the put buffer. If the put buffer is already full, txChar returns –1 to indicate
failure.
*/
CPU_INT16S PutByte(CPU_INT16S txChar)
{
  if(BfrPairSwappable(&oBfrPair))
    BfrPairSwap(&oBfrPair);
  
  if(!PutBfrClosed(&oBfrPair))
  {
    //unmask TX interrupt
    USART2->CR1 |= TXIEENA;
  }
     
  return PutBfrAddByte(&oBfrPair,txChar);
}

/*--------------- G e t B y t e ( ) ---------------

PURPOSE
If the iBfrPair get buffer is not empty, remove and return the next
byte from the buffer. if the buffer is empty, return -1 indicating failure.

INPUT PARAMETERS
none

RETURN VALUE
On success, GetByte() returns the character read
from get buffer. If the get buffer is empty, GetByte()
returns –1 to indicate failure.
*/
CPU_INT16S GetByte(void)
{ 
  if(BfrPairSwappable(&iBfrPair))
    BfrPairSwap(&iBfrPair);
  
  if(GetBfrClosed(&oBfrPair))
  {
    //unmask TX interrupt
    USART2->CR1 |= RXIEENA;
  }
  
  return GetBfrRemByte(&iBfrPair);
}

/*--------------- S e r v i c e T x ( ) ---------------

PURPOSE
If TXE = 1 and the oBfrPair get buffer is closed, then output one byte
to the UART Tx and return. If TXE = 0, just return. If the get buffer is
not closed mask the Tx and return

INPUT PARAMETERS
none
*/
void ServiceTx(void)
{
  CPU_INT16S c;
  
  if(USART2->SR & USART_TXE)
  {
    if(!GetBfrClosed(&oBfrPair))
    {
      //mask TX interrupt
      USART2->CR1 = TXIEENA^USARTINIT;
      return;
    }
    
    c = GetBfrRemByte(&oBfrPair);
    
    //buffer empty?
    if(c==-1)
    {
      return;
    }
    //Ok to output
    USART2->DR = c;
    return;
  }
  //TX was 0
  return;
}


/*--------------- S e r v i c e R x ( ) ---------------

PURPOSE
If RXNE = 1 and the iBfrPair put buffer is not full, then read a byte
from the UART Rx and add it to the put buffer. If RXNE = 0 or the put
buffer is full, just return.

INPUT PARAMETERS
none
*/
void ServiceRx(void)
{
  CPU_INT16S c;
  
  if(USART2->SR & USART_RXNE)
  {
    //if buffer is full, return
    if(PutBfrClosed(&iBfrPair))
    {
      USART2->CR1 &= RXIEENA^USARTINIT;
      return;
    }
    
    //now we are ready to get a byte
    c = USART2->DR;
    //add it to put buffer
    PutBfrAddByte(&iBfrPair,c);
    
    //done!
    return;
  }
  //RX was 0
  return;
}

/*--------------- S e r v i c e R x ( ) ---------------

PURPOSE
Call ServiceRx() to handle Rx interrupts and then call ServiceTx()
to handle Tx interrupts.

INPUT PARAMETERS
none
*/
void SerialISR(void)
{
  ServiceRx();
  
  ServiceTx();
}
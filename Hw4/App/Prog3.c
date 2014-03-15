/*--------------- P r o g 4 . c ---------------

by: David Tyler
    ECE Dept.
    UMASS Lowell

PURPOSE
Receive wireless sensor network packets from the RS232 port,
interpret and display the messages using uC/OS-III.

CHANGES
01-29-2013 dwt -  Created
*/

#include "includes.h"
#include "Intrpt.h"
#include "BfrPair.h"
#include "Payload.h"
#include "Error.h"
#include "PktParser.h"
#include "SerIODriver.h"
#include "assert.h"
/*----- c o n s t a n t    d e f i n i t i o n s -----*/

#define Init_STK_SIZE 128      // Init task Priority
#define Init_PRIO 2             // Init task Priority

// Define RS232 baud rate.
#define BaudRate 9600

/*----- G l o b a l    V a r i a b l e s -----*/

static  OS_TCB   initTCB;                         // Init task TCB
static  CPU_STK  initStk[Init_STK_SIZE];          // Space for Init task stack

/*----- f u n c t i o n    p r o t o t y p e s -----*/

static void Init(void *p_arg);
/*--------------- m a i n ( ) -----------------*/

CPU_INT32S main()
{
    OS_ERR osErr;
    
//  Initialize the STM32F107 eval. board.
    BSP_IntDisAll();           /* Disable all interrupts. */

    //Init uC
    OSInit(&osErr);                /* Initialize BSP functions */
    assert(osErr==OS_ERR_NONE);
    
    //Create init task
    OSTaskCreate(&initTCB,"Init Task",Init,NULL,Init_PRIO,&initStk[0],
                 Init_STK_SIZE/10,Init_STK_SIZE,0,0,0,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),&osErr);
    assert(osErr==OS_ERR_NONE);
    
    OSStart(&osErr);
    assert(osErr==OS_ERR_NONE);
}


/*--------------- I n i t ( ) ---------------*/

/*
PURPOSE
Perform O/S and application initialization.

INPUT PARAMETERS
data		-- pointer to task data (not used)

GLOBALS
*/
static void Init(void *p_arg)
{
    CPU_INT32U  cpu_clk_freq;                                     /* CPU Clock frequency */
    CPU_INT32U  cnts;                                             /* CPU clock interval */
    OS_ERR      err;                                              /* OS Error code */
    
    BSP_Init();                                                   /* Initialize BSP functions  */
    CPU_Init();                                                   /* Initialize the uC/CPU services */

    cpu_clk_freq = BSP_CPU_ClkFreq();                             /* Determine SysTick reference freq. */                                                                        
    cnts         = cpu_clk_freq / (CPU_INT32U)OSCfg_TickRate_Hz;  /* Determine nbr SysTick increments */
    OS_CPU_SysTickInit(cnts);                                     /* Init uC/OS periodic time src (SysTick). */

#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);                                 /* Compute CPU capacity with no task running */
#endif

    CPU_IntDisMeasMaxCurReset();

    // Initialize USART2.
    BSP_Ser_Init(BaudRate);

    // Initialize the serial I/O driver. 
    InitSerIO();    
    
    // Create and initialize the Payload Buffer Pair and the Reply Buffer
  // Pair.
    PayloadInit();
    
    // Delete the Init task.
    OSTaskDel(&initTCB, &err);
    assert(err == OS_ERR_NONE);
}
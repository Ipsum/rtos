/*=============== U C O S D e m o . c ===============*/

/*
BY:	George Cheney
		16.472 / 16.572 Embedded Real Time Systems
		Electrical and Computer Engineering Dept.
		UMASS Lowell
*/

/*
PURPOSE
Demonstrate Producer/Consumer task relationship.

DEMONSTRATES
Multitasking
Semaphores
Circular Buffer

CHANGES
03-10-2011  - Release to class.
*/


/*
NOTE: This example is derived from example code provided by Micrium whose copyright notice is below.

*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                             (c) Copyright 2009; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            EXAMPLE CODE
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                     Micrium uC-Eval-STM32F107
*                                        Evaluation Board
*
* Filename      : app.c
* Version       : V1.00
* Programmer(s) : JJL
                  EHS
*********************************************************************************************************
*/

#include <includes.h>
#include <assert.h>
#include "Producer.h"
#include "Consumer.h"
#include "SerIODriver.h"

/*------ c o n s t a n t    d e f i n i t i o n s -----*/

#define Init_STK_SIZE 128      // Init task Priority
#define Init_PRIO 2             // Init task Priority

// USART2 Settings
#define BaudRate 9600           // Baud Rate Setting

/*----- G l o b a l    V a r i a b l e s -----*/

static  OS_TCB   initTCB;                         // Init task TCB
static  CPU_STK  initStk[Init_STK_SIZE];          // Space for Init task stack

/*----- f u n c t i o n    p r o t o t y p e s -----*/

static  void  Init  (void *p_arg);

//--------------- m a i n ( ) ---------------

CPU_VOID  main (CPU_VOID)
{
  // OS Error Code
  OS_ERR  err;                          

  // Disable all interrupts.
  BSP_IntDisAll();                      

  // Init uC/OS-III.
  OSInit(&err);                         
  assert(err == OS_ERR_NONE);
  
  // Create the init task.
  OSTaskCreate(&initTCB,            // Task Control Block                
               "Init Task",         // Task name
               Init,                // Task entry point
               NULL,                // Address of optional task data block
               Init_PRIO,           // Task priority
               &initStk[0],         // Base address of task stack space
               Init_STK_SIZE / 10,  // Stack water mark limit
               Init_STK_SIZE,       // Task stack size
               0,                   // This task has no task queue
               0,                   // Number of clock ticks (defaults to 10)
               0,                   // Pointer to TCB extension
               (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),  // Task options
               &err);               // Address to return O/S error code

  /* Verify successful task creation. */
  assert(err == OS_ERR_NONE);

  // Start multitasking.
  OSStart(&err);                        
  assert(err == OS_ERR_NONE);
}


/*--------------- I n i t ( ) ---------------*/

/*
PURPOSE
Perform O/S and application initialization.

INPUT PARAMETERS
data		-- pointer to task data (not used)

GLOBALS
*/

static  void  Init (void *p_arg)
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
    
    // Create the Producer and Consumer tasks.
    CreateProducerTask();
    CreateConsumerTask();
    
    // Delete the Init task.
    OSTaskDel(&initTCB, &err);
    assert(err == OS_ERR_NONE);
}



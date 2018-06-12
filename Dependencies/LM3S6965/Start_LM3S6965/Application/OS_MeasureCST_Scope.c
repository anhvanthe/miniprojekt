/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*                        The Embedded Experts                        *
**********************************************************************
*                                                                    *
*       (c) 1995 - 2017 SEGGER Microcontroller GmbH & Co. KG         *
*                                                                    *
*       Internet: segger.com  Support: support_embos@segger.com      *
*                                                                    *
**********************************************************************
*                                                                    *
*       embOS * Real time operating system for microcontrollers      *
*                                                                    *
*       Please note:                                                 *
*                                                                    *
*       Knowledge of this file may under no circumstances            *
*       be used to write a similar product or a real-time            *
*       operating system for in-house use.                           *
*                                                                    *
*       Thank you for your fairness !                                *
*                                                                    *
**********************************************************************
*                                                                    *
*       OS version: 4.34.1                                           *
*                                                                    *
**********************************************************************

----------------------------------------------------------------------
File    : OS_MeasureCST_Scope.c
Purpose : embOS sample program allowing measurement of the embOS
          context switching time by using an oscilloscope. To do so,
          it sets and clears a port pin (as defined in BSP.c). The
          context switching time is

            Time = (d - c) - (b - a)

             -----   --                   ---------------
                  | |  |                 |
                   -    -----------------
                  ^ ^  ^                 ^
                  a b  c                 d

          The time between c and d is the context switching time, but
          note that the real context switching time is shorter, as the
          signal also contains the overhead of switching the LED on
          and off. The time of this overhead is visible on an
          oscilloscope as a small peak between a and b.
--------  END-OF-HEADER  ---------------------------------------------
*/

#include "RTOS.h"
#include "BSP.h"

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

static OS_STACKPTR int StackHP[128], StackLP[128];   /* Task stacks */
static OS_TASK         TCBHP, TCBLP;         /* Task-control-blocks */

/*********************************************************************
*
*       Local functions
*
**********************************************************************
*/

/*********************************************************************
*
*       HPTask()
*/
static void HPTask(void) {
  while (1) {
    OS_Suspend(NULL);  // Suspend high priority task
    BSP_ClrLED(0);     // Stop measurement
  }
}

/*********************************************************************
*
*       LPTask()
*/
static void LPTask(void) {
  while (1) {
    OS_Delay(100);      // Synchronize to tick to avoid jitter
    //
    // Display measurement overhead
    //
    BSP_SetLED(0);
    BSP_ClrLED(0);
    //
    // Perform measurement
    //
    BSP_SetLED(0);      // Start measurement
    OS_Resume(&TCBHP);  // Resume high priority task to force task switch
  }
}

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/

/*********************************************************************
*
*       main()
*/
int main(void) {
  OS_InitKern();                   /* Initialize OS                 */
  OS_InitHW();                     /* Initialize Hardware for OS    */
  BSP_Init();                      /* Initialize LED ports          */
  /* You need to create at least one task before calling OS_Start() */
  OS_CREATETASK(&TCBHP, "HP Task", HPTask, 100, StackHP);
  OS_CREATETASK(&TCBLP, "LP Task", LPTask,  50, StackLP);
  OS_Start();                      /* Start multitasking            */
  return 0;
}

/****** End Of File *************************************************/

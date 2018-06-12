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
File    : OS_ThreadLocalStorage.c
Purpose : embOS sample application to demonstrate the usage of TLS.
          TLS support is CPU and compiler specific and may not be
          implemented in all OS ports.
NOTE    : Usage of thread local storage from different tasks requires
          thread safe system libraries, because the TLS is located
          on the heap memory and is dynamically created during runtime.
          xmtx.c from the Setup folder has to be added to the project
          to support thread safe libraries.
--------  END-OF-HEADER  ---------------------------------------------
*/

#include "RTOS.h"
#include <errno.h>

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

static OS_STACKPTR int StackHP[128], StackLP[128], StackMP[128];  /* Task stacks */
static OS_TASK         TCBHP, TCBLP, TCBMP;               /* Task-control-blocks */

/*********************************************************************
*
*       Local functions
*
**********************************************************************
*/

/*********************************************************************
*
*       HPTask() with thread local storage
*/
static void HPTask(void) {
  //
  // Initialize TLS for this task
  //
  OS_ExtendTaskContext_TLS();
  while (errno != 0) {
    //
    // errno is local to this task, hence we should not arrive here
    //
  }
  //
  // Simulate a task specific error
  //
  errno = 3;
  while (1) {
    OS_Delay(10);
      while (errno != 3) {
        //
        // errno is local to this task, hence we should not arrive here
        //
      }
  }
}

/*********************************************************************
*
*       MPTask() with thread local storage
*/
static void MPTask(void) {
  //
  // Initialize TLS for this task
  //
  OS_ExtendTaskContext_TLS();
  while (errno != 0) {
    //
    // errno is local to this task, hence we should not arrive here
    //
  }
  //
  // Simulate a task specific error
  //
  errno = 2;
  while (1) {
    OS_Delay(10);
    while (errno != 2) {
        //
        // errno is local to this task, hence we should not arrive here
        //
    }
  }
}

/*********************************************************************
*
*       LPTask() without thread local storage
*/
static void LPTask(void) {
  while (errno != 1) {
    //
    // errno is not local to this task, hence we expect the global
    // value that was set in main() and should not arrive here
    //
  }
  while (1) {
    OS_Delay(50);
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
  errno = 1;                       /* Simulate an error             */
  OS_InitKern();                   /* Initialize OS                 */
  OS_InitHW();                     /* Initialize Hardware for OS    */
  /* You need to create at least one task before calling OS_Start() */
  OS_CREATETASK(&TCBHP, "HP Task", HPTask, 100, StackHP);
  OS_CREATETASK(&TCBMP, "MP Task", MPTask,  70, StackMP);
  OS_CREATETASK(&TCBLP, "LP Task", LPTask,  50, StackLP);
  OS_Start();                      /* Start multitasking            */
  return 0;
}

/****** End Of File *************************************************/

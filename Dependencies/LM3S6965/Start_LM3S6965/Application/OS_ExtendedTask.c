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
File    : OS_ExtendedTask.c
Purpose : embOS sample program demonstrating the extension of tasks.
          This sample application is described in the generic manual
          in chapter:
          "Extending the task context by using own task structures"
--------  END-OF-HEADER  ---------------------------------------------
*/

#include "RTOS.h"
#include <stdio.h>

/*********************************************************************
*
*       Types, local
*
**********************************************************************
*/

//
// Custom task structure with extended task context.
//
typedef struct {
  OS_TASK Task;     // OS_TASK has to be the first element
  OS_TIME Timeout;  // Any other data type may be used to extend the context
  char*   pString;  // Any number of elements may be used to extend the context
} MY_APP_TASK;

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

static OS_STACKPTR int StackHP[128], StackLP[128];   /* Task stacks */
static MY_APP_TASK     TCBHP, TCBLP;         /* Task-control-blocks */

/*********************************************************************
*
*       Local functions
*
**********************************************************************
*/

/*********************************************************************
*
*       MyTask()
*/
static void MyTask(void) {
  MY_APP_TASK* pThis;
  OS_TIME      Timeout;
  char*        pString;

  pThis = (MY_APP_TASK*)OS_GetTaskID();
  while (1) {
    Timeout = pThis->Timeout;
    pString = pThis->pString;
    printf(pString);
    OS_Delay(Timeout);
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
  /* You need to create at least one task before calling OS_Start() */
  //
  // Create the extended tasks just as normal tasks.
  // Note that the first parameter has to be of type OS_TASK
  //
  OS_CREATETASK(&TCBHP.Task, "HP Task", MyTask, 100, StackHP);
  OS_CREATETASK(&TCBLP.Task, "LP Task", MyTask,  50, StackLP);
  //
  // Give task contexts individual data
  //
  TCBHP.Timeout = 200;
  TCBHP.pString = "HP task running\n";
  TCBLP.Timeout = 500;
  TCBLP.pString = "LP task running\n";
  OS_Start();                      /* Start multitasking            */
  return 0;
}

/****** End Of File *************************************************/

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
File    : OS_ExtendTaskContext.c
Purpose : embOS sample program demonstrating the dynamic extension of
          tasks' contexts. This is done by adding a global variable to
          the task context of certain tasks.
--------  END-OF-HEADER  ---------------------------------------------
*/

#include "RTOS.h"

/*********************************************************************
*
*       Types, local
*
**********************************************************************
*/

//
// Custom structure with task context extension.
// In this case, the extended task context consists of just
// a single member, which is a global variable.
//
typedef struct {
  int GlobalVar;
} CONTEXT_EXTENSION;

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

static OS_STACKPTR int StackHP[128], StackLP[128];   /* Task stacks */
static OS_TASK         TCBHP, TCBLP;         /* Task-control-blocks */
static int             GlobalVar;

/*********************************************************************
*
*       Local functions
*
**********************************************************************
*/

/*********************************************************************
*
*       _Save()
*
* Function description
*   This function saves an extended task context.
*/
static void OS_STACKPTR* _Save(void OS_STACKPTR* pStack) {
  CONTEXT_EXTENSION* p;
  //
  // Create pointer to our structure
  //
  p = ((CONTEXT_EXTENSION*)pStack) - (1 - OS_STACK_AT_BOTTOM);
  //
  // Save all members of the structure
  //
  p->GlobalVar = GlobalVar;
  return (void OS_STACKPTR*)p;
}

/*********************************************************************
*
*       _Restore()
*
* Function description
*   This function restores an extended task context.
*/
static void OS_STACKPTR* _Restore(const void OS_STACKPTR* pStack) {
  const CONTEXT_EXTENSION* p;
  //
  // Create pointer to our structure
  //
  p = ((const CONTEXT_EXTENSION *)pStack) - (1 - OS_STACK_AT_BOTTOM);
  //
  // Restore all members of the structure
  //
  GlobalVar = p->GlobalVar;
  return (void OS_STACKPTR*)p;
}

/*********************************************************************
*
*       Public API structure
*/
const OS_EXTEND_TASK_CONTEXT _SaveRestore = {
  _Save,    // Function pointer to save the task context
  _Restore  // Function pointer to restore the task context
};

/*********************************************************************
*
*       HPTask()
*
* Function description
*   During the execution of this function, the thread-specific
*   global variable GlobalVar always has the same value of 1.
*/
static void HPTask(void) {
  OS_ExtendTaskContext(&_SaveRestore);
  GlobalVar = 1;
  while (1) {
    OS_Delay(10);
  }
}

/*********************************************************************
*
*       LPTask()
*
* Function description
*   During the execution of this function, the thread-specific
*   global variable GlobalVar always has the same value of 2.
*/
static void LPTask(void) {
  OS_ExtendTaskContext(&_SaveRestore);
  GlobalVar = 2;
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
  OS_InitKern();                   /* Initialize OS                 */
  OS_InitHW();                     /* Initialize Hardware for OS    */
  /* You need to create at least one task before calling OS_Start() */
  OS_CREATETASK(&TCBHP, "HP Task", HPTask, 100, StackHP);
  OS_CREATETASK(&TCBLP, "LP Task", LPTask,  50, StackLP);
  OS_Start();                      /* Start multitasking            */
  return 0;
}

/****** End Of File *************************************************/

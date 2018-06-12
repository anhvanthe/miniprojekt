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
File    : OS_EventObject.c
Purpose : embOS sample program demonstrating the usage of event objects.
--------  END-OF-HEADER  ---------------------------------------------
*/

#include "RTOS.h"

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static OS_STACKPTR int StackHP[128], StackLP[128], StackHW[128];  /* Task stacks */
static OS_TASK         TCBHP, TCBLP, TCBHW;               /* Task-control-blocks */
static OS_EVENT        HW_Event;

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
  //
  // Wait until HW module is set up
  //
  OS_EVENT_Wait(&HW_Event);
  while (1) {
    OS_Delay(50);
  }
}

/*********************************************************************
*
*       LPTask()
*/
static void LPTask(void) {
  //
  // Wait until HW module is set up
  //
  OS_EVENT_Wait(&HW_Event);
  while (1) {
    OS_Delay(200);
  }
}

/*********************************************************************
*
*       HWTask()
*/
static void HWTask(void) {
  //
  // Wait until HW module is set up
  //
  OS_Delay(100);
  //
  // Init done, send broadcast to waiting tasks
  //
  OS_EVENT_Set(&HW_Event);
  while (1) {
    OS_Delay(40);
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
  OS_CREATETASK(&TCBHW, "HWTask", HWTask, 25, StackHW);
  OS_EVENT_Create(&HW_Event);
  OS_Start();                      /* Start multitasking            */
  return 0;
}

/****** End Of File *************************************************/

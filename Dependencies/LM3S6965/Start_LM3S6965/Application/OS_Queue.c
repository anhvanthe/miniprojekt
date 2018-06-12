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
File    : OS_Queue.c
Purpose : embOS sample program demonstrating the usage of queues.
--------  END-OF-HEADER  ---------------------------------------------
*/

#include "RTOS.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define MESSAGE_ALIGNMENT    4u  // Depends on core/compiler
#define MESSAGES_SIZE_HELLO  (7u + OS_Q_SIZEOF_HEADER + MESSAGE_ALIGNMENT)
#define MESSAGES_SIZE_WORLD  (9u + OS_Q_SIZEOF_HEADER + MESSAGE_ALIGNMENT)
#define QUEUE_SIZE           (MESSAGES_SIZE_HELLO + MESSAGES_SIZE_WORLD)

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static OS_STACKPTR int StackHP[128], StackLP[128];   /* Task stacks */
static OS_TASK         TCBHP, TCBLP;         /* Task-control-blocks */
static OS_Q            MyQueue;
static char            MyQBuffer[QUEUE_SIZE];

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
  char* pData;
  int   Len;

  while (1) {
    Len = OS_Q_GetPtr(&MyQueue, (void**)&pData);
    OS_Delay(10);
    //
    // Evaluate Message
    //
    if (Len) {
      OS_SendString(pData);
      OS_Q_Purge(&MyQueue);
    }
  }
}

/*********************************************************************
*
*       LPTask()
*/
static void LPTask(void) {
  while (1) {
    OS_Q_Put(&MyQueue, "\nHello", 7);
    OS_Q_Put(&MyQueue, "\nWorld !", 9);
    OS_Delay(500);
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
  OS_InitKern();                /* Initialize OS                    */
  OS_InitHW();                  /* Initialize Hardware for OS       */
  OS_Q_Create(&MyQueue, &MyQBuffer, sizeof(MyQBuffer));
  /* You need to create at least one task before calling OS_Start() */
  OS_CREATETASK(&TCBHP, "HP Task", HPTask, 100, StackHP);
  OS_CREATETASK(&TCBLP, "LP Task", LPTask,  50, StackLP);
  OS_SendString("embOS OS_Queue example");
  OS_SendString("\n\nDemonstrating message passing\n");
  OS_Start();                   /* Start multitasking               */
  return 0;
}

/****** End Of File *************************************************/

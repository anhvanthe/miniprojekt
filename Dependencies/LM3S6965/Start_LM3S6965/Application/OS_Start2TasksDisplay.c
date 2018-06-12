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
File    : OS_Start2TasksDisplay.c
Purpose : embOS sample program running two simple tasks on Luminary Ekx-LM3S6965.
--------  END-OF-HEADER  ---------------------------------------------
*/

#include "RTOS.h"
#include "osram128x64x4.h"

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

static OS_STACKPTR int StackHP[256], StackLP[256];   /* Task stacks */
static OS_TASK         TCBHP, TCBLP;         /* Task-control-blocks */
static OS_RSEMA        _SemaDisplay;
static OS_U8           _IsInited;

/*********************************************************************
*
*       Local functions
*
**********************************************************************
*/

/*********************************************************************
*
*  The following code would most likely be placed in a separate file
*  in a real application.
*/
static void _InitIfRequired(void) {
  if (_IsInited == 0) {
    OS_CREATERSEMA(&_SemaDisplay);
    OSRAM128x64x4Init(1000000);
    OSRAM128x64x4StringDraw("embOS on LM3S6965", 16, 20, 11);
    OSRAM128x64x4StringDraw("www.segger.com"   , 26, 30, 11);
  }
  _IsInited = 1;
}

static void _Lock(void) {
  _InitIfRequired();         // Perform automatic initialisation so that explicit call to _Init is not required
  OS_Use(&_SemaDisplay);
}

static void _Unlock(void) {
  _InitIfRequired();         // Perform automatic initialisation so that explicit call to _Init is not required
  OS_Unuse(&_SemaDisplay);
}

void DISPLAY_String(const char*, unsigned int, unsigned int);  // Avoid "No Prototype" warning
void DISPLAY_String(const char* sText, unsigned int vPos, unsigned int hPos) {
  _Lock();
  OSRAM128x64x4StringDraw(sText, vPos, hPos, 11);
  _Unlock();
}

void DISPLAY_Init(void);  // Avoid "No Prototype" warning
void DISPLAY_Init(void) {
  _InitIfRequired();
}

/*********************************************************************
*
*       HPTask()
*/
static void HPTask(void) {
  while (1) {
    DISPLAY_String("HP Task", 0, 6);
    OS_Delay(50);
    DISPLAY_String("                     ", 0, 6);
    OS_Delay(50);
  }
}

/*********************************************************************
*
*       LPTask()
*/
static void LPTask(void) {
  while (1) {
    DISPLAY_String("LP Task", 0, 44);
    OS_Delay(200);
    DISPLAY_String("                     ", 0, 44);
    OS_Delay(200);
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
  OS_IncDI();                      /* Initially disable interrupts  */
  OS_InitKern();                   /* Initialize OS                 */
  OS_InitHW();                     /* Initialize Hardware for OS    */
  DISPLAY_Init();                  /* Initialize LM3S6965 display   */
  /* You need to create at least one task before calling OS_Start() */
  OS_CREATETASK(&TCBHP, "HP Task", HPTask, 100, StackHP);
  OS_CREATETASK(&TCBLP, "LP Task", LPTask,  50, StackLP);
  OS_Start();                      /* Start multitasking            */
  return 0;
}

/****** End Of File *************************************************/

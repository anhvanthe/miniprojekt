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
File    : OS_SWTimer.c
Purpose : embOS sample program running two simple software timer.
--------  END-OF-HEADER  ---------------------------------------------
*/

#include "RTOS.h"
#include "BSP.h"

static OS_TIMER TIMER50, TIMER200;

static void Timer50(void) {
  BSP_ToggleLED(0);
  OS_RetriggerTimer(&TIMER50);
}

static void Timer200(void) {
  BSP_ToggleLED(1);
  OS_RetriggerTimer(&TIMER200);
}

/*********************************************************************
*
*       main()
*/
int main(void) {
  OS_InitKern();                   /* Initialize OS                 */
  OS_InitHW();                     /* Initialize Hardware for OS    */
  BSP_Init();                      /* Initialize LED ports          */
  OS_CREATETIMER(&TIMER50,  Timer50,   50);
  OS_CREATETIMER(&TIMER200, Timer200, 200);
  OS_Start();                      /* Start multitasking            */
  return 0;
}

/****** End Of File *************************************************/

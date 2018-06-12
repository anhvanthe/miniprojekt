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
File    : exceptions.h
Purpose : CMSIS compatible definition of exception handler for LM3S6965
          To be included in all sources which implement
          or reference exception/irq handler
--------  END-OF-HEADER  ---------------------------------------------
*/

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#ifdef __cplusplus
  extern "C" {
#endif

/* Define WEAK attribute                                            */
/* All default exception handlers are implemented weak,             */
/* so they can be replaced by the application.                      */
#if defined   ( __CC_ARM   )
  #define WEAK __attribute__ ((weak))
#elif defined ( __ICCARM__ )
  #define WEAK __weak
#elif defined (  __GNUC__  )
  #define WEAK __attribute__ ((weak))
#endif

/********************************************************************/

/****** Undefined (not implemented) interrupts **********************/

extern WEAK void IrqHandlerNotUsed(void);

/****** CM3 internal exceptions *************************************/

extern WEAK void NMI_Handler(void);
extern WEAK void HardFault_Handler(void);
extern WEAK void MemManage_Handler(void);
extern WEAK void BusFault_Handler(void);
extern WEAK void UsageFault_Handler(void);
extern WEAK void SVC_Handler(void);
extern WEAK void DebugMon_Handler(void);
extern WEAK void PendSV_Handler(void);
extern WEAK void SysTick_Handler(void);

/****** Controller specific peripheral interrupts *******************/

extern WEAK void GPIOPortA_IRQHandler(void);
extern WEAK void GPIOPortB_IRQHandler(void);
extern WEAK void GPIOPortC_IRQHandler(void);
extern WEAK void GPIOPortD_IRQHandler(void);
extern WEAK void GPIOPortE_IRQHandler(void);
extern WEAK void UART0_IRQHandler(void);
extern WEAK void UART1_IRQHandler(void);
extern WEAK void SSI0_IRQHandler(void);
extern WEAK void I2C0_IRQHandler(void);
extern WEAK void PWMFault_IRQHandler(void);
extern WEAK void PWMGen0_IRQHandler(void);
extern WEAK void PWMGen1_IRQHandler(void);
extern WEAK void PWMGen2_IRQHandler(void);
extern WEAK void QEI0_IRQHandler(void);
extern WEAK void ADCSeq0_IRQHandler(void);
extern WEAK void ADCSeq1_IRQHandler(void);
extern WEAK void ADCSeq2_IRQHandler(void);
extern WEAK void ADCSeq3_IRQHandler(void);
extern WEAK void Watchdog_IRQHandler(void);
extern WEAK void Timer0A_IRQHandler(void);
extern WEAK void Timer0B_IRQHandler(void);
extern WEAK void Timer1A_IRQHandler(void);
extern WEAK void Timer1B_IRQHandler(void);
extern WEAK void Timer2A_IRQHandler(void);
extern WEAK void Timer2B_IRQHandler(void);
extern WEAK void Comp0_IRQHandler(void);
extern WEAK void Comp1_IRQHandler(void);
extern WEAK void SysCtrl_IRQHandler(void);
extern WEAK void FlashCtrl_IRQHandler(void);
extern WEAK void GPIOPortF_IRQHandler(void);
extern WEAK void GPIOPortG_IRQHandler(void);
extern WEAK void USART2_IRQHandler(void);
extern WEAK void Timer3A_IRQHandler(void);
extern WEAK void Timer3B_IRQHandler(void);
extern WEAK void I2C1_IRQHandler(void);
extern WEAK void QEI1_IRQHandler(void);
extern WEAK void Ethernet_IRQHandler(void);
extern WEAK void Hibernate_IRQHandler(void);

#ifdef __cplusplus
  }
#endif

/********************************************************************/

#endif                                  /* Avoid multiple inclusion */

/*************************** End of file ****************************/

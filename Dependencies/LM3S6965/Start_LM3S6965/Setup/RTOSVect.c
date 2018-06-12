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
File    : RTOSVect.c
Purpose : Vector table for LM3S6965, CMSIS compatible
--------  END-OF-HEADER  ---------------------------------------------
*/

#include "exceptions.h"   // CMSIS compatible exception handler definition
#include "RTOS.h"

#ifdef __cplusplus
  extern "C" {
#endif

/*********************************************************************
*
*       Reference to other modules
*
**********************************************************************
*/
#ifdef __ICCARM__
  #pragma language=extended
  #if (__VER__ < 500)
    #pragma segment="CSTACK"
    extern void __program_start(void);
  #else
    #pragma section="CSTACK"
    extern void __iar_program_start(void);
  #endif  // #if (__VER__ < 500)
#endif    // #ifdef __ICCARM__

#ifdef __CC_ARM
  extern unsigned int Image$$CSTACK$$ZI$$Limit;
  extern void __main(void);
#endif

/* Define WEAK attribute */
#if defined   ( __CC_ARM   )
    #define WEAK __attribute__ ((weak))
#elif defined ( __ICCARM__ )
    #define WEAK __weak
#elif defined (  __GNUC__  )
    #define WEAK __attribute__ ((weak))
#endif

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define NVIC_HFSR          *((volatile unsigned long *)(0xE000ED2CuL))

/*********************************************************************
*
*       Local functions
*
**********************************************************************
*/

/*********************************************************************
*
*       _IllegalException()
*
*       Is called from any exception handler which is not implemented
*       by the application (not overwriiten by user).
*       We implemented an endless loop here, so the programm stops
*       when any of the exceptions is called.
*       Using a debugger and setting a breakpoint here allows to
*       analyze which exception / interrupt was called.
*       Normally, the CPU should not arrive here.
*/
static void _IllegalException(void) {
  while(1) {  // Unhandled exception
  }
}

/*********************************************************************
*
*       Dummy system exception handler
*
*       We implemented a call to _IllegalException(), so the program
*       calls this function when any of the exceptions is called.
*       Using a debugger and setting a breakpoint in _IllegelException()
*       allows to analyze which exception / interrupt was called.
*/
WEAK void NMI_Handler         (void) { _IllegalException(); }
WEAK void MemManage_Handler   (void) { _IllegalException(); }
WEAK void BusFault_Handler    (void) { _IllegalException(); }
WEAK void UsageFault_Handler  (void) { _IllegalException(); }
WEAK void SVC_Handler         (void) { _IllegalException(); }
WEAK void DebugMon_Handler    (void) { _IllegalException(); }
WEAK void PendSV_Handler      (void) { _IllegalException(); }
WEAK void SysTick_Handler     (void) { _IllegalException(); }
WEAK void HardFault_Handler   (void) { _IllegalException(); }

/*********************************************************************
*
*       Default dummy interrupt handler
*
*       We implemented a call to _IllegalException(), so the program
*       calls this function when any of the exceptions is called.
*       Using a debugger and setting a breakpoint in _IllegelException()
*       allows to analyze the call stack and shows
*       which exception / interrupt was called.
*/
WEAK void GPIOPortA_IRQHandler  (void) { _IllegalException(); }
WEAK void GPIOPortB_IRQHandler  (void) { _IllegalException(); }
WEAK void GPIOPortC_IRQHandler  (void) { _IllegalException(); }
WEAK void GPIOPortD_IRQHandler  (void) { _IllegalException(); }
WEAK void GPIOPortE_IRQHandler  (void) { _IllegalException(); }
WEAK void UART0_IRQHandler      (void) { _IllegalException(); }
WEAK void UART1_IRQHandler      (void) { _IllegalException(); }
WEAK void SSI0_IRQHandler       (void) { _IllegalException(); }
WEAK void I2C0_IRQHandler       (void) { _IllegalException(); }
WEAK void PWMFault_IRQHandler   (void) { _IllegalException(); }
WEAK void PWMGen0_IRQHandler    (void) { _IllegalException(); }
WEAK void PWMGen1_IRQHandler    (void) { _IllegalException(); }
WEAK void PWMGen2_IRQHandler    (void) { _IllegalException(); }
WEAK void QEI0_IRQHandler       (void) { _IllegalException(); }
WEAK void ADCSeq0_IRQHandler    (void) { _IllegalException(); }
WEAK void ADCSeq1_IRQHandler    (void) { _IllegalException(); }
WEAK void ADCSeq2_IRQHandler    (void) { _IllegalException(); }
WEAK void ADCSeq3_IRQHandler    (void) { _IllegalException(); }
WEAK void Watchdog_IRQHandler   (void) { _IllegalException(); }
WEAK void Timer0A_IRQHandler    (void) { _IllegalException(); }
WEAK void Timer0B_IRQHandler    (void) { _IllegalException(); }
WEAK void Timer1A_IRQHandler    (void) { _IllegalException(); }
WEAK void Timer1B_IRQHandler    (void) { _IllegalException(); }
WEAK void Timer2A_IRQHandler    (void) { _IllegalException(); }
WEAK void Timer2B_IRQHandler    (void) { _IllegalException(); }
WEAK void Comp0_IRQHandler      (void) { _IllegalException(); }
WEAK void Comp1_IRQHandler      (void) { _IllegalException(); }

WEAK void SysCtrl_IRQHandler    (void) { _IllegalException(); }
WEAK void FlashCtrl_IRQHandler  (void) { _IllegalException(); }
WEAK void GPIOPortF_IRQHandler  (void) { _IllegalException(); }
WEAK void GPIOPortG_IRQHandler  (void) { _IllegalException(); }

WEAK void USART2_IRQHandler     (void) { _IllegalException(); }

WEAK void Timer3A_IRQHandler    (void) { _IllegalException(); }
WEAK void Timer3B_IRQHandler    (void) { _IllegalException(); }
WEAK void I2C1_IRQHandler       (void) { _IllegalException(); }
WEAK void QEI1_IRQHandler       (void) { _IllegalException(); }
WEAK void Ethernet_IRQHandler   (void) { _IllegalException(); }
WEAK void Hibernate_IRQHandler  (void) { _IllegalException(); }
WEAK void IrqHandlerNotUsed     (void) { _IllegalException(); }

/*********************************************************************
*
*       Global code
*
**********************************************************************
*/

/*********************************************************************
*
*       Exception vector table
*/
#ifdef __ICCARM__
  #if (__VER__ < 500)
    #pragma location = "INTVEC"
  #else
    #pragma location = ".intvec"
  #endif  // #if (__VER__ < 500)
#endif    // #ifdef __ICCARM__
#ifdef __CC_ARM
  #pragma arm section rodata = "INTVEC"
#endif

/****** LM3S6965 Table entries ******************************/

const intvec_elem __vector_table[] =
{
#ifdef __ICCARM__
  { .__ptr = __sfe( "CSTACK" ) },    // Initial stack pointer value
#if (__VER__ < 500)
  __program_start,                   // Startup code entry point
#else
  __iar_program_start,               // Startup code entry point
#endif  // #if (__VER__ >= 500)
#endif  // #ifdef __ICCARM__
#ifdef __CC_ARM
  (intfunc) &Image$$CSTACK$$ZI$$Limit,  // Initial stack pointer
  __main,                               // Startup code entry point
#endif

  /**** System exceptions *****************/

  NMI_Handler,
  HardFault_Handler,
  MemManage_Handler,
  BusFault_Handler,
  UsageFault_Handler,
  0,                          // Reserved
  0,                          // Reserved
  0,                          // Reserved
  0,                          // Reserved
  SVC_Handler,
  DebugMon_Handler,
  0,                          // Reserved
  PendSV_Handler,             // The OS scheduler
  SysTick_Handler,            // The OS timer

  /**** Peripheral exceptions / interrupts */

  GPIOPortA_IRQHandler,
  GPIOPortB_IRQHandler,
  GPIOPortC_IRQHandler,
  GPIOPortD_IRQHandler,
  GPIOPortE_IRQHandler,
  UART0_IRQHandler,
  UART1_IRQHandler,
  SSI0_IRQHandler,
  I2C0_IRQHandler,
  PWMFault_IRQHandler,
  PWMGen0_IRQHandler,
  PWMGen1_IRQHandler,
  PWMGen2_IRQHandler,
  QEI0_IRQHandler,
  ADCSeq0_IRQHandler,
  ADCSeq1_IRQHandler,
  ADCSeq2_IRQHandler,
  ADCSeq3_IRQHandler,
  Watchdog_IRQHandler,
  Timer0A_IRQHandler,
  Timer0B_IRQHandler,
  Timer1A_IRQHandler,
  Timer1B_IRQHandler,
  Timer2A_IRQHandler,
  Timer2B_IRQHandler,
  Comp0_IRQHandler,
  Comp1_IRQHandler,
  IrqHandlerNotUsed,
  SysCtrl_IRQHandler,
  FlashCtrl_IRQHandler,
  GPIOPortF_IRQHandler,
  GPIOPortG_IRQHandler,
  IrqHandlerNotUsed,
  USART2_IRQHandler,
  IrqHandlerNotUsed,
  Timer3A_IRQHandler,
  Timer3B_IRQHandler,
  I2C1_IRQHandler,
  QEI1_IRQHandler,
  IrqHandlerNotUsed,
  IrqHandlerNotUsed,
  IrqHandlerNotUsed,
  Ethernet_IRQHandler,
  Hibernate_IRQHandler,
  IrqHandlerNotUsed,
  IrqHandlerNotUsed,
  IrqHandlerNotUsed,
  IrqHandlerNotUsed,
  IrqHandlerNotUsed,
  IrqHandlerNotUsed,
  IrqHandlerNotUsed,
  IrqHandlerNotUsed,
  IrqHandlerNotUsed,
  IrqHandlerNotUsed,
  IrqHandlerNotUsed
};

#ifdef __cplusplus
  }
#endif

/****** End of file *************************************************/

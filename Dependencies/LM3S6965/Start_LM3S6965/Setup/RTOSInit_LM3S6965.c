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
File    : RTOSInit_LM3S6965.c for Luminary LM3S6965

Purpose : Initializes and handles the hardware for embOS as far
          as required by embOS
          Feel free to modify this file acc. to your target system.
--------  END-OF-HEADER  ---------------------------------------------
*/

#include "RTOS.h"
#include "SEGGER_SYSVIEW.h"
#ifdef __ICCARM__  // IAR
#include "exceptions.h"           /* CMSIS compatible irq interface */
#endif

/*********************************************************************
*
*       Configuration
*
**********************************************************************
*/

/*********************************************************************
*
*       Clock frequency settings (configuration)
*/
#ifndef   OS_FSYS                   /* CPU Main clock frequency     */
  #define OS_FSYS 8000000uL
#endif

#ifndef   OS_PCLK_TIMER             /* Peripheral clock for timer   */
  #define OS_PCLK_TIMER (OS_FSYS)   /* May vary from CPU clock      */
#endif                              /* depending on CPU             */

#ifndef   OS_PCLK_UART              /* Peripheral clock for UART    */
  #define OS_PCLK_UART (OS_FSYS)    /* May vary from CPU clock      */
#endif                              /* depending on CPU             */

#ifndef   OS_TICK_FREQ
  #define OS_TICK_FREQ (1000u)
#endif

#ifndef   OS_USE_VARINTTABLE        /* The interrupt vector table   */
  #define OS_USE_VARINTTABLE (0)    /* may be located in RAM        */
#endif

#ifdef __ICCARM__  // IAR
#define OS_Systick SysTick_Handler  /* Map OS systick handler to CMSIS compatible handler name */
#endif

/*********************************************************************
*
*       Configuration of communication to embOSView
*/
#ifndef   OS_VIEW_IFSELECT
  #define OS_VIEW_IFSELECT  OS_VIEW_IF_JLINK
#endif

/*********************************************************************
*
*       UART settings for embOSView
*       If you do not want (or can not due to hardware limitations)
*       to dedicate a UART to OSView, please define it to be -1
*/
#if (OS_VIEW_IFSELECT == OS_VIEW_IF_UART)
  #ifndef   OS_UART
    #define OS_UART (-1)
  #endif

  #ifndef   OS_BAUDRATE
    #define OS_BAUDRATE (38400)
  #endif
#endif

/*********************************************************************
*
*       Vector table
*/
#if (defined __SES_ARM)           // SEGGER Embedded Studio
  extern int _vectors;
  #define __Vectors    _vectors
#elif (defined __CROSSWORKS_ARM)  // Rowley CrossStudio
  extern int _vectors;
  #define __Vectors    _vectors
#elif (defined __ICCARM__)        // IAR
  #define __Vectors    __vector_table
#elif (defined __GNUC__)          // GCC
  extern unsigned char __Vectors;
#elif (defined __CC_ARM)          // KEIL
  extern unsigned char __Vectors;
#endif

/*********************************************************************
*
*       Local defines (sfrs and addresses used in RTOSInit.c)
*
**********************************************************************
*/
#define NVIC_VTOR         (*(volatile OS_U32*) (0xE000ED08uL))

#define SYS_TICK_BASE_ADDR       (0xE000E010uL)
#define SYS_TICK_CONTROL         (*(volatile OS_U32*)(SYS_TICK_BASE_ADDR + 0x00))
#define SYS_TICK_RELOAD          (*(volatile OS_U32*)(SYS_TICK_BASE_ADDR + 0x04))
#define SYS_TICK_VALUE           (*(volatile OS_U32*)(SYS_TICK_BASE_ADDR + 0x08))
#define SYS_TICK_CALIBRATION     (*(volatile OS_U32*)(SYS_TICK_BASE_ADDR + 0x0C))

#define SYS_TICK_ENABLE_BIT      (0)
#define SYS_TICK_INT_ENABLE_BIT  (1)
#define SYS_TICK_CLK_SOURCE_BIT  (2)

#define SYS_INT_CTRL_STATE       (*(volatile OS_U32*)(0xE000ED04uL))
#define SYS_PENDSTSET            (26)

#define SYSCTRL_BASE_ADDR        (0x400FE000uL)
#define SYSCTRL_LDOPCTL          (*(volatile OS_U32*) (SYSCTRL_BASE_ADDR + 0x34))  // LDO power control register
#define SYSCTRL_RIS              (*(volatile OS_U32*) (SYSCTRL_BASE_ADDR + 0x50))  // Raw interrupt status register
#define SYSCTRL_MISC             (*(volatile OS_U32*) (SYSCTRL_BASE_ADDR + 0x58))  // Interrupt status register
#define SYSCTRL_RCC              (*(volatile OS_U32*) (SYSCTRL_BASE_ADDR + 0x60))  // Run-mode clock config register
#define SYSCTRL_RCGC0            (*(volatile OS_U32*) (SYSCTRL_BASE_ADDR + 0x100))
#define SYSCTRL_RCGC1            (*(volatile OS_U32*) (SYSCTRL_BASE_ADDR + 0x104))
#define SYSCTRL_RCGC2            (*(volatile OS_U32*) (SYSCTRL_BASE_ADDR + 0x108))

#define SYSCTRL_RCC_BYPASS       (0x00000800uL)  // PLL bypass
#define SYSCTRL_RCC_USE_SYSDIV   (0x00400000uL)  // Use sytem clock divider
#define SYSCTRL_RCC_IOSCDIS      (0x00000002uL)  // Internal oscillator disable
#define SYSCTRL_RCC_MOSCDIS      (0x00000001uL)  // Main oscillator disable
#define SYSCTRL_RCC_XTAL_MASK    (0x000003C0uL)  // Crystal attached to main osc
#define SYSCTRL_RCC_OSCSRC_MASK  (0x00000030uL)  // Oscillator input select
#define SYSCTRL_RCC_PWRDN        (0x00002000uL)  // PLL power down
#define SYSCTRL_RCC_OE           (0x00001000uL)  // PLL output enable
#define SYSCTRL_INT_PLL_LOCK     (0x00000040uL)  // PLL lock interrupt
#define SYSCTRL_RCC_SYSDIV_MASK  (0x07800000uL)  // System clock divider

#define SYSPRI1_ADDR             (0xE000ED18uL)
#define SYSHND_CTRL_ADDR         (0xE000ED24uL)    // System Handler Control and State
#define SYSHND_CTRL              (*(volatile OS_U32*) (SYSHND_CTRL_ADDR))

#define WDT_BASE_ADDR            (0x40000000uL)
#define WDTCTL                   (*(volatile OS_U32*) (WDT_BASE_ADDR + 0x008))
#define WDTLOCK                  (*(volatile OS_U32*) (WDT_BASE_ADDR + 0xC00))

#define NVIC_SYS_HND_CTRL_MEM    (0x00010000uL)  // Mem manage fault enable
#define NVIC_SYS_HND_CTRL_BUS    (0x00020000uL)  // Bus fault enable
#define NVIC_SYS_HND_CTRL_USAGE  (0x00040000uL)  // Usage fault enable

#define NVIC_BASE_ADDR           (0xE000E000uL)

#define NVIC_PRIOBASE_ADDR       (0xE000E400uL)
#define NVIC_ENABLE_ADDR         (0xE000E100uL)
#define NVIC_DISABLE_ADDR        (0xE000E180uL)
#define NVIC_VTOREG_ADDR         (0xE000ED08uL)

#define NUM_INTERRUPTS           (16+55)

/*********************************************************************
*
* The following can be used as as arguments for the PLL activation
* if required in __low_level_init()
*
**********************************************************************
*/
#define SYSCTRL_SYSDIV_1         (0x07800000uL)  // Processor clock is osc/pll /1
#define SYSCTRL_SYSDIV_4         (0x01C00000uL)  // Processor clock is osc/pll /4
#define SYSCTRL_SYSDIV_10        (0x04C00000uL)  // Processor clock is osc/pll /10
#define SYSCTRL_USE_PLL          (0x00000000uL)  // System clock is the PLL clock
#define SYSCTRL_USE_OSC          (0x00003800uL)  // System clock is the osc clock
#define SYSCTRL_XTAL_6MHZ        (0x000002C0uL)  // External crystal is 6MHz
#define SYSCTRL_XTAL_8MHZ        (0x00000380uL)  // External crystal is 8MHz
#define SYSCTRL_OSC_MAIN         (0x00000000uL)  // Oscillator source is main osc

/*****  Interrupt ID numbers ****************************************/
#define ISR_ID_NMI               (2)             // NMI fault
#define ISR_ID_HARD              (3)             // Hard fault
#define ISR_ID_MPU               (4)             // MPU fault
#define ISR_ID_BUS               (5)             // Bus fault
#define ISR_ID_USAGE             (6)             // Usage fault
#define ISR_ID_SVCALL            (11)            // SVCall
#define ISR_ID_DEBUG             (12)            // Debug monitor
#define ISR_ID_PENDSV            (14)            // PendSV
#define ISR_ID_SYSTICK           (15)            // System Tick

#define OS_ISR_ID_TICK           ISR_ID_SYSTICK  // We use Sys-Timer

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

#if (OS_VIEW_IFSELECT == OS_VIEW_IF_JLINK)
  #include "JLINKMEM.h"
  const OS_U32 OS_JLINKMEM_BufferSize = 32u;  // Size of the communication buffer for JLINKMEM
#else
  const OS_U32 OS_JLINKMEM_BufferSize = 0;    // Communication not used
#endif

/*********************************************************************
*
*       Local functions
*
**********************************************************************
*/

/*********************************************************************
*
*       _OS_GetHWTimerCycles()
*
* Function description
*   Returns the current hardware timer count value
*
* Return value
*   Current timer count value
*/
static unsigned int _OS_GetHWTimerCycles(void) {
  return SYS_TICK_VALUE;
}

/*********************************************************************
*
*       _OS_GetHWTimer_IntPending()
*
* Function description
*   Returns if the hardware timer interrupt pending flag is set
*
* Return value
*   == 0; Interrupt pending flag not set
*   != 0: Interrupt pending flag set
*/
static unsigned int _OS_GetHWTimer_IntPending(void) {
  return SYS_INT_CTRL_STATE & (1uL << SYS_PENDSTSET);
}

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/

/*********************************************************************
*
*       OS_Systick()
*
* Function description
*   This is the code that gets called when the processor receives a
*   _SysTick exception. SysTick is used as OS timer tick.
*
* NOTES:
*   (1) It has to be inserted in the interrupt vector table, if RAM
*       vectors are not used. Therefore it is declared public
*/
#ifdef __cplusplus
extern "C" {
#endif
void OS_Systick(void);
#ifdef __cplusplus
}
#endif
void OS_Systick(void) {
  OS_EnterNestableInterrupt();
  OS_TICK_Handle();
  #if (OS_VIEW_IFSELECT == OS_VIEW_IF_JLINK)
    JLINKMEM_Process();
  #endif
  OS_LeaveNestableInterrupt();
}

/*********************************************************************
*
*       OS_InitHW()
*
*       Initialize the hardware (timer) required for embOS to run.
*       May be modified, if an other timer should be used
*/
#define OS_TIMER_RELOAD (OS_PCLK_TIMER / OS_TICK_FREQ)
void OS_InitHW(void) {
  OS_U8 TickPrio;

  //
  // Structure with information about timer frequency, tick frequency, etc.
  // for micro second precise system time.
  //
  OS_SYSTIMER_CONFIG SysTimerConfig = {OS_PCLK_TIMER, OS_TICK_FREQ, 0, _OS_GetHWTimerCycles, _OS_GetHWTimer_IntPending};

  OS_IncDI();
  //
  // Initialize OS timer, clock soure = core clock
  //
  SYS_TICK_RELOAD  = OS_TIMER_RELOAD;
  SYS_TICK_CONTROL = (1uL << SYS_TICK_ENABLE_BIT) | (1uL << SYS_TICK_CLK_SOURCE_BIT);
  //
  // Install Systick Timer Handler and enable timer interrupt
  //
  OS_ARM_InstallISRHandler(OS_ISR_ID_TICK, (OS_ISR_HANDLER*)OS_Systick);
  //
  // Initialize NVIC vector base address. Might be necessary for RAM targets or application not running from 0
  //
  NVIC_VTOR = (OS_U32)&__Vectors;
  //
  // Set the interrupt priority for the system timer to 2nd lowest level to ensure the timer can preempt PendSV handler
  //
  OS_ARM_ISRSetPrio(OS_ISR_ID_TICK, 0xFFu);              // Set to lowest level, ALL BITS set
  TickPrio  = OS_ARM_ISRSetPrio(OS_ISR_ID_TICK, 0xFFu);  // Read priority back to examine relevant preemption-level-bits
  TickPrio -= 1;                                         // Set to low preemption level, 1 level higher than lowest
  OS_ARM_ISRSetPrio(OS_ISR_ID_TICK, TickPrio);
  OS_ARM_EnableISR(OS_ISR_ID_TICK);
  //
  // Setup values for usec precise system time functions
  //
  OS_Config_SysTimer(&SysTimerConfig);
  //
  // Configure and initialize SEGGER SystemView
  //
#if OS_PROFILE
  SEGGER_SYSVIEW_Conf();
#endif
  //
  // Initialize the optional communication for embOSView
  //
#if (OS_VIEW_IFSELECT != OS_VIEW_DISABLED)
  OS_COM_Init();
#endif
  OS_DecRI();
}

/*********************************************************************
*
*       OS_Idle()
*
*       Please note:
*       This is basically the "core" of the idle loop.
*       This core loop can be changed, but:
*       The idle loop does not have a stack of its own, therefore no
*       functionality should be implemented that relies on the stack
*       to be preserved. However, a simple program loop can be programmed
*       (like toggling an output or incrementing a counter)
*/
void OS_Idle(void) {     // Idle loop: No task is ready to execute
  while (1) {
    #if ((OS_VIEW_IFSELECT != OS_VIEW_IF_JLINK) && (OS_DEBUG == 0))     // Enter CPU halt mode when not in DEBUG build and J-Link communication not used
      #ifdef __ICCARM__  // IAR
        __asm volatile(" wfi");
      #endif
      #ifdef __CC_ARM    // KEIL
        __wfi();
      #endif
      #ifdef __GNUC__    // GCC
        __asm(" wfi");
      #endif
    #endif
  }
}

/*********************************************************************
*
*       OS_GetTime_Cycles()
*
*       This routine is required for task-info via embOSView or high
*       resolution time measurement functions.
*       It returns the system time in timer clock cycles.
*/
OS_U32 OS_GetTime_Cycles(void) {
  OS_U32 Time;
  OS_U32 Cnt;

  Time = OS_GetTime32();
  Cnt  = (OS_TIMER_RELOAD) - SYS_TICK_VALUE;
  //
  // Check if timer interrupt pending ...
  //
  if (SYS_INT_CTRL_STATE & (1uL << SYS_PENDSTSET)) {
    Cnt = OS_TIMER_RELOAD - SYS_TICK_VALUE;          // Interrupt pending, re-read timer and adjust result
    Time++;
  }
  return (OS_TIMER_RELOAD * Time) + Cnt;
}

/*********************************************************************
*
*       OS_ConvertCycles2us()
*
*       Convert Cycles into micro seconds.
*
*       If your clock frequency is not a multiple of 1 MHz,
*       you may have to modify this routine in order to get proper
*       diagnostics.
*
*       This routine is required for profiling or high resolution time
*       measurement only. It does not affect operation of the OS.
*/
OS_U32 OS_ConvertCycles2us(OS_U32 Cycles) {
  return Cycles/(OS_PCLK_TIMER/1000000u);
}

/*********************************************************************
*
*       Optional communication with embOSView
*
**********************************************************************
*/

#if (OS_VIEW_IFSELECT == OS_VIEW_IF_JLINK)                    // Communication via JTAG / SWD

static void _JLINKMEM_OnRx(OS_U8 Data);
static void _JLINKMEM_OnTx(void);
static OS_INT _JLINKMEM_GetNextChar(void);

/*********************************************************************
*
*       _JLINKMEM_OnRx()
*/
static void _JLINKMEM_OnRx(OS_U8 Data) {
  OS_OnRx(Data);
}

/*********************************************************************
*
*       _JLINKMEM_OnTx()
*/
static void _JLINKMEM_OnTx(void) {
  OS_OnTx();
}

/*********************************************************************
*
*       _JLINKMEM_GetNextChar()
*/
static OS_INT _JLINKMEM_GetNextChar(void) {
  return OS_COM_GetNextChar();
}

/*********************************************************************
*
*       OS_COM_Init()
*       Initialize memory access for embOSView
*/
void OS_COM_Init(void) {
  JLINKMEM_SetpfOnRx(_JLINKMEM_OnRx);
  JLINKMEM_SetpfOnTx(_JLINKMEM_OnTx);
  JLINKMEM_SetpfGetNextChar(_JLINKMEM_GetNextChar);
}

/*********************************************************************
*
*       OS_COM_Send1()
*       Send one character via memory
*/
void OS_COM_Send1(OS_U8 c) {
  JLINKMEM_SendChar(c);
}

#elif (OS_VIEW_IFSELECT == OS_VIEW_DISABLED)

void OS_COM_Send1(OS_U8 c) {
  OS_USEPARA(c);           /* Avoid compiler warning */
  OS_COM_ClearTxActive();  /* Let the OS know that Tx is not busy */
}

/*********************************************************************
*
*       OS_COM_IsrHandler
*
* Function description
*   The communication interrupt handler for UART communication
*   to embOSView.
*
* NOTES:
*   (1) It has to be inserted in the interrupt vector table, if RAM
*       vectors are not used. Therefore is is declared public
*/
void OS_COM_IsrHandler(void) {
  while(1);
}

#else
  #error "Selected embOSView interface is currently not supported."
#endif

/*********************************************************************
*
*       OS interrupt handler and ISR specific functions
*
**********************************************************************
*/

#if OS_USE_VARINTTABLE
  //
  // The interrupt vector table may be located anywhere in RAM
  //
  #ifdef __ICCARM__  // IAR
    #pragma data_alignment=512
    __no_init void (*g_pfnRAMVectors[NUM_INTERRUPTS]) (void);
  #endif  // __ICCARM__

  #ifdef __CC_ARM    // KEIL
    __attribute__ (zero_init, aligned(512)) void (*g_pfnRAMVectors[NUM_INTERRUPTS])(void);
  #endif

  #ifdef __GNUC__    // GCC
    void (*g_pfnRAMVectors[NUM_INTERRUPTS]) (void) __attribute__ ((aligned (512)));
  #endif
#endif

/*********************************************************************
*
*       OS_ARM_InstallISRHandler
*/
OS_ISR_HANDLER* OS_ARM_InstallISRHandler (int ISRIndex, OS_ISR_HANDLER* pISRHandler) {
#if OS_USE_VARINTTABLE
  OS_ISR_HANDLER*  pOldHandler;
  OS_U32           ulIdx;
  OS_U32*          pVect;

  pOldHandler = NULL;
  //
  // Check whether the RAM vector table has been initialized.
  //
  if ((*(OS_U32*)NVIC_VTOREG_ADDR) != (unsigned long)g_pfnRAMVectors) {
    //
    // Copy the vector table from the beginning of FLASH to the RAM vector table.
    //
    pVect = (OS_U32*)(*(OS_U32*)NVIC_VTOREG_ADDR);
    for(ulIdx = 0; ulIdx < NUM_INTERRUPTS; ulIdx++) {
      g_pfnRAMVectors[ulIdx] = (void (*)(void))(pVect[ulIdx]);
    }
    //
    // Program the NVIC vector base register to point at the RAM vector table.
    //
    *(OS_U32*)NVIC_VTOREG_ADDR = (OS_U32)g_pfnRAMVectors;
  }
  //
  // Save the interrupt handler.
  //
  pOldHandler = g_pfnRAMVectors[ISRIndex];
  g_pfnRAMVectors[ISRIndex] = pISRHandler;
  return (pOldHandler);
#else
  //
  // The function does nothing if vector table is constant
  //
  OS_USEPARA(ISRIndex);
  OS_USEPARA(pISRHandler);
  return (NULL);
#endif
}

/*********************************************************************
*
*       OS_ARM_EnableISR
*/
void OS_ARM_EnableISR(int ISRIndex) {
  OS_DI();
  if (ISRIndex < NUM_INTERRUPTS) {
    if (ISRIndex >= 16) {
      //
      // Enable standard "external" interrupts, starting at index 16
      //
      ISRIndex -= 16;
      *(((OS_U32*) NVIC_ENABLE_ADDR) + (ISRIndex >> 5)) = (1uL << (ISRIndex & 0x1Fu));
    } else if (ISRIndex == ISR_ID_MPU) {
      //
      // Enable the MemManage interrupt.
      //
      SYSHND_CTRL |= NVIC_SYS_HND_CTRL_MEM;
    } else if (ISRIndex == ISR_ID_BUS) {
      //
      // Enable the bus fault interrupt.
      //
      SYSHND_CTRL |= NVIC_SYS_HND_CTRL_BUS;
    } else if (ISRIndex == ISR_ID_USAGE) {
      //
      // Enable the usage fault interrupt.
      //
      SYSHND_CTRL |= NVIC_SYS_HND_CTRL_USAGE;
    } else if (ISRIndex == ISR_ID_SYSTICK) {
      //
      // Enable the System Tick interrupt.
      //
      SYS_TICK_CONTROL |= (1uL << SYS_TICK_INT_ENABLE_BIT);
    }
  }
  OS_RestoreI();
}

/*********************************************************************
*
*       OS_ARM_DisableISR
*/
void OS_ARM_DisableISR(int ISRIndex) {
  OS_DI();
  if (ISRIndex < NUM_INTERRUPTS) {
    if (ISRIndex >= 16) {
      //
      // Disable standard "external" interrupts
      //
      ISRIndex -= 16;
      *(((OS_U32*) NVIC_DISABLE_ADDR) + (ISRIndex >> 5)) = (1uL << (ISRIndex & 0x1Fu));
    } else if (ISRIndex == ISR_ID_MPU) {
      //
      // Disable the MemManage interrupt.
      //
      SYSHND_CTRL &= ~NVIC_SYS_HND_CTRL_MEM;
    } else if (ISRIndex == ISR_ID_BUS) {
      //
      // Disable the bus fault interrupt.
      //
      SYSHND_CTRL &= ~NVIC_SYS_HND_CTRL_BUS;
    } else if (ISRIndex == ISR_ID_USAGE) {
      //
      // Disable the usage fault interrupt.
      //
      SYSHND_CTRL &= ~NVIC_SYS_HND_CTRL_USAGE;
    } else if (ISRIndex == ISR_ID_SYSTICK) {
      //
      // Enable the System Tick interrupt.
      //
      SYS_TICK_CONTROL &= ~(1uL << SYS_TICK_INT_ENABLE_BIT);
    }
  }
  OS_RestoreI();
}

/*********************************************************************
*
*       OS_ARM_ISRSetPrio
*
*   Notes:
*     (1) Some priorities of system handler are reserved
*         0..3 : Priority can not be set
*         7..10: Reserved
*         13   : Reserved
*     (2) System handler use different control register. This affects
*         ISRIndex 0..15
*/
int OS_ARM_ISRSetPrio(int ISRIndex, int Prio) {
  OS_U8* pPrio;
  int    OldPrio;

  OldPrio = 0;
  if (ISRIndex < NUM_INTERRUPTS) {
    if (ISRIndex >= 16) {
      //
      // Handle standard "external" interrupts
      //
      ISRIndex -= 16;                   // Adjust index
      OS_DI();
      pPrio   = (OS_U8*)(NVIC_PRIOBASE_ADDR + ISRIndex);
      OldPrio = *pPrio;
      *pPrio  = Prio;
      OS_RestoreI();
    } else {
      //
      // Handle System Interrupt controller
      //
      if ((ISRIndex < 4) | ((ISRIndex >= 7) && (ISRIndex <= 10)) | (ISRIndex == 13)) {
        //
        // Reserved ISR channel, do nothing
        //
      } else {
        //
        // Set priority in system interrupt priority control register
        //
        OS_DI();
        pPrio = (OS_U8*)(SYSPRI1_ADDR);
        ISRIndex -= 4;                  // Adjust Index
        OldPrio = pPrio[ISRIndex];
        pPrio[ISRIndex] = Prio;
        OS_RestoreI();
      }
    }
  }
  return OldPrio;
}

/*********************************************************************
*
*       __low_level_init()
*
* Function description
*   Called from startup code.
*   Sets the clocking to run directly from the crystal, but
*   can be used to initialize PLL as early as possible.
*   The PLL output is 200 MHz. You have to define a proper divider
*   to set the appropriate CPU speed.
*
*   For example:
*
*   (SYSCTRL_SYSDIV_4 | SYSCTRL_USE_PLL | SYSCTRL_OSC_MAIN | SYSCTRL_XTAL_8MHZ)
*   Sets the clocking to run at 50MHz from the PLL.
*
*   (SYSCTRL_SYSDIV_10 | SYSCTRL_USE_PLL | SYSCTRL_OSC_MAIN | SYSCTRL_XTAL_8MHZ)
*   Sets the clocking to run at 20MHz from the PLL.
*
*/
#ifdef __cplusplus
extern "C" {
#endif
int __low_level_init(void);    // Avoid "no prototype" warning
#ifdef __cplusplus
  }
#endif
int __low_level_init(void) {
  volatile unsigned long ulDelay, ulConfig;
  unsigned long ulRCC;

  //
  // Disable watchdog
  //
  SYSCTRL_RCGC0 |= (1uL << 3);      // Activate clock for WDT module, keep other bits
  WDTLOCK        = 0x1ACCE551uL;    // Allow access of watchdog module
  WDTCTL        &= ~(0x3uL);        // Disable watchdog reset and interrupt, keep reserved bits
  WDTLOCK        = 0x0u;            // Lock access of watchdog module
  //
  // Increase the LDO voltage to 2.75V to guarantee PLL function on higher frequencies
  // Required for silicon revision A1 and A2
  //
  SYSCTRL_LDOPCTL = 0x1Bu;
  //
  // Set the clocking to run directly from the crystal.
  //
  ulConfig = (SYSCTRL_SYSDIV_1 | SYSCTRL_USE_OSC | SYSCTRL_OSC_MAIN | SYSCTRL_XTAL_8MHZ);
  //
  // Get the current value of the RCC register.
  //
  ulRCC = SYSCTRL_RCC;
  //
  // Bypass the PLL and system clock dividers for now.
  //
  ulRCC |= SYSCTRL_RCC_BYPASS;
  ulRCC &= ~(SYSCTRL_RCC_USE_SYSDIV);
  //
  // Write the new RCC value.
  //
  SYSCTRL_RCC = ulRCC;
  //
  // Make sure that the PLL and system clock dividers are bypassed for now.
  //
  ulRCC |= SYSCTRL_RCC_BYPASS;
  ulRCC &= ~(SYSCTRL_RCC_USE_SYSDIV);
  //
  // Make sure that the required oscillators are enabled.  For now, the
  // previously enabled oscillators must be enabled along with the newly
  // requested oscillators.
  //
  ulRCC &= (~(SYSCTRL_RCC_IOSCDIS | SYSCTRL_RCC_MOSCDIS) |
            (ulConfig & (SYSCTRL_RCC_IOSCDIS | SYSCTRL_RCC_MOSCDIS)));
  //
  // Set the new crystal value, oscillator source, and PLL configuration.
  //
  ulRCC &= ~(SYSCTRL_RCC_XTAL_MASK | SYSCTRL_RCC_OSCSRC_MASK |
             SYSCTRL_RCC_PWRDN | SYSCTRL_RCC_OE);
  ulRCC |= ulConfig & (SYSCTRL_RCC_XTAL_MASK | SYSCTRL_RCC_OSCSRC_MASK |
                       SYSCTRL_RCC_PWRDN | SYSCTRL_RCC_OE);
  //
  // Clear the PLL lock interrupt.
  //
  SYSCTRL_MISC = SYSCTRL_INT_PLL_LOCK;
  //
  // Write the new RCC value.
  //
  SYSCTRL_RCC = ulRCC;
  //
  // Wait for a bit so that new crystal value and oscillator source can take
  // effect.  One of the oscillators may need to be started as well.
  //
  for(ulDelay = 0; ulDelay < 16; ulDelay++) {
  }
  //
  // Disable the appropriate oscillators.
  //
  ulRCC &= ~(SYSCTRL_RCC_IOSCDIS | SYSCTRL_RCC_MOSCDIS);
  ulRCC |= ulConfig & (SYSCTRL_RCC_IOSCDIS | SYSCTRL_RCC_MOSCDIS);
  //
  // Write the new RCC value.
  //
  SYSCTRL_RCC = ulRCC;
  //
  // Set the requested system divider.  This will not get written
  // immediately.
  //
  ulRCC &= ~(SYSCTRL_RCC_SYSDIV_MASK | SYSCTRL_RCC_USE_SYSDIV);
  ulRCC |= ulConfig & (SYSCTRL_RCC_SYSDIV_MASK | SYSCTRL_RCC_USE_SYSDIV);
  //
  // See if the PLL output is being used to clock the system.
  //
  if(!(ulConfig & SYSCTRL_RCC_BYPASS)) {
    //
    // Wait until the PLL has locked.
    //
    for(ulDelay = 32768; ulDelay > 0; ulDelay--) {
      if(SYSCTRL_RIS & SYSCTRL_INT_PLL_LOCK) {
        break;
      }
    }
    //
    // Enable use of the PLL.
    //
    ulRCC &= ~(SYSCTRL_RCC_BYPASS);
  }
  //
  // Write the final RCC value.
  //
  SYSCTRL_RCC = ulRCC;
  //
  // Delay for a little bit so that the system divider takes effect.
  //
  for(ulDelay = 0; ulDelay < 16; ulDelay++) {
  }
  //
  // Initialize NVIC vector base address. Might be necessary for RAM targets or application not running from 0
  //
  NVIC_VTOR = (OS_U32)&__Vectors;

  return 1;                       // Always initialize segments !
}

/****** End Of File *************************************************/

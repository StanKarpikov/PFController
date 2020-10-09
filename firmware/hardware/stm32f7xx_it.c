/**
 * @file stm32f7xx_it.c
 * @author Stanislav Karpikov
 * @brief Interrupt management module
 */

/** @addtogroup hdw_irq
 * @{
 */
/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include "stdint.h"
#include "stm32f7xx.h"
#include "string.h"

#include "BSP/gpio.h"
#include "BSP/debug.h"
#include "BSP/system.h"

/*--------------------------------------------------------------
                       PRIVATE FUNCTIONS
--------------------------------------------------------------*/

/**
 * @brief  Extract registers from the stack and print the contents
 * 
 * @stack_address  Address of the stack
 */
void get_registers_from_stack(uint32_t* stack_address)
{
    volatile unsigned int lr = stack_address[5];
    volatile unsigned int pc = stack_address[6];
    volatile unsigned int psr = stack_address[7];

    gpio_error_led_on();
    /* See: 4.3.10. Configurable Fault Status Register for deyeils */
    /* http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0553a/Cihcfefj.html */

    volatile uint32_t CFSR = SCB->CFSR;
    char error_msg[96] = "";

    if (SCB->HFSR & SCB_HFSR_FORCED_Msk)
    {
        /* UsageFault Status Register. */
        if ((CFSR & (1 << 25)) != 0)
        {
            strcpy(error_msg, "Divide by zero");
            BREAKPOINT();
        }

        if ((CFSR & (1 << 24)) != 0)
        {
            strcpy(error_msg, "Unaligned access");
            BREAKPOINT();
        }

        if ((CFSR & (1 << 19)) != 0)
        {
            strcpy(error_msg, "No coprocessor UsageFault");
            BREAKPOINT();
        }

        if ((CFSR & (1 << 18)) != 0)
        {
            strcpy(error_msg, "Invalid PC load UsageFault");
            BREAKPOINT();
        }

        if ((CFSR & (1 << 17)) != 0)
        {
            strcpy(error_msg,
                   "The processor has attempted to execute an instruction that makes illegal use of the EPSR");
            BREAKPOINT();
        }

        if ((CFSR & (1 << 16)) != 0)
        {
            strcpy(error_msg, "Undefined instruction");
            BREAKPOINT();
        }

        /* BusFault Status Register */
        if ((CFSR & (1 << 15)) != 0)
        {
            strcpy(error_msg, "BFAR holds a valid fault address");
            BREAKPOINT();
        }

        if ((CFSR & (1 << 13)) != 0)
        {
            strcpy(error_msg,
                   "a bus fault occurred during floating-point lazy state preservation");
            BREAKPOINT();
        }

        if ((CFSR & (1 << 12)) != 0)
        {
            strcpy(error_msg,
                   "stacking for an exception entry has caused one or more BusFaults");
            BREAKPOINT();
        }

        if ((CFSR & (1 << 11)) != 0)
        {
            strcpy(error_msg,
                   "unstack for an exception return has caused one or more BusFaults");
            BREAKPOINT();
        }

        if ((CFSR & (1 << 10)) != 0)
        {
            strcpy(error_msg, "Imprecise data bus error");
            BREAKPOINT();
        }

        if ((CFSR & (1 << 9)) != 0)
        {
            strcpy(error_msg, "Precise data bus error");
            BREAKPOINT();
        }

        if ((CFSR & (1 << 8)) != 0)
        {
            strcpy(error_msg, "Instruction bus error:");
            BREAKPOINT();
        }

        /* MemManage Fault Status Register */
        if ((CFSR & (1 << 7)) != 0)
        {
            strcpy(error_msg, "MemManage Fault Address Register (MMFAR) valid flag:");
            BREAKPOINT();
        }

        if ((CFSR & (1 << 5)) != 0)
        {
            strcpy(error_msg,
                   "MemManage fault occurred during floating-point lazy state preservation");
            BREAKPOINT();
        }

        if ((CFSR & (1 << 4)) != 0)
        {
            strcpy(error_msg, "MemManage fault on stacking for exception entry");
            BREAKPOINT();
        }

        if ((CFSR & (1 << 3)) != 0)
        {
            strcpy(error_msg, "MemManage fault on unstacking for a return from exception");
            BREAKPOINT();
        }

        if ((CFSR & (1 << 1)) != 0)
        {
            strcpy(error_msg, "Data access violation flag");
            BREAKPOINT();
        }

        if ((CFSR & (1 << 0)) != 0)
        {
            strcpy(error_msg, "Instruction access violation flag");
            BREAKPOINT();
        }
    }

    snprintf(error_msg, sizeof(error_msg), "%s LR=%08X", error_msg, lr);
    snprintf(error_msg, sizeof(error_msg), "%s PC=%08X", error_msg, pc);
    snprintf(error_msg, sizeof(error_msg), "%s PSR=%08X", error_msg, psr);

    BREAKPOINT();

    /*
		for(volatile int i=0;i<0xFFFFFFF;i++);
		NVIC_SystemReset();
		*/
}

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

/**
 * @brief Hard fault interrupt handler 
 */
__asm void HardFault_Handler(void)
{
    // clang-format off

    tst lr, #4
    ite eq
    mrseq r0, msp
    mrsne r0, psp
    ldr r1, [r0, #24]
    b __cpp(get_registers_from_stack)
    // clang-format on
}

/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
    while (1)
    {
    }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
    while (1)
    {
    }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
    while (1)
    {
    }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
}
/** @} */
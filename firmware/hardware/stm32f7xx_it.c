/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f7xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f7xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
 
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void _get_registers_from_stack(uint32_t* stack_address)
{

    volatile unsigned int lr = stack_address[ 5 ];
    volatile unsigned int pc = stack_address[ 6 ];
    volatile unsigned int psr = stack_address[ 7 ];

		HAL_GPIO_WritePin(GPIOD, LED_1_Pin,GPIO_PIN_SET);
    /* See: 4.3.10. Configurable Fault Status Register for deyeils */
    /* http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0553a/Cihcfefj.html */

    volatile uint32_t CFSR = SCB->CFSR;
    char error_msg [96] = "";

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

		for(volatile int i=0;i<0xFFFFFFF;i++);
		// USER CODE END HardFault_IRQn 0 
		NVIC_SystemReset();
}
__asm void HardFault_Handler(void)
{
    tst lr, #4
    ite eq
    mrseq r0, msp
    mrsne r0, psp
    ldr r1, [r0, #24]
    b __cpp(_get_registers_from_stack)
}

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_adc1;
extern DMA_HandleTypeDef hdma_tim1_ch1;
extern DMA_HandleTypeDef hdma_tim1_ch2;
extern DMA_HandleTypeDef hdma_tim1_ch3;
extern DMA_HandleTypeDef hdma_tim8_ch2;
extern DMA_HandleTypeDef hdma_tim8_ch3;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern UART_HandleTypeDef huart1;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M7 Processor Interruption and Exception Handlers          */ 
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler1(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */
	DINT;
	currentTime++;
	EINT;
  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F7xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f7xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */

  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream0 global interrupt.
  */
void DMA2_Stream0_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream0_IRQn 0 */

  /* USER CODE END DMA2_Stream0_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_adc1);
  /* USER CODE BEGIN DMA2_Stream0_IRQn 1 */

  /* USER CODE END DMA2_Stream0_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream1 global interrupt.
  */
void DMA2_Stream1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream1_IRQn 0 */

  /* USER CODE END DMA2_Stream1_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_tim1_ch1);
  /* USER CODE BEGIN DMA2_Stream1_IRQn 1 */

  /* USER CODE END DMA2_Stream1_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream2 global interrupt.
  */
void DMA2_Stream2_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream2_IRQn 0 */

  /* USER CODE END DMA2_Stream2_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_tim1_ch2);
  /* USER CODE BEGIN DMA2_Stream2_IRQn 1 */

  /* USER CODE END DMA2_Stream2_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream3 global interrupt.
  */
void DMA2_Stream3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream3_IRQn 0 */

  /* USER CODE END DMA2_Stream3_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_tim8_ch2);
  /* USER CODE BEGIN DMA2_Stream3_IRQn 1 */

  /* USER CODE END DMA2_Stream3_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream4 global interrupt.
  */
void DMA2_Stream4_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream4_IRQn 0 */

  /* USER CODE END DMA2_Stream4_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_tim8_ch3);
  /* USER CODE BEGIN DMA2_Stream4_IRQn 1 */

  /* USER CODE END DMA2_Stream4_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream5 global interrupt.
  */
void DMA2_Stream5_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream5_IRQn 0 */

  /* USER CODE END DMA2_Stream5_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart1_rx);
  /* USER CODE BEGIN DMA2_Stream5_IRQn 1 */

  /* USER CODE END DMA2_Stream5_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream6 global interrupt.
  */
void DMA2_Stream6_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream6_IRQn 0 */

  /* USER CODE END DMA2_Stream6_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_tim1_ch3);
  /* USER CODE BEGIN DMA2_Stream6_IRQn 1 */

  /* USER CODE END DMA2_Stream6_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream7 global interrupt.
  */
void DMA2_Stream7_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream7_IRQn 0 */

  /* USER CODE END DMA2_Stream7_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart1_tx);
  /* USER CODE BEGIN DMA2_Stream7_IRQn 1 */

  /* USER CODE END DMA2_Stream7_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

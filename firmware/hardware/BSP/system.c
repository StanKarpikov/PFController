/**
 * @file system.h
 * @author Stanislav Karpikov
 * @brief Board support package: system tools
 */

/** @addtogroup hdw_bsp_system
 * @{
 */
 
/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include "BSP/bsp.h"
#include "BSP/system.h"
#include "stm32f7xx_hal.h"

/*--------------------------------------------------------------
                       PRIVATE DEFINES
--------------------------------------------------------------*/

#define TIME_MAX_VALUE (4133894400000ULL) /**< Maximum time constant (used to exclude wrong packets) */

/*--------------------------------------------------------------
                       PRIVATE DATA
--------------------------------------------------------------*/

static uint32_t current_time = 0; /**< Time accumulator variable, 64-bit Unix timestamp (with us) */

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

/**
 * @brief Initializes the Global MSP
 */
void HAL_MspInit(void)
{
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_RCC_SYSCFG_CLK_ENABLE();
}

/*
 * @brief Increment the time tick
 */
void system_increment_time(void)
{
    uint64_t current_time_next = current_time;
    current_time_next++;

    ENTER_CRITICAL();
    current_time = current_time_next;
    EXIT_CRITICAL();
}

/*
 * @brief Set the local time
 *
 * @param time 64-bit Unix timestamp (with us)
 */
void system_set_time(uint64_t time)
{
    if (time > TIME_MAX_VALUE)
    {
        time = 0;
    }
    ENTER_CRITICAL();
    current_time = time;
    EXIT_CRITICAL();
}

/*
 * @brief Get the local time
 *
 * @return 64-bit Unix timestamp (with us)
 */
uint64_t system_get_time(void)
{
    ENTER_CRITICAL();
    uint64_t time = current_time;
    EXIT_CRITICAL();
    return time;
}

/*
 * @brief Wait for a delay (hard waiting)
 *
 * @param delay_ticks Ticks to wait
 *
 * @return The status of the operation
 */
status_t system_delay_ticks(uint32_t delay_ticks)
{
    HAL_Delay(delay_ticks);
    return PFC_SUCCESS;
}

/**
  * @brief System Clock Configuration
  */
void SystemClock_Config(void)
{
#if defined (BOARD_STM32F767_MAIN)
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    /* Configure the main internal regulator output voltage   */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    /* Initializes the CPU, AHB and APB busses clocks   */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 200;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        error_handler();
    }
    /* Activate the Over-Drive mode   */
    if (HAL_PWREx_EnableOverDrive() != HAL_OK)
    {
        error_handler();
    }
    /* Initializes the CPU, AHB and APB busses clocks   */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6) != HAL_OK)
    {
        error_handler();
    }

    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1;
    PeriphClkInitStruct.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
        error_handler();
    }
#elif defined(BOARD_STM32F723_DISCO)
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    error_handler();
  }
  /** Activate the Over-Drive mode 
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    error_handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    error_handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART6;
  PeriphClkInitStruct.Usart6ClockSelection = RCC_USART6CLKSOURCE_PCLK2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    error_handler();
  }
#endif
}

/*
 * @brief Reset of all peripherals, Initializes the Flash interface and the Systick
 *
 * @param The status of the operation
 */
status_t system_init(void)
{
    HAL_Init();
    SystemClock_Config();
    return PFC_SUCCESS;
}

/**
 * @brief This function handles System tick timer
 */
void SysTick_Handler(void)
{
    system_increment_time();

    HAL_IncTick();
}
/** @} */

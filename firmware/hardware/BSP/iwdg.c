/**
 * @file iwdg.c
 * @author Stanislav Karpikov
 * @brief Board support package: Independent watchdog
 */

/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include "BSP/iwdg.h"
#include "BSP/debug.h"
#include "stm32f7xx_hal.h"

/*--------------------------------------------------------------
                       PUBLIC DEFINES
--------------------------------------------------------------*/

#undef ENABLE_WATCHDOG /**< Define to enable Watchdog module */
#define WATCHDOG_WINDOW (4095) /**< Window size for the watchdog module */
#define WATCHDOG_RELOAD (4095) /**< Reload value for the watchdog module */
#define WATCHDOG_PRESCALER IWDG_PRESCALER_128 /**< Prescaler for the watchdog module */
		 
/*--------------------------------------------------------------
                       PRIVATE DATA
--------------------------------------------------------------*/

#if defined(ENABLE_WATCHDOG)
static IWDG_HandleTypeDef hiwdg; 
#endif

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

/*
  * @brief IWDG Initialization Function
	*
  * @return The status of the operation
  */
status_t iwdg_init(void)
{
#if defined(ENABLE_WATCHDOG)
    /* Approx. 10 sec */
    hiwdg.Instance = IWDG;
    hiwdg.Init.Prescaler = WATCHDOG_PRESCALER;
    hiwdg.Init.Window = WATCHDOG_WINDOW;
    hiwdg.Init.Reload = WATCHDOG_RELOAD;
    if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
    {
        error_handler();
    }
#endif
    return PFC_SUCCESS;
}

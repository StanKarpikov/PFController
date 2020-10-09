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

#undef ENABLE_WATCHDOG

/*--------------------------------------------------------------
                       PRIVATE DATA
--------------------------------------------------------------*/

#if defined(ENABLE_WATCHDOG)
static IWDG_HandleTypeDef hiwdg;
#endif

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

/**
  * @brief IWDG Initialization Function
  * @param None
  * @retval None
  */
status_t iwdg_init(void)
{
#if defined(ENABLE_WATCHDOG)
    /* Approx. 10 sec */
    hiwdg.Instance = IWDG;
    hiwdg.Init.Prescaler = IWDG_PRESCALER_128;
    hiwdg.Init.Window = 4095;
    hiwdg.Init.Reload = 4095;
    if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
    {
        error_handler();
    }
#endif
    return PFC_SUCCESS;
}

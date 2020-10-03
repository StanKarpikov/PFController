/**
 * @file iwdg.c
 * @author Stanislav Karpikov
 * @brief Board support package: Independent watchdog
 */

/*---------------------  INCLUDES  -------------------------------------------*/

#include "BSP/iwdg.h"
#include "BSP/debug.h"
#include "stm32f7xx_hal.h"

IWDG_HandleTypeDef hiwdg;

/**
  * @brief IWDG Initialization Function
  * @param None
  * @retval None
  */
void MX_IWDG_Init(void)
{

  /* USER CODE BEGIN IWDG_Init 0 */
	return;
  /* USER CODE END IWDG_Init 0 */

  /* USER CODE BEGIN IWDG_Init 1 */
//10 sec
  /* USER CODE END IWDG_Init 1 */
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_128;
  hiwdg.Init.Window = 4095;
  hiwdg.Init.Reload = 4095;
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN IWDG_Init 2 */

  /* USER CODE END IWDG_Init 2 */

}

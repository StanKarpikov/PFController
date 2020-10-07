/**
 * @file debug.c
 * @author Stanislav Karpikov
 * @brief Board support package: Debug utils
 */

/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include "BSP/debug.h"
#include "BSP/uart.h"
#include "stm32f7xx_hal.h"

/*--------------------------------------------------------------
                       PUBLIIC FUNCTIONS
--------------------------------------------------------------*/

int fputc(int c, FILE *f)
{
    if (c == '\n') fputc('\r', f);
    //HAL_GPIO_WritePin(UART6_DE_GPIO_Port, UART6_DE_Pin, GPIO_PIN_SET);
    uart_debug_transmit((uint8_t *)&c, 1);
    //HAL_GPIO_WritePin(UART6_DE_GPIO_Port, UART6_DE_Pin, GPIO_PIN_RESET);

    return 1;
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void error_handler(void)
{
    BREAKPOINT();
	/*
    for (volatile int i = 0; i < 0xFFFFFFF; i++);
    NVIC_SystemReset();
    while (1);
	*/
}

bool is_debug_session(void)
{
    return (CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk) == 0U ? false : true;
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

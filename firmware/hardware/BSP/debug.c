/**
 * @file debug.c
 * @author Stanislav Karpikov
 * @brief Board support package: Debug utils
 */

/** @addtogroup hdw_bsp_debug
 * @{
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

/*
 * @brief Overrides the default putc handler (used in printf)
 *
 * @param c Character to transmit
 * @param f The pointer to the file
 *
 * @return The number of bytes written
 */
int fputc(int c, FILE *f)
{
    if (c == '\n') fputc('\r', f);
    //HAL_GPIO_WritePin(UART6_DE_GPIO_Port, UART6_DE_Pin, GPIO_PIN_SET);
    uart_debug_transmit((uint8_t *)&c, 1);
    //HAL_GPIO_WritePin(UART6_DE_GPIO_Port, UART6_DE_Pin, GPIO_PIN_RESET);

    return 1;
}

/*
 * @brief  This function is executed in case of error occurrence.
 */
void error_handler(void)
{
    BREAKPOINT();
    /* TODO: Add sophisticated error processing */
    /*
    for (volatile int i = 0; i < 0xFFFFFFF; i++);
    NVIC_SystemReset();
    while (1);
	  */
}

/*
 * @brief Check if the debug session is enabled (a debugger is connected)
 *
 * @retval true A debug session is enabled
 * @retval false A debug session is disabled
 */
bool is_debug_session(void)
{
    return (CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk) == 0U ? false : true;
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file pointer to the source file name
  * @param  line assert_param error line source number
  */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}
#endif /* USE_FULL_ASSERT */

/** @} */
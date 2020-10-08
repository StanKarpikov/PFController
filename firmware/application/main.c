/**
 * @file main.h
 * @author Stanislav Karpikov
 * @brief Main file with initialisation and entry point
 */

/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include "EEPROM_emulation.h"
#include "pfc_logic.h"
#include "command_processor.h"
#include "events_process.h"
#include "settings.h"
/* BSP */
#include "BSP/adc.h"
#include "BSP/debug.h"
#include "BSP/dma.h"
#include "BSP/gpio.h"
#include "BSP/iwdg.h"
#include "BSP/system.h"
#include "BSP/timer.h"
#include "BSP/uart.h"
/* app */
#include "adc_logic.h"
/* settings */
#include "defines.h"
#include "string.h"

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

/**
  * @brief  The application entry point.
  * @return Status
  */
int main(void)
{
    system_init();

    ReadSettings();

    /* Initialize all peripherals */
    gpio_init();
    dma_init();
    adc_init();
	  timer_init();
    uart_init();
    iwdg_init();

    system_delay_ticks(STARTUP_TIMEOUT);

    adc_logic_start();

    protocol_hw_init();

    gpio_status_led_on();

    while (1)
    {
        protocol_work();

        algorithm_process();
        
        adc_set_temperature(28);  //TODO: Add temperature sensor measurement
        events_check_temperature();
    }
}

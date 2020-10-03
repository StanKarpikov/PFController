/**
 * @file main.h
 * @author Stanislav Karpikov
 * @brief Main file with initialisation and entry point
 */

/*---------------------  INCLUDES  -------------------------------------------*/

#include "main.h"
#include "device.h"
#include "clogic.h"
#include "algorithm.h"
#include "events_process.h"
#include "settings.h"
#include "EEPROM_emulation.h"

/* BSP */
#include "BSP/adc.h"
#include "BSP/timer.h"
#include "BSP/iwdg.h"
#include "BSP/uart.h"
#include "BSP/debug.h"
#include "BSP/system.h"
#include "BSP/dma.h"
#include "BSP/gpio.h"

/* app */
#include "adc_logic.h"

/*-------------------  PRIVATE DATA  -----------------------------------------*/

float sinusoid[2000];


/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  system_init();

	EEPROMClass();
	memset(&KKM,0,sizeof(KKM));
	ReadSettings(&KKM.settings);
	KKM.status=KKM_STATE_INIT; //!< Текущее состояние работы

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_TIM8_Init();
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  MX_TIM9_Init();
  MX_TIM2_Init();
  MX_IWDG_Init();

	
	HAL_Delay(1000);
	
	adc_start();

	protocol_hw_init();
	
	gpio_status_led_on();
	
	for(int i=0;i<2000;i++){
		sinusoid[i]=sin((float)i/2000.0f*2.0f*3.1416f)*2000.0f;
	}

  while (1){
		protocol_work(&KKM.protocol);

		algorithm_work();
		//
		KKM.temperature=28;//TODO:
		events_check_temperature();
  }
}

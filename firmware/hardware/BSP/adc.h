/**
 * @file adc.h
 * @author Stanislav Karpikov
 * @brief Board support package: ADC (header)
 */

#ifndef _ADC_H
#define _ADC_H

/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include "stdint.h"
#include "BSP/debug.h"

/*--------------------------------------------------------------
                       PUBLIC TYPES
--------------------------------------------------------------*/

/**
 * @brief DMA transfer complete callback type
 */
typedef void (*ADC_TRANSFER_CALLBACK) (void);


/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/
void adc_init(void);
status_t adc_register_callbacks(ADC_TRANSFER_CALLBACK cptl_callback, ADC_TRANSFER_CALLBACK half_cplt_callback);
status_t adc_start(uint32_t* buffer, uint32_t buffer_size);
status_t adc_stop(void);


#endif /* _ADC_H */

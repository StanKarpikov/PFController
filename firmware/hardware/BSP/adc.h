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

/**
 * @brief ADC Initialization Function
 *
 * @return The status of the operation
 */
status_t adc_init(void);

/**
 * @brief Set callbacks for the ADC module
 *
 * @param cptl_callback ADC DMA full complete callback
 * @param half_cplt_callback ADC DMA half complete callback
 *
 * @return The status of the operation
 */
status_t adc_register_callbacks(ADC_TRANSFER_CALLBACK cptl_callback, ADC_TRANSFER_CALLBACK half_cplt_callback);

/**
 * @brief Start the ADC DMA conversion 
 *
 * @param buffer A buffer for the data
 * @param buffer_size The buffer size
 *
 * @return The status of the operation
 */
status_t adc_start(uint32_t* buffer, uint32_t buffer_size);

/**
 * @brief Stop the ADC DMA conversion
 *
 * @return The status of the operation
 */
status_t adc_stop(void);

#endif /* _ADC_H */

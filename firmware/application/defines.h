/**
 * @file defines.h
 * @author Stanislav Karpikov
 * @brief Global settings and defines
 */

#ifndef __DEFINES_H__
#define __DEFINES_H__

/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include "stdint.h"

/*--------------------------------------------------------------
                       PUBLIC DEFINES
--------------------------------------------------------------*/

/* Hardware settings (TODO: move to the panel configuration) */
#define STARTUP_STABILISATION_TIME (100)
#define SYNC_MINIMUM_PHASE (0.03f)
#define PRELOAD_DELAY (100)

#define EPS 0 /**< Used in ADC callback processing */

#define STARTUP_TIMEOUT (1000)
#define PWM_PERIOD (2000)

#define PFC_NCHAN   (3)    //three-phase network
#define BUF_NUM     (2)    //Double buffering
#define ADC_VAL_NUM (128)  //Count per period

#define SYNC_PHASE_ERROR    (0.03f)
#define SYNC_PHASE_DELTAMIN (5e-4)


#define MATH_PI (3.141592653589793)

#endif

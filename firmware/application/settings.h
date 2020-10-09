/**
 * @file settings.h
 * @author Stanislav Karpikov
 * @brief PFC settings to store in NV memory (header)
 */

#ifndef _SETTINGS_H
#define _SETTINGS_H

/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include "defines.h"
#include "stdint.h"
#include "adc_logic.h"

/*--------------------------------------------------------------
                       PUBLIC TYPES
--------------------------------------------------------------*/

/** PWM settings */
typedef struct
{
		float L_coefficient; /**< The calibration coefficient to include the channel inductance */
		uint32_t active_channels[PFC_NCHAN]; /**< Parameters of the channels */
}settings_pwm_t;

/** Filters coefficients settings */
typedef struct
{
		float K_I; /**< Current coefficient */
		float K_U; /**< Voltage coefficient */
		float K_Ucap; /**< Capacitor voltage coefficient */
}settings_filters_t;

/** Calibrations settings */
typedef struct
{
		float calibration[ADC_CHANNEL_NUMBER];  /**< Calibrations: proportional coefficients */
		float offset[ADC_CHANNEL_NUMBER];       /**< ADC channel offsets */
}settings_calibrations_t;

/** Protection settings */
typedef struct
{
		float Ucap_min;       /**< settings: minimum capacitor voltage */
		float Ucap_max;       /**< settings: maximum capacitor voltage */
		float temperature;  /**< settings: the device temperature */
		float U_min;        /**< settings: minimum grid voltage */
		float U_max;				/**< settings: maximum grid voltage */
		float F_min;     		/**< settings: minimum grid frequency */
		float F_max;     		/**< settings: maximum grid frequency */
		float I_max_rms;    /**< settings: maximum current (RMS) */
		float I_max_peak;   /**< settings: maximum current (peak) */
} settings_protection_t;

/** Protection settings */
typedef struct
{
		float ctrl_Ucap_Kp; /**< The capacitors charge PID settings: the proportional coefficient */
		float ctrl_Ucap_Ki; /**< The capacitors charge PID settings: the integral coefficient */
		float ctrl_Ucap_Kd; /**< The capacitors charge PID settings: the differential coefficient */
		float Ucap_nominal; /**< The capacitors nominal voltage */
		float Ucap_precharge;  /**< The precharge level (capacitor voltage) */
} settings_capacitors_t;
		
/** Settings structure */
typedef struct
{
    settings_calibrations_t calibrations; /**< The calibrations settings */
    settings_filters_t filters; /**< The filters settings */
    settings_pwm_t pwm; /**< The PWM settings */
    settings_protection_t protection; /**< The protection settings */
    settings_capacitors_t capacitors; /**< The capacitors settings */
    uint16_t magic; /**< The maic word */
} settings_t;

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

/**
 * @brief Read the PWM settings from the current settings storage
 * 
 * @return The PWM settings structure
 */
settings_pwm_t settings_get_pwm(void);

/**
 * @brief Read the filters settings from the current settings storage
 * 
 * @return The filters settings structure
 */
settings_filters_t settings_get_filters(void);

/**
 * @brief Read the calibrations settings from the current settings storage
 * 
 * @return The calibrations settings structure
 */
settings_calibrations_t settings_get_calibrations(void);

/**
 * @brief Read the protection settings from the current settings storage
 * 
 * @return The protection settings structure
 */
settings_protection_t settings_get_protection(void);

/**
 * @brief Read the capacitors settings from the current settings storage
 * 
 * @return The capacitors settings structure
 */
settings_capacitors_t settings_get_capacitors(void);

/**
 * @brief Write the PWM settings to the current settings storage
 *
 * @param pwm The PWM settings structure
 * 
 * @return The status of the structure
 */
status_t settings_set_pwm(settings_pwm_t pwm);

/**
 * @brief Write the filters settings to the current settings storage
 *
 * @param pwm The filters settings structure
 * 
 * @return The status of the structure
 */
status_t settings_set_filters(settings_filters_t filters);

/**
 * @brief Write the calibrations settings to the current settings storage
 *
 * @param pwm The calibrations settings structure
 * 
 * @return The status of the structure
 */
status_t settings_set_calibrations(settings_calibrations_t calibrations);

/**
 * @brief Write the protection settings to the current settings storage
 *
 * @param pwm The protection settings structure
 * 
 * @return The status of the structure
 */
status_t settings_set_protection(settings_protection_t protection);

/**
 * @brief Write the capacitors settings to the current settings storage
 *
 * @param pwm The capacitors settings structure
 * 
 * @return The status of the structure
 */
status_t settings_set_capacitors(settings_capacitors_t capacitors);

/*
 * @brief Save the current settings to the non-volatile memory
 *
 * @return The status of the structure
 */
status_t settings_save(void);

/*
 * @brief Read the current settings from the non-volatile memory
 *
 * @return The status of the structure
 */
status_t settings_read(void);

#endif /* _SETTINGS_H */

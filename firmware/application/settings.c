/**
 * @file settings.c
 * @author Stanislav Karpikov
 * @brief PFC settings to store in NV memory
 */

/** @addtogroup app_settings
 * @{
 */
 

/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include "settings.h"

#include "EEPROM_emulation.h"
#include "string.h"

/*--------------------------------------------------------------
                       DEFINES
--------------------------------------------------------------*/

#define DEFAULT_UCAP_MIN       (200U)      /**< Default settings: minimum capacitor voltage */
#define DEFAULT_UCAP_MAX       (800U)      /**< Default settings: maximum capacitor voltage */
#define DEFAULT_TEMPERATURE    (50U)       /**< Default settings: the device temperature */
#define DEFAULT_U_MIN          (220 * 0.8) /**< Default settings: minimum grid voltage */
#define DEFAULT_U_MAX          (200 * 1.3) /**< Default settings: maximum grid voltage */
#define DEFAULT_F_MIN          (45)        /**< Default settings: minimum grid frequency */
#define DEFAULT_F_MAX          (55)        /**< Default settings: maximum grid frequency */
#define DEFAULT_I_MAX_RMS      (10)        /**< Default settings: maximum current (RMS) */
#define DEFAULT_I_MAX_PEAK     (15)        /**< Default settings: maximum current (peak) */
#define DEFAULT_UCAP_NOMINAL   (750)       /**< Default settings: nominal capacitor voltage */
#define DEFAULT_UCAP_PRECHARGE (250)       /**< Default settings: precharge level for capacitor voltage */

/*--------------------------------------------------------------
                       PRIVATE DATA
--------------------------------------------------------------*/

static settings_t settings = {0}; /**< The internal settings storage in RAM */

/*--------------------------------------------------------------
                       PRIVATE FUNCTIONS
--------------------------------------------------------------*/

/**
 * @brief Read settings data from the EEPROM memory
 * 
 * @param[out] settings A pointer to the settings structure
 * 
 * @return The status of the operation
 */
static status_t eeprom_settings_read(settings_t *settings)
{
    uint16_t mem[sizeof(settings_t) / 2];

    for (int address = 0; address < sizeof(settings_t) / 2; address++)
    {
        if (EEPROM_OK != eeprom_read_variable(address, &mem[address])) return PFC_ERROR_GENERIC;
    }
    memcpy(settings, mem, sizeof(settings_t));
    if (settings->magic != MAGIC_WORD) return PFC_ERROR_GENERIC;

    return PFC_SUCCESS;
}

/**
 * @brief Fill a settings structure with the default values
 * 
 * @param[out] settings A pointer to the settings structure
 */
static void settings_fill_defaults(settings_t *settings)
{
    memset(settings, 0, sizeof(settings_t));

    settings->pwm.L_coefficient = 1;

    settings->protection.Ucap_min = DEFAULT_UCAP_MIN;
    settings->protection.Ucap_max = DEFAULT_UCAP_MAX;
    settings->protection.temperature = DEFAULT_TEMPERATURE;
    settings->protection.U_min = DEFAULT_U_MIN;
    settings->protection.U_max = DEFAULT_U_MAX;
    settings->protection.F_min = DEFAULT_F_MIN;
    settings->protection.F_max = DEFAULT_F_MAX;
    settings->protection.I_max_rms = DEFAULT_I_MAX_RMS;
    settings->protection.I_max_peak = DEFAULT_I_MAX_PEAK;

    settings->capacitors.Ucap_nominal = DEFAULT_UCAP_NOMINAL;
    settings->capacitors.Ucap_precharge = DEFAULT_UCAP_PRECHARGE;

    for (int i = 0; i < ADC_CHANNEL_NUMBER; i++)
    {
        settings->calibrations.calibration[i] = 1;
    }
}

/**
 * @brief Write settings to the EEPROM memory
 * 
 * @param[in] settings A pointer to the settings structure
 *
 * @return The status of the operation
 */
static status_t eeprom_settings_write(settings_t *settings)
{
    uint16_t data;
    uint16_t mem[sizeof(settings_t) / 2];

    settings->magic = MAGIC_WORD;
    memcpy(mem, settings, sizeof(settings_t));
    for (int address = 0; address < sizeof(settings_t) / 2; address++)
    {
        data = mem[address];
        if (EEPROM_OK != eeprom_update_variable(address, data)) return PFC_ERROR_GENERIC;
    }
    return PFC_SUCCESS;
}

/**
 * @brief Lock the settings data
 */
static void settings_lock(void)
{
    /*TODO: add mutex lock */
}

/**
 * @brief Unlock the settings data
 */
static void settings_unlock(void)
{
    /*TODO: add mutex unlock */
}

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

/**
 * @brief Save the current settings to the non-volatile memory
 *
 * @return The status of the structure
 */
status_t settings_save(void)
{
    if (eeprom_settings_write(&settings) != PFC_SUCCESS)
    {
        //ERROR(ERROR_LOG("EEPROM Write Settings FAILED!\n"));
        return PFC_ERROR_GENERIC;
    }
    return PFC_SUCCESS;
}

/**
 * @brief Read the current settings from the non-volatile memory
 *
 * @return The status of the structure
 */
status_t settings_read(void)
{
    if (eeprom_settings_read(&settings) != PFC_SUCCESS)
    {
        //WARNING(DEF_LOG("EEPROM Read Settings FAILED! May be corrupted or first time running\n"));
        settings_fill_defaults(&settings);
        settings_save();
    }
    return PFC_SUCCESS;
}

/*
 * @brief Write the PWM settings to the current settings storage
 *
 * @param pwm The PWM settings structure
 * 
 * @return The status of the structure
 */
status_t settings_set_pwm(settings_pwm_t pwm)
{
    settings_lock();
    settings.pwm = pwm;
    settings_unlock();

    return PFC_SUCCESS;
}

/*
 * @brief Write the filters settings to the current settings storage
 *
 * @param pwm The filters settings structure
 * 
 * @return The status of the structure
 */
status_t settings_set_filters(settings_filters_t filters)
{
    settings_lock();
    settings.filters = filters;
    settings_unlock();

    return PFC_SUCCESS;
}

/*
 * @brief Write the calibrations settings to the current settings storage
 *
 * @param pwm The calibrations settings structure
 * 
 * @return The status of the structure
 */
status_t settings_set_calibrations(settings_calibrations_t calibrations)
{
    settings_lock();
    settings.calibrations = calibrations;
    settings_unlock();

    return PFC_SUCCESS;
}

/*
 * @brief Write the protection settings to the current settings storage
 *
 * @param pwm The protection settings structure
 * 
 * @return The status of the structure
 */
status_t settings_set_protection(settings_protection_t protection)
{
    settings_lock();
    settings.protection = protection;
    settings_unlock();

    return PFC_SUCCESS;
}

/*
 * @brief Write the capacitors settings to the current settings storage
 *
 * @param pwm The capacitors settings structure
 * 
 * @return The status of the structure
 */
status_t settings_set_capacitors(settings_capacitors_t capacitors)
{
    settings_lock();
    settings.capacitors = capacitors;
    settings_unlock();

    return PFC_SUCCESS;
}

/*
 * @brief Read the PWM settings from the current settings storage
 * 
 * @return The PWM settings structure
 */
settings_pwm_t settings_get_pwm(void)
{
    settings_pwm_t retval;
    settings_lock();
    retval = settings.pwm;
    settings_unlock();
    return retval;
}

/*
 * @brief Read the filters settings from the current settings storage
 * 
 * @return The filters settings structure
 */
settings_filters_t settings_get_filters(void)
{
    settings_filters_t retval;
    settings_lock();
    retval = settings.filters;
    settings_unlock();
    return retval;
}

/*
 * @brief Read the calibrations settings from the current settings storage
 * 
 * @return The calibrations settings structure
 */
settings_calibrations_t settings_get_calibrations(void)
{
    settings_calibrations_t retval;
    settings_lock();
    retval = settings.calibrations;
    settings_unlock();
    return retval;
}

/*
 * @brief Read the protection settings from the current settings storage
 * 
 * @return The protection settings structure
 */
settings_protection_t settings_get_protection(void)
{
    settings_protection_t retval;
    settings_lock();
    retval = settings.protection;
    settings_unlock();
    return retval;
}

/*
 * @brief Read the capacitors settings from the current settings storage
 * 
 * @return The capacitors settings structure
 */
settings_capacitors_t settings_get_capacitors(void)
{
    settings_capacitors_t retval;
    settings_lock();
    retval = settings.capacitors;
    settings_unlock();
    return retval;
}
/** @} */

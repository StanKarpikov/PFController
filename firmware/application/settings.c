/**
 * @file settings.c
 * @author Stanislav Karpikov
 * @brief PFC settings to store in NV memory
 */
 
#ifndef _SETTINGS_H_
#define _SETTINGS_H_

/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include "settings.h"

#include "EEPROM_emulation.h"
#include "string.h"

/*--------------------------------------------------------------
                       PRIVATE DATA
--------------------------------------------------------------*/

static SETTINGS settings={0};
		
/*--------------------------------------------------------------
                       PRIVATE FUNCTIONS
--------------------------------------------------------------*/

static int EEPROM_ReadSettings(SETTINGS *settings)
{
    uint16_t mem[sizeof(SETTINGS) / 2];

    for (int ADDR = 0; ADDR < sizeof(SETTINGS) / 2; ADDR++)
    {
        if (EEPROM_OK != EEreadB(ADDR, &mem[ADDR])) return -1;
    }
    memcpy(settings, mem, sizeof(SETTINGS));
    if (settings->magic != MAGIC_2BYTE) return -1;

    return 1;
}

static void MakeDefaults(SETTINGS *settings)
{
    memset(settings, 0, sizeof(SETTINGS));

    settings->PWM.L_coefficient = 1;

    settings->PROTECTION.Ud_min = 200;
    settings->PROTECTION.Ud_max = 800;
    settings->PROTECTION.temperature = 50;
    settings->PROTECTION.U_min = 220 * 0.8;
    settings->PROTECTION.U_max = 200 * 1.3;
    settings->PROTECTION.Fnet_min = 45;
    settings->PROTECTION.Fnet_max = 55;
    settings->PROTECTION.I_max_rms = 10;
    settings->PROTECTION.I_max_peak = 15;

    settings->CAPACITORS.Ud_nominal = 750;
    settings->CAPACITORS.Ud_precharge = 250;

    for (int i = 0; i < ADC_CHANNEL_NUMBER; i++)
    {
        settings->CALIBRATIONS.calibration[i] = 1;
    }
}


static int EEPROM_WriteSettings(SETTINGS *settings)
{
    uint16_t DATA;
    uint16_t mem[sizeof(SETTINGS) / 2];

    settings->magic = MAGIC_2BYTE;
    memcpy(mem, settings, sizeof(SETTINGS));
    for (int ADDR = 0; ADDR < sizeof(SETTINGS) / 2; ADDR++)
    {
        DATA = mem[ADDR];
        if (EEPROM_OK != EEwrite(ADDR, DATA)) return -1;
    }
    return 1;
}

static void settings_lock(void)
{
	/*TODO: add mutex lock */
}

static void settings_unlock(void)
{
	/*TODO: add mutex unlock */
}
/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

int SaveSettings(SETTINGS *settings)
{
    //return 0;//TODO:
    if (EEPROM_WriteSettings(settings) < 0)
    {
        //ERROR(ERROR_LOG("EEPROM Write Settings FAILED!\n"));
        return -1;
    }
    return 1;
}

int ReadSettings(SETTINGS *settings)
{
    if (EEPROM_ReadSettings(settings) < 0)
    {
        //WARNING(DEF_LOG("EEPROM Read Settings FAILED! May be corrupted or first time running\n"));
        MakeDefaults(settings);
        SaveSettings(settings);
    }
    return 1;
}


status_t settings_set_pwm(settings_pwm_t pwm)
{
	settings_lock();
	settings.PWM = pwm;
	settings_unlock();
	
	return PFC_SUCCESS;
}

status_t settings_set_filters(settings_filters_t filters)
{
	settings_lock();
	settings.FILTERS = filters;
	settings_unlock();
	
	return PFC_SUCCESS;
}

status_t settings_set_calibrations(settings_calibrations_t calibrations)
{
	settings_lock();
	settings.CALIBRATIONS = calibrations;
	settings_unlock();
	
	return PFC_SUCCESS;
}

status_t settings_set_protection(settings_protection_t protection)
{
	settings_lock();
	settings.PROTECTION = protection;
	settings_unlock();
	
	return PFC_SUCCESS;
}

status_t settings_set_capacitors(settings_capacitors_t capacitors)
{
	settings_lock();
	settings.CAPACITORS = capacitors;
	settings_unlock();
	
	return PFC_SUCCESS;
}

settings_pwm_t settings_get_pwm(void)
{
	settings_pwm_t retval;
	settings_lock();
	retval = settings.PWM;
	settings_unlock();
	return retval;
}

settings_filters_t settings_get_filters(void)
{
	settings_filters_t retval;
	settings_lock();
	retval = settings.FILTERS;
	settings_unlock();
	return retval;
}

settings_calibrations_t settings_get_calibrations(void)
{
	settings_calibrations_t retval;
	settings_lock();
	retval = settings.CALIBRATIONS;
	settings_unlock();
	return retval;
}

settings_protection_t settings_get_protection(void)
{
	settings_protection_t retval;
	settings_lock();
	retval = settings.PROTECTION;
	settings_unlock();
	return retval;
}

settings_capacitors_t settings_get_capacitors(void)
{
	settings_capacitors_t retval;
	settings_lock();
	retval = settings.CAPACITORS;
	settings_unlock();
	return retval;
}

#endif

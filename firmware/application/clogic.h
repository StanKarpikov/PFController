#ifndef __CLOGIC_H
#define __CLOGIC_H

#include "defines.h"
#include "settings.h"

uint32_t clogic_state(void);
void clogic_set_state(KKM_STATUS state);
void clogic_do(void);
void clogic_disable_pwm(void);
void clogic_restore_pwm(void);
extern uint8_t PWMon;
	
#endif

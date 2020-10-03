/**
 * @file debug.h
 * @author Stanislav Karpikov
 * @brief Board support package: Debug utils (header)
 */

#ifndef _DEBUG_H
#define _DEBUG_H

/*---------------------  INCLUDES  -------------------------------------------*/

#include "stdbool.h"

/*-------------------  PUBLIC DEFINES  ---------------------------------------*/

/*--------------------  PUBLIC MACRO  ----------------------------------------*/


#define ARGUMENT_ASSERT(ARGUMENT) \
	do{ if(ARGUMENT == 0)return PFC_ERROR_DATA;}while(0)

#define BREAKPOINT() \
	do{ \
		if( is_debug_session() )\
		{\
			__asm  {BKPT 0}\
		} \
	}while(0)
							
#define DINT __disable_irq()
#define EINT __enable_irq()
	
/*------------------  PUBLIC FUNCTIONS  --------------------------------------*/

void Error_Handler(void);
bool is_debug_session(void);

/*---------------------  PUBLIC DATA  ----------------------------------------*/

/** Common error codes */
typedef enum{
	PFC_ERROR_DATA 		= -3,  /**< Wrong input arguments */
	PFC_ERROR_HAL 		= -2,  /**< Error at the lower HAL level */
	PFC_ERROR_GENERIC = -1,  /**< Generic error code */
	PFC_SUCCESS 			=  0,	 /**< Successfull execution */
}status_t;


#endif /* _DEBUG_H */

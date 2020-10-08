/**
 * @file debug.h
 * @author Stanislav Karpikov
 * @brief Board support package: Debug utils (header)
 */

#ifndef _DEBUG_H
#define _DEBUG_H

/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include "stdbool.h"

/*--------------------------------------------------------------
                       PUBLIC DEFINES
--------------------------------------------------------------*/

#define UNUSED(X) (void)X

/*--------------------------------------------------------------
                       PUBLIC MACRO
--------------------------------------------------------------*/

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
	
/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/
	
void error_handler(void);
bool is_debug_session(void);
	
/*--------------------------------------------------------------
                       PUBLIC DATA
--------------------------------------------------------------*/

/** Common error codes */
typedef enum{
	PFC_ERROR_DATA 		= -3,  /**< Wrong input arguments */
	PFC_ERROR_HAL 		= -2,  /**< Error at the lower HAL level */
	PFC_ERROR_GENERIC = -1,  /**< Generic error code */
	PFC_NULL	 				=  0,	 /**< Successfull execution, but no data provided */
	PFC_SUCCESS 			=  1,	 /**< Successfull execution */
	PFC_WARNING 			=  2,	 /**< Execution ok, but requires attention */
}status_t;


#endif /* _DEBUG_H */

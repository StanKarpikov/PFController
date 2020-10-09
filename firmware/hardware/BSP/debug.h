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

#define UNUSED(X) (void)X /**< Unused variable */

/*--------------------------------------------------------------
                       PUBLIC MACRO
--------------------------------------------------------------*/

/** Check an argument for a NULL value */
#define ARGUMENT_ASSERT(ARGUMENT)                 \
    do                                            \
    {                                             \
        if (ARGUMENT == 0) return PFC_ERROR_DATA; \
    } while (0)

/** Set a software breakpoint */
#define BREAKPOINT()            \
    do                          \
    {                           \
        if (is_debug_session()) \
        {                       \
            __asm {BKPT 0}      \
        }                       \
    } while (0)

#define ENTER_CRITICAL() __disable_irq() /**< Enter a critical section (disable interrupts) */
#define EXIT_CRITICAL()  __enable_irq()  /**< Exit a critical section (enable interrupts) */

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

/**
 * @brief  This function is executed in case of error occurrence.
 */
void error_handler(void);

/**
 * @brief Check if the debug session is enabled (a debugger is connected)
 *
 * @retval true A debug session is enabled
 * @retval false A debug session is disabled
 */
bool is_debug_session(void);

/*--------------------------------------------------------------
                       PUBLIC DATA
--------------------------------------------------------------*/

/** Common error codes */
typedef enum
{
    PFC_ERROR_DATA = -3,    /**< Wrong input arguments */
    PFC_ERROR_HAL = -2,     /**< Error at the lower HAL level */
    PFC_ERROR_GENERIC = -1, /**< Generic error code */
    PFC_NULL = 0,           /**< Successfull execution, but no data provided */
    PFC_SUCCESS = 1,        /**< Successfull execution */
    PFC_WARNING = 2,        /**< Execution ok, but requires attention */
} status_t;

#endif /* _DEBUG_H */

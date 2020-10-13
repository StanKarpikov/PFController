/**
 * @file eeprom_emulation.c
 * @author Stanislav Karpikov
 * @brief Emulate EEPROM in flash memory
 */

/** @addtogroup mdw_eeprom_emulation
 * @{
 */
 

/** NOTE: See http://www.st.com/web/en/resource/technical/document/application_note/CD00165693.pdf */

/*--------------------------------------------------------------
                       INCLUDES
--------------------------------------------------------------*/

#include "eeprom_emulation.h"
#include "BSP/bsp.h"
#include "BSP/system.h"
#include "stm32f7xx_hal.h"

/*--------------------------------------------------------------
                       DEFINES
--------------------------------------------------------------*/

#define __IO volatile

#define EEPROM_PAGE0_BASE ((uint32_t)(EEPROM_START_ADDRESS))                         /**< EEPROM memory storage: the first page address */
#define EEPROM_PAGE1_BASE ((uint32_t)(EEPROM_START_ADDRESS + EEPROM_PAGE_FULL_SIZE)) /**< EEPROM memory storage: the second page address */

#define EEPROM_DEFAULT_DATA (0xFFFF) /**< Default data. Should be 0xFFFF due to the flash technology limitations */

/*--------------------------------------------------------------
                       PRIVATE DATA
--------------------------------------------------------------*/

static uint32_t page_base_first = EEPROM_PAGE0_BASE;    /**< The first page address */
static uint32_t page_base_second = EEPROM_PAGE1_BASE;   /**< The second page address */
static uint32_t page_size = EEPROM_PAGE_SIZE;           /**< The size of a page */
static eeprom_status_t eeprom_status = EEPROM_NOT_INIT; /**< The EEPROM module status */

/*--------------------------------------------------------------
                       PRIVATE TYPES
--------------------------------------------------------------*/

/** The status of the page that is written in the base page address */
typedef enum
{
    EEPROM_ERASED = ((uint16_t)0xFFFF),       /**< The page is erased and can be used as a storage after initialisation */
    EEPROM_RECEIVE_DATA = ((uint16_t)0xEEEE), /**< The page is receiving the data */
    EEPROM_VALID_PAGE = ((uint16_t)0x0000)    /**< The page is valid and can be used */
} eeprom_page_state_t;

/*--------------------------------------------------------------
                       PRIVATE FUNCTIONS
--------------------------------------------------------------*/

/**
 * @brief Connection to the flash adapter: erate a page
 *
 * @param page_base The address of the memory range to erase
 * 
 * @return The status of the operation
 */
static eeprom_status_t eeprom_adapter_erase(uint32_t page_base)
{
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t page_error = 0;
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.NbSectors = 1;
    EraseInitStruct.Sector = (page_base == EEPROM_PAGE0_BASE) ? EEPROM_PAGE0_SECTOR : EEPROM_PAGE1_SECTOR;
    EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;

    ENTER_CRITICAL();
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &page_error) != HAL_OK)
    {
        //EEPROM_LOG( "HAL_FLASHEx_Erase Err: %X, %X\n",HAL_FLASH_GetError(),page_error);
        EXIT_CRITICAL();
        return EEPROM_BAD_FLASH;
    }
    EXIT_CRITICAL();
    return EEPROM_OK;
}

/**
 * @brief Connection to the flash adapter: program a half word (16 bit)
 *
 * @param page_base The address of the half word
 * @param data The data to be wriiten
 * 
 * @return The status of the operation
 */
static eeprom_status_t eeprom_adapter_program_halfword(uint32_t page_base, uint32_t data)
{
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

    ENTER_CRITICAL();
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, page_base, data) != HAL_OK)
    {
        //EEPROM_LOG("Write error at 0x%08X\n",page_base);
        EXIT_CRITICAL();
        return EEPROM_BAD_FLASH;
    }
    EXIT_CRITICAL();
    return EEPROM_OK;
}

/**
 * @brief Unlock the flash memory
 *
 * @return The status of the operation
 */
static eeprom_status_t eeprom_flash_unlock(void)
{
    HAL_FLASH_Unlock();
    return EEPROM_OK;
}

/**
 * @brief Lock the flash memory
 *
 * @return The status of the operation
 */
static eeprom_status_t eeprom_flash_lock(void)
{
    HAL_FLASH_Lock();
    return EEPROM_OK;
}

/**
  * @brief  Check if a page is blank
  *
  * @param  page_base The address of the page
  *
  * @return The status of the page
  *	@retval -1 The page not empty after being erased
  *	@retval 0 The page is blank
  */
static int eeprom_check_page(uint32_t page_base, uint16_t status)
{
    uint32_t page_end = page_base + (uint32_t)EEPROM_PAGE_SIZE;

    // Page eeprom_status not EEPROM_ERASED and not a "state"
    if ((*(__IO uint16_t *)page_base) != EEPROM_ERASED && (*(__IO uint16_t *)page_base) != status)
        return -1;
    for (page_base += 4; page_base < page_end; page_base += 4)
        if ((*(__IO uint32_t *)page_base) != 0xFFFFFFFF)  // Verify if slot is empty
            return -1;
    return 0;
}

/**
  * @brief  Erase page and increment the erase counter (address: page + 2)
  *
  * @param  page_base The address of the page
  * 
  * @return The status of the operation
  */
static eeprom_status_t eeprom_erase_page(uint32_t page_base)
{
    eeprom_status_t flash_status;
    uint16_t data = (*(__IO uint16_t *)(page_base));
    if ((data == EEPROM_ERASED) || (data == EEPROM_VALID_PAGE) || (data == EEPROM_RECEIVE_DATA))
        data = (*(__IO uint16_t *)(page_base + 2)) + 1;
    else
        data = 0;

    flash_status = eeprom_adapter_erase(page_base);
    if (flash_status == 0)
        flash_status = eeprom_adapter_program_halfword(page_base + 2, data);

    return flash_status;
}

/**
  * @brief  Check if a page is blank and erase the page
  *
  * @param  page_base The address of the page
  *
  * @return The status of the operation. Flash error code on write Flash error
  *	@retval EEPROM_BAD_FLASH	The page not empty after erase
  *	@retval EEPROM_OK		The page is blank
  */
static eeprom_status_t eeprom_check_erase_page(uint32_t page_base, uint16_t status)
{
    eeprom_status_t flash_status;
    if (eeprom_check_page(page_base, status) != 0)
    {
        flash_status = eeprom_erase_page(page_base);
        if (flash_status != EEPROM_OK)
            return flash_status;
        return (eeprom_status_t)eeprom_check_page(page_base, status);
    }
    return EEPROM_OK;
}

/**
  * @brief  Find a valid page for write or read operation
  *
  * @return Valid page address (PAGE0 or PAGE1) or NULL in case of no valid page was found
  */
static uint32_t eeprom_find_valid_page(void)
{
    uint16_t status0 = (*(__IO uint16_t *)page_base_first);   // Get Page0 actual status
    uint16_t status1 = (*(__IO uint16_t *)page_base_second);  // Get Page1 actual status

    if (status0 == EEPROM_VALID_PAGE && status1 == EEPROM_ERASED)
        return page_base_first;
    if (status1 == EEPROM_VALID_PAGE && status0 == EEPROM_ERASED)
        return page_base_second;

    return 0;
}

/**
  * @brief  Calculate the count of unique variables in the EEPROM memory
  *
	* @param  page_base The address of the first slot to check (address: page + 4)
  * @param	address_to_skip A 16 bit virtual address of the variable to excluse (or 0XFFFF)
  *
  * @return The count of variables
  */
static uint16_t eeprom_get_variables_count(uint32_t page_base, uint16_t address_to_skip)
{
    uint16_t variable_address, next_address;
    uint32_t idx;
    uint32_t page_end = page_base + (uint32_t)EEPROM_PAGE_SIZE;
    uint16_t count = 0;

    for (page_base += 6; page_base < page_end; page_base += 4)
    {
        variable_address = (*(__IO uint16_t *)page_base);
        if (variable_address == 0xFFFF || variable_address == address_to_skip)
            continue;

        count++;
        for (idx = page_base + 4; idx < page_end; idx += 4)
        {
            next_address = (*(__IO uint16_t *)idx);
            if (next_address == variable_address)
            {
                count--;
                break;
            }
        }
    }
    return count;
}

/**
  * @brief  Transfer the last updated variables data from a full page to an empty one.
  *
  * @param  new_page The new page base address
  * @param	old_page The old page base address
  *	@param	address_to_skip A 16 bit virtual address of the variable (or 0xFFFF)
  *
  * @return The status of the operation, Flash error code on write Flash error
  * @retval EEPROM_OK On success
  * @retval EEPROM_OUT_SIZE If valid new page is full
  */
static eeprom_status_t eeprom_transfer_page(uint32_t new_page, uint32_t old_page, uint16_t address_to_skip)
{
    uint32_t old_end, new_end;
    uint32_t old_index, new_index, idx;
    uint16_t address, data, found;
    eeprom_status_t flash_status;

    // Transfer process: transfer variables from old to the new active page
    new_end = new_page + ((uint32_t)EEPROM_PAGE_FULL_SIZE);

    // Find first free element in new page
    for (new_index = new_page + 4; new_index < new_end; new_index += 4)
        if ((*(__IO uint32_t *)new_index) == 0xFFFFFFFF)  // Verify if element
            break;                                        //  contents are 0xFFFFFFFF
    if (new_index >= new_end)
        return EEPROM_OUT_SIZE;

    old_end = old_page + 4;
    old_index = old_page + (uint32_t)(EEPROM_PAGE_SIZE - 2);

    for (; old_index > old_end; old_index -= 4)
    {
        address = *(__IO uint16_t *)old_index;
        if (address == 0xFFFF || address == address_to_skip)
            continue;  // it's means that power off after write data

        found = 0;
        for (idx = new_page + 6; idx < new_index; idx += 4)
            if ((*(__IO uint16_t *)(idx)) == address)
            {
                found = 1;
                break;
            }

        if (found)
            continue;

        if (new_index < new_end)
        {
            data = (*(__IO uint16_t *)(old_index - 2));

            flash_status = eeprom_adapter_program_halfword(new_index, data);
            if (flash_status != EEPROM_OK)
                return flash_status;

            flash_status = eeprom_adapter_program_halfword(new_index + 2, address);
            if (flash_status != EEPROM_OK)
                return flash_status;

            new_index += 4;
        }
        else
            return EEPROM_OUT_SIZE;
    }

    // Erase the old Page: Set old Page status to EEPROM_EEPROM_ERASED status
    data = eeprom_check_erase_page(old_page, EEPROM_ERASED);
    if (data != EEPROM_OK)
        return (eeprom_status_t)data;

    // Set new Page status
    flash_status = eeprom_adapter_program_halfword(new_page, EEPROM_VALID_PAGE);
    if (flash_status != EEPROM_OK)
        return flash_status;

    return EEPROM_OK;
}

/**
  * @brief  Format the storage: erase PAGE0 and PAGE1 and write the EEPROM_VALID_PAGE / 0 header to PAGE0
  *
  * @retval The status of the last operation (Flash write or erase) during EEPROM formating
  */
static eeprom_status_t eeprom_format_storage(void)
{
    eeprom_status_t status;
    eeprom_status_t flash_status;

    // Erase Page0
    status = eeprom_check_erase_page(page_base_first, EEPROM_VALID_PAGE);
    if (status != EEPROM_OK)
        return status;
    if ((*(__IO uint16_t *)page_base_first) == EEPROM_ERASED)
    {
        // Set Page0 as valid page: Write VALID_PAGE at Page0 base address
        flash_status = eeprom_adapter_program_halfword(page_base_first, EEPROM_VALID_PAGE);
        if (flash_status != EEPROM_OK)
            return flash_status;
    }
    // Erase Page1
    status = eeprom_check_erase_page(page_base_second, EEPROM_ERASED);
    return status;
}

/**
  * @brief  Verify if active page is full and Writes variable in EEPROM.
  *
  * @param  address: 16 bit virtual address of the variable
  * @param  data: 16 bit data to be written as variable value
  *
  * @return The status of the operation, Flash error code: on write Flash error
  * @retval EEPROM_OK on success
  * @retval EEPROM_PAGE_FULL if valid page is full (need page transfer)
  * @retval EEPROM_NO_VALID_PAGE if no valid page was found
  * @retval EEPROM_OUT_SIZE if EEPROM size exceeded
  */
static eeprom_status_t eeprom_verify_page_full_write_variable(uint16_t address, uint16_t data)
{
    eeprom_status_t flash_status;
    uint32_t idx, page_base, page_end, new_page;
    uint16_t count;

    // Get valid Page for write operation
    page_base = eeprom_find_valid_page();
    if (page_base == 0)
        return EEPROM_NO_VALID_PAGE;

    // Get the valid Page end address
    page_end = page_base + page_size;  // Set end of page

    for (idx = page_end - 2; idx > page_base; idx -= 4)
    {
        if ((*(__IO uint16_t *)idx) == address)  // Find last value for address
        {
            count = (*(__IO uint16_t *)(idx - 2));  // Read last data
            if (count == data)
                return EEPROM_OK;
            if (count == 0xFFFF)
            {
                flash_status = eeprom_adapter_program_halfword(idx - 2, data);  // Set variable data
                if (flash_status == EEPROM_OK)
                    return EEPROM_OK;
            }
            break;
        }
    }

    // Check each active page address starting from begining
    for (idx = page_base + 4; idx < page_end; idx += 4)
        if ((*(__IO uint32_t *)idx) == 0xFFFFFFFF)                      // Verify if element
        {                                                               //  contents are 0xFFFFFFFF
            flash_status = eeprom_adapter_program_halfword(idx, data);  // Set variable data
            if (flash_status != EEPROM_OK)
                return flash_status;
            flash_status = eeprom_adapter_program_halfword(idx + 2, address);  // Set variable virtual address
            if (flash_status != EEPROM_OK)
                return flash_status;
            return EEPROM_OK;
        }

    // Empty slot not found, need page transfer
    // Calculate unique variables in page
    count = eeprom_get_variables_count(page_base, address) + 1;
    if (count >= (page_size / 4 - 1))
        return EEPROM_OUT_SIZE;

    if (page_base == page_base_second)
        new_page = page_base_first;  // New page address where variable will be moved to
    else
        new_page = page_base_second;

    // Set the new Page status to RECEIVE_DATA status
    flash_status = eeprom_adapter_program_halfword(new_page, EEPROM_RECEIVE_DATA);
    if (flash_status != EEPROM_OK)
        return flash_status;

    // Write the variable passed as parameter in the new active page
    flash_status = eeprom_adapter_program_halfword(new_page + 4, data);
    if (flash_status != EEPROM_OK)
        return flash_status;

    flash_status = eeprom_adapter_program_halfword(new_page + 6, address);
    if (flash_status != EEPROM_OK)
        return flash_status;

    return eeprom_transfer_page(new_page, page_base, address);
}

/**
  * @brief  Writes/updates the variable data in EEPROM
  *
  * @param  address The variable virtual address
  * @param  data A 16 bit data to be written
  *
  * @return The status of the operation, or Flash error code on write Flash error
  *	@retval EEPROM_OK on success
  *	@retval EEPROM_BAD_ADDRESS if address = 0xFFFF
  *	@retval EEPROM_PAGE_FULL if valid page is full
  *	@retval EEPROM_NO_VALID_PAGE if no valid page was found
  *	@retval EEPROM_OUT_SIZE if no empty EEPROM variables
  */
static eeprom_status_t eeprom_write(uint16_t address, uint16_t data)
{
    if (eeprom_status == EEPROM_NOT_INIT)
        if (eeprom_init() != EEPROM_OK)
            return eeprom_status;

    if (address == 0xFFFF)
        return EEPROM_BAD_ADDRESS;

    eeprom_flash_unlock();

    // Write the variable virtual address and value in the EEPROM
    eeprom_status_t status = eeprom_verify_page_full_write_variable(address, data);

    if (status == EEPROM_NO_VALID_PAGE)
    {
        eeprom_adapter_erase(EEPROM_PAGE0_BASE);
        eeprom_adapter_erase(EEPROM_PAGE1_BASE);
        status = eeprom_verify_page_full_write_variable(address, data);
    }

    eeprom_flash_lock();

    return status;
}

/*--------------------------------------------------------------
                       PUBLIC FUNCTIONS
--------------------------------------------------------------*/

/*
  * @brief  Init the EEPROM module
  *
  * @return The status of the operation
  */
eeprom_status_t eeprom_init(void)
{
    eeprom_page_state_t status0, status1;
    eeprom_status_t flash_status;

    eeprom_flash_unlock();
    eeprom_status = EEPROM_NO_VALID_PAGE;

    status0 = (eeprom_page_state_t)(*(__IO uint16_t *)page_base_first);
    status1 = (eeprom_page_state_t)(*(__IO uint16_t *)page_base_second);

    switch (status0)
    {
            /*
		Page0				Page1
		-----				-----
	EEPROM_ERASED		EEPROM_VALID_PAGE			Page1 valid, Page0 erased
						EEPROM_RECEIVE_DATA			Page1 need set to valid, Page0 erased
						EEPROM_ERASED				make EE_Format
						any							Error: EEPROM_NO_VALID_PAGE
*/
        case EEPROM_ERASED:
            if (status1 == EEPROM_VALID_PAGE)  // Page0 erased, Page1 valid
                eeprom_status = eeprom_check_erase_page(page_base_first, EEPROM_ERASED);
            else if (status1 == EEPROM_RECEIVE_DATA)  // Page0 erased, Page1 receive
            {
                flash_status = eeprom_adapter_program_halfword(page_base_second, EEPROM_VALID_PAGE);
                if (flash_status != EEPROM_OK)
                    eeprom_status = flash_status;
                else
                    eeprom_status = eeprom_check_erase_page(page_base_first, EEPROM_ERASED);
            }
            else if (status1 == EEPROM_ERASED)  // Both in erased state so format EEPROM
                eeprom_status = eeprom_format_storage();
            break;
            /*
		Page0				Page1
		-----				-----
	EEPROM_RECEIVE_DATA	EEPROM_VALID_PAGE			Transfer Page1 to Page0
						EEPROM_ERASED				Page0 need set to valid, Page1 erased
						any							EEPROM_NO_VALID_PAGE
*/
        case EEPROM_RECEIVE_DATA:
            if (status1 == EEPROM_VALID_PAGE)  // Page0 receive, Page1 valid
                eeprom_status = eeprom_transfer_page(page_base_first, page_base_second, 0xFFFF);
            else if (status1 == EEPROM_ERASED)  // Page0 receive, Page1 erased
            {
                eeprom_status = eeprom_check_erase_page(page_base_second, EEPROM_ERASED);
                if (eeprom_status == EEPROM_OK)
                {
                    flash_status = eeprom_adapter_program_halfword(page_base_first, EEPROM_VALID_PAGE);
                    if (flash_status != EEPROM_OK)
                        eeprom_status = flash_status;
                    else
                        eeprom_status = EEPROM_OK;
                }
            }
            break;
            /*
		Page0				Page1
		-----				-----
	EEPROM_VALID_PAGE	EEPROM_VALID_PAGE			Error: EEPROM_NO_VALID_PAGE
						EEPROM_RECEIVE_DATA			Transfer Page0 to Page1
						any							Page0 valid, Page1 erased
*/
        case EEPROM_VALID_PAGE:
            if (status1 == EEPROM_VALID_PAGE)  // Both pages valid
                eeprom_status = EEPROM_NO_VALID_PAGE;
            else if (status1 == EEPROM_RECEIVE_DATA)
                eeprom_status = eeprom_transfer_page(page_base_second, page_base_first, 0xFFFF);
            else
                eeprom_status = eeprom_check_erase_page(page_base_second, EEPROM_ERASED);
            break;
            /*
		Page0				Page1
		-----				-----
		any				EEPROM_VALID_PAGE			Page1 valid, Page0 erased
						EEPROM_RECEIVE_DATA			Page1 valid, Page0 erased
						any							EEPROM_NO_VALID_PAGE
*/
        default:
            if (status1 == EEPROM_VALID_PAGE)
                eeprom_status = eeprom_check_erase_page(page_base_first, EEPROM_ERASED);  // Check/Erase Page0
            else if (status1 == EEPROM_RECEIVE_DATA)
            {
                flash_status = eeprom_adapter_program_halfword(page_base_second, EEPROM_VALID_PAGE);
                if (flash_status != EEPROM_OK)
                    eeprom_status = flash_status;
                else
                    eeprom_status = eeprom_check_erase_page(page_base_first, EEPROM_ERASED);
            }
            else
            {
                eeprom_format_storage();
            }
            break;
    }
    eeprom_flash_lock();
    return eeprom_status;
}

/**
  * @brief	Returns the last stored variable data, if found,
  *			which correspond to the passed virtual address
  *
  * @param  address The variable virtual address
  * @param  data A pointer to data variable
  *
  * @return The status of the operation
  * @retval EEPROM_OK if variable was found
  * @retval EEPROM_BAD_ADDRESS if the variable was not found
  * @retval EEPROM_NO_VALID_PAGE if no valid page was found.
  */
eeprom_status_t eeprom_read_variable(uint16_t address, uint16_t *data)
{
    uint32_t page_base, page_end;

    // Set default data (empty EEPROM)
    *data = EEPROM_DEFAULT_DATA;

    if (eeprom_status == EEPROM_NOT_INIT)
        if (eeprom_init() != EEPROM_OK)
            return eeprom_status;

    // Get active Page for read operation
    page_base = eeprom_find_valid_page();
    if (page_base == 0)
        return EEPROM_NO_VALID_PAGE;

    // Get the valid Page end address
    page_end = page_base + ((uint32_t)(page_size - 2));

    // Check each active page address starting from end
    for (page_base += 6; page_end >= page_base; page_end -= 4)
        if ((*(__IO uint16_t *)page_end) == address)  // Compare the read address with the virtual address
        {
            *data = (*(__IO uint16_t *)(page_end - 2));  // Get content of address-2 which is variable value
            return EEPROM_OK;
        }

    // Return ReadStatus value: (0: variable exist, 1: variable doesn't exist)
    return EEPROM_BAD_ADDRESS;
}

/*
  * @brief  Writes/upadtes the variable data in EEPROM.
            The value is written only if differs from the one already saved at the same address
  *
  * @param  address The variable virtual address
  * @param  data A 16 bit data to be written
  *
  * @return The status of the operation, Flash error code: on write Flash error
  *	@retval EEPROM_SAME_VALUE If new data matches existing EEPROM data
  *	@retval EEPROM_OK on success
  *	@retval EEPROM_BAD_ADDRESS if address = 0xFFFF
  *	@retval EEPROM_PAGE_FULL if valid page is full
  *	@retval EEPROM_NO_VALID_PAGE if no valid page was found
  *	@retval EEPROM_OUT_SIZE if no empty EEPROM slots
  */
eeprom_status_t eeprom_update_variable(uint16_t address, uint16_t data)
{
    uint16_t curent_data = 0;
    eeprom_status_t status = eeprom_read_variable(address, &curent_data);
    if (status != EEPROM_BAD_ADDRESS)
    {
        if (status != EEPROM_OK)
        {
            return status;
        }
        if (curent_data == data)
        {
            return EEPROM_SAME_VALUE;
        }
    }
    return eeprom_write(address, data);
}
/** @} */

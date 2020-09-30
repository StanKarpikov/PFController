#ifndef __EEPROME_H
#define __EEPROME_H
#include "defines.h"
//===================================================
#define MAGIC_2BYTE (0x55AA)
//===================================================
#define EEPROM_ERASED 			((uint16_t)0xFFFF)
#define EEPROM_RECEIVE_DATA ((uint16_t)0xEEEE)
#define EEPROM_VALID_PAGE 	((uint16_t)0x0000)

#define EEPROM_PAGE_SIZE 				((uint32_t)8*1024)//8 Kbytes
#define EEPROM_PAGE_FULL_SIZE 	((uint32_t)256*1024)//256 Kbytes
#define EEPROM_START_ADDRESS		((uint32_t)(0x08180000))

/* Pages 0 and 1 base and end addresses */
#define EEPROM_PAGE0_BASE		((uint32_t)(EEPROM_START_ADDRESS))
#define EEPROM_PAGE1_BASE		((uint32_t)(EEPROM_START_ADDRESS + EEPROM_PAGE_FULL_SIZE))

#define EEPROM_PAGE0_SECTOR		(FLASH_SECTOR_12)
#define EEPROM_PAGE1_SECTOR		(FLASH_SECTOR_13)

/* Page status definitions */
#define EEPROM_ERASED					((uint16_t)0xFFFF)	/* PAGE is empty */
#define EEPROM_RECEIVE_DATA		((uint16_t)0xEEEE)	/* PAGE is marked to receive data */
#define EEPROM_VALID_PAGE			((uint16_t)0x0000)	/* PAGE containing valid data */

/* Page full define */

#define EEPROM_OK				 				((uint16_t)0x0000)
#define EEPROM_OUT_SIZE					((uint16_t)0x0081)
#define EEPROM_BAD_ADDRESS		 	((uint16_t)0x0082)
#define EEPROM_BAD_FLASH		 		((uint16_t)0x0083)
#define EEPROM_NOT_INIT			 		((uint16_t)0x0084)
#define EEPROM_SAME_VALUE		 		((uint16_t)0x0085)
#define EEPROM_NO_VALID_PAGE	 	((uint16_t)0x00AB)

#define EEPROM_DEFAULT_DATA			(0xFFFF)

	void EEPROMClass(void);

	int EEinitA(void);
	int EEinitB(uint32_t, uint32_t, uint32_t);

	uint16_t EEformat(void);

	uint16_t EEerases(uint16_t *);
	uint16_t EEreadA (uint16_t address);
	uint16_t EEreadB (uint16_t address, uint16_t *data);
	uint16_t EEwrite(uint16_t address, uint16_t data);
	uint16_t EEupdate(uint16_t address, uint16_t data);
	uint16_t EEcount(uint16_t *);
	uint16_t EEmaxcount(void);

	extern uint32_t PageBase0;
	extern uint32_t PageBase1;
	extern uint32_t PageSize;

	int EE_ErasePage(uint32_t);

	int EE_CheckPage(uint32_t, uint16_t);
	int EE_CheckErasePage(uint32_t, uint16_t);
	uint16_t EE_Format(void);
	uint32_t EE_FindValidPage(void);
	uint16_t EE_GetVariablesCount(uint32_t, uint16_t);
	int EE_PageTransfer(uint32_t, uint32_t, uint16_t);
	int EE_VerifyPageFullWriteVariable(uint16_t, uint16_t);

#endif

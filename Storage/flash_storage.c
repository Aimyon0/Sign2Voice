/**
 * flash_storage.c — STM32H743 Flash Config Storage
 *
 * STM32H743 is DUAL-BANK (stm32h743xx.h defines DUAL_BANK).
 * Bank 2 Sector 7: 0x081E0000 - 0x081FFFFF (128KB).
 * Bank 1: 0x08000000-0x080FFFFF, Bank 2: 0x08100000-0x081FFFFF.
 *
 * Scatter file ER_IROM1 is 0x08000000 0x001E0000 (excludes this sector).
 *
 * flash_storage_write() runs from SRAM (.RAM_D2 section).
 * STM32H7 cannot fetch instructions from Flash while erasing/programming it.
 */
#include "flash_storage.h"
#include "stm32h7xx_hal.h"
#include <string.h>

/* ---- Bank 2 Sector 7 ---- */
#define SECTOR_ADDR  0x081E0000U
#define SECTOR_BANK  FLASH_BANK_2   /* DUAL_BANK: 0x081E0000 is Bank 2 */
#define SECTOR_NUM   FLASH_SECTOR_7
#define MAX_SIZE     256U

/* ---- Write (must execute from SRAM) ---- */
__attribute__((section(".RAM_D2")))
ErrorCode flash_storage_write(const void *data, uint32_t size)
{
    if (!data || size == 0 || size > MAX_SIZE)
        return ERR_INVALID_PARAM;

    /* Copy to 32-byte-aligned SRAM buffer.
     * HAL_FLASH_Program reads 8 words (32 bytes) from DataAddress.
     * Source MUST be in SRAM — H7 stalls flash reads during write. */
    __attribute__((aligned(32))) uint8_t buf[MAX_SIZE];
    memcpy(buf, data, size);

    /* Ensure buffer is flushed to physical SRAM before flash op */
    SCB_CleanDCache_by_Addr((uint32_t *)buf, (size + 31) & ~31U);

    __disable_irq();

    /* Set programming delay for high-frequency operation (400MHz HCLK) */
    __HAL_FLASH_SET_PROGRAM_DELAY(FLASH_BANK_2);

    if (HAL_FLASH_Unlock() != HAL_OK) {
        __enable_irq();
        return ERR_FLASH;
    }

    /* ---- Erase Bank 2 Sector 7 ---- */
    FLASH_EraseInitTypeDef erase = {0};
    uint32_t err_sector = 0;
    erase.TypeErase    = FLASH_TYPEERASE_SECTORS;
    erase.Banks        = SECTOR_BANK;     /* FLASH_BANK_2 */
    erase.Sector       = SECTOR_NUM;      /* FLASH_SECTOR_7 */
    erase.NbSectors    = 1;
    erase.VoltageRange = FLASH_VOLTAGE_RANGE_3;

    if (HAL_FLASHEx_Erase(&erase, &err_sector) != HAL_OK
        || err_sector != 0xFFFFFFFFU) {
        HAL_FLASH_Lock();
        __enable_irq();
        return ERR_FLASH;
    }

    /* ---- Program flashwords (32 bytes each) ---- */
    uint32_t addr = SECTOR_ADDR;
    for (uint32_t i = 0; i < (size + 31) / 32; i++) {
        uint32_t src = (uint32_t)&buf[i * 32];
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD,
                              addr, src) != HAL_OK) {
            HAL_FLASH_Lock();
            __enable_irq();
            return ERR_FLASH;
        }
        addr += 32;
    }

    HAL_FLASH_Lock();
    __enable_irq();
    return ERR_OK;
}

/* ---- Read (simple memory-mapped access) ---- */
ErrorCode flash_storage_read(void *data, uint32_t size)
{
    if (!data || size == 0 || size > MAX_SIZE)
        return ERR_INVALID_PARAM;
    memcpy(data, (const void *)SECTOR_ADDR, size);
    return ERR_OK;
}

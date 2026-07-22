#ifndef FLASH_STORAGE_H
#define FLASH_STORAGE_H
#include <stdint.h>
#include "error_code.h"
ErrorCode flash_storage_write(const void *data, uint32_t size);
ErrorCode flash_storage_read(void *data, uint32_t size);
#endif

#ifndef ERROR_CODE_H
#define ERROR_CODE_H
typedef enum {
    ERR_OK = 0, ERR_CAMERA = -1, ERR_AI = -2, ERR_AUDIO = -3,
    ERR_LCD = -4, ERR_FLASH = -5, ERR_TIMEOUT = -6,
    ERR_INVALID_PARAM = -7, ERR_NULL_POINTER = -8, ERR_BUSY = -9,
} ErrorCode;
#endif

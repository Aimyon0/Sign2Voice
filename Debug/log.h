#ifndef LOG_H
#define LOG_H
#include <stdio.h>
#define LOG_LEVEL_NONE  0
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_WARN  2
#define LOG_LEVEL_INFO  3
#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_NONE
#endif
#if LOG_LEVEL >= 1
#define LOG_ERROR(fmt, ...) printf("[ERR] " fmt "\r\n", ##__VA_ARGS__)
#else
#define LOG_ERROR(fmt, ...) ((void)0)
#endif
#if LOG_LEVEL >= 2
#define LOG_WARN(fmt, ...)  printf("[WARN] " fmt "\r\n", ##__VA_ARGS__)
#else
#define LOG_WARN(fmt, ...)  ((void)0)
#endif
#if LOG_LEVEL >= 3
#define LOG_INFO(fmt, ...)  printf("[INFO] " fmt "\r\n", ##__VA_ARGS__)
#else
#define LOG_INFO(fmt, ...)  ((void)0)
#endif
#define LOG_RAW(fmt, ...)   printf(fmt "\r\n", ##__VA_ARGS__)
void log_init(void);
#endif

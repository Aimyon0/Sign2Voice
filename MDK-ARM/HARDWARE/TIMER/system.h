#ifndef __SYSTEM_H
#define __SYSTEM_H

#include "stm32h7xx.h"
#include "core_cm7.h"
#include "stm32h7xx_hal.h"

// 0 不支持 OS，1 支持 OS
#define SYSTEM_SUPPORT_OS 0

///////////////////////////////////////////////////////////////////////////////////
// 简化数据类型定义（如确有需要）
typedef int32_t   s32;
typedef int16_t   s16;
typedef int8_t    s8;
typedef uint32_t  u32;
typedef uint16_t  u16;
typedef uint8_t   u8;

typedef const int32_t  sc32;
typedef const int16_t  sc16;
typedef const int8_t   sc8;
typedef const uint32_t uc32;
typedef const uint16_t uc16;
typedef const uint8_t  uc8;

typedef __IO int32_t  vs32;
typedef __IO int16_t  vs16;
typedef __IO int8_t   vs8;
typedef __IO uint32_t vu32;
typedef __IO uint16_t vu16;
typedef __IO uint8_t  vu8;

typedef __I int32_t  vsc32;
typedef __I int16_t  vsc16;
typedef __I int8_t   vsc8;
typedef __I uint32_t vuc32;
typedef __I uint16_t vuc16;
typedef __I uint8_t  vuc8;

// 常用宏定义
#define ON  1
#define OFF 0

// Cache透写模式（将SCB->CACR寄存器的Bit[2]置1）
#define WRITE_THROUGH() (SCB->CACR |= (1UL << 2))

// 函数声明
u8 Get_ICahceSta(void);  // 判断 I-Cache 是否开启
u8 Get_DCahceSta(void);  // 判断 D-Cache 是否开启

#endif /* __SYSTEM_H */




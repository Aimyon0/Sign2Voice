
#include "system.h"

void assert_failed(uint8_t* file, uint32_t line)
{ 
	while (1)
	{
	}
}
u8 Get_ICahceSta(void)
{
    u8 sta;
    sta=((SCB->CCR)>>17)&0X01;
    return sta;
}

u8 Get_DCahceSta(void)
{
    u8 sta;
    sta=((SCB->CCR)>>16)&0X01;
    return sta;
}

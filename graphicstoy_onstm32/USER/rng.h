#ifndef _RNG_H_
#define _RNG_H_
#include "stm32f4xx_rng.h"
#include "stm32f4xx.h"

void RNG_Init();
uint32_t RNG_GetRandomInRange(uint32_t min,uint32_t max);
#endif

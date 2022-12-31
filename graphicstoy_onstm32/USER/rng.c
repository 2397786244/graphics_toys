#include "rng.h"
#include "delay.h"

void RNG_Init(){
	u16 i;
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG,ENABLE);
	RNG_Cmd(ENABLE);
}
uint32_t RNG_GetRandomInRange(uint32_t min,uint32_t max){
	return min + RNG_GetRandomNumber()  % (max - min + 1);
}
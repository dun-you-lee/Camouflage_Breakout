#include "stm32f4xx.h"
//#include "user_defined.h"	// for delay_ms(wait_ms)
void delay_ms(uint32_t wait_ms);
//////////////////////////////////////////////////////////////////////////////////	 
 
//RNG (random generator)	   
//STM32F4
//  * It delivers 32-bit random numbers, produced by an analog generator
//  * 40 periods (0.84us) of the PLL48CLK clock signal between two consecutive random numbers
//  * Monitoring of the RNG entropy to flag abnormal behavior (generation of stable values, 
//    or of a stable sequence of values)
//  * It can be disabled to reduce power consumption.
////////////////////////////////////////////////////////////////////////////////// 

//RNG initialization
// return 1: RNG OK
// return 2: RNG Failure
uint8_t RNG_Init(void)
{
	uint16_t retry=0; 
	uint32_t status = 0;
	
  RCC->AHB2ENR |= RCC_AHB2ENR_RNGEN;   //bit 6: ENABLE RNG clock from  PLL48CLK
	while(status==0)	 /* Delay after an RCC peripheral clock enabling */
	{
		status = RCC->AHB2ENR & RCC_AHB2ENR_RNGEN;
	}
	
	status = 0;
	RNG->CR |= RNG_CR_RNGEN;		//1: random Number Generator is enabled.
	
	while((status==0) && retry<1000)	// wait for RNG_FLAG_DRDY=1
	{
		retry++;
		delay_ms(1);
		status = RNG->SR & RNG_SR_DRDY;
	}
	if(retry>=1000) return 2;  // random generator initialization failed
	return 1;					// RNG OK
}

// To get random number
uint32_t RNG_Get_RandomNum(void)
{	 
	while((RNG->SR & RNG_SR_DRDY) == 0);	
	/*
		Once the RNG_DR register has been read, 
		this bit returns to 0 until a new valid value is computed.
	*/
	return RNG->DR;   //DR: RandomNumber register;	
}

/*
As required by the FIPS PUB (Federal Information Processing Standard Publication) 140-2, 
the first random number generated after setting the RNGEN bit should not be used, but 
saved for comparison with the next generated random number. 
//############################################
Each subsequent generated random number has to be compared with the previously generated number. 
The test fails if any two compared numbers are equal (continuous random number generator test).
##############################################
*/
//random number interval: [min,max]
int RNG_Get_RandomRange(int min,int max)
{ 
	static uint32_t last_value;
	uint32_t ranNum;
	
	ranNum = RNG_Get_RandomNum();	
	ranNum %= max-min+1;	// get residual
	if (ranNum == last_value){
		ranNum = RNG_Get_RandomNum();
		ranNum %= max-min+1;	// get residual
	}
	last_value = ranNum;
	return ((int) ranNum + min);
}

//RNG Closing
void RNG_Close(void)
{
	uint32_t status = 1;
	
	RNG->CR &= ~RNG_CR_RNGEN;	// 0: Random number generator is disabled

  RCC->AHB2ENR &= ~RCC_AHB2ENR_RNGEN;   //bit 6: DISABLE RNG clock from  PLL48CLK
	while(!status)	 /* Delay after an RCC peripheral clock enabling */
	{
		status = RCC->AHB2ENR & RCC_AHB2ENR_RNGEN;
	}
}




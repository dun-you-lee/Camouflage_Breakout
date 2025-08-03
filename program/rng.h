#ifndef __RNG_H
#define __RNG_H	 
	
unsigned char  RNG_Init(void);		
unsigned int RNG_Get_RandomNum(void);
int RNG_Get_RandomRange(int min,int max);
void RNG_Close(void);
#endif


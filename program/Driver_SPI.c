#include "stm32f4xx.h"
#include <stdbool.h>

#define pin_0 0
#define pin_1 1
#define pin_2 2
#define pin_8 8
#define pin_9 9
#define pin_10 10
#define pin_13 13
#define pin_14 14

#define bit_GPIOA 0
#define bit_GPIOB 1
#define bit_GPIOF 5

#ifndef Bit
#define Bit(x) 	(0x01ul<<x)
#endif



void Driver_GPIO(void)
{
/*  --------------------------------------------------------------
		GPIOA: INPUT, pull-down
		--------------------------------------------------------------
		| Pin : PA |    0
		| Key 1    |   KEY1
		--------------------------------------------------------------
*/
		RCC->AHB1ENR |=  (1UL << bit_GPIOA);     // enable clock for GPIOA
		// 0:PORTA, 1: PORTB, ..., 10: PORTK
	  //GPIOA->MODER   = (GPIOA->MODER & ~(0x03ul<<(pin_13*2)) );
	  //GPIOA->MODER   = (GPIOA->MODER & ~(0x03ul<<(pin_14*2)) );
		GPIOA->MODER   = (GPIOA->MODER & ~(0x03ul<<(pin_0*2)) );                       //Input mode (00b)
		GPIOA->PUPDR   = (GPIOA->PUPDR & ~(0x03ul<<(pin_0*2)) ) | (0x02ul<<(pin_0*2)); //pull-down (10b)
		GPIOA->AFR[0]  &= ~(0x0Ful<<pin_0*4);                                          // AF0 (0000b)

/*  --------------------------------------------------------------
		GPIOF: INPUT, floating (no Pull-Up or Pull-Down)
		--------------------------------------------------------------
		| Pin : PF        |    10
		| touch Module    |   TP_INT
		--------------------------------------------------------------
*/
		RCC->AHB1ENR |=  (1UL << bit_GPIOF);     // enable clock for GPIOF
		// 0:PORTA, 1: PORTB, ..., 10: PORTK
		GPIOF->MODER   = (GPIOF->MODER & ~(0x03ul<<(pin_10*2)) ); //Input mode (00b)
		GPIOF->PUPDR   = (GPIOF->PUPDR & ~(0x03ul<<(pin_10*2)) ); //no pull-up or pull-down (00b)
		GPIOF->AFR[1]  &= ~(0x0Ful<<(pin_10 - pin_8)*4);          // AF0 (0000b)

/*  --------------------------------------------------------------
    GPIOF: INPUT, floating  (no Pull-Up or Pull-Down) 
    -------------------------------------------------------------
    | Pin : PF         |    8     
    | SPI              |   MISO
    --------------------------------------------------------------
*/
		RCC->AHB1ENR |=  (1UL << bit_GPIOF);     // enable clock for GPIOF
		// 0:PORTA, 1: PORTB, ..., 10: PORTK
		GPIOF->MODER   = (GPIOF->MODER & ~(0x03ul<<(pin_8*2)) ); //Input mode (00b)
		GPIOF->PUPDR   = (GPIOF->PUPDR & ~(0x03ul<<(pin_8*2)) ); //no pull-up or pull-down (00b)
		GPIOF->AFR[1]  &= ~(0x0Ful<<(pin_8 - pin_8)*4);          // AF0 (0000b)
		
/*  --------------------------------------------------------------
    GPIOB: OUTPUT, Speed_25MHz medium, push pull, no pull 
    --------------------------------------------------------------
    | Pin: PB |     1         2   
    | SPI     |    SCK     CS
    | initial |   0 (low)  1 (high)
    --------------------------------------------------------------
*/
		RCC->AHB1ENR |=  (1UL << bit_GPIOB);                      // enable clock for GPIOB
    // 0:PORTA, 1: PORTB, ..., 10: PORTK
		GPIOB->BSRR |= Bit(1) << 16;	// Initial output value for SCK = 0
		GPIOB->BSRR |= Bit(2);		    // Initial output value for CS = 1
		
		GPIOB->MODER   = (GPIOB->MODER   & ~(0x03ul<<(pin_1*2) | 0x03ul<<(pin_2*2)) | (0x01ul<<(pin_1*2) | (0x01ul<<(pin_2*2)))); //Output mode (01b)
		GPIOB->OSPEEDR = (GPIOB->OSPEEDR & ~(0x03ul<<(pin_1*2) | 0x03ul<<(pin_2*2)) | (0x01ul<<(pin_1*2) | (0x01ul<<(pin_2*2)))); //medium speed (01b)
		GPIOB->OTYPER  &= ~(0x01ul<<pin_1 | 0x01ul<<pin_2);                           // push-pull (0b)
		GPIOB->PUPDR   = (GPIOB->PUPDR & ~(0x03ul<<(pin_1*2) | 0x03ul<<(pin_2*2)) );  // no PU, no PD (00b)
		GPIOB->AFR[0]  &= ~(0x0Ful<<pin_1*4 | 0x0Ful<<pin_2*4);                   // AF0 (0000b)

/*  --------------------------------------------------------------
    GPIOF: OUTPUT, Speed_25MHz medium, push pull, no pull 
    --------------------------------------------------------------
    | Pin: PF |    9   
    | SPI     |   MOSI
   --------------------------------------------------------------
*/	
		RCC->AHB1ENR |=  (1UL << bit_GPIOF);     // enable clock for GPIOF
		// 0:PORTA, 1: PORTB, ..., 10: PORTK
		GPIOF->MODER   = (GPIOF->MODER   & ~(0x03ul<<(pin_9*2)) | (0x01ul<<(pin_9*2))); //Output mode (01b)
		GPIOF->OSPEEDR = (GPIOF->OSPEEDR & ~(0x03ul<<(pin_9*2)) | (0x01ul<<(pin_9*2))); //medium speed (01b)
		GPIOF->OTYPER  &= ~(0x01ul<<pin_9);                       // push-pull (0b)
		GPIOF->PUPDR   = (GPIOF->PUPDR & ~(0x03ul<<(pin_9*2)) );  // no PU, no PD (00b)
		GPIOF->AFR[1]  &= ~(0x0Ful<<(pin_9 - pin_8)*4);           // AF0 (0000b)
		
}





#define CS_pin 	Bit(2)	// PB2
#define SCK_pin 	Bit(1)	// PB1
#define MISO_pin 	Bit(8)	// PF8
#define MOSI_pin 	Bit(9)	// PF9
#define GPIO_MI	GPIOF	// PF8
#define GPIO_MO	GPIOF	// PF9
#define GPIO_SCK 	GPIOB	// PB1
#define GPIO_CS 	GPIOB	// PB2

/***********************************************
*  Output Function: Driver_SPICS	(CS pin)
*  Object: touch ic spi enable/unable
*  brief:	set CS = 0 if t_f=0; CS = 1 if t_f = 1.
***********************************************/
void Driver_SPICS(bool t_f)
{
if (!t_f) 	// t_f = 0
  GPIO_CS->BSRR = CS_pin<<16;	// reset ==> 0
else	// t_f = 1
  GPIO_CS->BSRR = CS_pin;		// set ==> 1
}

/***********************************************
*  Output Function: Driver_SPISCK	(SCK pin)
*  Object: touch spi clock output
*  brief:	set SCK = 0 if t_f=0; SCK = 1 if t_f = 1.
***********************************************/
void Driver_SPISCK(bool t_f)
{
    if (t_f) GPIO_SCK->BSRR =  SCK_pin;
    else     GPIO_SCK->BSRR = (SCK_pin << 16);
}



// MOSI ??:t_f=0 ???,t_f=1 ???
void Driver_SPIMOSI(bool t_f) {
    if (t_f) GPIO_MO->BSRR =  MOSI_pin;
    else     GPIO_MO->BSRR = (MOSI_pin << 16);
}

// MISO ??:?????
bool Driver_SPIMISO(void) {
    return (GPIO_MI->IDR & MISO_pin) ? true : false;
}



void Delay_SPI(uint16_t Num)
{
	volatile uint16_t Timer;
	while(Num--)
	{
	 	Timer = 20;
		while(Timer--);
	}
}

//--------- Receive Data from SPI
//#define Test_NormalCase
uint16_t Touch_SPIRead(void)
{
			uint8_t i;
			uint16_t Val = 0;

	//	for(i=0; i<12; i++)			// 12-bit data: max value = 4095
			// the least 4 significant bits are ignorable
		for(i=0; i<16; i++){			// 16-bit data: max value = 65536
		    Val <<= 1;
			#ifndef Test_NormalCase
			  Driver_SPISCK(1);	// as soon as the master captures data
	      Delay_SPI(1);
			#endif
				Driver_SPISCK(0);	// the slave loads data
	      Delay_SPI(1);
	      if(Driver_SPIMISO()){ // if MISO is high
		        Val++;
				}
				#ifdef  Test_NormalCase
			  Driver_SPISCK(1);	// as soon as the master captures data
	      Delay_SPI(1);
				#endif
				
		}
		#ifdef   Test_NormalCase
		    Driver_SPISCK(0);	// as soon as the master captures data
	      Delay_SPI(1);
		#endif
    return Val;
}

//--------- Send data to SPI
void Touch_SPIWrite(uint8_t Val)
{
    uint8_t i;
    Driver_SPISCK(0);  // make sure that the idle clock is low
    for(i=0; i<8; i++)		// 8 bit mode
	  {
	    if(Val & Bit(7)) // most significant bit first
	       Driver_SPIMOSI(1); // if Bit value = 1
	    else
	      Driver_SPIMOSI(0); // if Bit value = 0
	    Val <<= 1;
      Driver_SPISCK(0); // load at the FALLing edge of SCK clock
	    Delay_SPI(1);	 // hold half period
      Driver_SPISCK(1); // capture at the RISing edge of SCK Clock
	    Delay_SPI(1);	 // hold half period
    }
    Driver_SPISCK(0); 		// idle mode
    Delay_SPI(1);
}




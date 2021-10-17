/*
 * GccApplication1.c
 *
 * Created: 12/2/2019 2:23:56 PM
 * Author : noahthom
 */ 

#include "touchscreen.h"


// uint16_t readX(void)
// {
// 	uint16_t x_coord;
// 	DDRC |= (1<<DDC1)|(1<<DDC0);
// 	PORTC |= (1<<PORTC1);
// 	PORTC &= ~(1<<PORTC0);
// 	x_coord = ReadADC(3);
// 	return x_coord;
// 
// 	ADMUX |= (1<<REFS0) | (1<<MUX0) | (1<<MUX1);
// 	ADMUX &= ~(1<<MUX2) & ~(1<<MUX3);
//  	ADCSRA |= (1<<ADEN) | (1<<ADSC) | (1<<ADATE);
// 	x_coord = ADCL;
// 	x_coord = x_coord + (ADCH<<8); 
// 
// }

// uint16_t readY(void)
// {
// 	uint16_t y_coord;
// 	DDRC |= (1<<DDC2)|(1<<DDC3);
// 	PORTC |= (1<<PORTC2);
// 	PORTC &= ~(1<<PORTC3);
// 	y_coord = ReadADC(2);
// 	return y_coord;
// 	ADMUX |= (1<<REFS0) | (1<<MUX0);
// 	ADMUX &= ~(1<<MUX2) & ~(1<<MUX3) & ~(1<<MUX1);
// 	ADCSRA |= (1<<ADEN) | (1<<ADSC) | (1<<ADATE);
// 	y_coord = ADCL;
// 	y_coord = y_coord + (ADCH<<8);
// 	return y_coord;
// }

uint16_t ReadADC(uint8_t ADCchannel)
{
	//Select the ADC channel entered
	ADMUX = (ADMUX & 0xF0) | (ADCchannel & 0x0F);
	//Starting the conversion
	ADCSRA |= (1<<ADSC);
	//Waiting until ADC conversion is complete
	while( ADCSRA & (1<<ADSC) );
	return ADC;
}
 
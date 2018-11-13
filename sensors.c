#include <avr/io.h>
#define F_CPU 16E6 //-> Dit moet wel als je Visual Studio gebruikt
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <avr/eeprom.h>
#include <inttypes.h>
#include <string.h>
#include "variables.h"

void adc_init(void)
{
	//ADMUX = (1<<REFS0) | (1 << ADLAR);     //select AVCC as reference
	//ADCSRA = (1<<ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);  //enable and prescale = 128 (16MHz/128 = 125kHz)
	ADMUX |= (1 << REFS0) | (1 << ADLAR);
	//enable ADC and set pre-scaler to 128
	ADCSRA = (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2) | (1 << ADEN);
}

uint16_t adc_read()
{
	
	ADCSRA |= (1 << ADSC);

	/* This is an idle loop that just wait around until the conversion
	 * is finished.  It constantly checks ADCSRA's ADSC bit, which we just
	 * set above, to see if it is still set.  This bit is automatically
	 * reset (zeroed) when the conversion is ready so if we do this in
	 * a loop the loop will just go until the conversion is ready. */
	loop_until_bit_is_clear(ADCSRA, ADSC);
	/* Finally, we return the converted value to the calling function. */
	return ADCH;
}

uint16_t readLight()
{
	ADMUX = 0;
	ADMUX |= (1 << REFS0) | (1 << ADLAR);
	
	return adc_read();
}

void getLight() {
	int sensorValue = readLight();
	
	printf("2:%i \r\n", sensorValue);
}


void getTemperature() {
	int sensorInput;

	sensorInput = readTemperature();    //read the analog sensor and store it

	printf("2:%i\r\n", (int)sensorInput);
}

void getDistance()
{
	printf("2:%i\r\n", (int)distance_cm);
}


int readTemperature()
{
	//zet refs0 en adlar en zet pin 0 en 1 aan (3)
	ADMUX = 0;
	ADMUX |= (1 << REFS0) | (1 << ADLAR) | (1 << MUX1) | (1 << MUX0);
	
	return adc_read() - 18;
}
#include <avr/io.h>
#define F_CPU 16E6 //-> Dit moet wel als je Visual Studio gebruikt
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <avr/eeprom.h>

#include "temperature_sensor.h"
#include "serial.h"
#include "light_sensor.h"
#include "scheduler.h"
#include "panel.h"
#include "distance_sensor.h"

uint8_t EEMEM DeviceName[10];
uint8_t EEMEM TempMax;
uint8_t EEMEM TempMin;
uint8_t EEMEM LightThreshold;
uint8_t EEMEM Distance;


// C-reference: http://pubs.opengroup.org/onlinepubs/9699919799/
// Hier kun je oa. een beschrijving van de functies printf en atoi vinden.

void adc_init(void)
{
	ADMUX = (1<<REFS0);     //select AVCC as reference
	ADCSRA = (1<<ADEN) | 7;  //enable and prescale = 128 (16MHz/128 = 125kHz)
}

int readAdc(char chan)
{
	ADMUX	&=	0xf0;
	ADMUX	|=	chan;

	/* This starts the conversion. */
	ADCSRA |= _BV(ADSC);

	/* This is an idle loop that just wait around until the conversion
	 * is finished.  It constantly checks ADCSRA's ADSC bit, which we just
	 * set above, to see if it is still set.  This bit is automatically
	 * reset (zeroed) when the conversion is ready so if we do this in
	 * a loop the loop will just go until the conversion is ready. */
	while ( (ADCSRA & _BV(ADSC)) );

	/* Finally, we return the converted value to the calling function. */
	return ADC;
}


int main() {
	//https://www.avrfreaks.net/forum/tut-c-using-eeprom-memory-avr-gcc?page=all
	
	 uint16_t SRAMLightThreshold;
	 uint8_t  SRAMDeviceName[10];

	SRAMLightThreshold = eeprom_read_byte(&LightThreshold);
	
	ser_init();
	adc_init();
	initPanel();
	SCH_Init_T1();
	SCH_Add_Task(getLight, 2, 10);
	SCH_Add_Task(getTemperature, 2, 10);
	
	//panelDown();
	panelUp();
	
	while (1) {	
		SCH_Dispatch_Tasks();
		//getLight();
		if (getNumericLightValue() < 7) {
			panelDown();
		} else {
			panelUp();
		}
		//printf("%s", getLight());
		getTemperature();
		_delay_ms(500);
	}
}
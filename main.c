#include <avr/io.h>
#define F_CPU 16E6 //-> Dit moet wel als je Visual Studio gebruikt
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <avr/eeprom.h>

//---------- include variables -------------//
int panel_is_down = 0;

uint8_t EEMEM DeviceName[10];
uint8_t EEMEM TempMax;
uint8_t EEMEM TempMin;
uint8_t EEMEM LightThreshold;
uint8_t EEMEM Distance;

const char* getLight();
void initPanel();
void getTemperature();
int readLight();
int readTemperature();
void adc_change_channel(int channel);

//------------------------------ start of include files --------------------------------------//
//panel
int panelUp()
{
	if (panel_is_down == 1) {
		//clear red light first
		PORTB &= ~(1 << 0);
		int i;
		PORTB |= 1 << 1; //set green led
		
		//toggle yellow led
		for (i = 0; i < 5; i ++) {
			PORTB ^= 1 << 2;
			_delay_ms(1000);
		}
		
		PORTB &= ~(1 << 2);
		panel_is_down = 0;
		
		return 0;
	}
	
	return 1;
}

int panelDown()
{
	if (panel_is_down == 0) {
		//clear green light first
		PORTB &= ~(1 << 1);
		int i;
		PORTB |= 1 << 0; //set red led
		
		//toggle yellow led
		for (i = 0; i < 5; i ++) {
			PORTB ^= 1 << 2;
			_delay_ms(1000);
		}
		
		PORTB &= ~(1 << 2);
		panel_is_down = 1;
		
		return 0;
	}
	
	return 1;
}

void initPanel()
{
	//first set the B port with leds
	DDRB = 0x7;
	PORTB = 0x7;
	_delay_ms(200);
	PORTB = 0x1;
}

//light
const char* getLight() {
	//https://stackoverflow.com/questions/1496313/returning-c-string-from-a-function
	
	int sensorValue = readLight()/128;
	printf("Lichtwaarde: %i \n \r", readLight());
}

int getNumericLightValue()
{
	int sensorValue = readLight()/128;
	
	return sensorValue;
}
//temp
void getTemperature() {
	//int sensePin = A0;  //This is the Arduino Pin that will read the sensor output
	int sensorInput;    //The variable we will use to store the sensor input
	double temp;        //The variable we will use to store temperature in degrees.


	// put your main code here, to run repeatedly:
	sensorInput = readTemperature();    //read the analog sensor and store it
	temp = sensorInput - 22;

	printf("Temp       : %i \n \r", (int)temp);
}

// C-reference: http://pubs.opengroup.org/onlinepubs/9699919799/
// Hier kun je oa. een beschrijving van de functies printf en atoi vinden.

void adc_init(void)
{
	//ADMUX = (1<<REFS0) | (1 << ADLAR);     //select AVCC as reference
	//ADCSRA = (1<<ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);  //enable and prescale = 128 (16MHz/128 = 125kHz)
	ADMUX |= (1 << REFS0) | (1 << ADLAR);
	//enable ADC and set pre-scaler to 128
	ADCSRA = (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2) | (1 << ADEN);
}

int adc_read()
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

int readLight()
{
	if (ADMUX != 96) { //niet channel 0 - omdat alleen ADLAR en REFS0 gezet zou moeten zijn voor channel 1
		ADMUX = 0;
		ADMUX |= (1 << REFS0) | (1 << ADLAR);
	} 
	
	return adc_read();
}


void init_timer()
{
	
	TCCR0A = (1 << WGM00) | (1 << COM0A1);
	TCCR0B = (1 << CS01) | (1 << CS00);
	OCR0A = 0;
}

int readTemperature()
{
	ADMUX = 0;
	ADMUX |= (1 << REFS0) | (1 << ADLAR) | (1 << MUX1) | (1 << MUX0);
	
	return adc_read();
}

void listen()
{
	char input[40];
	
	ser_readln(input, sizeof(input), 1);
	
	if(strcmp(input, "get_temperature") == 0) {
		printf("2 25\n\r");	
	} else if (strcmp(input, "get_light") == 0) {
		printf("2 25\n\r");	
	} else if (strcmp(input, "get_temperature") == 0) {
		printf("2 25\n\r");
	} else if (strcmp(input, "get_temperature") == 0) {
		printf("2 25\n\r");
	} else if (strcmp(input, "get_temperature") == 0) {
		printf("2 25\n\r");
	} else if (strcmp(input, "get_temperature") == 0) {
		printf("2 25\n\r");
	} else if (strcmp(input, "get_temperature") == 0) {
		printf("2 25\n\r");
	} else if (strcmp(input, "get_temperature") == 0) {
		printf("2 25\n\r");
	} else if (strcmp(input, "get_temperature") == 0) {
		printf("2 25\n\r");
	} else if (strcmp(input, "get_temperature") == 0) {
		printf("2 25\n\r");
	} else if (strcmp(input, "get_temperature") == 0) {
		printf("2 25\n\r");
	} else if (strcmp(input, "get_temperature") == 0) {
		printf("2 25\n\r");
	} else if (strcmp(input, "get_temperature") == 0) {
		printf("2 25\n\r");
	} else if (strcmp(input, "get_temperature") == 0) {
		printf("2 25\n\r");
	} else if (strcmp(input, "get_temperature") == 0) {
		printf("2 25\n\r");
	} else if (strcmp(input, "get_temperature") == 0) {
		printf("2 25\n\r");
	} else if (strcmp(input, "get_temperature") == 0) {
		printf("2 25\n\r");
	} else if (strcmp(input, "get_temperature") == 0) {
		printf("2 25\n\r");
	} else if (strcmp(input, "get_temperature") == 0) {
		printf("2 25\n\r");
	} else if (strcmp(input, "get_temperature") == 0) {
		printf("2 25\n\r");
	} else if (strcmp(input, "get_temperature") == 0) {
		printf("2 25\n\r");
	} else if (strcmp(input, "get_temperature") == 0) {
		printf("2 25\n\r");
	} else if (strcmp(input, "get_temperature") == 0) {
		printf("2 25\n\r");
	} else if (strcmp(input, "get_temperature") == 0) {
		printf("2 25\n\r");
	} else if (strcmp(input, "get_temperature") == 0) {
		printf("2 25\n\r");
	} 
}


#include "serial.h"

#include "scheduler.h"

#include "distance_sensor.h"

int main() {
	//https://www.avrfreaks.net/forum/tut-c-using-eeprom-memory-avr-gcc?page=all
	
	 uint16_t SRAMLightThreshold;
	 uint8_t  SRAMDeviceName[10];

	SRAMLightThreshold = eeprom_read_byte(&LightThreshold);
	
	ser_init();
	adc_init();
	init_timer();
	initPanel();
	//Initialize_external_interrupt;
	
	//panelDown();
	//panelUp();
	
	while (1) {
		getLight();
		//_delay_ms(500);
		//printf("%s", getLight());
		getTemperature();
		_delay_ms(500);
		//listen();
	}
}
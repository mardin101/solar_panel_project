#include <avr/io.h>
#define F_CPU 16E6 //-> Dit moet wel als je Visual Studio gebruikt
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <avr/eeprom.h>
#include <inttypes.h>
#include <string.h>
#include <errno.h>

//---------- include variables -------------//
int panel_is_down = 0;

uint8_t EEMEM DeviceName[10];
uint16_t EEMEM TempMax;
uint16_t EEMEM TempMin;
uint16_t EEMEM LightThresholdMin;
uint16_t EEMEM LightThresholdMax;
uint16_t EEMEM Distance;
uint8_t EEMEM FirstBoot;
uint16_t EEMEM Mode;

void initPanel();
void getTemperature();
void getLight();
uint16_t readLight();
uint16_t readTemperature();
uint16_t read_adc();

//------------------------------ start of include files --------------------------------------//

//eeprom
const char* getDeviceName()
{
	uint8_t  SRAMDeviceName[10]; 
	uint8_t SRAMFirstBoot;
	
	SRAMFirstBoot = eeprom_read_word(&FirstBoot);
	
	eeprom_read_block((void*)&SRAMDeviceName, (const void*)&DeviceName, 10);

	return (const char*)SRAMDeviceName;
}

void initEEPROM()
{
	uint8_t SRAMFirstBoot;
	
	SRAMFirstBoot = eeprom_read_word(&FirstBoot);

	if (SRAMFirstBoot != 0) {
		char string[] = "no name";
		eeprom_write_block((const void*)string, DeviceName, 10);
		eeprom_write_word((uint16_t *) &Mode, 0x1);
		eeprom_write_word((uint16_t *)&TempMin, 0x0);
		eeprom_write_word((uint16_t *) &TempMax, 0x50);
		eeprom_write_word((uint16_t *) &LightThresholdMin, 0xDF);
		eeprom_write_word((uint16_t *) &LightThresholdMax, 0xEF);
		
		SRAMFirstBoot = 0x0;
		//eeprom_update_byte((uint8_t*)FirstBoot, SRAMFirstBoot);
		eeprom_write_word(&FirstBoot, SRAMFirstBoot);
	}
}

uint16_t getWordFromEEPROM(uint16_t location)
{
	//todo fuck off this shit
	uint16_t SRAMvalue;
	SRAMvalue = eeprom_read_word(&location);
	
	return (uint16_t)SRAMvalue;
}

void setMode()
{
	
}

void setTempMin()
{
	
}

void setTempMax(uint16_t temp)
{
	//todo fix this
	eeprom_write_word((uint16_t *)&TempMax, temp));
	
	uint16_t SRAMtemp;
	SRAMtemp = eeprom_read_word(&TempMax);
	printf("nieuwe waarde: %i", SRAMtemp);
}

void setLightMin()
{
	
	
}

void setLightMax()
{
	
	
}

void setDeviceName()
{
	
}


void run()
{
	uint8_t temp = readTemperature();
	uint16_t SRAMTempMin;
	uint16_t SRAMTempMax;
	
	getTemperature();
	SRAMTempMin = eeprom_read_word(&TempMin);
	SRAMTempMax = eeprom_read_word(&TempMax);
	
	printf("huidig: %i, eeprom: %i \n\r", temp, SRAMTempMax);
	
	//Check for climate change
	if (temp > SRAMTempMax) {
		panelUp();
	} else if (temp < SRAMTempMin) {
		panelDown();
	}
	
	//Light?!
	uint16_t SRAMLightMin;
	uint16_t SRAMLightMax;
	
	SRAMLightMax = eeprom_read_word(&LightThresholdMax);
	SRAMLightMin = eeprom_read_word(&LightThresholdMin);
	
	printf("min: %i max: %i huidig: %i \n\r", SRAMLightMin, SRAMLightMax, readLight());
	if (readLight() > SRAMLightMax) {
		panelDown();
	} else if(readLight() < SRAMLightMin) {
		panelUp();
	}
	
	//int light = getLight();
	_delay_ms(500);
}

void get_config()
{
	int id = 1337;
	uint8_t modus = getWordFromEEPROM(Mode);
	uint8_t tempMin = getWordFromEEPROM(TempMax);
	uint8_t tempMax = getWordFromEEPROM(TempMin);
	uint8_t lightMin = getWordFromEEPROM(LightThresholdMin);
	uint8_t lightMax = getWordFromEEPROM(LightThresholdMax);
	const char* deviceName = getDeviceName();
	
	printf("2 %i,%s,%i,%i,%i,%i,%i \n\r", id, deviceName, modus, tempMin, tempMax, lightMin,lightMax);
}

int getMode()
{
	
}
//panel
int panelUp()
{
	printf("in up fucntie\n\r");
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
	printf("in down fucntie\n\r");
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
void getLight() {
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

	printf("Temp       : %i \n \r", (int)sensorInput);
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
	if (ADMUX != 96) { //niet channel 0 - omdat alleen ADLAR en REFS0 gezet zou moeten zijn voor channel 1
		ADMUX = 0;
		ADMUX |= (1 << REFS0) | (1 << ADLAR);
	} 
	
	return adc_read()/10;
}


void init_timer()
{
	
	TCCR0A = (1 << WGM00) | (1 << COM0A1);
	TCCR0B = (1 << CS01) | (1 << CS00);
	OCR0A = 0;
}

uint16_t readTemperature()
{
	//zet refs0 en adlar en zet pin 0 en 1 aan (3)
	ADMUX = 0;
	ADMUX |= (1 << REFS0) | (1 << ADLAR) | (1 << MUX1) | (1 << MUX0);
	
	return adc_read() - 18;
}
void deleteEnd (char* myStr){

	printf ("%s\n", myStr);
	char *del = &myStr[strlen(myStr)];

	while (del > myStr && *del != '/') {
		del--;
		
		if (*del== ':') {
			*del= '\0';
		}
	}

	return;
}


void listen()
{
	char input[40];

	ser_readln(input, sizeof(input), 1);

	const char delimiter = ':';
	char *value;
	
	value = strchr(input, delimiter);
	value++;
	deleteEnd(input);
	int newValue;
	
	newValue = strtol(value, NULL, 10);

	if(strcmp(&input, 			"get_temperature") == 0) {
		getTemperature();
	} else if (strcmp(&input, 	"get_light") == 0) {
		getLight();
	} else if (strcmp(&input, 	"get_distance") == 0) {
		//distance = getDistance();
		//printf("2 %i\n\r", distance);
	} else if (strcmp(&input, 	"panel_out") == 0) {
		panelDown();
		printf("2 \n\r");
	} else if (strcmp(&input, 	"panel_in") == 0) {
		panelUp();
		printf("2 \n\r");
	} else if (strcmp(&input, 	"set_light_threshold_minimum") == 0) {
		setLightMin(value);
		printf("2 \n\r");
	} else if (strcmp(&input, 	"set_light_threshold_maximum") == 0) {
		setLightMax(value);
		printf("2 \n\r");
	} else if (strcmp(&input, 	"set_temperature_threshold_minimum") == 0) {
		setTempMin(value);
		printf("2 \n\r");
	} else if (strcmp(&input, 	"set_temperature_threshold_maximum") == 0) {
		setTempMax(value);
	} else if (strcmp(&input, 	"set_max_distance") == 0) {
		//setDistancceMax(value);
		printf("2 \n\r");
	} else if (strcmp(&input, 	"handshake") == 0) {
		get_config();
	} else if (strcmp(&input, 	"set_mode_automatic") == 0) {
		setMode(0);
		printf("2 \n\r");
	} else if (strcmp(&input, 	"set_mode_manual") == 0) {
		setMode(1);
		printf("2 \n\r");
	} else if (strcmp(&input, "set_device_name") == 0) {
		setDeviceName(value);
		printf("2 \n\r");
	} else {
		printf("5 wrong_arguments");
	}
}




#include "serial.h"

#include "scheduler.h"

#include "distance_sensor.h"

int main() {
	//https://www.avrfreaks.net/forum/tut-c-using-eeprom-memory-avr-gcc?page=all
	
	ser_init();
	adc_init();
	init_timer();
	initPanel();
	initEEPROM();
	
	SCH_Init_T1();
	//SCH_Add_Task(listen, 10, 20);
	//SCH_Add_Task(panelDown, 200, 300);
	//SCH_Add_Task(panelUp, 200, 300);
	SCH_Add_Task(run, 10, 10);
	SCH_Start();
	
	//panelDown();
	//panelUp();
	
	while (1) {
		//SCH_Dispatch_Tasks();
		listen();
		//panelDown();
		//getDeviceName();
		//get_config();
		//_delay_ms(1000);
	}
}
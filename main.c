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

#include "serial.h"
#include "scheduler.h"
#include "distance_sensor.h"

//---------- include variables -------------//
int panel_is_down = 0;

uint8_t EEMEM DeviceName[10];
uint16_t EEMEM TempMax;
uint16_t EEMEM TempMin;
uint16_t EEMEM LightThresholdMin;
uint16_t EEMEM LightThresholdMax;
uint8_t EEMEM FirstBoot;
uint16_t EEMEM Mode;
uint16_t EEMEM MaxDistance;

void initPanel();
void getTemperature();
void getLight();
uint16_t readLight();
uint16_t readTemperature();
uint16_t read_adc();

//------------------------------ start of include files --------------------------------------//

//eeprom
void initEEPROM()
{
	uint8_t SRAMFirstBoot;
	
	SRAMFirstBoot = eeprom_read_word(&FirstBoot);

	if (SRAMFirstBoot != 0) {
		char string[] = "NO-NAME";
		eeprom_write_block((const void*)string, DeviceName, 10);
		eeprom_write_word((uint16_t *) &Mode, 0x1);
		eeprom_write_word((uint16_t *)&TempMin, 0x0);
		eeprom_write_word((uint16_t *) &TempMax, 0x50);
		eeprom_write_word((uint16_t *) &LightThresholdMin, 0xDF);
		eeprom_write_word((uint16_t *) &LightThresholdMax, 0xEF);
		eeprom_write_word((uint16_t *) &LightThresholdMax, 0xEF);
		eeprom_write_word((uint16_t *) &MaxDistance, 0x96);
		
		SRAMFirstBoot = 0x0;
		eeprom_write_word(&FirstBoot, SRAMFirstBoot);
	}
}

/************************************************************************/
/* Sets the mode either to automatic or manual, depending on value
/************************************************************************/
void setMode(uint16_t mode)
{
	eeprom_write_word((uint16_t *)&Mode, mode);
}

/************************************************************************/
/* Set the minimal temperature for the panel to go up in EEPROM
/************************************************************************/
void setTempMin(uint16_t temp)
{
	eeprom_write_word((uint16_t *)&TempMin, temp);
}

/************************************************************************/
/* Set the maximal temperature for the panel to go up in EEPROM         
/************************************************************************/
void setTempMax(uint16_t temp)
{
	eeprom_write_word((uint16_t *)&TempMax, temp);
}

/************************************************************************/
/* Sets the minimal light threshold for the panel to down up in EEPROM
/************************************************************************/
void setLightMin(uint16_t light)
{
	eeprom_write_word((uint16_t *)&LightThresholdMin, light);
}

/************************************************************************/
/* Sets the maximal light threshold for the panel to down up in EEPROM                                                                     */
/************************************************************************/
void setLightMax(uint16_t light)
{
	eeprom_write_word((uint16_t *)&LightThresholdMax, light);
}

/************************************************************************/
/* Sets the device name in EEPROM. Device name can not be longer than 10 characters
/************************************************************************/
void setDeviceName(char* newName)
{
	int length = strlen(newName);
	
	if (length > 10) {
		printf("5:name_too_long \r");
	} else {
		eeprom_write_block((const void*)newName, DeviceName, 10);
		printf("2\r\n");	
	}
}

/************************************************************************/
/* Sets the maximum distance the panel can go down in EEPROM
/************************************************************************/
void setDistanceMax(uint16_t distance)
{
	eeprom_write_word((uint16_t *)&MaxDistance, distance);
}

/************************************************************************/
/* Sets the value of the device name stored in EEPROM in to the string variable
/************************************************************************/
void getDeviceName(char* string, int buffersize)
{
	char  SRAMDeviceName[10];
	
	eeprom_read_block((void*)&SRAMDeviceName, (const void*)&DeviceName, 10);
	
	strncpy(string, SRAMDeviceName, buffersize-1);
	string[buffersize-1] = '\0';
}

/************************************************************************/
/* Prints a string with all the values stored in EEPROM
/************************************************************************/
void get_config()
{
	uint16_t SRAMMode;
	uint16_t SRAMTempMax;
	uint16_t SRAMTempMin;
	uint16_t SRAMLightThresholdMax;
	uint16_t SRAMLightThresholdMin;
	uint16_t SRAMMaxDistance;
	
	char SRAMDeviceName[10];
	
	int id = 1337; //unique project identifier
	SRAMMode = eeprom_read_word(&Mode);
	SRAMTempMax = eeprom_read_word(&TempMax);
	SRAMTempMin = eeprom_read_word(&TempMin);
	SRAMLightThresholdMax = eeprom_read_word(&LightThresholdMax);
	SRAMLightThresholdMin = eeprom_read_word(&LightThresholdMin);
	SRAMMaxDistance = eeprom_read_word(&MaxDistance);
	
	getDeviceName(SRAMDeviceName, sizeof(SRAMDeviceName));

	printf("2:%i,%s,%i,%i,%i,%i,%i,%i \r\n", id, SRAMDeviceName, SRAMMode, SRAMTempMin, SRAMTempMax, SRAMLightThresholdMin,SRAMLightThresholdMax, SRAMMaxDistance);
}

/************************************************************************/
/* Prints the mode the device is currently running at
/************************************************************************/
void getMode()
{
	uint16_t SRAMMode;
	SRAMMode = eeprom_read_word(&Mode);
	printf("2:%i\r\n");
}

/************************************************************************/
/* This is the main function of the application. It checks for temperature conditions 
and does something with the panel accordingly.
It also checks if its light enough for the panel to go down or up.
/************************************************************************/
void run()
{
	uint8_t temp = readTemperature();
	uint16_t SRAMTempMin;
	uint16_t SRAMTempMax;

	SRAMTempMin = eeprom_read_word(&TempMin);
	SRAMTempMax = eeprom_read_word(&TempMax);
	
	
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
	
	uint16_t isManual;
	isManual = eeprom_read_word(&Mode);
	if (isManual != 1) {
		if (readLight() * 10 > SRAMLightMax) {
			panelDown();
		} else if(readLight() * 10 < SRAMLightMin) {
			panelUp();
		}	
	}
}
/************************************************************************/
/* Pulls the panel up
/************************************************************************/
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

/************************************************************************/
/* Lets the panel go down
/************************************************************************/
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

/************************************************************************/
/* Initialises the panel. Leds will  blink foor 200 ms, then the red 
led lights indicating the panel is up
/************************************************************************/
void initPanel()
{
	//first set the B port with leds
	DDRB = 0x7;
	PORTB = 0x7;
	_delay_ms(200);
	PORTB = 0x1;
}


void getLight() {
	int sensorValue = readLight();
	
	printf("2:%i \r\n", sensorValue);
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


	// put your main code here, to run repeatedly:
	sensorInput = readTemperature();    //read the analog sensor and store it

	printf("2:%i\r\n", (int)sensorInput);
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
	//niet channel 0 - omdat alleen ADLAR en REFS0 gezet zou moeten zijn voor channel 1
	ADMUX = 0;
	ADMUX |= (1 << REFS0) | (1 << ADLAR);
	
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

/************************************************************************/
/* Deletes the end of a string after the delimiter
/************************************************************************/
void deleteEnd (char* myStr){
	char *del = &myStr[strlen(myStr)];

	while (del > myStr && *del != '/') {
		del--;
		
		if (*del== ':') {
			*del= '\0';
		}
	}

	return;
}

char* concat(const char *s1, const char *s2)
{
	char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
	// in real code you would check for errors in malloc here
	strcpy(result, s1);
	strcat(result, s2);
	return result;
}

/************************************************************************/
/* Listen function. Listens to serial connection for commands
/************************************************************************/
void listen()
{
	char input[40];
	char* test;
	//uint8_t c = ser_receive();
	
	if (UCSR0A & (1 << RXC0)) {
		ser_readln(input, sizeof(input), 0);
		//test = concat(c, input);
		//printf("%s", test);
		const char delimiter = ':';
		char *value;
		
		value = strchr(input, delimiter);
		value++; //remove spacer from string
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
			printf("2:\r\n");
			panelDown();
		} else if (strcmp(&input, 	"panel_in") == 0) {
			printf("2:\r\n");
			panelUp();
		} else if (strcmp(&input, 	"set_light_threshold_minimum") == 0) {
			setLightMin(newValue);
			printf("2:\r\n");
		} else if (strcmp(&input, 	"set_light_threshold_maximum") == 0) {
			setLightMax(newValue);
			printf("2:\r\n");
		} else if (strcmp(&input, 	"set_temperature_threshold_minimum") == 0) {
			setTempMin(newValue);
			printf("2:\r\n");
		} else if (strcmp(&input, 	"set_temperature_threshold_maximum") == 0) {
			setTempMax(newValue);
			printf("2:\r\n");
		} else if (strcmp(&input, 	"set_max_distance") == 0) {
			setDistanceMax(newValue);
			printf("2:\r\n");
		} else if (strcmp(&input, 	"handshake") == 0) {
			get_config();
		} else if (strcmp(&input, 	"set_mode_automatic") == 0) {
			setMode(0);
			printf("2:\r\n");
		} else if (strcmp(&input, 	"set_mode_manual") == 0) {
			setMode(1);
			printf("2:\r\n");
		} else if (strcmp(&input, "set_device_name") == 0) {
			setDeviceName(value);
			printf("2:\r\n");
		} else {
			printf("4:unkown_command \r");
		}
	}
}





/************************************************************************/
/* Main function. Inits all the components of the application and runs the application
/************************************************************************/
int main() {
	ser_init();
	adc_init();
	init_timer();
	initPanel();
	initEEPROM();
	
	//SCH_Init_T1();
	//SCH_Add_Task(listen, 10, 20);
	
	//SCH_Start();
	
	//Initialize_timer0();
	//Initialize_external_interrupt();
	//panelUp();
	
	while (1) {
		//Send_signal();
		//SCH_Dispatch_Tasks();
		listen();
		run();
	}
}
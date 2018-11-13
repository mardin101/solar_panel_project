#include <avr/io.h>
#define F_CPU 16E6 //-> Dit moet wel als je Visual Studio gebruikt
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <avr/eeprom.h>
#include <inttypes.h>
#include <string.h>
#include <avr/interrupt.h>

#include "variables.h"
#include "serial.h"
#include "scheduler.h"
#include "distance_sensor.h"
#include "sensors.h"
#include "eeprom.h"
#include "panel.h"


uint8_t EEMEM DeviceName[10];
uint16_t EEMEM TempMax;
uint16_t EEMEM TempMin;
uint16_t EEMEM LightThresholdMin;
uint16_t EEMEM LightThresholdMax;
uint8_t EEMEM FirstBoot;
uint16_t EEMEM Mode;
uint16_t EEMEM MaxDistance;

int dist, d[4], c=0, t=0;

void run();
void deleteEnd (char* str);
void listen();
int main();


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
	
	int light = readLight();
	
	if (isManual != 1) {
		if (light > SRAMLightMax) {
			panelDown();
		} else if(light < SRAMLightMin) {
			panelUp();
		}	
	}
}

/************************************************************************/
/* Deletes the end of a string after the delimiter
/************************************************************************/
void deleteEnd (char* str) {
	char *del = &str[strlen(str)];

	while (del > str && *del != '/') {
		del--;
		
		if (*del== ':') {
			*del= '\0';
		}
	}

	return;
}


/************************************************************************/
/* Listen function. Listens to serial connection for commands
/************************************************************************/
void listen()
{
	char input[40];
	char *withoutSpace;
	
	if (UCSR0A & (1 << RXC0)) {
		ser_readln(input, sizeof(input), 0);
		
		const char delimiter = ':';
		char *value;
		
		if (input[0] == '\n') {
			withoutSpace = input + 1;	
		} else {
			withoutSpace = input;	
		}
		
		value = strchr(input, delimiter);
		value++; //remove spacer from string
		deleteEnd(input);
		
		int newValue;
		newValue = strtol(value, NULL, 10);
		
		if(strcmp(input, 			"get_temperature") == 0) {
			getTemperature();
		} else if (strcmp(withoutSpace, 	"get_light") == 0) {
			getLight();
		} else if (strcmp(withoutSpace, 	"get_distance") == 0) {
			getDistance();
		} else if (strcmp(withoutSpace, 	"panel_out") == 0) {
			printf("2:\r\n");
			panelDown();
		} else if (strcmp(withoutSpace, 	"panel_in") == 0) {
			printf("2:\r\n");
			panelUp();
		} else if (strcmp(withoutSpace, 	"set_light_threshold_minimum") == 0) {
			setLightMin(newValue);
			printf("2:\r\n");
		} else if (strcmp(withoutSpace, 	"set_light_threshold_maximum") == 0) {
			setLightMax(newValue);
			printf("2:\r\n");
		} else if (strcmp(withoutSpace, 	"set_temperature_threshold_minimum") == 0) {
			setTempMin(newValue);
			printf("2:\r\n");
		} else if (strcmp(withoutSpace, 	"set_temperature_threshold_maximum") == 0) {
			setTempMax(newValue);
			printf("2:\r\n");
		} else if (strcmp(withoutSpace, 	"set_max_distance") == 0) {
			setDistanceMax(newValue);
			printf("2:\r\n");
		} else if (strcmp(input, 	"handshake") == 0) {
			get_config();
		} else if (strcmp(withoutSpace, 	"set_mode_automatic") == 0) {
			setMode(0);
			printf("2:\r\n");
		} else if (strcmp(withoutSpace, 	"set_mode_manual") == 0) {
			setMode(1);
			printf("2:\r\n");
		} else if (strcmp(withoutSpace, "set_device_name") == 0) {
			setDeviceName(value);
			printf("2:\r\n");
		} else {
			printf("4:unknown_command\r\n", input);
		}
		
		UCSR0A &=~ (1 << RXC0);
	}
}


/************************************************************************/
/* Main function. Inits all the components of the application and runs the application
/************************************************************************/
int main() {
	ser_init();
	adc_init();
	//init_timer();
	initPanel();
	initEEPROM();
	
	
	//SCH_Init_T1();
	//SCH_Add_Task(listen, 10, 20);
	
	//SCH_Start();
	
	Initialize_timer0();
	Initialize_external_interrupt();
	Initialize_Ports();
	//panelUp();
	
	while (1) {
		//Send_signal();
		//SCH_Dispatch_Tasks();
		listen();
		run();
		_delay_ms(1);
	}
}
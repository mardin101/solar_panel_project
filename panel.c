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

int panel_is_down = 0;
/************************************************************************/
/* Pulls the panel up
/************************************************************************/
int panelUp()
{
	if (panel_is_down == 1) {
		//clear red light first
		PORTB &= ~(1 << 0);
		int i;
		PORTB |= (1 << 1); //set green led
		
		uint16_t SRAMMaxDistance = eeprom_read_word(&MaxDistance);
		
		//printf("%i", distance_cm);
		while (distance_cm > SRAMMaxDistance) {
			PORTB ^= (1 << 2);
			Send_signal();
			_delay_ms(1000);
			//printf("%i \r\n", distance_cm);
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
		//printf("We gaan panel naar beneden doen\n\r");
		//printf("distance is nu: %i \n\r", distance_cm);
		//clear green light first
		PORTB &= ~(1 << 1);
		int i;
		PORTB |= (1 << 0); //set red led
		
		//toggle yellow led
		uint16_t SRAMMaxDistance = eeprom_read_word(&MaxDistance);
		while (distance_cm < SRAMMaxDistance) {
			PORTB ^= (1 << 2);
			Send_signal();
			_delay_ms(1000);
			//printf("%i \r\n", distance_cm);
		}
		
		PORTB &= ~(1 << 2);
		panel_is_down = 1;
		
		return 0;
	} else {
		//printf("Panel was al naar beneden\n\r");
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
	PORTB = 0x2;
}
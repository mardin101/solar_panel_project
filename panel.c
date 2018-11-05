/*
 * panel.c
 *
 * Created: 2-11-2018 12:19:24
 *  Author: gebruiker
 */ 

#define F_CPU 16E6 //-> Dit moet wel als je Visual Studio gebruikt
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int panel_is_down = 0;

int panelUp()
{
	if (panel_is_down) {
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
	if (!panel_is_down) {
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
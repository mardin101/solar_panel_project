/*
 * light_sensor.c
 *
 * Created: 1-11-2018 15:28:56
 *  Author: gebruiker
 */ 

#include <stdio.h>

const char* getLight1() {
	//https://stackoverflow.com/questions/1496313/returning-c-string-from-a-function
	static char* brightness[] = {"Very dark", "Dark", "Dim dark", "Dim", "Dim light", "Light", "Bright","Very Bright"};
	int sensorValue = readLight()/128;
	printf("Lichtwaarde: %i \n \r", readLight());
	
	if (sensorValue > 0 && sensorValue <= 8) {
		return brightness[sensorValue];
	} else {
		return "Foute meetwaarde";
	}
}

int getNumericLightValue()
{
	int sensorValue = readLight()/128;
	
	return sensorValue;
}
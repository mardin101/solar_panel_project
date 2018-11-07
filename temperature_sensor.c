/*
 * temperature_sensor.c
 *
 * Created: 1-11-2018 15:26:49
 *  Author: gebruiker
 */ 

#include <stdio.h>

void getTemperature() {
	//int sensePin = A0;  //This is the Arduino Pin that will read the sensor output
	int sensorInput;    //The variable we will use to store the sensor input
	double temp;        //The variable we will use to store temperature in degrees.


	// put your main code here, to run repeatedly:
	sensorInput = readLight();    //read the analog sensor and store it
	temp = temp * 5;                 //multiply by 5V to get voltage
	temp = (double)sensorInput / 1024;       //find percentage of input reading
	temp = temp - 0.5;               //Subtract the offset
	temp = temp * 100;               //Convert to degrees

	printf("Temperatuur: %i \n \r", (int)sensorInput);
}





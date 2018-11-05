/*
 * distance_sensor.h
 *
 * Created: 31-10-2018 14:43:00
 *  Author: gebruiker
 */ 


#ifndef DISTANCE_SENSOR_H_
#define DISTANCE_SENSOR_H_

void Send_signal(void);
void Initialize_external_interrupt (void);
void Initialize_timer0 (void);

unsigned char working;
unsigned char rising_edge;
uint16_t timer_value;
int distance_cm;
uint8_t error;




#endif /* DISTANCE_SENSOR_H_ */
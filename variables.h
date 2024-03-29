/*
 * variables.h
 *
 * Created: 12-11-2018 10:52:00
 *  Author: gebruiker
 */ 


#ifndef VARIABLES_H_
#define VARIABLES_H_

extern uint8_t EEMEM DeviceName[10];
extern uint16_t EEMEM TempMax;
extern uint16_t EEMEM TempMin;
extern uint16_t EEMEM LightThresholdMin;
extern uint16_t EEMEM LightThresholdMax;
extern uint8_t EEMEM FirstBoot;
extern uint16_t EEMEM Mode;
extern uint16_t EEMEM MaxDistance;

extern unsigned char working;
extern unsigned char rising_edge;
extern uint16_t timer_value;
extern int distance_cm;
extern uint8_t error;

#endif /* VARIABLES_H_ */
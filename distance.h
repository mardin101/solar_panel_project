/*
 * distance.h
 *
 * Created: 31-10-2018 14:10:48
 *  Author: gebruiker
 */ 


#ifndef DISTANCE_H_
#define DISTANCE_H_

#define HIGH 0x1
#define LOW 0x0

#define BEGIN 0x1
#define END 0x0

void init_ports(void);
void init_timer(void);
void init_ext_int(void);
void show_leds(uint8_t n);
void setLed(uint8_t value, uint8_t position);
void sendCommand(uint8_t value);
void write(uint8_t pin, uint8_t val);
void shiftOut (uint8_t val);
int calc_cm(int counter);

const uint8_t data = 0;
const uint8_t clock = 1;
const uint8_t strobe = 2;

#endif /* DISTANCE_H_ */
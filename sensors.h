#ifndef SENSORS_H_
#define SENSORS_H_

void getTemperature();
void getLight();
uint16_t readLight();
uint16_t adc_read();
void adc_init(void);
int readTemperature();


#endif /* SENSORS_H_ */
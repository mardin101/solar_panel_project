/*
 * eeprom.h
 *
 * Created: 12-11-2018 10:13:26
 *  Author: gebruiker
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_

void initEEPROM();
void setMode(uint16_t mode);
void setTempMin(uint16_t temp);
void setTempMax(uint16_t temp);
void setLightMin(uint16_t light);
void setLightMax(uint16_t light);
void setDeviceName(char* newName);
void setDistanceMax(uint16_t distance);
void getDeviceName(char* string, int buffersize);
void get_config();
void getMode();

#endif /* EEPROM_H_ */
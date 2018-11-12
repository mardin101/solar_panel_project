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

/************************************************************************/
/* Initialises the eeprom with default values if this device never has booted yet.
/* Since all the EEPROM is filled with 1's, we take a random byte and read its value.
/* If its a one, we can safely assume we can set default values.
/************************************************************************/
void initEEPROM()
{
	uint8_t SRAMFirstBoot;
	
	SRAMFirstBoot = eeprom_read_word(&FirstBoot);

	if (SRAMFirstBoot != 0) {
		char string[] = "NO-NAME";
		eeprom_write_block((const void*)string, DeviceName, 10);
		eeprom_write_word((uint16_t *) &Mode, 0x1);
		eeprom_write_word((uint16_t *)&TempMin, 0x0);
		eeprom_write_word((uint16_t *) &TempMax, 0x50);
		eeprom_write_word((uint16_t *) &LightThresholdMin, 0xDF);
		eeprom_write_word((uint16_t *) &LightThresholdMax, 0xEF);
		eeprom_write_word((uint16_t *) &LightThresholdMax, 0xEF);
		eeprom_write_word((uint16_t *) &MaxDistance, 0x96);
		
		SRAMFirstBoot = 0x0;
		eeprom_write_word(&FirstBoot, SRAMFirstBoot);
	}
}

/************************************************************************/
/* Sets the mode either to automatic or manual, depending on value
/************************************************************************/
void setMode(uint16_t mode)
{
	eeprom_write_word((uint16_t *)&Mode, mode);
}

/************************************************************************/
/* Set the minimal temperature for the panel to go up in EEPROM
/************************************************************************/
void setTempMin(uint16_t temp)
{
	eeprom_write_word((uint16_t *)&TempMin, temp);
}

/************************************************************************/
/* Set the maximal temperature for the panel to go up in EEPROM         
/************************************************************************/
void setTempMax(uint16_t temp)
{
	eeprom_write_word((uint16_t *)&TempMax, temp);
}

/************************************************************************/
/* Sets the minimal light threshold for the panel to down up in EEPROM
/************************************************************************/
void setLightMin(uint16_t light)
{
	eeprom_write_word((uint16_t *)&LightThresholdMin, light);
}

/************************************************************************/
/* Sets the maximal light threshold for the panel to down up in EEPROM                                                                     */
/************************************************************************/
void setLightMax(uint16_t light)
{
	eeprom_write_word((uint16_t *)&LightThresholdMax, light);
}

/************************************************************************/
/* Sets the device name in EEPROM. Device name can not be longer than 10 characters
/************************************************************************/
void setDeviceName(char* newName)
{
	int length = strlen(newName);
	
	if (length > 10) {
		printf("5:name_too_long \r");
	} else {
		eeprom_write_block((const void*)newName, DeviceName, 10);
		printf("2\r\n");	
	}
}

/************************************************************************/
/* Sets the maximum distance the panel can go down in EEPROM
/************************************************************************/
void setDistanceMax(uint16_t distance)
{
	eeprom_write_word((uint16_t *)&MaxDistance, distance);
}

/************************************************************************/
/* Sets the value of the device name stored in EEPROM in to the string variable
/************************************************************************/
void getDeviceName(char* string, int buffersize)
{
	char  SRAMDeviceName[10];
	
	eeprom_read_block((void*)&SRAMDeviceName, (const void*)&DeviceName, 10);
	
	strncpy(string, SRAMDeviceName, buffersize-1);
	string[buffersize-1] = '\0';
}

/************************************************************************/
/* Prints a string with all the values stored in EEPROM
/************************************************************************/
void get_config()
{
	uint16_t SRAMMode;
	uint16_t SRAMTempMax;
	uint16_t SRAMTempMin;
	uint16_t SRAMLightThresholdMax;
	uint16_t SRAMLightThresholdMin;
	uint16_t SRAMMaxDistance;
	
	char SRAMDeviceName[10];
	
	int id = 1337; //unique project identifier
	SRAMMode = eeprom_read_word(&Mode);
	SRAMTempMax = eeprom_read_word(&TempMax);
	SRAMTempMin = eeprom_read_word(&TempMin);
	SRAMLightThresholdMax = eeprom_read_word(&LightThresholdMax);
	SRAMLightThresholdMin = eeprom_read_word(&LightThresholdMin);
	SRAMMaxDistance = eeprom_read_word(&MaxDistance);
	
	getDeviceName(SRAMDeviceName, sizeof(SRAMDeviceName));

	printf("2:%i,%s,%i,%i,%i,%i,%i,%i \r\n", id, SRAMDeviceName, SRAMMode, SRAMTempMin, SRAMTempMax, SRAMLightThresholdMin,SRAMLightThresholdMax, SRAMMaxDistance);
}

/************************************************************************/
/* Prints the mode the device is currently running at
/************************************************************************/
void getMode()
{
	uint16_t SRAMMode;
	SRAMMode = eeprom_read_word(&Mode);
	printf("2:%i\r\n");
}
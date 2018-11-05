/*
 * Distance_sensor.c
 *
 * Created: 6/19/2015 7:58:44 PM
 *  Author: Savvas Kokkinidis _ sabbaskok@hotmail.com
 */ 


#define F_CPU 16000000UL			     // 16.0 Mhz clock
#define F_CPU_div    F_CPU/2000000   // In our case is 8, use for correct uSec and centimeter calculation


//* Libraries ** //
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "serial.h"
//********//


//** I/O pin ****//
#define Trig 6					
#define MOSI 5
#define SCK 7
#define SS 4
#define INT1_pin 3
//**********//

//**********        MAX7219 (check datasheet)      **********//

#define SetLight	 0x0A				
#define DecodingMode 0x09
#define SetScanLimit 0x0B
#define TurnOnOff    0x0C
#define SetValueOfDigit0 0x01
#define SetValueOfDigit1 0x02
#define SetValueOfDigit2 0x03
#define SetValueOfDigit3 0x04
//*************** ***** ***** ***** ****// 

#define TimeOutTime  300				// MAX time (on msec) for TimeOut Error
#define Delay_Until_New_Measure 60		// Delay between measures (on msec) , may occur problem if delay is short.Check datasheet
#define max_ticks   TimeOutTime * F_CPU_div*1000	// MAX ticks Timer0 can reach before seems like Timeout to us.

char Digit0,Digit1,Digit2,Digit3 ;     // Variables for display each digit.
volatile uint32_t distance_avg = 0;
volatile char Busy_measure = 0;
volatile char Activated_sensor = 0;
volatile uint32_t Timer0_counter = 0 ;
volatile char TimeOUT = 0;
volatile uint32_t distance[5];
volatile int i=0;
volatile char ready_to_display = 0 ;

void execute(unsigned char cmd,unsigned char data)
{
	PORTB &= ~(1<<SS) ;				// Pull down SS, initialise packet
	SPDR = cmd ;					// Start cmd transmision
	while(!(SPSR & (1<<SPIF)));		// wait trasnf to finish									// Test PIN used on debug
	SPDR = data ;					// start data transmision
	while(!(SPSR & (1<<SPIF)));		// wait trasnf to finish
	PORTB |= (1<<SS) ;				// terminate packet,pull SS high
}

void Fire_Sensor() 
{
	PORTD |= (1<<Trig);				// Set Trig pin high for 10us to start measure.    
	Busy_measure =1 ;               // We going to start new measure, so sensor is busy !
	_delay_us(10);					// 10us Delay.
	PORTD &= ~(1<<Trig);			// Set Trig pin LOW.
}



int main(void)
{
	// INT1 Use/Configuration	
	DDRD &= ~(1<<INT1_pin);		// Set PD2 as input (Using for interrupt INT1).
	PORTD |= (1<<INT1_pin);		// Enable PD2 pull-up resistor.
	GICR  |= (1<<INT1);			// Enable INT1 interrupts.
	MCUCR |= (1<<ISC10) ;		// Trigger INT1 on any logic change.
	// Trig Conf
	DDRD |= (1<<Trig);			// Triger pin as output
	PORTD &= ~(1<<Trig);		// LOW
	//SPI Use/Configuration	
	DDRB = (1<<MOSI) | (1<<SS) | (1<<SCK) ;     // outputs for SPI
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0);   // SPI as master
	// Use/Configuration	 of Timer0 (8 bit timer)
	TCCR0 |= (1<<CS00);						   // NO prescaller , evvery tick is 1us  - START 
	TCNT0 = 0;								   // Clear counter
	TIMSK |= (1<<TOIE0);					   // Enable Timer0-Overflow interrupts
	sei();										// Global interrupts Enabled.
	ser_init();
		
		
	// MAX7219 configuration	
	execute(DecodingMode,0b00001111)  ;   // enable decoding for digits 0,1,2,3
	execute(SetScanLimit,4)	;			  // how many segments we have connect. 4 in our case
	execute(TurnOnOff,1);				  // TURN max7219 ON
	execute(SetLight,2) ;				  // 0-16 values , 16 is the  MAX light
	

	
	while(1)
	{
			if(Busy_measure == 0)
			{
				_delay_ms(Delay_Until_New_Measure);			// Dont fire sensor very often.In order to prevent trigger signal to the echo signal (datasheet)
				Fire_Sensor();								// Start new measure.
			}					
							
							
			// Sensor datasheet says that can measure 2-400cm , so 4th digit no need. Its optional
							
				if(ready_to_display==1) {
					distance_avg= (distance[0]+distance[1]+distance[2]+distance[3]+distance[4])/5 ; // AVG distance , to avoid some errors
					printf(distance_avg);
					ready_to_display=0;
				}

			}
}



ISR(TIMER0_OVF_vect)  // Here every time Timer0 Overflow
{
	if(Activated_sensor)	   // Only if sensor is active
	{
		Timer0_counter++;          // How many times Timer0 got OVF ?
		uint32_t ticks = Timer0_counter * 256 + TCNT0;		// calculate total Timer0 ticks
		TimeOUT=0;
		if(ticks > max_ticks)
		{
			Activated_sensor=0;	 // * Free the sensor to new measures*/
			Busy_measure=0;		 //  *********
			TimeOUT =1 ;        // Timeout ; Used it as you want.
			
		}
				
	}

}


ISR(INT1_vect)
{
	     if (Busy_measure==1.)		// Care only if sensor is started
		 {
		       if (Activated_sensor == 0)  // High_Pulse '0' -> '1' , start time measure
					{ 
					TCNT0 = 0;			    // Reset Timer0/Counter	
			        Activated_sensor = 1;	// Now its not free
			        Timer0_counter = 0;		// Clear counter
			        } 
					else					// Low Pulse '1' -> '0', we have  our result
					{
			        Activated_sensor = 0 ; 
			        distance[i] = (Timer0_counter * 256 + TCNT0) / (F_CPU_div * 58);  
					 // us/58 = .. cm.  Every 8 ticks is 1us (8Mhz clock, NO prescaler). http://www.micropik.com/PDF/HCSR04.pdf 
			        Busy_measure = 0 ;    // Ready for new measure
					i++;
						if(i==5)			// we got the 5 value we wanted; reset pointer and rdy to diplay value
						{
							i=0;					//clear pointer
							ready_to_display=1;		// we can diplay the distance, after we find the AVG
						}
						
		        }
	        }
	
	
}
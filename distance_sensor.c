#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void Send_signal(void);
void Initialize_external_interrupt (void);
void Initialize_timer0 (void);

unsigned char working;
unsigned char rising_edge;
uint16_t timer_value;
int distance_cm;
uint8_t error;

ISR (TIMER0_OVF_vect)
{
	if(rising_edge==1) //Check if there was echo
	{
		timer_value++;
		/*Check if isnt out of range*/
		if(timer_value > 91)
		{
			rising_edge = 0;
			error = 1;
		}
	}
}
ISR (INT0_vect)
{
	if(rising_edge==0) {
		rising_edge=1;
		TCNT0 = 0;
		timer_value = 0;
	} else { //Check if echo turned low, calculate distance 
		rising_edge = 0;
		distance_cm = (timer_value*256 + TCNT0)/58;
	}
	
}

void Send_signal()
{
		error = 0;		//Clean errors

		PORTD &=~ (1 << PIND4);
		_delay_us(1);
		PORTD |= (1 << PIND4); //Send 10us second pulse
		_delay_us(10);
		PORTD &=~ (1 << PIND4);
}

void Initialize_external_interrupt()
{
	EICRA |= (1 << ISC00);    // set INT0 to trigger on ANY logic change
	EIMSK |= (1 << INT0);     // Turns on INT0
	sei();
}


void Initialize_Ports()
{
	//init port
	DDRD |= 0b00011111;
	DDRD &=~ (1 << PIND2);
	_delay_us(50);
	PORTD = 0;
	_delay_us(100);
}

void Initialize_timer0()
{
	TCCR0B |= (1 << CS01); //prescaling x8
	TCNT0 = 0;			//Reset timer
	TIMSK0 |= (1 << TOIE0); //Timer overflow interrupt enable
}
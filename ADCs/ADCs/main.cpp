/*
 * ADCs.cpp
 *
 * Created: 02/02/2025 06:44:27 a.m.
 * Author : LSL
 */

/*The ISR(ADC_vect) interrupt is executed when the ADC conversion has completed. 
This means that when you start a conversion with ADCSRA |= (1 << ADSC), the 
microcontroller waits for the conversion to complete and then executes the interrupt routine.*/ 

#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

float readADC(int channel);
volatile int flag = 0;

int main(void) {
	cli();  //Disable global interrupts

	DDRD |= (1<<5);  // PORTD pin 5 as output
	DDRD |= (1<<7);  // PORTD pin 7 as output
	DDRD |= (1<<0);  // PORTD pin 0 as output

	/*************** REFSn:?Reference Selection [n = 1:0] ************/
	ADMUX &=~ (1<<REFS1); //AVCC with external capacitor at AREF pin
	ADMUX |= (1<<REFS0);
	
	/*************** ADLAR:?ADC Left Adjust Result *******************/
	ADMUX &=~ (1 << ADLAR); //ADCH
	
	/*************** ADEN:?ADC Enable *******************************/
	ADCSRA |= (1<<ADEN);
	
	/*************** ADPSn:?ADC Prescaler Select [n = 2:0] **********/
	ADCSRA |= (1<<ADPS2); //16
	ADCSRA &=~ (1<<ADPS1);
	ADCSRA &=~ (1<<ADPS0);

	float adc1 = 0, adc2 = 0, voltage1 = 0, voltage2 = 0;
	
	ADCSRA |= (1<<ADIE); //ADC Conversion Complete Interrupt is activated

	sei();  //Enable global interrupts

	while (1) {

		adc1 = readADC(0); 
		voltage1 = (adc1 * 5.0) / 1024.0;   

		if (voltage1 >= 3.0) {
			PORTD |= (1<<5);  
		}else{
			PORTD &=~ (1<<5);  
		}
				
		adc2= readADC(2);
		voltage2 = (adc2 * 5.0) / 1024.0;

		if (voltage2 <= 3.0) {
			PORTD |= (1<<7);  
		}else{
			PORTD &=~ (1<<7);  
		}
		
		if(flag == 1){
			PORTD |= (1<<0);	
		}
	}

	return 0;
}

float readADC(int channel) {
	//Select ADC channel
	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);  //Clear the lowest 4 bits of ADMUX and set the channel

	ADCSRA |= (1 << ADSC);  //Start the conversion

	while (ADCSRA & (1 << ADSC));  //Wait until ADSC goes to 0

	return ADC;
}

ISR(ADC_vect){
	flag = 1;
}

#include <stdint.h>
#include <avr/io.h>
//#include <avr/iotn2313.h>
#include <avr/interrupt.h>

/*
 * main.c
 *
 *  Created on: Oct 26, 2011
 *      Author: sean
 */

void setup();
void calc_checksum();
void calc_length();

int main(void) {
	setup();

	// turn LED off.
	PORTD &= ~_BV(PD3);

	for (;;) {}

	return(1);
}

void setup() {
	// disable global interrupts
	cli();

	// Set clock pre-scalar to 2
	CLKPR = 0x80;
	CLKPR = 0x01;

	// Set PD2 as input pin. set to 0
	DDRD &= ~_BV(PD2);

	// Enable pull up resistor for PD2
	PORTD |= _BV(PD2);

	// Set PD3 as output pin. set to 1
	DDRD |= _BV(PD3);

	// TODO: remove
	DDRB |= _BV(PB4);

	// Set USART baud rate to 9600, UBBR = 11
	UBRRH = 0x00;
	UBRRL = 0b00001011;

	// Enable USART receive interrupt
	UCSRB |= (1 << RXCIE);

	// Enable USART transmit and receive
	UCSRB |= (1 << RXEN) | (1 << TXEN);

	// Enable global interrupts
	sei();
}

ISR(USART_RX_vect) {
	uint8_t usart_input = UDR;

	// Exit if we did not get ASCII S
	if (usart_input != 0x53) {
		return;
	}

	// TODO: remove the LED toggle test
	PORTB ^= _BV(PB4);

	// read input
	if (bit_is_clear(PIND, PD2)) {
		// 3.3V between PD2 and VCC, turn LED on
		PORTD |= _BV(PD3);
		UDR = 0x59; // ASCII Y
	} else {
		// voltage between VCC and PD2 is 0V, turn LED off
		PORTD &= ~_BV(PD3);
		UDR = 0x4E; // ASCII N
	}
}

//
// Implement API mode 2
//

// TODO calculate checksum byte
// for API mode
void calc_checksum() {

}

// TODO verify checksum byte
// for API mode
void verify_checksum() {

}

// TODO calculate length(MSB + LSB)
// TODO figure out how to calculate the length (what is the length?)
void calc_length() {

}



// TODO: implement TX Request (64-bit address)
//                       _____Frame_Data_____
// start_delim  length   API_Identifier  Data  checksum
// 0x7E         MSB LSB  0x00            XXX   1 byte

// TODO: implement RX Packet (64-bit address)
//                       _____Frame_Data_____
// start_delim  length   API_Identifier  Data checksum
// 0x7E         MSB LSB  0x80            XXX  1 byte
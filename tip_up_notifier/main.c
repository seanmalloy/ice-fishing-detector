#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "xbee1.h"
// #include <avr/iotn2313.h>   // eventually remove this 

void setup(void);

int main(void) {
	setup();

	// turn LED off.
	PORTB |= _BV(PB4);

	for (;;) {}
	return(1);
}

void setup(void) {
	// disable global interrupts
	cli();

	// Set clock pre-scalar to 2
	CLKPR = 0x80;
	CLKPR = 0x01;

	TCCR1B |= (1 << WGM12);   // enable CTC mode for timer 1
	TIMSK |= (1 << OCIE1A);   // enable CTC interrupt

	// Set PB4 as output. Set to 1
	DDRB |= _BV(PB4);

	// TODO: remove this code
	// Set PB3 as output. Set to 1
	DDRB |= _BV(PB3);

	// Set USART baud rate to 9600, UBBR = 11
	UBRRH = 0x00;
	UBRRL = 0b00001011;
	// UBRRL = 0b00010111;

	// Enable USART receive interrupt
		UCSRB |= (1 << RXCIE);

	// Enable USART transmit and receive
	UCSRB |= (1 << RXEN) | (1 << TXEN);

	// enable global interrupts
	sei();

	// 1Hz frequency for timer 1
	OCR1A = 28799;

	// start Timer1 with pre-scalar of 64
	TCCR1B |= (1 << CS10) | (1 << CS11);
}

ISR(USART_RX_vect) {
	uint8_t input;
	input = UDR;

	// Receive "Y"
	if (input == 0x59) {
		// Turn LED on
		PORTB &= ~_BV(PB4);
	} else {
		// turn LED off
		PORTB |= _BV(PB4);
	}
}

ISR(TIMER1_COMPA_vect) {
	// TODO: remove this code
	// Toggle LED
	PORTB ^= _BV(PB3);

        // START
        // 5/9/2012 - Length in API packets looks correct now
        //            Next step verify frame data and checksum.
        unsigned int addr = 1;
        unsigned char data[1] = { 0x53 }; // ASCI captial S
        tx_request16 tx_packet = create_tx_request16(data, addr);

	// send data out the UART
        while (( UCSRA & (1 << UDRE )) == 0) {}; // Do nothing until UDR is ready for more data to be written to it
	UDR = tx_packet.start_delim;
        while (( UCSRA & (1 << UDRE )) == 0) {};
	UDR = tx_packet.msb_length;
        while (( UCSRA & (1 << UDRE )) == 0) {};
	UDR = tx_packet.lsb_length;
        while (( UCSRA & (1 << UDRE )) == 0) {};
	UDR = tx_packet.api_id;
        while (( UCSRA & (1 << UDRE )) == 0) {};
	UDR = tx_packet.frame_id;
        while (( UCSRA & (1 << UDRE )) == 0) {};
	UDR = tx_packet.dest_addr;
        while (( UCSRA & (1 << UDRE )) == 0) {};
	UDR = tx_packet.tx_opts;
        for (unsigned int i = 0; i != 1; i++) {
            while (( UCSRA & (1 << UDRE )) == 0) {};
	    UDR = tx_packet.rf_data[i];
        }
        while (( UCSRA & (1 << UDRE )) == 0) {};
	UDR = tx_packet.checksum;
}


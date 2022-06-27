#include <util/delay.h>
#include <APRS.h>
#include <avr/io.h>

int main(void){
	
	APRS_init();	
	DDRB |= _BV(DDB5);

	while(1){
		PORTB |= _BV(PB5);
		APRS_send_packet();	
		PORTB &= ~(_BV(PB5));
		_delay_ms(2000);
	}

	return 0;
}
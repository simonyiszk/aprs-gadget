#include <util/delay.h>
#include <APRS.h>

extern volatile uint8_t symbol;

int main(void){
	
	APRS_init();

	while(1){
		APRS_send_packet();		
		_delay_ms(2000);
	}

	return 0;
}
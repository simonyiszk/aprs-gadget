/*#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>

#define BAUD 115200 //kevés :( 
#define BRC ((F_CPU / 16 / BAUD) - 1)
//#define BRC2 ((F_CPU / 16 / BAUD) - 1) //for double speed

volatile unsigned char index = 0;
const char* text = "CSUMPA\r\n";

inline void setupTimer(){
  TCCR1A = 0; //output compare pins disconnected, CTC (clear timer on compare) mode
  TCCR1B = (1<<WGM12); //CTC mode, no prescale, timer STOPPED
  TCCR1C = 0; //don't force output compare

  //note: To do a 16-bit write, the high byte must be written before the low byte. For a 16-bit read, the low byte must be read before the high byte.
  
  TCNT1H = 0; //clear timer
  TCNT1L = 0;

  OCR1AH = 0x03; //set value for comparison: ADC max. sampling rate is ~19kHz nominally, use smaller than that but which is multiple of 1200 -> 18k
  OCR1AL = 0x79; // (16MHZ -> (no prescale) -> 18000 -> ~889 = 0x0771) TODO: PLUSMINUS 1 because of overflow?

  TIMSK1 = (1<<OCIE1A); //enable interrupt on compare match A
}

inline void startTimer(){
  TCCR1B |= (1<<CS10); //start timer, no prescaler
}

ISR(TIMER1_COMPA_vect){
  UDR0 = text[index];
  index = (index+1) & 7;
}

int main(void){
  
  //BAUD RATE: 250k
  UBRR0H = 0; //(BRC >> 8);
  UBRR0L = 7; //BRC;
  UCSR0A = (1<<U2X0); //double the speed
  UCSR0B = (1 << TXEN0); //enable TX
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); //async, 8 bit data, 1 stop bit, no parity
  setupTimer();

  SREG = (1<<7); //enable global interrupt
  startTimer();

  while(1){
    
  }
}*/
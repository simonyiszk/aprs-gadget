#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint16_t ADCin = 0x0000;

void setupGPIO(){
  DDRB = (1<<DDB5); //built-in LED output
  PORTB = (1<<PORTB5); //on
}

inline void setupADC(){
  ADMUX = (1<<6); //ADC0, internal reference
  ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1); //enable ADC, no interrupt, 64 prescale (16M -> 250k -> 13cycle -> ~19k sample max.)
  DIDR0 = (1<<ADC5D)|(1<<ADC4D)|(1<<ADC3D)|(1<<ADC2D)|(1<<ADC1D)|(1<<ADC0D); //disable all digital input buffer to reduce power consumption
}

inline void setupTimer(){
  TCCR1A = 0; //output compare pins disconnected, CTC (clear timer on compare) mode
  TCCR1B = (1<<WGM12); //CTC mode, no prescale, timer STOPPED
  TCCR1C = 0; //don't force output compare

  //note: To do a 16-bit write, the high byte must be written before the low byte. For a 16-bit read, the low byte must be read before the high byte.
  
  TCNT1H = 0; //clear timer
  TCNT1L = 0;

  OCR1AH = 0x03; //set value for comparison: ADC max. sampling rate is ~19kHz nominally, use smaller than that but which is multiple of 1200 -> 18k
  OCR1AL = 0x79; // (16MHZ -> (no prescale) -> 18000 -> ~889 = 0x0379) TODO: PLUSMINUS 1 because of overflow?

  TIMSK1 = (1<<OCIE1A); //enable interrupt on compare match A
}

inline void setupUART(){
  //BAUD RATE: 250k to tranfer 18000 sampled bytes per sec with minimal UART timing error
  UBRR0H = 0; //UBRR0 = fosc/(8*BAUD) - 1, when speed mode is double
  UBRR0L = 7;
  UCSR0A = (1<<U2X0); //double speed mode
  UCSR0B = (1<<TXEN0); //enable TX
  UCSR0C = (1<<UCSZ01)|(1<<UCSZ00); //async, 8 bit data, 1 stop bit, no parity
}

inline void startTimer(){
  TCCR1B |= (1<<CS10); //start timer, no prescaler
}

inline void startADC(){
  ADCSRA |= (1<<ADSC); //begin new ADC conversion
}

int main(void){
  
  setupGPIO();
  setupADC();
  setupTimer();
  setupUART();
  SREG = (1<<7); //enable global interrupt

  startTimer();

  while(1){

  }
}

ISR(TIMER1_COMPA_vect){
  ADCin = ADCL;
  ADCin |= (ADCH<<8);

  UDR0 = (ADCin>>2) & 0xFF; //TODO: optimize shift value
  
  startADC();
}
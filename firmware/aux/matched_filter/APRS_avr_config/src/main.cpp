#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h> 
#include <stdint.h>

volatile uint16_t ADCin = 0x0000;

//TODO: számábrázolási formátumot kitalálni!!!

const uint16_t matchedfilter[4][7] PROGMEM =
{
  {0},  //1200Hz I
  {0},  //1200Hz Q
  {0},  //2200Hz I
  {0}   //2200Hz Q
};

inline void config(void)
{
  //GPIO CONFIG
  DDRB   = 0b00100000; //port I/O mode: 0 input, 1 output
  PORTB |= 0b00100000; //set PB5 (pin 13) high

  //TIMER CONFIG
  // 16-bit timer (Timer1) needed due to the limited ADC sampling rate thus more accurate timing required

  TCCR1A = 0b00000000; //OC0A, OC0B pins disconnected, CTC (clear timer on compare) mode
  TCCR1B = 0b00001000; //CTC mode, no prescale, timer STOPPED
  TCCR1C = 0b00000000; //don't force output compare

  //note: To do a 16-bit write, the high byte must be written before the low byte. For a 16-bit read, the low byte must be read before the high byte.

  TCNT1H = 0b00000000; //clear timer
  TCNT1L = 0b00000000; //clear timer

  OCR1AH = 0x07;       //set value for comparison: ADC max. sampling rate is ~9600Hz nominally, use smaller than that but which is multiple of 1200 -> 8400
  OCR1AL = 0x71;       // (16MHZ -> (no prescale) -> 8400 -> ~1905 = 0x0771) TODO: PLUSMINUS 1 because of overflow?

  TIMSK1 = 0b00000010; //enable interrupt on compare match A

  //ADC CONFIG
  ADMUX = 0b01000000; //AVcc as reference, right-adjusted result, source channel: ADC0

  ADCSRA = 0b10000111; //enable ADC, don't start yet, single conversion mode, disable interrupt, prescale: 128 (for 125kHz which is in the 50kHz-200kHz recommended range)
  //there comes the limitation: one ADC conversion takes ~13 cycle of the ADC clk. 125kHz -> 13 -> 9615Hz nominal!

  //in libaprs, the selected prescale is 16, and auto trigger is set at timer1 match ICR1

  DIDR0 = 0b001111111; //disable digital input buffer on all analog pins

  SREG |= (1 << 7); //enable global interrupt
  TCCR1B |= 1;      //start timer
}

int process(uint16_t)
{
  PORTB ^= 0b00100000;
  return 0;
}

ISR(TIMER1_COMPA_vect)  //Timer 1 interrupt routine, happens ~8400 times at every second
{
  //output prev data >>

  ADCin = ADCL; //read ADCL first then ADCH
  ADCin |= ADCH << 8;

  ADCSRA |= 1 << 6; //start new conversion

  process(ADCin);
}

int main(void)
{

  config();

  while (1);
  return 0;
}
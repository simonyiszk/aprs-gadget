#include <avr/io.h>
#include <avr/interrupt.h>
#include <FSK.h>

const uint8_t sine[198] = {
    0x7F,0x83,0x87,0x8B,0x8F,0x93,0x97,0x9B,0x9F,0xA3,0xA7,
    0xAB,0xAF,0xB2,0xB6,0xBA,0xBD,0xC1,0xC4,0xC8,0xCB,0xCE,
    0xD1,0xD4,0xD7,0xDA,0xDD,0xE0,0xE2,0xE5,0xE7,0xEA,0xEC,
    0xEE,0xF0,0xF2,0xF3,0xF5,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,
    0xFD,0xFE,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE,0xFE,
    0xFD,0xFC,0xFB,0xFA,0xF9,0xF8,0xF7,0xF5,0xF3,0xF2,0xF0,
    0xEE,0xEC,0xEA,0xE7,0xE5,0xE2,0xE0,0xDD,0xDA,0xD7,0xD4,
    0xD1,0xCE,0xCB,0xC8,0xC4,0xC1,0xBD,0xBA,0xB6,0xB2,0xAF,
    0xAB,0xA7,0xA3,0x9F,0x9B,0x97,0x93,0x8F,0x8B,0x87,0x83,
    0x7F,0x7B,0x77,0x73,0x6F,0x6B,0x67,0x63,0x5F,0x5B,0x57,
    0x53,0x4F,0x4C,0x48,0x44,0x41,0x3D,0x3A,0x36,0x33,0x30,
    0x2D,0x2A,0x27,0x24,0x21,0x1E,0x1C,0x19,0x17,0x14,0x12,
    0x10,0x0E,0x0C,0x0B,0x09,0x07,0x06,0x05,0x04,0x03,0x02,
    0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x09,0x0B,0x0C,0x0E,
    0x10,0x12,0x14,0x17,0x19,0x1C,0x1E,0x21,0x24,0x27,0x2A,
    0x2D,0x30,0x33,0x36,0x3A,0x3D,0x41,0x44,0x48,0x4C,0x4F,
    0x53,0x57,0x5B,0x5F,0x63,0x67,0x6B,0x6F,0x73,0x77,0x7B
};

static volatile bool FSK_bit = false;
static volatile bool EOP = false;

ISR(TIMER1_OVF_vect){

    static uint8_t dds_idx = 0;
    static uint8_t period = 0;

    OCR1AH = 0;
    OCR1AL = sine[dds_idx];
    
    if(FSK_bit == true){
        dds_idx += 6;
    }else{
        dds_idx += 11;
    }

    if(dds_idx > 197){
        dds_idx -= 198;
    }

    period++;
    if(period==33){
        period = 0;
        EOP = true;
    }
}

void FSK_init(void){
    DDRB = _BV(DDB1); //set PD9 as output (OC1A)
    
    TCNT1H = 0;     //reset timer
    TCNT1L = 0;

    TCCR1A = _BV(COM1A1) | _BV(WGM11); // Clear OC1A on compare match, Fast PWM mode, ICR1 as TOP
    TCCR1B = _BV(WGM13) | _BV(WGM12);

    OCR1AH = 0;     //load first value to compare to
    OCR1AL = sine[0];

#if F_CPU == 16000000L
    ICR1H = 0x01;   //load TOP value: 403 for 39.6kHz PWM
    ICR1L = 0x93;
#elif F_CPU == 12000000L
    ICR1H = 0x01;   //load TOP value: 302 for 39.6kHz PWM
    ICR1L = 0x2E;
#else
    #error Specified CPU frequency not supported
#endif

    TIMSK1 = _BV(TOIE1); //enable overflow interrupt
    sei();  //enable global inetrrupt
}


void FSK_start(void){
    TCCR1B |= _BV(CS00); //enable timer1 with no prescaler
}

void FSK_setbit(bool bit){
    while(!EOP);
    FSK_bit = bit;
    EOP = false;
}

void FSK_stop(void){
    TCCR1B &= ~_BV(CS00);
    PORTB &= ~_BV(PB1);
}
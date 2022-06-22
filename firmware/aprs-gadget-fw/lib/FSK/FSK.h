#ifndef _waveform_gen
#define _waveform_gen

#include <stdint.h>
#include <stdbool.h>

void FSK_init(void);
void FSK_start(void);
void FSK_setbit(bool bit);
void FSK_stop(void);

#endif
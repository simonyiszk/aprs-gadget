/*
 * bitfifo.h
 *
 *  Created on: Aug 28, 2022
 *      Author: pali
 */

#ifndef INC_FIFO_H_
#define INC_FIFO_H_

#include <stdint.h>

typedef struct {
	uint8_t *buffer;
	uint32_t size;
	uint32_t start, end;
} FIFO;

void FIFO_init(FIFO* fifo, uint8_t* array, uint32_t size_in_bytes);

void FIFO_push(FIFO* fifo, uint8_t bit);

uint8_t FIFO_pop(FIFO* fifo);

void FIFO_reset(FIFO *fifo);

uint8_t FIFO_isempty(FIFO *fifo);

#endif /* INC_FIFO_H_ */

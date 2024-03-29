/*
 * bitfifo.c
 *
 *  Created on: Aug 28, 2022
 *      Author: pali
 */

#include <fifo.h>

void FIFO_init(FIFO* fifo, uint8_t* array, uint32_t size_in_bytes){
	fifo->buffer = array;
	fifo->start = 0;
	fifo->end = 0;
	fifo->size = size_in_bytes << 3;
}

void FIFO_push(FIFO* fifo, uint8_t bit){
	if(bit){
		fifo->buffer[(fifo->end)>>3] |= (1 << ((fifo->end) % 8));
	}else{
		fifo->buffer[(fifo->end)>>3] &= ~(1 << ((fifo->end) % 8));
	}
	fifo->end = (fifo->end + 1) % fifo->size;
}

uint8_t FIFO_pop(FIFO* fifo){
	uint8_t bit = fifo->buffer[(fifo->start)>>3] & (1 << ((fifo->start) % 8));
	fifo->start = (fifo->start + 1) % fifo->size;
	return bit;
}

void FIFO_reset(FIFO *fifo){
	fifo->start = 0;
	fifo->end = 0;
}

uint8_t FIFO_isempty(FIFO *fifo){
	return (fifo->start==fifo->end ? 1 : 0);
}

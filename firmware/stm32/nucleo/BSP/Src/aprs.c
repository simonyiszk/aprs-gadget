/*
 * aprs.c
 *
 *  Created on: Aug 28, 2022
 *      Author: pali
 */

#include <string.h>
#include <aprs.h>
#include <fsk.h>

static uint16_t CRC_value = 0xFFFF;

static void reset_crc(void){
	CRC_value = 0xFFFF;
}

static void update_crc(uint8_t bit){

    uint16_t acc = CRC_value ^ bit;
    CRC_value >>= 1;

    if(acc & 0x01){
    	CRC_value ^= 0x8408;
    }
}

void APRS_Init(void){
    FSK_Init();
}

static void APRS_send_byte(uint8_t byte, uint8_t bitStuff){

    static uint8_t FSK_bit = 0;
    static uint8_t bit_count = 0;

    for(uint8_t i=0; i<8; i++){
        uint8_t current_bit = byte & 0x01;
        update_crc(current_bit);

        if(current_bit){
            FSK_AddBit(FSK_bit);
            bit_count++;
            if((bitStuff) && (bit_count==5)){
                FSK_bit ^= 1;
                FSK_AddBit(FSK_bit);
                bit_count = 0;
            }
        }else{
            FSK_bit ^= 1;
            FSK_AddBit(FSK_bit);
            bit_count = 0;
        }
        byte >>= 1;
    }
}

static void APRS_send_string(char* string, uint8_t size){
    for(uint8_t i=0; i<size; i++){
        APRS_send_byte(string[i], 1);
    }
}

static void APRS_send_flag(uint8_t count){
    for(uint8_t i=0; i<count; i++){
        APRS_send_byte(0x7E, 0);
    }
}

static void APRS_send_crc(void){
    uint8_t CRC_low = CRC_value ^ 0xFF;
    uint8_t CRC_high = (CRC_value >> 8) ^ 0xFF;
    APRS_send_byte(CRC_low, 1);
    APRS_send_byte(CRC_high, 1);
}

static void APRS_send_address(char* addr){
    uint8_t temp = strlen(addr);
    for(int j=0; j<temp; j++)
        APRS_send_byte(addr[j] << 1, 1);
    if(temp < 6){
        for(int j=0; j<(6 - temp); j++)
            APRS_send_byte(' ' << 1, 1);
    }
}

static void APRS_send_header(char* dest, char* src, uint8_t ssid, char* repeater, uint8_t hops){

    APRS_send_address(dest);
    APRS_send_byte('0'<<1, 1);

    APRS_send_address(src);
    APRS_send_byte((ssid+'0')<<1, 1);

    APRS_send_address(repeater);
    APRS_send_byte(((hops+'0')<<1)+1, 1);

    APRS_send_byte(0x03, 1);
    APRS_send_byte(0xF0, 1);
}

void APRS_SendPacket(void){
    APRS_send_flag(80);
    reset_crc();
    APRS_send_header("BEACON","HA3PB",1,"WIDE2",2);
    //APRS_send_string(">helo", 5);
    //APRS_send_string("/152100h4636.89N/01850.83ElTest", 31);
    APRS_send_string("!4636.89N/01850.83ElTest", 24);
    APRS_send_crc();
    APRS_send_flag(3);
    FSK_Send();
}

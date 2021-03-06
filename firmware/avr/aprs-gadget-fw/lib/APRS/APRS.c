#include <string.h>
#include <APRS.h>
#include <FSK.h>

static uint16_t CRC = 0xFFFF;

static void reset_crc(void){
    CRC = 0xFFFF;
}

static void update_crc(bool bit){
    
    uint16_t acc = CRC ^ bit;
    CRC >>= 1;

    if(acc & 0x01){
        CRC ^= 0x8408;
    }
}

void APRS_init(void){
    FSK_init();
}

static void APRS_send_byte(uint8_t byte, bool bitStuff){

    static bool FSK_bit = false;
    static uint8_t bit_count = 0;

    FSK_start();

    for(uint8_t i=0; i<8; i++){
        bool current_bit = byte & 0x01;
        update_crc(current_bit);

        if(current_bit){
            FSK_setbit(FSK_bit);
            bit_count++;
            if((bitStuff) && (bit_count==5)){
                FSK_bit ^= 1;
                FSK_setbit(FSK_bit);
                bit_count = 0;
            }
        }else{
            FSK_bit ^= 1;
            FSK_setbit(FSK_bit);
            bit_count = 0;
        }
        byte >>= 1;
    }

    FSK_stop();
}

static void APRS_send_string(char* string, uint8_t size){
    for(uint8_t i=0; i<size; i++){
        APRS_send_byte(string[i], true);
    }
}

static void APRS_send_flag(uint8_t count){
    for(uint8_t i=0; i<count; i++){
        APRS_send_byte(0x7E, false);
    }
}

static void APRS_send_crc(void){
    uint8_t CRC_low = CRC ^ 0xFF;
    uint8_t CRC_high = (CRC >> 8) ^ 0xFF;
    APRS_send_byte(CRC_low, true);
    APRS_send_byte(CRC_high, true);
}

static void APRS_send_address(char* addr){
    uint8_t temp = strlen(addr);
    for(int j=0; j<temp; j++)
        APRS_send_byte(addr[j] << 1, true);
    if(temp < 6){
        for(int j=0; j<(6 - temp); j++)
            APRS_send_byte(' ' << 1, true);
    }
}

static void APRS_send_header(char* dest, char* src, uint8_t ssid, char* repeater, uint8_t hops){
    
    APRS_send_address(dest);
    APRS_send_byte('0'<<1, true);

    APRS_send_address(src);
    APRS_send_byte((ssid+'0')<<1, true);

    APRS_send_address(repeater);
    APRS_send_byte(((hops+'0')<<1)+1, true);

    APRS_send_byte(0x03, true);
    APRS_send_byte(0xF0, true);
}

void APRS_send_packet(void){    
    APRS_send_flag(80);
    reset_crc();
    APRS_send_header("APRS","HA3PB",1,"WIDE2",2);    
    //APRS_send_string(">helo", 5);
    //APRS_send_string("/152100h4636.89N/01850.83ElTest", 31);
    APRS_send_string("!4636.89N/01850.83ElTest", 24);
    APRS_send_crc();
    APRS_send_flag(3);
}
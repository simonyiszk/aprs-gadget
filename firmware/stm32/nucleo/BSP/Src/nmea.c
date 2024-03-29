/*
 * nmea.c
 *
 *  Created on: Sep 3, 2022
 *      Author: pali
 */

#include <string.h>
#include <usart.h>
#include <nmea.h>

static NMEA_PacketTypeDef NMEA = {0};

extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_rx;

void NMEA_Receive(void){
	if(!NMEA.transfer_in_progress && !NMEA.packet_available){
		NMEA.transfer_in_progress = 1;
		HAL_UARTEx_ReceiveToIdle_DMA(&huart1, NMEA.packet, 1024);
		__HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);
	}
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if (huart->Instance == USART1){
		NMEA.packet_available = 1;
		NMEA.end = &NMEA.packet[Size];
		NMEA.transfer_in_progress = 0;
	}
}

uint8_t NMEA_PacketReady(void){
	return NMEA.packet_available;
}

uint8_t NMEA_ReadNextSentence(char* sentence){
	static uint8_t* next = NMEA.packet;

	if(NMEA.packet_available){
		uint8_t length = 0;
		while(*next !='\n'){
			*sentence = *next;
			sentence++;
			next++;
			length++;
		}
		*sentence = '\0';
		next++;

		//ready to receive and process next packet
		if(next==NMEA.end){
			next = NMEA.packet;
			NMEA.packet_available = 0;
		}
		return length;
	}
	return 0;
}

static uint8_t xorhash(const char* const sentence){
	uint8_t checksum = 0;
	uint8_t i = 0;

	if(sentence[i++]!='$'){								//sentence starts with '$'
		return 0xFF;
	}

	while(sentence[i] != '*' && sentence[i] != '\r'){	//calculate hash from sentence
		checksum ^= sentence[i++];
	}

	if(sentence[i]=='\0'){								//checksum missing
		return 0xFF;
	}

	i++; checksum -= (sentence[i] - (sentence[i] > '9' ? ('A'-10) : '0')) << 4;
	i++; checksum -= (sentence[i] - (sentence[i] > '9' ? ('A'-10) : '0'));

	//convert last 2 characters (checksum) to hex byte value and subtract them from hash
	//if the final checksum is 0, the sentence was correct

	return checksum;
}

uint8_t NMEA_SplitSentence(char* sentence, char** fields, uint8_t max_fields){

	if(xorhash(sentence)!=0){							//invalid sentence
		return 0;
	}

	uint8_t i=0;
	sentence++; 										//skip starting '$'

	for(i=0; i<max_fields && *sentence!='*'; i++){		//read sentence until '*' or next comma
		fields[i] = sentence;							//begin a field
		while(*sentence!=',' && *sentence!='*'){		//read characters into it
			sentence++;
		}
		if(*sentence==','){								//split sentence at comma
			*sentence = '\0';							//close the current field
			sentence++;
		}
	}
	*sentence = '\0'; 									//trim last field at '*'

	return i;											//return number of extracted field
}

NMEA_sentence_type NMEA_GetNMEASentenceType(const char* const field){
	if(strlen(field)!=5){
		return UNKNOWN;
	}
	if(strstr(field, "RMC")!=NULL){
		return RMC;
	}else if(strstr(field, "VTG")!=NULL){
		return VTG;
	}else if(strstr(field, "GGA")!=NULL){
		return GGA;
	}else if(strstr(field, "GSA")!=NULL){
		return GSA;
	}else if(strstr(field, "GSV")!=NULL){
		return GSV;
	}else if(strstr(field, "GLL")!=NULL){
		return GLL;
	}else if(strstr(field, "TXT")!=NULL){
		return TXT;
	}
	return UNKNOWN;
}

NMEA_talker_id NMEA_GetTalkerID(const char* const field){
	if(strlen(field)!=5){
		return ID_UNKNOWN;
	}
	if(field[1]=='P'){
		return ID_GPS;
	}else if(field[1]=='N'){
		return ID_GLONASS;
	}
	return ID_UNKNOWN;
}

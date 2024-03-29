/*
 * nmea.h
 *
 *  Created on: Sep 3, 2022
 *      Author: pali
 */

#ifndef INC_NMEA_H_
#define INC_NMEA_H_

#include <stdint.h>

typedef struct{
	uint8_t packet[1024];
	uint8_t* end;
	uint8_t transfer_in_progress;
	uint8_t packet_available;
}NMEA_PacketTypeDef;

typedef enum{
	UNKNOWN,
	RMC,
	VTG,
	GGA,
	GSA,
	QSA,
	GSV,
	GLL,
	TXT
}NMEA_sentence_type;

typedef enum{
	ID_GPS,
	ID_GLONASS,
	ID_UNKNOWN
}NMEA_talker_id;

void NMEA_Receive(void);
uint8_t NMEA_PacketReady(void);
uint8_t NMEA_ReadNextSentence(char* sentence);

uint8_t NMEA_SplitSentence(char* sentence, char** fields, uint8_t max_fields);
NMEA_sentence_type NMEA_GetSentenceType(const char* const field);
NMEA_talker_id NMEA_GetTalkerID(const char* const field);

#endif /* INC_NMEA_H_ */

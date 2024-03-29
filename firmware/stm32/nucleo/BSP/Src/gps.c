/*
 * gps.c
 *
 *  Created on: Aug 30, 2022
 *      Author: pali
 */

#include <stdio.h>
#include <string.h>
#include <nmea.h>
#include <gps.h>

static GPS_TypeDef GPS = {0};

uint8_t ReceiveGPS(void){

	char sentence[84];								//storing NMEA sentence
    char* field[MAX_NUMBER_OF_FIELDS];				//storing fields of the sentence

    if(NMEA_ReadNextSentence(sentence)){
        if(NMEA_SplitSentence(sentence, field, MAX_NUMBER_OF_FIELDS)){			//split sentence into fields
            NMEA_sentence_type type = NMEA_GetSentenceType(field[0]);
            if(type==RMC){														//date
                GPS.AVAILABLE_SATELLITES = 0;
				sscanf(field[9], "%02hhu%02hhu%02hu", &GPS.day, &GPS.month, &GPS.year);
                GPS.year += 2000;
            }else if(type==GGA){												//gps position data
                sscanf(field[1], "%02hhu%02hhu%02hhu", &GPS.hour, &GPS.min, &GPS.sec);
                getDecimalDegree(field[2], field[3], &GPS.LATITUDE);
                getDecimalDegree(field[4], field[5], &GPS.LONGITUDE);
                sscanf(field[7], "%02hhu", &GPS.USED_SATELLITES);
                sscanf(field[9], "%d", &GPS.ALTITUDE);
                GPS.validity = *field[6];
			}else if(type==VTG){												//ground speed
                sscanf(field[7], "%d", &GPS.SPEED);
            }else if(type==GSV){												//satellite info
				NMEA_talker_id id = NMEA_GetTalkerID(field[0]);					//"GP" or "GL" supported
				static uint8_t total_sequences[2] = {0, 0};
				static uint8_t sequence_number[2] = {0, 0};
				static uint8_t visible_satellites[2] = {0, 0};
				if(id!=ID_UNKNOWN){
					sscanf(field[1], "%hhu", &total_sequences[id]);
					sscanf(field[2], "%hhu", &sequence_number[id]);
					sscanf(field[3], "%hhu", &visible_satellites[id]);
					uint8_t satellite_count = 4;
					if(sequence_number[id]==total_sequences[id] && (total_sequences[id]%4)!=0){
						satellite_count = total_sequences[id] % 4;				//last sentence may contain
					}															//less than 4 satellites data
					for(uint8_t i=0; i<satellite_count; i++){
						uint8_t snr = 0;
						sscanf(field[7 + i*4], "%hhu", &snr);					//get signal-noise ratio of
						if(snr>0){												//each satellite
							GPS.AVAILABLE_SATELLITES++;
						}
					}
					GPS.VISIBLE_SATELLITES = visible_satellites[ID_GPS] + visible_satellites[ID_GLONASS];
				}
            }else if(type==TXT){												//end of data stream
                return 1;
            }
        }
    }
    return 0;
}

void GPS_GetMaidenheadLocator(char* locator_string, uint8_t resolution){
	const double scaling[6]={360., 360./18., 360./18./10., 360./18./10./24., 360./18./10./24./10., 360./18./10./24./10./24.};
	if(resolution > 5){
		resolution = 5;
	}
	if(GPS.validity=='1'){
		for (uint8_t i=0;i<resolution;i++)
		{
		locator_string[i*2] = ((int)floor(fmod((180.0+GPS.LONGITUDE), scaling[i]) / scaling[i+1])) + ((i&1) ? '0':'A');
		locator_string[i*2+1] = ((int)floor(fmod((90.0+GPS.LATITUDE), (scaling[i]/2)) / (scaling[i+1]/2))) + ((i&1) ? '0':'A');
		}
		locator_string[resolution*2]=0;
	}
}

static void getDecimalDegree(char* gpsposition, char* hemisphere, double* output){

	//converts coordinates from DDMM.MMMM format to DD.DDDDDD

	if(strlen(gpsposition)==0){
		return;
	}

	char* dot = gpsposition;						//for finding decimal point
	int16_t degrees = 0;
	uint32_t minutes = 0, decimal = 0;

	do{
		degrees = degrees * 10 + *dot - '0';    	//parsing degrees
	}while(*(++dot+2)!='.');  						//stops on 2nd char before '.'
	do{
		minutes = minutes * 10 + *dot - '0';		//parsing minutes
	}while(*(++dot)!='.');    						//stops on '.'
	while(*++dot){            						//read till end of string
		decimal = decimal * 10 + *dot - '0';
	}

	//converting coordinate format
  	*output = (degrees + (minutes + decimal / (double)10000) / 60) * (*hemisphere=='N' || *hemisphere=='E' ? 1 : -1);
}

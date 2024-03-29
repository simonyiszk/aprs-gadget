/*
 * gps.h
 *
 *  Created on: Aug 30, 2022
 *      Author: pali
 */

#ifndef INC_GPS_H_
#define INC_GPS_H_

#include <stdint.h>

typedef struct{
    double LATITUDE;
    double LONGITUDE;
    int ALTITUDE;
    int SPEED;

    uint8_t VISIBLE_SATELLITES;
    uint8_t AVAILABLE_SATELLITES;
    uint8_t USED_SATELLITES;

    uint16_t year;
    uint8_t month, day;
    uint8_t hour, min, sec;

    char validity;
}GPS_TypeDef;

static const uint8_t MAX_NUMBER_OF_FIELDS = 24;

uint8_t ReceiveGPS(void);
static void getDecimalDegree(char* gpsposition, char* hemisphere, double* output);

#endif /* INC_GPS_H_ */

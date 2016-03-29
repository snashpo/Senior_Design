/*
 * printer.c
 *
 *  Created on: Mar 20, 2016
 *      Author: Nash
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "display.h"
#include "retargettextdisplay.h"
#include "textdisplay.h"
#include "rtcdrv.h"
#include "bspconfig.h"
#include "em_device.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_lcd.h"
#include "em_system.h"
#include "em_timer.h"
#include "em_chip.h"
#include "em_rtc.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "printer.h"
#include "gps.h"
#include "sd_i2c.h"
#include "bmp180.h"
#include "lsm303.h"
#include "l3gd20h.h"
#include "ina219.h"


int16_t dc = 0;

void printGPS(int8_t nmea) {
    DISPLAY_Init();
	RETARGET_TextDisplayInit();
	printf("\f");
	printf("\n- ECEN 404 SatTrack -\n");
	printf("\n----- GPS  DEMO -----\n");


	//Time taken
	printf("\n Time: ");
	for(int x = 7; x<9; x=x+1){
		  printf("%c", (data[nmea][x]));
		}
	printf(":");
	for(int x = 9; x<11; x=x+1){
	  printf("%c", (data[nmea][x]));
	}
	printf(":");
	for(int x = 11; x<13; x=x+1){
		  printf("%c", (data[nmea][x]));
		}
	printf(" UTC");


	//Date taken
	printf("\n Date: ");
	for(int x = 59; x<61; x=x+1){
		  printf("%c", (data[nmea][x]));
		}
	printf("/");
	for(int x = 57; x<59; x=x+1){
	  printf("%c", (data[nmea][x]));
	}
	printf("/");
	for(int x = 61; x<63; x=x+1){
		  printf("%c", (data[nmea][x]));
		}


	//Latitude
	printf("\n\n Lat: ");
	for(int x = 20; x<22; x=x+1){
		  printf("%c", (data[nmea][x]));
		}
	printf(" Deg ");
	for(int x = 22; x<27; x=x+1){
	  printf("%c", (data[nmea][x]));
	}
	printf("'");
	printf("%c", (data[nmea][30]));


	//Longitude
	printf("\n Lon: ");
	for(int x = 33; x<35; x=x+1){
		  printf("%c", (data[nmea][x]));
		}
	printf(" Deg ");
	for(int x = 35; x<40; x=x+1){
	  printf("%c", (data[nmea][x]));
	}
	printf("'");
	printf("%c", (data[nmea][43]));


	//Speed (Knots)
	printf("\n\n Speed: ");
	for(int x = 45; x<49; x=x+1){
		  printf("%c", (data[nmea][x]));
		}
	printf(" Knots ");

	//Page Number
	printf("\n\n\n     Reading: %d",(nmea));

}

void printSENSORS()
{
	printf("\n\n Gyroscope\n X: \n Y: \n Z: \n\n Accelerometer\n X: \n Y: \n Z: \n\n Weather\n T: \n P: \n\n Voltage: \n");

	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_UP_ONE_LINE );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_UP_ONE_LINE );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_UP_ONE_LINE );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_UP_ONE_LINE );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_UP_ONE_LINE );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_UP_ONE_LINE );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_UP_ONE_LINE );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_UP_ONE_LINE );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_UP_ONE_LINE );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_UP_ONE_LINE );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_UP_ONE_LINE );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_UP_ONE_LINE );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_UP_ONE_LINE );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_UP_ONE_LINE );

	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );

	printf("%d\n", (Gyro.x));
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf("%d\n", (Gyro.y));
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf("%d\n\n\n", (Gyro.z));
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf("%d\n", (Accel.x));
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf("%d\n", (Accel.y));
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf("%d\n\n\n", (Accel.z));
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf("%d C\n", (BMP.temp));
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf("%d Pa\n\n", (BMP.pres));
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf("%d Pa\n", (INA.shunt_voltage));
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_RIGHT_ONE_CHAR );

	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_UP_ONE_LINE );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_UP_ONE_LINE );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_UP_ONE_LINE );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_UP_ONE_LINE );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_UP_ONE_LINE );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_UP_ONE_LINE );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_UP_ONE_LINE );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_UP_ONE_LINE );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_UP_ONE_LINE );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_UP_ONE_LINE );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_UP_ONE_LINE );
	printf( TEXTDISPLAY_ESC_SEQ_CURSOR_UP_ONE_LINE );
}


void concact(){
	//printf("\n ");

	con[dc] = '/';
	dc++;
	con[dc] = 'g';
	dc++;
	con[dc] = 'x';
	dc++;
	con[dc] = ':';
	dc++;
	sprintf(gyrox, "%d", Gyro.x);
	for(int i = 0; i < strlen(gyrox); i++) {
	con[dc] = gyrox[i];
	dc++;
	}


	con[dc] = '/';
	dc++;
	con[dc] = 'g';
	dc++;
	con[dc] = 'y';
	dc++;
	con[dc] = ':';
	dc++;
	sprintf(gyrox, "%d", Gyro.y);
	for(int i = 0; i < strlen(gyrox); i++) {
	con[dc] = gyrox[i];
	dc++;
	}

	con[dc] = '/';
	dc++;
	con[dc] = 'g';
	dc++;
	con[dc] = 'z';
	dc++;
	con[dc] = ':';
	dc++;
	sprintf(gyrox, "%d", Gyro.z);
	for(int i = 0; i < strlen(gyrox); i++) {
	con[dc] = gyrox[i];
	dc++;
	}

	con[dc] = '/';
	dc++;
	con[dc] = 'a';
	dc++;
	con[dc] = 'x';
	dc++;
	con[dc] = ':';
	dc++;
	sprintf(gyrox, "%d", Accel.x);
	for(int i = 0; i < strlen(gyrox); i++) {
	con[dc] = gyrox[i];
	dc++;
	}


	con[dc] = '/';
	dc++;
	con[dc] = 'a';
	dc++;
	con[dc] = 'y';
	dc++;
	con[dc] = ':';
	dc++;
	sprintf(gyrox, "%d", Accel.y);
	for(int i = 0; i < strlen(gyrox); i++) {
	con[dc] = gyrox[i];
	dc++;
	}


	con[dc] = '/';
	dc++;
	con[dc] = 'a';
	dc++;
	con[dc] = 'z';
	dc++;
	con[dc] = ':';
	dc++;
	sprintf(gyrox, "%d", Accel.z);
	for(int i = 0; i < strlen(gyrox); i++) {
	con[dc] = gyrox[i];
	dc++;
	}


	con[dc] = '/';
	dc++;
	con[dc] = 't';
	dc++;
	con[dc] = 'e';
	dc++;
	con[dc] = ':';
	dc++;
	sprintf(gyrox, "%d", BMP.temp);
	for(int i = 0; i < strlen(gyrox); i++) {
	con[dc] = gyrox[i];
	dc++;
	}


	con[dc] = '/';
	dc++;
	con[dc] = 'p';
	dc++;
	con[dc] = 'r';
	dc++;
	con[dc] = ':';
	dc++;
	sprintf(gyrox, "%d", BMP.pres);
	for(int i = 0; i < strlen(gyrox); i++) {
	con[dc] = gyrox[i];
	dc++;
	}


	con[dc] = '/';
	dc++;
	con[dc] = 'b';
	dc++;
	con[dc] = 'v';
	dc++;
	con[dc] = ':';
	dc++;
	sprintf(gyrox, "%d", INA.shunt_voltage);
	for(int i = 0; i < strlen(gyrox); i++) {
	con[dc] = gyrox[i];
	dc++;
	}

	con[dc] = '/';
	dc++;
	con[dc] = 'g';
	dc++;
	con[dc] = 'p';
	dc++;
	con[dc] = ':';
	dc++;
	for(int i = 0; i < 71; i++) {
	con[dc] = data[0][i];
	dc++;
	}



}

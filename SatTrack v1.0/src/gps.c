
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "display.h"
#include "retargettextdisplay.h"
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
#include "gps.h"

#define GPS_PORT gpioPortC
#define TX_PIN 0
#define RX_PIN 1
#define ENABLE 2
#define FIX 13

#define LED_PORT    gpioPortC
#define LED0_PIN    10
#define LED1_PIN	11

#define EXT_LED		gpioPortF
#define RED_LED		3
#define YEL_LED		4
#define GRE_LED		5

#define BUT_PORT	gpioPortC
#define LEFT_BUT	9
#define RIGHT_BUT	8
#define I2C_PORT	gpioPortE

#define TERMINATION_CHAR   0x0A

const char nmea_output[] = "$PMTK314,1,1,1,1,1,5,1,1,1,1,1,1,0,1,1,1,1*2D<CR><LF> ";
const char ack[] = "$PMTK001,101,0*33<CR><LF>";
const char cold_restart[] = "$PMTK104*37<CR><LF>";
const char cold_start[] = "$PMTK103*30<CR><LF>";

//char data[21][72];
char rx_char = 0;

volatile uint32_t msTicks;		//count 1ms time ticks

int8_t n = 0;
int8_t b = 0;

void initGPS() {

	CMU_ClockEnable(cmuClock_GPIO, true);                   // enable GPIO peripheral clock
	CMU_ClockEnable(cmuClock_USART1, true);                 // enable USART1 peripheral clock

	GPIO_PinModeSet(GPS_PORT, TX_PIN, gpioModePushPull, 1); // set TX pin to push-pull output, initialize high (otherwise glitches can occur)
	GPIO_PinModeSet(GPS_PORT, RX_PIN, gpioModeInput, 0);    // set RX pin as input (no filter)

	USART_InitAsync_TypeDef uartInit =
	{
		.enable = usartDisable,     // wait to enable transmitter and receiver
		.refFreq = 0,               // setting refFreq to 0 will invoke the CMU_ClockFreqGet() function and measure the HFPER clock
		.baudrate = 9600,         // desired baud rate
		.oversampling = usartOVS16, // set oversampling to x16
		.databits = usartDatabits8, // 8 data bits
		.parity = usartNoParity,    // no parity bits
		.stopbits = usartStopbits1, // 1 stop bit
		.mvdis = false,             // use majority voting
		.prsRxEnable = false,       // not using PRS input
		.prsRxCh = usartPrsRxCh0,   // doesn't matter what channel we select
	};
	USART_InitAsync(USART1, &uartInit);      // apply configuration to USART1, where pins are connected
	USART1->ROUTE = USART_ROUTE_RXPEN | USART_ROUTE_TXPEN | _USART_ROUTE_LOCATION_LOC0; // clear buffers, enable transmitter and receiver pins

	USART_IntClear(USART1, _USART_IF_MASK);  // clear all USART interrupt flags
	NVIC_ClearPendingIRQ(USART1_RX_IRQn);    // clear pending RX interrupt flag in NVIC
	NVIC_ClearPendingIRQ(USART1_TX_IRQn);    // clear pending TX interrupt flag in NVIC

	USART_Enable(USART1, usartEnable);       // enable transmitter and receiver


}

void sendnmea() {
	  for(int i=0; i<sizeof(nmea_output) - 1; i++) {
	    while(!(USART1->STATUS & (1 << 6)));   // wait for TX buffer to empty
	    USART1->TXDATA = nmea_output[i];       // send character
	  }
}

void sendack() {
	  for(int i=0; i<sizeof(ack); i++) {
	    while(!(USART1->STATUS & (1 << 6)));   // wait for TX buffer to empty
	    USART1->TXDATA = ack[i];       // send character
	  }
}

void coldrestart() {
	  for(int i=0; i<sizeof(cold_restart); i++) {
	    while(!(USART1->STATUS & (1 << 6)));   // wait for TX buffer to empty
	    USART1->TXDATA = cold_restart[i];       // send character
	  }
}

void printstuff(int8_t nmea) {
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

void readdata() {
	fullread = 0;
	if(USART1->STATUS & (1 << 7)) {        // if RX buffer contains valid data
		rx_char = USART1->RXDATA;            // store the data
		data[b][n] = rx_char;					// place char into array
		n = n + 1;
		if(rx_char == 10){		//if char is newline
			if(data[b][3] != 82 && data[b][4] != 77 && data[b][5] != 67){			//check for RMC data only
				n = 0;
			}
			else {		//if RMC, move to next position in data array and go back to row
				b = b + 1;
				n = 0;
			}
		}
		if(b > 1) {		//if at end of array, go back and overwrite starting at 1
					b = 0;
					fullread = 1;
				}
		//if(c > 21) {
			//c = 0;
			//break;
		//}
	  }
}

void GPStimeout() {
	for(int t = 0; t <21; t++) {
		data[t][0] = 84;
		data[t][1] = 73;
		data[t][2] = 77;
		data[t][3] = 69;
		data[t][4] = 79;
		data[t][5] = 85;
		data[t][6] = 84;
	}

}






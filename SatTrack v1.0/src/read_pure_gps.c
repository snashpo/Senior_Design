
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

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

#include "display.h"
#include "retargettextdisplay.h"
#include "bspconfig.h"



#define GPS_PORT gpioPortC
#define TX_PIN 0
#define RX_PIN 1

#define LED_PORT    gpioPortC
#define LED0_PIN    10
#define LED1_PIN	11

#define EXT_LED		gpioPortF
#define RED_LED		3
#define YEL_LED		4
#define GRE_LED		5

const char nmea_output[] = "$PMTK514,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1*2D<CR><LF> ";
const char ack[] = "$PMTK001,101,0*33<CR><LF>";
const char cold_restart[] = "$PMTK104*37<CR><LF>";
const char cold_start[] = "$PMTK103*30<CR><LF>";

char data[21][75];
char rx_char = 0;
int word = 0;

volatile uint32_t msTicks;		//count 1ms time ticks

int8_t n = 0;
int8_t c = 0;
int8_t r = 0;

void initGPS() {

	CMU_OscillatorEnable(cmuOsc_HFXO, true, true);          // enable HF XTAL osc and wait for it to stabilize
	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);        // select HF XTAL osc as system clock source (24MHz)

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

void coldrestart() {
	  for(int i=0; i<sizeof(cold_restart); i++) {
	    while(!(USART1->STATUS & (1 << 6)));   // wait for TX buffer to empty
	    USART1->TXDATA = cold_restart[i];       // send character
	  }
}

void readdata() {
	if(USART1->STATUS & (1 << 7)) {        // if RX buffer contains valid data
		rx_char = USART1->RXDATA;            // store the data
		data[c][n] = rx_char;					// place char into array
		n = n + 1;
		if(rx_char == 10){
				c = c + 1;
				n = 0;
			}
		//if(c > 21) {
			//c = 0;
			//break;
		//}
	  }
}

void initGPIO()
{
	  CMU_ClockEnable(cmuClock_GPIO, true);                           // enable GPIO peripheral clock
	  GPIO_PinModeSet(LED_PORT, LED0_PIN, gpioModePushPull, 0);       // configure LED0 pin as push-pull output with standard drive strength
	  GPIO_PinModeSet(LED_PORT, LED1_PIN, gpioModePushPull, 0);  // configure LED1 pin as push-pull output with alternate drive strength
	  GPIO_PinModeSet(EXT_LED, RED_LED, gpioModePushPull, 0);  // configure LED1 pin as push-pull output with alternate drive strength
	  GPIO_PinModeSet(EXT_LED, YEL_LED, gpioModePushPull, 0);  // configure LED1 pin as push-pull output with alternate drive strength
	  GPIO_PinModeSet(EXT_LED, GRE_LED, gpioModePushPull, 0);  // configure LED1 pin as push-pull output with alternate drive strength
	  GPIO_DriveModeSet(LED_PORT, gpioDriveModeLowest);               // set alternate drive strength to lowest setting (0.5mA)
	  GPIO_PinOutClear(LED_PORT, LED0_PIN);                             // turn on LED0
	  GPIO_PinOutClear(LED_PORT, LED1_PIN);                             // turn on LED1
	  GPIO_PinOutClear(EXT_LED, RED_LED);
	  GPIO_PinOutClear(EXT_LED, YEL_LED);                           // turn on LED1
	  GPIO_PinOutClear(EXT_LED, GRE_LED);
}


int main () {

  CHIP_Init();
  initGPS();
  coldrestart();
  initGPIO();
  sendnmea();
  while(1) {
	  GPIO_PinOutToggle(EXT_LED, RED_LED);
	  readdata();
	  if(c > 21) {
			c = 0;
			GPIO_PinOutClear(EXT_LED, RED_LED);
			GPIO_PinOutToggle(EXT_LED, GRE_LED);
		}
  	  }

}


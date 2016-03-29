/*
 * main.c
 *
 *  Created on: Mar 20, 2016
 *      Author: Nash Porter
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
#include "em_i2c.h"


#include "sd_i2c.h"
#include "bmp180.h"
#include "lsm303.h"
#include "l3gd20h.h"
#include "ina219.h"
#include "gps.h"
#include "printer.h"




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

#define I2C_PORT	gpioPortE		//Port E
#define I2C_SDA_PIN 12		//SDA connected to pin 12
#define I2C_SCL_PIN	13		//SCL connected to pin 13
#define SENSOR_SLAVE_ADDR 0x39		//BMP180 Temperature sensor address register address
#define TOP 27342


char data[2][72];		//GPS Data

volatile uint32_t msTicks;		//count 1ms time ticks

int8_t c = 0;
int8_t r = 0;
int8_t fullread = 0;

int8_t fix_val = 0;
int8_t satfix = 0;
int8_t timeout = 0;

int32_t inter = 1000;
int32_t ud = 0;
int counter = 0;

volatile uint32_t msTicks; /* counts 1ms timeTicks */

/* Local prototypes */
void Delay(uint32_t dlyTicks);

//char datat[100];
//char gyrox[8];

void initMCU() {

	CMU_OscillatorEnable(cmuOsc_HFXO, true, true);          // enable HF XTAL osc and wait for it to stabilize
	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);        // select HF XTAL osc as system clock source (24MHz)
	CMU_HFRCOBandSet(cmuHFRCOBand_7MHz);		//Select 7Mhz Clock
}

void initGPIO()
{
	  CMU_ClockEnable(cmuClock_GPIO, true);                           // enable GPIO peripheral clock

	  GPIO_PinModeSet(GPS_PORT, FIX, gpioModeInput, 0);    // set FIX pin as input (no filter)
	  GPIO_PinModeSet(GPS_PORT, ENABLE, gpioModePushPull, 0);    // set ENABLE pin as output, initialize low

	  GPIO_PinModeSet(LED_PORT, LED0_PIN, gpioModePushPull, 0);       // configure LED0 pin as push-pull output with standard drive strength
	  GPIO_PinModeSet(LED_PORT, LED1_PIN, gpioModePushPull, 0);  // configure LED1 pin as push-pull output with alternate drive strength
	  GPIO_PinModeSet(EXT_LED, RED_LED, gpioModePushPull, 0);  // configure LED1 pin as push-pull output with alternate drive strength
	  GPIO_PinModeSet(EXT_LED, YEL_LED, gpioModePushPull, 0);  // configure LED1 pin as push-pull output with alternate drive strength
	  GPIO_PinModeSet(EXT_LED, GRE_LED, gpioModePushPull, 0);  // configure LED1 pin as push-pull output with alternate drive strength
	  GPIO_DriveModeSet(LED_PORT, gpioDriveModeLowest);               // set alternate drive strength to lowest setting (0.5mA)
	  GPIO_PinOutClear(LED_PORT, LED0_PIN);                             // turn on LED0
	  GPIO_PinOutSet(LED_PORT, LED1_PIN);                             // turn on LED1
	  GPIO_PinOutClear(EXT_LED, RED_LED);
	  GPIO_PinOutClear(EXT_LED, YEL_LED);                           // turn on LED1
	  GPIO_PinOutClear(EXT_LED, GRE_LED);

	  GPIO_PinModeSet(BUT_PORT, LEFT_BUT, gpioModeInputPull, 1);		//configure left button as input with pull-up enabled
	  GPIO_PinModeSet(BUT_PORT, RIGHT_BUT, gpioModeInputPull, 1);		//'' but with right button

	  NVIC_EnableIRQ(GPIO_ODD_IRQn);	//enable gpio_even interrupt vector in nvic
	  GPIO_IntConfig(GPS_PORT, FIX, true, false, true);		//configure FIX pin interrupt on rising
}

void GPIO_ODD_IRQHandler(void)
{
	uint16_t intFlags = GPIO_IntGet();
	/* clear flag for FIX interrupt */
	GPIO_IntClear(intFlags);
	GPIO_PinOutToggle(EXT_LED, RED_LED);
	fix_val++;
}



void initTimer()
{
  TIMER_Init_TypeDef initValues = TIMER_INIT_DEFAULT;

  /* Enable clock for TIMER0 */
  CMU_ClockEnable(cmuClock_TIMER0, true);
  CMU_ClockEnable(cmuClock_I2C0, true);

  /* Enable underflow and overflow interrupt for TIMER0*/
  TIMER_IntEnable(TIMER0, TIMER_IF_OF);

  /* Enable TIMER0 interrupt vector in NVIC */
  NVIC_EnableIRQ(TIMER0_IRQn);

  /* Set TIMER0 Top value */
  //TIMER_TopSet(TIMER0, 27342);
  TIMER_TopSet(TIMER0, 7000);

  /* Initialize TIMER0 in Up mode with 1024x prescaling */
  initValues.prescale = timerPrescale1024;
  initValues.mode     = timerModeUp;
  TIMER_Init(TIMER0, &initValues);

  /* Start TIMER0 */
  TIMER0->CMD = TIMER_CMD_START;
}

void TIMER0_IRQHandler(void)
{
  //int32_t bmp180_raw;
  /* Store the interrupt flags before clearing */
  uint16_t intFlags = TIMER_IntGet(TIMER0);

  /* Clear the interrupt flags. Only clear the flags that were stored in */
  /* intFlags in case other flags have been set since then */
  TIMER_IntClear(TIMER0, intFlags);

  /* Overflow interrupt occurred */
  if(intFlags & TIMER_IF_OF)
  {
	  GPIO_PinOutToggle(LED_PORT, LED0_PIN );                             // turn on LED0
	  GPIO_PinOutToggle(LED_PORT, LED1_PIN );                             // turn on LED0
	 //GPIO_PinOutToggle(EXT_LED, YEL_LED);
	 // GPIO_PinOutToggle(LED_PORT, LED1_PIN );
	 // GPIO_PinOutToggle(LED_PORT, OPLED_PIN);

	  timeout++;

	  counter = counter + 1;
	  if(counter > 10) {		//count to 10. then check to see how many fix vals have occured.
		  if(fix_val < 3) {		//if fix_val is greater than 2, then no fix. If less than 2, then solid fix
		  		  fix_val = 0;
		  		  satfix = 1;		//solid fix, so notify
		  	  }
		  else {
			  satfix = 0;
		  }
		  counter = 0;		//reset counter
	  }

  }

}

/**************************************************************************//**
 * @brief SysTick_Handler
 * Interrupt Service Routine for system tick counter
 *****************************************************************************/
void SysTick_Handler(void)
{
  msTicks++;       /* increment counter necessary in Delay()*/
}

/**************************************************************************//**
 * @brief Delays number of msTick Systicks (typically 1 ms)
 * @param dlyTicks Number of ticks to delay
 *****************************************************************************/
void Delay(uint32_t dlyTicks)
{
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks) ;
}




/****************************************************************************************************************************/
/****************************************************************************************************************************/
/*														MAIN																*/

int main () {

	CHIP_Init();				//Initialize Chip

	initMCU();
	initGPIO();				//Initialize GPIO
	sdi2c_Init();				//Initialize I2C
	initTimer();		//initialize timer for timer interuprt

	/* Setup SysTick Timer for 10 msec interrupts  */
	if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) {
	while (1) ;
	}

	/*Enable all sensors and GPS */
	GPIO_PinOutSet(GPS_PORT, ENABLE);			//Enable GPS Sensor board
	BMP180_GetCalData();		//Enable and get calibration data from BMP180
	initLSM303_LowPower();		//Enable Accel in low power mode
	initL3GD20H_Normal();			//Enable Gyro in normal mode
	GPIO_PinOutToggle(EXT_LED, YEL_LED);

	/* Acceleromter Data */
	Delay(10);
	LSM303_GetAccelData();
	LSM303_PowerOff();
	GPIO_PinOutToggle(EXT_LED, YEL_LED);

	/*Gyroscope Data */
	Delay(10);
	L3GD20H_GetGyroData();
	L3GD20H_PowerOff();
	GPIO_PinOutToggle(EXT_LED, YEL_LED);

	/*Temperature/Barometric Pressure */
	Delay(10);
	BMP180_GetTemp();
	BMP180_GetPressure();
	BMP180_CalcRealTemperature();
	BMP180_CalcRealPressure();
	GPIO_PinOutToggle(EXT_LED, YEL_LED);

	/*Power Sensor*/
	INA219_GetVoltage();
	GPIO_PinOutClear(EXT_LED, YEL_LED);

	GPIO_PinOutToggle(EXT_LED, GRE_LED);
	Delay(100);
	GPIO_PinOutToggle(EXT_LED, GRE_LED);


	/* GPS Information */
	timeout = 0;		//reset timeout
	initGPS();		//Initialize GPS UART
	coldrestart();		//Send cold restart command
	while(fullread == 0) {		//while not a full read, do the following
		if(satfix == 1) {			//if there is a satellite fix, then...
			GPIO_PinOutSet(EXT_LED, YEL_LED);		//toggle LED for debug
			readdata();		//readdata
			timeout = 0;
		}
		if(timeout > 20) {
			GPIO_PinOutClear(EXT_LED, YEL_LED);
			GPStimeout();
			break;
		}
	}

	GPIO_PinOutSet(EXT_LED, GRE_LED);
	Delay(500);

	printGPS(0);		//Print GPS to screen for debug

	concact();


	/* Infinite loop */
	while(1) {
	  if(GPIO_PinInGet(BUT_PORT, RIGHT_BUT) == 0) {
		  r = r + 1;
		  if(r > 2) {		//wrap around to beginning
			  r = 0;
		  }
		  printGPS(r);
	  }
	  if(GPIO_PinInGet(BUT_PORT, LEFT_BUT) == 0) {
			  r = r - 1;
			  if(r < 0) {		//wrap around to beginning
				  r = 2;
			  }
			  printSENSORS();
		  }
//	  if(GPIO_PinInGet(GPS_PORT, FIX) == 1) {
//		  GPIO_PinOutToggle(EXT_LED, RED_LED);
//	  }
	}
}



#include <stdio.h>
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
#include "em_i2c.h"
#include "em_usart.h"
#include "display.h"
#include "retargettextdisplay.h"
#include "textdisplay.h"
#include "bspconfig.h"
#include "rtcdrv.h"
#include "sd_i2c.h"
#include "bmp180.h"
#include "lsm303.h"
#include "l3gd20h.h"
#include "ina219.h"


#define LED_PORT    gpioPortC
#define LED0_PIN    10
#define LED1_PIN	11

#define EXT_LED		gpioPortF
#define RED_LED		3
#define YEL_LED		4
#define GRE_LED		5

#define I2C_PORT	gpioPortE		//Port E
#define I2C_SDA_PIN 12		//SDA connected to pin 12
#define I2C_SCL_PIN	13		//SCL connected to pin 13

#define SENSOR_SLAVE_ADDR 0x39		//BMP180 Temperature sensor address register address
#define TOP 27342

#define BUT_PORT	gpioPortC
#define LEFT_BUT	9
#define RIGHT_BUT	8

int32_t inter = 1000;
int32_t ud = 0;
int counter = 0;

volatile uint32_t msTicks; /* counts 1ms timeTicks */

/* Local prototypes */
void Delay(uint32_t dlyTicks);




//*********************************************************************************************LSM303 Data************************************************************************************
//*********************************************************************************************LSM303 Data************************************************************************************
//*********************************************************************************************LSM303 Data************************************************************************************

/*struct lsm303 {
	int lx;
	int hx;
	int ly;
	int hy;
	int lz;
	int hz;
};*/
//struct lsm303 Accel;


//*********************************************************************************************Everything Else************************************************************************************
//*********************************************************************************************Everything Else************************************************************************************
//*********************************************************************************************Everything Else************************************************************************************

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
  TIMER_TopSet(TIMER0, 27342);

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
	  GPIO_PinOutToggle(EXT_LED, YEL_LED);
	 // GPIO_PinOutToggle(LED_PORT, LED1_PIN );
	 // GPIO_PinOutToggle(LED_PORT, OPLED_PIN);
	  counter = counter + 1;

  }

  //Read part# and rev ID from sensor (see BMP180 datasheet for details)
  /*
  			tx_buffer[0] = 0x2E;                           // access ID register

  			sensor_message.addr = (SENSOR_SLAVE_ADDR << 1);
  			sensor_message.flags = I2C_FLAG_WRITE_READ;    // indicate combined write/read
  			sensor_message.buf[0].len = 1;                 // specify # bytes to be written
  			sensor_message.buf[0].data = tx_buffer;        // point to tx_buffer
  			sensor_message.buf[1].len = 1;                 // specify # bytes to be read
  			sensor_message.buf[1].data = rx_buffer;        // point to rx_buffer

  			ret = I2C_TransferInit(I2C0, &sensor_message); // start I2C write/read transaction with sensor
  			while(ret == i2cTransferInProgress) {          // continue until all data has been received
  				ret = I2C_Transfer(I2C0);
  			}
	*/

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

	  GPIO_PinModeSet(BUT_PORT, LEFT_BUT, gpioModeInputPull, 1);		//configure left button as input with pull-up enabled
	  GPIO_PinModeSet(BUT_PORT, RIGHT_BUT, gpioModeInputPull, 1);		//'' but with right button
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

void updatescreen()
{
	//printf("  %dT     %dPa  \r", (real_temp), (real_pres));
	//printf(" %d %d %d  \r", (Accel.hx), (Accel.hy), (Accel.hz));
	//printf(" %d  \r", (Accel.x_long));
	//printf("   %d  %d  %d  \r", (Accel.x), (Accel.y), (Accel.z));
	//printf(" %d %d %d  \r", (Gyro.hx), (Gyro.hy), (Gyro.hz));
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
	printf("%d Pa\n", (BMP.pres));
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
	printf("\n ");
	datat[0] = '/';
	datat[1] = 'g';
	datat[2] = 'x';
	datat[3] = ':';
	sprintf(gyrox, "%d", Gyro.x);
	datat[4] = gyrox[0];
	datat[5] = gyrox[1];
	datat[6] = gyrox[2];
	datat[7] = gyrox[3];
	datat[8] = '/';
	datat[9] = 'g';
	datat[10] = 'y';
	datat[11] = ':';
	sprintf(gyrox, "%d", Gyro.y);
	datat[12] = gyrox[0];
	datat[13] = gyrox[1];
	datat[14] = '/';
	datat[15] = 'g';
	datat[16] = 'z';
	datat[17] = ':';
	sprintf(gyrox, "%d", Gyro.z);
	datat[18] = gyrox[0];
	for(int i = 0; i <25 ; i++) {
		printf("%c" , (datat[i]));
	}


}





//********************************************************************************************* Main ************************************************************************************
//********************************************************************************************* Main ************************************************************************************
//********************************************************************************************* Main ************************************************************************************



int main(void){

	CHIP_Init();		//initialize chip defaults
	//initTimer();		//initialize timer for interrupt
	initGPIO();			//initialize GPIO

	DISPLAY_Init();
	RETARGET_TextDisplayInit();

	/* Setup SysTick Timer for 10 msec interrupts  */
	if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000))
	{
	while (1) ;
	}

	printf("\n Gyroscope\n X: \n Y: \n Z: \n\n Accelerometer\n X: \n Y: \n Z: \n\n Weather\n T: \n P: \n");

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

	sdi2c_Init();
	BMP180_GetCalData();
	initLSM303_LowPower();
	initL3GD20H_Normal();
	//initINA219_Normal();
	//LSM303_Zero();

	while(1) {
		GPIO_PinOutToggle(EXT_LED, GRE_LED);
		if(GPIO_PinInGet(BUT_PORT, RIGHT_BUT) == 0) {
			LSM303_Zero();
		}

		initLSM303_LowPower();
		//Delay(10);
		LSM303_GetAccelData();
		LSM303_PowerOff();

		//initL3GD20H_Normal();
		//Delay(10);
		L3GD20H_GetGyroData();
		//L3GD20H_PowerOff();

		BMP180_GetTemp();
		BMP180_GetPressure();
		BMP180_CalcRealTemperature();
		BMP180_CalcRealPressure();

		INA219_GetVoltage();
		//concact();
		//break;
		GPIO_PinOutToggle(EXT_LED, GRE_LED);


		updatescreen();

	}

}

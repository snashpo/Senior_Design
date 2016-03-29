

#ifndef GPS_H_
#define GPS_H_


//#include "em_device.h"
//#include "em_emu.h"
//#include "em_gpio.h"
//#include "em_chip.h"
//#include "em_cmu.h"
//#include "em_timer.h"
//#include "bsp.h"

//Global Variables
extern int8_t fullread;
extern char data[2][72];

//Declare Functions
void initGPS();
void sendnmea();
void sendack();
void coldrestart();
void checkdata();
void readdata();
void printstuff(int8_t);
void GPStimeout();

// Simple delay on SysTick
void Delay(uint32_t dlyTicks);
extern volatile uint32_t msTicks;


#endif /* GPS_H_ */

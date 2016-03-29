
#include <stdio.h>
#include <stdlib.h>
//#include <math.h>

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
#include "sd_i2c.h"
#include "bmp180.h"

#define OSS 0

/*int16_t AC1;
int16_t AC2;
int16_t AC3;
uint16_t AC4;
uint16_t AC5;
uint16_t AC6;
int16_t B1;
int16_t B2;
int16_t MB;
int16_t MC;
int16_t MD;

int32_t X1;
int32_t X2;
int32_t X3;
int32_t B3;
uint32_t B4;
int32_t B5;
int32_t B6;
int32_t B7;*/

int32_t P;
int T = 0;

double po = 1013.25;
double alt = 0;




void BMP180_GetCalData(void){		//described in BMP180 datasheet on page 13

	BMP.AC1 = sdi2c_ReadReg(0xee, 0xaa);
	//  AC1 = 408;
	BMP.AC2 = sdi2c_ReadReg(0xee, 0xac);
	//  AC2 = -72;
	BMP.AC3 = sdi2c_ReadReg(0xee, 0xae);
	BMP.AC4 = sdi2c_ReadReg(0xee, 0xb0);
	BMP.AC5 = sdi2c_ReadReg(0xee, 0xb2);
	BMP.AC6 = sdi2c_ReadReg(0xee, 0xb4);
	BMP.B1 = sdi2c_ReadReg(0xee, 0xb6);
	BMP.B2 = sdi2c_ReadReg(0xee, 0xb8);
	BMP.MB = sdi2c_ReadReg(0xee, 0xba);
	BMP.MC = sdi2c_ReadReg(0xee, 0xbc);
	BMP.MD = sdi2c_ReadReg(0xee, 0xbe);

}

void BMP180_GetTemp(){
	BMP.rawtemp = 0;
	sdi2c_WriteReg(0xee, 0xf4, 0x2e);		//0xee = peripheral address, 0xf4 = control register address on bmp180, 0x2e = value for temperature
	BMP.rawtemp = sdi2c_ReadReg(0xee, 0xf6);
}


void BMP180_GetPressure(){

  uint16_t MSB;
  uint16_t LSB;
  uint16_t XLSB;

  sdi2c_WriteReg(0xee, 0xf4, 0x34 + (OSS << 6));

  MSB = sdi2c_ReadReg(0xee, 0xf6);
  LSB = sdi2c_Read8bitReg(0xee, 0xf7);
  XLSB = sdi2c_Read8bitReg(0xee, 0xf8);

  BMP.rawpres = ( (MSB << 16)  + (LSB<<8) + XLSB ) >> ( 8 - OSS );
}

void BMP180_CalcRealTemperature() {		//given in BMP180 Datasheet on page 15

	BMP.temp = 0;
	BMP.X1 = (BMP.rawtemp - BMP.AC6) * BMP.AC5 / (1 << 15);
	BMP.X2 = BMP.MC * (1 << 11) / (BMP.X1 + BMP.MD);
	BMP.B5 = BMP.X1 + BMP.X2;

	T = (BMP.B5 + 8)/(1 << 4);
	T = T * 0.1;		//Temp from above is in .1 degrees Celcius
/*	T = T * 1.8;
	T = T + 32;*/
	BMP.temp = T;
}

void BMP180_CalcRealPressure() {		//straight from datasheet

	BMP.X1 = (BMP.rawtemp - BMP.AC6) * BMP.AC5 / (1 << 15);
	BMP.X2 = BMP.MC * (1 << 11) / (BMP.X1 + BMP.MD);
	BMP.B5 = BMP.X1 + BMP.X2;

	BMP.B6 = BMP.B5 - 4000;
	BMP.X1 = (BMP.B2 * (BMP.B6 * BMP.B6 / (1<<12) )) / (1<<11);
	BMP.X2 = BMP.AC2 * BMP.B6 / (1<<11);
	BMP.X3 = BMP.X1 + BMP.X2;
	BMP.B3 = (((BMP.AC1 * 4 + BMP.X3) << OSS) + 2) / 4;
	BMP.X1 = BMP.AC3 * BMP.B6 / (1<<13);
	BMP.X2 = (BMP.B1 * (BMP.B6 * BMP.B6 / (1<<12))) / (1<<16);
	BMP.X3 = ((BMP.X1 + BMP.X2) + 2) / (1<<2);
	BMP.B4 = BMP.AC4 * (uint32_t)(BMP.X3 + 32768) / (1<<15);
	BMP.B7 = ((uint32_t) BMP.rawpres - BMP.B3) * (50000 >> OSS);



	if(BMP.B7 < 0x80000000){
	P = (BMP.B7 * 2) / BMP.B4;
	}
	else{
	P = (BMP.B7 / BMP.B4) * 2;
	}

	BMP.X1 = (P / (1<<8)) * (P / (1<<8));
	BMP.X1 = (BMP.X1 * 3038) / (1<<16);
	BMP.X2 = (-7357 * P) / (1<<16);

	P = P + (BMP.X1 + BMP.X2 + 3791) / (1<<4);
	//P = P * 0.0000986923266716; //puts in atmospheres
	BMP.pres = P;
}

/*double BMP180_CalcAlt(double pres) {
	alt = pres/po;
	alt = pow(alt, (1/5.255));
	alt = 1-alt;
	alt = 44330 * alt;
	return alt;
}*/


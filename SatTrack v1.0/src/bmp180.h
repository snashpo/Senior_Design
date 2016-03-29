/*
 * bmp180.h
 *
 *  Created on: Dec 11, 2015
 *      Author: Nash
 */

#ifndef BMP180_H_
#define BMP180_H_

void BMP180_GetCalData();
void BMP180_GetTemp();
void BMP180_GetPressure();
void BMP180_CalcRealTemperature();
void BMP180_CalcRealPressure();
double power(int x, unsigned int y);

struct bmp180 {

	int rawtemp;
	int rawpres;
	int temp;
	int pres;

	int16_t AC1;
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
	int32_t B7;
};
struct bmp180 BMP;



#endif /* BMP180_H_ */

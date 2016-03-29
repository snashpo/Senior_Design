/*
 * lsm303.h
 *
 *  Created on: Dec 11, 2015
 *      Author: Nash
 */

#ifndef LSM303_H_
#define LSM303_H_


void initLSM303_Normal();
void initLSM303_LowPower();
void initLSM303_LowerPower();
void LSM303_PowerOff();
void LSM303_GetAccelData();
int32_t LSM303_GetMagData(int32_t addr);
void LSM303_Zero();

struct lsm303 {
	int32_t  lx;
	int32_t  hx;
	int16_t  ly;
	int16_t  hy;
	int16_t  lz;
	int16_t  hz;
	int16_t x;
	int16_t y;
	int16_t z;

	int32_t x_long;
	int32_t y_long;
	int32_t z_long;

	int16_t x_zero;
	int16_t y_zero;
	int16_t z_zero;
};
struct lsm303 Accel;

#endif /* LSM303_H_ */

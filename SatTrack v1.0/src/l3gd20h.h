/*
 * lsm303.h
 *
 *  Created on: Dec 11, 2015
 *      Author: Nash
 */

#ifndef L3GD20H_H_
#define L3GD20H_H_


void initL3GD20H_Normal();
void L3GD20H_Sleep();
void L3GD20H_PowerOff();
void L3GD20H_GetGyroData();
int32_t L3GD20H_GetMagData(int32_t addr);
void L3GD20H_Zero();

struct l3gd20h {
	int32_t  lx;
	int32_t  hx;
	int16_t  ly;
	int16_t  hy;
	int16_t  lz;
	int16_t  hz;

	int x_long;
	int y_long;
	int z_long;

	int16_t x;
	int16_t y;
	int16_t z;
};
struct l3gd20h Gyro;

#endif /* L3GD20H_H_ */

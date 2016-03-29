/*
 * ina219.h
 *
 *  Created on: Dec 11, 2015
 *      Author: Nash
 */

#ifndef INA219_H_
#define INA219_H_


void initINA219_Normal();
void initINA219_LowPower();
void initINA219_LowerPower();
void INA219_PowerOff();
void INA219_GetVoltage();

struct ina219 {

	int bus_voltage;
	int shunt_voltage;
	int current;
	int power;
	int calibration;
};
struct ina219 INA;

#endif /* INA219_H_ */

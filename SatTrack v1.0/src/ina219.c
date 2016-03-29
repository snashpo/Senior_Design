
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
#include "sd_i2c.h"
#include "INA219.h"
//#include "sd_wip_2.h"

#define INA219  0x81  // assuming SA0 grounded  //0x19

#define CONFIG_REG 0x00
#define SHUNT_VOLTAGE_REG 0x01
#define BUS_VOLTAGE_REG 0x02
#define POWER_REG 0x03
#define CURRENT_REG 0x04
#define CALIBRATION_REG 0x05

void initINA219_Normal() {

	sdi2c_WriteReg(INA219, CONFIG_REG, 0x9C1E);	//Enable Accelerometer

}

void initINA219_LowPower() {

	//sdi2c_WriteReg(INA219, CTRL_REG1_A, 0x8F);	//Enable Accelerometer low power @ 1.620kHz

}

void initINA219_LowerPower() {

	//sdi2c_WriteReg(INA219, CTRL_REG1_A, 0x1F);	//Enable Accelerometer low power @ 1Hz

}

void INA219_PowerOff() {

	//sdi2c_WriteReg(INA219, CTRL_REG1_A, 0x00);	//Power Off

}

void INA219_GetVoltage() {

	INA.bus_voltage = sdi2c_ReadReg(INA219, BUS_VOLTAGE_REG);
	INA.shunt_voltage = sdi2c_ReadReg(INA219, SHUNT_VOLTAGE_REG);
	INA.power = sdi2c_ReadReg(INA219, POWER_REG);
	INA.current = sdi2c_ReadReg(INA219, CURRENT_REG);
	INA.calibration = sdi2c_ReadReg(INA219, CALIBRATION_REG);

}





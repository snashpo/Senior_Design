
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
#include "lsm303.h"
//#include "sd_wip_2.h"

#define LSM303_MAG  0x1E  // assuming SA0 grounded
#define LSM303_ACC  0x32  // assuming SA0 grounded  //0x19

#define X 0
#define Y 1
#define Z 2

/* LSM303 Register definitions */
#define D_SA0_HI_ADDRESS 0x1D
#define D_SA0_LO_ADDRESS 0x1E
#define DLHC_DLM_DLH_MAG_ADDRESS          0x1E
#define DLHC_DLM_DLH_ACC_SA0_HIGH_ADDRESS 0x1D
#define DLM_DLH_ACC_SA0_LOW_ADDRESS       0x18


#define CTRL_REG1_A 0x20
#define CTRL_REG2_A 0x21
#define CTRL_REG3_A 0x22
#define CTRL_REG4_A 0x23
#define CTRL_REG5_A 0x24
#define CTRL_REG6_A 0x25
#define REFERENCE_A 0x26
#define STATUS_REG_A 0x27
#define OUT_X_L_A 0x28
#define OUT_X_H_A 0x29
#define OUT_Y_L_A 0x2A
#define OUT_Y_H_A 0x2B
#define OUT_Z_L_A 0x2C
#define OUT_Z_H_A 0x2D
#define FIFO_CTRL_REG_A 0x2E
#define FIFO_SRC_REG_A 0x2F
#define INT1_CFG_A 0x30
#define INT1_SOURCE_A 0x31
#define INT1_THS_A 0x32
#define INT1_DURATION_A 0x33

#define CRA_REG_M 0x00
#define CRB_REG_M 0x01
#define MR_REG_M 0x02
#define OUT_X_H_M 0x03
#define OUT_X_L_M 0x04
#define OUT_Y_H_M 0x05
#define OUT_Y_L_M 0x06
#define OUT_Z_H_M 0x07
#define OUT_Z_L_M 0x08
#define SR_REG_M 0x09
#define IRA_REG_M 0x0A
#define IRB_REG_M 0x0B
#define IRC_REG_M 0x0C

int16_t lo_val = 0;
int16_t hi_val = 0;
int32_t final = 0;

int32_t x1 = 0;
int32_t x2 = 0;
int32_t y1 = 0;
int32_t y2 = 0;
int32_t z1 = 0;
int32_t z2 = 0;





void initLSM303_Normal() {

	sdi2c_WriteReg(LSM303_ACC, CTRL_REG2_A, 0x00);	//Enable Accelerometer
	sdi2c_WriteReg(LSM303_ACC, CTRL_REG1_A, 0x27);	//Enable Accelerometer

}

void initLSM303_LowPower() {

	sdi2c_WriteReg(LSM303_ACC, CTRL_REG2_A, 0x00);	//Enable Accelerometer
	sdi2c_WriteReg(LSM303_ACC, CTRL_REG1_A, 0x8F);	//Enable Accelerometer low power @ 1.620kHz

}

void initLSM303_LowerPower() {

	sdi2c_WriteReg(LSM303_ACC, CTRL_REG2_A, 0x00);	//Enable Accelerometer
	sdi2c_WriteReg(LSM303_ACC, CTRL_REG1_A, 0x1F);	//Enable Accelerometer low power @ 1Hz

}

void LSM303_PowerOff() {

	sdi2c_WriteReg(LSM303_ACC, CTRL_REG2_A, 0x00);	//Enable Accelerometer
	sdi2c_WriteReg(LSM303_ACC, CTRL_REG1_A, 0x00);	//Power Off

}

void LSM303_GetAccelData() {

	Accel.lx = sdi2c_ReadReg(LSM303_ACC, OUT_X_L_A);
	Accel.hx = sdi2c_ReadReg(LSM303_ACC, OUT_X_H_A);
	Accel.ly = sdi2c_ReadReg(LSM303_ACC, OUT_Y_L_A);
	Accel.hy = sdi2c_ReadReg(LSM303_ACC, OUT_Y_H_A);
	Accel.lz = sdi2c_ReadReg(LSM303_ACC, OUT_Z_L_A);
	Accel.hz = sdi2c_ReadReg(LSM303_ACC, OUT_Z_H_A);

	Accel.x_long = (Accel.hx << 16) | Accel.lx;


	Accel.x = (Accel.hx >> 2) - Accel.x_zero;
	Accel.y = (Accel.hy >> 2) - Accel.y_zero;
	Accel.z = (Accel.hz >> 2) - Accel.z_zero;
//	Accel.x = Accel.lx;
//	Accel.y = Accel.ly;
//	Accel.z = Accel.lz;

}

void LSM303_Zero() {

	Accel.x_zero = sdi2c_ReadReg(LSM303_ACC, OUT_X_H_A);
	Accel.y_zero = sdi2c_ReadReg(LSM303_ACC, OUT_Y_H_A);
	Accel.z_zero = sdi2c_ReadReg(LSM303_ACC, OUT_Z_H_A);

	Accel.x_zero = Accel.x_zero >> 4;
	Accel.y_zero = Accel.y_zero >> 4;
	Accel.z_zero = Accel.z_zero >> 4;
}




int32_t LSM303_GetMagData(int32_t addr)
{

  return sdi2c_ReadReg(0x19, addr);			//go to peripheral, and read the address where data stored
}




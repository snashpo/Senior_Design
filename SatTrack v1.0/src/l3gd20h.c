
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
#include "l3gd20h.h"
//#include "sd_wip_2.h"

#define L3GD20H_GYRO  0xD6  // assuming SA0 grounded  //0x19

/* L3GD20H Register definitions */
#define D_SA0_HI_ADDRESS 0x1D
#define D_SA0_LO_ADDRESS 0x1E
#define DLHC_DLM_DLH_MAG_ADDRESS          0x1E
#define DLHC_DLM_DLH_ACC_SA0_HIGH_ADDRESS 0x1D
#define DLM_DLH_ACC_SA0_LOW_ADDRESS       0x18


#define CTRL_REG1 0x20
#define CTRL_REG2 0x21
#define CTRL_REG3 0x22
#define CTRL_REG4 0x23
#define CTRL_REG5 0x24
#define REFERENCE 0x25
#define OUT_TEMP 0x26
#define STATUS_REG 0x27
#define OUT_X_L 0x28
#define OUT_X_H 0x29
#define OUT_Y_L 0x2A
#define OUT_Y_H 0x2B
#define OUT_Z_L 0x2C
#define OUT_Z_H 0x2D
#define FIFO_CTRL_REG 0x2E
#define FIFO_SRC_REG 0x2F
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

void initL3GD20H_Normal()
{

	sdi2c_WriteReg(L3GD20H_GYRO, CTRL_REG1, 0xF);	//Enable Gyro
	sdi2c_WriteReg(L3GD20H_GYRO, CTRL_REG2, 0x00);	//Enable Gyro


}

void L3GD20H_Sleep() {

	sdi2c_WriteReg(L3GD20H_GYRO, CTRL_REG1, 0x8);	//Sleep gyro

}

void L3GD20H_PowerOff() {

	sdi2c_WriteReg(L3GD20H_GYRO, CTRL_REG1, 0x0);	//Power off gyro

}

void L3GD20H_GetGyroData() {

	Gyro.lx = sdi2c_ReadReg(L3GD20H_GYRO, OUT_X_L);
	Gyro.hx = sdi2c_ReadReg(L3GD20H_GYRO, OUT_X_H);
	Gyro.ly = sdi2c_ReadReg(L3GD20H_GYRO, OUT_Y_L);
	Gyro.hy = sdi2c_ReadReg(L3GD20H_GYRO, OUT_Y_H);
	Gyro.lz = sdi2c_ReadReg(L3GD20H_GYRO, OUT_Z_L);
	Gyro.hz = sdi2c_ReadReg(L3GD20H_GYRO, OUT_Z_H);

	Gyro.x_long = (Gyro.hx << 16) + Gyro.lx;

	Gyro.x = Gyro.hx >> 4;
	Gyro.y = Gyro.hy >> 4;
	Gyro.z = Gyro.hz >> 4;



}

void L3GD20H_Zero() {

	Gyro.lx = 0;
	Gyro.hx = 0;
	Gyro.ly = 0;
	Gyro.hy = 0;
	Gyro.lz = 0;
	Gyro.hz = 0;
}




int32_t L3GD20H_GetMagData(int32_t addr)
{

  return sdi2c_ReadReg(0x19, addr);			//go to peripheral, and read the address where data stored
}




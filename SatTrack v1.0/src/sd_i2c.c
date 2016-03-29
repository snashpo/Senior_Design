

#include <stddef.h>		//c standards
#include "sd_i2c.h"		//bmp180 header file
#include "em_cmu.h"		//clock measurement unit
#include "em_gpio.h"	//gpio header



I2C_TransferSeq_TypeDef wseq;
int8_t wdata[3];

I2C_TransferSeq_TypeDef rseq;				//create a new sequence
//I2C_TransferReturn_TypeDef ret;			//define return type enumeration

uint8_t rregid[1];							//register id number
uint8_t rdata[2];							//data of size 2 with length of 8 each


void sdi2c_Init(void)
{

  const I2C_Init_TypeDef init = I2C_INIT_DEFAULT;

  CMU_ClockEnable(cmuClock_HFPER, true);		//enable cmu peripheral clock to high frequency
  CMU_ClockEnable(cmuClock_I2C0, true);			//i2c clock 0 enable

  GPIO_PinModeSet(gpioPortE, 12, gpioModeWiredAnd, 1); 		//sda		pin PE12
  GPIO_PinModeSet(gpioPortE, 13, gpioModeWiredAnd, 1); 		//scl		pin PE13

  /* Enable pins at location 6 */
  I2C0->ROUTE = I2C_ROUTE_SDAPEN |
                I2C_ROUTE_SCLPEN |
                (I2C_ROUTE_LOCATION_LOC6);

  I2C_Init(I2C0, &init);						//initialize i2c by pointing to reigster block and initialization structure (above)
}



I2C_TransferReturn_TypeDef sdi2c_Transfer(I2C_TransferSeq_TypeDef *seq)		//function to check the status of the I2C transfer. 1 = still in progress, 0 means transfer done
{
  uint32_t timeout = 100000;
  I2C_TransferReturn_TypeDef ret;			//create a return status from i2c peripheral

  /* Do a polled transfer */
  ret = I2C_TransferInit(I2C0, seq);
  while (ret == i2cTransferInProgress && timeout--)					//ret = 1 while transfer is in progress, 0 when done
  {
    ret = I2C_Transfer(I2C0);

  }

  return(ret);
}




int sdi2c_WriteReg(uint8_t addr, uint8_t reg, uint8_t value){

  //I2C_TransferSeq_TypeDef seq;				//seq has addr data flags and length. see i2c header for more
  I2C_TransferReturn_TypeDef ret;			//create a return status from i2c peripheral
  //uint8_t data[3];

  wseq.addr = addr;							//input address to peripheral seen on datasheet (0xee for BMP180 sensor)
  wseq.flags = I2C_FLAG_WRITE;

  /*    ----------    Select register to be written    ----------    */
  wdata[0] = reg;							//function input register value (0xF4 is control register address based on BMP180 datasheet)
  wseq.buf[0].data = wdata;					//since we are writing data, write from buf[0]. Data portion of sequence is control register address

  /*    ----------    Only 1 byte reg    ----------    */
  wdata[1] = value;							//value input is control reigster value (0x2e = temperature, 0x34 = pressure 1 oss1)
  wseq.buf[0].len = 2;						//

  /*    ----------    Send sequence    ----------    */
  ret = sdi2c_Transfer(&wseq);				//transfer sequence in above function with newly made sequence
  if (ret != i2cTransferDone)				//wait for ret to be done (return status)
  {
    return((int)ret);						//return the numerical value of the ret if done
  }

  return(ret);								//else return the ret and keep waiting for transfer to complete

}

uint16_t sdi2c_ReadReg(uint8_t addr, uint8_t reg){

//  I2C_TransferSeq_TypeDef seq;				//create a new sequence
  I2C_TransferReturn_TypeDef ret;			//define return type enumeration
//
//  uint8_t regid[1];							//register id number
//  uint8_t data[2];							//data of size 2 with length of 8 each

  rseq.addr = addr;							//addr to BMP180 peripheral (0xee)
  rseq.flags = I2C_FLAG_WRITE_READ;			//read and write values here,

  /*    ----------    Select register to be read    ----------    */
  rregid[0] = reg;							//register value. For MSB of BMP180, 0xF6
  rseq.buf[0].data = rregid;					//data value is read the MSB of temp
  rseq.buf[0].len = 1;						//send address and control value


  /*    ----------    2 bytes reg    ----------    */
  //rdata[0] = 0;								//initialize for debugging
  //rdata[1] = 0;								//initialize for debugging
  rseq.buf[1].data = rdata;					//read into buf[1], set equal to 0
  rseq.buf[1].len = 2;						//buf[1] len is just he data coming out


  ret = sdi2c_Transfer(&rseq);				//send sequence
  if (ret != i2cTransferDone)				//wait until transfer is done
  {
    return(0);								//return zeros until transfer is done
  }

  return((rdata[0] << 8) | rdata[1]);			//if done, OR (add) with data. Add 8 trailing zeros

}

uint8_t sdi2c_Read8bitReg(uint8_t addr, uint8_t reg){		//same as read above but only with length 1

  I2C_TransferSeq_TypeDef seq;
  I2C_TransferReturn_TypeDef ret;
  uint8_t regid[1];
  uint8_t data[2];

  seq.addr = addr;
  seq.flags = I2C_FLAG_WRITE_READ;
  /* Select register to be read */
  regid[0] = reg;
  seq.buf[0].data = regid;
  seq.buf[0].len = 1;


  /* 1 bytes reg */
  data[0] = 0;
  seq.buf[1].data = data;
  seq.buf[1].len = 1;


  ret = sdi2c_Transfer(&seq);
  if (ret != i2cTransferDone)
  {
    return(0);
  }

  return (data[0]);

}

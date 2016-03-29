

#ifndef sdi2c_H_
#define sdi2c_H_

#ifndef __I2CDRV_H
#define __I2CDRV_H

#include "em_i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

void sdi2c_Init(void);		//declare function
I2C_TransferReturn_TypeDef sdi2c_Transfer(I2C_TransferSeq_TypeDef *seq);		//return specific codes based on transfer_return types based in em_i2c header

int sdi2c_WriteReg(uint8_t addr, uint8_t reg, uint8_t value);
uint16_t sdi2c_ReadReg(uint8_t addr, uint8_t reg);
uint8_t sdi2c_Read8bitReg(uint8_t addr, uint8_t reg);

#ifdef __cplusplus
}
#endif

#endif /* __I2CDRV_H */

#endif /* sdi2c_H_ */

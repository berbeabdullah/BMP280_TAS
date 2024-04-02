/*
 * BMP280.h
 *
 *  Created on: Mar 26, 2024
 *      Author: ABDULLAH
 */

#ifndef INC_BMP280_H_
#define INC_BMP280_H_

#include "stm32f4xx_hal.h"

//Oversampling deginations
#define OSRS_OFF		0x00
#define OSRS_1			0x01
#define OSRS_2			0x02
#define OSRS_4			0x03
#define OSRS_8			0x04
#define OSRS_16			0x05
//Mode definations
#define MODE_SLEEP		0x00
#define MODE_FORCED		0x01
#define MODE_NORMAL		0x03
//Standny time
#define T_SB_0p5		0x00
#define T_SB_62p5		0x01
#define T_SB_125		0x02
#define T_SB_250		0x03
#define T_SB_500		0x04
#define T_SB_1k			0x05
#define T_SB_2k			0x06
#define T_SB_4k			0x07
//IRR Filter coefficient
#define IRR_OFF			0x00
#define IRR_2			0x01
#define IRR_4			0x02
#define IRR_8			0x03
#define IRR_16			0x04
//Register defination
#define REG_ID			0xD0
#define REG_RESET		0xE0
#define REG_STATUS		0xF3
#define REG_CTRL_MEAS	0xF4
#define REG_CONFIG		0xF5
#define REG_PRESS		0xF7
#define REG_TEMP		0xFA




extern int BMP280_Config (uint8_t osrs_t, uint8_t osrs_p, uint8_t mode, uint8_t t_sb, uint8_t filter);

extern void TrimRead(void);

extern int BMP_ReadRaw(void);

extern void BMP280_WakeUp(void);

extern void BMP280_Measure (void);


#endif /* INC_BMP280_H_ */

/*
 * 		Driver for the CS4270 Audio codec from Cirrus Logic
 *
 *  	Created on: Nov 24, 2024
 *      Author: chase callender
 */

#ifndef INC_CODEC_H_
#define INC_CODEC_H_

#include "stm32f4xx_hal.h" //required for I2C

#define CODEC_I2C_ADDR					(0x48 << 1)		//device address shifted by 1 position to allow for the read/write bit

#define CODEC_DEVICE_ID					0x0C 			//Device ID and Rev are held in the same register with ID being the MSBs (5 MSBs)
#define CODEC_DEVICE_REV 				0x03			//This is set to 3 as this is the rev used during Dev

#define CODEC_I2C_DEVICE_ID_ADDR 		0x01
#define CODEC_I2C_POWER_CTRL_ADDR 		0x02
#define CODEC_I2C_MODE_CTRL_ADDR 		0x03
#define CODEC_I2C_ADCDAC_CTRL_ADDR 		0x04
#define CODEC_I2C_TRANSITION_CTRL_ADDR 	0x05
#define CODEC_I2C_MUTE_CTRL_ADDR 		0x06
#define CODEC_I2C_DACA_VOL_CTRL_ADDR 	0x07
#define CODEC_I2C_DACB_VOL_CTRL_ADDR 	0x08

HAL_StatusTypeDef codec_read_register(I2C_HandleTypeDef *i2c_handle, uint8_t reg_addr, uint8_t *data);
void codec_configure(I2C_HandleTypeDef *i2c_handle);
void codec_hardware_reset_pin_set(void);
void codec_hardware_reset_pin_clear(void);

#endif /* INC_CODEC_H_ */

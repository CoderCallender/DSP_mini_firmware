/*
 * 		Driver for the CS4270 Audio codec from Cirrus Logic
 *
 *  	Created on: Nov 24, 2024
 *      Author: chase callender
 */


#include "codec.h"
#include "main.h"

//Function to read one register from the codec
//inputs = I2C handle (eg i2c1 or i2c2 etc), register address to be read, pointer to the data to be stored
HAL_StatusTypeDef codec_read_register(I2C_HandleTypeDef *i2c_handle, uint8_t reg_addr, uint8_t *data)
{
	return HAL_I2C_Mem_Read(i2c_handle, CODEC_I2C_ADDR, reg_addr, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
}


HAL_StatusTypeDef codec_write_register(I2C_HandleTypeDef *i2c_handle, uint8_t reg_addr, uint8_t data)
{
	return HAL_I2C_Mem_Write(i2c_handle, CODEC_I2C_ADDR, reg_addr, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);
}

//suppper scabby. Needs work but too excited to see hardware work to write this properly
void codec_configure(I2C_HandleTypeDef *i2c_handle)
{
	HAL_StatusTypeDef err;
	uint8_t read_data;

	err = codec_write_register(i2c_handle, CODEC_I2C_POWER_CTRL_ADDR, 0x00);
	if(err != HAL_OK)
	{
		while(1);
	}




	//if(codec_write_register(i2c_handle, CODEC_I2C_MODE_CTRL_ADDR, 0x00) != HAL_OK)
	//{
	//	while(1);
	//}

	if(codec_write_register(i2c_handle, CODEC_I2C_ADCDAC_CTRL_ADDR, 0x09) != HAL_OK)
	{
		while(1);
	}

	err = codec_read_register(i2c_handle, CODEC_I2C_ADCDAC_CTRL_ADDR, &read_data);

	err = codec_write_register(i2c_handle, CODEC_I2C_MUTE_CTRL_ADDR, 0x10);
	if(err != HAL_OK)
	{
		while(1);
	}

	err = codec_write_register(i2c_handle, CODEC_I2C_DACA_VOL_CTRL_ADDR, 0x00);
	if(err != HAL_OK)
	{
		while(1);
	}
}


//puts the codec into a reset state via hardware pin (nrst)
void codec_hardware_reset_pin_set(void)
{
	HAL_GPIO_WritePin(CODEC_RST_GPIO_Port, CODEC_RST_Pin, 0);
}

//puts the codec into a run state via hardware pin (nrst)
void codec_hardware_reset_pin_clear(void)
{
	HAL_GPIO_WritePin(CODEC_RST_GPIO_Port, CODEC_RST_Pin, 1);
}


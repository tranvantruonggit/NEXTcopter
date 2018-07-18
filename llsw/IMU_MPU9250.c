/*
 * IMU_MPU9250.c
 *
 *  Created on: Apr 1, 2018
 *      Author: tranvantruong
 */

#include "IMU_MPU9250.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_dma.h"
const SPI_InitTypeDef MPU9250_SpiConf =
{
		SPI_Direction_2Lines_FullDuplex,
		SPI_Mode_Master,
		SPI_DataSize_16b,
		SPI_CPOL_Low,
		SPI_CPHA_1Edge,
		SPI_NSS_Hard,
		SPI_BaudRatePrescaler_16,
		SPI_FirstBit_MSB,
		SPI_CRC_Tx

};

const DMA_Stream_TypeDef MPU9250_DmaConf={

};

#if 0
const DMA_InitTypeDef MPU9250_SpiRx_Dma =
{
	DMA_Channel_0,
	TBD,
	TBD,
	DMA_DIR_MemoryToPeripheral,
	TBD, /* DMA Buffer size */
	DMA_PeripheralInc_Disable, /* TBD */
	DMA_MemoryInc_Enable,
	DMA_PeripheralDataSize_Byte,
	DMA_MemoryDataSize_Byte,
	DMA_Mode_Normal,
	DMA_Priority_High,
	DMA_FIFOMode_Disable,/ * TBD */
	DMA_FIFOThreshold_HalfFull,
	DMA_MemoryInc_Disable,
	DMA_PeripheralBurst_Single
};
		
#endif

void IMU_MPU9250_Init( void )
{
}

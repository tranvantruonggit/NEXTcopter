/* Board includes */
#include "stm32f4_discovery.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_rcc.h"
#include "rtos.h"

static void NEXTcopter_IMU_init( void );
static void NEXTcopter_IMU_init( void )
{
/*setup SPI for IMU*/
}

void task_platform_init( void )
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure2;
    GPIO_InitTypeDef GPIO_InitStructure_I2S;
    I2S_InitTypeDef I2S_InitType;
    I2C_InitTypeDef I2C_InitType;

    // Init LED
    /* Init for GPIO_A ( I2S_WS signal), GPIO B (I2C_SDA & I2C_SCL),
    GPIOC (I2S_MCK, I2S_SCK, I2S_SD) and GPIOD (Reset pin on CS43L22) */
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA |
                            RCC_AHB1Periph_GPIOB |
                            RCC_AHB1Periph_GPIOC |
                            RCC_AHB1Periph_GPIOD, ENABLE);

    /* Init clock for SPI and I2C ip */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1 | RCC_APB1Periph_SPI3, ENABLE);

    /* enable clock for I2S ip */
    RCC_PLLI2SCmd(ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // Init PushButton
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    GPIO_InitStructure2.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure2.GPIO_Pin =  GPIO_Pin_0;
    GPIO_InitStructure2.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure2.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure2.GPIO_OType = GPIO_OType_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure2);

    /* Setup GPIO pin for i2s scl and sda pin ) */
    GPIO_InitStructure_I2S.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_9;
    GPIO_InitStructure_I2S.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure_I2S.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure_I2S.GPIO_OType = GPIO_OType_OD;

    /* set alternative function for I2S pin */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_SPI3);

    /* Intit type for I2S module */
    I2S_InitType.I2S_AudioFreq = I2S_AudioFreq_48k;
    I2S_InitType.I2S_MCLKOutput = I2S_MCLKOutput_Enable;
    I2S_InitType.I2S_Mode = I2S_Mode_MasterTx;
    I2S_InitType.I2S_Standard = I2S_Standard_Phillips;
    I2S_InitType.I2S_CPOL = I2S_CPOL_Low;
    I2S_Init( SPI3, &I2S_InitType );
    I2S_Cmd( SPI3, ENABLE );

    I2C_InitType.I2C_ClockSpeed = 100000;
    I2C_InitType.I2C_Mode = I2C_Mode_I2C;
    I2C_InitType.I2C_OwnAddress1 = 99;
    I2C_InitType.I2C_Ack = I2C_Ack_Enable;
    I2C_InitType.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitType.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_Init( I2C1, & I2C_InitType );
    I2C_Cmd( I2C2, ENABLE );

    /* Pulse DAC reset signal to HIGH */
    GPIO_SetBits( GPIOD, GPIO_Pin_4 );
}

void task_platform_1000ms( void )
{
	GPIO_ToggleBits(GPIOD, GPIO_Pin_13); // @suppress("Type cannot be resolved")
}

void task_platform_100ms( void )
{

}

void task_platform_20ms( void )
{

}

void task_platform_10ms( void )
{

}

void task_platform_1ms( void )
{

}

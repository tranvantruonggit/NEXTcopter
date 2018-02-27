/* Board includes */
#include "stm32f4_discovery.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_rcc.h"
/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#define BLOCK_

void ToggleLED_Timer(void*);
void DetectButtonPress(void*);
void ToggleLED_IPC(void*);
void initHW();

void Task_1ms( void* );
void Task_2ms( void* );

typedef struct st_TaskAttrib_t
{
	TaskFunction_t entry;
	const char* const taskName;
	unsigned short stackDepth;
	void *params;
	UBaseType_t uxPriority;
	TaskHandle_t taskHandle;
} TaskAttrib_t;

TaskAttrib_t taskTable[] = 
{

};

xQueueHandle pbq;

int main(void)
{

    initHW();

    /* Create IPC variables */
    pbq = xQueueCreate(10, sizeof(int));
    if (pbq == 0) {
        while(1); /* fatal error */
    }

    /* Create tasks */
    xTaskCreate(
        ToggleLED_Timer,                 /* Function pointer */
        "Task1",                          /* Task name - for debugging only*/
        configMINIMAL_STACK_SIZE,         /* Stack depth in words */
        (void*) NULL,                     /* Pointer to tasks arguments (parameter) */
        tskIDLE_PRIORITY + 2UL,           /* Task priority*/
        NULL                              /* Task handle */
    );

    xTaskCreate(
        DetectButtonPress,
        "Task2",
        configMINIMAL_STACK_SIZE,
        (void*) NULL,
        tskIDLE_PRIORITY + 2UL,
        NULL);

    xTaskCreate(
        ToggleLED_IPC,
        "Task3",
        configMINIMAL_STACK_SIZE,
        (void*) NULL,
        tskIDLE_PRIORITY + 2UL,
        NULL);
    xTaskCreate( Task_1ms,
                 "Task1ms",
                 configMINIMAL_STACK_SIZE,
                 (void*)NULL,
                 tskIDLE_PRIORITY + 2UL,
                 NULL);
    /* Start the RTOS Scheduler */
    vTaskStartScheduler();

    /* HALT */
    while(1);
}

void Task_1ms( void* param  )
{
    volatile float test1 = 100.034f;
    uint32_t i,j;
    volatile uint32_t testVal = 100;
    TickType_t xLastWakeTime;
    static uint8_t period = 1;
    xLastWakeTime = xTaskGetTickCount();
    while(1) {
        GPIO_ToggleBits(GPIOD, GPIO_Pin_12);
        for(i = 0; i< testVal; i++) {
            test1 = test1*test1 + 1.0023;
            for ( j=0 ; j > 1000; j++ );
            GPIO_ToggleBits(GPIOD, GPIO_Pin_15);
        }

        vTaskDelayUntil( &xLastWakeTime, period );
    }
}
/**
 * TASK 1: Toggle LED via RTOS Timer
 */
void ToggleLED_Timer(void *pvParameters)
{

    while (1) {
        GPIO_ToggleBits(GPIOD, GPIO_Pin_13);

        /*
        Delay for a period of time. vTaskDelay() places the task into
        the Blocked state until the period has expired.
        The delay period is spacified in 'ticks'. We can convert
        yhis in milisecond with the constant portTICK_RATE_MS.
        */
        vTaskDelay( 1000 );
    }
}

/**
 * TASK 2: Detect Button Press
 * 			And Signal Event via Inter-Process Communication (IPC)
 */
void DetectButtonPress(void *pvParameters)
{

    int sig = 1;

    while (1) {
        /* Detect Button Press  */
        if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)>0) {
            while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)>0)
                vTaskDelay(100 / portTICK_RATE_MS); /* Button Debounce Delay */
            while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)==0)
                vTaskDelay(100 / portTICK_RATE_MS); /* Button Debounce Delay */

            xQueueSendToBack(pbq, &sig, 0); /* Send Message */
        }
    }
}

/**
 * TASK 3: Toggle LED via Inter-Process Communication (IPC)
 *
 */
void ToggleLED_IPC(void *pvParameters)
{

    int sig;
    portBASE_TYPE status;

    while (1) {
        status = xQueueReceive(pbq, &sig, portMAX_DELAY); /* Receive Message */
        /* portMAX_DELAY blocks task indefinitely if queue is empty */
        if(status == pdTRUE) {
            GPIO_ToggleBits(GPIOD,GPIO_Pin_14);
        }
    }
}

/**
 * Init HW
 */
void initHW()
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

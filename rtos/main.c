/* Board includes */
#include "stm32f4_discovery.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_rcc.h"
/* Kernel includes. */
#include "rtos.h"
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
void Task_10ms( void* );
void Task_100ms( void* );
void Task_1000ms( void* );


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
		{
			Task_1000ms,
			"Task_1000ms",
			configMINIMAL_STACK_SIZE,
			(void*)NULL,
			tskIDLE_PRIORITY+2,
			NULL
		},
		{
			Task_100ms,
			"Task_100ms",
			configMINIMAL_STACK_SIZE,
			(void*)NULL,
			tskIDLE_PRIORITY+2,
			NULL
		},
		{
			Task_10ms,
			"Task_10ms",
			configMINIMAL_STACK_SIZE,
			(void*)NULL,
			tskIDLE_PRIORITY+2,
			NULL
		},
		{
			Task_1ms,
			"Task_1ms",
			configMINIMAL_STACK_SIZE,
			(void*)NULL,
			tskIDLE_PRIORITY+8,
			NULL
		}
};

#define NO_OF_TASKS	(sizeof(taskTable)/sizeof(TaskAttrib_t))
#define RTOS_CREATE_TASK(t)	xTaskCreate((t).entry,\
										(t).taskName,\
										(t).stackDepth,\
										(t).params,\
										(t).uxPriority,\
										(t).taskHandle)

xQueueHandle pbq;

int main(void)
{
	int i;

    /* Create IPC variables */
    pbq = xQueueCreate(10, sizeof(int));
	task_platform_init();
	task_app_init();

    if (pbq == 0) {
        while(1); /* fatal error */
    }

    /* Create tasks */
    for( i =0; i<NO_OF_TASKS; i++ )
    {
    	RTOS_CREATE_TASK(taskTable[i]);

    }
    //xTaskCreate(taskTable[0].entry,taskTable[0].taskName,taskTable[0].stackDepth,taskTable[0].params,taskTable[0].uxPriority,taskTable[0].taskHandle);

    //RTOS_CREATE_TASK(taskTable[0]);
    /* Start the RTOS Scheduler */

    vTaskStartScheduler();

    /* HALT */
    while(1);
}

void Task_10ms( void* param  )
{
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    while(1)
    {
    	task_platform_10ms();

        vTaskDelayUntil( &xLastWakeTime, 10 );
    }
}

void Task_100ms( void* param  )
{
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    while(1)
    {
    	task_platform_100ms();

        vTaskDelayUntil( &xLastWakeTime, 100 );
    }
}

void Task_1000ms( void* param  )
{
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    while(1)
    {
    	task_platform_1000ms();

        vTaskDelayUntil( &xLastWakeTime, 1000 );
    }
}

void Task_1ms( void* param  )
{
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    while(1) {
    	task_platform_1ms();
        vTaskDelayUntil( &xLastWakeTime, 1 );
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

}

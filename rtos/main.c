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

int main(void)
{
	int i;

    /* Create IPC variables */
	task_platform_init();
	task_app_init();



    /* Create tasks */
    for( i =0; i<NO_OF_TASKS; i++ )
    {
    	RTOS_CREATE_TASK(taskTable[i]);

    }

    /* Start the RTOS Scheduler */

    vTaskStartScheduler();

    /* HALT */
    while(1);
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

void Task_10ms( void* param  )
{
	int t_taskCounter = 0;
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    while(1)
    {
    	task_platform_10ms();
    	if( 0 ==(t_taskCounter) )
		{
    		/* run this task every 5 time task 100ms is called */
			task_app_500ms();
		}
		t_taskCounter = (t_taskCounter^0x1U);
        vTaskDelayUntil( &xLastWakeTime, 10 );
    }
}

void Task_100ms( void* param  )
{
	int t_taskCounter = 0;
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    while(1)
    {
    	task_platform_100ms();
    	task_app_100ms();
    	if( 0 ==(t_taskCounter) )
		{
    		/* run this task every 5 time task 100ms is called */
			task_app_500ms();
		}
		t_taskCounter = (t_taskCounter+1)%5;
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



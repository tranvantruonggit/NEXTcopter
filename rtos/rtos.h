#ifndef RTOS_H__
#define RTOS_H__
#include "FreeRTOS.h"

extern void task_app_init( void );
extern void task_app_1000ms( void );
extern void task_app_500ms( void );
extern void task_app_100ms( void );

extern void task_platform_init( void );
extern void task_platform_1000ms( void );
extern void task_platform_100ms( void );
extern void task_platform_20ms( void );
extern void task_platform_10ms( void );
extern void task_platform_1ms( void );
#endif /* RTOS_H__ */

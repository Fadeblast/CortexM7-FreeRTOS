#ifndef FREERTOS_HOOKS_H
#define FREERTOS_HOOKS_H

#include "FreeRTOS.h"
#include "task.h"

// 声明断言函数
void vAssertCalled( const char *pcFile, uint32_t ulLine );

// 声明栈溢出钩子函数
void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName );

#endif /* FREERTOS_HOOKS_H */
#include "FreeRTOS.h"
#include "task.h"  // 必须包含，因为栈溢出钩子函数用到 TaskHandle_t

/************************* 1. 实现 vAssertCalled 断言函数 *************************/
/* 
 * 功能：FreeRTOS 断言失败时调用（如参数错误、系统异常）
 * 参数：
 *   pcFile - 断言失败的文件路径
 *   ulLine - 断言失败的行号
 */
void vAssertCalled( uint32_t ulLine,const char *pcFile)
{
    /* 调试阶段：打印断言信息（根据你的串口/调试工具修改） */
    // 示例1：串口打印（需自行实现串口输出函数）
    // uart_printf("ASSERT FAILED: File %s, Line %lu\r\n", pcFile, ulLine);
    
    // 示例2：Keil MDK 调试输出（无需串口，调试器可查看）
    __asm volatile ( "bkpt #0" );  // 触发硬件断点，暂停调试
    
    /* 生产环境：可添加容错逻辑（如复位系统、记录错误日志） */
    // for( ;; );  // 死循环，防止程序继续运行
}

/************************* 2. 实现 vApplicationStackOverflowHook 栈溢出钩子函数 *************************/
/* 
 * 功能：FreeRTOS 检测到任务栈溢出时调用
 * 参数：
 *   xTask - 栈溢出的任务句柄
 *   pcTaskName - 栈溢出的任务名
 */
void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
    /* 调试阶段：打印栈溢出信息 */
    // 示例1：串口打印
    // uart_printf("STACK OVERFLOW: Task %s (Handle: 0x%08X)\r\n", pcTaskName, xTask);
    
    // 示例2：Keil MDK 调试输出
    __asm volatile ( "bkpt #0" );  // 触发断点，便于定位问题
    
    /* 生产环境：容错处理（如重启任务、记录日志、复位系统） */
    // ( void ) xTask;
    // ( void ) pcTaskName;
    // for( ;; );
}

/************************* 可选：补充其他常用钩子函数（避免后续报错） *************************/
/* 空闲任务钩子函数（若 configUSE_IDLE_HOOK=1 需实现） */
void vApplicationIdleHook( void )
{
    // 空闲任务时执行的逻辑（如低功耗、系统监控）
}

/* 定时器服务任务钩子函数（若 configUSE_TIMERS=1 且 configUSE_TIMER_SERVICE_TASK_HOOK=1 需实现） */
void vApplicationTimerServiceTaskHook( void )
{
    // 定时器服务任务执行时的钩子逻辑
}
/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/


#ifndef PORTMACRO_H
#define PORTMACRO_H

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */
#include "stdint.h"
/* Type definitions. */
#define portCHAR		char
#define portFLOAT		float
#define portDOUBLE		double
#define portLONG		long
#define portSHORT		short
#define portSTACK_TYPE	uint32_t
#define portBASE_TYPE	long

typedef portSTACK_TYPE StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;

#if( configUSE_16_BIT_TICKS == 1 )
	typedef uint16_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffff
#else
	typedef uint32_t TickType_t;
	#define portMAX_DELAY ( TickType_t ) 0xffffffffUL

	/* 32-bit tick type on a 32-bit architecture, so reads of the tick count do
	not need to be guarded with a critical section. */
	#define portTICK_TYPE_IS_ATOMIC 1
#endif
/*-----------------------------------------------------------*/

/* Architecture specifics. */
#define portSTACK_GROWTH			( -1 )
#define portTICK_PERIOD_MS			( ( TickType_t ) 1000 / configTICK_RATE_HZ )
#define portBYTE_ALIGNMENT			8

/* Constants used with memory barrier intrinsics. */
#define portSY_FULL_READ_WRITE		( 15 )

/*-----------------------------------------------------------*/

/* Scheduler utilities. */
#define portYIELD()																\
{																				\
	__asm volatile (															\
		/* 步骤1：加载 PendSV 置位值(0x10000000)到 r0 */						\
		"mov r0, #0x10000000\n\t"												\
		/* 步骤2：加载 NVIC_INT_CTRL 寄存器地址(0xE000ED04)到 r1 */				\
		"ldr r1, =0xE000ED04\n\t"												\
		/* 步骤3：将 r0 的值写入 r1 指向的寄存器（触发 PendSV） */				\
		"str r0, [r1]\n\t"														\
		/* 步骤4：同步屏障确保操作生效 */										\
		"dsb\n\t"																\
		"isb\n\t"																\
		/* 无输出操作数 */														\
		:																		\
		/* 无输入操作数（直接写立即数，避免约束问题） */						\
		:																		\
		/* 破坏列表：告诉编译器修改了 r0/r1 和内存，避免优化 */					\
		: "r0", "r1", "memory", "cc"											\
	);																			\
}
/*-----------------------------------------------------------*/

#define portNVIC_INT_CTRL_REG		( * ( ( volatile uint32_t * ) 0xe000ed04 ) )
#define portNVIC_PENDSVSET_BIT		( 1UL << 28UL )
#define portEND_SWITCHING_ISR( xSwitchRequired ) if( xSwitchRequired != pdFALSE ) portYIELD()
#define portYIELD_FROM_ISR( x ) portEND_SWITCHING_ISR( x )
/*-----------------------------------------------------------*/

/* Critical section management. */
extern void vPortEnterCritical( void );
extern void vPortExitCritical( void );

#define portDISABLE_INTERRUPTS()				vPortRaiseBASEPRI()
#define portENABLE_INTERRUPTS()					vPortSetBASEPRI( 0 )
#define portENTER_CRITICAL()					vPortEnterCritical()
#define portEXIT_CRITICAL()						vPortExitCritical()
#define portSET_INTERRUPT_MASK_FROM_ISR()		ulPortRaiseBASEPRI()
#define portCLEAR_INTERRUPT_MASK_FROM_ISR( x )	vPortSetBASEPRI( x )

/*-----------------------------------------------------------*/

/* Tickless idle/low power functionality. */
#ifndef portSUPPRESS_TICKS_AND_SLEEP
	extern void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime );
	#define portSUPPRESS_TICKS_AND_SLEEP( xExpectedIdleTime ) vPortSuppressTicksAndSleep( xExpectedIdleTime )
#endif
/*-----------------------------------------------------------*/

/* Port specific optimisations. */
#ifndef configUSE_PORT_OPTIMISED_TASK_SELECTION
	#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1
#endif

#if configUSE_PORT_OPTIMISED_TASK_SELECTION == 1

	/* Check the configuration. */
	#if( configMAX_PRIORITIES > 32 )
		#error configUSE_PORT_OPTIMISED_TASK_SELECTION can only be set to 1 when configMAX_PRIORITIES is less than or equal to 32.  It is very rare that a system requires more than 10 to 15 difference priorities as tasks that share a priority will time slice.
	#endif

	/* Store/clear the ready priorities in a bit map. */
	#define portRECORD_READY_PRIORITY( uxPriority, uxReadyPriorities ) ( uxReadyPriorities ) |= ( 1UL << ( uxPriority ) )
	#define portRESET_READY_PRIORITY( uxPriority, uxReadyPriorities ) ( uxReadyPriorities ) &= ~( 1UL << ( uxPriority ) )

	/*-----------------------------------------------------------*/
	/* 第一步：编译器兼容 - 直接定义 __clz 函数（无需依赖 core_cm7.h） */
#ifdef __GNUC__
    // GCC 编译器：直接映射到 GCC 原生内置函数 __builtin_clz
    #define __clz(x) __builtin_clz((uint32_t)x)
#elif defined(__ARMCC_VERSION)
    // ARMCC 编译器：用内联汇编实现 clz 指令（不依赖头文件）
    __STATIC_FORCEINLINE uint32_t __clz(uint32_t ulValue)
    {
        uint32_t ulResult;
        __asm volatile ("clz %0, %1" : "=r"(ulResult) : "r"(ulValue));
        return ulResult;
    }
#else
    // 其他编译器：兜底实现（软件模拟 clz，避免编译错误）
    static uint32_t __clz(uint32_t ulValue)
    {
        uint32_t ulCount = 0UL;
        if(ulValue == 0UL) return 32UL;
        while((ulValue & 0x80000000UL) == 0UL)
        {
            ulCount++;
            ulValue <<= 1UL;
        }
        return ulCount;
    }
#endif
	#define portGET_HIGHEST_PRIORITY( uxTopPriority, uxReadyPriorities ) uxTopPriority = ( 31UL - ( uint32_t ) __clz( ( uxReadyPriorities ) ) )

#endif /* taskRECORD_READY_PRIORITY */
/*-----------------------------------------------------------*/

/* Task function macros as described on the FreeRTOS.org WEB site.  These are
not necessary for to use this port.  They are defined so the common demo files
(which build with all the ports) will build. */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void *pvParameters )
/*-----------------------------------------------------------*/

#ifdef configASSERT
	void vPortValidateInterruptPriority( void );
	#define portASSERT_IF_INTERRUPT_PRIORITY_INVALID() 	vPortValidateInterruptPriority()
#endif

/* portNOP() is not required by this port. */
#define portNOP()

#define portINLINE __inline

#ifndef portFORCE_INLINE
    #if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
        #define portFORCE_INLINE __attribute__((always_inline)) //static inline
    #elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION < 6000000)
        #define portFORCE_INLINE static __forceinline
    #elif defined(__GNUC__)
        #define portFORCE_INLINE static inline __attribute__((always_inline))
    #elif defined(_MSC_VER)
        #define portFORCE_INLINE static __forceinline
    #else
        #define portFORCE_INLINE static inline
    #endif
#endif

/*-----------------------------------------------------------*/

static portFORCE_INLINE void vPortSetBASEPRI( uint32_t ulBASEPRI )
{
    __asm volatile (
        // 原逻辑：将 ulBASEPRI 的值写入 BASEPRI 寄存器
        // %0 绑定输入操作数 ulBASEPRI，编译器自动分配寄存器（如 r0）
        "msr basepri, %0\n\t"
        // 无输出操作数
        :
        // 输入操作数：%0 对应 ulBASEPRI（r 表示加载到通用寄存器）
        : "r"(ulBASEPRI)
        // 破坏列表：告诉编译器修改了系统寄存器/状态，避免优化
        : "cc", "memory"
    );
}
/*-----------------------------------------------------------*/

static portFORCE_INLINE void vPortRaiseBASEPRI( void )
{
    const uint32_t ulNewBASEPRI = configMAX_SYSCALL_INTERRUPT_PRIORITY;

    __asm volatile (
        // 步骤1：将 ulNewBASEPRI 加载到 r0（通用寄存器）
        "mov r0, %0\n\t"
        // 步骤2：关闭全局中断（IRQ），进入临界区
        "cpsid i\n\t"
        // 步骤3：通过 r0 将值写入 BASEPRI 寄存器（符合 ARM 指令规则）
        "msr basepri, r0\n\t"
        // 步骤4：同步屏障确保写操作生效
        "dsb\n\t"
        "isb\n\t"
        // 步骤5：开启全局中断（IRQ）
        "cpsie i\n\t"
        // 无输出操作数
        :
        // 输入操作数：%0 对应 ulNewBASEPRI（r 表示加载到通用寄存器）
        : "r"(ulNewBASEPRI)
        // 破坏列表：告诉编译器修改了 r0、状态寄存器(cc)和内存
        : "r0", "cc", "memory"
    );
}
/*-----------------------------------------------------------*/

static portFORCE_INLINE void vPortClearBASEPRIFromISR( void )
{
    __asm volatile (
        "/* Set BASEPRI to 0 so no interrupts are masked.  This function is only\n"
        "used to lower the mask in an interrupt, so memory barriers are not\n"
        "used. */\n"
        // 步骤1：将立即数0加载到r0（通用寄存器，符合msr指令规则）
        "mov r0, #0\n\t"
        // 步骤2：通过r0将0写入BASEPRI寄存器（解除所有中断屏蔽）
        "msr basepri, r0\n\t"
        // 无输出/输入操作数
        :
        :
        // 破坏列表：告诉编译器r0被修改，避免优化错误
        : "r0", "cc", "memory"
    );
}
/*-----------------------------------------------------------*/

static portFORCE_INLINE uint32_t ulPortRaiseBASEPRI( void )
{
    uint32_t ulReturn;
    const uint32_t ulNewBASEPRI = configMAX_SYSCALL_INTERRUPT_PRIORITY;

    __asm volatile (
        // 1. 读取当前 BASEPRI 到 r0（绑定到 ulReturn）
        "mrs %0, basepri\n\t"
        // 2. 关闭全局中断（临界区保护）
        "cpsid i\n\t"
        // 3. 将新的 BASEPRI 值（r1）写入寄存器
        "msr basepri, %1\n\t"
        // 4. 同步屏障确保操作生效
        "dsb\n\t"
        "isb\n\t"
        // 5. 开启全局中断
        "cpsie i\n\t"
        // 输出操作数：%0 绑定 ulReturn（=r 表示写入寄存器）
        : "=r"(ulReturn)
        // 输入操作数：%1 绑定 ulNewBASEPRI（r 表示读取寄存器）
        : "r"(ulNewBASEPRI)
        // 破坏列表：告诉编译器修改了状态寄存器（cc），避免优化
        : "cc", "memory"
    );

    return ulReturn;
}

/*-----------------------------------------------------------*/

static portFORCE_INLINE BaseType_t xPortIsInsideInterrupt( void )
{
uint32_t ulCurrentInterrupt;
BaseType_t xReturn;

	/* Obtain the number of the currently executing interrupt. */
	__asm volatile (
    "mrs ulCurrentInterrupt, ipsr\n"
);

	if( ulCurrentInterrupt == 0 )
	{
		xReturn = pdFALSE;
	}
	else
	{
		xReturn = pdTRUE;
	}

	return xReturn;
}


#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */


/* 
 * Change Logs
 * Date           Author       Note
 * 2020-7-20      YHL          Create File
 * 2020-7-30      YHL          Add YIELD Function
 * 
 */

#include "Xintype.h"
#include "XinTask.h"

#define INITIAL_XPSR               (0x01000000UL)
#define START_FUN_ADDRESS_MASK     ((stack_type) 0xfffffffeUL)

#define SYSTICK_CTRL_REG			( * ( ( volatile uint32_t * ) 0xe000e010 ) )
#define SYSTICK_LOAD_REG			( * ( ( volatile uint32_t * ) 0xe000e014 ) )
#define SYSTICK_CURRENT_VALUE_REG	( * ( ( volatile uint32_t * ) 0xe000e018 ) )
#define SYSTICK_INT_BIT			    ( 1UL << 1UL )
#define SYSTICK_ENABLE_BIT			( 1UL << 0UL )
#define SYSTICK_CLK_BIT			    ( 1UL << 2UL )


#define NVIC_INT_CTRL_REG		( * ( ( volatile uint32_t * ) 0xe000ed04 ) )
#define NVIC_PENDSVSET_BIT		( 1UL << 28UL )    



#define NVIC_INT_CTRL_REG		( * ( ( volatile uint32_t * ) 0xe000ed04 ) )
#define NVIC_PENDSVSET_BIT		( 1UL << 28UL )    


/**
 *  task_exit_error 错误代码
 */
void task_exit_error(void)
{
    for(;;);
}
/**
 * xin_init_task_stack 任务私有栈初始化
 *            +---------------------------+
 *            |    XPSR   |               |
 *      ||    |    R15(PC)|-- >task code  | 
 *     \||/   |    R14(LR)|              \|/
 *      \/    |    R12    |           auto load
 *            |    R3     |              /|\
 *            |    R2     |               |
 *            |    R1     |               |
 *            |    R0     +---------------+
 *            |    R11    |               |
 *            |    R10    |               |
 *            |    R9     |              \|/
 *            |    R8     |         manual load
 *            |    R7     |              /|\
 *            |    R6     |               |
 *            |    R5     |               |
 * top   ___\ |    R4     +---------------+
 *          / |USER STACK |
 *            +-----------+
 *
 * @p_stack_top 栈顶指针
 * @task_code   任务函数
 * @parameter   任务形参
 */
void xin_init_task_stack(stack_type *p_stack_top , TASK_FUN task_code , void *parameter)
{
    p_stack_top--;
    *p_stack_top = INITIAL_XPSR;
    p_stack_top--;
    *p_stack_top = ((stack_type)task_code)&START_FUN_ADDRESS_MASK;
    p_stack_top--;
    *p_stack_top = (stack_type)task_exit_error;
    p_stack_top -= 5;
    *p_stack_top = (stack_type)parameter;
    p_stack_top -= 8; 
}

/**
 * Xin_start_first_task 开始XinOS第一个任务，使能SVC中断
 */ 
__asm void Xin_start_first_task( void )
{
    PRESERVE8

    /* Use the NVIC offset register to locate the stack. */
    ldr r0, =0xE000ED08
    ldr r0, [r0]
    ldr r0, [r0]

    /* Set the msp back to the start of the stack. */
    msr msp, r0
    /* Globally enable interrupts. */
    cpsie i
    cpsie f
    dsb
    isb
    /* Call SVC to start the first task. */
    svc 0
    nop
    nop
}
/**
 * SVC_Handler 执行Xin_start_first_task进入该服务函数
 * 
*/
__asm void SVC_Handler( void )
{ 
    extern g_current_tcb;
    
    PRESERVE8
	
    ldr r3,=  g_current_tcb              /*获取当前TCB地址*/
	ldr r1, [r3]
	ldr r0, [r1]
	
    ldmia  r0!, {r4-r11}		         /*保存需要手动保存的寄存器*/
    msr psp, r0					         /*设置PSP位当前任务栈顶*/

    isb
    mov r0, #0
    msr	basepri, r0
    orr r14, #0xd
    bx r14
}
void systick_init(void)
{
    /* 1、关闭sys_tick */
	SYSTICK_CTRL_REG = 0UL;
	SYSTICK_CURRENT_VALUE_REG = 0UL;
    
    SYSTICK_LOAD_REG = 25000UL - 1UL;
	SYSTICK_CTRL_REG = ( SYSTICK_CLK_BIT | SYSTICK_INT_BIT | SYSTICK_ENABLE_BIT );
}
void SysTick_Handler(void)
{
   /*使能PendSV中断，当所有中断执行完毕以后进入PendSV_Handler服务*/
    NVIC_INT_CTRL_REG |= NVIC_PENDSVSET_BIT; 
}

void port_yield()
{}


/**
 * PendSV_Handler 空闲中断服务函数
 * 
 * 
*/
__asm void PendSV_Handler( void )
{
	extern g_current_tcb;               /*当前任务控制块*/
	extern xin_task_switch_context;     /*任务切换函数*/

	PRESERVE8

	mrs r0, psp
	isb

	ldr	r3, =g_current_tcb		/* Get the location of the current TCB. */
	ldr	r2, [r3]

	stmdb r0!, {r4-r11}			/* Save the remaining registers. */
	str r0, [r2]				/* Save the new top of stack into the first member of the TCB. */

	stmdb sp!, {r3, r14}
	mov r0, #0
	msr basepri, r0
	dsb
	isb
	bl xin_task_switch_context
	mov r0, #0
	msr basepri, r0
	ldmia sp!, {r3, r14}

	ldr r1, [r3]
	ldr r0, [r1]				/* The first item in pxCurrentTCB is the task top of stack. */
	ldmia r0!, {r4-r11}			/* Pop the registers and the critical nesting count. */
	msr psp, r0
	isb
	bx r14
	nop
}

/* Scheduler utilities. */
#define SY_FULL_READ_WRITE		( 15 )
void portYIELD(void)																
{
	/* Set a PendSV to request a context switch. */
	NVIC_INT_CTRL_REG |= NVIC_PENDSVSET_BIT;
	__dsb( SY_FULL_READ_WRITE );
	__isb( SY_FULL_READ_WRITE );
}

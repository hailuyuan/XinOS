/*
 * Change Logs
 * Date           Author       Notes 
 * 2020-7-20      YHL          Create File
 * 2020-7-29      YHL          Support Task Tick 
 *  
 */
#ifndef XinTask_H
#define XinTask_H

#include "Xinlist.h"
#include "XinConfig.h"
#include "XinType.h"


typedef enum
{
    RUNNING = 0,
    READY,
    BLOCKED,
    SUSPENDED
}TASK_STATUS;


struct Task_TCB
{
    stack_type         *p_stack_top;                         /*任务栈顶地址*/
    TASK_FUN            task_fun;                            /*任务函数*/
    size_type           stack_depth;                         /*任务栈深*/
    struct XIN_LIST     list;                               /*任务链表*/
    TASK_STATUS         task_status;
    priority_type       priority;                            /*任务优先级*/
    uint8_t             task_name[CONFIG_MAX_TASK_NAME];     /*任务名称*/

    uint32_t            task_init_tick;
    uint32_t            task_remain_tick;


};





extern int xin_os_init(void);
extern int xin_task_init( struct Task_TCB *tcb , TASK_FUN task_code ,\
                         void *task_parameter , stack_type *p_stack ,\
                         size_type stack_depth, uint8_t task_priority ,\
                         char *task_name ,uint32_t task_tick );
extern void   xin_os_start(void);
extern struct Task_TCB *get_current_tcb();
extern struct Task_TCB *g_current_tcb;
extern void   xin_task_switch_context(void);
extern int    set_task_ready_map(uint8_t priority);
extern void   reset_task_ready_map(uint8_t priority)






#endif  //XinTask_H

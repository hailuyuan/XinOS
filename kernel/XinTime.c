/*
 * Change Logs
 * Date           Author       Notes
 * 2020-7-29      YHL          Create File
 * 
 */

#include "XinType.h"
#include "XinTask.h"
#include "port_cm3.h"
#include "XinList.h"


uint32_t sys_tick;



/**
 * task_sleep_tick 系统延时N个
 * 
 */
void task_sleep_tick(uint32_t tick)
{
    struct Task_TCB *this_task = get_current_tcb();
    struct XIN_LIST *list      = list_entry(this_task,struct Task_TCB,list);
    
    /*1、关中断*/
    
    /*2、设置任务状态*/
    this_task->task_status = SUSPENDED;
    /*3、如果是一个优先级下的最后一个任务节点，复位该优先级映射*/
    if(list->next = list->next)   
    {
        reset_task_ready_map(this_task->priority);
    }
    /*4、从就绪队列中移除*/
    XIN_LIST_DEL(this_task);
    /*5、加入休眠队列*/
    






    
    /*开中断*/
    portYIELD();

}









void update_sys_tick(void)
{
    ++sys_tick;
}

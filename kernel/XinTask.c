/*
 * Change Logs
 * Date           Author       Notes
 * 2020-7-20      YHL          Create File
 * 2020-7-28      YHL          Use The BitMap Algorthm For Scheduler   
 * 2020-7-30      YHL          Support Task Tisk
 * 
 */

#include "XinType.h"
#include "XinTask.h"
#include "XinTool.h"
#include "XinConfig.h"
#include "XinMalloc.h"
#include "XinLog.h"
#include "port_cm3.h"


#ifdef USE_LOG_MSG
    #include "XinLog.h"
#endif // USE_LOG_MSG



#define ALLIGN_MASK                (0x7)

/*当前活动的任务控制块*/
struct Task_TCB *g_current_tcb;


/*系统空闲任务*/
static void   IDEL_task(void);              

/**
 * 采用BitMap算法实现O(1)调度
 * 目前只支持8个优先级
 * 优先级与ARRAY_SIZE(BitMap)呈指数增长 
 * 
 *   +---------------+---------------------------------+
 *   | task_priority |          task_list              |       
 *   +---------------+---------------------------------+
 *   |     0         | >> tcb0->next  >> tcb0->next... |
 *   |               | << tcb0->prve  << tcb0->prve... |
 *   +---------------+---------------------------------+                                 
 *   |     1         | >> tcb0->next  >> tcb0->next... |
 *   |               | << tcb0->prve  << tcb0->prve... |
 *   +---------------+---------------------------------+
 *   |               | >> tcb0->next  >> tcb0->next... |
 *   |   ...N        | << tcb0->prve  << tcb0->prve... |
 *   +---------------+---------------------------------+ 
 * 
 * 
*/
static uint8_t prio_bit_map[256]=
{
    0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,     /*00 - 0F*/
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,     /*10 - 1F*/
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,     /*20 - 2F*/
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,     /*30 - 3F*/
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,     /*40 - 4F*/
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,     /*50 - 5F*/
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,     /*60 - 6F*/
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,     /*70 - 7F*/
    7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,     /*80 - 8F*/
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,     /*90 - 9F*/
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,     /*A0 - AF*/
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,     /*B0 - BF*/
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,     /*C0 - CF*/
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,     /*D0 - DF*/
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,     /*E0 - EF*/
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0      /*F0 - FF*/
};

static uint8_t task_ready_map;
static struct  XIN_LIST priority_array[CONFIG_MAX_TASK_PRIO];

struct XIN_LIST wait_queue;

/**
 * set_task_ready_map 设置task_ready_map
 * @priority          任务优先级
 */
int set_task_ready_map(uint8_t priority)
{
    if(priority>7)
    {
        LOG("system don't support this priority %d",priority);
        return -1;
    }
    task_ready_map |= (1<<priority);

    return 0;
}

/**
 * reset_task_ready_map 清除task_ready_map
 * @priority          任务优先级
 */ 
void reset_task_ready_map(uint8_t priority)
{
    if(priority>7)
    {
        LOG("system don't support this priority %d",priority);
    }
    task_ready_map &= ~(1<<priority);
}

/**
 * find_higest_priority 找到最高优先级队列
 */
static struct XIN_LIST *find_higest_priority(void)
{
    return &(priority_array[prio_bit_map[task_ready_map]]);
}

/**
 * get_current_tcb 获取当前任务控制块
 * 
 */
struct Task_TCB *get_current_tcb()
{
    return g_current_tcb;
}
/**
 * xin_os_init XinOS 系统初始化函数
 * 
 * @retrun value 0 OK   -1 err 
 */
int xin_os_init(void)
{
    int index;
    /*初数化优先级队列*/
    for(index=0 ; index<CONFIG_MAX_TASK_PRIO ; ++index)
    {
        XIN_INIT_LIST_HEAD(&priority_array[index]);
    }
    return 0;
}

/**
 * xin_os_start 开始任务调度
 * 
 * 
 */
static struct     Task_TCB IDEL_tcb;
static stack_type IDEL_stack[256];
void xin_os_start(void)
{
    XIN_INIT_LIST_HEAD(&wait_queue);
    
    xin_task_init(&IDEL_tcb,IDEL_task,(void *)0,IDEL_stack,ARRAY_SIZE(IDEL_stack),7, "IDLE_TASK" , 5);            /*创建IDLE任务,优先级为7*/
	
	g_current_tcb = &IDEL_tcb;            /*设置当前任务控制块为IDEL控制块*/
	 
    Xin_start_first_task();               /*开始调度器*/   
}

/**
 * xin_task_init 初始化任务
 * @tcb          任务控制块
 * @task_code    任务函数
 * @p_stack      任务栈地址
 * @size         任务栈大小
 */
int xin_task_init( struct Task_TCB *tcb , TASK_FUN task_code ,\
void *task_parameter , stack_type *p_stack , size_type stack_depth, uint8_t task_priority ,char *task_name ,uint32_t task_tick )
{
     /*1、栈地址4字节对齐*/
     uint32_t   stack_uint32      = (uint32_t)p_stack;
     stack_type *p_stack_alligned = p_stack;

     int i;

     if(stack_uint32 & ALLIGN_MASK)
     {
         stack_uint32     = stack_uint32 & (~(size_type)ALLIGN_MASK);
         stack_uint32    += sizeof(size_type);
         p_stack_alligned = (stack_type *)stack_uint32;
     }
     /*2、计算栈顶指针*/
     tcb->p_stack_top = p_stack_alligned + stack_depth - (uint32_t)1;
     /*3、添加任务函数*/
     tcb->task_fun    = task_code;

     /*4、根据任务优先级，把TCB添加到对应的队列上*/
     XIN_LIST_ADD_TAIL(&priority_array[task_priority],&tcb->list);
     set_task_ready_map(task_priority);                            /*设置任务优先级映射表*/
   
    /* 5、设置任务状态*/
     tcb->task_status = READY;
     /*6、复制任务名称到TCB*/
     if(task_name == (void *)0)
     {
         LOG("Please set task name \n");
         return -1;
     }
     for(i=0 ; i<CONFIG_MAX_TASK_NAME ; ++i)
     {
        tcb->task_name[i] = task_name[i];
        if(task_name[i] == '\0')
            break;
     }
     tcb->task_name[CONFIG_MAX_TASK_NAME-1] = '\0';    /*保证最后一位为0*/

    /*初始化任务栈*/
    tcb->p_stack_top = xin_init_task_stack(tcb->p_stack_top,task_code,task_parameter);


    /*任务时间片初始化*/
    tcb->task_init_tick   = task_tick;
    tcb->task_remain_tick = task_tick;
 
    return 0;
}

/**
 * xin_task_switch_context 切换任务上下文
 * 通过修改 g_current_tcb 指向的任务控制块
 * 来实现任务的切换
 */ 
void xin_task_switch_context()
{
    #if 0
     if(g_current_tcb == list_entry(priority_array[0].next,struct Task_TCB,list))
        g_current_tcb = list_entry(priority_array[0].next->next,struct Task_TCB,list);
     else
     {
        g_current_tcb = list_entry(priority_array[0].next,struct Task_TCB,list);
     }
    #endif  //just for test ASM

    struct Task_TCB *to_tcb;
    struct XIN_LIST *head;

    head   = find_higest_priority();
    to_tcb = list_entry(head->next,struct Task_TCB,list);

    if(to_tcb->task_remain_tick > 0+1)
    {
        --to_tcb->task_remain_tick;
    }
    else
    {
        to_tcb->task_remain_tick = to_tcb->task_init_tick;
        XIN_LIST_DEL(&to_tcb->list);
        XIN_LIST_ADD_TAIL(head,&to_tcb->list);    /*添加到队列尾部*/
        head  = find_higest_priority();
        to_tcb = list_entry(head->next,struct Task_TCB,list);
    }
    g_current_tcb = to_tcb;

    #if 0
    if(from_tcb == to_tcb)
    {

    }
        
    else
    {
        /* code */
    }
    
    g_current_tcb = to_tcb;
    #endif
}

/**
 * IDLE_task 系统空闲任务
 */
static  void delay (uint32_t count)
 {
 for (; count!=0; count--);
 }
static void IDEL_task(void)
{
    systick_init();
   
    for(;;)
    {   
        delay(100);
    }
}
















/*
 * Change Logs
 * Date           Author       Notes
 * 2020-7-20      YHL          Create File  
 * 
 * 
 */

#ifndef XINTIME_H
#define XINTIME_H

#include "XinType.h"
#include "XinList.h"

struct Time_TCB
{
    uint32_t init_time;                    /*调用start()以后，赋值为当前sys_tick*/
    uint32_t set_time;                     /*触发时间 = set_time +  init_time*/
    XIN_LIST list;
    void (*time_call_back)(void *private_data);
    void *private_data;
    void (*start)(void);
    void (*init)(uint32_t init_time,uint32_t set_time,void (*time_call_back)(void *private_data));
};









#endif // !XINTIME_H
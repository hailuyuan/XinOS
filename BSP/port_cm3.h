/*
 * Change Logs
 * Date           Author       Notes 
 * 2020-7-20      YHL          Create File
 * 
 * 
 * 
 */
#ifndef PORT_CM3_H
#define PORT_CM3_H

#include "XinType.h"


extern void Xin_start_first_task( void );

extern void systick_init(void);

extern stack_type *xin_init_task_stack(stack_type *p_stack_top , TASK_FUN task_code , void *parameter);

extern void portYIELD(void);

#endif //PORT_CM3_H

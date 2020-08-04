/**
 * just for test
 * 
 */
#include "XinTask.h"
#include "XinTool.h"

struct Task_TCB first_tcb;
stack_type      first_stack[256];


struct Task_TCB second_tcb;
stack_type      second_stack[256];



int ii = 0;
int iii = 0;

static  void delay (uint32_t count)
 {
 for (; count!=0; count--);
 }


void first_task(void)
{
    
    for(;;)
    {
        
        ii=!ii;
        //delay(5);
    }
}
void second_task(void)
{
    
    for(;;)
    {
        
        iii=!iii;
        //delay(5);
    }
}


int main(void)
{   
    xin_os_init();
    xin_task_init(&first_tcb, first_task ,(void *)0,first_stack,ARRAY_SIZE(first_stack) , 0 , "frist_task" , 5);
    
    xin_task_init(&second_tcb, second_task ,(void *)0,second_stack,ARRAY_SIZE(second_stack) , 0 , "second_task" , 5);
	xin_os_start();
    return 0;
}

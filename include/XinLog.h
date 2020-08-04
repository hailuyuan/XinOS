/*
 * Change Logs
 * Date           Author       Notes 
 * 2020-7-20      YHL          Create File
 * 
 * 
 * 
 */
#include "XinConfig.h"

#ifndef XINLOG_H
#define XINLOG_H

#ifdef USR_STANDARD_LIB
    #include <stdio.h>
    #define Xin_print(fmt,args...)  printf(fmt,##args)
#endif // USR_STANDARD_LIB

#ifdef USE_LOG_MSG
     #define LOG(fmt,args...) Xin_print(fmt,##args)
#else
     #define LOG(fmt,args...) 
#endif //USE_LOG_MSG

#endif // XINLOG_H

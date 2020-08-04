/*
 * Change Logs
 * Date           Author       Notes 
 * 2020-7-20      YHL          Create File
 * 
 * 
 * 
 */

#include <XinMalloc.h>
#include <XinConfig.h>
#include <XinType.h>

#ifdef USR_STANDARD_LIB
#include <stdlib.h>
void* xin_malloc(size_type size)
{
    return malloc(size);
}
void xin_free(void *p)
{
    xin_free(p);
}
#endif //USR_STANDARD_LIB

#ifdef USR_XINMALLOC_LIB




#endif // USR_XINMALLOC_LIB

/*
 * Change Logs
 * Date           Author       Notes 
 * 2020-7-20      YHL          Create File
 * 
 * 
 * 
 */
#ifndef XINTOOL_H
#define XINTOOL_H




/*计算数组大小*/
#define ARRAY_SIZE(arry) (sizeof(arry)/sizeof(arry[0]))

/*根据结构体成员计算结构体地址*/
/*form linux kernel*/

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER) 

#define container_of(ptr, type, member) ({			        \
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offsetof(type,member) );})
/*end form linux kernel*/




#endif //XINTOOL_H

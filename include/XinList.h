/*
 * Change Logs
 * Date           Author       Note
 * 2020-1-20      YHL          Create File
 * 2020-7-20      YHL          Support XinOS
 * 
 * 
 */
#ifndef  XINLIST_H
#define  XINLIST_H

#include "XinTool.h"

/**
 * XIN_LIST 双向链表结构
 */
struct XIN_LIST{
    struct XIN_LIST *prev;
    struct XIN_LIST *next;
};

/**
 * XIN_INIT_LIST_HEAD 初始化链表头
 * @list 链表头名称
 */
static inline void sXIN_INIT_LIST_HEAD(struct XIN_LIST *list)
{
	list->next = list;
	list->prev = list;
}

/**
 * XIN_LIST_ADD 链表插入函数
 * @prev 要插入新节点位置的前一个节点
 * @next 要插入新节点位置的后一个节点
 * @new  将要插入的新节点
 */
static inline void XIN_LIST_ADD(struct XIN_LIST *prev ,
                    struct XIN_LIST *next , struct XIN_LIST *new)
{
    prev->next = new;
    next->prev = new;
    new->prev  = prev;
    new->next  = next;
}
/**
 * XIN_LIST_ADD_TAIL 链表节点插入函数
 * @head 链表头节点
 * @new  将要插入的新节点
 *
 */
static inline void XIN_LIST_ADD_TAIL(struct XIN_LIST *head ,
                    struct XIN_LIST *new)
{
    XIN_LIST_ADD(head->prev,head,new);
}

/**
 * XIN_LIST_DEL 链表节点删除函数
 * @node 将要删除的节点
 */
static inline void XIN_LIST_DEL(struct XIN_LIST *node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

/**
 * XIN_LIST_ISEMPTY 链表是否为空
 */
static inline uint8_t  XIN_LIST_ISEMPTY(struct XIN_LIST *node)
{
    return (node == node->prev)&&(node == node->next) ? 1:0;
} 

/*from Linux kernel*/

/**  
 * list_entry 根据成员指针找到结构体指针
 * @ptr    成员指针
 * @head   结构体类型
 * @member 在结构体中链表对应的成员名称
 */
#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)
	
/**  
 *  list_for_each_entry 遍历整个链表
 *  @pos    循环变量
 *  @head   链表头节点
 *  @member 在结构体中链表对应的成员名称
 */
#define list_for_each_entry(pos, head, member)				    \
	for (pos = list_entry((head)->next, typeof(*pos), member);	\
	     &pos->member != (head); 	\
	     pos = list_entry(pos->member.next, typeof(*pos), member))

/*end from Linux kernel*/
#endif  //XINLIST_H

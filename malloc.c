#include "minicrt.h"

/**************************************************************************************
 *                             malloc/free的实现:空闲链表法
 * 1.注意malloc返回的地址是有效空间的地址,在这个地址下面隐藏了头信息!!!
 * 
 * ***********************************************************************************/

typedef struct _heap_header{
    enum{
        HEAP_BLOCK_FREE=0xABABABAB,         // magic number of free block
        HEAP_BLOCK_USED=0xCDCDCDCD,         // magic number of used block
    }type;                                  // block type FREE/USED
    unsigned size;                 // block size including header
    struct _heap_header* next;
    struct _heap_header* prev;          
} heap_header;

#define ADDR_ADD(a,o) (((char*)(a))+o)
#define HEADER_SIZE (sizeof(heap_header))


// 空闲链表(static将变量可见性隐藏在当前编译单元内!)
static heap_header* list_head=NULL;



void free(void* ptr){

    heap_header* header=(heap_header*) ADDR_ADD(ptr,-HEADER_SIZE);
    if(header->type!=HEAP_BLOCK_USED)           // 要释放的是空闲块,直接返回               
        return;                             
    header->type=HEAP_BLOCK_FREE;
    
    // 合并前面的空闲块
    if(header->prev!=NULL && header->prev->type==HEAP_BLOCK_FREE){
        header->prev->next=header->next;
        if(header->next!=NULL)
            header->next->prev=header->prev;
        header->prev->size+=header->size;
        header=header->prev;
    }

    // 合并后面的空闲块
    if(header->next!=NULL && header->next->type==HEAP_BLOCK_FREE){
        header->size+=header->next->size;
        header->next=header->next->next;
    }
}



void* malloc(unsigned size){
    heap_header* header;
    if(size==0)
        return NULL;
    header=list_head;

    // 遍历空闲链表,找到一个大小合适的...
    while(header!=0){
        if(header->type==HEAP_BLOCK_USED){
            header=header->next;
            continue;
        }

        // 如果当前空闲块下能容纳下要新分配的内存(size+HEADER_SIZE)
        // 且不会浪费更多空间(浪费的空间的在一个HEADER_SIZE内 => header->size <= size+HEADER_SIZE*2)
        // 则直接将当前空闲块分配出去
        if(header->size > size+HEADER_SIZE && header->size <= size+HEADER_SIZE*2){
            header->type=HEAP_BLOCK_USED;
        }

        // 如果当前空闲块过大,则需要分裂,然后再分配出去
        if(header->size > size+HEADER_SIZE*2){
            // split
            heap_header* next=(heap_header*)ADDR_ADD(header,size+HEADER_SIZE);
            next->prev=header;
            next->next=header->next;
            next->type=HEAP_BLOCK_FREE;
            next->size=header->size -(size-HEADER_SIZE);
            header->next=next;
            header->size=size+HEADER_SIZE;
            header->type=HEAP_BLOCK_USED;
            return ADDR_ADD(header,HEADER_SIZE);
        }
        header=header->next;
    }

    // 没有找到合适的空闲块
    return NULL;
}


#ifndef WIN32

// linux brk system call
static int brk(void* end_data_segment){
    int ret=0;
    // brk systrm call number:45
    // in /usr/include/asm-i386/unistd.h
    // #define __NR_brk 45
    asm("movl $45, %%eax    \n\t"
        "movl %1, %%ebx     \n\t"
        "int $0x80      \n\t"
        "movl %%eax,%0      \n\t"
        :"=r"(ret):"m"(end_data_segment));
}

#endif

#ifdef  WIN32
#include<Windows.h>
#endif


// 为进程初始化32MB的堆空间(不够时再通过brk系统调用分配)
// 将整个初始堆空间加入空闲链表
int mini_crt_heap_init(){
    void* base=NULL;
    heap_header* header=NULL;
    // 32MB heap size
    unsigned heap_size=1024*1024*32;
#ifdef  WIN32
    base=VirtualAlloc(0,heap_size,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
    if(base==NULL)
        return 0;
#else
    base=(void*)brk(0);             // 获取到当前数据段的基址
    void* end=ADDR_ADD(base,heap_size);
    end=(void*)brk(end);
    if(!end) return 0;
#endif

    header=(heap_header*)base;
    header->size=heap_size;
    header->type=HEAP_BLOCK_FREE;
    header->next=NULL;
    header->prev=NULL;

    list_head=header;
    return 1;
}
/*
==========测试动态内存越界使用和内存泄露================= 

移植说明:
1、拷贝mem_dbg.c到chdrv/rtos目录，并修改makefile文件将mem_dbg.o添加进去 
2、拷贝mem_dbg.h到drv/include 
3、在ch_rtos.h的文件结束位置包含头文件:
#include "mem_dbg.h"
4、在ch_rtos.c内，把内存相关的如下4函数剪切粘贴到文件ch_rtos.c的末尾，并在粘贴的起始位置加上:
#undef CH_RTOS_AllocateMemory
#undef CH_RTOS_AllocateAndClearMemory
#undef CH_RTOS_FreeMemory
#undef CH_RTOS_ReallocMemory
5、重新编译所有引用到rtos内存函数的模块 
 
sqzow20140911
*/

#ifndef __MEM_DBG_H__
#define __MEM_DBG_H__

#ifdef __cplusplus
extern "C" {
#endif
/*所有需要调试内存函数的地方必须包含该头文件*/


#ifdef IN_MEM_DBG
#define MAX_FUNC_NAME		32
/*每次内存多分配SAVE SIZE个字节，防止越界使用，如果不够可以增加*/
#define SAFE_SIZE			32 

#define LOCK_MEM			CH_RTOS_WaitSemaphore(gh_sem_lock, -1)
#define UNLOCK_MEM			CH_RTOS_SignalSemaphore(gh_sem_lock)

/*调试打印*/
#if 1
#define DBG_LOG(...)
#define DBG_LOG2(...)  \
		CH_COM_DebugPrintf("%s#%d:", __FUNCTION__, __LINE__);	\
		CH_COM_DebugPrintf(__VA_ARGS__);
#else
#define DBG_LOG(...)  
#endif

typedef struct mallocb_info_s
{
	char name[MAX_FUNC_NAME];
	int 	line;
	int 	nums;
	struct mallocb_info_s *next, *prev;
}mallocb_info_t;

typedef struct mallocp_s
{
	int true_size;
	mallocb_info_t *info;
	struct mallocp_s *next, *prev;
}mallocp_t;

/*定义真正调用的内存分配函数，可以根据需要进行适配*/
#define real_malloc(p, size) 				CH_RTOS_AllocateMemory(p, size)
#define real_malloc_and_clear(p, size) 		CH_RTOS_AllocateAndClearMemory(p, size) 
#define real_free(p, mem) 					CH_RTOS_FreeMemory(p, mem)
#define real_realloc(h, p, size) 			CH_RTOS_ReallocMemory(h, p, size)

#else

/*定义需要调试的内存分配函数，可以根据需要进行适配*/
#define CH_RTOS_AllocateMemory(h, size) 			dbg_malloc(h, size, __FUNCTION__, __LINE__)
#define CH_RTOS_AllocateAndClearMemory(h, size) 	dbg_malloc0(h, size, __FUNCTION__, __LINE__) 
#define CH_RTOS_FreeMemory(h, mem) 				dbg_free(h, mem, __FUNCTION__, __LINE__)
#define CH_RTOS_ReallocMemory(h, p, size) 			dbg_realloc(h, p, size, __FUNCTION__, __LINE__)


void* dbg_malloc(void* heap, int size , char *fun, int line);

int dbg_free(void* heap,void *ptr, char *fun, int line);

void* dbg_realloc (void * heap, void * ptr, int size, char *fun, int line);

void* dbg_malloc0(void* heap, int size , char *fun, int line);

/*检查当前有多少次内存分配没有被释放
其中同一个调用位置占用的次数大于min_nums时打印出来*/
int dbg_checkmem(int min_nums);

#endif

#ifdef __cplusplus
}
#endif
#endif



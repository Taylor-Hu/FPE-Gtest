/*
==========���Զ�̬�ڴ�Խ��ʹ�ú��ڴ�й¶================= 

��ֲ˵��:
1������mem_dbg.c��chdrv/rtosĿ¼�����޸�makefile�ļ���mem_dbg.o��ӽ�ȥ 
2������mem_dbg.h��drv/include 
3����ch_rtos.h���ļ�����λ�ð���ͷ�ļ�:
#include "mem_dbg.h"
4����ch_rtos.c�ڣ����ڴ���ص�����4��������ճ�����ļ�ch_rtos.c��ĩβ������ճ������ʼλ�ü���:
#undef CH_RTOS_AllocateMemory
#undef CH_RTOS_AllocateAndClearMemory
#undef CH_RTOS_FreeMemory
#undef CH_RTOS_ReallocMemory
5�����±����������õ�rtos�ڴ溯����ģ�� 
 
sqzow20140911
*/

#ifndef __MEM_DBG_H__
#define __MEM_DBG_H__

#ifdef __cplusplus
extern "C" {
#endif
/*������Ҫ�����ڴ溯���ĵط����������ͷ�ļ�*/


#ifdef IN_MEM_DBG
#define MAX_FUNC_NAME		32
/*ÿ���ڴ�����SAVE SIZE���ֽڣ���ֹԽ��ʹ�ã����������������*/
#define SAFE_SIZE			32 

#define LOCK_MEM			CH_RTOS_WaitSemaphore(gh_sem_lock, -1)
#define UNLOCK_MEM			CH_RTOS_SignalSemaphore(gh_sem_lock)

/*���Դ�ӡ*/
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

/*�����������õ��ڴ���亯�������Ը�����Ҫ��������*/
#define real_malloc(p, size) 				CH_RTOS_AllocateMemory(p, size)
#define real_malloc_and_clear(p, size) 		CH_RTOS_AllocateAndClearMemory(p, size) 
#define real_free(p, mem) 					CH_RTOS_FreeMemory(p, mem)
#define real_realloc(h, p, size) 			CH_RTOS_ReallocMemory(h, p, size)

#else

/*������Ҫ���Ե��ڴ���亯�������Ը�����Ҫ��������*/
#define CH_RTOS_AllocateMemory(h, size) 			dbg_malloc(h, size, __FUNCTION__, __LINE__)
#define CH_RTOS_AllocateAndClearMemory(h, size) 	dbg_malloc0(h, size, __FUNCTION__, __LINE__) 
#define CH_RTOS_FreeMemory(h, mem) 				dbg_free(h, mem, __FUNCTION__, __LINE__)
#define CH_RTOS_ReallocMemory(h, p, size) 			dbg_realloc(h, p, size, __FUNCTION__, __LINE__)


void* dbg_malloc(void* heap, int size , char *fun, int line);

int dbg_free(void* heap,void *ptr, char *fun, int line);

void* dbg_realloc (void * heap, void * ptr, int size, char *fun, int line);

void* dbg_malloc0(void* heap, int size , char *fun, int line);

/*��鵱ǰ�ж��ٴ��ڴ����û�б��ͷ�
����ͬһ������λ��ռ�õĴ�������min_numsʱ��ӡ����*/
int dbg_checkmem(int min_nums);

#endif

#ifdef __cplusplus
}
#endif
#endif



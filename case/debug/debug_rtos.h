#ifndef __DEBUG_RTOS_H__
#define __DEBUG_RTOS_H__



extern "C" {

CH_RTOS_RESULT_e  __real_CH_RTOS_CreateTask(
					void        (*rpfn_Function)(void*),
					void        *rpv_Param,
					void        *rpv_StackPtr,
					S32         ri_StackSize,
					S32         ri_Priority,
					const S8    *rpuc_Name,
					CH_RTOS_TaskHandle *rph_Task);

CH_RTOS_RESULT_e  __real_CH_RTOS_DeleteTask(CH_RTOS_TaskHandle rh_Task);

CH_RTOS_RESULT_e  __real_CH_RTOS_KillTask(CH_RTOS_TaskHandle  rh_Task);

CH_RTOS_RESULT_e  __real_CH_RTOS_CreateMessageQuene(
					S32                        ri_MessageSize,
					S32                        ri_MessageNumber,
					CH_RTOS_MessageQHandle    *rph_MessageQ);

CH_RTOS_RESULT_e  __real_CH_RTOS_DeleteMessageQuene(	CH_RTOS_MessageQHandle    rh_MessageQ);

CH_RTOS_RESULT_e  __real_CH_RTOS_CreateSemaphore (S32 ri_Count, CH_RTOS_SemaphoreHandle *rph_Semaphore);

CH_RTOS_RESULT_e  __real_CH_RTOS_DeleteSemaphore (CH_RTOS_SemaphoreHandle rh_Semaphore);

CH_RTOS_RESULT_e  __real_CH_RTOS_CreateEvent(CH_RTOS_EventHandle *rph_Event);

CH_RTOS_RESULT_e  __real_CH_RTOS_DeleteEvent(CH_RTOS_EventHandle rh_Event);

CH_RTOS_RESULT_e  __real_CH_RTOS_CreateMutex (CH_RTOS_MutexHandle *rph_Mutex,U32 ui_Mode);

CH_RTOS_RESULT_e  __real_CH_RTOS_DeleteMutex (CH_RTOS_MutexHandle rh_Mutex);

CH_RTOS_RESULT_e  __real_CH_RTOS_CreateTimer(
					void 						(*rpfn_TimerCallBack)(void *),
					U32							rui_TimerCallBackArg,
					U32							rui_TimeMS,
					CH_RTOS_TIMER_MODE_e		renm_TimerMode,
					CH_RTOS_TimerHandle			*rph_Timer);

CH_RTOS_RESULT_e  __real_CH_RTOS_DeleteTimer(CH_RTOS_TimerHandle rh_Timer);

}

#endif







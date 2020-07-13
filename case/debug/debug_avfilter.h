#ifndef __DEBUG_AVFILTER_H__
#define __DEBUG_AVFILTER_H__

/*
获取已经创建好的AVFILTER handle
*/
CHDRV_AVFILTER_Handle avfilter_GetHandle(int index);

/*
获取已经创建好的AVFILTER handle的个数
*/
int  avfilter_GetHandleNums(void);


extern "C" {
CHDRV_AVFILTER_RESULT_e  __real_CHDRV_AVFILTER_AllocateFilter( S32 ri_Demux,
														S32 ri_Channel,
														CHDRV_AVFILTER_TYPE_e renm_FilterType,
														CHDRV_AVFILTER_Handle *rph_Handle);
CHDRV_AVFILTER_RESULT_e  __real_CHDRV_AVFILTER_DeleteFilter(CHDRV_AVFILTER_Handle  rh_Handle);
CHDRV_AVFILTER_RESULT_e  __real_CHDRV_AVFILTER_SetPid(CHDRV_AVFILTER_Handle rh_Handle, U16 rus_Pid);
CHDRV_AVFILTER_RESULT_e  __real_CHDRV_AVFILTER_EnableFilter(CHDRV_AVFILTER_Handle  rh_Handle);
CHDRV_AVFILTER_RESULT_e  __real_CHDRV_AVFILTER_DisableFilter(CHDRV_AVFILTER_Handle  rh_Handle);

}
#endif





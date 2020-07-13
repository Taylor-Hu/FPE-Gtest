#ifndef __DEBUG_VOUT_H__
#define __DEBUG_VOUT_H__

/*
获取已经创建好的vwin handle
*/
CHDRV_VOUT_PathHandle vout_GetHandle(int index);

/*
获取已经创建好的vwin handle的个数
*/
int vout_GetHandleNums(void);



extern "C" {
CHDRV_VOUT_RESULT_e  __real_CHDRV_VOUT_Open(S32 ri_PathIndex,
                                    CHDRV_VOUT_LayerEnable_t *rpstru_LayerEnable,
                                    CHDRV_VOUT_PathHandle *rph_VideoOutputHandle);

CHDRV_VOUT_RESULT_e  __real_CHDRV_VOUT_Close(CHDRV_VOUT_PathHandle  rh_Handle);
}

#endif





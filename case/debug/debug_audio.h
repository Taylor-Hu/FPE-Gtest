#ifndef __DEBUG_AUDIO_H__
#define __DEBUG_AUDIO_H__

/*
获取已经创建好的route handle
*/
CHDRV_AUDIO_Handle audio_GetHandle(int index);

/*
获取已经创建好的route handle的个数
*/
int audio_GetHandleNums(void);

extern "C" {
CHDRV_AUDIO_RESULT_e  __real_CHDRV_AUDIO_Open(S32 ri_Channel, CHDRV_AUDIO_Handle *rph_Handle);
CHDRV_AUDIO_RESULT_e  __real_CHDRV_AUDIO_Close(CHDRV_AUDIO_Handle  rh_Handle);
CHDRV_AUDIO_RESULT_e __real_CHDRV_AUDIO_StartDecode(CHDRV_AUDIO_Handle  rh_Handle ,
    CHDRV_AUDIO_StartDecodeParam_t *rpstru_Attrib );
CHDRV_AUDIO_RESULT_e __real_CHDRV_AUDIO_StopDecode(CHDRV_AUDIO_Handle  rh_Handle );
CHDRV_AUDIO_RESULT_e  __real_CHDRV_AUDIO_InstallCallback(CHDRV_AUDIO_Handle rh_Handle,
    CHDRV_AUDIO_CALLBACK rpfn_Callback );
 CHDRV_AUDIO_RESULT_e  __real_CHDRV_AUDIO_UnInstallCallback(CHDRV_AUDIO_Handle rh_Handle,
    CHDRV_AUDIO_CALLBACK rpfn_Callback );
}
#endif




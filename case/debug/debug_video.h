#ifndef __DEBUG_VIDEO_H__
#define __DEBUG_VIDEO_H__

/*
获取已经创建好的route handle
*/
CHDRV_VIDEO_Handle video_GetHandle(int index);

/*
获取已经创建好的route handle的个数
*/
int video_GetHandleNums(void);


extern "C" {
CHDRV_VIDEO_RESULT_e  __real_CHDRV_VIDEO_Open(S32 ri_Channel, CHDRV_VIDEO_Handle *rph_Handle);
CHDRV_VIDEO_RESULT_e  __real_CHDRV_VIDEO_Close(CHDRV_VIDEO_Handle  rh_Handle);
CHDRV_VIDEO_RESULT_e __real_CHDRV_VIDEO_StartDecode(
	CHDRV_VIDEO_Handle  rh_Handle ,
	CHDRV_VIDEO_StartDecode_t *rpstru_Attrib );
CHDRV_VIDEO_RESULT_e __real_CHDRV_VIDEO_StopDecode(CHDRV_VIDEO_Handle  rh_Handle );
CHDRV_VIDEO_RESULT_e  __real_CHDRV_VIDEO_InstallCallback(
	CHDRV_VIDEO_Handle rh_Handle,
	CHDRV_VIDEO_DecodeCallback rpfn_Callback );
CHDRV_VIDEO_RESULT_e  __real_CHDRV_VIDEO_UnInstallCallback(
	CHDRV_VIDEO_Handle rh_Handle,
	CHDRV_VIDEO_DecodeCallback rpfn_Callback);
}

#endif



#ifndef __DEBUG_VWIN_H__
#define __DEBUG_VWIN_H__

/*
��ȡ�Ѿ������õ�vwin handle
*/
CHDRV_VWINDOW_Handle vwin_GetHandle(int index);

/*
��ȡ�Ѿ������õ�vwin handle�ĸ���
*/
int vwin_GetHandleNums(void);

const char* layer_strings(CH_LAYER_e event);


extern "C" {
CHDRV_VWINDOW_RESULT_e  __real_CHDRV_VWINDOW_Open(CH_LAYER_e renm_Layer, CHDRV_VIDEO_Handle rh_Handle,
	CHDRV_VWINDOW_Handle *rph_WinHandle);
CHDRV_VWINDOW_RESULT_e  __real_CHDRV_VWINDOW_Close(CHDRV_VWINDOW_Handle  rh_Handle);
CHDRV_VWINDOW_RESULT_e  __real_CHDRV_VWINDOW_Show(CHDRV_VWINDOW_Handle  rh_Handle);
CHDRV_VWINDOW_RESULT_e  __real_CHDRV_VWINDOW_Hide(CHDRV_VWINDOW_Handle  rh_Handle);
CHDRV_VWINDOW_RESULT_e  __real_CHDRV_VWINDOW_SetIOArea(CHDRV_VWINDOW_Handle rh_WinHandle,
												CH_Rect_t *rpstru_InputArea,
												CH_Rect_t *rpstru_OutputArea);
}

#endif




#ifndef __DEBUG_SECTION_H__
#define __DEBUG_SECTION_H__

CHDRV_OSD_RegionHandle osd_GetHandle(CH_LAYER_e	 enm_Layer,
	CHDRV_OSD_REGION_TYPE_e	enm_RegionType);

int osd_GetHandleNums(void);


extern "C" {
CHDRV_SECTION_RESULT_e  __real_CHDRV_SECTION_AllocChannel (S32 ri_Demux,
													U16 rus_Pid,
													CHDRV_SECTION_FilterCallBack rfun_DetectCallback,
													CHDRV_SECTION_ChannelHandle	 *rph_Channel,
													S32 ri_BufferSize);

CHDRV_SECTION_RESULT_e  __real_CHDRV_SECTION_FreeChannel(CHDRV_SECTION_ChannelHandle rh_Channel);

CHDRV_SECTION_RESULT_e  __real_CHDRV_SECTION_StartChannel (CHDRV_SECTION_ChannelHandle	rh_Channel);

CHDRV_SECTION_RESULT_e  __real_CHDRV_SECTION_StopChannel (CHDRV_SECTION_ChannelHandle	rh_Channel);

CHDRV_SECTION_RESULT_e  __real_CHDRV_SECTION_SetChannelPid(CHDRV_SECTION_ChannelHandle rh_Channel,U16 rus_Pid);

CHDRV_SECTION_RESULT_e  __real_CHDRV_SECTION_AllocFilter(S32 ri_Demux,
												  CHDRV_SECTION_AllocParam_t *rpstru_Param,
												  CHDRV_SECTION_FilterHandle *rph_Filter);

CHDRV_SECTION_RESULT_e  __real_CHDRV_SECTION_FreeFilter(CHDRV_SECTION_FilterHandle	rh_Filter);


CHDRV_SECTION_RESULT_e  __real_CHDRV_SECTION_AssociateFilterToChannel (CHDRV_SECTION_FilterHandle rh_Filter,
								CHDRV_SECTION_ChannelHandle rh_Channel);

CHDRV_SECTION_RESULT_e   __real_CHDRV_SECTION_DisAssociateFilterToChannel (CHDRV_SECTION_FilterHandle rh_Filter,
								CHDRV_SECTION_ChannelHandle rh_Channel);

CHDRV_SECTION_RESULT_e   __real_CHDRV_SECTION_SetFilter(CHDRV_SECTION_FilterHandle rh_Filter,
												U8 *rpuc_Data,
												U8 *rpuc_Mask,
												U8 *rpuc_Pattern,
												S32 ri_DataLen,
												CH_BOOL	rb_NotMatchMode);


CHDRV_SECTION_RESULT_e   __real_CHDRV_SECTION_StartFilter(CHDRV_SECTION_FilterHandle rh_Filter);

CHDRV_SECTION_RESULT_e   __real_CHDRV_SECTION_StopFilter(CHDRV_SECTION_FilterHandle rh_Filter);

CHDRV_SECTION_RESULT_e   __real_CHDRV_SECTION_ReadFilterData(CHDRV_SECTION_ChannelHandle rh_Channel,
													 U8 *rpuc_DataBuf,
													 S32 *rpi_Datalen);
}

#endif


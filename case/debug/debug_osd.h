#ifndef __DEBUG_OSD_H__
#define __DEBUG_OSD_H__

CHDRV_OSD_RegionHandle osd_GetHandle(CH_LAYER_e	 enm_Layer,
	CHDRV_OSD_REGION_TYPE_e	enm_RegionType);

int osd_GetHandleNums(void);


extern "C" {
CHDRV_OSD_RESULT_e  __real_CHDRV_OSD_CreateRegion (
					CHDRV_OSD_RegionParam_t	*rpstru_Params,
					CHDRV_OSD_RegionHandle		*rph_Region);

CHDRV_OSD_RESULT_e  __real_CHDRV_OSD_DeleteRegion(CHDRV_OSD_RegionHandle rh_Region);
CHDRV_OSD_RESULT_e __real_CHDRV_OSD_ShowRegion(CHDRV_OSD_RegionHandle rh_Region);
CHDRV_OSD_RESULT_e __real_CHDRV_OSD_HideRegion(CHDRV_OSD_RegionHandle rh_Region);
CHDRV_OSD_RESULT_e __real_CHDRV_OSD_UnlinkRegion (
    CHDRV_OSD_RegionHandle  rh_DrawRegion,
    CHDRV_OSD_RegionHandle  rh_DisplayRegion);
CHDRV_OSD_RESULT_e __real_CHDRV_OSD_LinkRegion (
    CHDRV_OSD_RegionHandle  rh_DrawRegion,
    CHDRV_OSD_RegionHandle  rh_DisplayRegion);
}

#endif

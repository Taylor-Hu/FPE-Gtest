#ifndef __DEBUG_ROUTE_H__
#define __DEBUG_ROUTE_H__

/*
��ȡ�Ѿ������õ�route handle
*/
CHDRV_ROUTE_Handle route_GetHandle(int demux);

/*
��ȡ�Ѿ������õ�route handle�ĸ���
*/
int route_GetHandleNums(void);



extern "C" {
CHDRV_ROUTE_RESULT_e  __real_CHDRV_ROUTE_Create(CHDRV_ROUTE_Param_t *rpstru_Param, CHDRV_ROUTE_Handle *rph_RouteHandle);
CHDRV_ROUTE_RESULT_e __real_CHDRV_ROUTE_Destroy(CHDRV_ROUTE_Handle rh_RouteHandle);


}
#endif


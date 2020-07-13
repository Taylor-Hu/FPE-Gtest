/**
@file test_route.cpp
@def group route测试集

@author 网络公司基础技术部:	邹书强
@date   2014年4月10日  
@version V4.0
@todo     ---时间---作者-------------修改部分及原因----------
@todo     1 2014年4月10日 邹书强  创建


@{
*/

/*测试集名称定义*/
#ifdef CASE_NAME
#undef CASE_NAME
#endif
#define CASE_NAME TestRoute


#include "test_menu.h"
#include "test_modules.h"


void CASE_NAME::SetUpTestCase() {
//excute before each test suit
//std::cout << "init SetUp rtos TestCase " << std::endl;
}
void CASE_NAME::TearDownTestCase() {
//excute after each test suit
//std::cout << "deinit route rtos TestCase " << std::endl;
}

void CASE_NAME::SetUp()
{
//execute before testcase
//std::cout << "Chdrv SetUp" << std::endl;
}
void CASE_NAME::TearDown()
{
//execute before testcase
//std::cout << "Chdrv TearDown" << std::endl;
}

#if ENABLE_TESTCASE

CASE_F(TEST_ROUTE_INIT1,
	"测试多次初始化时是否返回OK") {
	EXPECT_EQ(CHDRV_ROUTE_OK, CHDRV_ROUTE_Init());
	EXPECT_EQ(CHDRV_ROUTE_OK, CHDRV_ROUTE_Init()) << "多次初始化应该返回OK";
}


CASE_F(TEST_ROUTE_INIT2,
	"测试模块是否可被销毁") {
	EXPECT_EQ(CHDRV_ROUTE_OK, CHDRV_ROUTE_Term());
	EXPECT_EQ(CHDRV_ROUTE_OK, CHDRV_ROUTE_Init());
}


CASE_F(TEST_ROUTE_INIT3,
	"测试CHDRV_ROUTE_GetVersion是否成功") {
	CHDRV_Version_t stru_Ver;
	
	memset(&stru_Ver, 0, sizeof(stru_Ver));
	EXPECT_EQ(CHDRV_ROUTE_OK, CHDRV_ROUTE_GetVersion(&stru_Ver));
	EXPECT_NE(0, stru_Ver);
	LOG("\nstru_Ver=%x\n", stru_Ver);
}

CASE_F(TEST_ROUTE_CREATE1,
	"测试错误参数能否创建ROUTE成功") {
	CHDRV_ROUTE_Param_t stru_Param;
	CHDRV_ROUTE_Handle h_RouteHandle;
		
	memset(&stru_Param, 0, sizeof(stru_Param));

	//第一路
	stru_Param.i_Demux = 0;
	stru_Param.enm_RouteType = CHDRV_ROUTE_TYPE_MAX;
	EXPECT_EQ(CHDRV_ROUTE_INVALID_PARAM, CHDRV_ROUTE_Create(&stru_Param, &h_RouteHandle));
	

	//第二路
	stru_Param.i_Demux = 0;
	stru_Param.enm_RouteType = CHDRV_ROUTE_TS_RECORD;
	EXPECT_EQ(CHDRV_ROUTE_INVALID_PARAM, CHDRV_ROUTE_Create(NULL, &h_RouteHandle));
	

	//第三路
	stru_Param.i_Demux = 0;
	stru_Param.enm_RouteType = CHDRV_ROUTE_TS_LIVE_VIEW;
	EXPECT_EQ(CHDRV_ROUTE_INVALID_PARAM, CHDRV_ROUTE_Create(&stru_Param, NULL));
}


CASE_F(TEST_ROUTE_CREATE2,
	"测试正常参数能否创建ROUTE成功,分别创建LIVE/PLAYBACK/RECORD类型的route各一个") {
	CHDRV_ROUTE_Param_t stru_Param;
	CHDRV_ROUTE_Handle h_RouteHandle;
		
	memset(&stru_Param, 0, sizeof(stru_Param));

	stru_Param.enm_VLayer = CH_LAYER_VIDEO1;

	//第一路
	stru_Param.i_Demux = 0;
	stru_Param.enm_RouteType = CHDRV_ROUTE_TS_LIVE_VIEW;
	EXPECT_EQ(CHDRV_ROUTE_OK, CHDRV_ROUTE_Create(&stru_Param, &h_RouteHandle));
	EXPECT_NE((void*)NULL, h_RouteHandle) << "handle 不应为NULL";


	//第二路
	stru_Param.i_Demux = 1;
	stru_Param.enm_RouteType = CHDRV_ROUTE_TS_RECORD;
	EXPECT_EQ(CHDRV_ROUTE_OK, CHDRV_ROUTE_Create(&stru_Param, &h_RouteHandle));
	EXPECT_NE((void*)NULL, h_RouteHandle) << "handle 不应为NULL";


	//第三路
	stru_Param.i_Demux = 2;
	stru_Param.enm_RouteType = CHDRV_ROUTE_TS_PLAYBACK;
	EXPECT_EQ(CHDRV_ROUTE_OK, CHDRV_ROUTE_Create(&stru_Param, &h_RouteHandle));
	EXPECT_NE((void*)NULL, h_RouteHandle) << "handle 不应为NULL";

}


CASE_F(TEST_ROUTE_GETDEVICE_INFO1,
	"测试能否得到正常的设备信息") {
	int i;
	CHDRV_ROUTE_DeviceInfo_t stru_DeviceInfo;

	for(i = 0; i < route_GetHandleNums(); i++)
	{
		EXPECT_EQ(CHDRV_ROUTE_OK, CHDRV_ROUTE_GetDeviceInfo(route_GetHandle(i), &stru_DeviceInfo));
		switch(i)
		{
			case 0:
				EXPECT_EQ(stru_DeviceInfo.enm_RouteType, CHDRV_ROUTE_TS_LIVE_VIEW);
				break;
			
			case 1:
				EXPECT_EQ(stru_DeviceInfo.enm_RouteType, CHDRV_ROUTE_TS_RECORD);
				break;
			
			case 2:
				EXPECT_EQ(stru_DeviceInfo.enm_RouteType, CHDRV_ROUTE_TS_PLAYBACK);
				break;

			default:
				break;
		}
		EXPECT_NE((void*)NULL, stru_DeviceInfo.h_ADecode);
		EXPECT_NE((void*)NULL, stru_DeviceInfo.h_VDecode);
		EXPECT_NE((void*)NULL, stru_DeviceInfo.h_VWindow);
		EXPECT_NE((void*)NULL, stru_DeviceInfo.stru_AvFilters.h_AudioFilter);
		EXPECT_NE((void*)NULL, stru_DeviceInfo.stru_AvFilters.h_VideoFilter);
		EXPECT_NE((void*)NULL, stru_DeviceInfo.stru_AvFilters.h_PCRFilter);
		
	}
	RUN_CMD("route.info");
}


CASE_F(TEST_ROUTE_GET_TUNER,
	"能够得到正确的相应tuenr handle，不为NULL") {
	int i;
	S32 i_Tuner = 0;
	CH_BOOL b_IsOwner = CH_FALSE;

	for(i = 0; i < route_GetHandleNums(); i++)
	{
		EXPECT_EQ(CHDRV_ROUTE_OK, CHDRV_ROUTE_GetTuner(route_GetHandle(i), &i_Tuner, &b_IsOwner));
		
		EXPECT_GE(i_Tuner, 0);
		EXPECT_LT(i_Tuner, fw_getenv_i("TUNER_NUMS"));
	}
}



CASE_F(TEST_ROUTE_GET_DEMUX,
	"能够得到正确的相应demux id") {
	int i;
	S32 i_Demux = 0;

	for(i = 0; i < route_GetHandleNums(); i++)
	{
		EXPECT_EQ(CHDRV_ROUTE_OK, CHDRV_ROUTE_GetDemux(route_GetHandle(i), &i_Demux));
		
		EXPECT_GE(i_Demux, 0);
	}
}

CASE_F(TEST_ROUTE_GET_FILTER,
	"能够得到正确的相应filter handle，不为NULL") {
	int i;
	CHDRV_ROUTE_AVFilters_t stru_AVFilters;

	for(i = 0; i < route_GetHandleNums(); i++)
	{
		EXPECT_EQ(CHDRV_ROUTE_OK, CHDRV_ROUTE_GetFilters(route_GetHandle(i), &stru_AVFilters));
		
		EXPECT_NE(stru_AVFilters.h_AudioFilter, (void*)NULL);
		EXPECT_NE(stru_AVFilters.h_PCRFilter, (void*)NULL);
		EXPECT_NE(stru_AVFilters.h_VideoFilter, (void*)NULL);
	}
}

CASE_F(TEST_ROUTE_GET_VDECODER,
	"能够得到正确的相应vdecoder handle，不为NULL") {
	int i;
	CHDRV_VIDEO_Handle h_VDecode = NULL;

	for(i = 0; i < route_GetHandleNums(); i++)
	{
		EXPECT_EQ(CHDRV_ROUTE_OK, CHDRV_ROUTE_GetVDecode(route_GetHandle(i), &h_VDecode));
		
		EXPECT_NE(h_VDecode, (void*)NULL);
	}
}

CASE_F(TEST_ROUTE_GET_ADECODER,
	"能够得到正确的相应audio handle，不为NULL") {
	int i;
	CHDRV_AUDIO_Handle h_ADecode = NULL;

	for(i = 0; i < route_GetHandleNums(); i++)
	{
		EXPECT_EQ(CHDRV_ROUTE_OK, CHDRV_ROUTE_GetADecode(route_GetHandle(i), &h_ADecode));
		
		EXPECT_NE(h_ADecode, (void*)NULL);
	}
}


CASE_F(TEST_ROUTE_GET_VWIN,
	"能够得到正确的相应vwindow handle，不为NULL") {
	int i;
	CHDRV_VWINDOW_Handle h_vwin = NULL;

	for(i = 0; i < route_GetHandleNums(); i++)
	{
		EXPECT_EQ(CHDRV_ROUTE_OK, CHDRV_ROUTE_GetVWindowHandle(route_GetHandle(i), &h_vwin));
		
		EXPECT_NE(h_vwin, (void*)NULL);
	}
}



CASE_F(TEST_ROUTE_DESTROY,
	"测试Destroy能否成功") {
	int i;

	for(i = 0; i < route_GetHandleNums(); i++)
	{
		EXPECT_EQ(CHDRV_ROUTE_OK, CHDRV_ROUTE_Destroy(route_GetHandle(i)));
	}
}
#endif /* ENABLE_TESTCASE */

static int route_init(int argc, CMD_PARAM argv[])
{
	int ret;

	ret = CHDRV_ROUTE_Init();
	if(ret != CHDRV_ROUTE_OK)
	{
		LOG("ERROR: CHDRV_ROUTE_Init return %d\n", ret);
		return ret;
	}
	
	return 0;
}
static int route_term(int argc, CMD_PARAM argv[])
{
		int ret;

	ret = CHDRV_ROUTE_Term();
	if(ret != CHDRV_ROUTE_OK)
	{
		LOG("ERROR: CHDRV_ROUTE_Term return %d\n", ret);
		return ret;
	}
	
	return 0;
}

static int route_ver(int argc, CMD_PARAM argv[])
{

	int ret;
	CHDRV_Version_t stru_Ver;
	
	ret = CHDRV_ROUTE_GetVersion(&stru_Ver);
	if(ret != CHDRV_ROUTE_OK)
	{
		LOG("ERROR: CHDRV_ROUTE_GetVersion return %d\n", ret);
		return ret;
	}
	LOG("version=0x%x\n",stru_Ver);
	return 0;
}


static int route_create(int argc, CMD_PARAM argv[])
{

	CHDRV_ROUTE_Param_t stru_Param;
	CHDRV_ROUTE_Handle h_RouteHandle;
	int i, ret;
	CHDRV_ROUTE_DeviceInfo_t stru_DeviceInfo;

	for(i = 0; i < route_GetHandleNums(); i++)
	{
		LOG("handle %d=%#x\n", i, route_GetHandle(i));
		EXPECT_EQ(CHDRV_ROUTE_OK, ret = CHDRV_ROUTE_GetDeviceInfo(route_GetHandle(i), &stru_DeviceInfo));
		if(ret == CHDRV_ROUTE_OK
			&& stru_DeviceInfo.enm_RouteType ==  (CHDRV_ROUTE_TYPE_e)argv[2].get_i()
			&& stru_DeviceInfo.i_DemuxId ==  argv[1].get_i())
		{
			LOG("type %d was created yet!\n", argv[2].get_i());
			return 0;
		}
	}
	memset(&stru_Param, 0, sizeof(stru_Param));

	stru_Param.enm_VLayer = CH_LAYER_VIDEO1;

	stru_Param.i_Demux = argv[1].get_i();
	stru_Param.enm_RouteType = (CHDRV_ROUTE_TYPE_e)argv[2].get_i();
	EXPECT_EQ(CHDRV_ROUTE_OK, CHDRV_ROUTE_Create(&stru_Param, &h_RouteHandle));
	EXPECT_NE((void*)NULL, h_RouteHandle) << "handle 不应为NULL";
	if(h_RouteHandle)
	{
		LOG("handle=%#x\n", h_RouteHandle);
	}
	return 0;
}

static int route_delete(int argc, CMD_PARAM argv[])
{

	int i;
	CHDRV_ROUTE_DeviceInfo_t stru_DeviceInfo;

	for(i = 0; i < route_GetHandleNums(); i++)
	{
		EXPECT_EQ(CHDRV_ROUTE_OK, CHDRV_ROUTE_GetDeviceInfo(route_GetHandle(i), &stru_DeviceInfo));
		if(stru_DeviceInfo.enm_RouteType ==  (CHDRV_ROUTE_TYPE_e)argv[2].get_i()
			&& stru_DeviceInfo.i_DemuxId ==  argv[1].get_i())
		{
			break;
		}
	}
	if(i == route_GetHandleNums())
	{
		LOG("route device_id=%d, type=%d was not be created!\n", argv[1].get_i(), argv[2].get_i());
		return -1;
	}
	EXPECT_EQ(CHDRV_ROUTE_OK, CHDRV_ROUTE_Destroy(route_GetHandle(i)));
	return 0;
}


REGISTER_CMD(route_init, "初始化route模块", "",  "");
REGISTER_CMD(route_term, "销毁route模块", "",  "");
REGISTER_CMD(route_ver, "打印route模块版本号", "",  "");
REGISTER_CMD(route_create, "创建一路route", "demux_id | route_type",  "0 0");
REGISTER_CMD(route_delete, "删除一路已经被创建过的route", "demux_id | route_type",  "0 0");




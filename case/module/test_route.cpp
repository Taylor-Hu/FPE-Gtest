/**
@file test_route.cpp
@def group route���Լ�

@author ���繫˾����������:	����ǿ
@date   2014��4��10��  
@version V4.0
@todo     ---ʱ��---����-------------�޸Ĳ��ּ�ԭ��----------
@todo     1 2014��4��10�� ����ǿ  ����


@{
*/

/*���Լ����ƶ���*/
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
	"���Զ�γ�ʼ��ʱ�Ƿ񷵻�OK") {
	EXPECT_EQ(CHDRV_ROUTE_OK, CHDRV_ROUTE_Init());
	EXPECT_EQ(CHDRV_ROUTE_OK, CHDRV_ROUTE_Init()) << "��γ�ʼ��Ӧ�÷���OK";
}


CASE_F(TEST_ROUTE_INIT2,
	"����ģ���Ƿ�ɱ�����") {
	EXPECT_EQ(CHDRV_ROUTE_OK, CHDRV_ROUTE_Term());
	EXPECT_EQ(CHDRV_ROUTE_OK, CHDRV_ROUTE_Init());
}


CASE_F(TEST_ROUTE_INIT3,
	"����CHDRV_ROUTE_GetVersion�Ƿ�ɹ�") {
	CHDRV_Version_t stru_Ver;
	
	memset(&stru_Ver, 0, sizeof(stru_Ver));
	EXPECT_EQ(CHDRV_ROUTE_OK, CHDRV_ROUTE_GetVersion(&stru_Ver));
	EXPECT_NE(0, stru_Ver);
	LOG("\nstru_Ver=%x\n", stru_Ver);
}

CASE_F(TEST_ROUTE_CREATE1,
	"���Դ�������ܷ񴴽�ROUTE�ɹ�") {
	CHDRV_ROUTE_Param_t stru_Param;
	CHDRV_ROUTE_Handle h_RouteHandle;
		
	memset(&stru_Param, 0, sizeof(stru_Param));

	//��һ·
	stru_Param.i_Demux = 0;
	stru_Param.enm_RouteType = CHDRV_ROUTE_TYPE_MAX;
	EXPECT_EQ(CHDRV_ROUTE_INVALID_PARAM, CHDRV_ROUTE_Create(&stru_Param, &h_RouteHandle));
	

	//�ڶ�·
	stru_Param.i_Demux = 0;
	stru_Param.enm_RouteType = CHDRV_ROUTE_TS_RECORD;
	EXPECT_EQ(CHDRV_ROUTE_INVALID_PARAM, CHDRV_ROUTE_Create(NULL, &h_RouteHandle));
	

	//����·
	stru_Param.i_Demux = 0;
	stru_Param.enm_RouteType = CHDRV_ROUTE_TS_LIVE_VIEW;
	EXPECT_EQ(CHDRV_ROUTE_INVALID_PARAM, CHDRV_ROUTE_Create(&stru_Param, NULL));
}


CASE_F(TEST_ROUTE_CREATE2,
	"�������������ܷ񴴽�ROUTE�ɹ�,�ֱ𴴽�LIVE/PLAYBACK/RECORD���͵�route��һ��") {
	CHDRV_ROUTE_Param_t stru_Param;
	CHDRV_ROUTE_Handle h_RouteHandle;
		
	memset(&stru_Param, 0, sizeof(stru_Param));

	stru_Param.enm_VLayer = CH_LAYER_VIDEO1;

	//��һ·
	stru_Param.i_Demux = 0;
	stru_Param.enm_RouteType = CHDRV_ROUTE_TS_LIVE_VIEW;
	EXPECT_EQ(CHDRV_ROUTE_OK, CHDRV_ROUTE_Create(&stru_Param, &h_RouteHandle));
	EXPECT_NE((void*)NULL, h_RouteHandle) << "handle ��ӦΪNULL";


	//�ڶ�·
	stru_Param.i_Demux = 1;
	stru_Param.enm_RouteType = CHDRV_ROUTE_TS_RECORD;
	EXPECT_EQ(CHDRV_ROUTE_OK, CHDRV_ROUTE_Create(&stru_Param, &h_RouteHandle));
	EXPECT_NE((void*)NULL, h_RouteHandle) << "handle ��ӦΪNULL";


	//����·
	stru_Param.i_Demux = 2;
	stru_Param.enm_RouteType = CHDRV_ROUTE_TS_PLAYBACK;
	EXPECT_EQ(CHDRV_ROUTE_OK, CHDRV_ROUTE_Create(&stru_Param, &h_RouteHandle));
	EXPECT_NE((void*)NULL, h_RouteHandle) << "handle ��ӦΪNULL";

}


CASE_F(TEST_ROUTE_GETDEVICE_INFO1,
	"�����ܷ�õ��������豸��Ϣ") {
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
	"�ܹ��õ���ȷ����Ӧtuenr handle����ΪNULL") {
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
	"�ܹ��õ���ȷ����Ӧdemux id") {
	int i;
	S32 i_Demux = 0;

	for(i = 0; i < route_GetHandleNums(); i++)
	{
		EXPECT_EQ(CHDRV_ROUTE_OK, CHDRV_ROUTE_GetDemux(route_GetHandle(i), &i_Demux));
		
		EXPECT_GE(i_Demux, 0);
	}
}

CASE_F(TEST_ROUTE_GET_FILTER,
	"�ܹ��õ���ȷ����Ӧfilter handle����ΪNULL") {
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
	"�ܹ��õ���ȷ����Ӧvdecoder handle����ΪNULL") {
	int i;
	CHDRV_VIDEO_Handle h_VDecode = NULL;

	for(i = 0; i < route_GetHandleNums(); i++)
	{
		EXPECT_EQ(CHDRV_ROUTE_OK, CHDRV_ROUTE_GetVDecode(route_GetHandle(i), &h_VDecode));
		
		EXPECT_NE(h_VDecode, (void*)NULL);
	}
}

CASE_F(TEST_ROUTE_GET_ADECODER,
	"�ܹ��õ���ȷ����Ӧaudio handle����ΪNULL") {
	int i;
	CHDRV_AUDIO_Handle h_ADecode = NULL;

	for(i = 0; i < route_GetHandleNums(); i++)
	{
		EXPECT_EQ(CHDRV_ROUTE_OK, CHDRV_ROUTE_GetADecode(route_GetHandle(i), &h_ADecode));
		
		EXPECT_NE(h_ADecode, (void*)NULL);
	}
}


CASE_F(TEST_ROUTE_GET_VWIN,
	"�ܹ��õ���ȷ����Ӧvwindow handle����ΪNULL") {
	int i;
	CHDRV_VWINDOW_Handle h_vwin = NULL;

	for(i = 0; i < route_GetHandleNums(); i++)
	{
		EXPECT_EQ(CHDRV_ROUTE_OK, CHDRV_ROUTE_GetVWindowHandle(route_GetHandle(i), &h_vwin));
		
		EXPECT_NE(h_vwin, (void*)NULL);
	}
}



CASE_F(TEST_ROUTE_DESTROY,
	"����Destroy�ܷ�ɹ�") {
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
	EXPECT_NE((void*)NULL, h_RouteHandle) << "handle ��ӦΪNULL";
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


REGISTER_CMD(route_init, "��ʼ��routeģ��", "",  "");
REGISTER_CMD(route_term, "����routeģ��", "",  "");
REGISTER_CMD(route_ver, "��ӡrouteģ��汾��", "",  "");
REGISTER_CMD(route_create, "����һ·route", "demux_id | route_type",  "0 0");
REGISTER_CMD(route_delete, "ɾ��һ·�Ѿ�����������route", "demux_id | route_type",  "0 0");




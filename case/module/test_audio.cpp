/**
@file test_audio.cpp
@def group audio测试集

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
#define CASE_NAME TestAudio


#include "test_menu.h"
#include "test_modules.h"
//#include "psi_parse.h"


void CASE_NAME::SetUpTestCase() {
//excute before each test suit
//std::cout << "init SetUp rtos TestCase " << std::endl;
RUN_CMD("tuner.lock", fw_getenv("TUNER_PARAMS1"));

}
void CASE_NAME::TearDownTestCase() {
//excute after each test suit
//std::cout << "deinit audio rtos TestCase " << std::endl;
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

static CHDRV_AUDIO_Handle gh_AudioHandle;

extern int aud_info(int argc, CMD_PARAM argv[]);

void audio_callback(CHDRV_AUDIO_Handle rh_Handle ,
					CHDRV_AUDIO_CALLBACK_EVENT_e renm_Event,
					U32 *rui_Arg)
{
	
}	
#if ENABLE_TESTCASE

CASE_F(TEST_AUDIO_INIT1,
	"测试多次初始化时是否返回OK") {
	EXPECT_EQ(CHDRV_AUDIO_OK, CHDRV_AUDIO_Init());
	EXPECT_EQ(CHDRV_AUDIO_OK, CHDRV_AUDIO_Init()) << "多次初始化应该返回OK";
}

CASE_F(TEST_AUDIO_INIT2,
	"测试CHDRV_AUDIO_Term是否成功"){

	EXPECT_EQ(CHDRV_AUDIO_Term(),CHDRV_AUDIO_OK);
}

/*无CHDRV_AUDIO_GetVersion函数
CASE_F(TEST_AUDIO_INIT3,
	"测试CHDRV_AUDIO_GetVersion是否成功"){

	
}
*/

CASE_F(TEST_AUDIO_INIT4,
	"测试没有初始化时，其他函数是否能调用成功"){

	EXPECT_EQ(CHDRV_AUDIO_Open(0,&gh_AudioHandle),CHDRV_AUDIO_NOT_INIT);
}

CASE_F(TEST_AUDIO_INIT5,
	"测试调用CHDRV_AUDIO_Term后，再调用其他函数是否能成功"){

	CHDRV_AUDIO_Init();
	CHDRV_AUDIO_Term();
	EXPECT_EQ(CHDRV_AUDIO_Open(0,&gh_AudioHandle),CHDRV_AUDIO_NOT_INIT);
}

CASE_F(TEST_AUDIO_PARAMS1,
	"open无效参数测试"){

	CHDRV_AUDIO_Init();
	EXPECT_EQ(CHDRV_AUDIO_Open(0,NULL),CHDRV_AUDIO_INVALID_PARAM);
}

CASE_F(TEST_AUDIO_PARAMS2,
	"close无效参数测试"){

	EXPECT_EQ(CHDRV_AUDIO_Close(NULL),CHDRV_AUDIO_INVALID_PARAM);
}

CASE_F(TEST_AUDIO_PARAMS3,
	"start无效参数测试"){

	if(audio_GetHandleNums() > 0)
	{
		gh_AudioHandle = audio_GetHandle(0);
	}
	else
	{
		CHDRV_AUDIO_Open(0,&gh_AudioHandle);
	}
	
	EXPECT_EQ(CHDRV_AUDIO_StartDecode(gh_AudioHandle,NULL),CHDRV_AUDIO_INVALID_PARAM);
}

CASE_F(TEST_AUDIO_PARAMS4,
	"stop无效参数测试"){

	CHDRV_AUDIO_StartDecodeParam_t stru_vidDecode;

	stru_vidDecode.enm_syncmode = CHDRV_AUDIO_SYNC;
	stru_vidDecode.enm_StreamType = CH_AUDIO_CODE_MPEG1;
	stru_vidDecode.enm_StreamPacketType = CHDRV_AUDIO_PACKET_PES;
	CHDRV_AUDIO_StartDecode(gh_AudioHandle,&stru_vidDecode);

	EXPECT_EQ(CHDRV_AUDIO_StopDecode(NULL),CHDRV_AUDIO_INVALID_PARAM);

	EXPECT_EQ(CHDRV_AUDIO_StopDecode(gh_AudioHandle),CHDRV_AUDIO_OK);
}
/***没有接口
CASE_F(TEST_AUDIO_PARAMS5,
	"pause无效参数测试"){
	
	EXPECT_EQ(CHDRV_AUDIO_RamPauseDecode(NULL),CHDRV_AUDIO_OK);
}
***/

/****没有接口
CASE_F(TEST_AUDIO_PARAMS6,
	"resume无效参数测试"){

	CHDRV_AUDIO_RamPauseDecode(gh_AudioHandle);
	
	EXPECT_EQ(CHDRV_AUDIO_RamResumeDecode(NULL),CHDRV_AUDIO_OK);
}
*****/

CASE_F(TEST_AUDIO_PARAMS7,
	"ram start无效参数测试"){

	EXPECT_EQ(CHDRV_AUDIO_RamStartDecode(gh_AudioHandle,NULL),CHDRV_AUDIO_INVALID_PARAM);
}

CASE_F(TEST_AUDIO_PARAMS8,
	"ram input无效参数测试"){

	CHDRV_AUDIO_StartDecodeParam_t stru_vidDecode;

	memset(&stru_vidDecode, 0, sizeof(stru_vidDecode));
	stru_vidDecode.enm_syncmode = CHDRV_AUDIO_SYNC;
	stru_vidDecode.enm_StreamType = CH_AUDIO_CODE_MPEG1;
	stru_vidDecode.enm_StreamPacketType = CHDRV_AUDIO_PACKET_PES;
	
	
	CHDRV_AUDIO_RamStartDecode(gh_AudioHandle,&stru_vidDecode);
	if( fw_getenv_i("DEAD_HALT_TEST"))
	{
		EXPECT_EQ(CHDRV_AUDIO_RamInputData(gh_AudioHandle,NULL,0),CHDRV_AUDIO_INVALID_PARAM);
	}
	else
	{
		GTEST_FAIL() << "应通过死亡测试";
	}
}


CASE_F(TEST_AUDIO_PARAMS9,
	"ram stop无效参数测试"){

	int buffer_len = 0;
	char *buff = NULL;
	
	buffer_len = fsys_read_file(fw_getenv("BEEP_FILE_1"),buff);
	
	if( fw_getenv_i("DEAD_HALT_TEST"))
	{
		CHDRV_AUDIO_RamInputData(gh_AudioHandle,buff,buffer_len);
	}
	else
	{
		GTEST_FAIL() << "应通过死亡测试";
	}
	
	

	CH_RTOS_FreeMemory(gh_APP_HEAP, buff);
	EXPECT_EQ(CHDRV_AUDIO_RamStopDecode(NULL),CHDRV_AUDIO_INVALID_PARAM);
}

CASE_F(TEST_AUDIO_PARAMS10,
	"ram pause无效参数测试"){

	EXPECT_EQ(CHDRV_AUDIO_RamPauseDecode(NULL),CHDRV_AUDIO_INVALID_PARAM);
}

CASE_F(TEST_AUDIO_PARAMS11,
	"ram resume无效参数测试"){

	CHDRV_AUDIO_RamPauseDecode(gh_AudioHandle);

	EXPECT_EQ(CHDRV_AUDIO_RamResumeDecode(NULL),CHDRV_AUDIO_INVALID_PARAM);
}



CASE_F(TEST_AUDIO_PARAMS12,
	"install callback无效参数测试"){

	EXPECT_EQ(CHDRV_AUDIO_InstallCallback(gh_AudioHandle,NULL),CHDRV_AUDIO_OK);
}

CASE_F(TEST_AUDIO_PARAMS13,
	"uninstall callback无效参数测试"){

	CHDRV_AUDIO_InstallCallback(gh_AudioHandle,audio_callback);

	EXPECT_EQ(CHDRV_AUDIO_UnInstallCallback(gh_AudioHandle,NULL),CHDRV_AUDIO_OK);
}

CASE_F(TEST_AUDIO_PARAMS14,
	"get status无效参数测试"){

	if( fw_getenv_i("DEAD_HALT_TEST"))
	{
		EXPECT_EQ(CHDRV_AUDIO_GetStatus(gh_AudioHandle,NULL),CHDRV_AUDIO_INVALID_PARAM);
	}
	else
	{
		GTEST_FAIL() << "应通过死亡测试";
	}
	
	
	
	CHDRV_AUDIO_RamStopDecode(gh_AudioHandle);

	//CHDRV_AUDIO_Close(gh_AudioHandle);

}

/***没有实现
CASE_F(TEST_AUDIO_PARAMS15,
	"get streaminfo 无效参数测试"){

	
}
*****/

/*
CASE_F(TEST_AUDIO_PARAMS16,
	"ram set decodeex param无效参数测试"){

	
}
*/
/**头文件里定义.c文件里没有实现
CASE_F(TEST_AUDIO_PARAMS17,
	"ram start ex无效参数测试"){

	EXPECT_EQ(CHDRV_AUDIO_RamStartDecodeEx(gh_AudioHandle,NULL),CHDRV_AUDIO_OK);
}

CASE_F(TEST_AUDIO_PARAMS18,
	"ram input ex无效参数测试"){

	CHDRV_AUDIO_StartDecodeParam_t stru_vidDecode;

	stru_vidDecode.enm_syncmode = CHDRV_AUDIO_SYNC;
	stru_vidDecode.enm_StreamType = CH_AUDIO_CODE_MPEG1;
	stru_vidDecode.enm_StreamPacketType = CHDRV_AUDIO_PACKET_PES;
	
	CHDRV_AUDIO_RamStartDecodeEx(gh_AudioHandle,&stru_vidDecode);

	EXPECT_EQ(CHDRV_AUDIO_RamInputDataEx(gh_AudioHandle,NULL,0),CHDRV_AUDIO_OK);
}

CASE_F(TEST_AUDIO_PARAMS19,
	"ram stop ex无效参数测试"){
	int buffer_len = 0;
	char *buff = NULL;
	
	read_Data("BEEF_FILE",buff);

	CHDRV_AUDIO_RamInputData(gh_AudioHandle,buff,buffer_len);

	EXPECT_EQ(CHDRV_AUDIO_RamStopDecodeEx(NULL),CHDRV_AUDIO_OK);

}

CASE_F(TEST_AUDIO_PARAMS20,
	"ram pause ex无效参数测试"){

	EXPECT_EQ(CHDRV_AUDIO_RamPauseDecodeEx(NULL),CHDRV_AUDIO_OK);
}

CASE_F(TEST_AUDIO_PARAMS21,
	"ram resume ex无效参数测试"){

	CHDRV_AUDIO_RamPauseDecodeEx(gh_AudioHandle);

	EXPECT_EQ(CHDRV_AUDIO_RamResumeDecodeEx(NULL),CHDRV_AUDIO_OK);
}

CASE_F(TEST_AUDIO_PARAMS22,
	"ram get buffer status无效参数测试"){

	EXPECT_EQ(CHDRV_AUDIO_RamDecodeGetBuffStatus(gh_AudioHandle,NULL),CHDRV_AUDIO_OK);
}

CASE_F(TEST_AUDIO_PARAMS23,
	"ram clear data无效参数测试"){

	EXPECT_EQ(CHDRV_AUDIO_RamDecodeClearData(NULL),CHDRV_AUDIO_OK);

	CHDRV_AUDIO_RamStopDecodeEx(gh_AudioHandle);

	CHDRV_AUDIO_Close(gh_AudioHandle);

}
*/
CASE_F(TEST_AUDIO_FUNCTION1,
	"以正确参数open 1路audio，是否返回成功，且句柄不为NULL"){

	EXPECT_EQ(CHDRV_AUDIO_Open(0,&gh_AudioHandle),CHDRV_AUDIO_OK);
	EXPECT_NE(gh_AudioHandle,(void *)NULL);

}

CASE_F(TEST_AUDIO_FUNCTION2,
	"调用get status，获得的状态应为idle"){

	CHDRV_AUDIO_STATUS_e enm_audStatus;

	CHDRV_AUDIO_GetStatus(gh_AudioHandle,&enm_audStatus);

	EXPECT_EQ(enm_audStatus,CHDRV_AUDIO_STATUS_IDLE);

	//CHDRV_AUDIO_Close(gh_AudioHandle);

}
#endif /* ENABLE_TESTCASE */

/*
CASE_F(TEST_AUDIO_INIT2,
	"测试模块是否可被销毁") {
	EXPECT_EQ(CHDRV_AUDIO_OK, CHDRV_AUDIO_Term());
	EXPECT_EQ(CHDRV_AUDIO_OK, CHDRV_AUDIO_Init());
}


CASE_F(TEST_AUDIO_INIT3,
	"测试CHDRV_AUDIO_GetVersion是否成功") {
	CHDRV_Version_t stru_Ver;
	
	memset(&stru_Ver, 0, sizeof(stru_Ver));
	//EXPECT_EQ(CHDRV_AUDIO_OK, CHDRV_AUDIO_GetVersion(&stru_Ver));
	//EXPECT_NE(0, stru_Ver);
	LOG("stru_Ver=%x\n", stru_Ver);
}
*/

void aud_CALLBACK(CHDRV_AUDIO_Handle rh_Handle,
					CHDRV_AUDIO_CALLBACK_EVENT_e renm_Event,
					U32 *rui_Arg)
{}

static int aud_start(int argc, CMD_PARAM argv[])
{
	int ret;
	CHDRV_AUDIO_Handle handle = NULL;
	CHDRV_AUDIO_StartDecodeParam_t stru_DecodePara;

	if(audio_GetHandleNums() <= argv[1].get_i())
	{
		ret = CHDRV_AUDIO_Open(argv[1], &handle);
		if(ret != CHDRV_AUDIO_OK)
		{
			//尝试从route中获取，有些平台已从route中创建
			if(route_GetHandleNums() > 0)
			{
				 CHDRV_ROUTE_GetADecode(route_GetHandle(0), &handle);
			}
			else
			{
				LOG("ERROR: CHDRV_AUDIO_Open return %d\n", ret);
				return ret;
			}
		}
		if(handle == NULL)
		{
			LOG("ERROR: CHDRV_AUDIO_Open handle=NULL\n");
			return -1;
		}
		ret = CHDRV_AUDIO_InstallCallback(handle, aud_CALLBACK);
		if(ret != CHDRV_AUDIO_OK)
		{
			LOG("ERROR: CHDRV_AUDIO_InstallCallback return %d\n", ret);
		}
#ifdef CHSSA_CHIP_7583
		chdrv_platform_connect_demux_adecoder(0,0,handle);
#endif
	}
	else
	{
		handle = audio_GetHandle(argv[1].get_i());
	}
	memset(&stru_DecodePara, 0, sizeof(stru_DecodePara));
	
	
#if (defined(CFG_CHSSA_CHIP_MON_CC6000) || defined(CFG_CHSSA_CHIP_MON_CC5000))
	stru_DecodePara.enm_StreamPacketType = CHDRV_AUDIO_PACKET_MPEG1_PACKET;
#else
	stru_DecodePara.enm_StreamPacketType = CHDRV_AUDIO_PACKET_TS;
#endif /* CFG_CHSSA_CHIP_MON_CC6000 */
	stru_DecodePara.enm_syncmode = CHDRV_AUDIO_SYNC;
	if(argv[2].get_i() < 0 || argv[2].get_i() >= CH_AUDIO_CODE_TYPE_MAX)
	{
		LOG("error: audio code type only support 0 ~ %d:\n"
			"CH_AUDIO_CODE_MPEG1	0\n"
			"CH_AUDIO_CODE_MPEG2	1\n"
			"CH_AUDIO_CODE_MP3		2\n"
			"CH_AUDIO_CODE_AC3		3\n"
			"CH_AUDIO_CODE_AAC_ADTS	4\n"
			"CH_AUDIO_CODE_AAC_LOAS	5\n"
			"CH_AUDIO_CODE_HEAAC_ADTS	6\n"
			"CH_AUDIO_CODE_HEAAC_LOAS	7\n"
			"CH_AUDIO_CODE_WMA			8\n"
			"CH_AUDIO_CODE_AC3_PLUS	9\n" 
			"CH_AUDIO_CODE_LPCM	10\n"
			"CH_AUDIO_CODE_DTS		11\n"
			"CH_AUDIO_CODE_ATRAC	12\n"
			"CH_AUDIO_CODE_COOK	13\n"
			"CH_AUDIO_CODE_DRA		14\n"
			,
			CH_AUDIO_CODE_TYPE_MAX - 1);
		return -1;
	}
	stru_DecodePara.enm_StreamType = (CH_AUDIO_CODE_TYPE_e)argv[2].get_i();
	ret = CHDRV_AUDIO_StartDecode(handle, &stru_DecodePara);
	if(ret != CHDRV_AUDIO_OK)
	{
		LOG("ERROR: CHDRV_AUDIO_StartDecode return %d\n", ret);
		return ret;
	}
	
	LOG("ok, handle no = %d\n", audio_GetHandleNums());
	
	return 0;
}

static int aud_stop(int argc, CMD_PARAM argv[])
{
	int ret;
	CHDRV_VOUT_PathHandle handle = NULL;

	if(argv[1].get_i() >= audio_GetHandleNums())
	{
		LOG("error: please open audio first! handle no=%d\n", argv[1].get_i() );
		return -1;
	}
	ret = CHDRV_AUDIO_StopDecode(audio_GetHandle(argv[1].get_i()));
	if(ret != CHDRV_AUDIO_OK)
	{
		LOG("error: CHDRV_AUDIO_StopDecode return %d!\n", ret);
		return -1;
	}
	if(!(route_GetHandleNums() > 0 
		&& CHDRV_ROUTE_GetADecode(route_GetHandle(0), &handle) == CHDRV_ROUTE_OK
		&& handle == audio_GetHandle(argv[1].get_i())))
	{
		ret = CHDRV_AUDIO_UnInstallCallback(audio_GetHandle(argv[1].get_i()), aud_CALLBACK);
		if(ret != CHDRV_AUDIO_OK)
		{
			LOG("ERROR: CHDRV_AUDIO_UnInstallCallback return %d\n", ret);
		}
		ret = CHDRV_AUDIO_Close(audio_GetHandle(argv[1].get_i()));
		if(ret != CHDRV_AUDIO_OK)
		{
			LOG("error: CHDRV_AUDIO_Close return %d!\n", ret);
			return -1;
		}
	}
	return 0;
}
static int aud_init(int argc, CMD_PARAM argv[])
{
	int ret;

	ret = CHDRV_AUDIO_Init();
	LOG("CHDRV_AUDIO_Init return %d\n", ret);
	return ret;	
}

int aud_term(int argc, CMD_PARAM argv[])
{
	int ret;

	ret = CHDRV_AUDIO_Term();
	LOG("CHDRV_AUDIO_Term return %d\n", ret);
	return ret;	
}

/**没有实现
int aud_pause(int argc, CMD_PARAM argv[])
{
	int ret;
	if(argv[1].get_i() >= audio_GetHandleNums())
	{
		LOG("error: please open audio first! handle no=%d\n", argv[1].get_i() );
		return -1;
	}
	
	ret = CHDRV_AUDIO_RamPauseDecode(audio_GetHandle(argv[1].get_i()));
	if(ret != CHDRV_AUDIO_OK)
	{
		LOG("error: CHDRV_AUDIO_RamPauseDecode return %d!\n", ret);
		return -1;
	}
	return 0;
}
***/
/**没实现
int aud_resume(int argc, CMD_PARAM argv[])
{
	int ret;
	if(argv[1].get_i() >= audio_GetHandleNums())
	{
		LOG("error: please open audio first! handle no=%d\n", argv[1].get_i() );
		return -1;
	}

	ret = CHDRV_AUDIO_RamResumeDecode(audio_GetHandle(argv[1].get_i()));
	if(ret != CHDRV_AUDIO_OK)
	{
		LOG("error: CHDRV_AUDIO_RamResumeDecode return %d!\n", ret);
		return -1;
	}
	return 0;
}
*****/
int aud_ram_start(int argc, CMD_PARAM argv[])
{
	int ret;
	CHDRV_AUDIO_Handle handle = NULL;
	CHDRV_AUDIO_StartDecodeParam_t stru_DecodePara;

	if(audio_GetHandleNums() == 0)
	{
		ret = CHDRV_AUDIO_Open(argv[1].get_i(), &handle);
		if(ret != CHDRV_AUDIO_OK)
		{
			LOG("ERROR: CHDRV_AUDIO_Open return %d\n", ret);
			return ret;
		}
	}
	else
	{
		handle = audio_GetHandle(0);
	}

	ret = CHDRV_AUDIO_InstallCallback(handle, aud_CALLBACK);
	if(ret != CHDRV_AUDIO_OK)
	{
		LOG("ERROR: CHDRV_AUDIO_InstallCallback return %d\n", ret);
	}

	memset(&stru_DecodePara, 0, sizeof(stru_DecodePara));
	stru_DecodePara.enm_StreamPacketType = (CHDRV_AUDIO_STREAM_PACKET_e)argv[4].get_i();
	stru_DecodePara.enm_syncmode = CHDRV_AUDIO_SYNC;
	if(argv[3].get_i() < 0 || argv[3].get_i() >= CH_AUDIO_CODE_TYPE_MAX)
	{
		LOG("error: audio code type only support 0 ~ %d:\n"
			"CH_AUDIO_CODE_MPEG1	0\n"
			"CH_AUDIO_CODE_MPEG2	1\n"
			"CH_AUDIO_CODE_MP3		2\n"
			"CH_AUDIO_CODE_AC3		3\n"
			"CH_AUDIO_CODE_AAC_ADTS	4\n"
			"CH_AUDIO_CODE_AAC_LOAS	5\n"
			"CH_AUDIO_CODE_HEAAC_ADTS	6\n"
			"CH_AUDIO_CODE_HEAAC_LOAS	7\n"
			"CH_AUDIO_CODE_WMA			8\n"
			"CH_AUDIO_CODE_AC3_PLUS	9\n" 
			"CH_AUDIO_CODE_LPCM	10\n"
			"CH_AUDIO_CODE_DTS		11\n"
			"CH_AUDIO_CODE_ATRAC	12\n"
			"CH_AUDIO_CODE_COOK	13\n"
			"CH_AUDIO_CODE_DRA		14\n"
			,
			CH_AUDIO_CODE_TYPE_MAX - 1);
		return -1;
	}
	stru_DecodePara.enm_StreamType = (CH_AUDIO_CODE_TYPE_e)argv[3].get_i();
	ret = CHDRV_AUDIO_RamStartDecode(handle, &stru_DecodePara);
	if(ret != CHDRV_AUDIO_OK)
	{
		LOG("ERROR: CHDRV_AUDIO_RamStartDecode return %d\n", ret);
		return ret;
	}

	int buffer_len = 0;
	char *buff = NULL;
	
	buffer_len = fsys_read_file(argv[2].get_str(),buff);

	CHDRV_AUDIO_RamInputData(handle,buff,buffer_len);
	
	CH_RTOS_FreeMemory(gh_APP_HEAP, buff);
	return 0;
}

int aud_ram_pause(int argc, CMD_PARAM argv[])
{
	int ret;
	if(argv[1].get_i() >= audio_GetHandleNums())
	{
		LOG("error: please open audio first! handle no=%d\n", argv[1].get_i() );
		return -1;
	}
	
	ret = CHDRV_AUDIO_RamPauseDecode(audio_GetHandle(argv[1].get_i()));
	if(ret != CHDRV_AUDIO_OK)
	{
		LOG("error: CHDRV_AUDIO_RamPauseDecode return %d!\n", ret);
		return -1;
	}
	return 0;
}

int aud_ram_resume(int argc, CMD_PARAM argv[])
{
	int ret;
	if(argv[1].get_i() >= audio_GetHandleNums())
	{
		LOG("error: please open audio first! handle no=%d\n", argv[1].get_i() );
		return -1;
	}

	ret = CHDRV_AUDIO_RamResumeDecode(audio_GetHandle(argv[1].get_i()));
	if(ret != CHDRV_AUDIO_OK)
	{
		LOG("error: CHDRV_AUDIO_RamResumeDecode return %d!\n", ret);
		return -1;
	}
	return 0;
}

int aud_ram_stop(int argc, CMD_PARAM argv[])
{
	int ret;

	if(argv[1].get_i() >= audio_GetHandleNums())
	{
		LOG("error: please open audio first! handle no=%d\n", argv[1].get_i() );
		return -1;
	}
	ret = CHDRV_AUDIO_RamStopDecode(audio_GetHandle(argv[1].get_i()));
	if(ret != CHDRV_AUDIO_OK)
	{
		LOG("error: CHDRV_AUDIO_RamStopDecode return %d!\n", ret);
		return -1;
	}
	ret = CHDRV_AUDIO_UnInstallCallback(audio_GetHandle(argv[1].get_i()), aud_CALLBACK);
	if(ret != CHDRV_AUDIO_OK)
	{
		LOG("ERROR: CHDRV_AUDIO_UnInstallCallback return %d\n", ret);
	}
	ret = CHDRV_AUDIO_Close(audio_GetHandle(argv[1].get_i()));
	if(ret != CHDRV_AUDIO_OK)
	{
		LOG("error: CHDRV_AUDIO_Close return %d!\n", ret);
		return -1;
	}
	return 0;

}

/**
int aud_info(int argc, CMD_PARAM argv[])
{

}
**/

int aud_status(int argc, CMD_PARAM argv[])
{
	int ret,i;
	CHDRV_AUDIO_STATUS_e enm_AudioStatus;
	
	for(i=0;i<audio_GetHandleNums();i++)
	{
		ret = CHDRV_AUDIO_GetStatus(audio_GetHandle(i),&enm_AudioStatus);
		if(ret != CHDRV_AUDIO_OK)
		{
			LOG("ERROR: CHDRV_AUDIO_GetStatus return %d\n", ret);
		}
	}
	LOG("aud_status ok\n");
	return 0;

}
static int aud_restart(int argc, CMD_PARAM argv[])
{
	int ret;
	CHDRV_VOUT_PathHandle handle = audio_GetHandle(argv[1].get_i());

	if(argv[1].get_i() >= audio_GetHandleNums())
	{
		LOG("error: please open audio first! handle no=%d\n", argv[1].get_i() );
		return -1;
	}
	ret = CHDRV_AUDIO_StopDecode(audio_GetHandle(argv[1].get_i()));
	if(ret != CHDRV_AUDIO_OK)
	{
		LOG("error: CHDRV_AUDIO_StopDecode return %d!\n", ret);
		return -1;
	}

	CHDRV_AUDIO_StartDecodeParam_t stru_DecodePara;

	memset(&stru_DecodePara, 0, sizeof(stru_DecodePara));
#if (defined(CFG_CHSSA_CHIP_MON_CC6000) || defined(CFG_CHSSA_CHIP_MON_CC5000))
	stru_DecodePara.enm_StreamPacketType = CHDRV_AUDIO_PACKET_MPEG1_PACKET;
#else
	stru_DecodePara.enm_StreamPacketType = CHDRV_AUDIO_PACKET_TS;
#endif /* CFG_CHSSA_CHIP_MON_CC6000 */	
	stru_DecodePara.enm_syncmode = CHDRV_AUDIO_SYNC;
	if(argv[2].get_i() < 0 || argv[2].get_i() >= CH_AUDIO_CODE_TYPE_MAX)
	{
		LOG("error: audio code type only support 0 ~ %d:\n"
			"CH_AUDIO_CODE_MPEG1	0\n"
			"CH_AUDIO_CODE_MPEG2	1\n"
			"CH_AUDIO_CODE_MP3		2\n"
			"CH_AUDIO_CODE_AC3		3\n"
			"CH_AUDIO_CODE_AAC_ADTS	4\n"
			"CH_AUDIO_CODE_AAC_LOAS	5\n"
			"CH_AUDIO_CODE_HEAAC_ADTS	6\n"
			"CH_AUDIO_CODE_HEAAC_LOAS	7\n"
			"CH_AUDIO_CODE_WMA			8\n"
			"CH_AUDIO_CODE_AC3_PLUS	9\n" 
			"CH_AUDIO_CODE_LPCM	10\n"
			"CH_AUDIO_CODE_DTS		11\n"
			"CH_AUDIO_CODE_ATRAC	12\n"
			"CH_AUDIO_CODE_COOK	13\n"
			"CH_AUDIO_CODE_DRA		14\n"
			,
			CH_AUDIO_CODE_TYPE_MAX - 1);
		return -1;
	}
	stru_DecodePara.enm_StreamType = (CH_AUDIO_CODE_TYPE_e)argv[2].get_i();
	ret = CHDRV_AUDIO_StartDecode(handle, &stru_DecodePara);
	if(ret != CHDRV_AUDIO_OK)
	{
		LOG("ERROR: CHDRV_AUDIO_StartDecode return %d\n", ret);
		return ret;
	}
	
	return 0;
}


REGISTER_CMD(aud_init, "初始化audio模块", "",  "");
REGISTER_CMD(aud_term, "销毁audio模块", "",  "");
REGISTER_CMD(aud_ram_start, "开始注入播放解码", "device_id| filename |stream_type |packege_type",  "0 c:/1.mp3 2 0");
REGISTER_CMD(aud_ram_pause, "暂停注入播放", "device_id",  "0");
REGISTER_CMD(aud_ram_resume, "恢复注入播放", "device_id",  "0");
REGISTER_CMD(aud_ram_stop, "停止注入播放", "device_id",  "0");
REGISTER_CMD(aud_status, "打印播放状态", "device_id",  "0");
REGISTER_CMD(aud_start, "开始live播放解码", "device_id|stream_type",  "0 0");
REGISTER_CMD(aud_stop, "停止解码", "device_id",  "0");
REGISTER_CMD(aud_restart, "重新启停audio解码", "device_id | stream_type",  "0 0");
REGISTER_CMD(aud_info, "获得audio的相关信息，打印到串口", "",  "");







/**
@file test_audio.cpp
@def group audio���Լ�

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
	"���Զ�γ�ʼ��ʱ�Ƿ񷵻�OK") {
	EXPECT_EQ(CHDRV_AUDIO_OK, CHDRV_AUDIO_Init());
	EXPECT_EQ(CHDRV_AUDIO_OK, CHDRV_AUDIO_Init()) << "��γ�ʼ��Ӧ�÷���OK";
}

CASE_F(TEST_AUDIO_INIT2,
	"����CHDRV_AUDIO_Term�Ƿ�ɹ�"){

	EXPECT_EQ(CHDRV_AUDIO_Term(),CHDRV_AUDIO_OK);
}

/*��CHDRV_AUDIO_GetVersion����
CASE_F(TEST_AUDIO_INIT3,
	"����CHDRV_AUDIO_GetVersion�Ƿ�ɹ�"){

	
}
*/

CASE_F(TEST_AUDIO_INIT4,
	"����û�г�ʼ��ʱ�����������Ƿ��ܵ��óɹ�"){

	EXPECT_EQ(CHDRV_AUDIO_Open(0,&gh_AudioHandle),CHDRV_AUDIO_NOT_INIT);
}

CASE_F(TEST_AUDIO_INIT5,
	"���Ե���CHDRV_AUDIO_Term���ٵ������������Ƿ��ܳɹ�"){

	CHDRV_AUDIO_Init();
	CHDRV_AUDIO_Term();
	EXPECT_EQ(CHDRV_AUDIO_Open(0,&gh_AudioHandle),CHDRV_AUDIO_NOT_INIT);
}

CASE_F(TEST_AUDIO_PARAMS1,
	"open��Ч��������"){

	CHDRV_AUDIO_Init();
	EXPECT_EQ(CHDRV_AUDIO_Open(0,NULL),CHDRV_AUDIO_INVALID_PARAM);
}

CASE_F(TEST_AUDIO_PARAMS2,
	"close��Ч��������"){

	EXPECT_EQ(CHDRV_AUDIO_Close(NULL),CHDRV_AUDIO_INVALID_PARAM);
}

CASE_F(TEST_AUDIO_PARAMS3,
	"start��Ч��������"){

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
	"stop��Ч��������"){

	CHDRV_AUDIO_StartDecodeParam_t stru_vidDecode;

	stru_vidDecode.enm_syncmode = CHDRV_AUDIO_SYNC;
	stru_vidDecode.enm_StreamType = CH_AUDIO_CODE_MPEG1;
	stru_vidDecode.enm_StreamPacketType = CHDRV_AUDIO_PACKET_PES;
	CHDRV_AUDIO_StartDecode(gh_AudioHandle,&stru_vidDecode);

	EXPECT_EQ(CHDRV_AUDIO_StopDecode(NULL),CHDRV_AUDIO_INVALID_PARAM);

	EXPECT_EQ(CHDRV_AUDIO_StopDecode(gh_AudioHandle),CHDRV_AUDIO_OK);
}
/***û�нӿ�
CASE_F(TEST_AUDIO_PARAMS5,
	"pause��Ч��������"){
	
	EXPECT_EQ(CHDRV_AUDIO_RamPauseDecode(NULL),CHDRV_AUDIO_OK);
}
***/

/****û�нӿ�
CASE_F(TEST_AUDIO_PARAMS6,
	"resume��Ч��������"){

	CHDRV_AUDIO_RamPauseDecode(gh_AudioHandle);
	
	EXPECT_EQ(CHDRV_AUDIO_RamResumeDecode(NULL),CHDRV_AUDIO_OK);
}
*****/

CASE_F(TEST_AUDIO_PARAMS7,
	"ram start��Ч��������"){

	EXPECT_EQ(CHDRV_AUDIO_RamStartDecode(gh_AudioHandle,NULL),CHDRV_AUDIO_INVALID_PARAM);
}

CASE_F(TEST_AUDIO_PARAMS8,
	"ram input��Ч��������"){

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
		GTEST_FAIL() << "Ӧͨ����������";
	}
}


CASE_F(TEST_AUDIO_PARAMS9,
	"ram stop��Ч��������"){

	int buffer_len = 0;
	char *buff = NULL;
	
	buffer_len = fsys_read_file(fw_getenv("BEEP_FILE_1"),buff);
	
	if( fw_getenv_i("DEAD_HALT_TEST"))
	{
		CHDRV_AUDIO_RamInputData(gh_AudioHandle,buff,buffer_len);
	}
	else
	{
		GTEST_FAIL() << "Ӧͨ����������";
	}
	
	

	CH_RTOS_FreeMemory(gh_APP_HEAP, buff);
	EXPECT_EQ(CHDRV_AUDIO_RamStopDecode(NULL),CHDRV_AUDIO_INVALID_PARAM);
}

CASE_F(TEST_AUDIO_PARAMS10,
	"ram pause��Ч��������"){

	EXPECT_EQ(CHDRV_AUDIO_RamPauseDecode(NULL),CHDRV_AUDIO_INVALID_PARAM);
}

CASE_F(TEST_AUDIO_PARAMS11,
	"ram resume��Ч��������"){

	CHDRV_AUDIO_RamPauseDecode(gh_AudioHandle);

	EXPECT_EQ(CHDRV_AUDIO_RamResumeDecode(NULL),CHDRV_AUDIO_INVALID_PARAM);
}



CASE_F(TEST_AUDIO_PARAMS12,
	"install callback��Ч��������"){

	EXPECT_EQ(CHDRV_AUDIO_InstallCallback(gh_AudioHandle,NULL),CHDRV_AUDIO_OK);
}

CASE_F(TEST_AUDIO_PARAMS13,
	"uninstall callback��Ч��������"){

	CHDRV_AUDIO_InstallCallback(gh_AudioHandle,audio_callback);

	EXPECT_EQ(CHDRV_AUDIO_UnInstallCallback(gh_AudioHandle,NULL),CHDRV_AUDIO_OK);
}

CASE_F(TEST_AUDIO_PARAMS14,
	"get status��Ч��������"){

	if( fw_getenv_i("DEAD_HALT_TEST"))
	{
		EXPECT_EQ(CHDRV_AUDIO_GetStatus(gh_AudioHandle,NULL),CHDRV_AUDIO_INVALID_PARAM);
	}
	else
	{
		GTEST_FAIL() << "Ӧͨ����������";
	}
	
	
	
	CHDRV_AUDIO_RamStopDecode(gh_AudioHandle);

	//CHDRV_AUDIO_Close(gh_AudioHandle);

}

/***û��ʵ��
CASE_F(TEST_AUDIO_PARAMS15,
	"get streaminfo ��Ч��������"){

	
}
*****/

/*
CASE_F(TEST_AUDIO_PARAMS16,
	"ram set decodeex param��Ч��������"){

	
}
*/
/**ͷ�ļ��ﶨ��.c�ļ���û��ʵ��
CASE_F(TEST_AUDIO_PARAMS17,
	"ram start ex��Ч��������"){

	EXPECT_EQ(CHDRV_AUDIO_RamStartDecodeEx(gh_AudioHandle,NULL),CHDRV_AUDIO_OK);
}

CASE_F(TEST_AUDIO_PARAMS18,
	"ram input ex��Ч��������"){

	CHDRV_AUDIO_StartDecodeParam_t stru_vidDecode;

	stru_vidDecode.enm_syncmode = CHDRV_AUDIO_SYNC;
	stru_vidDecode.enm_StreamType = CH_AUDIO_CODE_MPEG1;
	stru_vidDecode.enm_StreamPacketType = CHDRV_AUDIO_PACKET_PES;
	
	CHDRV_AUDIO_RamStartDecodeEx(gh_AudioHandle,&stru_vidDecode);

	EXPECT_EQ(CHDRV_AUDIO_RamInputDataEx(gh_AudioHandle,NULL,0),CHDRV_AUDIO_OK);
}

CASE_F(TEST_AUDIO_PARAMS19,
	"ram stop ex��Ч��������"){
	int buffer_len = 0;
	char *buff = NULL;
	
	read_Data("BEEF_FILE",buff);

	CHDRV_AUDIO_RamInputData(gh_AudioHandle,buff,buffer_len);

	EXPECT_EQ(CHDRV_AUDIO_RamStopDecodeEx(NULL),CHDRV_AUDIO_OK);

}

CASE_F(TEST_AUDIO_PARAMS20,
	"ram pause ex��Ч��������"){

	EXPECT_EQ(CHDRV_AUDIO_RamPauseDecodeEx(NULL),CHDRV_AUDIO_OK);
}

CASE_F(TEST_AUDIO_PARAMS21,
	"ram resume ex��Ч��������"){

	CHDRV_AUDIO_RamPauseDecodeEx(gh_AudioHandle);

	EXPECT_EQ(CHDRV_AUDIO_RamResumeDecodeEx(NULL),CHDRV_AUDIO_OK);
}

CASE_F(TEST_AUDIO_PARAMS22,
	"ram get buffer status��Ч��������"){

	EXPECT_EQ(CHDRV_AUDIO_RamDecodeGetBuffStatus(gh_AudioHandle,NULL),CHDRV_AUDIO_OK);
}

CASE_F(TEST_AUDIO_PARAMS23,
	"ram clear data��Ч��������"){

	EXPECT_EQ(CHDRV_AUDIO_RamDecodeClearData(NULL),CHDRV_AUDIO_OK);

	CHDRV_AUDIO_RamStopDecodeEx(gh_AudioHandle);

	CHDRV_AUDIO_Close(gh_AudioHandle);

}
*/
CASE_F(TEST_AUDIO_FUNCTION1,
	"����ȷ����open 1·audio���Ƿ񷵻سɹ����Ҿ����ΪNULL"){

	EXPECT_EQ(CHDRV_AUDIO_Open(0,&gh_AudioHandle),CHDRV_AUDIO_OK);
	EXPECT_NE(gh_AudioHandle,(void *)NULL);

}

CASE_F(TEST_AUDIO_FUNCTION2,
	"����get status����õ�״̬ӦΪidle"){

	CHDRV_AUDIO_STATUS_e enm_audStatus;

	CHDRV_AUDIO_GetStatus(gh_AudioHandle,&enm_audStatus);

	EXPECT_EQ(enm_audStatus,CHDRV_AUDIO_STATUS_IDLE);

	//CHDRV_AUDIO_Close(gh_AudioHandle);

}
#endif /* ENABLE_TESTCASE */

/*
CASE_F(TEST_AUDIO_INIT2,
	"����ģ���Ƿ�ɱ�����") {
	EXPECT_EQ(CHDRV_AUDIO_OK, CHDRV_AUDIO_Term());
	EXPECT_EQ(CHDRV_AUDIO_OK, CHDRV_AUDIO_Init());
}


CASE_F(TEST_AUDIO_INIT3,
	"����CHDRV_AUDIO_GetVersion�Ƿ�ɹ�") {
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
			//���Դ�route�л�ȡ����Щƽ̨�Ѵ�route�д���
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

/**û��ʵ��
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
/**ûʵ��
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


REGISTER_CMD(aud_init, "��ʼ��audioģ��", "",  "");
REGISTER_CMD(aud_term, "����audioģ��", "",  "");
REGISTER_CMD(aud_ram_start, "��ʼע�벥�Ž���", "device_id| filename |stream_type |packege_type",  "0 c:/1.mp3 2 0");
REGISTER_CMD(aud_ram_pause, "��ͣע�벥��", "device_id",  "0");
REGISTER_CMD(aud_ram_resume, "�ָ�ע�벥��", "device_id",  "0");
REGISTER_CMD(aud_ram_stop, "ֹͣע�벥��", "device_id",  "0");
REGISTER_CMD(aud_status, "��ӡ����״̬", "device_id",  "0");
REGISTER_CMD(aud_start, "��ʼlive���Ž���", "device_id|stream_type",  "0 0");
REGISTER_CMD(aud_stop, "ֹͣ����", "device_id",  "0");
REGISTER_CMD(aud_restart, "������ͣaudio����", "device_id | stream_type",  "0 0");
REGISTER_CMD(aud_info, "���audio�������Ϣ����ӡ������", "",  "");







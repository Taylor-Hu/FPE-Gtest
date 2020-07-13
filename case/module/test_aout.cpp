/**
@file test_aout.cpp
@def group aout测试集

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
#define CASE_NAME TestAout


#include "test_menu.h"
#include "test_modules.h"

//定义错误参数
#define  ERROR_DEVICEID  						(-2)
#define  ERROR_LEFTVOLUME  						(-2)
#define  ERROR_RIGHTVOLUME 						(-2)
#define  ERROR_ENM_SOUNDCHANNEL 				(-2)


//定义合法参数
#define  NORMAL_DEVIDEID 						(0)
#define  NORMAL_LEFTVOLUME  					(30)
#define  NORMAL_RIGHTVOLUME  					(30)
#define  NORMAL_ENM_SOUNDCHANNEL  				(1)

#define  DEVICE_CVBS								(0)
#define  DEVICE_SPDIF								(1)
#define  DEVICE_HDMI								(2)
#define MAX_DEVICE_ID					3

//定义音量1 到音量5 从小到大变化
#define  LeftVolume1							(0)
#define  RightVolume1							(0)
#define  LeftVolume2							(10)
#define  RightVolume2							(10)
#define  LeftVolume3							(30)
#define  RightVolume3							(30)
#define  LeftVolume4							(50)
#define  RightVolume4							(50)
#define  LeftVolume5							(63)
#define  RightVolume5							(63)


//定义全局变量
static CHDRV_AOUT_DeviceHandle gh_SoundHandle[MAX_DEVICE_ID];


void CASE_NAME::SetUpTestCase() 
{
	//excute before each test suit
	//std::cout << "init SetUp rtos TestCase " << std::endl;

	int prognum = 0;

	prognum = request_play_program("AV_MULAUDIO_1", "TUNER_PARAMS1");
	RUN_CMD("tuner.lock %s", fw_getenv("TUNER_PARAMS1"));

#ifdef CFG_CHSSA_HD_SUPPORT	
	RUN_CMD("vout.set_format 6");
#else
	RUN_CMD("vout.set_format 1");
#endif	
	
	RUN_CMD("av.start %d %d\n",get_demux_id("TUNER_PARAMS1"),prognum);

									//播放节目
	int type;
	do
	{
		type = switch_multiaud(NULL, NULL);
	}
	while(type  < CH_AUDIO_CODE_TYPE_MAX 
		&& type != CH_AUDIO_CODE_MPEG1
		&& type != CH_AUDIO_CODE_MPEG2);
}
void CASE_NAME::TearDownTestCase() 
{
	//excute after each test suit
	//std::cout << "deinit tuner rtos TestCase " << std::endl;

	RUN_CMD("av.stop 0");			//停止播放
	EXPECT_EQ(CHDRV_AOUT_OK, CHDRV_AOUT_Term());
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
	dbg_checkmem(0);
}
#if ENABLE_TESTCASE

//测试CHDRV_AOUT_SetMute静音或者取消静音
//rb_MuteOnOff : 
// CH_TRUE --> 测试静音状态 
// CH_FALSE --> 测试取消静音状态
static CHDRV_AOUT_RESULT_e  TEST_CHDRV_AOUT_SetMute(CH_BOOL rb_MuteOnOff)
{

	 int i_DeviceID = 0;

	CH_BOOL b_MuteOnOff = CH_FALSE;	

	for(i_DeviceID = 0; i_DeviceID < MAX_DEVICE_ID; i_DeviceID++)
	{
		if( i_DeviceID == DEVICE_SPDIF && fw_getenv_i("SUPPORT_SPDIF") == 1)
		{
			PROMPT("请连接SPDIF,确认SPDIF通道是否处于%s状态,是请输入Y，否则输入N:", rb_MuteOnOff ? "静音": "非静音");	
		}
		else if(i_DeviceID == DEVICE_HDMI && fw_getenv_i("SUPPORT_HDMI") == 1)
		{
			PROMPT("请连接HDMI,并把电视设置为HDMI输入,确认HDMI通道是否处于%s状态,是请输入Y，否则输入N:", rb_MuteOnOff ? "静音": "非静音");	
		}
		else if(i_DeviceID == DEVICE_CVBS && fw_getenv_i("SUPPORT_CVBS") == 1)
		{
			PROMPT("请连接CVBS,并把电视设置为CVBS输入,确认LR通道是否处于%s状态,是请输入Y，否则输入N:", rb_MuteOnOff ? "静音": "非静音");	
		}
		else
		{
			continue;
		}
		EXPECT_EQ(CHDRV_AOUT_OK, CHDRV_AOUT_SetMute(gh_SoundHandle[i_DeviceID], rb_MuteOnOff));

			
		EXPECT_EQ(CH_TRUE,  test_input_yes()); 		//判断静音状态是否设置成功
		EXPECT_EQ(CHDRV_AOUT_OK, CHDRV_AOUT_GetMuteStatus(gh_SoundHandle[i_DeviceID], &b_MuteOnOff));
		EXPECT_EQ(rb_MuteOnOff, b_MuteOnOff);		//获取到的状态应跟设置的状态一致
	}
	return CHDRV_AOUT_OK;		
}
//测试CHDRV_AOUT_SetSoundChannelMode设置模拟声道
static CHDRV_AOUT_RESULT_e  TEST_CHDRV_AOUT_SetSoundChannelMode(int device_id, CHDRV_AOUT_SOUND_CHANNEL_e renm_SoundChannel)
{
	const char *pstr_DeviceName[MAX_DEVICE_ID] = {"CVBS", "SPDIF", "HDMI"};
	if(renm_SoundChannel >= CHDRV_AOUT_SOUND_CHANNEL_MAX)
	{
		LOG("TEST_CHDRV_AOUT_SetSoundChannelMode INVALID_PARAM\n");	
		return CHDRV_AOUT_INVALID_PARAM;
	}
	EXPECT_EQ(CHDRV_AOUT_OK, CHDRV_AOUT_SetSoundChannelMode(gh_SoundHandle[device_id], renm_SoundChannel));		//设置为左声道
	
	if(renm_SoundChannel == CHDRV_AOUT_SOUND_LEFT_TO_BOTH)
	{
		PROMPT("请确认当前%s输出声道是否为左声道，是请输入Y，否则输入N:", pstr_DeviceName[device_id]);	
	}
	else if(renm_SoundChannel == CHDRV_AOUT_SOUND_RIGHT_TO_BOTH)
	{
		PROMPT("请确认当前%s输出声道是否为右声道，是请输入Y，否则输入N:", pstr_DeviceName[device_id]);	
	}
	else if(renm_SoundChannel == CHDRV_AOUT_SOUND_STEREO)
	{
		PROMPT("请确认当前%s输出声道是否为立体声道，是请输入Y，否则输入N:", pstr_DeviceName[device_id]);	
	}
	else
	{
		
	}

	
	EXPECT_EQ(CH_TRUE, test_input_yes()) << "测试员认为"<< pstr_DeviceName[device_id] << "声道控制未起作用";			//测试员听到的输出声道应跟设置的模拟声道一致

	return CHDRV_AOUT_OK;
}

//测试SPDIF、HDMI压缩和非压缩模式
//rb_SetOutMode     : CH_TRUE : HDMI			CH_FALSE : SPDIF
//rb_CompressMode : CH_TRUE : 压缩模式	CH_FALSE : 非压缩模式
static CHDRV_AOUT_RESULT_e  TEST_CHDRV_AOUT_SetSPDIF_HDMIOutMode(int ri_deviceId, CH_BOOL rb_CompressMode)
{
	
	EXPECT_EQ(CHDRV_AOUT_OK, CHDRV_AOUT_SetSPDIFOutMode(rb_CompressMode));				//设置SPDIFOutMode


	PROMPT("请确认%s输出格式是否为%s，是请输入Y，否则输入N:", 
		ri_deviceId == DEVICE_HDMI ? "HDMI" : "SPDIF",
		rb_CompressMode ? "dolby" : "pcm");
	


	EXPECT_EQ(CH_TRUE, test_input_yes()) << (ri_deviceId == DEVICE_HDMI ? "HDMI" : "SPDIF") << "输出" << (rb_CompressMode ? "压缩" : "不压缩") << "模式不符合预期";

	return CHDRV_AOUT_OK;
}


CASE_F(TEST_AOUT_INIT1,
	"测试多次初始化时是否返回OK") {
	EXPECT_EQ(CHDRV_AOUT_OK, CHDRV_AOUT_Init());
	EXPECT_EQ(CHDRV_AOUT_OK, CHDRV_AOUT_Init()) << "多次初始化应该返回OK";
}


CASE_F(TEST_AOUT_INIT2,
	"测试模块是否可被销毁") {
	EXPECT_EQ(CHDRV_AOUT_OK, CHDRV_AOUT_Term());
	EXPECT_EQ(CHDRV_AOUT_OK, CHDRV_AOUT_Init());
}


CASE_F(TEST_AOUT_INIT3,
	"测试CHDRV_AOUT_GetVersion是否成功") {
	CHDRV_Version_t stru_Ver;
	
	memset(&stru_Ver, 0, sizeof(stru_Ver));
	EXPECT_EQ(CHDRV_AOUT_OK, CHDRV_AOUT_GetVersion(&stru_Ver));
	EXPECT_NE(0, stru_Ver);
	LOG("\nstru_Ver=%x\n", stru_Ver);
}

CASE_F(TEST_AOUT_PARAMS1,
	"测试CHDRV_AOUT_Open错误参数测试")
{
	CHDRV_AOUT_DeviceHandle h_SoundHandle;
	
	EXPECT_EQ(CHDRV_AOUT_INVALID_PARAM, CHDRV_AOUT_Open(ERROR_DEVICEID, &h_SoundHandle));
//	EXPECT_EQ(CHDRV_AOUT_INVALID_PARAM, CHDRV_AOUT_Open(NORMAL_DEVIDEID, NULL));
//CHDRV_AOUT_Open 里面没有对错误参数判断，空指针传进去会导致死机
}

CASE_F(TEST_AOUT_PARAMS2,
	"测试CHDRV_AOUT_SetMute和CHDRV_AOUT_GetMuteStatus错误参数")
{
	CHDRV_AOUT_DeviceHandle h_SoundHandle = NULL;
	CH_BOOL b_MuteOnOff = CH_TRUE;
	
	EXPECT_EQ(CHDRV_AOUT_INVALID_PARAM, CHDRV_AOUT_SetMute(NULL, b_MuteOnOff));
	EXPECT_EQ(CHDRV_AOUT_INVALID_PARAM, CHDRV_AOUT_GetMuteStatus(NULL, &b_MuteOnOff));
	EXPECT_EQ(CHDRV_AOUT_INVALID_PARAM, CHDRV_AOUT_GetMuteStatus(h_SoundHandle, NULL));
}

CASE_F(TEST_AOUT_PARAMS3,
	"测试CHDRV_AOUT_SetVolume和GetVolume错误参数")
{
	CHDRV_AOUT_DeviceHandle h_SoundHandle = NULL;
	CH_BOOL b_MuteOnOff = CH_TRUE;
		
	EXPECT_EQ(CHDRV_AOUT_INVALID_PARAM, CHDRV_AOUT_SetVolume(NULL, NORMAL_LEFTVOLUME, NORMAL_RIGHTVOLUME));
	EXPECT_EQ(CHDRV_AOUT_INVALID_PARAM, CHDRV_AOUT_SetVolume(h_SoundHandle, ERROR_LEFTVOLUME, NORMAL_RIGHTVOLUME));
	EXPECT_EQ(CHDRV_AOUT_INVALID_PARAM, CHDRV_AOUT_SetVolume(h_SoundHandle, NORMAL_LEFTVOLUME, ERROR_RIGHTVOLUME));
	EXPECT_EQ(CHDRV_AOUT_INVALID_PARAM, CHDRV_AOUT_GetMuteStatus(NULL, &b_MuteOnOff));
	EXPECT_EQ(CHDRV_AOUT_INVALID_PARAM, CHDRV_AOUT_GetMuteStatus(h_SoundHandle, NULL));
}

CASE_F(TEST_AOUT_PARAMS4,
	"测试CHDRV_AOUT_SetSoundChannelMode和GetSoundChannelMode错误参数")
{
	CHDRV_AOUT_DeviceHandle h_SoundHandle = NULL;
	CHDRV_AOUT_SOUND_CHANNEL_e enm_ErrorSoundChannel = CHDRV_AOUT_SOUND_MONO;

	EXPECT_EQ(CHDRV_AOUT_INVALID_PARAM, CHDRV_AOUT_SetSoundChannelMode(NULL, CHDRV_AOUT_SOUND_MONO));
	EXPECT_EQ(CHDRV_AOUT_INVALID_PARAM, CHDRV_AOUT_SetSoundChannelMode(h_SoundHandle, CHDRV_AOUT_SOUND_CHANNEL_MAX));
	EXPECT_EQ(CHDRV_AOUT_INVALID_PARAM, CHDRV_AOUT_GetSoundChannelMode(NULL, &enm_ErrorSoundChannel));
	EXPECT_EQ(CHDRV_AOUT_INVALID_PARAM, CHDRV_AOUT_GetSoundChannelMode(h_SoundHandle, NULL));
}

#if 0 	/*sqzou20171128*/
CASE_F(TEST_AOUT_PARAMS5,
	"测试CHDRV_AOUT_SetSPDIFOutMode和CHDRV_AOUT_SetHDMIOutMode错误参数")
{
	CH_BOOL b_CompressMode = CH_FALSE;

	EXPECT_EQ(CHDRV_AOUT_INVALID_PARAM, CHDRV_AOUT_SetSPDIFOutMode(b_CompressMode));
	EXPECT_EQ(CHDRV_AOUT_INVALID_PARAM, CHDRV_AOUT_SetHDMIOutMode(b_CompressMode));
}
#endif

CASE_F(TEST_AOUT_PARAMS6,
	"测试CHDRV_AOUT_Close错误参数")
{
	EXPECT_EQ(CHDRV_AOUT_INVALID_PARAM, CHDRV_AOUT_Close(NULL));
}

CASE_F(TEST_AOUT_OPEN,
	"测试CHDRV_AOUT_Open正确参数打开，返回值应正确，handle不为NULL")
{
	
	 int i_DeviceID = 0;
	for(i_DeviceID = 0; i_DeviceID < MAX_DEVICE_ID; i_DeviceID++)
	{
		if((i_DeviceID == DEVICE_CVBS && fw_getenv_i("SUPPORT_CVBS") == 1)
			|| (i_DeviceID == DEVICE_SPDIF && fw_getenv_i("SUPPORT_SPDIF") == 1)
			|| (i_DeviceID == DEVICE_HDMI && fw_getenv_i("SUPPORT_HDMI") == 1))
		{
			EXPECT_EQ(CHDRV_AOUT_OK, CHDRV_AOUT_Open(i_DeviceID, &gh_SoundHandle[i_DeviceID]));
		}
	}
}



CASE_F(TEST_AOUT_MUTE,
	"测试静音或者取消静音,对应端口应能关闭输出;CHDRV_AOUT_GetMuteStatus能否正确返回静音状态")
{
	EXPECT_EQ(CHDRV_AOUT_OK, TEST_CHDRV_AOUT_SetMute(CH_TRUE));				//测试静音状态								//设置音频输出通道为DVBS
	EXPECT_EQ(CHDRV_AOUT_OK, TEST_CHDRV_AOUT_SetMute(CH_FALSE));			//测试取消静音状态										//设置音频输出通道为DVBS
}

CASE_F(TEST_AOUT_VOLUME,
	"测试设置不同音量在CVBS端口能听到音量变化,错误参数不会改变原来设定值;GetVolume获取当前音量正确")
{	
	S8 c_LeftVolume = 0;
	S8 c_RightVolume = 0;
	 int i_DeviceID = 0;

	 for(i_DeviceID = 0; i_DeviceID < MAX_DEVICE_ID; i_DeviceID++)
	 {
		if((i_DeviceID == DEVICE_CVBS && fw_getenv_i("SUPPORT_CVBS") == 1)
			|| (i_DeviceID == DEVICE_HDMI && fw_getenv_i("SUPPORT_HDMI") == 1))
		{
		
			PROMPT("请连接%s,并把电视切换到%s输入,然后回车", DEVICE_CVBS == i_DeviceID ? "CVBS" : "HDMI",
				DEVICE_CVBS == i_DeviceID ? "CVBS" : "HDMI");	
			test_get_input();
			
			PROMPT("请仔细听几秒,然后确认输出音量是否从小到大变化，准备好以后按回车");
			test_input_none();
			EXPECT_EQ(CHDRV_AOUT_OK, CHDRV_AOUT_SetVolume(gh_SoundHandle[i_DeviceID], LeftVolume1, RightVolume1));		//设置音量1
			CH_RTOS_TaskDelay(1000);  
			EXPECT_EQ(CHDRV_AOUT_OK, CHDRV_AOUT_SetVolume(gh_SoundHandle[i_DeviceID], LeftVolume2, RightVolume2));		//设置音量1
			CH_RTOS_TaskDelay(1000);  
			EXPECT_EQ(CHDRV_AOUT_OK, CHDRV_AOUT_SetVolume(gh_SoundHandle[i_DeviceID], LeftVolume3, RightVolume3));		//设置音量2
			CH_RTOS_TaskDelay(1000);  
			EXPECT_EQ(CHDRV_AOUT_OK, CHDRV_AOUT_SetVolume(gh_SoundHandle[i_DeviceID], LeftVolume4, RightVolume4));		//设置音量2
			CH_RTOS_TaskDelay(1000);  
			EXPECT_EQ(CHDRV_AOUT_OK, CHDRV_AOUT_SetVolume(gh_SoundHandle[i_DeviceID], LeftVolume5, RightVolume5));		//设置音量3

			PROMPT("请确认输出音量是否从小到大变化，是请输入Y，否则输入N:");
			

			EXPECT_EQ(CH_TRUE, test_input_yes()) << "测试员认为音量控制不起作用";								//测试员听到的音量变化应该跟设置的一样(从小到大)
			EXPECT_EQ(CHDRV_AOUT_INVALID_PARAM, CHDRV_AOUT_SetVolume(gh_SoundHandle, ERROR_LEFTVOLUME, ERROR_RIGHTVOLUME));	//设置错误参数
			EXPECT_EQ(CHDRV_AOUT_OK, CHDRV_AOUT_GetVolume(gh_SoundHandle, &c_LeftVolume, &c_RightVolume));			//获取音量参数
			EXPECT_EQ(LeftVolume5, c_LeftVolume) << "获取的音量与设置的不一致";					//获取到的音量应跟设置的一致,100 * LeftVolume3/64 是根据CH_PLATFORM_AOUT_Volume_ch2mstar 算出原来的音量	
			EXPECT_EQ(RightVolume5, c_RightVolume) << "获取的音量与设置的不一致";				//获取到的音量应跟设置的一致,100 * RightVolume3/64 是根据CH_PLATFORM_AOUT_Volume_ch2mstar 算出原来的音量	
		}
	}
}


CASE_F(TEST_AOUT_CHANNEL,
	"设置SetSoundChannelMode不同参数能够正确响应,错误参数不会改变原先设定值;GetSoundChannelMode获取数据正确")
{	
	CHDRV_AOUT_SOUND_CHANNEL_e enm_GetSoundChannel;
	 int i_DeviceID = 0;

	 for(i_DeviceID = 0; i_DeviceID < MAX_DEVICE_ID; i_DeviceID++)
	 {
		if((i_DeviceID == DEVICE_CVBS && fw_getenv_i("SUPPORT_CVBS") == 1)
			|| (i_DeviceID == DEVICE_HDMI && fw_getenv_i("SUPPORT_HDMI") == 1))
		{
			EXPECT_EQ(CHDRV_AOUT_OK, TEST_CHDRV_AOUT_SetSoundChannelMode(i_DeviceID, CHDRV_AOUT_SOUND_LEFT_TO_BOTH));							//测试左声道
			EXPECT_EQ(CHDRV_AOUT_OK, TEST_CHDRV_AOUT_SetSoundChannelMode(i_DeviceID, CHDRV_AOUT_SOUND_RIGHT_TO_BOTH));							//测试右声道
			EXPECT_EQ(CHDRV_AOUT_OK, TEST_CHDRV_AOUT_SetSoundChannelMode(i_DeviceID, CHDRV_AOUT_SOUND_STEREO));									//测试立体声道
			EXPECT_EQ(CHDRV_AOUT_INVALID_PARAM, CHDRV_AOUT_SetSoundChannelMode(gh_SoundHandle[i_DeviceID], CHDRV_AOUT_SOUND_CHANNEL_MAX));		//设置错误参数
			EXPECT_EQ(CHDRV_AOUT_OK, CHDRV_AOUT_GetSoundChannelMode(gh_SoundHandle[i_DeviceID], &enm_GetSoundChannel));							//获取设置的模拟声道参数
			EXPECT_EQ(CHDRV_AOUT_SOUND_STEREO, enm_GetSoundChannel) << "获取的声道参数应跟最后设置的声道参数不一致";
		}
	}
}



CASE_F(TEST_AOUT_SPDIF,
	"测试CHDRV_AOUT_SetSPDIFOutMode设置为压缩和非压缩方式，端口应正确输出，返回值正确")
{	

	int type;
	do
	{
		type = switch_multiaud(NULL, NULL);
	}
	while(type  < CH_AUDIO_CODE_TYPE_MAX 
		&& type != CH_AUDIO_CODE_AC3);

	if(fw_getenv_i("SUPPORT_SPDIF") == 1)
	{
		PROMPT("请用可以显示SPDIF输入格式的设备连接SPDIF,准备好以后回车");	
		test_get_input();
		EXPECT_EQ(CHDRV_AOUT_OK, TEST_CHDRV_AOUT_SetSPDIF_HDMIOutMode(DEVICE_SPDIF, CH_TRUE));			//测试SPDIF压缩方式
		EXPECT_EQ(CHDRV_AOUT_OK, TEST_CHDRV_AOUT_SetSPDIF_HDMIOutMode(DEVICE_SPDIF, CH_FALSE));			//测试SPDIF非压缩方式
	}
}


CASE_F(TEST_AOUT_HDMIOUT,
	"测试CHDRV_AOUT_SetHDMIOutMode设置为压缩和非压缩方式，端口应能正确输出，返回值正确")
{		
	if(fw_getenv_i("SUPPORT_HDMI") == 1)
	{
		PROMPT("请用可以显示HDMI音频输入格式的设备连接HDMI,准备好以后回车");	
		test_get_input();
		EXPECT_EQ(CHDRV_AOUT_OK, TEST_CHDRV_AOUT_SetSPDIF_HDMIOutMode(DEVICE_HDMI, CH_TRUE));			//测试HDMI压缩方式
		EXPECT_EQ(CHDRV_AOUT_OK, TEST_CHDRV_AOUT_SetSPDIF_HDMIOutMode(DEVICE_HDMI, CH_FALSE));			//测试HDMI非压缩方式
	}
}

CASE_F(TEST_AOUT_CLOSE,
	"测试CHDRV_AOUT_Close删除句柄，返回值正确")
{	
	 int i_DeviceID = 0;

	 for(i_DeviceID = 0; i_DeviceID < MAX_DEVICE_ID; i_DeviceID++)
	 {
		if((i_DeviceID == DEVICE_CVBS && fw_getenv_i("SUPPORT_CVBS") == 1)
				|| (i_DeviceID == DEVICE_SPDIF && fw_getenv_i("SUPPORT_SPDIF") == 1)
				|| (i_DeviceID == DEVICE_HDMI && fw_getenv_i("SUPPORT_HDMI") == 1))
		{
			EXPECT_EQ(CHDRV_AOUT_OK, CHDRV_AOUT_Close(gh_SoundHandle[i_DeviceID]));	
			gh_SoundHandle[i_DeviceID] = NULL;
		}
	}
}
#endif /* ENABLE_TESTCASE */

extern int aout_info(int argc, CMD_PARAM argv[]);


extern int aout_open(int argc, CMD_PARAM argv[]);

extern int aout_close(int argc, CMD_PARAM argv[]);

extern int aout_mute(int argc, CMD_PARAM argv[]);

extern int aout_volume(int argc, CMD_PARAM argv[]);

extern int aout_mode(int argc, CMD_PARAM argv[]);

REGISTER_CMD(aout_info, "显示aout信息", "",  "");
REGISTER_CMD(aout_open, "创建aout句柄", "device_id",  "0");
REGISTER_CMD(aout_close, "删除aout句柄", "device_id",  "0");
REGISTER_CMD(aout_mute, "设置所有设备为静音", "flag",  "false");
REGISTER_CMD(aout_volume, "设置所有设备的音量", "vol",  "63");
REGISTER_CMD(aout_mode, "设置所有设备的模式", "mode(0~4)",  "4");






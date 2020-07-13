/**
@file test_mplay.cpp
@def group mplay测试集

@author 网络公司基础技术部:	邹书强
@date   2014年4月10日  
@version V4.0
@todo     ---时间---作者-------------修改部分及原因----------
@todo     1 2014年4月10日 邹书强  创建

@todo     2 2018年7月24日 蔡晓丽增加测试用列

@{
*/

/*测试集名称定义*/
#ifdef CASE_NAME
#undef CASE_NAME
#endif
#define CASE_NAME TestMplay


#include "test_menu.h"
#include "test_modules.h"
#include "ch_stbtype.h"
#include "psi_parse.h"


//定义错误参数
#define MPLAY_ERRO_POSSECEND			(-1)
#define MPLAY_ERRO_PARAM				(-1)
#define EMPLAY_RRO_PARAMEX			(-1)
#define MPLAY_ERRO_STEPMS				(-1)
#define MPLAY_ERRO_STOPMS				(-1)

//定义合法参数
#define MPLAY_NORMAL_POSSECEND			(5)
#define MPLAY_NORMAL_PARAM				(5)
#define MPLAY_NORMAL_PARAMEX			(5)
#define MPLAY_NORMAL_STEPMS				(5)
#define MPLAY_NORMAL_STOPMS				(5)

#define MPLAY_URLSUM					(5)

//定义全局变量
CH_MPLAY_Handle gh_GtestMplayHandle = NULL;  /*gh_GtestMplayHandle*/
S32 gi_AudioTrackNum = 0;
U16 guc_TestCount = 0;		//记录第几次测试
U16 gus_UrlConut = 0;		//记录有定义的URL数量
U8 guc_UrlType[][50] ={"TEST_MPLAY_URL_LOCAL", "TEST_MPLAY_URL_HTTP", "TEST_MPLAY_URL_HTTPS", "TEST_MPLAY_URL_RTSP", "TEST_MPLAY_URL_RTSPS"}; 
U8 (*gpuc_UrlType)[50] = guc_UrlType;
U8 guc_Protocol[][10] = {"", "http://", "https://", "rtsp://", "rtsps://"};

/*每种媒体格式进行一次测试，需要测试的媒体格式为，测试的种类在参数MPLAY_URLSUM显示*/
/*本地媒体播放格式5种*/
/*MKV,AVI ,WMV,MP4,FLV,ASF*/
/*网络协议测试 4种*/
/*http,https,rtsp,rtsps*/
/*流媒体格式测试2种*/
/*HLS,Smooth Stream,*/
/*MPLAY_URLSUM 至少11次*/

U8 guc_UrlName[MPLAY_URLSUM][256] = {""};				//保存有定义的url
U8 (*gpuc_UrlName)[256] = guc_UrlName;

void CASE_NAME::SetUpTestCase() {
//excute before each test suit
//std::cout << "init SetUp rtos TestCase " << std::endl;

	system("mount /dev/sda1 /home");
	RUN_CMD("route.create 0 0");
	RUN_CMD("vout.open 0 1 1 1 0 0 0 0\n");
#ifdef CFG_CHSSA_HD_SUPPORT	
	RUN_CMD("vout.open 1 1 1 1 0 0 0 0\n");
	RUN_CMD("vout.set_format 6");
#else
	RUN_CMD("vout.set_format 1");
#endif	

	if(guc_TestCount == 0)		//第一次开始测试前先获取有定义的URL
	{
		U8 uc_Count = 0;

		for(uc_Count=0;uc_Count<MPLAY_URLSUM;uc_Count++)
		{
			U8 *puc_Url = NULL;
		
			puc_Url = (U8*)fw_getenv((char*)gpuc_UrlType[uc_Count]); 	//获取test_config.cfg中的URL
			
			if(puc_Url == NULL)			//URL 未定义
			{
				continue;
			}
			else
			{
				strcat(gpuc_UrlName[gus_UrlConut], guc_Protocol[gus_UrlConut]);	//通过fw_getenv无法获取"http://192.168.32.32/1.mp4"中的"//",拆成"http://"和"192.168.32.32/1.mp4"两部分,再用strcat拼接
				strcat(gpuc_UrlName[gus_UrlConut], puc_Url); 				//更新guc_UrlName
				gus_UrlConut++;
			}
		}
	    
		if(gus_UrlConut == 0) //一个URL都没有定义
		{
			PROMPT("\n未定义播放路径,请在test_config.cfg中定义要测试的播放路径\n");
			test_get_input();
			UnitTest *pCTest = testing::UnitTest::GetInstance();		
			TestCase* test_case = pCTest->current_test_case();		
			test_case->Run();
		}
		else
		{
			PROMPT("\n现在开始测试码流%s,请准备好测试环境后回车\n", gpuc_UrlName[guc_TestCount]);
			test_get_input();
		}
	}
	
	
}
void CASE_NAME::TearDownTestCase() {
//excute after each test suit
//std::cout << "deinit pvrn rtos TestCase " << std::endl;
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

CASE_F(TEST_MPLAY_INIT1,
	"测试多次初始化时是否返回OK") {

	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Init());
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Init()) << "多次初始化应该返回OK";	
}



CASE_F(TEST_MPLAY_INIT2,
	"测试模块是否可被销毁") {
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Term());
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Init());
}


CASE_F(TEST_MPLAY_INIT3,
	"测试CH_MPLAY_GetVersion是否成功") {
	CHDRV_Version_t stru_Ver;
	
	memset(&stru_Ver, 0, sizeof(stru_Ver));
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_GetVersion(&stru_Ver));
	EXPECT_NE(0, stru_Ver);
	LOG("\nstru_Ver=%x\n", stru_Ver);
}

CASE_F(TEST_MPLAY_OPEN,
	"测试模块Open与Close可否正常对应") {
	EXPECT_EQ(CH_MPLAY_OK,CH_MPLAY_Open(NULL, &gh_GtestMplayHandle));
	EXPECT_EQ(CH_MPLAY_OK,CH_MPLAY_Close(gh_GtestMplayHandle));
}

CASE_F(TEST_MPLAY_PARAMS1,
	"CH_MPLAY_Close错误参数测试") {
	
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Close(NULL));	
}

CASE_F(TEST_MPLAY_PARAMS2,
	"CH_MPLAY_Ctrl错误参数测试") {

	CH_MPLAY_CTRL_e enm_CtrlType = CH_MPLAY_CMD_GET_TRACK_NUM;
	S32 i_Param = -1;
	S32 i_ParamEx = 0;
		
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Ctrl(NULL, enm_CtrlType, i_Param, i_ParamEx));	
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Ctrl(gh_GtestMplayHandle, CH_MPLAY_CMD_MAX, i_Param, i_ParamEx));	
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Ctrl(gh_GtestMplayHandle, CH_MPLAY_CMD_SET_TRACK_INDEX, i_Param, i_ParamEx));	
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Ctrl(gh_GtestMplayHandle, CH_MPLAY_CMD_GET_TRACK_NUM, i_Param, i_ParamEx));	
}

CASE_F(TEST_MPLAY_PARAMS3,
	"CH_MPLAY_GetMediaInfo错误参数测试") {

	U8 uc_url = 0;
	CH_MPLAY_MediaInfo_t stru_MediaInfo;
	memset(&stru_MediaInfo, 0, sizeof(stru_MediaInfo));
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_GetMediaInfo(NULL, &stru_MediaInfo));	
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_GetMediaInfo(&uc_url, NULL));	
}

CASE_F(TEST_MPLAY_PARAMS4,
	"CH_MPLAY_GetStatus错误参数测试") {

	CH_MPLAY_STATUS_t stru_PlayStatus;

	memset(&stru_PlayStatus, 0, sizeof(stru_PlayStatus));
	int dead_test = fw_getenv_i("DEAD_HALT_TEST");	
	
	if(dead_test == 1)	
	{		
		EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_GetStatus(NULL, &stru_PlayStatus));	
		EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_GetStatus(gh_GtestMplayHandle, NULL));	
	}	
	else	
	{		
		EXPECT_EQ(dead_test, 1) << "应通过死亡测试";	
	}
}

CASE_F(TEST_MPLAY_PARAMS5,
	"CH_MPLAY_Open错误参数测试") {

	U32 i_Route = 0;
	int dead_test = fw_getenv_i("DEAD_HALT_TEST");	
	
	if(dead_test == 1)	
	{		
		/*EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Open(NULL, &gh_GtestMplayHandle));	*/
		EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Open(&i_Route, NULL));	
	}	
	else	
	{		
		EXPECT_EQ(dead_test, 1) << "应通过死亡测试";	
	}
}

CASE_F(TEST_MPLAY_PARAMS6,
	"CH_MPLAY_Pause错误参数测试") {
	
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Pause(NULL));	
}

CASE_F(TEST_MPLAY_PARAMS7,
	"CH_MPLAY_Poll错误参数测试") {
//	CH_MPLAY_Poll为static 类型,无法调用
//	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Poll(NULL));	
}

CASE_F(TEST_MPLAY_PARAMS8,
	"CH_MPLAY_Resume错误参数测试") {
	
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Resume(NULL));	
}

CASE_F(TEST_MPLAY_PARAMS9,
	"CH_MPLAY_Seek错误参数测试") {
	
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Seek(NULL, MPLAY_NORMAL_POSSECEND));	
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Seek(gh_GtestMplayHandle, MPLAY_ERRO_POSSECEND));	
}

CASE_F(TEST_MPLAY_PARAMS10,
	"CH_MPLAY_SetConfig错误参数测试") {

	CH_MPLAY_CFG_TYPE_e enm_Type = CH_MPLAY_CFG_CB;
	
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_SetConfig(CH_MPLAY_CFG_MAX, MPLAY_NORMAL_PARAM, MPLAY_NORMAL_PARAMEX));	
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_SetConfig(enm_Type, MPLAY_ERRO_PARAM, MPLAY_NORMAL_PARAMEX));	
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_SetConfig(enm_Type, MPLAY_NORMAL_PARAM, EMPLAY_RRO_PARAMEX));	
}

CASE_F(TEST_MPLAY_PARAMS11,
	"CH_MPLAY_Speed错误参数测试") {

	CH_MPLAY_SPEED_PLAY_e enm_Type = CH_MPLAY_PLAY_FFW;
	CH_MPLAY_PLAY_SPEED_e enm_Speed = CH_MPLAY_PLAY_SPEED_1;
	
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Speed(NULL, enm_Type, enm_Speed));	
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Speed(gh_GtestMplayHandle, CH_MPLAY_PLAY_TYPE_MAX, enm_Speed));	
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Speed(gh_GtestMplayHandle, enm_Type, CH_MPLAY_PLAY_SPEED_MAX));	
}

CASE_F(TEST_MPLAY_PARAMS12,
	"CH_MPLAY_Start错误参数测试") {

	U8 uc_url = 0;
	int dead_test = fw_getenv_i("DEAD_HALT_TEST");	
	
	if(dead_test == 1)	
	{		
		EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Start(NULL, &uc_url));	
		EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Start(gh_GtestMplayHandle, NULL));	
	}	
	else	
	{		
		EXPECT_EQ(dead_test, 1) << "应通过死亡测试";	
	}
}

CASE_F(TEST_MPLAY_PARAMS13,
	"CH_MPLAY_Step错误参数测试") {

	CH_MPLAY_STEP_PLAY_e enm_Step = CH_MPLAY_PLAY_FSTEP;
	
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Step(NULL, enm_Step, MPLAY_NORMAL_STEPMS, MPLAY_NORMAL_STOPMS));	
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Step(gh_GtestMplayHandle, CH_MPLAY_PLAY_MAX, MPLAY_NORMAL_STEPMS, MPLAY_NORMAL_STOPMS));	
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Step(gh_GtestMplayHandle, enm_Step, MPLAY_ERRO_STEPMS, MPLAY_NORMAL_STOPMS));	
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Step(gh_GtestMplayHandle, enm_Step, MPLAY_NORMAL_STEPMS, MPLAY_ERRO_STOPMS));	
}


CASE_F(TEST_MPLAY_PARAMS14,
	"CH_MPLAY_Stop错误参数测试") {
	
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Stop(NULL));	
}

CASE_F(TEST_MPLAY_START1,
	"调用open创建一个句柄，route参数从test route模块获取") {

	U32 i_Route = 0;

	gh_GtestMplayHandle = NULL;
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Open(&i_Route, &gh_GtestMplayHandle));
	EXPECT_EQ(NULL, gh_GtestMplayHandle) << "句柄应不为空\n";	
}

CASE_F(TEST_MPLAY_START2,
	"getstatus状态应为IDL") {

	CH_MPLAY_STATUS_t stru_PlayStatus;

	memset(&stru_PlayStatus, 0, sizeof(stru_PlayStatus));
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_GetStatus(gh_GtestMplayHandle, &stru_PlayStatus));
	EXPECT_EQ(CH_MPLAY_STATUS_IDLE, stru_PlayStatus.enm_Status);
	
}

CASE_F(TEST_MPLAY_START3,
	"调用get media info，判断主要信息是否正确，url参数从env获取") {

	CH_MPLAY_MediaInfo_t stru_MediaInfo;
	
	memset(&stru_MediaInfo, 0, sizeof(stru_MediaInfo));
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_GetMediaInfo(gpuc_UrlName[guc_TestCount], &stru_MediaInfo));
}

CASE_F(TEST_MPLAY_START4,
	"调用start开始播放，url参数从env获取；假设其他模块均工作正常，此时应有图像声音出来") {
	
	char *pc_GetInputChar = NULL;

	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Start(gh_GtestMplayHandle, gpuc_UrlName[guc_TestCount]));

	PROMPT("\n请确认是否有图像声音输出\n");
	PROMPT("\n是 --> 输入\"y\",然后回车\n");
	PROMPT("\n否 --> 输入\"n\",然后回车\n");
	pc_GetInputChar = test_get_input();
	EXPECT_EQ(pc_GetInputChar[0], 'y') <<  "此时应有图像声音输出";
}

CASE_F(TEST_MPLAY_START5,
	"getstatus状态应为play") {

	CH_MPLAY_STATUS_t stru_PlayStatus;

	memset(&stru_PlayStatus, 0, sizeof(stru_PlayStatus));
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_GetStatus(gh_GtestMplayHandle, &stru_PlayStatus));
	EXPECT_EQ(CH_MPLAY_STATUS_PLAYING, stru_PlayStatus.enm_Status);	
	EXPECT_LE(stru_PlayStatus.i_PlaySecend,0)<<  "此时获取的播放时间应该大于0";	
	EXPECT_LE(stru_PlayStatus.i_TotalSecend,0)<<  "此时获取的播放总时长应该大于0";	
}

CASE_F(TEST_MPLAY_START6,
	"仔细观察画面是否流畅，声音是否饱满连续") {

	char *pc_GetInputChar = NULL;
	
	PROMPT("\n请仔细观察几秒,看画面是否流畅,声音是否饱满连续\n");
	PROMPT("\n是 --> 输入\"y\",然后回车\n");
	PROMPT("\n否 --> 输入\"n\",然后回车\n");
	pc_GetInputChar = test_get_input();
	
	EXPECT_EQ(pc_GetInputChar[0], 'y') <<  "播放应正常流畅";
}

CASE_F(TEST_MPLAY_START7,
	"调用CH_MPLAY_Ctrl，获取音轨信息和描述；判断是否正确") {

	S32 i_ParamEx = 0;
	S32 ri_Param=0;
	S32 i_AudioTrackNum = 0;

	i_AudioTrackNum = fw_getenv_i("TEST_MPLAY_AudioTrack_Num");						
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Ctrl(gh_GtestMplayHandle, CH_MPLAY_CMD_GET_TRACK_NUM,ri_Param,&gi_AudioTrackNum));	//获取音轨数量
	EXPECT_EQ(i_AudioTrackNum, gi_AudioTrackNum);		//获取的音轨数量应与实际的一致		
}

CASE_F(TEST_MPLAY_START8,
	"如果音轨数量大于1，测试能否正确切换音轨，get应与set的值一致；效果通过询问测试员") {

	S32 i_ParamEx = -1;
	S32 ri_Param=0;
	S32 set_audioindex=0;
	if(gi_AudioTrackNum > 1)
	{
		EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Ctrl(gh_GtestMplayHandle, CH_MPLAY_CMD_GET_TRACK_INDEX, ri_Param, &i_ParamEx)); //获取当前音轨ID
		EXPECT_GE(i_ParamEx,0)<<"获取当前音轨ID应大于等于0";
		set_audioindex=gi_AudioTrackNum-i_ParamEx-1;
		EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Ctrl(gh_GtestMplayHandle, CH_MPLAY_CMD_SET_TRACK_INDEX, set_audioindex, &i_ParamEx)); //切换音轨
		PROMPT("\n请确认音轨是否切换且视频与声音是否同步\n");
	}
}
CASE_F(TEST_MPLAY_START9,
	"测试能否正确获取当前的音轨ID号") {
	S32 i_ParamEx = 0;
	S32 ri_Param=0;
	if(gi_AudioTrackNum > 1)
	{
		EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Ctrl(gh_GtestMplayHandle, CH_MPLAY_CMD_GET_TRACK_DES, ri_Param, &i_ParamEx)); //获取特定音轨描述
		EXPECT_NE(CH_AUDIO_CODE_TYPE_MAX,i_ParamEx)<<"获取当前音轨编码格式应该有效";
	}
}
CASE_F(TEST_MPLAY_START10,
	"测试暂停播放，画面和声音应立刻停止") {
	
	char *pc_GetInputChar = NULL;
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Pause(gh_GtestMplayHandle));
	PROMPT("\n请确认当前是否处于暂停状态\n");
	PROMPT("\n是 --> 输入\"y\",然后回车\n");
	PROMPT("\n否 --> 输入\"n\",然后回车\n");
	pc_GetInputChar = test_get_input();
	EXPECT_EQ(pc_GetInputChar[0], 'y') <<  "当前应处于暂停状态";	
}

CASE_F(TEST_MPLAY_START11,
	"getstatus状态应全部正确") {
	CH_MPLAY_STATUS_t stru_PlayStatus;
	memset(&stru_PlayStatus, 0, sizeof(stru_PlayStatus));
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_GetStatus(gh_GtestMplayHandle, &stru_PlayStatus));
	EXPECT_EQ(CH_MPLAY_STATUS_PAUSE, stru_PlayStatus.enm_Status);
}

CASE_F(TEST_MPLAY_START12,
	"测试恢复播放，画面和声音应立刻恢复，音轨应与暂停前的设置一致") {

	S32 i_ParamEx = 0;
	S32 i_AudioTrackNum = 0;
	char *pc_GetInputChar = NULL;
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Ctrl (gh_GtestMplayHandle, CH_MPLAY_CMD_GET_TRACK_NUM, i_AudioTrackNum, &i_ParamEx));	//获取音轨数量
	EXPECT_EQ(i_AudioTrackNum, gi_AudioTrackNum);		//获取的音轨数量应与暂停前一致	
	
	PROMPT("\n请确认当前是否处于播放状态\n");
	PROMPT("\n是 --> 输入\"y\",然后回车\n");
	PROMPT("\n否 --> 输入\"n\",然后回车\n");
	pc_GetInputChar = test_get_input();
	EXPECT_EQ(pc_GetInputChar[0], 'y') <<  "当前应处于播放状态";	
}


//确认快进快退倍速状态
//renm_Type : 快进快退
//renm_Speed : 倍速
CH_MPLAY_RESULT_e TEST_MPLAY_Confirm_FfSpeed(CH_MPLAY_SPEED_PLAY_e renm_Type, CH_MPLAY_PLAY_SPEED_e renm_Speed)
{
	char *pc_GetInputChar = NULL;
	char c_PlayMode[4][10] = {"快进", "快退", "慢放", "慢退"};
	char (*pc_PlayMode)[10] = NULL;		//创建数组指针，指向c_PlayMode

	if(renm_Type == CH_MPLAY_PLAY_TYPE_MAX || renm_Speed == CH_MPLAY_PLAY_SPEED_MAX)
	{
		LOG("TEST_MPLAY_FF_FB INVALID_PARAM\n");	
		return CH_MPLAY_INVALID_PARAM;
	}
	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Speed(gh_GtestMplayHandle, renm_Type, renm_Speed));	

	if(renm_Type == CH_MPLAY_PLAY_FFW)
	{
		pc_PlayMode = &c_PlayMode[0];
	}
	else if(renm_Type == CH_MPLAY_PLAY_FBW)
	{
		pc_PlayMode = &c_PlayMode[1];
	}
	else if(renm_Type == CH_MPLAY_PLAY_SFW)
	{
		pc_PlayMode = &c_PlayMode[2];
	}
	else
	{
		pc_PlayMode = &c_PlayMode[3];
	}
	switch(renm_Speed)
	{
		case CH_MPLAY_PLAY_SPEED_1:		PROMPT("\n当前为1倍速%s播放,请确认播放效果是否符合\n", pc_PlayMode);break;
		case CH_MPLAY_PLAY_SPEED_2:		PROMPT("\n当前为2倍速%s播放,请确认播放效果是否符合\n", pc_PlayMode);break;
		case CH_MPLAY_PLAY_SPEED_4:		PROMPT("\n当前为4倍速%s播放,请确认播放效果是否符合\n", pc_PlayMode);break;
		case CH_MPLAY_PLAY_SPEED_8:		PROMPT("\n当前为8倍速%s播放,请确认播放效果是否符合\n", pc_PlayMode);break;
		case CH_MPLAY_PLAY_SPEED_16:	PROMPT("\n当前为16倍速%s播放,请确认播放效果是否符合\n", pc_PlayMode);break;
		case CH_MPLAY_PLAY_SPEED_32:	PROMPT("\n当前为32倍速%s播放,请确认播放效果是否符合\n", pc_PlayMode);break;
		case CH_MPLAY_PLAY_SPEED_64:	PROMPT("\n当前为64倍速%s播放,请确认播放效果是否符合\n", pc_PlayMode);break;
		case CH_MPLAY_PLAY_SPEED_128:	PROMPT("\n当前为128倍速%s播放,请确认播放效果是否符合\n", pc_PlayMode);break;
		default: PROMPT("\n请确认播放效果是否符合\n");break;
	}
	PROMPT("\n是 --> 输入\"y\",然后回车\n");
	PROMPT("\n否 --> 输入\"n\",然后回车\n");
	pc_GetInputChar = test_get_input();
	EXPECT_EQ(pc_GetInputChar[0], 'y') <<  "播放效果跟设置的不符合";	

	return CH_MPLAY_OK;	
	
}

//测试快进快退播放
//renm_Type: 快进快退
CH_MPLAY_RESULT_e TEST_MPLAY_FF_FB(CH_MPLAY_SPEED_PLAY_e renm_Type,CH_MPLAY_PLAY_SPEED_e renm_speed)
{
	CH_MPLAY_STATUS_t stru_PlayStatus;

	if(renm_Type == CH_MPLAY_PLAY_TYPE_MAX)
	{
		LOG("TEST_MPLAY_FF_FB INVALID_PARAM\n");	
		return CH_MPLAY_INVALID_PARAM;
	}
	memset(&stru_PlayStatus, 0, sizeof(stru_PlayStatus));
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_Confirm_FfSpeed(renm_Type, renm_speed));		// 1 倍速快进
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_GetStatus(gh_GtestMplayHandle, &stru_PlayStatus));
    EXPECT_EQ(renm_Type, stru_PlayStatus.enm_Status);
	return CH_MPLAY_OK;
	
}

CASE_F(TEST_MPLAY_SPEED1,
	"测试2倍数快进并通过getstatus查看是否正确") {
	LOG("\n现在测试2倍数快进,请仔细观察效果是否跟设置的一致\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_FFW,CH_MPLAY_PLAY_SPEED_2));
}



CASE_F(TEST_MPLAY_SPEED2,
	"测试4倍数快进并通过getstatus查看是否正确") {
	LOG("\n从2倍速恢复正常播放，查看音视频是否正常流畅播放\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n现在测试4倍数快进,请仔细观察效果是否跟设置的一致\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_FFW,CH_MPLAY_PLAY_SPEED_4));
}



CASE_F(TEST_MPLAY_SPEED3,
	"测试8倍数快进并通过getstatus查看是否正确") {
	LOG("\n从4倍速恢复正常播放，查看音视频是否正常流畅播放\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n现在测试8倍数快进,请仔细观察效果是否跟设置的一致\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_FFW,CH_MPLAY_PLAY_SPEED_8));
}

CASE_F(TEST_MPLAY_SPEED4,
	"测试16倍数快进并通过getstatus查看是否正确") {
	LOG("\n从8倍速恢复正常播放，查看音视频是否正常流畅播放\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n现在测试16倍数快进,请仔细观察效果是否跟设置的一致\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_FFW,CH_MPLAY_PLAY_SPEED_16));
}

CASE_F(TEST_MPLAY_SPEED5,
	"测试32倍数快进并通过getstatus查看是否正确") {
	LOG("\n从16倍速恢复正常播放，查看音视频是否正常流畅播放\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n现在测试32倍数快进,请仔细观察效果是否跟设置的一致\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_FFW,CH_MPLAY_PLAY_SPEED_32));
}


CASE_F(TEST_MPLAY_SPEED6,
	"测试64倍数快进并通过getstatus查看是否正确") {
	LOG("\n从32倍速恢复正常播放，查看音视频是否正常流畅播放\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n现在测试64倍数快进,请仔细观察效果是否跟设置的一致\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_FFW,CH_MPLAY_PLAY_SPEED_64));
}

CASE_F(TEST_MPLAY_SPEED7,
	"测试128倍数快进并通过getstatus查看是否正确") {
	LOG("\n从64倍速恢复正常播放，查看音视频是否正常流畅播放\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n现在测试128倍数快进,请仔细观察效果是否跟设置的一致\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_FFW,CH_MPLAY_PLAY_SPEED_128));
}

/*快退*/

CASE_F(TEST_MPLAY_SPEED8,
	"测试2倍数快退并通过getstatus查看是否正确") {
	LOG("\n从64倍速快进恢复正常播放，查看音视频是否正常流畅播放\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n现在测试2倍数快退,请仔细观察效果是否跟设置的一致\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_FBW,CH_MPLAY_PLAY_SPEED_2));
}



CASE_F(TEST_MPLAY_SPEED9,
	"测试4倍数快退并通过getstatus查看是否正确") {
	LOG("\n从2倍速快退恢复正常播放，查看音视频是否正常流畅播放\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n现在测试4倍数快退,请仔细观察效果是否跟设置的一致\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_FBW,CH_MPLAY_PLAY_SPEED_4));
}



CASE_F(TEST_MPLAY_SPEED10,
	"测试8倍数快退并通过getstatus查看是否正确") {
	LOG("\n从4倍速快退恢复正常播放，查看音视频是否正常流畅播放\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n现在测试8倍数快退,请仔细观察效果是否跟设置的一致\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_FBW,CH_MPLAY_PLAY_SPEED_8));
}



CASE_F(TEST_MPLAY_SPEED11,
	"测试16倍数快退并通过getstatus查看是否正确") {
	LOG("\n从8倍速快退恢复正常播放，查看音视频是否正常流畅播放\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n现在测试16倍数快退,请仔细观察效果是否跟设置的一致\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_FBW,CH_MPLAY_PLAY_SPEED_16));
}




CASE_F(TEST_MPLAY_SPEED12,
	"测试32倍数快退并通过getstatus查看是否正确") {
	LOG("\n从16倍速快退恢复正常播放，查看音视频是否正常流畅播放\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n现在测试32倍数快退,请仔细观察效果是否跟设置的一致\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_FBW,CH_MPLAY_PLAY_SPEED_32));
}

CASE_F(TEST_MPLAY_SPEED13,
	"测试64倍数快退并通过getstatus查看是否正确") {
	LOG("\n从32倍速快退恢复正常播放，查看音视频是否正常流畅播放\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n现在测试64倍数快退,请仔细观察效果是否跟设置的一致\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_FBW,CH_MPLAY_PLAY_SPEED_64));
}

CASE_F(TEST_MPLAY_SPEED14,
	"测试128倍数快退并通过getstatus查看是否正确") {
	LOG("\n从64倍速快退恢复正常播放，查看音视频是否正常流畅播放\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n现在测试128倍数快退,请仔细观察效果是否跟设置的一致\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_FBW,CH_MPLAY_PLAY_SPEED_128));
}


/*慢进*/
CASE_F(TEST_MPLAY_SPEED15,
	"测试1/2速率慢放并通过getstatus查看是否正确") {
	LOG("\n从128倍速快退恢复正常播放，查看音视频是否正常流畅播放\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n现在测试1/2速率慢放,请仔细观察效果是否跟设置的一致\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_SFW,CH_MPLAY_PLAY_SPEED_2));
}

CASE_F(TEST_MPLAY_SPEED16,
	"测试1/4速率慢放并通过getstatus查看是否正确") {
	LOG("\n从1/2速率恢复正常播放，查看音视频是否正常流畅播放\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n现在测试1/4速率慢放,请仔细观察效果是否跟设置的一致\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_SFW,CH_MPLAY_PLAY_SPEED_4));
}


CASE_F(TEST_MPLAY_SPEED17,
	"测试1/8速率慢放并通过getstatus查看是否正确") {
	LOG("\n从1/4速率恢复正常播放，查看音视频是否正常流畅播放\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n现在测试1/8速率慢放,请仔细观察效果是否跟设置的一致\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_SFW,CH_MPLAY_PLAY_SPEED_8));
}

CASE_F(TEST_MPLAY_SPEED18,
	"测试1/16速率慢放并通过getstatus查看是否正确") {
	LOG("\n从1/8速率恢复正常播放，查看音视频是否正常流畅播放\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n现在测试1/16速率慢放,请仔细观察效果是否跟设置的一致\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_SFW,CH_MPLAY_PLAY_SPEED_16));
}

CASE_F(TEST_MPLAY_SPEED19,
	"测试1/32速率慢放并通过getstatus查看是否正确") {
	LOG("\n从1/16速率恢复正常播放，查看音视频是否正常流畅播放\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n现在测试1/32速率慢放,请仔细观察效果是否跟设置的一致\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_SFW,CH_MPLAY_PLAY_SPEED_32));
}

CASE_F(TEST_MPLAY_SPEED20,
	"测试1/64速率慢放并通过getstatus查看是否正确") {
	LOG("\n从1/32速率恢复正常播放，查看音视频是否正常流畅播放\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n现在测试1/64速率慢放,请仔细观察效果是否跟设置的一致\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_SFW,CH_MPLAY_PLAY_SPEED_64));
}

CASE_F(TEST_MPLAY_SPEED21,
	"测试1/128速率慢放并通过getstatus查看是否正确") {
	LOG("\n从1/64速率恢复正常播放，查看音视频是否正常流畅播放\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n现在测试1/128速率慢放,请仔细观察效果是否跟设置的一致\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_SFW,CH_MPLAY_PLAY_SPEED_128));
}

CASE_F(TEST_MPLAY_SPEED22,
	"测试1/2速率慢退并通过getstatus查看是否正确") {
	LOG("\n从1/128速率慢放恢复正常播放，查看音视频是否正常流畅播放\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n现在测试1/2速率慢退,请仔细观察效果是否跟设置的一致\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_SBW,CH_MPLAY_PLAY_SPEED_2));
}



CASE_F(TEST_MPLAY_SPEED23,
	"测试1/4速率慢退并通过getstatus查看是否正确") {
	LOG("\n从1/2速率慢退恢复正常播放，查看音视频是否正常流畅播放\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n现在测试1/4速率慢退,请仔细观察效果是否跟设置的一致\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_SBW,CH_MPLAY_PLAY_SPEED_4));
}

CASE_F(TEST_MPLAY_SPEED24,
	"测试1/8速率慢退并通过getstatus查看是否正确") {
	LOG("\n从1/4速率慢退恢复正常播放，查看音视频是否正常流畅播放\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n现在测试1/8速率慢退,请仔细观察效果是否跟设置的一致\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_SBW,CH_MPLAY_PLAY_SPEED_8));
}

CASE_F(TEST_MPLAY_SPEED25,
	"测试1/16速率慢退并通过getstatus查看是否正确") {
	LOG("\n从1/8速率慢退恢复正常播放，查看音视频是否正常流畅播放\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n现在测试1/16速率慢退,请仔细观察效果是否跟设置的一致\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_SBW,CH_MPLAY_PLAY_SPEED_16));
}

CASE_F(TEST_MPLAY_SPEED26,
	"测试1/32速率慢退并通过getstatus查看是否正确") {
	LOG("\n从1/16速率慢退恢复正常播放，查看音视频是否正常流畅播放\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n现在测试1/32速率慢退,请仔细观察效果是否跟设置的一致\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_SBW,CH_MPLAY_PLAY_SPEED_32));
}

CASE_F(TEST_MPLAY_SPEED27,
	"测试1/64速率慢退并通过getstatus查看是否正确") {
	LOG("\n从1/32速率慢退恢复正常播放，查看音视频是否正常流畅播放\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n现在测试1/64速率慢退,请仔细观察效果是否跟设置的一致\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_SBW,CH_MPLAY_PLAY_SPEED_64));
}

CASE_F(TEST_MPLAY_SPEED28,
	"测试1/128速率慢退并通过getstatus查看是否正确") {
	LOG("\n从1/64速率慢退恢复正常播放，查看音视频是否正常流畅播放\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n现在测试1/128速率慢退,请仔细观察效果是否跟设置的一致\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_SBW,CH_MPLAY_PLAY_SPEED_128));
}



CASE_F(TEST_MPLAY_SPEED29,
	"从1/128速率慢退恢复正常播放，查看音视频是否正常流畅播放") {
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
}



CASE_F(TEST_MPLAY_SEEK1,
	"调用SEEK接口，SEEK到0秒测试能否正常起播") {
	char *pc_GetInputChar = NULL;
	S32 i_PosSecend = 0;
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Seek(gh_GtestMplayHandle,i_PosSecend));	
}

//考虑到测试视频长度和进行seek操作时播放的进度不一定
//为避免seek到不能播放的时间点,没有统一的seek 2分钟
CASE_F(TEST_MPLAY_SEEK2,
	"根据getstatus获得的进度信息，分别向前/向后seek 2分钟，再getstatus判断进度是否与seek后的进度相符")
	{
	CH_MPLAY_STATUS_t stru_PlayStatus;
	S32 i_PosSecend = 0;
	memset(&stru_PlayStatus, 0, sizeof(stru_PlayStatus));
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_GetStatus(gh_GtestMplayHandle, &stru_PlayStatus));
	i_PosSecend = stru_PlayStatus.i_TotalSecend/4;
	if(stru_PlayStatus.i_PlaySecend >= stru_PlayStatus.i_TotalSecend/2)		//当前播放时间处于码流的后半段
	{
		EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Seek(gh_GtestMplayHandle, stru_PlayStatus.i_PlaySecend - i_PosSecend));	//向前seek 总播放长度的1/4
		EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_GetStatus(gh_GtestMplayHandle, &stru_PlayStatus));
		EXPECT_EQ(stru_PlayStatus.i_PlaySecend - i_PosSecend, stru_PlayStatus.i_PlaySecend);			//当前播放进度与seek 后的进度应相符
		EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Seek(gh_GtestMplayHandle, stru_PlayStatus.i_PlaySecend + i_PosSecend));	//向后seek 总播放长度的1/4
		EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_GetStatus(gh_GtestMplayHandle, &stru_PlayStatus));
		EXPECT_EQ(stru_PlayStatus.i_PlaySecend + i_PosSecend, stru_PlayStatus.i_PlaySecend);			//当前播放进度与seek 后的进度应相符
	}
	else
	{
		EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Seek(gh_GtestMplayHandle, stru_PlayStatus.i_PlaySecend + i_PosSecend));	//向后seek 总播放长度的1/4
		EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_GetStatus(gh_GtestMplayHandle, &stru_PlayStatus));
		EXPECT_EQ(stru_PlayStatus.i_PlaySecend + i_PosSecend, stru_PlayStatus.i_PlaySecend);			//当前播放进度与seek 后的进度应相符
		EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Seek(gh_GtestMplayHandle, stru_PlayStatus.i_PlaySecend - i_PosSecend));	//向前seek 总播放长度的1/4
		EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_GetStatus(gh_GtestMplayHandle, &stru_PlayStatus));
		EXPECT_EQ(stru_PlayStatus.i_PlaySecend - i_PosSecend, stru_PlayStatus.i_PlaySecend);			//当前播放进度与seek 后的进度应相符
	}
}



CASE_F(TEST_MPLAY_STOP1,
	"调用stop停止播放，画面声音是否立刻停止，返回值是否正确") {

	char *pc_GetInputChar = NULL;
	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Stop(gh_GtestMplayHandle));
	PROMPT("\n请确认画面声音是否立刻停止\n");
	PROMPT("\n是 --> 输入\"y\",然后回车\n");
	PROMPT("\n否 --> 输入\"n\",然后回车\n");
	pc_GetInputChar = test_get_input();
	EXPECT_EQ(pc_GetInputChar[0], 'y') <<  "当前应处于停止播放状态";	
}

CASE_F(TEST_MPLAY_UPDATEURL,
	"从env分别获取rtsp、http、https、rtsps是否被定义,如有,重复上述测试过程;每种测试均要通过,如失败,给出失败的url定义;如果没有被定义,则不测试") {

	guc_TestCount++;
	for(;guc_TestCount<gus_UrlConut;)	//测试下一个有定义的URL
	{
		PROMPT("\n现在开始测试码流%s,请准备好测试环境后回车\n", gpuc_UrlName[guc_TestCount]);
		test_get_input();
		
		UnitTest *pCTest = testing::UnitTest::GetInstance();		
		TestCase* test_case = pCTest->current_test_case();		
		test_case->Run();
	}

	if(guc_TestCount==gus_UrlConut)		//有定义的URL全部测试完成
	{		
		gh_GtestMplayHandle = NULL;
		gus_UrlConut = 0;
		guc_TestCount = 0;									//清除测试次数记录
		memset(guc_UrlName, 0, sizeof(guc_UrlName));		//清空guc_UrlName		
	}

}



/*=========================================================================*/
/* 测试用列函数编写*/
/*=========================================================================*/


static CH_MPLAY_Handle gph_MplayHandle = NULL;
void* gph_Route=NULL;

static int mplay_start(int argc, CMD_PARAM argv[])
{
	int ret;

	RUN_CMD("route.create 0 0");
	RUN_CMD("vout.open 0 1 1 1 0 0 0 0\n");
#ifdef CFG_CHSSA_HD_SUPPORT	
	RUN_CMD("vout.open 1 1 1 1 0 0 0 0\n");
	RUN_CMD("vout.set_format 6");
#else
	RUN_CMD("vout.set_format 1");
#endif	

	if(gph_MplayHandle == NULL)
	{
		ret = CH_MPLAY_Open(gph_Route, &gph_MplayHandle);
		if(ret != CH_MPLAY_OK )
		{
			LOG("CH_MPLAY_Open return %d\n", ret);
		}
	}
	ret = CH_MPLAY_Start(gph_MplayHandle, (S8*)argv[1].get_str());
	if(ret != CH_MPLAY_OK )
	{
		LOG("CH_MPLAY_Start return %d\n", ret);
	}
	
	return 0;
}


static int mplay_stop(int argc, CMD_PARAM argv[])
{
	int ret;

	if(gph_MplayHandle)
	{
		ret = CH_MPLAY_Stop(gph_MplayHandle);
		if(ret != CH_MPLAY_OK )
		{
			LOG("CH_MPLAY_Stop return %d\n", ret);
		}
		ret = CH_MPLAY_Close(gph_MplayHandle);
		if(ret != CH_MPLAY_OK )
		{
			LOG("CH_MPLAY_Close return %d\n", ret);
		}
		gph_MplayHandle = NULL;
	}

	return 0;
}


/*初始化检测*/
static int mplay_init(int argc, CMD_PARAM argv[])
{
	int ret;
	ret = CH_MPLAY_Init();
	LOG("CH_MPLAY_Init return %d\n", ret);
	return ret;
}
/*退出mplay测试*/

static int mplay_term(int argc, CMD_PARAM argv[])
{
	int ret;

	ret = CH_MPLAY_Term();
	LOG("CH_MPLAY_Term return %d\n", ret);
	return ret;	
}

/**/






/*=========================================================================*/
/*注册命令编写*/
/*=========================================================================*/

REGISTER_CMD(mplay_start, "开始播放媒体文件", "url",  "/home/pianai.mp4");
REGISTER_CMD(mplay_stop, "停止播放媒体文件", "",  "");




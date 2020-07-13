/**
@file test_mplay.cpp
@def group mplay���Լ�

@author ���繫˾����������:	����ǿ
@date   2014��4��10��  
@version V4.0
@todo     ---ʱ��---����-------------�޸Ĳ��ּ�ԭ��----------
@todo     1 2014��4��10�� ����ǿ  ����

@todo     2 2018��7��24�� ���������Ӳ�������

@{
*/

/*���Լ����ƶ���*/
#ifdef CASE_NAME
#undef CASE_NAME
#endif
#define CASE_NAME TestMplay


#include "test_menu.h"
#include "test_modules.h"
#include "ch_stbtype.h"
#include "psi_parse.h"


//����������
#define MPLAY_ERRO_POSSECEND			(-1)
#define MPLAY_ERRO_PARAM				(-1)
#define EMPLAY_RRO_PARAMEX			(-1)
#define MPLAY_ERRO_STEPMS				(-1)
#define MPLAY_ERRO_STOPMS				(-1)

//����Ϸ�����
#define MPLAY_NORMAL_POSSECEND			(5)
#define MPLAY_NORMAL_PARAM				(5)
#define MPLAY_NORMAL_PARAMEX			(5)
#define MPLAY_NORMAL_STEPMS				(5)
#define MPLAY_NORMAL_STOPMS				(5)

#define MPLAY_URLSUM					(5)

//����ȫ�ֱ���
CH_MPLAY_Handle gh_GtestMplayHandle = NULL;  /*gh_GtestMplayHandle*/
S32 gi_AudioTrackNum = 0;
U16 guc_TestCount = 0;		//��¼�ڼ��β���
U16 gus_UrlConut = 0;		//��¼�ж����URL����
U8 guc_UrlType[][50] ={"TEST_MPLAY_URL_LOCAL", "TEST_MPLAY_URL_HTTP", "TEST_MPLAY_URL_HTTPS", "TEST_MPLAY_URL_RTSP", "TEST_MPLAY_URL_RTSPS"}; 
U8 (*gpuc_UrlType)[50] = guc_UrlType;
U8 guc_Protocol[][10] = {"", "http://", "https://", "rtsp://", "rtsps://"};

/*ÿ��ý���ʽ����һ�β��ԣ���Ҫ���Ե�ý���ʽΪ�����Ե������ڲ���MPLAY_URLSUM��ʾ*/
/*����ý�岥�Ÿ�ʽ5��*/
/*MKV,AVI ,WMV,MP4,FLV,ASF*/
/*����Э����� 4��*/
/*http,https,rtsp,rtsps*/
/*��ý���ʽ����2��*/
/*HLS,Smooth Stream,*/
/*MPLAY_URLSUM ����11��*/

U8 guc_UrlName[MPLAY_URLSUM][256] = {""};				//�����ж����url
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

	if(guc_TestCount == 0)		//��һ�ο�ʼ����ǰ�Ȼ�ȡ�ж����URL
	{
		U8 uc_Count = 0;

		for(uc_Count=0;uc_Count<MPLAY_URLSUM;uc_Count++)
		{
			U8 *puc_Url = NULL;
		
			puc_Url = (U8*)fw_getenv((char*)gpuc_UrlType[uc_Count]); 	//��ȡtest_config.cfg�е�URL
			
			if(puc_Url == NULL)			//URL δ����
			{
				continue;
			}
			else
			{
				strcat(gpuc_UrlName[gus_UrlConut], guc_Protocol[gus_UrlConut]);	//ͨ��fw_getenv�޷���ȡ"http://192.168.32.32/1.mp4"�е�"//",���"http://"��"192.168.32.32/1.mp4"������,����strcatƴ��
				strcat(gpuc_UrlName[gus_UrlConut], puc_Url); 				//����guc_UrlName
				gus_UrlConut++;
			}
		}
	    
		if(gus_UrlConut == 0) //һ��URL��û�ж���
		{
			PROMPT("\nδ���岥��·��,����test_config.cfg�ж���Ҫ���ԵĲ���·��\n");
			test_get_input();
			UnitTest *pCTest = testing::UnitTest::GetInstance();		
			TestCase* test_case = pCTest->current_test_case();		
			test_case->Run();
		}
		else
		{
			PROMPT("\n���ڿ�ʼ��������%s,��׼���ò��Ի�����س�\n", gpuc_UrlName[guc_TestCount]);
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
	"���Զ�γ�ʼ��ʱ�Ƿ񷵻�OK") {

	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Init());
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Init()) << "��γ�ʼ��Ӧ�÷���OK";	
}



CASE_F(TEST_MPLAY_INIT2,
	"����ģ���Ƿ�ɱ�����") {
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Term());
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Init());
}


CASE_F(TEST_MPLAY_INIT3,
	"����CH_MPLAY_GetVersion�Ƿ�ɹ�") {
	CHDRV_Version_t stru_Ver;
	
	memset(&stru_Ver, 0, sizeof(stru_Ver));
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_GetVersion(&stru_Ver));
	EXPECT_NE(0, stru_Ver);
	LOG("\nstru_Ver=%x\n", stru_Ver);
}

CASE_F(TEST_MPLAY_OPEN,
	"����ģ��Open��Close�ɷ�������Ӧ") {
	EXPECT_EQ(CH_MPLAY_OK,CH_MPLAY_Open(NULL, &gh_GtestMplayHandle));
	EXPECT_EQ(CH_MPLAY_OK,CH_MPLAY_Close(gh_GtestMplayHandle));
}

CASE_F(TEST_MPLAY_PARAMS1,
	"CH_MPLAY_Close�����������") {
	
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Close(NULL));	
}

CASE_F(TEST_MPLAY_PARAMS2,
	"CH_MPLAY_Ctrl�����������") {

	CH_MPLAY_CTRL_e enm_CtrlType = CH_MPLAY_CMD_GET_TRACK_NUM;
	S32 i_Param = -1;
	S32 i_ParamEx = 0;
		
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Ctrl(NULL, enm_CtrlType, i_Param, i_ParamEx));	
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Ctrl(gh_GtestMplayHandle, CH_MPLAY_CMD_MAX, i_Param, i_ParamEx));	
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Ctrl(gh_GtestMplayHandle, CH_MPLAY_CMD_SET_TRACK_INDEX, i_Param, i_ParamEx));	
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Ctrl(gh_GtestMplayHandle, CH_MPLAY_CMD_GET_TRACK_NUM, i_Param, i_ParamEx));	
}

CASE_F(TEST_MPLAY_PARAMS3,
	"CH_MPLAY_GetMediaInfo�����������") {

	U8 uc_url = 0;
	CH_MPLAY_MediaInfo_t stru_MediaInfo;
	memset(&stru_MediaInfo, 0, sizeof(stru_MediaInfo));
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_GetMediaInfo(NULL, &stru_MediaInfo));	
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_GetMediaInfo(&uc_url, NULL));	
}

CASE_F(TEST_MPLAY_PARAMS4,
	"CH_MPLAY_GetStatus�����������") {

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
		EXPECT_EQ(dead_test, 1) << "Ӧͨ����������";	
	}
}

CASE_F(TEST_MPLAY_PARAMS5,
	"CH_MPLAY_Open�����������") {

	U32 i_Route = 0;
	int dead_test = fw_getenv_i("DEAD_HALT_TEST");	
	
	if(dead_test == 1)	
	{		
		/*EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Open(NULL, &gh_GtestMplayHandle));	*/
		EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Open(&i_Route, NULL));	
	}	
	else	
	{		
		EXPECT_EQ(dead_test, 1) << "Ӧͨ����������";	
	}
}

CASE_F(TEST_MPLAY_PARAMS6,
	"CH_MPLAY_Pause�����������") {
	
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Pause(NULL));	
}

CASE_F(TEST_MPLAY_PARAMS7,
	"CH_MPLAY_Poll�����������") {
//	CH_MPLAY_PollΪstatic ����,�޷�����
//	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Poll(NULL));	
}

CASE_F(TEST_MPLAY_PARAMS8,
	"CH_MPLAY_Resume�����������") {
	
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Resume(NULL));	
}

CASE_F(TEST_MPLAY_PARAMS9,
	"CH_MPLAY_Seek�����������") {
	
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Seek(NULL, MPLAY_NORMAL_POSSECEND));	
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Seek(gh_GtestMplayHandle, MPLAY_ERRO_POSSECEND));	
}

CASE_F(TEST_MPLAY_PARAMS10,
	"CH_MPLAY_SetConfig�����������") {

	CH_MPLAY_CFG_TYPE_e enm_Type = CH_MPLAY_CFG_CB;
	
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_SetConfig(CH_MPLAY_CFG_MAX, MPLAY_NORMAL_PARAM, MPLAY_NORMAL_PARAMEX));	
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_SetConfig(enm_Type, MPLAY_ERRO_PARAM, MPLAY_NORMAL_PARAMEX));	
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_SetConfig(enm_Type, MPLAY_NORMAL_PARAM, EMPLAY_RRO_PARAMEX));	
}

CASE_F(TEST_MPLAY_PARAMS11,
	"CH_MPLAY_Speed�����������") {

	CH_MPLAY_SPEED_PLAY_e enm_Type = CH_MPLAY_PLAY_FFW;
	CH_MPLAY_PLAY_SPEED_e enm_Speed = CH_MPLAY_PLAY_SPEED_1;
	
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Speed(NULL, enm_Type, enm_Speed));	
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Speed(gh_GtestMplayHandle, CH_MPLAY_PLAY_TYPE_MAX, enm_Speed));	
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Speed(gh_GtestMplayHandle, enm_Type, CH_MPLAY_PLAY_SPEED_MAX));	
}

CASE_F(TEST_MPLAY_PARAMS12,
	"CH_MPLAY_Start�����������") {

	U8 uc_url = 0;
	int dead_test = fw_getenv_i("DEAD_HALT_TEST");	
	
	if(dead_test == 1)	
	{		
		EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Start(NULL, &uc_url));	
		EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Start(gh_GtestMplayHandle, NULL));	
	}	
	else	
	{		
		EXPECT_EQ(dead_test, 1) << "Ӧͨ����������";	
	}
}

CASE_F(TEST_MPLAY_PARAMS13,
	"CH_MPLAY_Step�����������") {

	CH_MPLAY_STEP_PLAY_e enm_Step = CH_MPLAY_PLAY_FSTEP;
	
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Step(NULL, enm_Step, MPLAY_NORMAL_STEPMS, MPLAY_NORMAL_STOPMS));	
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Step(gh_GtestMplayHandle, CH_MPLAY_PLAY_MAX, MPLAY_NORMAL_STEPMS, MPLAY_NORMAL_STOPMS));	
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Step(gh_GtestMplayHandle, enm_Step, MPLAY_ERRO_STEPMS, MPLAY_NORMAL_STOPMS));	
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Step(gh_GtestMplayHandle, enm_Step, MPLAY_NORMAL_STEPMS, MPLAY_ERRO_STOPMS));	
}


CASE_F(TEST_MPLAY_PARAMS14,
	"CH_MPLAY_Stop�����������") {
	
	EXPECT_EQ(CH_MPLAY_INVALID_PARAM, CH_MPLAY_Stop(NULL));	
}

CASE_F(TEST_MPLAY_START1,
	"����open����һ�������route������test routeģ���ȡ") {

	U32 i_Route = 0;

	gh_GtestMplayHandle = NULL;
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Open(&i_Route, &gh_GtestMplayHandle));
	EXPECT_EQ(NULL, gh_GtestMplayHandle) << "���Ӧ��Ϊ��\n";	
}

CASE_F(TEST_MPLAY_START2,
	"getstatus״̬ӦΪIDL") {

	CH_MPLAY_STATUS_t stru_PlayStatus;

	memset(&stru_PlayStatus, 0, sizeof(stru_PlayStatus));
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_GetStatus(gh_GtestMplayHandle, &stru_PlayStatus));
	EXPECT_EQ(CH_MPLAY_STATUS_IDLE, stru_PlayStatus.enm_Status);
	
}

CASE_F(TEST_MPLAY_START3,
	"����get media info���ж���Ҫ��Ϣ�Ƿ���ȷ��url������env��ȡ") {

	CH_MPLAY_MediaInfo_t stru_MediaInfo;
	
	memset(&stru_MediaInfo, 0, sizeof(stru_MediaInfo));
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_GetMediaInfo(gpuc_UrlName[guc_TestCount], &stru_MediaInfo));
}

CASE_F(TEST_MPLAY_START4,
	"����start��ʼ���ţ�url������env��ȡ����������ģ���������������ʱӦ��ͼ����������") {
	
	char *pc_GetInputChar = NULL;

	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Start(gh_GtestMplayHandle, gpuc_UrlName[guc_TestCount]));

	PROMPT("\n��ȷ���Ƿ���ͼ���������\n");
	PROMPT("\n�� --> ����\"y\",Ȼ��س�\n");
	PROMPT("\n�� --> ����\"n\",Ȼ��س�\n");
	pc_GetInputChar = test_get_input();
	EXPECT_EQ(pc_GetInputChar[0], 'y') <<  "��ʱӦ��ͼ���������";
}

CASE_F(TEST_MPLAY_START5,
	"getstatus״̬ӦΪplay") {

	CH_MPLAY_STATUS_t stru_PlayStatus;

	memset(&stru_PlayStatus, 0, sizeof(stru_PlayStatus));
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_GetStatus(gh_GtestMplayHandle, &stru_PlayStatus));
	EXPECT_EQ(CH_MPLAY_STATUS_PLAYING, stru_PlayStatus.enm_Status);	
	EXPECT_LE(stru_PlayStatus.i_PlaySecend,0)<<  "��ʱ��ȡ�Ĳ���ʱ��Ӧ�ô���0";	
	EXPECT_LE(stru_PlayStatus.i_TotalSecend,0)<<  "��ʱ��ȡ�Ĳ�����ʱ��Ӧ�ô���0";	
}

CASE_F(TEST_MPLAY_START6,
	"��ϸ�۲컭���Ƿ������������Ƿ�������") {

	char *pc_GetInputChar = NULL;
	
	PROMPT("\n����ϸ�۲켸��,�������Ƿ�����,�����Ƿ�������\n");
	PROMPT("\n�� --> ����\"y\",Ȼ��س�\n");
	PROMPT("\n�� --> ����\"n\",Ȼ��س�\n");
	pc_GetInputChar = test_get_input();
	
	EXPECT_EQ(pc_GetInputChar[0], 'y') <<  "����Ӧ��������";
}

CASE_F(TEST_MPLAY_START7,
	"����CH_MPLAY_Ctrl����ȡ������Ϣ���������ж��Ƿ���ȷ") {

	S32 i_ParamEx = 0;
	S32 ri_Param=0;
	S32 i_AudioTrackNum = 0;

	i_AudioTrackNum = fw_getenv_i("TEST_MPLAY_AudioTrack_Num");						
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Ctrl(gh_GtestMplayHandle, CH_MPLAY_CMD_GET_TRACK_NUM,ri_Param,&gi_AudioTrackNum));	//��ȡ��������
	EXPECT_EQ(i_AudioTrackNum, gi_AudioTrackNum);		//��ȡ����������Ӧ��ʵ�ʵ�һ��		
}

CASE_F(TEST_MPLAY_START8,
	"���������������1�������ܷ���ȷ�л����죬getӦ��set��ֵһ�£�Ч��ͨ��ѯ�ʲ���Ա") {

	S32 i_ParamEx = -1;
	S32 ri_Param=0;
	S32 set_audioindex=0;
	if(gi_AudioTrackNum > 1)
	{
		EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Ctrl(gh_GtestMplayHandle, CH_MPLAY_CMD_GET_TRACK_INDEX, ri_Param, &i_ParamEx)); //��ȡ��ǰ����ID
		EXPECT_GE(i_ParamEx,0)<<"��ȡ��ǰ����IDӦ���ڵ���0";
		set_audioindex=gi_AudioTrackNum-i_ParamEx-1;
		EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Ctrl(gh_GtestMplayHandle, CH_MPLAY_CMD_SET_TRACK_INDEX, set_audioindex, &i_ParamEx)); //�л�����
		PROMPT("\n��ȷ�������Ƿ��л�����Ƶ�������Ƿ�ͬ��\n");
	}
}
CASE_F(TEST_MPLAY_START9,
	"�����ܷ���ȷ��ȡ��ǰ������ID��") {
	S32 i_ParamEx = 0;
	S32 ri_Param=0;
	if(gi_AudioTrackNum > 1)
	{
		EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Ctrl(gh_GtestMplayHandle, CH_MPLAY_CMD_GET_TRACK_DES, ri_Param, &i_ParamEx)); //��ȡ�ض���������
		EXPECT_NE(CH_AUDIO_CODE_TYPE_MAX,i_ParamEx)<<"��ȡ��ǰ��������ʽӦ����Ч";
	}
}
CASE_F(TEST_MPLAY_START10,
	"������ͣ���ţ����������Ӧ����ֹͣ") {
	
	char *pc_GetInputChar = NULL;
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Pause(gh_GtestMplayHandle));
	PROMPT("\n��ȷ�ϵ�ǰ�Ƿ�����ͣ״̬\n");
	PROMPT("\n�� --> ����\"y\",Ȼ��س�\n");
	PROMPT("\n�� --> ����\"n\",Ȼ��س�\n");
	pc_GetInputChar = test_get_input();
	EXPECT_EQ(pc_GetInputChar[0], 'y') <<  "��ǰӦ������ͣ״̬";	
}

CASE_F(TEST_MPLAY_START11,
	"getstatus״̬Ӧȫ����ȷ") {
	CH_MPLAY_STATUS_t stru_PlayStatus;
	memset(&stru_PlayStatus, 0, sizeof(stru_PlayStatus));
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_GetStatus(gh_GtestMplayHandle, &stru_PlayStatus));
	EXPECT_EQ(CH_MPLAY_STATUS_PAUSE, stru_PlayStatus.enm_Status);
}

CASE_F(TEST_MPLAY_START12,
	"���Իָ����ţ����������Ӧ���ָ̻�������Ӧ����ͣǰ������һ��") {

	S32 i_ParamEx = 0;
	S32 i_AudioTrackNum = 0;
	char *pc_GetInputChar = NULL;
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Ctrl (gh_GtestMplayHandle, CH_MPLAY_CMD_GET_TRACK_NUM, i_AudioTrackNum, &i_ParamEx));	//��ȡ��������
	EXPECT_EQ(i_AudioTrackNum, gi_AudioTrackNum);		//��ȡ����������Ӧ����ͣǰһ��	
	
	PROMPT("\n��ȷ�ϵ�ǰ�Ƿ��ڲ���״̬\n");
	PROMPT("\n�� --> ����\"y\",Ȼ��س�\n");
	PROMPT("\n�� --> ����\"n\",Ȼ��س�\n");
	pc_GetInputChar = test_get_input();
	EXPECT_EQ(pc_GetInputChar[0], 'y') <<  "��ǰӦ���ڲ���״̬";	
}


//ȷ�Ͽ�����˱���״̬
//renm_Type : �������
//renm_Speed : ����
CH_MPLAY_RESULT_e TEST_MPLAY_Confirm_FfSpeed(CH_MPLAY_SPEED_PLAY_e renm_Type, CH_MPLAY_PLAY_SPEED_e renm_Speed)
{
	char *pc_GetInputChar = NULL;
	char c_PlayMode[4][10] = {"���", "����", "����", "����"};
	char (*pc_PlayMode)[10] = NULL;		//��������ָ�룬ָ��c_PlayMode

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
		case CH_MPLAY_PLAY_SPEED_1:		PROMPT("\n��ǰΪ1����%s����,��ȷ�ϲ���Ч���Ƿ����\n", pc_PlayMode);break;
		case CH_MPLAY_PLAY_SPEED_2:		PROMPT("\n��ǰΪ2����%s����,��ȷ�ϲ���Ч���Ƿ����\n", pc_PlayMode);break;
		case CH_MPLAY_PLAY_SPEED_4:		PROMPT("\n��ǰΪ4����%s����,��ȷ�ϲ���Ч���Ƿ����\n", pc_PlayMode);break;
		case CH_MPLAY_PLAY_SPEED_8:		PROMPT("\n��ǰΪ8����%s����,��ȷ�ϲ���Ч���Ƿ����\n", pc_PlayMode);break;
		case CH_MPLAY_PLAY_SPEED_16:	PROMPT("\n��ǰΪ16����%s����,��ȷ�ϲ���Ч���Ƿ����\n", pc_PlayMode);break;
		case CH_MPLAY_PLAY_SPEED_32:	PROMPT("\n��ǰΪ32����%s����,��ȷ�ϲ���Ч���Ƿ����\n", pc_PlayMode);break;
		case CH_MPLAY_PLAY_SPEED_64:	PROMPT("\n��ǰΪ64����%s����,��ȷ�ϲ���Ч���Ƿ����\n", pc_PlayMode);break;
		case CH_MPLAY_PLAY_SPEED_128:	PROMPT("\n��ǰΪ128����%s����,��ȷ�ϲ���Ч���Ƿ����\n", pc_PlayMode);break;
		default: PROMPT("\n��ȷ�ϲ���Ч���Ƿ����\n");break;
	}
	PROMPT("\n�� --> ����\"y\",Ȼ��س�\n");
	PROMPT("\n�� --> ����\"n\",Ȼ��س�\n");
	pc_GetInputChar = test_get_input();
	EXPECT_EQ(pc_GetInputChar[0], 'y') <<  "����Ч�������õĲ�����";	

	return CH_MPLAY_OK;	
	
}

//���Կ�����˲���
//renm_Type: �������
CH_MPLAY_RESULT_e TEST_MPLAY_FF_FB(CH_MPLAY_SPEED_PLAY_e renm_Type,CH_MPLAY_PLAY_SPEED_e renm_speed)
{
	CH_MPLAY_STATUS_t stru_PlayStatus;

	if(renm_Type == CH_MPLAY_PLAY_TYPE_MAX)
	{
		LOG("TEST_MPLAY_FF_FB INVALID_PARAM\n");	
		return CH_MPLAY_INVALID_PARAM;
	}
	memset(&stru_PlayStatus, 0, sizeof(stru_PlayStatus));
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_Confirm_FfSpeed(renm_Type, renm_speed));		// 1 ���ٿ��
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_GetStatus(gh_GtestMplayHandle, &stru_PlayStatus));
    EXPECT_EQ(renm_Type, stru_PlayStatus.enm_Status);
	return CH_MPLAY_OK;
	
}

CASE_F(TEST_MPLAY_SPEED1,
	"����2���������ͨ��getstatus�鿴�Ƿ���ȷ") {
	LOG("\n���ڲ���2�������,����ϸ�۲�Ч���Ƿ�����õ�һ��\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_FFW,CH_MPLAY_PLAY_SPEED_2));
}



CASE_F(TEST_MPLAY_SPEED2,
	"����4���������ͨ��getstatus�鿴�Ƿ���ȷ") {
	LOG("\n��2���ٻָ��������ţ��鿴����Ƶ�Ƿ�������������\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n���ڲ���4�������,����ϸ�۲�Ч���Ƿ�����õ�һ��\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_FFW,CH_MPLAY_PLAY_SPEED_4));
}



CASE_F(TEST_MPLAY_SPEED3,
	"����8���������ͨ��getstatus�鿴�Ƿ���ȷ") {
	LOG("\n��4���ٻָ��������ţ��鿴����Ƶ�Ƿ�������������\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n���ڲ���8�������,����ϸ�۲�Ч���Ƿ�����õ�һ��\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_FFW,CH_MPLAY_PLAY_SPEED_8));
}

CASE_F(TEST_MPLAY_SPEED4,
	"����16���������ͨ��getstatus�鿴�Ƿ���ȷ") {
	LOG("\n��8���ٻָ��������ţ��鿴����Ƶ�Ƿ�������������\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n���ڲ���16�������,����ϸ�۲�Ч���Ƿ�����õ�һ��\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_FFW,CH_MPLAY_PLAY_SPEED_16));
}

CASE_F(TEST_MPLAY_SPEED5,
	"����32���������ͨ��getstatus�鿴�Ƿ���ȷ") {
	LOG("\n��16���ٻָ��������ţ��鿴����Ƶ�Ƿ�������������\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n���ڲ���32�������,����ϸ�۲�Ч���Ƿ�����õ�һ��\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_FFW,CH_MPLAY_PLAY_SPEED_32));
}


CASE_F(TEST_MPLAY_SPEED6,
	"����64���������ͨ��getstatus�鿴�Ƿ���ȷ") {
	LOG("\n��32���ٻָ��������ţ��鿴����Ƶ�Ƿ�������������\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n���ڲ���64�������,����ϸ�۲�Ч���Ƿ�����õ�һ��\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_FFW,CH_MPLAY_PLAY_SPEED_64));
}

CASE_F(TEST_MPLAY_SPEED7,
	"����128���������ͨ��getstatus�鿴�Ƿ���ȷ") {
	LOG("\n��64���ٻָ��������ţ��鿴����Ƶ�Ƿ�������������\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n���ڲ���128�������,����ϸ�۲�Ч���Ƿ�����õ�һ��\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_FFW,CH_MPLAY_PLAY_SPEED_128));
}

/*����*/

CASE_F(TEST_MPLAY_SPEED8,
	"����2�������˲�ͨ��getstatus�鿴�Ƿ���ȷ") {
	LOG("\n��64���ٿ���ָ��������ţ��鿴����Ƶ�Ƿ�������������\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n���ڲ���2��������,����ϸ�۲�Ч���Ƿ�����õ�һ��\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_FBW,CH_MPLAY_PLAY_SPEED_2));
}



CASE_F(TEST_MPLAY_SPEED9,
	"����4�������˲�ͨ��getstatus�鿴�Ƿ���ȷ") {
	LOG("\n��2���ٿ��˻ָ��������ţ��鿴����Ƶ�Ƿ�������������\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n���ڲ���4��������,����ϸ�۲�Ч���Ƿ�����õ�һ��\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_FBW,CH_MPLAY_PLAY_SPEED_4));
}



CASE_F(TEST_MPLAY_SPEED10,
	"����8�������˲�ͨ��getstatus�鿴�Ƿ���ȷ") {
	LOG("\n��4���ٿ��˻ָ��������ţ��鿴����Ƶ�Ƿ�������������\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n���ڲ���8��������,����ϸ�۲�Ч���Ƿ�����õ�һ��\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_FBW,CH_MPLAY_PLAY_SPEED_8));
}



CASE_F(TEST_MPLAY_SPEED11,
	"����16�������˲�ͨ��getstatus�鿴�Ƿ���ȷ") {
	LOG("\n��8���ٿ��˻ָ��������ţ��鿴����Ƶ�Ƿ�������������\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n���ڲ���16��������,����ϸ�۲�Ч���Ƿ�����õ�һ��\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_FBW,CH_MPLAY_PLAY_SPEED_16));
}




CASE_F(TEST_MPLAY_SPEED12,
	"����32�������˲�ͨ��getstatus�鿴�Ƿ���ȷ") {
	LOG("\n��16���ٿ��˻ָ��������ţ��鿴����Ƶ�Ƿ�������������\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n���ڲ���32��������,����ϸ�۲�Ч���Ƿ�����õ�һ��\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_FBW,CH_MPLAY_PLAY_SPEED_32));
}

CASE_F(TEST_MPLAY_SPEED13,
	"����64�������˲�ͨ��getstatus�鿴�Ƿ���ȷ") {
	LOG("\n��32���ٿ��˻ָ��������ţ��鿴����Ƶ�Ƿ�������������\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n���ڲ���64��������,����ϸ�۲�Ч���Ƿ�����õ�һ��\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_FBW,CH_MPLAY_PLAY_SPEED_64));
}

CASE_F(TEST_MPLAY_SPEED14,
	"����128�������˲�ͨ��getstatus�鿴�Ƿ���ȷ") {
	LOG("\n��64���ٿ��˻ָ��������ţ��鿴����Ƶ�Ƿ�������������\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n���ڲ���128��������,����ϸ�۲�Ч���Ƿ�����õ�һ��\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_FBW,CH_MPLAY_PLAY_SPEED_128));
}


/*����*/
CASE_F(TEST_MPLAY_SPEED15,
	"����1/2�������Ų�ͨ��getstatus�鿴�Ƿ���ȷ") {
	LOG("\n��128���ٿ��˻ָ��������ţ��鿴����Ƶ�Ƿ�������������\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n���ڲ���1/2��������,����ϸ�۲�Ч���Ƿ�����õ�һ��\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_SFW,CH_MPLAY_PLAY_SPEED_2));
}

CASE_F(TEST_MPLAY_SPEED16,
	"����1/4�������Ų�ͨ��getstatus�鿴�Ƿ���ȷ") {
	LOG("\n��1/2���ʻָ��������ţ��鿴����Ƶ�Ƿ�������������\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n���ڲ���1/4��������,����ϸ�۲�Ч���Ƿ�����õ�һ��\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_SFW,CH_MPLAY_PLAY_SPEED_4));
}


CASE_F(TEST_MPLAY_SPEED17,
	"����1/8�������Ų�ͨ��getstatus�鿴�Ƿ���ȷ") {
	LOG("\n��1/4���ʻָ��������ţ��鿴����Ƶ�Ƿ�������������\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n���ڲ���1/8��������,����ϸ�۲�Ч���Ƿ�����õ�һ��\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_SFW,CH_MPLAY_PLAY_SPEED_8));
}

CASE_F(TEST_MPLAY_SPEED18,
	"����1/16�������Ų�ͨ��getstatus�鿴�Ƿ���ȷ") {
	LOG("\n��1/8���ʻָ��������ţ��鿴����Ƶ�Ƿ�������������\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n���ڲ���1/16��������,����ϸ�۲�Ч���Ƿ�����õ�һ��\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_SFW,CH_MPLAY_PLAY_SPEED_16));
}

CASE_F(TEST_MPLAY_SPEED19,
	"����1/32�������Ų�ͨ��getstatus�鿴�Ƿ���ȷ") {
	LOG("\n��1/16���ʻָ��������ţ��鿴����Ƶ�Ƿ�������������\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n���ڲ���1/32��������,����ϸ�۲�Ч���Ƿ�����õ�һ��\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_SFW,CH_MPLAY_PLAY_SPEED_32));
}

CASE_F(TEST_MPLAY_SPEED20,
	"����1/64�������Ų�ͨ��getstatus�鿴�Ƿ���ȷ") {
	LOG("\n��1/32���ʻָ��������ţ��鿴����Ƶ�Ƿ�������������\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n���ڲ���1/64��������,����ϸ�۲�Ч���Ƿ�����õ�һ��\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_SFW,CH_MPLAY_PLAY_SPEED_64));
}

CASE_F(TEST_MPLAY_SPEED21,
	"����1/128�������Ų�ͨ��getstatus�鿴�Ƿ���ȷ") {
	LOG("\n��1/64���ʻָ��������ţ��鿴����Ƶ�Ƿ�������������\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n���ڲ���1/128��������,����ϸ�۲�Ч���Ƿ�����õ�һ��\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_SFW,CH_MPLAY_PLAY_SPEED_128));
}

CASE_F(TEST_MPLAY_SPEED22,
	"����1/2�������˲�ͨ��getstatus�鿴�Ƿ���ȷ") {
	LOG("\n��1/128�������Żָ��������ţ��鿴����Ƶ�Ƿ�������������\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n���ڲ���1/2��������,����ϸ�۲�Ч���Ƿ�����õ�һ��\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_SBW,CH_MPLAY_PLAY_SPEED_2));
}



CASE_F(TEST_MPLAY_SPEED23,
	"����1/4�������˲�ͨ��getstatus�鿴�Ƿ���ȷ") {
	LOG("\n��1/2�������˻ָ��������ţ��鿴����Ƶ�Ƿ�������������\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n���ڲ���1/4��������,����ϸ�۲�Ч���Ƿ�����õ�һ��\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_SBW,CH_MPLAY_PLAY_SPEED_4));
}

CASE_F(TEST_MPLAY_SPEED24,
	"����1/8�������˲�ͨ��getstatus�鿴�Ƿ���ȷ") {
	LOG("\n��1/4�������˻ָ��������ţ��鿴����Ƶ�Ƿ�������������\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n���ڲ���1/8��������,����ϸ�۲�Ч���Ƿ�����õ�һ��\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_SBW,CH_MPLAY_PLAY_SPEED_8));
}

CASE_F(TEST_MPLAY_SPEED25,
	"����1/16�������˲�ͨ��getstatus�鿴�Ƿ���ȷ") {
	LOG("\n��1/8�������˻ָ��������ţ��鿴����Ƶ�Ƿ�������������\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n���ڲ���1/16��������,����ϸ�۲�Ч���Ƿ�����õ�һ��\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_SBW,CH_MPLAY_PLAY_SPEED_16));
}

CASE_F(TEST_MPLAY_SPEED26,
	"����1/32�������˲�ͨ��getstatus�鿴�Ƿ���ȷ") {
	LOG("\n��1/16�������˻ָ��������ţ��鿴����Ƶ�Ƿ�������������\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n���ڲ���1/32��������,����ϸ�۲�Ч���Ƿ�����õ�һ��\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_SBW,CH_MPLAY_PLAY_SPEED_32));
}

CASE_F(TEST_MPLAY_SPEED27,
	"����1/64�������˲�ͨ��getstatus�鿴�Ƿ���ȷ") {
	LOG("\n��1/32�������˻ָ��������ţ��鿴����Ƶ�Ƿ�������������\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n���ڲ���1/64��������,����ϸ�۲�Ч���Ƿ�����õ�һ��\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_SBW,CH_MPLAY_PLAY_SPEED_64));
}

CASE_F(TEST_MPLAY_SPEED28,
	"����1/128�������˲�ͨ��getstatus�鿴�Ƿ���ȷ") {
	LOG("\n��1/64�������˻ָ��������ţ��鿴����Ƶ�Ƿ�������������\n");	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
	LOG("\n���ڲ���1/128��������,����ϸ�۲�Ч���Ƿ�����õ�һ��\n");	
	EXPECT_EQ(CH_MPLAY_OK, TEST_MPLAY_FF_FB(CH_MPLAY_PLAY_SBW,CH_MPLAY_PLAY_SPEED_128));
}



CASE_F(TEST_MPLAY_SPEED29,
	"��1/128�������˻ָ��������ţ��鿴����Ƶ�Ƿ�������������") {
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Resume(gh_GtestMplayHandle));
}



CASE_F(TEST_MPLAY_SEEK1,
	"����SEEK�ӿڣ�SEEK��0������ܷ�������") {
	char *pc_GetInputChar = NULL;
	S32 i_PosSecend = 0;
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Seek(gh_GtestMplayHandle,i_PosSecend));	
}

//���ǵ�������Ƶ���Ⱥͽ���seek����ʱ���ŵĽ��Ȳ�һ��
//Ϊ����seek�����ܲ��ŵ�ʱ���,û��ͳһ��seek 2����
CASE_F(TEST_MPLAY_SEEK2,
	"����getstatus��õĽ�����Ϣ���ֱ���ǰ/���seek 2���ӣ���getstatus�жϽ����Ƿ���seek��Ľ������")
	{
	CH_MPLAY_STATUS_t stru_PlayStatus;
	S32 i_PosSecend = 0;
	memset(&stru_PlayStatus, 0, sizeof(stru_PlayStatus));
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_GetStatus(gh_GtestMplayHandle, &stru_PlayStatus));
	i_PosSecend = stru_PlayStatus.i_TotalSecend/4;
	if(stru_PlayStatus.i_PlaySecend >= stru_PlayStatus.i_TotalSecend/2)		//��ǰ����ʱ�䴦�������ĺ���
	{
		EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Seek(gh_GtestMplayHandle, stru_PlayStatus.i_PlaySecend - i_PosSecend));	//��ǰseek �ܲ��ų��ȵ�1/4
		EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_GetStatus(gh_GtestMplayHandle, &stru_PlayStatus));
		EXPECT_EQ(stru_PlayStatus.i_PlaySecend - i_PosSecend, stru_PlayStatus.i_PlaySecend);			//��ǰ���Ž�����seek ��Ľ���Ӧ���
		EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Seek(gh_GtestMplayHandle, stru_PlayStatus.i_PlaySecend + i_PosSecend));	//���seek �ܲ��ų��ȵ�1/4
		EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_GetStatus(gh_GtestMplayHandle, &stru_PlayStatus));
		EXPECT_EQ(stru_PlayStatus.i_PlaySecend + i_PosSecend, stru_PlayStatus.i_PlaySecend);			//��ǰ���Ž�����seek ��Ľ���Ӧ���
	}
	else
	{
		EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Seek(gh_GtestMplayHandle, stru_PlayStatus.i_PlaySecend + i_PosSecend));	//���seek �ܲ��ų��ȵ�1/4
		EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_GetStatus(gh_GtestMplayHandle, &stru_PlayStatus));
		EXPECT_EQ(stru_PlayStatus.i_PlaySecend + i_PosSecend, stru_PlayStatus.i_PlaySecend);			//��ǰ���Ž�����seek ��Ľ���Ӧ���
		EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Seek(gh_GtestMplayHandle, stru_PlayStatus.i_PlaySecend - i_PosSecend));	//��ǰseek �ܲ��ų��ȵ�1/4
		EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_GetStatus(gh_GtestMplayHandle, &stru_PlayStatus));
		EXPECT_EQ(stru_PlayStatus.i_PlaySecend - i_PosSecend, stru_PlayStatus.i_PlaySecend);			//��ǰ���Ž�����seek ��Ľ���Ӧ���
	}
}



CASE_F(TEST_MPLAY_STOP1,
	"����stopֹͣ���ţ����������Ƿ�����ֹͣ������ֵ�Ƿ���ȷ") {

	char *pc_GetInputChar = NULL;
	
	EXPECT_EQ(CH_MPLAY_OK, CH_MPLAY_Stop(gh_GtestMplayHandle));
	PROMPT("\n��ȷ�ϻ��������Ƿ�����ֹͣ\n");
	PROMPT("\n�� --> ����\"y\",Ȼ��س�\n");
	PROMPT("\n�� --> ����\"n\",Ȼ��س�\n");
	pc_GetInputChar = test_get_input();
	EXPECT_EQ(pc_GetInputChar[0], 'y') <<  "��ǰӦ����ֹͣ����״̬";	
}

CASE_F(TEST_MPLAY_UPDATEURL,
	"��env�ֱ��ȡrtsp��http��https��rtsps�Ƿ񱻶���,����,�ظ��������Թ���;ÿ�ֲ��Ծ�Ҫͨ��,��ʧ��,����ʧ�ܵ�url����;���û�б�����,�򲻲���") {

	guc_TestCount++;
	for(;guc_TestCount<gus_UrlConut;)	//������һ���ж����URL
	{
		PROMPT("\n���ڿ�ʼ��������%s,��׼���ò��Ի�����س�\n", gpuc_UrlName[guc_TestCount]);
		test_get_input();
		
		UnitTest *pCTest = testing::UnitTest::GetInstance();		
		TestCase* test_case = pCTest->current_test_case();		
		test_case->Run();
	}

	if(guc_TestCount==gus_UrlConut)		//�ж����URLȫ���������
	{		
		gh_GtestMplayHandle = NULL;
		gus_UrlConut = 0;
		guc_TestCount = 0;									//������Դ�����¼
		memset(guc_UrlName, 0, sizeof(guc_UrlName));		//���guc_UrlName		
	}

}



/*=========================================================================*/
/* �������к�����д*/
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


/*��ʼ�����*/
static int mplay_init(int argc, CMD_PARAM argv[])
{
	int ret;
	ret = CH_MPLAY_Init();
	LOG("CH_MPLAY_Init return %d\n", ret);
	return ret;
}
/*�˳�mplay����*/

static int mplay_term(int argc, CMD_PARAM argv[])
{
	int ret;

	ret = CH_MPLAY_Term();
	LOG("CH_MPLAY_Term return %d\n", ret);
	return ret;	
}

/**/






/*=========================================================================*/
/*ע�������д*/
/*=========================================================================*/

REGISTER_CMD(mplay_start, "��ʼ����ý���ļ�", "url",  "/home/pianai.mp4");
REGISTER_CMD(mplay_stop, "ֹͣ����ý���ļ�", "",  "");




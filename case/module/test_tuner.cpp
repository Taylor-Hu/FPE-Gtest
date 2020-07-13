/**
@file test_tuner.cpp
@def group tuner���Լ�

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
#define CASE_NAME TestTuner


#include "test_menu.h"
#include "test_modules.h"

static U32 LockCount0=0,LockCount1=0,LockCount2=0;
static U32 gui_ReceivedPatSections =0 ;
static int TpfindStatus =-1,schedule= -1;
static CHDRV_SECTION_ChannelHandle		gh_Channel = NULL;
static CHDRV_SECTION_FilterHandle		gh_Filter = NULL;
static U32 gui_TunerIndex = 0;

void init_variable(void)
 {
	char *file_buf = NULL;
	const char *sub_str = "TEST_TUNER_FUNC_18";
	char *p_str = NULL;
	int ret = 0;

	gui_TunerIndex = 0;
	ret = fsys_read_file("C:/test_proc.tmp", file_buf);
	if(ret <= 0)
	{
	   CH_RTOS_FreeMemory(gh_APP_HEAP, file_buf);
	   file_buf = NULL;
	   return;
	}
	p_str = file_buf;
	do{
		p_str = strstr(p_str, sub_str);
		if(!p_str)
		{
			LOG("not found!  dmxid=%d\n",gui_TunerIndex);
			sub_str = NULL;
			return;
		}

	   p_str = strstr(p_str, "\n");
	   if(p_str)
	   {
		   gui_TunerIndex++;
		   LOG("found!  dmxid=%d\n",gui_TunerIndex);
		   p_str++;
	   }
	   else
	   {
		   CH_RTOS_FreeMemory(gh_APP_HEAP, file_buf);
		   file_buf = NULL;
		   return;
	   }
	   
	}while(p_str);

 }


void test_tuner(void)
{
#if ENABLE_TESTCASE
	S32 i_DemuxNums = 0;

	i_DemuxNums = fw_getenv_i("TUNER_NUMS");
	if(gui_TunerIndex + 1 < i_DemuxNums)
	{
		gui_TunerIndex++;				//�豸�ż�1
	}
	else
	{
		gui_TunerIndex = 0;
	}
	
	if(gui_TunerIndex > 0)
	{
		PROMPT("TUNER����Ϊ%d,���ڿ�ʼ���Ե�%d��TUNER,��س������²���", i_DemuxNums, gui_TunerIndex + 1);
		test_get_input();
		
		UnitTest *pCTest = testing::UnitTest::GetInstance();
		const TestCase* test_case = pCTest->current_test_case();
		test_case->ClearResult();//0-----------clear result.
		test_case->Run();
	}
#endif /* ENABLE_TESTCASE */
}

void CASE_NAME::SetUpTestCase() {
//excute before each test suit
//std::cout << "init SetUp rtos TestCase " << std::endl;
	init_variable();
	RUN_CMD("route.create %d 0", get_demux_id());
}
void CASE_NAME::TearDownTestCase() {
//excute after each test suit
//std::cout << "deinit tuner rtos TestCase " << std::endl;
	test_tuner();
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
char *parse_key(char* line, char* key, char* value, int value_len)
{
	if(!(line && key && value) || value <= 0)
	{
		return NULL;
	}
	char *p = line;
	int i;
	
	value[0] = 0;
	do
	{
		p = strstr(p, key);
		if(p && *(p + strlen(key)) == '=')
		{
			p += strlen(key) + 1;
			for(i= 0; p[i] && i < value_len - 1; i++)
			{
				if((p[i] >= 'a' && p[i] <= 'z')
					|| (p[i] >= 'A' && p[i] <= 'Z')
					|| (p[i] >= '0' && p[i] <= '9')
					|| p[i] == '_')
				{
					value[i] = p[i];
				}
				else
				{
					break;
				}
			}
			value[i] = 0;
			LOG("[%s]=%s\n", key, value);
			return value;
		}
		if(p)p++;
	}while(p);

	LOG("ERROR: can not get keyword [%s] in %s\n", key, line);
	return NULL;
}
static void TEST_TUNER_MonitorCallback1(S32 ri_Device, 
														CHDRV_TUNER_EVENT_TYPE_e renm_EventType,
														void *rpv_ReturnData)
{
	if(renm_EventType == CHDRV_TUNER_LOCK_CHANGE)
	{
	  LockCount0++;
	}
}
static void TEST_TUNER_MonitorCallback2(S32 ri_Device, 
														CHDRV_TUNER_EVENT_TYPE_e renm_EventType,
														void *rpv_ReturnData)
{
	if(renm_EventType == CHDRV_TUNER_LOCK_CHANGE)
	{
	  LockCount1++;
	}
}
static void TEST_TUNER_MonitorCallback3(S32 ri_Device, 
														CHDRV_TUNER_EVENT_TYPE_e renm_EventType,
														void *rpv_ReturnData)
{
	if(renm_EventType == CHDRV_TUNER_LOCK_CHANGE)
	{
	  LockCount2++;
	}
}
CHDRV_TUNER_BLIND_SEARCH_COMMAND_e TEST_TUNER_TPFindCallback( 
									CHDRV_TUNER_BLIND_SEARCH_STATUS_e  renm_SearchStatus,
									CHDRV_TUNER_BlindSearchResult_t *rpstru_SearchRerultData)
{
	TpfindStatus = renm_SearchStatus;
	schedule = rpstru_SearchRerultData->c_Progress;
	return CHDRV_TUNER_BLIND_SEARCH_CONTINUE;
}

#if ENABLE_TESTCASE

CASE_F(TEST_TUNER_INIT1,
	"���Զ�γ�ʼ��ʱ�Ƿ񷵻�OK") {
	EXPECT_EQ(CHDRV_TUNER_OK, CHDRV_TUNER_Init());
	EXPECT_EQ(CHDRV_TUNER_OK, CHDRV_TUNER_Init()) << "��γ�ʼ��Ӧ�÷���OK";
}

CASE_F(TEST_TUNER_INIT2,
	"����ģ���Ƿ�ɱ�����") {
	EXPECT_EQ(CHDRV_TUNER_OK, CHDRV_TUNER_Term());
	EXPECT_EQ(CHDRV_TUNER_OK, CHDRV_TUNER_Init());
}

CASE_F(TEST_TUNER_INIT3,
	"����CHDRV_TUNER_GetVersion�Ƿ�ɹ�") {
	CHDRV_Version_t stru_Ver;
	
	memset(&stru_Ver, 0, sizeof(stru_Ver));
	EXPECT_EQ(CHDRV_TUNER_OK, CHDRV_TUNER_GetVersion(&stru_Ver));
	EXPECT_NE(0, stru_Ver);
	LOG("\nstru_Ver=%x\n", stru_Ver);
}
int tuner_get_device_id(void)
{
	char key[256];
	CMD_PARAM device_id;
	
	char str_tuner_params[16];
	
	sprintf(str_tuner_params, "TUNER_PARAMS%d", gui_TunerIndex + 1);
	
	parse_key(fw_getenv(str_tuner_params), "device_id", key, sizeof(key));

	device_id = (CMD_PARAM)key;

	return device_id.get_i();
}

CASE_F(TEST_TUNER_FUNC_1,
	"ע��3����ͬ�Ļص��������Ƿ�����سɹ�") {

	
	int device_id;

	device_id = tuner_get_device_id();
	EXPECT_EQ(CHDRV_TUNER_OK, CHDRV_TUNER_InstallCallBack(device_id,TEST_TUNER_MonitorCallback1));
	EXPECT_EQ(CHDRV_TUNER_OK, CHDRV_TUNER_InstallCallBack(device_id,TEST_TUNER_MonitorCallback2));
	EXPECT_EQ(CHDRV_TUNER_OK, CHDRV_TUNER_InstallCallBack(device_id,TEST_TUNER_MonitorCallback3));
}
CASE_F(TEST_TUNER_FUNC_2,
	"��ʾ����Ա���ո���������ò��Ի���������ָ���������ý�����Ƶ��Ӧ���سɹ���") {

	char str_tuner_params[16];

	sprintf(str_tuner_params, "TUNER_PARAMS%d", gui_TunerIndex + 1);
	request_play_program("AV_STREAM_COMMON", str_tuner_params);
	EXPECT_EQ(0, RUN_CMD("tuner.lock %s", fw_getenv(str_tuner_params)));
	}

CASE_F(TEST_TUNER_FUNC_5,
	"����CHDRV_TUNER_Set22KhzTone��Ӧ����OK") {
	char *p;
	char key[256];
	char str_tuner_params[16];

	sprintf(str_tuner_params, "TUNER_PARAMS%d", gui_TunerIndex + 1);
	p = parse_key(fw_getenv(str_tuner_params), "type", key, sizeof(key));
	if(p && !strcmp(p,"dvbs"))
	{
		EXPECT_EQ(CHDRV_TUNER_OK,CHDRV_TUNER_Set22KhzTone(0,CH_TRUE));
		EXPECT_EQ(CHDRV_TUNER_OK,CHDRV_TUNER_Set22KhzTone(0,CH_FALSE));
	}
}
CASE_F(TEST_TUNER_FUNC_6,
	"����CHDRV_TUNER_Set12VOut��Ӧ����ok") {

	char *p;
	char key[256];

	char str_tuner_params[16];

	sprintf(str_tuner_params, "TUNER_PARAMS%d", gui_TunerIndex + 1);
	p = parse_key(fw_getenv(str_tuner_params), "type", key, sizeof(key));
	if(p && !strcmp(p,"dvbs"))
	{
		EXPECT_EQ(CHDRV_TUNER_OK,CHDRV_TUNER_Set12VOut(0,CH_TRUE));
		EXPECT_EQ(CHDRV_TUNER_OK,CHDRV_TUNER_Set12VOut(0,CH_FALSE));	
	}
}

CASE_F(TEST_TUNER_FUNC_7,
	"����CHDRV_TUNER_SetLNBVoltage��Ӧ����ok") {

	char *p;
	char key[256];

	char str_tuner_params[16];

	sprintf(str_tuner_params, "TUNER_PARAMS%d", gui_TunerIndex + 1);
	p = parse_key(fw_getenv(str_tuner_params), "type", key, sizeof(key));
	if(p && !strcmp(p,"dvbs"))
	{
		EXPECT_EQ(CHDRV_TUNER_OK,CHDRV_TUNER_SetLNBVoltage(0,CHDRV_LNB_POWER_OFF));
		EXPECT_EQ(CHDRV_TUNER_OK,CHDRV_TUNER_SetLNBVoltage(0,CHDRV_LNB_POWER_13V));
		EXPECT_EQ(CHDRV_TUNER_OK,CHDRV_TUNER_SetLNBVoltage(0,CHDRV_LNB_POWER_18V));
	}
}


CASE_F(TEST_TUNER_FUNC_8,
	"����CHDRV_TUNER_InstallBlindSearchCallback��ע��ص�������Ӧ���سɹ�") {

	char *p;
	char key[256];

	char str_tuner_params[16];

	sprintf(str_tuner_params, "TUNER_PARAMS%d", gui_TunerIndex + 1);
	p = parse_key(fw_getenv(str_tuner_params), "type", key, sizeof(key));
	if(p && !strcmp(p,"dvbs"))
	{
		EXPECT_EQ(CHDRV_TUNER_OK,CHDRV_TUNER_InstallBlindSearchCallback(0,TEST_TUNER_TPFindCallback));
	}
   
}
CASE_F(TEST_TUNER_FUNC_9,
	"����CHDRV_TUNER_BlindSearch��Ӧ�ɹ�������callback���ȡ���������Ⱥ�״̬	") {
   
	char *p;
	char key[256];


	char str_tuner_params[16];
	sprintf(str_tuner_params, "TUNER_PARAMS%d", gui_TunerIndex + 1);
	p = parse_key(fw_getenv(str_tuner_params), "type", key, sizeof(key));
	if(p && !strcmp(p,"dvbs"))
	{
		CHDRV_TUNER_BlindSearchParams_t stu_para;
		stu_para.i_StartFre = 950000;
		stu_para.i_EndFre= 950000;
		stu_para.i_StepFre= 2000;
	       CHDRV_TUNER_BlindSearch(0, stu_para);
		EXPECT_LT(-1, TpfindStatus);
		EXPECT_LT(-1, schedule);		
	}
   
}


void  test_section_filterCall(CHDRV_SECTION_ChannelHandle rh_Channel,
	 		CHDRV_SECTION_EVENT_e 	renm_Event,
			CHDRV_SECTION_CallBackDataParam_t *rpstru_Param)
{
	 if(renm_Event == CHDRV_SECTION_EVENT_DATA_DETECT)
	 {
		gui_ReceivedPatSections++;
	}
}

static void alloc_section_channel(void)
{
	CHDRV_SECTION_AllocParam_t 		stru_Param;
	U8								Data[8]={0};
	U8								Mask[8]={0xff};

	if(gh_Channel == NULL)
	{
		EXPECT_EQ(CHDRV_SECTION_OK,CHDRV_SECTION_AllocChannel(get_demux_id(), 0, test_section_filterCall, &gh_Channel, 1024));
	}
	if(gh_Filter == NULL)
	{
		stru_Param.h_Channel = gh_Channel;
		stru_Param.enm_FilterSize = CHDRV_SECTION_FILTER_SIZE_8BYTE;
		stru_Param.enm_MatchMode = CHDRV_SECTION_MODE_MATCH;
		stru_Param.enm_SectionSize = CHDRV_SECTION_FILTER_SIZE_4K;
		EXPECT_EQ(CHDRV_SECTION_OK,CHDRV_SECTION_AllocFilter(get_demux_id(),&stru_Param,&gh_Filter));	
	}
	EXPECT_EQ(CHDRV_SECTION_OK,CHDRV_SECTION_SetFilter(gh_Filter,Data,Mask,Data,1,CH_FALSE));
	EXPECT_EQ(CHDRV_SECTION_OK,CHDRV_SECTION_StartFilter(gh_Filter));
	EXPECT_EQ(CHDRV_SECTION_OK,CHDRV_SECTION_StartChannel(gh_Channel));	
}

static void release_section_channel(void)
{
	if(gh_Filter)
	{
		EXPECT_EQ(CHDRV_SECTION_OK,CHDRV_SECTION_StopFilter(gh_Filter));
		EXPECT_EQ(CHDRV_SECTION_OK,CHDRV_SECTION_FreeFilter(gh_Filter));
		gh_Filter = NULL;
	}
	if(gh_Channel)
	{
		EXPECT_EQ(CHDRV_SECTION_OK,CHDRV_SECTION_StopChannel(gh_Channel));	
		EXPECT_EQ(CHDRV_SECTION_OK,CHDRV_SECTION_FreeChannel(gh_Channel));	
		gh_Channel = NULL;
	}
}
CASE_F(TEST_TUNER_FUNC_10,
	"����һ��pat��񣬵ȴ�6S��Ӧ�ܳ������ϵ��յ�pat����") {
	

	gui_ReceivedPatSections = 0;

	alloc_section_channel();
	CH_RTOS_TaskDelay(6000);

	CH_BOOL b_IfLock = CH_FALSE;
	CHDRV_TUNER_GetLockStatus(tuner_get_device_id(), &b_IfLock);
	EXPECT_LE(1, gui_ReceivedPatSections) << "��Ƶ��pat�����ղ������������������߻�������,��ǰtuner����״̬Ϊ" << (b_IfLock ? "LOCK" : "UNLOCK");
}


CASE_F(TEST_TUNER_FUNC_11,"����CHDRV_TUNER_GetLockStatusӦ�ɹ���״̬Ϊ����;����CHDRV_TUNER_GetErrorRateӦ�ɹ���������С��100000"){
	CH_BOOL lock = CH_FALSE;
	S32 rate = 0;

	int device_id;

	device_id = tuner_get_device_id();
	
	EXPECT_EQ(CHDRV_TUNER_OK,CHDRV_TUNER_GetLockStatus(device_id,&lock));
	EXPECT_EQ(1, lock);
	EXPECT_EQ(CHDRV_TUNER_OK,CHDRV_TUNER_GetErrorRate(device_id,&rate));
	EXPECT_GT(100000,rate);	
	EXPECT_LE(1, gui_ReceivedPatSections); 
}


	


CASE_F(TEST_TUNER_FUNC_12,
	"����CHDRV_TUNER_GetSNR��snr��Χ0~50;����CHDRV_TUNER_GetQuality��quality��ΧΪ0~100") {
	S16 snr = 0 ;
	S8 quality = 0;
	char str_tuner_params[32];
	int device_id;

	sprintf(str_tuner_params, "TUNER_PARAMS%d", gui_TunerIndex + 1);
	request_play_program("AV_STREAM_COMMON", str_tuner_params);
	EXPECT_EQ(0, RUN_CMD("tuner.lock %s", fw_getenv(str_tuner_params)));

	device_id = tuner_get_device_id();
	
	EXPECT_EQ(CHDRV_TUNER_OK,CHDRV_TUNER_GetSNR(device_id,&snr));
	EXPECT_GE(50,snr);
	EXPECT_LT(0,snr);
	EXPECT_EQ(CHDRV_TUNER_OK,CHDRV_TUNER_GetQuality(device_id,&quality));
	EXPECT_GE(100,quality);
	EXPECT_LT(0,quality);
}

CASE_F(TEST_TUNER_FUNC_13,
	"����CHDRV_TUNER_GetStrengthDB��db��ΧΪ0~100 ����CHDRV_TUNER_GetStrengthPercent��percent��ΧΪ0~100") {
	S8 strengthDB,strengthPer;

	int device_id;

	device_id = tuner_get_device_id();
	EXPECT_EQ(CHDRV_TUNER_OK,CHDRV_TUNER_GetStrengthDB(device_id, &strengthDB));
	EXPECT_GE(100,strengthDB);
	EXPECT_LT(0,strengthDB);
	EXPECT_EQ(CHDRV_TUNER_OK,CHDRV_TUNER_GetStrengthPercent(device_id, &strengthPer));
	EXPECT_GE(100,strengthPer);
	EXPECT_LT(0,strengthPer);
}

CASE_F(TEST_TUNER_FUNC_14,
	"����CHDRV_TUNER_Break��Ӧ����OK ע���3���ص��������յ�����״̬�ı��֪ͨ") {
        U8 temp_lock0,temp_lock1,temp_lock2;

      int device_id;

	device_id = tuner_get_device_id();
	
	temp_lock0= LockCount0;
	temp_lock1= LockCount1;
	temp_lock2= LockCount2;
	EXPECT_EQ(CHDRV_TUNER_OK,CHDRV_TUNER_Break(device_id));
	CH_RTOS_TaskDelay(1000);
	gui_ReceivedPatSections = 0;
	EXPECT_NE(temp_lock0, LockCount0) << "CHDRV_TUNER_Breakδ��ȷʵ��";
	EXPECT_NE(temp_lock1, LockCount1) << "CHDRV_TUNER_Breakδ��ȷʵ��";
	EXPECT_NE(temp_lock2, LockCount2) << "CHDRV_TUNER_Breakδ��ȷʵ��";	
}

CASE_F(TEST_TUNER_FUNC_15,
	"����CHDRV_TUNER_GetLockStatusӦ�ɹ���״̬Ϊδ����   ����һ��pat��񣬵ȴ�6S��Ӧ�����յ�����") {
	CH_BOOL lock = 0;

	int device_id;

	device_id = tuner_get_device_id();
	
	EXPECT_EQ(CHDRV_TUNER_OK,CHDRV_TUNER_GetLockStatus(device_id, &lock));
	EXPECT_EQ(0, lock) << "CHDRV_TUNER_Breakδ��ȷʵ��";
	EXPECT_EQ(0, gui_ReceivedPatSections) << "CHDRV_TUNER_Breakδ��ȷʵ��";
	
}

CASE_F(TEST_TUNER_FUNC_16,
	"����CHDRV_TUNER_GetErrorRateӦ�ɹ���������Ϊ1000000000����CHDRV_TUNER_GetSNR��snr��Χ0") {
	S32  rate;
	S16 snr;

	int device_id;

	device_id = tuner_get_device_id();
	EXPECT_EQ(CHDRV_TUNER_OK,CHDRV_TUNER_GetErrorRate(device_id, &rate));
	EXPECT_EQ(1000000000,rate) << "CHDRV_TUNER_Breakδ��ȷʵ��";
	EXPECT_EQ(CHDRV_TUNER_OK,CHDRV_TUNER_GetSNR(device_id, &snr));
	EXPECT_EQ(0,snr) << "CHDRV_TUNER_Breakδ��ȷʵ��";		
}

CASE_F(TEST_TUNER_FUNC_17,
	"����CHDRV_TUNER_GetQuality��quality��ΧΪ0����CHDRV_TUNER_GetStrengthDB��db��ΧΪ0~100 ����CHDRV_TUNER_GetStrengthPercent��percent��ΧΪ0~100") {

	S8 quality,strengthDB,strengthPer;

	int device_id;

	device_id = tuner_get_device_id();
	
	EXPECT_EQ(CHDRV_TUNER_OK,CHDRV_TUNER_GetQuality(device_id,&quality));
	EXPECT_EQ(0,quality) << "CHDRV_TUNER_Breakδ��ȷʵ��";
	EXPECT_EQ(CHDRV_TUNER_OK,CHDRV_TUNER_GetStrengthDB(device_id, &strengthDB));
	EXPECT_GT(100,strengthDB) << "CHDRV_TUNER_Breakδ��ȷʵ��";
	EXPECT_LT(0,strengthDB) << "CHDRV_TUNER_Breakδ��ȷʵ��";
	EXPECT_EQ(CHDRV_TUNER_OK,CHDRV_TUNER_GetStrengthPercent(device_id, &strengthPer));
	EXPECT_GT(100,strengthPer);
	EXPECT_LT(0,strengthPer);
}

CASE_F(TEST_TUNER_MODE,"����tuner�ڳ��õĸ���ģʽ���Ƿ���������Ƶ"){
	CH_BOOL lock;
	int retry = 0;;
	int device_id;
	int prognum = 0;
	device_id = tuner_get_device_id();
	char *p, *penv;
	char key[32];
	char params[512];
	char str[32];
	sprintf(str, "TUNER_PARAMS%d", gui_TunerIndex + 1);
	penv = fw_getenv(str);
	p = parse_key(penv, "type", key, sizeof(key));
	if(p && !strcmp(p,"dvbs"))
	{
		const char*pkeyword[] = {"type", "device_id", "freq", "pola", "symbo", "psk"};
		const char* pvalue[][2] = {{"27500", "qpsk"}, {"27500", "8psk"}, {"28888", "8psk"}, {"28888", "qpsk"}};
		int i, j;
		for(i = 0; i < 4;i ++)
		{
			memset(&params[0], 0, sizeof(params));
			for(j = 0; j < 4; j ++)
			{
				p = parse_key(penv, pkeyword[j], key, sizeof(key));
				if(p)
				{
					sprintf(str, "%s=%s,", pkeyword[j], p);
					strcat(params, str);
				}
				else
				{
					LOG("not find %s in %s\n", pkeyword[j], penv);
				}
			}
			sprintf(str, "%s=%s,", pkeyword[j], pvalue[i][0]);
			strcat(params, str);
			j++;
			sprintf(str, "%s=%s", pkeyword[j], pvalue[i][1]);
			strcat(params, str);
			fw_setenv("tuner_mode_test", params);
			sprintf(str, "TUNER_PARAMS%d", gui_TunerIndex + 1);
			penv = fw_getenv(str);//���ú�Ҫ���»�ȡenv������ֵ����
			prognum = request_play_program("AV_STREAM_COMMON", "tuner_mode_test");
			EXPECT_EQ(0, RUN_CMD("tuner.lock %s", params));
			gui_ReceivedPatSections = 0;
			for(retry = 0; retry < 10; retry++)
			{	
				if(CHDRV_TUNER_GetLockStatus(device_id, &lock) == CHDRV_TUNER_OK
					&& lock
					&& gui_ReceivedPatSections > 1)
				{
					break;
				}
				CH_RTOS_TaskDelay(1000);
			}
			
			if(retry < 10)
			{	
				release_section_channel();
				RUN_CMD("av.start %d %d\n", device_id, prognum);
				PROMPT("����Ƶ�����Ƿ���������������Y����������N:");
				EXPECT_EQ(test_input_yes(), CH_TRUE) << "��tunerģʽ[" << params << "]�£���Ŀ���Ų�����";
				alloc_section_channel();
			}
			else
			{
				FAIL() << "��tunerģʽ[" << params << "]�£�tuner�޷�����";
			}
		}
		return;
	}
	if(p && !strcmp(p,"dvbt"))
	{
		const char*pkeyword[] = {"type", "device_id", "freq", "bandwidth"};
		int pvalue[] = {6, 8};
		int i, j;
		for(i = 0; i < 2;i ++)
		{
			memset(&params[0], 0, sizeof(params));
			for(j = 0; j < 3; j ++)
			{
				p = parse_key(penv, pkeyword[j], key, sizeof(key));
				if(p)
				{
					sprintf(str, "%s=%s,", pkeyword[j], p);
					strcat(params, str);
				}
				else
				{
					LOG("not find %s in %s\n", pkeyword[j], penv);
				}
			}
			sprintf(str, "%s=%d", pkeyword[j], pvalue[i]);
			strcat(params, str);
			fw_setenv("tuner_mode_test", params);
			sprintf(str, "TUNER_PARAMS%d", gui_TunerIndex + 1);
			penv = fw_getenv(str);//���ú�Ҫ���»�ȡenv������ֵ����
			prognum = request_play_program("AV_STREAM_COMMON", "tuner_mode_test");
			EXPECT_EQ(0, RUN_CMD("tuner.lock %s", params));
			gui_ReceivedPatSections = 0;
			for(retry = 0; retry < 10; retry++)
			{
				if(CHDRV_TUNER_GetLockStatus(device_id, &lock) == CHDRV_TUNER_OK
					&& lock
					&& gui_ReceivedPatSections > 1)
				{
					break;
				}
				CH_RTOS_TaskDelay(1000);
			}
			
			if(retry < 10)
			{
				release_section_channel();
				RUN_CMD("av.start %d %d\n", device_id, prognum);
				PROMPT("����Ƶ�����Ƿ���������������Y����������N:");
				EXPECT_EQ(test_input_yes(), CH_TRUE) << "��tunerģʽ[" << params << "]�£���Ŀ���Ų�����";
				alloc_section_channel();
			}
			else
			{
				FAIL() << "��tunerģʽ[" << params << "]�£�tuner�޷�����";
			}
		}
		return;
	}
	if(p && !strcmp(p,"dvbc"))
	{
		const char*pkeyword[] = {"type", "device_id", "freq", "symbo", "qam"};
		int pvalue[][2] = {{6875, 64}, {6900, 256}};
		int i, j;
		for(i = 0; i < 2;i ++)
		{
			memset(&params[0], 0, sizeof(params));
			for(j = 0; j < 3; j ++)
			{
				p = parse_key(penv, pkeyword[j], key, sizeof(key));
				if(p)
				{
					sprintf(str, "%s=%s,", pkeyword[j], p);
					strcat(params, str);
				}
				else
				{
					LOG("not find %s in %s\n", pkeyword[j], penv);
				}
			}
			sprintf(str, "%s=%d,", pkeyword[j], pvalue[i][0]);
			strcat(params, str);
			j++;
			sprintf(str, "%s=%d", pkeyword[j], pvalue[i][1]);
			strcat(params, str);
			fw_setenv("tuner_mode_test", params);
			sprintf(str, "TUNER_PARAMS%d", gui_TunerIndex + 1);
			penv = fw_getenv(str);//���ú�Ҫ���»�ȡenv������ֵ����
			prognum = request_play_program("AV_STREAM_COMMON", "tuner_mode_test");
			EXPECT_EQ(0, RUN_CMD("tuner.lock %s", params));
			gui_ReceivedPatSections = 0;
			for(retry = 0; retry < 10; retry++)
			{	
				if(CHDRV_TUNER_GetLockStatus(device_id, &lock) == CHDRV_TUNER_OK
					&& lock
					&& gui_ReceivedPatSections > 1)
				{
					break;
				}
				CH_RTOS_TaskDelay(1000);
			}
			
			if(retry < 10)
			{
				release_section_channel();
				RUN_CMD("av.start %d %d\n", device_id, prognum);
				PROMPT("����Ƶ�����Ƿ���������������Y����������N:");
				EXPECT_EQ(test_input_yes(), CH_TRUE) << "��tunerģʽ[" << params << "]�£���Ŀ���Ų�����";
				alloc_section_channel();
			}
			else
			{
				FAIL() << "��tunerģʽ[" << params << "]�£�tuner�޷�����";
			}
		}
		return;
	}
}
CASE_F(TEST_TUNER_FUNC_18,
	"����CHDRV_TUNER_UninstallCallBack��ɾ����ע��Ļص�����") {

	int device_id;

	release_section_channel();
	device_id = tuner_get_device_id();
	EXPECT_EQ(CHDRV_TUNER_OK, CHDRV_TUNER_UninstallCallBack(device_id,TEST_TUNER_MonitorCallback1));
	EXPECT_EQ(CHDRV_TUNER_OK, CHDRV_TUNER_UninstallCallBack(device_id,TEST_TUNER_MonitorCallback2));
	EXPECT_EQ(CHDRV_TUNER_OK, CHDRV_TUNER_UninstallCallBack(device_id,TEST_TUNER_MonitorCallback3));


}

#endif /* ENABLE_TESTCASE */


int tuner_lock(int argc, CMD_PARAM argv[])
{
	char key[256];
	char *p;
	int loop;
	CMD_PARAM value;
	CHDRV_TUNER_IQ_TYPE_e enm_IQSpectrum = CHDRV_TUNER_IQ_AUTO;
	do
	{
		p = parse_key(argv[1].get_str(), "type", key, sizeof(key));
		if(p)
		{
			if(!strcmp(p, "dvbc"))
			{
				int freq = 0, symbol = 0, qam = 0, device_id = 0, ret = -1;
				 
    
				value = parse_key(argv[1].get_str(), "freq", key, sizeof(key));
				freq = value.get_i();
				value = parse_key(argv[1].get_str(), "symbo", key, sizeof(key));
				symbol = value.get_i();
				value = parse_key(argv[1].get_str(), "qam", key, sizeof(key));
				qam = value.get_i();
				switch(qam)
				{
					case 32:
						qam = CHDRV_TUNER_QAM_32;
						break;
					case 64:
						qam = CHDRV_TUNER_QAM_64;
						break;
					case 128:
						qam = CHDRV_TUNER_QAM_128;
						break;
					case 256:
						qam = CHDRV_TUNER_QAM_256;
						break;
					default:
						qam = CHDRV_TUNER_QAM_64;
						break;
				}
				value = parse_key(argv[1].get_str(), "device_id", key, sizeof(key));
				device_id = value.get_i();

				
    				CH_BOOL b_IfLock = CH_FALSE;
				ret = CHDRV_TUNER_Break(device_id);

				for(loop = 0; loop < 10; loop++)
				{
					ret = CHDRV_TUNER_QAM_SetIF(device_id,freq,symbol,(CHDRV_TUNER_QAM_MODE_e)qam,&enm_IQSpectrum,&b_IfLock);
					if((ret == CHDRV_TUNER_OK) && b_IfLock)
					{
						LOG("%s freq=%d,symbol=%d,qam=%d,return %d, lock=%d\n",argv[0].get_str(),freq,symbol,qam,ret,b_IfLock);
						return ret;
					}
					CH_RTOS_TaskDelay(1000);
				}
				LOG("%s freq=%d,symbol=%d,qam=%d,return %d, lock=%d\n",argv[0].get_str(),freq,symbol,qam,ret,b_IfLock);
				return ret;
			}
		else if(!strcmp(p, "dvbt"))
			{
				int freq = 0, bandwidth = 0,  device_id = 0, ret = -1;
    
				value = parse_key(argv[1].get_str(), "freq", key, sizeof(key));
				freq = value.get_i();
				value = parse_key(argv[1].get_str(), "bandwidth", key, sizeof(key));
				bandwidth = value.get_i();
				switch(bandwidth)
				{
					case 6:
						bandwidth = CHDRV_TUNER_COFDM_BW_6MHZ;
						break;
					case 7:
						bandwidth = CHDRV_TUNER_COFDM_BW_7MHZ;
						break;
					case 8:
						bandwidth = CHDRV_TUNER_COFDM_BW_8MHZ;
						break;
					default:
						bandwidth = CHDRV_TUNER_COFDM_BW_8MHZ;
						break;
				}
				value = parse_key(argv[1].get_str(), "device_id", key, sizeof(key));
				device_id = value.get_i();

				
    				CH_BOOL b_IfLock = CH_FALSE;
				ret = CHDRV_TUNER_Break(device_id);

				for(loop = 0; loop < 10; loop++)
				{
					ret = CHDRV_TUNER_COFDM_SetIF(device_id,freq,(CHDRV_TUNER_COFDM_BANDWIDTH_e)bandwidth,&enm_IQSpectrum,&b_IfLock);
					if((ret == CHDRV_TUNER_OK) && b_IfLock)
					{
						LOG("%s freq=%d,bandwidth=%d,return %d, lock=%d\n",argv[0].get_str(),freq,bandwidth,ret,b_IfLock);
						return ret;
					}
					CH_RTOS_TaskDelay(1000);
				}
				LOG("%s freq=%d,bandwidth=%d,return %d, lock=%d\n",argv[0].get_str(),freq,bandwidth,ret,b_IfLock);
				return ret;
			}	
		else if(!strcmp(p,"dvbs"))
		{
		 	S32 device_id,freq,sys,ret = -1;
			value = parse_key(argv[1].get_str(), "device_id", key, sizeof(key));
			device_id = value.get_i();
			value = parse_key(argv[1].get_str(), "freq", key, sizeof(key));
			freq = value.get_i();
			value = parse_key(argv[1].get_str(), "symbo", key, sizeof(key));
			sys = value.get_i();		

			CH_BOOL b_IfLock = CH_FALSE;
			ret = CHDRV_TUNER_Break(device_id);

			for(loop = 0; loop < 10; loop++)
			{
				ret = CHDRV_TUNER_QPSK_SetIF(device_id,freq,sys,&enm_IQSpectrum,&b_IfLock);
				if((ret == CHDRV_TUNER_OK) && b_IfLock)
				{
					LOG("%s freq=%d,symbol=%d,return %d, lock=%d\n",argv[0].get_str(),freq,sys,ret,b_IfLock);
					return ret;
				}
				CH_RTOS_TaskDelay(1000);
			}
	        
			LOG("%s freq=%d,symbol=%d,return %d, lock=%d\n",argv[0].get_str(),freq,sys,ret,b_IfLock);
			return ret;
		}
			}
	}while(0);

	return -2;
	
}


int tuner_status(int argc, CMD_PARAM argv[])
{
	int ret;
    	CH_BOOL b_IfLock = CH_FALSE;
	S8 c_Quality = 0, c_StrengthDB = 0, c_StrengthPercent = 0;
	S16 s_SNR = 0;

	ret = CHDRV_TUNER_GetLockStatus(argv[1].get_i(), &b_IfLock);
	CHDRV_TUNER_GetQuality(argv[1].get_i(), &c_Quality);
	CHDRV_TUNER_GetSNR(argv[1].get_i(), &s_SNR);
	CHDRV_TUNER_GetStrengthDB(argv[1].get_i(), &c_StrengthDB);
	CHDRV_TUNER_GetStrengthPercent(argv[1].get_i(), &c_StrengthPercent);
	LOG("CHDRV_TUNER_GetLockStatus return %d, lock=%d,quality=%d,snr=%d,strength db=%d,stringth percent=%d%%\n",
		ret, b_IfLock, c_Quality, s_SNR, c_StrengthDB, c_StrengthPercent);
	
	return ret;
}

static int tuner_init(int argc, CMD_PARAM argv[])
{
	int ret;

	ret = CHDRV_TUNER_Init();
	LOG("CHDRV_TUNER_Init return %d\n", ret);
	return ret;
}
static int tuner_term(int argc, CMD_PARAM argv[])
{
	int ret;

	ret = CHDRV_TUNER_Term();
	LOG("CHDRV_TUNER_Term return %d\n", ret);
	return ret;
}

static int tuner_ver(int argc, CMD_PARAM argv[])
{
	int ret;
	CHDRV_Version_t ver = 0;
	ret = CHDRV_TUNER_GetVersion(&ver);
	LOG("CHDRV_TUNER_Term return %d, version=%d\n", ret, ver);
	return ret;
}
static int tuner_params(int argc,CMD_PARAM argv[])
{
	CHDRV_TUNER_RESULT_e ret;
	S32 freq,sys;
	CHDRV_TUNER_QAM_MODE_e QamMd;
	CHDRV_TUNER_COFDM_BANDWIDTH_e Tband;
	int Qam_num, Band_num;
	S16 offset;
	CHDRV_TUNER_IQ_TYPE_e IQtype;
#ifdef CFG_CHSSA_SIGNAL_DVBC
	{
		ret = CHDRV_TUNER_QAM_GetCarrierParams(argv[1].get_i(),&freq,&sys,&QamMd,&offset,&IQtype);	
		switch(QamMd)
		{
			case 0: Qam_num = 8;
			break;
			case 1: Qam_num = 16;
			break;
			case 2: Qam_num = 32;
			break;	
			case 3: Qam_num = 64;
			break;	
			case 4: Qam_num = 128;
			break;	
			case 5: Qam_num = 256;
			break;	
			default:
			Qam_num = 64;
			break;
			
		}
		LOG("freq=%d,sym=%d,qammd=%d,offset=%d,iqtype=%d\n",freq,sys,Qam_num,offset,IQtype);
	}
#endif /* CFG_CHSSA_SIGNAL_DVBC */
#ifdef CFG_CHSSA_SIGNAL_DVBT
	{
		ret = CHDRV_TUNER_COFDM_GetCarrierParams(argv[1].get_i(),&freq,&Tband,&offset,&IQtype);	
		switch(Tband)
		{
			case 0: Band_num = 6;
			break;
			case 1: Band_num = 7;
			break;
			case 2: Band_num = 8;
			break;	
			default:
			Band_num = 8;
			break;
		}
		LOG("freq=%d,sym=%d,qammd=%d,offset=%d,iqtype=%d\n",freq,sys,Qam_num,offset,IQtype);
	}
#endif /* CFG_CHSSA_SIGNAL_DVBT */
#ifdef CFG_CHSSA_SIGNAL_DVBS	
	{
		ret = CHDRV_TUNER_QPSK_GetCarrierParams(argv[1].get_i(),&freq,&sys,&offset,&IQtype);	
		LOG("freq=%d,sym=%d,qammd=%d,offset=%d,iqtype=%d\n",freq,sys,offset,IQtype);
	}	
#endif	
		return ret;
}
static int tuner_set_22k(int argc,CMD_PARAM argv[])
{ 
	int ret = 0;
#ifdef CFG_CHSSA_SIGNAL_DVBS
	{
		ret = CHDRV_TUNER_Set22KhzTone(argv[1].get_i(),argv[2].get_i());
	}
#endif /* CFG_CHSSA_SIGNAL_DVBS */
	return ret;
}
static int tuner_set_12v(int argc,CMD_PARAM argv[])
{
	int ret = 0;
#ifdef CFG_CHSSA_SIGNAL_DVBS
	{
		ret = CHDRV_TUNER_Set12VOut(argv[1].get_i(),argv[2].get_i());
	}
#endif /* CFG_CHSSA_SIGNAL_DVBS */
	return ret;
}
static int tuner_set_lnb(int argc,CMD_PARAM argv[])
{
	int ret = 0;
#ifdef CFG_CHSSA_SIGNAL_DVBS
	
	CHDRV_TUNER_LNB_POWER_e renm_LnbPower;
	switch(argv[2].get_i())
	{
	case 0:
		renm_LnbPower = CHDRV_LNB_POWER_OFF;
		break;
	case 1:
		renm_LnbPower = CHDRV_LNB_POWER_13V;
		break;
	case 2:
		renm_LnbPower = CHDRV_LNB_POWER_18V;
		break;
	default :
		renm_LnbPower = CHDRV_LNB_POWER_OFF;
	}
	{
	ret = CHDRV_TUNER_SetLNBVoltage(argv[1].get_i(),renm_LnbPower);
	}
#endif /* CFG_CHSSA_SIGNAL_DVBS */

	return ret;
}
	
static  int tuner_blind_search(int argc,CMD_PARAM argv[])
{
	int ret = 0;
#ifdef CFG_CHSSA_SIGNAL_DVBS
	
	{
      CHDRV_TUNER_BlindSearchParams_t rstru_SearchParams;
	 rstru_SearchParams.i_StartFre= argv[2].get_i();
	 rstru_SearchParams.i_EndFre = argv[3].get_i();
	 rstru_SearchParams.i_StepFre  = argv[4].get_i();	 
      CHDRV_TUNER_BlindSearch(argv[1].get_i(), rstru_SearchParams);
	}
#endif /* CFG_CHSSA_SIGNAL_DVBS */
	return ret;
}

extern  int tuner_info(int argc,CMD_PARAM argv[]);
extern int tuner_adjust(int argc,CMD_PARAM argv[]);

REGISTER_CMD(tuner_init, "��ʼ��tunerģ��", "",  "");
REGISTER_CMD(tuner_term, "����tunerģ��", "",  "");
REGISTER_CMD(tuner_ver, "��ӡtunerģ��汾��", "",  "");
#ifdef CFG_CHSSA_SIGNAL_DVBT
REGISTER_CMD(tuner_lock,  "�����ַ�������Ƶ",
	"string(type=dvbs,device_id=0,freq=1472000,symbo=27500,pola=h��type=dvbc,device_id=0,freq=235000,symbo=6875,qam=64)", 
	"\"type=dvbt,device_id=0,freq=802000,bandwidth=8\"");
#elif defined(CFG_CHSSA_SIGNAL_DVBS)
REGISTER_CMD(tuner_lock,  "�����ַ�������Ƶ",
	"string(type=dvbc,device_id=0,freq=235000,symbo=6875,qam=64��type=dvbt,device_id=0,freq=802000,bandwidth=8)", 
	"\"type=dvbs,device_id=0,freq=1472000,symbo=27500,pola=h\"");
#else
REGISTER_CMD(tuner_lock,  "�����ַ�������Ƶ",
	"string(type=dvbs,device_id=0,freq=1472000,symbo=27500,pola=h��type=dvbt,device_id=0,freq=802000,bandwidth=8)", 
	"\"type=dvbc,device_id=0,freq=235000,symbo=6875,qam=64\"");
#endif
REGISTER_CMD(tuner_status, "��ӡ��ǰ������״̬���ź�ǿ�ȵ�״̬", "device_id",  "0");
REGISTER_CMD(tuner_params, "��ӡ��ǰ��tuner����", "device_id",  "0");
REGISTER_CMD(tuner_set_22k, "�����Ƿ���22K", "device_id",  "0");
REGISTER_CMD(tuner_set_12v, "�����Ƿ���12V����", "device_id",  "0");
REGISTER_CMD(tuner_set_lnb, "���ø�Ƶͷ�����ѹ", "device_id",  "0");
REGISTER_CMD(tuner_blind_search, "��ʼäɨ", "device_id",  "0");
REGISTER_CMD(tuner_info, "��ӡ��TUNER�����������Ϣ", "",  "");
REGISTER_CMD(tuner_adjust, "У׼tuner���ź�ǿ����ʾ", "device_id step",  "0 2");




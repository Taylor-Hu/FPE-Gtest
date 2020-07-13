/**
@file test_pvr.cpp
@def group pvr���Լ�

@author ���繫˾����������:	����ǿ
@date   2014��4��10��  
@version V4.0
@todo     ---ʱ��---����-------------�޸Ĳ��ּ�ԭ��----------
@todo     1 2014��4��10�� ����ǿ  ����


@{
*/
using namespace std;
/*���Լ����ƶ���*/
#ifdef CASE_NAME
#undef CASE_NAME
#endif
#define CASE_NAME TestPvr

#include "test_menu.h"
#include "test_modules.h"
#include "chdrv_pvr.h"
//����������
#define  PVR_ERRO_REC_CMD  						  (CHDRV_PVR_REC_CMD_MAX)
#define  PVR_ERRO_REPLAY_CMD  					(CHDRV_PVR_REPLAY_CMD_MAX)
#define  PVR_ERRO_REC_STATUS  					(CHDRV_PVR_REC_STATUS_MAX)
#define  PVR_ERRO_REPLAY_STATUS  				(CHDRV_PVR_REPLAY_STATUS_MAX)
#define  PVR_ERRO_REPLAY_SPEED     				(CHDRV_PVR_REPLAY_SPEED_MAX)
#define  PVR_ERRO_REPLAY_TYPE     				(CHDRV_PVR_REPLAY_TYPE_MAX)
//������ȷ����
#define  PVR_NORMAL_REC_CMD  					(CHDRV_PVR_REC_PAUSE)
#define  PVR_NORMAL_REPLAY_CMD  				(CHDRV_PVR_REPLAY_PAUSE)
#define  PVR_NORMAL_REC_STATUS  				(CHDRV_PVR_REC_STATUS_IDLE)
#define  PVR_NORMAL_REPLAY_STATUS  				(CHDRV_PVR_REPLAY_STATUS_IDLE)
#define  PVR_NORMAL_REPLAY_SPEED     			(CHDRV_PVR_REPLAY_SPEED_1)
#define  PVR_NORMAL_REPLAY_TYPE     			(CHDRV_PVR_REPLAY_NORMAL)

typedef enum
{
	PVR_NORMAL_REC_TYPE = 0,						    	
 	PVR_BACKGROUND_REC_TYPE,				    		
	PVR_NORMAL_ENCRYPT_REC_TYPE,					    
	PVR_BACKGROUND_ENCRYPT_REC_TYPE					
}PVR_REC_TYPE_e; 


const char* ResPathName = "C:/TS";

#define PVR_CRYTPKEY_LENGTH					(0)
#define PVR_RECTIME_LEN							(900)
#define LOOP_5								    	(5)

static CHDRV_PVR_Handle 			rh_RECHandle 	= NULL; 
static CHDRV_PVR_Handle 			rh_ReplayHandle = NULL;
static CHDRV_PVR_Handle 			rh_TimeshiftHandle =  NULL;

static CHDRV_PVR_RECStartParam_t 	RecParams; 
static CHDRV_PVR_RECStatus_t 		rpstru_Status_rec;
static CHDRV_PVR_ReplayStatus_t 	rpstru_Status_pb;
static CHDRV_PVR_TimeShiftStatus_t  rpstru_Status_timeshift;

//���ܺ�test_tuner.cpp��parse_key һģһ��������config
static char *parse_conf(char* line, char* key, char* value, int value_len)
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
				if(p[i] >= 'a'&&p[i] <= 'z'
					|| p[i] >= 'A'&&p[i] <= 'Z'
					|| p[i] >= '0'&&p[i] <= '9'
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

/* ��config�ļ���ȡ¼�Ƶ�ts��Ϣ */
static CHDRV_PVR_RESULT_e Set_RECStartParam(U8 uc_RecordType)
{
	CMD_PARAM value;
	char str[256] = " ";
	 memset(&(RecParams),0,sizeof(RecParams));
	if (uc_RecordType == PVR_NORMAL_REC_TYPE)
	{
		//parse
		value = parse_conf(fw_getenv("PVR_TS_DESCRIPS"), "vpid", str, sizeof(str));
		RecParams.stru_RecParam.stru_VideoPID[0].t_PID = value.get_i();
		value = parse_conf(fw_getenv("PVR_TS_DESCRIPS"), "vtype", str, sizeof(str));
    	RecParams.stru_RecParam.stru_VideoPID[0].enm_Type = (CH_VIDEO_CODE_TYPE_e)value.get_i();
		value = parse_conf(fw_getenv("PVR_TS_DESCRIPS"), "apid", str, sizeof(str));
    	RecParams.stru_RecParam.stru_AudioPID[0].t_PID = value.get_i();
		value = parse_conf(fw_getenv("PVR_TS_DESCRIPS"), "atype", str, sizeof(str));
    	RecParams.stru_RecParam.stru_AudioPID[0].enm_Type = (CH_AUDIO_CODE_TYPE_e)value.get_i();
		value = parse_conf(fw_getenv("PVR_TS_DESCRIPS"), "pcr_pid", str, sizeof(str));
    	RecParams.stru_RecParam.stru_OtherPID[0].t_PID = value.get_i();
	}
	else//for Encrypt
	{
		//parse
		//value = Parse_TS(fw_getenv("TS_DESCRIPS"), "vpid", str, sizeof(str));
		//RecParams.stru_RecParam.stru_VideoPID[0].t_PID = value.get_i();
		//value = Parse_TS(fw_getenv("TS_DESCRIPS"), "vtype", str, sizeof(str));
    	//RecParams.stru_RecParam.stru_VideoPID[0].enm_Type = value.get_i();
		//value = Parse_TS(fw_getenv("TS_DESCRIPS"), "apid", str, sizeof(str));
    	//RecParams.stru_RecParam.stru_AudioPID[0].t_PID = value.get_i();
		//value = Parse_TS(fw_getenv("TS_DESCRIPS"), "atype", str, sizeof(str));
    	//RecParams.stru_RecParam.stru_AudioPID[0].enm_Type = value.get_i();
		//value = Parse_TS(fw_getenv("TS_DESCRIPS"), "pcr_pid", str, sizeof(str));
    	//RecParams.stru_RecParam.stru_OtherPID[0].t_PID = value.get_i();
    	value = parse_conf(fw_getenv("PVR_TS_DESCRIPS"), "vpid", str, sizeof(str));
		RecParams.stru_RecParam.stru_VideoPID[0].t_PID = value.get_i();
		value = parse_conf(fw_getenv("PVR_TS_DESCRIPS"), "vtype", str, sizeof(str));
    	RecParams.stru_RecParam.stru_VideoPID[0].enm_Type = (CH_VIDEO_CODE_TYPE_e)value.get_i();
		value = parse_conf(fw_getenv("PVR_TS_DESCRIPS"), "apid", str, sizeof(str));
    	RecParams.stru_RecParam.stru_AudioPID[0].t_PID = value.get_i();
		value = parse_conf(fw_getenv("PVR_TS_DESCRIPS"), "atype", str, sizeof(str));
    	RecParams.stru_RecParam.stru_AudioPID[0].enm_Type = (CH_AUDIO_CODE_TYPE_e)value.get_i();
		value = parse_conf(fw_getenv("PVR_TS_DESCRIPS"), "pcr_pid", str, sizeof(str));
    	RecParams.stru_RecParam.stru_OtherPID[0].t_PID = value.get_i();
	}	
		RecParams.i_DurationS = 700;
    RecParams.puc_ResPathName = (U8*)ResPathName;
    RecParams.stru_RecParam.stru_OtherPID[0].enm_Type = CHDRV_PVR_PCR;
    RecParams.stru_RecParam.uc_AudioPidCount = 1;
    RecParams.stru_RecParam.uc_OtherPidCount = 1;
    RecParams.stru_RecParam.uc_VideoPidCount = 1;	
    RecParams.uc_RecordType = uc_RecordType;
    memset( RecParams.uc_CryptKeyBuf,0 ,PVR_CRYPTKEY_MAX_LENGTH);
    RecParams.uc_CryptKeyLength = PVR_CRYTPKEY_LENGTH;

	return CHDRV_PVR_OK;
}
#if 0
static CHDRV_PVR_RESULT_e Auto_Play_DTV()
{
	CHDRV_TUNER_Init();
	RUN_CMD("tuner.lock %s", fw_getenv("TUNER_PARAMS4"));
	RUN_CMD("av.start %s",fw_getenv("PVR_DTV_PROGNUM"));			//���Ž�Ŀ

	return CHDRV_PVR_OK;
}
#else
static CHDRV_PVR_RESULT_e Auto_Play_DTV()
{
int prognum = 0;

#ifdef CFG_CHSSA_HD_SUPPORT
prognum = request_play_program("HD_CHANNEL_TEST", "TUNER_PARAMS1");
#else
prognum = request_play_program("SD_CHANNEL_TEST", "TUNER_PARAMS1");
#endif

RUN_CMD("tuner.lock %s", fw_getenv("TUNER_PARAMS1"));
RUN_CMD("av.start %d %d\n",fw_getenv_i("TUNER_DEVICE_ID"),prognum);	//���Ž�Ŀ

return CHDRV_PVR_OK;
}

#endif
static CHDRV_PVR_RESULT_e Auto_Stop_DTV()
{
	RUN_CMD("av.stop %d",0);		
	CHDRV_TUNER_Term();
	
	return CHDRV_PVR_OK;
}
//¼�ƽ����ͷ���Դ
static CHDRV_PVR_RESULT_e Rec_Res_Term()
{
	rh_RECHandle = NULL;
	return CHDRV_PVR_OK;
}
/*�طŽ����ͷ���Դ�����´�¼�ƿ�������*/
static CHDRV_PVR_RESULT_e Replay_Res_Term()
{
	rh_ReplayHandle = NULL;
	CH_FSYS_DeleteFile((const S8 *)"/tmp/root_0_0/TS/record.ts",NULL,NULL);
	CH_FSYS_DeleteFile((const S8 *)"/tmp/root_0_0/TS/record.ts.meta",NULL,NULL);
	
	return CHDRV_PVR_OK;
}
/*ʱ�ƽ����ͷ���Դ�����´�ʱ�ƿ�������*/
static CHDRV_PVR_RESULT_e Timeshift_Res_Term()
{
	rh_TimeshiftHandle = NULL;
	CH_FSYS_DeleteFile((const S8 *)"/tmp/root_0_0/TS/record.ts",NULL,NULL);
	CH_FSYS_DeleteFile((const S8 *)"/tmp/root_0_0/TS/record.ts.meta",NULL,NULL);
	
	return CHDRV_PVR_OK;
}

void RecordCB( CHDRV_PVR_Handle  rh_Handle,CHDRV_PVR_REC_EVENT_e renm_RecEvent)
{
	if (rh_Handle == NULL)
	{
		printf("rh_Handle is null\n");
		return ;
	}
	switch(renm_RecEvent)
	{
		case CHDRV_PVR_REC_EVENT_STOP:
			
			printf("ֹͣ¼��\n");
			break;
		
		case CHDRV_PVR_REC_EVENT_PAUSE:	
			
			printf("��ͣ\n");
			break;
		case CHDRV_PVR_REC_EVENT_PROCESS:
			
			printf("¼�ƹ�����\n");
			break;
		
		case CHDRV_PVR_REC_EVENT_NODATA:	
			
			printf("ǰ��û�����ݹ���\n");
			break;
		case CHDRV_PVR_REC_EVENT_DATA_RESUME:
			
			printf("ǰ�����ݻָ�\n");
			break;
		
		case CHDRV_PVR_REC_EVENT_DISKFULL:	
			
			printf(" û��¼�ƿռ�,ʣ��ռ�Ϊ0Byte\n");
			break;

		case CHDRV_PVR_REC_EVENT_DISKFULL_WARNING:
			
			printf("����ʣ��ռ䵽�ﾯ����\n");
			break;
		
		case CHDRV_PVR_REC_EVENT_ERROR:	
			
			printf("����������¼�Ʋ��ܼ���\n");
			break;
		case CHDRV_PVR_REC_EVENT_DEVICE_REMOVED:
			
			printf("�洢�豸���Ƴ�\n");
			break;
		
		case CHDRV_PVR_REC_EVENT_TIMEOUT:	
			
			printf("��ʱ¼��ʱ�䵽\n");
			break;
		case CHDRV_PVR_REC_EVENT_FILE_WRITE_ERROR:
			
			printf("�ļ�д����\n");
			break;
		
		case CHDRV_PVR_REC_EVENT_WRITE_SPEEDLESS:	
			
			printf("Ӳ���ļ�д�ٶȲ���\n");
			break;
		default:

			printf("unknown event\n");
            return ;
	
	}
	return ;
}

void ReplayCB( CHDRV_PVR_Handle  rh_Handle,
               CHDRV_PVR_REPLAY_EVENT_e renm_ReplayEvent)
{
   if (rh_Handle == NULL)
	{
		printf("rh_Handle is null\n");
		return ;
	}
	switch(renm_ReplayEvent)
	{
	
		case CHDRV_PVR_REPLAY_EVENT_STOP:
			
			printf("ֹͣ�ط�\n");
			break;
		
		case CHDRV_PVR_REPLAY_EVENT_PAUSE:	
			
			printf("��ͣ�ط�\n");
			break;
		case CHDRV_PVR_REPLAY_EVENT_PLAYING:
			
			printf("�طŹ�����\n");
			break;
		
		case CHDRV_PVR_REPLAY_EVENT_FILE_TAIL:
			
			printf("ǰ�����ݻָ�\n");
			break;
		
		case CHDRV_PVR_REPLAY_EVENT_FILE_HEAD:	
			
			printf(" ����ʱ�����ļ�ͷ����תΪ��������\n");
			break;

		case CHDRV_PVR_REPLAY_EVENT_FILE_READ_ERROR:
			
			printf("�ļ�������\n");
			break;
		
		case CHDRV_PVR_REPLAY_EVENT_ERROR:	
			
			printf("���������²��Ų��ɹ�\n");
			break;
		case CHDRV_PVR_REPLAY_EVENT_DEVICE_REMOVED:
			
			printf("�洢�豸���Ƴ�\n");
			break;
		default:
			printf("unknown event\n");
            return ;
	
	}	
	return ;
}

void TimeShiftCB( CHDRV_PVR_Handle  rh_Handle,
                  CHDRV_PVR_REPLAY_EVENT_e renm_ReplayEvent)
{
   	if (rh_Handle == NULL)
	{
		printf("rh_Handle is null\n");
		return ;
	}
	switch(renm_ReplayEvent)
	{
		case CHDRV_PVR_REPLAY_EVENT_STOP:
			
			printf("ֹͣ�ط�\n");
			break;
		
		case CHDRV_PVR_REPLAY_EVENT_PAUSE:	
			
			printf("��ͣ�ط�\n");
			break;
		case CHDRV_PVR_REPLAY_EVENT_PLAYING:
			
			printf("�طŹ�����\n");
			break;
		
		case CHDRV_PVR_REPLAY_EVENT_FILE_TAIL:
			
			printf("ǰ�����ݻָ�\n");
			break;
		
		case CHDRV_PVR_REPLAY_EVENT_FILE_HEAD:	
			
			printf(" ����ʱ�����ļ�ͷ����תΪ��������\n");
			break;

		case CHDRV_PVR_REPLAY_EVENT_FILE_READ_ERROR:
			
			printf("�ļ�������\n");
			break;
		
		case CHDRV_PVR_REPLAY_EVENT_ERROR:	
			
			printf("���������²��Ų��ɹ�\n");
			break;
		case CHDRV_PVR_REPLAY_EVENT_DEVICE_REMOVED:
			
			printf("�洢�豸���Ƴ�\n");
			break;
		default:
			printf("unknown event\n");
            return ;
	
	}
	return ;
}

/*��ʼ����֮ǰ���Զ�����DTV������rec ��Ϣ*/
void CASE_NAME::SetUpTestCase() {
//excute before each test suit
	std::cout << "init SetUp pvr TestCase " << std::endl;

	
	Auto_Play_DTV();
	CH_FSYS_Init();
	Set_RECStartParam(PVR_NORMAL_REC_TYPE);//��ͨ¼��
	CH_FSYS_DeleteFile((const S8 *)"/tmp/root_0_0/TS/record.ts",NULL,NULL);
	CH_FSYS_DeleteFile((const S8 *)"/tmp/root_0_0/TS/record.ts.meta",NULL,NULL);
	CH_FSYS_DeleteFile((const S8 *)"/tmp/root_0_0/TS/record.ts.pmt",NULL,NULL);
	CH_FSYS_DeleteFile((const S8 *)"/tmp/root_0_0/TS/record.ts.idx",NULL,NULL);
	
}

/*�������ԣ��Զ�ֹͣDTV*/
void CASE_NAME::TearDownTestCase() {
//excute after each test suit
	std::cout << "deinit pvr TestCase " << std::endl;

	CHDRV_PVR_Term();
	Auto_Stop_DTV();
	CH_FSYS_Term();
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

// 2~32   (���ΪFPE����ϵͳ_����ģ���������V0.1�Ĳ����������)
#if 1
CASE_F(TEST_PVR_INIT1,
	"���Զ�γ�ʼ��ʱ�Ƿ񷵻�OK") {
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Init());
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Init()) << "��γ�ʼ��Ӧ�÷���OK";
}

CASE_F(TEST_PVR_INIT2,
	"����ģ���Ƿ�ɱ�����") {
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Init());
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Term());
}


CASE_F(TEST_PVR_INIT3,
	"����CHDRV_PVR_GetVersion�Ƿ�ɹ�") {
	CHDRV_Version_t stru_Ver;
	
	memset(&stru_Ver, 0, sizeof(stru_Ver));
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetVersion(&stru_Ver));
	EXPECT_NE(0, stru_Ver);
	LOG("\nstru_Ver=%x\n", stru_Ver);
}

CASE_F(TEST_PVR_INIT4,
	"����û�г�ʼ��ʱ�����������Ƿ��ܵ��óɹ�") {

	CHDRV_PVR_Handle rph_RECHandle = NULL;
	CHDRV_PVR_RECStartParam_t rpstru_RecParam;

	CHDRV_PVR_Handle rh_ReplayHandle = NULL;
	S32 ri_DstTimeSm = 0;

	CHDRV_PVR_Handle rph_TimeShiftHandle = NULL;
	CHDRV_PVR_RECStartParam_t rpstru_TimeshiftParam;
	CHDRV_PVR_TimeShiftStatus_t rpstru_TimeShiftStatus;

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Term());
	
	EXPECT_EQ(CHDRV_PVR_NOT_INIT, CHDRV_PVR_RECOpen(NULL,NULL,&rph_RECHandle));
	EXPECT_EQ(CHDRV_PVR_NOT_INIT, CHDRV_PVR_RECStart(rph_RECHandle,&rpstru_RecParam));
	
	EXPECT_EQ(CHDRV_PVR_NOT_INIT, CHDRV_PVR_RECControl(rph_RECHandle,PVR_NORMAL_REC_CMD));
	EXPECT_EQ(CHDRV_PVR_NOT_INIT, CHDRV_PVR_GetRECStatus(rph_RECHandle,&rpstru_Status_rec));
	EXPECT_EQ(CHDRV_PVR_NOT_INIT, CHDRV_PVR_RECStop(rph_RECHandle));
	EXPECT_EQ(CHDRV_PVR_NOT_INIT, CHDRV_PVR_RECClose(rph_RECHandle));
	
	EXPECT_EQ(CHDRV_PVR_NOT_INIT, CHDRV_PVR_ReplayOpen(NULL,NULL,&rph_RECHandle));
	EXPECT_EQ(CHDRV_PVR_NOT_INIT, CHDRV_PVR_ReplayControl(rh_ReplayHandle,PVR_NORMAL_REPLAY_CMD));
	EXPECT_EQ(CHDRV_PVR_NOT_INIT, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,PVR_NORMAL_REPLAY_TYPE,PVR_NORMAL_REPLAY_SPEED));
	EXPECT_EQ(CHDRV_PVR_NOT_INIT, CHDRV_PVR_ReplaySeek(rh_ReplayHandle,ri_DstTimeSm));
	EXPECT_EQ(CHDRV_PVR_NOT_INIT, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
	EXPECT_EQ(CHDRV_PVR_NOT_INIT, CHDRV_PVR_ReplayStop(rh_ReplayHandle));
	EXPECT_EQ(CHDRV_PVR_NOT_INIT, CHDRV_PVR_ReplayClose(rh_ReplayHandle));
	
	EXPECT_EQ(CHDRV_PVR_NOT_INIT, CHDRV_PVR_TimeShiftOpen(NULL,NULL,NULL,&rph_TimeShiftHandle));
	EXPECT_EQ(CHDRV_PVR_NOT_INIT, CHDRV_PVR_TimeShiftStart(rph_TimeShiftHandle,&rpstru_TimeshiftParam));
	EXPECT_EQ(CHDRV_PVR_NOT_INIT, CHDRV_PVR_TimeShiftReplayControl(rph_TimeShiftHandle,PVR_NORMAL_REPLAY_CMD));
	EXPECT_EQ(CHDRV_PVR_NOT_INIT, CHDRV_PVR_TimeShiftSpeedReplay(rph_TimeShiftHandle,PVR_NORMAL_REPLAY_TYPE,PVR_NORMAL_REPLAY_SPEED));
	EXPECT_EQ(CHDRV_PVR_NOT_INIT, CHDRV_PVR_TimeShiftGetStatus(rph_TimeShiftHandle,&rpstru_TimeShiftStatus));
	EXPECT_EQ(CHDRV_PVR_NOT_INIT, CHDRV_PVR_TimeShiftStop(rph_TimeShiftHandle));
	EXPECT_EQ(CHDRV_PVR_NOT_INIT, CHDRV_PVR_TimeShiftClose(rph_TimeShiftHandle));

}

CASE_F(TEST_PVR_INIT5,
	"���Ե���CHDRV_PVR_Term���ٵ������������Ƿ��ܳɹ�") {

	CHDRV_PVR_Handle rph_RECHandle = NULL;
	CHDRV_PVR_RECStartParam_t rpstru_RecParam;

	CHDRV_PVR_Handle rh_ReplayHandle = NULL;
	S32 ri_DstTimeSm = 0;
	CHDRV_PVR_ReplayStatus_t rpstru_Status_pb;

	CHDRV_PVR_Handle rph_TimeShiftHandle = NULL;
	CHDRV_PVR_RECStartParam_t rpstru_TimeshiftParam;
	CHDRV_PVR_TimeShiftStatus_t rpstru_TimeShiftStatus;

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Init());
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Term());
		
	EXPECT_EQ(CHDRV_PVR_NOT_INIT, CHDRV_PVR_RECOpen(NULL,NULL,&rph_RECHandle));
	EXPECT_EQ(CHDRV_PVR_NOT_INIT, CHDRV_PVR_RECStart(rph_RECHandle,&rpstru_RecParam));
	
	EXPECT_EQ(CHDRV_PVR_NOT_INIT, CHDRV_PVR_RECControl(rph_RECHandle,PVR_NORMAL_REC_CMD));
	EXPECT_EQ(CHDRV_PVR_NOT_INIT, CHDRV_PVR_GetRECStatus(rph_RECHandle,&rpstru_Status_rec));
	EXPECT_EQ(CHDRV_PVR_NOT_INIT, CHDRV_PVR_RECStop(rph_RECHandle));
	EXPECT_EQ(CHDRV_PVR_NOT_INIT, CHDRV_PVR_RECClose(rph_RECHandle));
	
	EXPECT_EQ(CHDRV_PVR_NOT_INIT, CHDRV_PVR_ReplayOpen(NULL,NULL,&rph_RECHandle));
	EXPECT_EQ(CHDRV_PVR_NOT_INIT, CHDRV_PVR_ReplayControl(rh_ReplayHandle,PVR_NORMAL_REPLAY_CMD));
	EXPECT_EQ(CHDRV_PVR_NOT_INIT, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,PVR_NORMAL_REPLAY_TYPE,PVR_NORMAL_REPLAY_SPEED));
	EXPECT_EQ(CHDRV_PVR_NOT_INIT, CHDRV_PVR_ReplaySeek(rh_ReplayHandle,ri_DstTimeSm));
	EXPECT_EQ(CHDRV_PVR_NOT_INIT, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
	EXPECT_EQ(CHDRV_PVR_NOT_INIT, CHDRV_PVR_ReplayStop(rh_ReplayHandle));
	EXPECT_EQ(CHDRV_PVR_NOT_INIT, CHDRV_PVR_ReplayClose(rh_ReplayHandle));
	
	EXPECT_EQ(CHDRV_PVR_NOT_INIT, CHDRV_PVR_TimeShiftOpen(NULL,NULL,NULL,&rph_TimeShiftHandle));
	EXPECT_EQ(CHDRV_PVR_NOT_INIT, CHDRV_PVR_TimeShiftStart(rph_TimeShiftHandle,&rpstru_TimeshiftParam));
	EXPECT_EQ(CHDRV_PVR_NOT_INIT, CHDRV_PVR_TimeShiftReplayControl(rph_TimeShiftHandle,PVR_NORMAL_REPLAY_CMD));
	EXPECT_EQ(CHDRV_PVR_NOT_INIT, CHDRV_PVR_TimeShiftSpeedReplay(rph_TimeShiftHandle,PVR_NORMAL_REPLAY_TYPE,PVR_NORMAL_REPLAY_SPEED));
	EXPECT_EQ(CHDRV_PVR_NOT_INIT, CHDRV_PVR_TimeShiftGetStatus(rph_TimeShiftHandle,&rpstru_TimeShiftStatus));
	EXPECT_EQ(CHDRV_PVR_NOT_INIT, CHDRV_PVR_TimeShiftStop(rph_TimeShiftHandle));
	EXPECT_EQ(CHDRV_PVR_NOT_INIT, CHDRV_PVR_TimeShiftClose(rph_TimeShiftHandle));

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Init());// Ϊ����������׼��
	

}


CASE_F(TEST_PVR_PARAMS1,
	"CHDRV_PVR_SetConfig�����������") {

	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_SetConfig(NULL));
}


CASE_F(TEST_PVR_PARAMS2,
	"CHDRV_PVR_GetConfig�����������") {

	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_GetConfig(NULL)); //
}

CASE_F(TEST_PVR_PARAMS3,
	"CHDRV_PVR_ResIsIntact�����������") {
	//EXPECT_EQ(CHDRV_PVR_NOT_SUPPORT, CHDRV_PVR_ResIsIntact(NULL)); // porting not support
}

CASE_F(TEST_PVR_PARAMS4,
	"CHDRV_PVR_GetResInfo�����������") {
	//EXPECT_EQ(CHDRV_PVR_NOT_SUPPORT, CHDRV_PVR_GetResInfo(NULL,NULL,NULL,NULL)); // porting not support
	//EXPECT_EQ(CHDRV_PVR_NOT_SUPPORT, CHDRV_PVR_GetResInfo(NULL,NULL,NULL,NULL));
	//EXPECT_EQ(CHDRV_PVR_NOT_SUPPORT, CHDRV_PVR_GetResInfo(NULL,NULL,NULL,NULL));
	//EXPECT_EQ(CHDRV_PVR_NOT_SUPPORT, CHDRV_PVR_GetResInfo(NULL,NULL,NULL,NULL));
}

CASE_F(TEST_PVR_PARAMS5,
	"CHDRV_PVR_GetResInfo�����������") {
	//EXPECT_EQ(CHDRV_PVR_NOT_SUPPORT, CHDRV_PVR_RepairResInfo(NULL,NULL)); // porting not support
	//EXPECT_EQ(CHDRV_PVR_NOT_SUPPORT, CHDRV_PVR_RepairResInfo(NULL,NULL)); 
}



CASE_F(TEST_PVR_PARAMS6,
	"CHDRV_PVR_RECOpen�����������") {

	//CHDRV_ROUTE_Handle rh_RouteHandle;
	//CHDRV_PVR_Handle rh_RECHandle ;
	//EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_RECOpen(NULL,NULL,&rh_RECHandle)); //���2���������
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_RECOpen(NULL,(CHDRV_PVR_REC_CALLBACK)RecordCB,NULL));//portingӦ��ʵ�ּ��null handle
}


CASE_F(TEST_PVR_PARAMS7,
	"CHDRV_PVR_RECStart�����������") {

	CHDRV_PVR_Handle rh_RECHandle = NULL;
	CHDRV_PVR_RECStartParam_t rpstru_RecParam;
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECOpen(NULL,(CHDRV_PVR_REC_CALLBACK)RecordCB,&rh_RECHandle)); 
	
	
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_RECStart(rh_RECHandle,NULL)); //����1���������
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_RECStart(NULL,&rpstru_RecParam)); //����2���������
	//free
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECClose(rh_RECHandle));
	
}


CASE_F(TEST_PVR_PARAMS8,
	"CHDRV_PVR_RECControl�����������") {

	CHDRV_PVR_Handle rh_RECHandle = NULL;
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECOpen(NULL,(CHDRV_PVR_REC_CALLBACK)RecordCB,&rh_RECHandle)); 
	
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_RECControl(NULL,PVR_NORMAL_REC_CMD)); //����1���������
	EXPECT_EQ(CHDRV_PVR_FAIL, CHDRV_PVR_RECControl(rh_RECHandle,PVR_ERRO_REC_CMD)); //����2���������

	//free
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECClose(rh_RECHandle));
}

CASE_F(TEST_PVR_PARAMS9,
	"CHDRV_PVR_GetRECStatus�����������") {

	CHDRV_PVR_Handle rh_RECHandle = NULL;
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECOpen(NULL,(CHDRV_PVR_REC_CALLBACK)RecordCB,&rh_RECHandle)); 
		
	CHDRV_PVR_RECStatus_t rpstru_Status;
	
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_GetRECStatus(rh_RECHandle,NULL)); //����1���������
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_GetRECStatus(NULL,&rpstru_Status)); //����2���������

	//�ͷž��
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECClose(rh_RECHandle));
}

CASE_F(TEST_PVR_PARAMS10,
	"CHDRV_PVR_RECStop�����������") {

	CHDRV_PVR_Handle rh_RECHandle = NULL;
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECOpen(NULL,(CHDRV_PVR_REC_CALLBACK)RecordCB,&rh_RECHandle));
	
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_RECStop(NULL)); //����1���������

	//�ͷž��
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECClose(rh_RECHandle));
}

CASE_F(TEST_PVR_PARAMS11,
	"CHDRV_PVR_RECClose�����������") {

	CHDRV_PVR_Handle rh_RECHandle = NULL;
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECOpen(NULL,(CHDRV_PVR_REC_CALLBACK)RecordCB,&rh_RECHandle));//�����¿������Ƿ�ִ��g_RecClosed = CH_FALSE
	
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_RECClose(NULL)); 
	
	//��_RecClosed == CH_TRUE���أ������ͷ�rh_RECHandle��open �ɹ�֮��closeӦ��ҲҪ�ɹ���replay��timeshift ͬ��
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECClose(rh_RECHandle));
}



CASE_F(TEST_PVR_PARAMS12,
	"CHDRV_PVR_RECClose�����������") {

	CHDRV_PVR_Handle rph_ReplayHandle = NULL;
	
	//EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_ReplayOpen(NULL,(CHDRV_PVR_REPLAY_CALLBACK)ReplayCB,&rph_ReplayHandle)); 
	//EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_ReplayOpen(NULL,NULL,&rph_ReplayHandle));
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_ReplayOpen(NULL,(CHDRV_PVR_REPLAY_CALLBACK)ReplayCB,NULL));//portingӦ��ʵ�ּ��null handle
}


CASE_F(TEST_PVR_PARAMS13,
	"CHDRV_PVR_ReplayStart�����������") {

	CHDRV_PVR_Handle rh_ReplayHandle = NULL;
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayOpen(NULL,ReplayCB,&rh_ReplayHandle));
	
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_ReplayStart(NULL,(U8*)ResPathName,NULL,PVR_CRYTPKEY_LENGTH)); //portingӦ��ʵ�ּ��null handle

	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_ReplayStart(rh_ReplayHandle,NULL,NULL,PVR_CRYTPKEY_LENGTH)); //����2���������

	//�ͷž��
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayClose(rh_ReplayHandle));
}



CASE_F(TEST_PVR_PARAMS14,
	"CHDRV_PVR_ReplayControl�����������") {

	CHDRV_PVR_Handle rh_ReplayHandle = NULL;
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayOpen(NULL,ReplayCB,&rh_ReplayHandle));
	
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_ReplayControl(NULL,PVR_NORMAL_REPLAY_CMD)); //����1���������
	EXPECT_EQ(CHDRV_PVR_FAIL, CHDRV_PVR_ReplayControl(rh_ReplayHandle,PVR_ERRO_REPLAY_CMD)); //����2���������

	//�ͷž��
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayClose(rh_ReplayHandle));
}


CASE_F(TEST_PVR_PARAMS15,
	"CHDRV_PVR_SpeedReplay�����������") {

	CHDRV_PVR_Handle rh_ReplayHandle = NULL;
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayOpen(NULL,ReplayCB,&rh_ReplayHandle));
	
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_SpeedReplay(NULL,PVR_NORMAL_REPLAY_TYPE,PVR_NORMAL_REPLAY_SPEED)); //����1���������
	EXPECT_EQ(CHDRV_PVR_FAIL, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,PVR_ERRO_REPLAY_TYPE,PVR_NORMAL_REPLAY_SPEED)); //����2���������
	EXPECT_EQ(CHDRV_PVR_FAIL, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,PVR_NORMAL_REPLAY_TYPE,PVR_ERRO_REPLAY_SPEED)); //����3���������

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayClose(rh_ReplayHandle));
}

CASE_F(TEST_PVR_PARAMS16,
	"CHDRV_PVR_ReplaySeek�����������") {

	S32 ri_DstTimeS = 0;
	
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_ReplaySeek(NULL,ri_DstTimeS)); //����1���������
	//EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_ReplaySeek(rh_ReplayHandle,-10));

}


CASE_F(TEST_PVR_PARAMS17,
	"CHDRV_PVR_GetReplayStatus�����������") {

	CHDRV_PVR_Handle rh_ReplayHandle = NULL;
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayOpen(NULL,ReplayCB,&rh_ReplayHandle));
	
	CHDRV_PVR_ReplayStatus_t  rpstru_Status;
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_GetReplayStatus(NULL,&rpstru_Status)); //����1���������
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,NULL)); //����2���������

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayClose(rh_ReplayHandle));
}


CASE_F(TEST_PVR_PARAMS18,
	"CHDRV_PVR_ReplayStop�����������") {

	CHDRV_PVR_Handle rh_ReplayHandle = NULL;
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayOpen(NULL,ReplayCB,&rh_ReplayHandle));
	
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_ReplayStop(NULL)); 

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayClose(rh_ReplayHandle));
	
	
}

CASE_F(TEST_PVR_PARAMS19,
	"CHDRV_PVR_ReplayClose�����������") {

	CHDRV_PVR_Handle rh_ReplayHandle = NULL;
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayOpen(NULL,ReplayCB,&rh_ReplayHandle));
	
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_ReplayClose(NULL));

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayClose(rh_ReplayHandle));
	
}


CASE_F(TEST_PVR_PARAMS20,
	"CHDRV_PVR_TimeShiftOpen�����������") {

	CHDRV_PVR_Handle rh_TimeShiftHandle = NULL;
	
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_TimeShiftOpen(NULL,NULL,(CHDRV_PVR_TIMESHIFT_CALLBACK)TimeShiftCB,NULL));//core dump  because null handle
	//EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_TimeShiftOpen(NULL,NULL,NULL,&rh_TimeShiftHandle));

	
}

CASE_F(TEST_PVR_PARAMS21,
	"CHDRV_PVR_TimeShiftStart�����������") {

	CHDRV_PVR_RECStartParam_t rpstru_TimeshiftParam;
	CHDRV_PVR_Handle rh_TimeShiftHandle = NULL;
	//Replay is opened!
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftOpen(NULL,NULL,(CHDRV_PVR_TIMESHIFT_CALLBACK)TimeShiftCB,&rh_TimeShiftHandle)); 
	
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_TimeShiftStart(NULL,&rpstru_TimeshiftParam)); //����1���������
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_TimeShiftStart(rh_TimeShiftHandle,NULL)); //����2���������

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftClose(rh_TimeShiftHandle)); 
	
}
CASE_F(TEST_PVR_PARAMS22,
	"CHDRV_PVR_TimeShiftReplayControl�����������") {

	CHDRV_PVR_Handle rh_TimeShiftHandle = NULL;
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftOpen(NULL,NULL,(CHDRV_PVR_TIMESHIFT_CALLBACK)TimeShiftCB,&rh_TimeShiftHandle)); //Replay is opened!
		
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_TimeShiftReplayControl(NULL,PVR_NORMAL_REPLAY_CMD)); //����1���������
	EXPECT_EQ(CHDRV_PVR_FAIL, CHDRV_PVR_TimeShiftReplayControl(rh_TimeShiftHandle,PVR_ERRO_REPLAY_CMD)); //����2���������
	printf("\nrh_TimeShiftHandle=0x%x\n",rh_TimeShiftHandle);
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftClose(rh_TimeShiftHandle)); 
	
}

CASE_F(TEST_PVR_PARAMS23,
	"CHDRV_PVR_TimeShiftReplayControl�����������") {

	CHDRV_PVR_Handle rh_TimeShiftHandle = NULL;
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftOpen(NULL,NULL,(CHDRV_PVR_TIMESHIFT_CALLBACK)TimeShiftCB,&rh_TimeShiftHandle)); 		

	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_TimeShiftSpeedReplay(NULL,PVR_NORMAL_REPLAY_TYPE,CHDRV_PVR_REPLAY_SPEED_1)); //����1���������
	EXPECT_EQ(CHDRV_PVR_FAIL, CHDRV_PVR_TimeShiftSpeedReplay(rh_TimeShiftHandle,PVR_ERRO_REPLAY_TYPE,CHDRV_PVR_REPLAY_SPEED_1)); //����2���������
	EXPECT_EQ(CHDRV_PVR_FAIL, CHDRV_PVR_TimeShiftSpeedReplay(rh_TimeShiftHandle,PVR_NORMAL_REPLAY_TYPE,PVR_ERRO_REPLAY_SPEED)); //

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftClose(rh_TimeShiftHandle)); 
	
}

CASE_F(TEST_PVR_PARAMS24,
	"CHDRV_PVR_TimeShiftGetStatus�����������") {

	CHDRV_PVR_Handle rh_TimeShiftHandle = NULL;
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftOpen(NULL,NULL,(CHDRV_PVR_TIMESHIFT_CALLBACK)TimeShiftCB,&rh_TimeShiftHandle)); //����3���������		

	CHDRV_PVR_TimeShiftStatus_t rpstru_TimeShiftStatus;
		
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_TimeShiftGetStatus(NULL,&rpstru_TimeShiftStatus)); //����1���������
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_TimeShiftGetStatus(rh_TimeShiftHandle,NULL)); //����2���������

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftClose(rh_TimeShiftHandle)); 
	
}

CASE_F(TEST_PVR_PARAMS25,
	"CHDRV_PVR_TimeShiftStop�����������") {

	CHDRV_PVR_Handle rh_TimeShiftHandle = NULL;
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftOpen(NULL,NULL,(CHDRV_PVR_TIMESHIFT_CALLBACK)TimeShiftCB,&rh_TimeShiftHandle)); 
			
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_TimeShiftStop(NULL)); 

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftClose(rh_TimeShiftHandle)); 
}

CASE_F(TEST_PVR_PARAMS26,
	"CHDRV_PVR_TimeShiftClose�����������") {
	
	CHDRV_PVR_Handle rh_TimeShiftHandle = NULL;
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftOpen(NULL,NULL,(CHDRV_PVR_TIMESHIFT_CALLBACK)TimeShiftCB,&rh_TimeShiftHandle));
	
	//��Ҫ����ִ��g_TimeShiftClosed = false�ĵط�����ΪCHDRV_PVR_TimeShiftOpen�Ѿ�ִ��,rh_TimeShiftHandle��Ϊ��
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_TimeShiftClose(NULL)); 
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftClose(rh_TimeShiftHandle)); //��g_TimeShiftClosed���أ������ͷ�rh_TimeShiftHandle
}
#endif

// 33~44
#if 1
CASE_F(TEST_PVR_REC1,
	"����rec open����һ����������Ƿ񷵻سɹ����Ҿ����ΪNULL") {

	Set_RECStartParam(PVR_NORMAL_REC_TYPE);
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Init());
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECOpen(NULL,RecordCB,&rh_RECHandle)); 
}

CASE_F(TEST_PVR_REC2,
	"����get rec status״̬ӦΪIDL") {

	CHDRV_PVR_RECStatus_t rpstru_Status;	 
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status));
	EXPECT_EQ(CHDRV_PVR_REC_STATUS_IDLE, rpstru_Status.enm_RECStatus); 
}

CASE_F(TEST_PVR_REC3,
	"����ȷ�Ĳ���start rec�ոմ�����handle���ļ����·����env��ȡ��¼�ƹ���������Ƶ��Ȼ��������") {

    EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECStart(rh_RECHandle,&RecParams));

}

CASE_F(TEST_PVR_REC4,
	"����get rec status����õ�״̬ӦΪRECORDING,����ʱʱ����¼�Ƶ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	LOG("¼��һ��Լ15���ӵ���Ƶ,�����ĵȺ�\n"); 
	/*rh_RECHandle δ����������������ʱ��� ��Щ�ͺ�*/
	for(i_Loop = 0;i_Loop < PVR_RECTIME_LEN;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status_rec));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_rec.enm_RECStatus);
		/*���ڶ��̵߳�ԭ��timeż��delay 1 s */
		if (i_Loop < 10)//���Խ׶ο���ֻ��ӡǰ�������Ϣ
		{
			LOG("��ǰ¼��ʱ��: %d\n", rpstru_Status_rec.i_RECTimeS);
			LOG("��ǰ¼�ƴ�С: %d\n", rpstru_Status_rec.i_RECFileLengthKB);
		}

		CH_RTOS_TaskDelay(1000);
	}
	
}

CASE_F(TEST_PVR_REC5,
	"����rec control��ͣ¼�ƣ�¼����ͣ����������Ƶ��Ȼ��������") {
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECControl(rh_RECHandle,CHDRV_PVR_REC_PAUSE));
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status_rec));
	printf("\ni_RECFileLengthKB=%d,i_RECTimeS=%d\n",rpstru_Status_rec.i_RECFileLengthKB,rpstru_Status_rec.i_RECTimeS);
	
	LOG("��ͣ¼��10s\n");
	CH_RTOS_TaskDelay(10*1000);
	
}

CASE_F(TEST_PVR_REC6,
	"����get rec status����õ�״̬ӦΪPAUSE,¼�Ƶ�ʱ�����ļ���С������֮ͣǰ����") {

	CHDRV_PVR_RECStatus_t rpstru_Status;
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status));
	EXPECT_EQ(CHDRV_PVR_REC_STATUS_PAUSE, rpstru_Status.enm_RECStatus); 
	EXPECT_EQ(rpstru_Status.i_RECFileLengthKB, rpstru_Status_rec.i_RECFileLengthKB);
	EXPECT_EQ(rpstru_Status.i_RECTimeS, rpstru_Status_rec.i_RECTimeS);
}


CASE_F(TEST_PVR_REC7,
	"�ظ�����rec control��ͣ¼�ƣ�Ӧ����ok") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECControl(rh_RECHandle,CHDRV_PVR_REC_PAUSE));	
}

CASE_F(TEST_PVR_REC8,
	"����rec control�ָ�¼�ƣ�¼�ƹ���������Ƶ��Ȼ��������") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECControl(rh_RECHandle,CHDRV_PVR_REC_RESUME));
	
}
CASE_F(TEST_PVR_REC9,
	"����get rec status����õ�״̬ӦΪRECORDING,����ʱʱ����¼�Ƶ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status_rec));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_rec.enm_RECStatus);
		LOG("��ǰ¼��ʱ��: %d\n", rpstru_Status_rec.i_RECTimeS);
		LOG("��ǰ¼�ƴ�С: %d\n", rpstru_Status_rec.i_RECFileLengthKB);
		CH_RTOS_TaskDelay(1000);
	} 
}

#if 1
CASE_F(TEST_PVR_REC10,
	"�ظ�����rec control�ָ�¼�ƣ�Ӧ����ok") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECControl(rh_RECHandle,CHDRV_PVR_REC_RESUME));
	
}
#endif

CASE_F(TEST_PVR_REC11,
	"����rec stop������¼������ֹͣ��") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECStop(rh_RECHandle));
	
}
CASE_F(TEST_PVR_REC12,
	"����rec close����������OK��") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECClose(rh_RECHandle));
	Rec_Res_Term();
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Term());
}
#if 0
CASE_F(TEST_PVR_GET_RES_INFO,
	"����get res info������ȷ��ȡ���ո���¼�ƵĽ�Ŀ����ϸ��Ϣ") {

	EXPECT_EQ(CHDRV_PVR_NOT_SUPPORT, CHDRV_PVR_GetResInfo(NULL,NULL,NULL,NULL));//not complete
}
#endif
#endif

//46~57

#if 1
CASE_F(TEST_PVR_REPLAY1,
	"����replay open����һ����������Ƿ񷵻سɹ����Ҿ����ΪNULL(route������test routeģ���ȡ)") {

	Auto_Stop_DTV();//��ʱ�취����Ϊporting δʵ���Զ��ر�DTV
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Init());
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayOpen(NULL,ReplayCB,&rh_ReplayHandle));
}

CASE_F(TEST_PVR_REPLAY2,
	"����get replay status״̬ӦΪIDL") {
 
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
	EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_IDLE, rpstru_Status_pb.enm_PlayStatus); 
}

CASE_F(TEST_PVR_REPLAY3,
	"����ȷ�Ĳ���start replay�ոմ�����handle���ļ�����·����env��ȡ���طŹ���������Ƶ��������") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayStart(rh_ReplayHandle, (U8*)ResPathName, NULL, 0));

}

CASE_F(TEST_PVR_REPLAY4,
	"����get replay status����õ�״̬ӦΪPLAYING,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_pb.enm_PlayStatus); 
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		
		
	} 
}

CASE_F(TEST_PVR_REPLAY5,
	"����replay control��ͣ�طţ�����ƵӦ����ֹͣ����") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayControl(rh_ReplayHandle,CHDRV_PVR_REPLAY_PAUSE));
	LOG("��ͣ����10s\n");
	CH_RTOS_TaskDelay(10*1000);
	
}
CASE_F(TEST_PVR_REPLAY6,
	"����get replay status����õ�״̬ӦΪPAUSE,�طŵ�ʱ�����ļ���С������֮ͣǰ����") {

	CHDRV_PVR_ReplayStatus_t rpstru_Status;
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status));
	EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PAUSE, rpstru_Status.enm_PlayStatus); 

	EXPECT_EQ(rpstru_Status.i_PlayTimeS, rpstru_Status_pb.i_PlayTimeS);
	EXPECT_EQ(rpstru_Status.i_ReadOffsetKB, rpstru_Status_pb.i_ReadOffsetKB);
}


CASE_F(TEST_PVR_REPLAY7,
	"�ظ�����replay control��ͣ�طţ�Ӧ����ok") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayControl(rh_ReplayHandle,CHDRV_PVR_REPLAY_PAUSE));	
}

CASE_F(TEST_PVR_REPLAY8,
	"����replay control�ָ��طţ�����ƵӦ������������") {

	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayControl(rh_ReplayHandle,CHDRV_PVR_REPLAY_PLAY));
	
}
CASE_F(TEST_PVR_REPLAY9,
	"����get replay status����õ�״̬ӦΪPLAYING,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_pb.enm_PlayStatus);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
		
	} 
}


CASE_F(TEST_PVR_REPLAY10,
	"�ظ�����replay control�ָ��طţ�Ӧ����ok") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayControl(rh_ReplayHandle,CHDRV_PVR_REPLAY_PLAY));
	
}

CASE_F(TEST_PVR_REPLAY11,
	"����replay stop�������ط�����ֹͣ��") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayStop(rh_ReplayHandle));
	
}
CASE_F(TEST_PVR_REPLAY12,
	"����replay close����������OK��") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayClose(rh_ReplayHandle));

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Term());

	Auto_Play_DTV();
}
#endif
#if 1 //59~73
CASE_F(TEST_PVR_REPLAY_FFW1,
	"�ظ�����TEST_PVR_REPLAY1-TEST_PVR_REPLAY4") {
	
	S32 i_Loop = 0;
	Auto_Stop_DTV();
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Init());
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayOpen(NULL,ReplayCB,&rh_ReplayHandle));
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
	EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_IDLE, rpstru_Status_pb.enm_PlayStatus); 
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayStart(rh_ReplayHandle, (U8*)ResPathName, NULL, 0));
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_pb.enm_PlayStatus); 
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
		
	} 
}

CASE_F(TEST_PVR_REPLAY_FFW2,
	"����speed replay����2���ٿ������Ƶ���̰�2���ٿ��������Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FFW,CHDRV_PVR_REPLAY_SPEED_2));
	LOG("2���ٿ��\n");
}

CASE_F(TEST_PVR_REPLAY_FFW3,
	"����get replay status����õ�״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_FFW4,
	"����speed replay����4���ٿ������Ƶ���̰�4���ٿ��������Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FFW,CHDRV_PVR_REPLAY_SPEED_4));
	LOG("4���ٿ��\n");
}

CASE_F(TEST_PVR_REPLAY_FFW5,
	"����get replay status����õ�״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_FFW6,
	"����speed replay����8���ٿ������Ƶ���̰�8���ٿ��������Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FFW,CHDRV_PVR_REPLAY_SPEED_8));
	LOG("8���ٿ��\n");
}

CASE_F(TEST_PVR_REPLAY_FFW7,
	"����get replay status����õ�״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_FFW8,
	"����speed replay����16���ٿ������Ƶ���̰�16���ٿ��������Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FFW,CHDRV_PVR_REPLAY_SPEED_16));
	LOG("16���ٿ��\n");
}

CASE_F(TEST_PVR_REPLAY_FFW9,
	"����get replay status����õ�״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_FFW10,
	"����speed replay����32���ٿ������Ƶ���̰�32���ٿ��������Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FFW,CHDRV_PVR_REPLAY_SPEED_32));
	LOG("32���ٿ��\n");
}

CASE_F(TEST_PVR_REPLAY_FFW11,
	"����get replay status����õ�״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_FFW12,
	"����speed replay����1���ٿ��������ƵӦ������������") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FFW,CHDRV_PVR_REPLAY_SPEED_1));
	LOG("1���ٿ��,����ƵӦ������������\n");
}

CASE_F(TEST_PVR_REPLAY_FFW13,
	"����get replay status����õ�״̬ӦΪPLAYING,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_pb.enm_PlayStatus);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_FFW14,
	"����replay stop�������ط�����ֹͣ��") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayStop(rh_ReplayHandle));
	
}


CASE_F(TEST_PVR_REPLAY_FFW15,
	"����replay close����������OK��") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayClose(rh_ReplayHandle));
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Term());
}
#endif

//75

#if 1
CASE_F(TEST_PVR_REPLAY_FBW1,
	"�ظ�����TEST_PVR_REPLAY1-TEST_PVR_REPLAY4") {
	
	S32 i_Loop = 0;

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Init());
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayOpen(NULL,ReplayCB,&rh_ReplayHandle));
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
	EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_IDLE, rpstru_Status_pb.enm_PlayStatus); 
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayStart(rh_ReplayHandle, (U8*)ResPathName, NULL, 0));
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_pb.enm_PlayStatus); 
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_FBW2,
	"����speed replay����2���ٿ��ˣ���Ƶ���̰�2���ٿ��ˣ�����Ϊ����") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplaySeek(rh_ReplayHandle,PVR_RECTIME_LEN));
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FBW,CHDRV_PVR_REPLAY_SPEED_2));
	LOG("2���ٿ���\n");
}

CASE_F(TEST_PVR_REPLAY_FBW3,
	"����get replay status����õ�״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_FBW4,
	"����speed replay����4���ٿ��ˣ���Ƶ���̰�4���ٿ��ˣ�����Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FBW,CHDRV_PVR_REPLAY_SPEED_4));
	LOG("4���ٿ���\n");
}

CASE_F(TEST_PVR_REPLAY_FBW5,
	"����get replay status����õ�״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_FBW6,
	"����speed replay����8���ٿ��ˣ���Ƶ���̰�8���ٿ��ˣ�����Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FBW,CHDRV_PVR_REPLAY_SPEED_8));
	LOG("8���ٿ���\n");
}

CASE_F(TEST_PVR_REPLAY_FBW7,
	"����get replay status����õ�״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_FBW8,
	"����speed replay����16���ٿ��ˣ���Ƶ���̰�16���ٿ��ˣ�����Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FBW,CHDRV_PVR_REPLAY_SPEED_16));
	LOG("16���ٿ���\n");
}

CASE_F(TEST_PVR_REPLAY_FBW9,
	"����get replay status����õ�״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_FBW10,
	"����speed replay����32���ٿ��ˣ���Ƶ���̰�32���ٿ��ˣ�����Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FBW,CHDRV_PVR_REPLAY_SPEED_32));
	LOG("32���ٿ���\n");
}

CASE_F(TEST_PVR_REPLAY_FBW11,
	"����get replay status����õ�״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_FBW12,
	"����speed replay����1���ٿ��ˣ�����ƵӦ������������") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FBW,CHDRV_PVR_REPLAY_SPEED_1));
	LOG("1���ٿ��ˣ�����ƵӦ������������\n");
}

CASE_F(TEST_PVR_REPLAY_FBW13,
	"����get replay status����õ�״̬ӦΪPLAYING,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_FBW14,
	"����replay stop�������ط�����ֹͣ��") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayStop(rh_ReplayHandle));
	
}
CASE_F(TEST_PVR_REPLAY_FBW15,
	"����replay close����������OK��") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayClose(rh_ReplayHandle));

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Term());
}
#endif
#if 1 //77
CASE_F(TEST_PVR_REPLAY_SFW1,
	"�ظ�����TEST_PVR_REPLAY1-TEST_PVR_REPLAY4") {
	
	S32 i_Loop = 0;

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Init());

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayOpen(NULL,ReplayCB,&rh_ReplayHandle));
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
	EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_IDLE, rpstru_Status_pb.enm_PlayStatus); 
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayStart(rh_ReplayHandle, (U8*)ResPathName, NULL, 0));
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_pb.enm_PlayStatus); 
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_SFW2,
	"����speed replay����1/2�����ţ���Ƶ���̰�1/2�����ţ�����Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_SFW,CHDRV_PVR_REPLAY_SPEED_2));
	LOG("1/2��������\n");
}

CASE_F(TEST_PVR_REPLAY_SFW3,
	"����get replay status����õ�״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_SFW4,
	"����speed replay����1/4�����ţ���Ƶ���̰�1/4�����ţ�����Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_SFW,CHDRV_PVR_REPLAY_SPEED_4));
	LOG("1/4��������\n");
}

CASE_F(TEST_PVR_REPLAY_SFW5,
	"����get replay status����õ�״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_SFW6,
	"����speed replay����1/8�����ţ���Ƶ���̰�1/8�����ţ�����Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_SFW,CHDRV_PVR_REPLAY_SPEED_8));
	LOG("1/8��������\n");
}

CASE_F(TEST_PVR_REPLAY_SFW7,
	"����get replay status����õ�״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_SFW8,
	"����speed replay����1/16�����ţ���Ƶ���̰�1/16�����ţ�����Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_SFW,CHDRV_PVR_REPLAY_SPEED_16));
	LOG("1/16��������\n");
}

CASE_F(TEST_PVR_REPLAY_SFW9,
	"����get replay status����õ�״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_SFW10,
	"����speed replay����1/32�����ţ���Ƶ���̰�1/32�����ţ�����Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_SFW,CHDRV_PVR_REPLAY_SPEED_32));
	LOG("1/32��������\n");
}

CASE_F(TEST_PVR_REPLAY_SFW11,
	"����get replay status����õ�״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_SFW12,
	"����speed replay����1�����ţ�����ƵӦ������������") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_SFW,CHDRV_PVR_REPLAY_SPEED_1));
	LOG("1�������ţ�����ƵӦ������������\n");
}

CASE_F(TEST_PVR_REPLAY_SFW13,
	"����get replay status����õ�״̬ӦΪPLAYING,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_pb.enm_PlayStatus);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_SFW14,
	"����replay stop�������ط�����ֹͣ��") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayStop(rh_ReplayHandle));
	
}
CASE_F(TEST_PVR_REPLAY_SFW15,
	"����replay close����������OK��") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayClose(rh_ReplayHandle));
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Term());
}
#endif


//81~85
#if 1

CASE_F(TEST_PVR_REPLAY_SEEK1,
	"�ظ�����TEST_PVR_REPLAY1-TEST_PVR_REPLAY4") {
	
	S32 i_Loop = 0;

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Init());
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayOpen(NULL,ReplayCB,&rh_ReplayHandle));
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
	EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_IDLE, rpstru_Status_pb.enm_PlayStatus); 
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayStart(rh_ReplayHandle, (U8*)ResPathName, NULL, 0));
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_pb.enm_PlayStatus); 
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_SEEK2,
	"����replay seek��ת��ָ����λ�ã�����ƵӦ������ת����λ����������") {

	S32 ri_DstTimeS = 100;
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplaySeek(rh_ReplayHandle,ri_DstTimeS));
	LOG("��ת��100s\n");
}

CASE_F(TEST_PVR_REPLAY_SEEK3,
	"����get replay status����õ�״̬ӦΪPLAYING,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_pb.enm_PlayStatus); 
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_SEEK4,
	"����replay stop�������ط�����ֹͣ��") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayStop(rh_ReplayHandle));
}

CASE_F(TEST_PVR_REPLAY_SEEK5,
	"����replay close����������OK.") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayClose(rh_ReplayHandle));
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Term());
	EXPECT_EQ(CHDRV_PVR_OK, Rec_Res_Term());
	EXPECT_EQ(CHDRV_PVR_OK, Replay_Res_Term());	
}
#endif

//86-98
#if 1
CASE_F(TEST_PVR_REC_REPLAY1,
	"�ظ�����TEST_PVR_REC1-TEST_PVR_REC4") {

	S32 i_Loop = 0;

	/*Auto_Play_DTV();
	CH_FSYS_Init();
	Set_RECStartParam(PVR_NORMAL_REC_TYPE);//��ͨ¼��
	CH_FSYS_DeleteFile((const S8 *)"/C:/TS/record.ts",NULL,NULL);
	CH_FSYS_DeleteFile((const S8 *)"/C:/TS/record.ts.meta",NULL,NULL);
	CH_FSYS_DeleteFile((const S8 *)"/C:/TS/record.ts.pmt",NULL,NULL);
	CH_FSYS_DeleteFile((const S8 *)"/C:/TS/record.ts.idx",NULL,NULL);*/

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Init());
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECOpen(NULL,RecordCB,&rh_RECHandle)); 
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status_rec));
	EXPECT_EQ(CHDRV_PVR_REC_STATUS_IDLE, rpstru_Status_rec.enm_RECStatus); 
    EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECStart(rh_RECHandle,&RecParams));
		 
	for(i_Loop = 0;i_Loop < LOOP_5*2;i_Loop++)
	{
		
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status_rec));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_rec.enm_RECStatus);
		LOG("��ǰ¼��ʱ��: %d\n", rpstru_Status_rec.i_RECTimeS);
		LOG("��ǰ¼�ƴ�С: %d\n", rpstru_Status_rec.i_RECFileLengthKB);
		CH_RTOS_TaskDelay(2000);
	}
}

CASE_F(TEST_PVR_REC_REPLAY2,
	"�ظ�����TEST_PVR_REPLAY1-TEST_PVR_REPLAY4") {
	
	S32 i_Loop = 0;

	Auto_Stop_DTV();
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayOpen(NULL,ReplayCB,&rh_ReplayHandle));
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
	EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_IDLE, rpstru_Status_pb.enm_PlayStatus); 
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayStart(rh_ReplayHandle, (U8*)ResPathName, NULL, 0));
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_pb.enm_PlayStatus); 
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
		
	} 
}
CASE_F(TEST_PVR_REC_REPLAY3,
	"����rec control��ͣ¼�ƣ�¼����ͣ����������Ƶ��Ȼ��������") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status_rec));
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECControl(rh_RECHandle,CHDRV_PVR_REC_PAUSE));
	LOG("��ͣ¼��10s\n");
	CH_RTOS_TaskDelay(10000);
	
}

CASE_F(TEST_PVR_REC_REPLAY4,
	"����get rec status����õ�״̬ӦΪPAUSE,¼�Ƶ�ʱ�����ļ���С������֮ͣǰ����") {

	CHDRV_PVR_RECStatus_t rpstru_Status;
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status));
	EXPECT_EQ(CHDRV_PVR_REC_STATUS_PAUSE, rpstru_Status.enm_RECStatus); 
	EXPECT_EQ(rpstru_Status.i_RECFileLengthKB, rpstru_Status_rec.i_RECFileLengthKB);
	EXPECT_EQ(rpstru_Status.i_RECTimeS, rpstru_Status_rec.i_RECTimeS);

}

CASE_F(TEST_PVR_REC_REPLAY5,
	"����replay control��ͣ�طţ�����ƵӦ����ֹͣ����") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayControl(rh_ReplayHandle,CHDRV_PVR_REPLAY_PAUSE));
	LOG("��ͣ����10s\n");
	CH_RTOS_TaskDelay(10000);
	
}

CASE_F(TEST_PVR_REC_REPLAY6,
	"����get replay status����õ�״̬ӦΪPAUSE,�طŵ�ʱ�����ļ���С������֮ͣǰ����") {

	CHDRV_PVR_ReplayStatus_t rpstru_Status;
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_RECHandle,&rpstru_Status));
	EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PAUSE, rpstru_Status.enm_PlayStatus); 
	EXPECT_EQ(rpstru_Status.i_ReadOffsetKB, rpstru_Status_pb.i_ReadOffsetKB);
	EXPECT_EQ(rpstru_Status.i_PlayTimeS, rpstru_Status_pb.i_PlayTimeS);
}

CASE_F(TEST_PVR_REC_REPLAY7,
	"����rec control�ָ�¼�ƣ�¼�ƹ���������Ƶ��Ȼֹͣ����") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECControl(rh_RECHandle,CHDRV_PVR_REC_RESUME));
	
}
CASE_F(TEST_PVR_REC_REPLAY8,
	"����get rec status����õ�״̬ӦΪRECORDING,����ʱʱ����¼�Ƶ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status_rec));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_rec.enm_RECStatus);
		LOG("��ǰ¼��ʱ��: %d\n", rpstru_Status_rec.i_RECTimeS);
		LOG("��ǰ¼�ƴ�С: %d\n", rpstru_Status_rec.i_RECFileLengthKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY9,
	"����replay control�ָ��طţ�����ƵӦ������������") {

	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayControl(rh_ReplayHandle,CHDRV_PVR_REPLAY_PLAY));
	
}
CASE_F(TEST_PVR_REC_REPLAY10,
	"����get replay status����õ�״̬ӦΪPLAYING,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_pb.enm_PlayStatus);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY11,
	"����replay stop�������ط�����ֹͣ��") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayStop(rh_ReplayHandle));
}

CASE_F(TEST_PVR_REC_REPLAY12,
	"����rec stop������¼������ֹͣ��") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECStop(rh_RECHandle));
}

CASE_F(TEST_PVR_REC_REPLAY13,
	"����replay close��rec close������������OK") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayClose(rh_ReplayHandle));
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECClose(rh_RECHandle));//��CHDRV_PVR_RECStop��ǰ������

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Term());
	EXPECT_EQ(CHDRV_PVR_OK, Rec_Res_Term());
	EXPECT_EQ(CHDRV_PVR_OK, Replay_Res_Term());	
}
#endif
//99-102
#if 1
CASE_F(TEST_PVR_REC_REPLAY_FFW1,
	"�ظ�����TEST_PVR_REC1-TEST_PVR_REC4") {

	S32 i_Loop = 0;

	//Auto_Play_DTV();
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Init());
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECOpen(NULL,RecordCB,&rh_RECHandle)); 
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status_rec));
	EXPECT_EQ(CHDRV_PVR_REC_STATUS_IDLE, rpstru_Status_rec.enm_RECStatus); 
    EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECStart(rh_RECHandle,&RecParams));

	LOG("¼��һ��Լ15���ӵ���Ƶ,�����ĵȺ�\n"); 
		 
	for(i_Loop = 0;i_Loop < PVR_RECTIME_LEN;i_Loop++)
	{
		CH_RTOS_TaskDelay(1000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status_rec));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_rec.enm_RECStatus);
		if (i_Loop < 10)//����ȫ����ʾ��ע�͵�
		{
			LOG("��ǰ¼��ʱ��: %d\n", rpstru_Status_rec.i_RECTimeS);
			LOG("��ǰ¼�ƴ�С: %d\n", rpstru_Status_rec.i_RECFileLengthKB);
		}
	}
}

CASE_F(TEST_PVR_REC_REPLAY_FFW2_1,
	"�ظ�����TEST_PVR_REPLAY1-TEST_PVR_REPLAY4") {
	
	S32 i_Loop = 0;

	Auto_Stop_DTV();
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayOpen(NULL,ReplayCB,&rh_ReplayHandle));
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
	EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_IDLE, rpstru_Status_pb.enm_PlayStatus); 
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayStart(rh_ReplayHandle, (U8*)ResPathName, NULL, 0));
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_pb.enm_PlayStatus); 
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_FFW2_2,
	"����speed replay����2���ٿ������Ƶ���̰�2���ٿ��������Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FFW,CHDRV_PVR_REPLAY_SPEED_2));
	LOG("2���ٿ��\n");
}

CASE_F(TEST_PVR_REC_REPLAY_FFW2_3,
	"����get replay status����õ�״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_FFW2_4,
	"����speed replay����4���ٿ������Ƶ���̰�4���ٿ��������Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FFW,CHDRV_PVR_REPLAY_SPEED_4));
	LOG("4���ٿ��\n");
}

CASE_F(TEST_PVR_REC_REPLAY_FFW2_5,
	"����get replay status����õ�״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_FFW2_6,
	"����speed replay����8���ٿ������Ƶ���̰�8���ٿ��������Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FFW,CHDRV_PVR_REPLAY_SPEED_8));
	LOG("8���ٿ��\n");
}

CASE_F(TEST_PVR_REC_REPLAY_FFW2_7,
	"����get replay status����õ�״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_FFW2_8,
	"����speed replay����16���ٿ������Ƶ���̰�16���ٿ��������Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FFW,CHDRV_PVR_REPLAY_SPEED_16));
	LOG("16���ٿ��\n");
}

CASE_F(TEST_PVR_REC_REPLAY_FFW2_9,
	"����get replay status����õ�״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_FFW2_10,
	"����speed replay����32���ٿ������Ƶ���̰�32���ٿ��������Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FFW,CHDRV_PVR_REPLAY_SPEED_32));
	LOG("32���ٿ��\n");
}

CASE_F(TEST_PVR_REC_REPLAY_FFW2_11,
	"����get replay status����õ�״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_FFW2_12,
	"����speed replay����1���ٿ��������ƵӦ������������") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FFW,CHDRV_PVR_REPLAY_SPEED_1));
	LOG("1���ٿ��\n");
}

CASE_F(TEST_PVR_REC_REPLAY_FFW2_13,
	"����get replay status����õ�״̬ӦΪPLAYING,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_pb.enm_PlayStatus);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_FFW2_14,
	"����replay stop�������ط�����ֹͣ��") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayStop(rh_ReplayHandle));
	
}


CASE_F(TEST_PVR_REC_REPLAY_FFW2_15,
	"����replay close����������OK��") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayClose(rh_ReplayHandle));//failed to MI_PVR_DeInit()!
	
}



CASE_F(TEST_PVR_REC_REPLAY_FFW3,
	"����rec stop������¼������ֹͣ��") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECStop(rh_RECHandle));
}

CASE_F(TEST_PVR_REC_REPLAY_FFW4,
	"����rec close����������OK.") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECClose(rh_RECHandle));
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Term());
	Rec_Res_Term();
	Replay_Res_Term();
	
}
#endif
//104
#if 1
CASE_F(TEST_PVR_REC_REPLAY_FBW1,
	"�ظ�����TEST_PVR_REC1-TEST_PVR_REC4") {

	S32 i_Loop = 0;

	//Auto_Play_DTV();
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Init());
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECOpen(NULL,RecordCB,&rh_RECHandle)); 
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status_rec));
	EXPECT_EQ(CHDRV_PVR_REC_STATUS_IDLE, rpstru_Status_rec.enm_RECStatus); 
    EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECStart(rh_RECHandle,&RecParams));

	LOG("¼��һ��Լ15���ӵ���Ƶ,�����ĵȺ�\n"); 
		 
	for(i_Loop = 0;i_Loop < PVR_RECTIME_LEN;i_Loop++)
	{
		CH_RTOS_TaskDelay(1000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status_rec));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_rec.enm_RECStatus);
		if (i_Loop < 10)
		{
			LOG("��ǰ¼��ʱ��: %d\n", rpstru_Status_rec.i_RECTimeS);
			LOG("��ǰ¼�ƴ�С: %d\n", rpstru_Status_rec.i_RECFileLengthKB);
		}
	}
}

CASE_F(TEST_PVR_REC_REPLAY_FBW2_1,
	"�ظ�����TEST_PVR_REPLAY1-TEST_PVR_REPLAY4") {
	
	S32 i_Loop = 0;

	Auto_Stop_DTV();
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayOpen(NULL,ReplayCB,&rh_ReplayHandle));
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
	EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_IDLE, rpstru_Status_pb.enm_PlayStatus); 
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayStart(rh_ReplayHandle, (U8*)ResPathName, NULL, 0));
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_pb.enm_PlayStatus); 
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_FBW2_2,
	"����speed replay����2���ٿ��ˣ���Ƶ���̰�2���ٿ��ˣ�����Ϊ����") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplaySeek(rh_ReplayHandle,PVR_RECTIME_LEN));
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FBW,CHDRV_PVR_REPLAY_SPEED_2));
	LOG("2���ٿ���\n");
}

CASE_F(TEST_PVR_REC_REPLAY_FBW2_3,
	"����get replay status����õ�״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_FBW2_4,
	"����speed replay����4���ٿ��ˣ���Ƶ���̰�4���ٿ��ˣ�����Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FBW,CHDRV_PVR_REPLAY_SPEED_4));
	LOG("4���ٿ���\n");
}

CASE_F(TEST_PVR_REC_REPLAY_FBW2_5,
	"����get replay status����õ�״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_FBW2_6,
	"����speed replay����8���ٿ��ˣ���Ƶ���̰�8���ٿ��ˣ�����Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FBW,CHDRV_PVR_REPLAY_SPEED_8));
	LOG("8���ٿ���\n");
}

CASE_F(TEST_PVR_REC_REPLAY_FBW2_7,
	"����get replay status����õ�״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_FBW2_8,
	"����speed replay����16���ٿ��ˣ���Ƶ���̰�16���ٿ��ˣ�����Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FBW,CHDRV_PVR_REPLAY_SPEED_16));
	LOG("16���ٿ���\n");
}

CASE_F(TEST_PVR_REC_REPLAY_FBW2_9,
	"����get replay status����õ�״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_FBW2_10,
	"����speed replay����32���ٿ��ˣ���Ƶ���̰�32���ٿ��ˣ�����Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FBW,CHDRV_PVR_REPLAY_SPEED_32));
	LOG("32���ٿ���\n");
}

CASE_F(TEST_PVR_REC_REPLAY_FBW2_11,
	"����get replay status����õ�״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_FBW2_12,
	"����speed replay����1���ٿ��ˣ�����ƵӦ������������") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FBW,CHDRV_PVR_REPLAY_SPEED_1));
	LOG("1���ٿ���\n");
}

CASE_F(TEST_PVR_REC_REPLAY_FBW2_13,
	"����get replay status����õ�״̬ӦΪPLAYING,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < 5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_FBW2_14,
	"����replay stop�������ط�����ֹͣ��") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayStop(rh_ReplayHandle));
	
}
CASE_F(TEST_PVR_REC_REPLAY_FBW2_15,
	"����replay close����������OK��") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayClose(rh_ReplayHandle));//failed to MI_PVR_DeInit()! please stop and close record!
}


CASE_F(TEST_PVR_REC_REPLAY_FBW3,
	"����rec stop������¼������ֹͣ��") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECStop(rh_RECHandle));
}

CASE_F(TEST_PVR_REC_REPLAY_FBW4,
	"����rec close����������OK.") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECClose(rh_RECHandle));
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Term());

	Rec_Res_Term();
	Replay_Res_Term();
	
}
#endif
//106
#if 1
CASE_F(TEST_PVR_REC_REPLAY_SFW1,
	"�ظ�����TEST_PVR_REC1-TEST_PVR_REC4") {

	S32 i_Loop = 0;

	//Auto_Play_DTV();
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Init());
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECOpen(NULL,RecordCB,&rh_RECHandle)); 
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status_rec));
	EXPECT_EQ(CHDRV_PVR_REC_STATUS_IDLE, rpstru_Status_rec.enm_RECStatus); 
    EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECStart(rh_RECHandle,&RecParams));

	LOG("¼��5����\n");
		 
	for(i_Loop = 0;i_Loop < 60*LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(1000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status_rec));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_rec.enm_RECStatus);
		if (i_Loop < 10)
		{
			LOG("��ǰ¼��ʱ��: %d\n", rpstru_Status_rec.i_RECTimeS);
			LOG("��ǰ¼�ƴ�С: %d\n", rpstru_Status_rec.i_RECFileLengthKB);
		}
	}
}

CASE_F(TEST_PVR_REC_REPLAY_SFW2_1,
	"�ظ�����TEST_PVR_REPLAY1-TEST_PVR_REPLAY4") {
	
	S32 i_Loop = 0;

	Auto_Stop_DTV();
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayOpen(NULL,ReplayCB,&rh_ReplayHandle));
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
	EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_IDLE, rpstru_Status_pb.enm_PlayStatus); 
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayStart(rh_ReplayHandle, (U8*)ResPathName, NULL, 0));
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_pb.enm_PlayStatus); 
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_SFW2_2,
	"����speed replay����1/2�����ţ���Ƶ���̰�1/2�����ţ�����Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_SFW,CHDRV_PVR_REPLAY_SPEED_2));
	LOG("1/2��������\n");
}

CASE_F(TEST_PVR_REC_REPLAY_SFW2_3,
	"����get replay status����õ�״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_SFW2_4,
	"����speed replay����1/4�����ţ���Ƶ���̰�1/4�����ţ�����Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_SFW,CHDRV_PVR_REPLAY_SPEED_4));
	LOG("1/4��������\n");
}

CASE_F(TEST_PVR_REC_REPLAY_SFW2_5,
	"����get replay status����õ�״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_SFW2_6,
	"����speed replay����1/8�����ţ���Ƶ���̰�1/8�����ţ�����Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_SFW,CHDRV_PVR_REPLAY_SPEED_8));
	LOG("1/8��������\n");
}

CASE_F(TEST_PVR_REC_REPLAY_SFW2_7,
	"����get replay status����õ�״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_SFW2_8,
	"����speed replay����1/16�����ţ���Ƶ���̰�1/16�����ţ�����Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_SFW,CHDRV_PVR_REPLAY_SPEED_16));
	LOG("1/16��������\n");
}

CASE_F(TEST_PVR_REC_REPLAY_SFW2_9,
	"����get replay status����õ�״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_SFW2_10,
	"����speed replay����1/32�����ţ���Ƶ���̰�1/32�����ţ�����Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_SFW,CHDRV_PVR_REPLAY_SPEED_32));
	LOG("1/32��������\n");
}

CASE_F(TEST_PVR_REC_REPLAY_SFW2_11,
	"����get replay status����õ�״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_SFW2_12,
	"����speed replay����1�����ţ�����ƵӦ������������") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_SFW,CHDRV_PVR_REPLAY_SPEED_1));
	LOG("1��������\n");
}

CASE_F(TEST_PVR_REC_REPLAY_SFW2_13,
	"����get replay status����õ�״̬ӦΪPLAYING,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_pb.enm_PlayStatus);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_SFW2_14,
	"����replay stop�������ط�����ֹͣ��") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayStop(rh_ReplayHandle));
	
}
CASE_F(TEST_PVR_REC_REPLAY_SFW2_15,
	"����replay close����������OK��") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayClose(rh_ReplayHandle));//failed to MI_PVR_DeInit()!please stop and close record!
}


CASE_F(TEST_PVR_REC_REPLAY_SFW3,
	"����rec stop������¼������ֹͣ��") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECStop(rh_RECHandle));
}

CASE_F(TEST_PVR_REC_REPLAY_SFW4,
	"����rec close����������OK.") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECClose(rh_RECHandle));
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Term());

	Rec_Res_Term();
	Replay_Res_Term();
}
#endif

//110-113
#if 1
CASE_F(TEST_PVR_REC_REPLAY_SEEK1,
	"�ظ�����TEST_PVR_REC1-TEST_PVR_REC4") {

	S32 i_Loop = 0;

	//Auto_Play_DTV();

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Init());
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECOpen(NULL,RecordCB,&rh_RECHandle)); 
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status_rec));
	EXPECT_EQ(CHDRV_PVR_REC_STATUS_IDLE, rpstru_Status_rec.enm_RECStatus); 
    EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECStart(rh_RECHandle,&RecParams));

	LOG("¼��1����\n");
		 
	for(i_Loop = 0;i_Loop < 60;i_Loop++)
	{
		CH_RTOS_TaskDelay(1000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status_rec));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_rec.enm_RECStatus);
		if (i_Loop < 10)
		{
			LOG("��ǰ¼��ʱ��: %d\n", rpstru_Status_rec.i_RECTimeS);
			LOG("��ǰ¼�ƴ�С: %d\n", rpstru_Status_rec.i_RECFileLengthKB);
		}
		
	}
}

CASE_F(TEST_PVR_REC_REPLAY_SEEK2,
	"�ظ�����TEST_PVR_REPLAY1-TEST_PVR_REPLAY4") {
	
	S32 i_Loop = 0;

	Auto_Stop_DTV();
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayOpen(NULL,ReplayCB,&rh_ReplayHandle));
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
	EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_IDLE, rpstru_Status_pb.enm_PlayStatus); 
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayStart(rh_ReplayHandle, (U8*)ResPathName, NULL, 0));
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_pb.enm_PlayStatus); 
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 

	S32 ri_DstTimeS = 30;
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplaySeek(rh_ReplayHandle,ri_DstTimeS));

	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_pb.enm_PlayStatus); 
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("��ǰ�ط�ƫ��: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayStop(rh_ReplayHandle));

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayClose(rh_ReplayHandle)); //failed to MI_PVR_DeInit(),please stop and close record!
}


CASE_F(TEST_PVR_REC_REPLAY_SEEK3,
	"����rec stop������¼������ֹͣ��") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECStop(rh_RECHandle));
}

CASE_F(TEST_PVR_REC_REPLAY_SEEK4,
	"����rec close����������OK.") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECClose(rh_RECHandle));//CHDRV_PVR_RECStop���������CHDRV_PVR_RECClose
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Term());

	Rec_Res_Term();
	Replay_Res_Term();
}
#endif
//115
#if 1 ///δ����
CASE_F(TEST_PVR_REC_ENCRYPT1,
	"�ظ����裨TEST_PVR_REC1-TEST_PVR_REC12����start recʱuc_RecordType����Ϊ����¼��") {
	S32 i_Loop = 0;
	CHDRV_PVR_RECStatus_t rpstru_Status;

	Set_RECStartParam(PVR_NORMAL_ENCRYPT_REC_TYPE);
	//Auto_Play_DTV();
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Init());
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECOpen(NULL,RecordCB,&rh_RECHandle)); 
 
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status_rec));
	EXPECT_EQ(CHDRV_PVR_REC_STATUS_IDLE, rpstru_Status_rec.enm_RECStatus); 

    EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECStart(rh_RECHandle,&RecParams));
	 
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status_rec));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_rec.enm_RECStatus);
		LOG("Record File Time: %d\n", rpstru_Status_rec.i_RECTimeS);
		LOG("Record File Size: %d\n", rpstru_Status_rec.i_RECFileLengthKB);	
	}
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECControl(rh_RECHandle,CHDRV_PVR_REC_PAUSE));
	LOG("��ͣ¼��5s\n");
	CH_RTOS_TaskDelay(5000);
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status));
	EXPECT_EQ(CHDRV_PVR_REC_STATUS_PAUSE, rpstru_Status.enm_RECStatus); 
	EXPECT_EQ(rpstru_Status.i_RECFileLengthKB, rpstru_Status_rec.i_RECFileLengthKB);
	EXPECT_EQ(rpstru_Status.i_RECTimeS, rpstru_Status_rec.i_RECTimeS);

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECControl(rh_RECHandle,CHDRV_PVR_REC_PAUSE));	
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECControl(rh_RECHandle,CHDRV_PVR_REC_RESUME));
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status_rec));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_rec.enm_RECStatus);
		LOG("Record File Time: %d\n", rpstru_Status_rec.i_RECTimeS);
		LOG("Record File Size: %d\n", rpstru_Status_rec.i_RECFileLengthKB);
		CH_RTOS_TaskDelay(2000);
	} 

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECControl(rh_RECHandle,CHDRV_PVR_REC_RESUME));
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECStop(rh_RECHandle));

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECClose(rh_RECHandle));

	EXPECT_EQ(CHDRV_PVR_NOT_SUPPORT, CHDRV_PVR_GetResInfo(NULL,NULL,NULL,NULL));

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Term());

	Rec_Res_Term();

}
	
#endif


//121-125
#if 0 //��ʱ���������
CASE_F(TEST_PVR_REC_ECM1,
	"�ظ����裨TEST_PVR_REC1-TEST_PVR_REC12����start recʱstru_RecParam�贫��ECM PID") {

	S32 i_Loop = 0;
	CHDRV_PVR_RECStatus_t rpstru_Status;

	Auto_Play_DTV();
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Init());
	
	//TEST_PVR_REC1
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECOpen(NULL,RecordCB,&rh_RECHandle)); 

	//TEST_PVR_REC2 
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status));
	EXPECT_EQ(CHDRV_PVR_REC_STATUS_IDLE, rpstru_Status.enm_RECStatus);

	//TEST_PVR_REC3
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECStart(rh_RECHandle,&RecParams));

	//TEST_PVR_REC4
	
	LOG("¼��һ��Լ10���ӵ���Ƶ,�����ĵȺ�\n"); 
	for(i_Loop = 0;i_Loop < 60;i_Loop++)
	{
		CH_RTOS_TaskDelay(1000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status_rec));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_rec.enm_RECStatus);
		LOG("��ǰ¼��ʱ��: %d\n", rpstru_Status_rec.i_RECTimeS);
		LOG("��ǰ¼�ƴ�С: %d\n", rpstru_Status_rec.i_RECFileLengthKB);	
		
	}
	//TEST_PVR_REC5
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECControl(rh_RECHandle,CHDRV_PVR_REC_PAUSE));
	LOG("��ͣ¼��5s\n");
	CH_RTOS_TaskDelay(5*1000);

	//TEST_PVR_REC6
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status));
	EXPECT_EQ(CHDRV_PVR_REC_STATUS_PAUSE, rpstru_Status.enm_RECStatus); 
	EXPECT_EQ(rpstru_Status.i_RECFileLengthKB, rpstru_Status_rec.i_RECFileLengthKB);
	EXPECT_EQ(rpstru_Status.i_RECTimeS, rpstru_Status_rec.i_RECTimeS);

	//TEST_PVR_REC7
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECControl(rh_RECHandle,CHDRV_PVR_REC_PAUSE));

	//TEST_PVR_REC8
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECControl(rh_RECHandle,CHDRV_PVR_REC_RESUME));

	//TEST_PVR_REC9
	for(i_Loop = 0;i_Loop < 10;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status_rec));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_rec.enm_RECStatus);
		LOG("��ǰ¼��ʱ��: %d\n", rpstru_Status_rec.i_RECTimeS);
		LOG("��ǰ¼�ƴ�С: %d\n", rpstru_Status_rec.i_RECFileLengthKB);
		CH_RTOS_TaskDelay(1000);
	} 

	//TEST_PVR_REC10
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECControl(rh_RECHandle,CHDRV_PVR_REC_RESUME));
	
	//TEST_PVR_REC11
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECStop(rh_RECHandle));

	//TEST_PVR_REC12
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECClose(rh_RECHandle));

	Rec_Res_Term();

	
}

CASE_F(TEST_PVR_REC_ECM2,
	"�ظ����裨TEST_PVR_REPLAY1-TEST_PVR_REPLAY3��������������") {

	Auto_Stop_DTV();
	//TEST_PVR_REPLAY1
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayOpen(NULL,ReplayCB,&rh_ReplayHandle));
	//TEST_PVR_REPLAY2
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
	EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_IDLE, rpstru_Status_pb.enm_PlayStatus); 
	//TEST_PVR_REPLAY3
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayStart(rh_ReplayHandle, (U8*)ResPathName, NULL, 0));
}

CASE_F(TEST_PVR_REC_ECM3,
	"ͨ��sectionģ������ȷ��ȡ���ط��е�ECM���ݡ�") {

}

CASE_F(TEST_PVR_REC_ECM4,
	"����replay stop�������ط�����ֹͣ��") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayStop(rh_ReplayHandle));
}

CASE_F(TEST_PVR_REC_ECM5,
	"����replay close����������OK.") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayClose(rh_ReplayHandle));

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Term());

	Replay_Res_Term();
}

#endif



///127-138
#if 1
CASE_F(TEST_PVR_TIMESHIFT1,
	"����timeshift open����һ����������Ƿ񷵻سɹ����Ҿ����ΪNULL") {

	//Auto_Play_DTV();
   CHDRV_PVR_Init();
	//Set_RECStartParam(PVR_NORMAL_REC_TYPE);

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftOpen(NULL,NULL,(CHDRV_PVR_TIMESHIFT_CALLBACK)TimeShiftCB,&rh_TimeshiftHandle)); 

}

CASE_F(TEST_PVR_TIMESHIFT2,
	"����get timeshift status״̬ӦΪIDL") {

		 
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
	EXPECT_EQ(CHDRV_PVR_REC_STATUS_IDLE, rpstru_Status_timeshift.enm_RECStatus); 
}

CASE_F(TEST_PVR_TIMESHIFT3,
	"����ȷ�Ĳ���start timeshift�ոմ�����handle���ļ�����·����env��ȡ��ʱ�ƿ�ʼ���ھ�֡״̬") {
	CH_RTOS_TaskDelay(5000);//�������Ž�Ŀ״̬
	Auto_Stop_DTV(); ///����timeshift����֮ǰ����ͣDTV��portingδʵ��ͣDTV����ʱ�ô˷�������
    EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftStart(rh_TimeshiftHandle,&RecParams));
	//EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftReplay2Pause(rh_TimeshiftHandle));
	LOG("ʱ�ƿ�ʼ���ھ�֡״̬,Y or N ?\n");
	
}

CASE_F(TEST_PVR_TIMESHIFT4,
	"����get timeshift status����õ�rec״̬ӦΪRECORDING,����ʱʱ����¼�Ƶ�ʱ�����ļ���С����õ�replay״̬ӦΪPAUSE,�طŵ�ʱ�����ļ���СΪ0") {

	S32 i_Loop = 0;
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PAUSE, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("��ǰ¼��ʱ��: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("��ǰ¼�ƴ�С: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);	
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("��ǰ�طŴ�С: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);	
	}
	
}

CASE_F(TEST_PVR_TIMESHIFT5,
	"����timeshift control���в��ţ�����ƵӦ������������") {

		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftReplayControl(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_PLAY));
		LOG("����ƵӦ������������\n");
	
}



CASE_F(TEST_PVR_TIMESHIFT6,
	"����get timeshift status����õ�rec״̬ӦΪRECORDING,����ʱʱ����¼�Ƶ�ʱ�����ļ���С����õ�replay״̬ӦΪPLAYING,����ʱʱ���»طŵ�ʱ�����ļ���С") {

		S32 i_Loop = 0;
		for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
		{
			CH_RTOS_TaskDelay(2000);
			EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
			EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
			EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_timeshift.enm_PlayStatus);
			LOG("��ǰ¼��ʱ��: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
			LOG("��ǰ¼�ƴ�С: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
			LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
			LOG("��ǰ�طŴ�С: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);
		}
	
}

CASE_F(TEST_PVR_TIMESHIFT7,
	"�ظ�����replay control���в��ţ�Ӧ����ok") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftReplayControl(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_PLAY));
	
}

CASE_F(TEST_PVR_TIMESHIFT8,
	"����replay control��ͣ���ţ�����ƵӦ����ֹͣ����") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftReplayControl(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_PAUSE));
}

CASE_F(TEST_PVR_TIMESHIFT9,
	"����get timeshift status����õ�rec״̬ӦΪRECORDING,����ʱʱ����¼�Ƶ�ʱ�����ļ���С����õ�replay״̬ӦΪPAUSE,�طŵ�ʱ�����ļ�������֮ͣǰ����") {


	CHDRV_PVR_TimeShiftStatus_t rpstru_Status;
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status));
	
	S32 i_Loop = 0;
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(1000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PAUSE, rpstru_Status.enm_PlayStatus);
		LOG("��ǰ¼��ʱ��: %d\n", rpstru_Status.i_TotalRECTimeS);
		LOG("��ǰ¼�ƴ�С: %d\n", rpstru_Status.i_RECFileLengthKB);
		EXPECT_EQ(rpstru_Status.i_PlayedTimeS, rpstru_Status_timeshift.i_PlayedTimeS);
		EXPECT_EQ(rpstru_Status.i_PlayedFileOffsetKB, rpstru_Status_timeshift.i_PlayedFileOffsetKB);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("��ǰ�طŴ�С: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);

	}
}

CASE_F(TEST_PVR_TIMESHIFT10,
	"�ظ�����replay control��ͣ���ţ�Ӧ����ok") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftReplayControl(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_PAUSE));
	
}


CASE_F(TEST_PVR_TIMESHIFT11,
	"����timeshift stop������ʱ������ֹͣ��") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftStop(rh_TimeshiftHandle));
	
}

CASE_F(TEST_PVR_TIMESHIFT12,
	"����timeshift close����������OK��") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftClose(rh_TimeshiftHandle));//called in CHDRV_PVR_TimeShiftStop
	CHDRV_PVR_Term();
	Timeshift_Res_Term();
}

#endif

//140 - 154
#if 1

CASE_F(TEST_PVR_TIMESHIFT_FFW1,
	"�ظ�����TEST_PVR_TIMESHIFT1-TEST_PVR_TIMESHIFT6") {

	//Auto_Play_DTV();
	
//TEST_PVR_TIMESHIFT1
    CHDRV_PVR_Init();
	//Set_RECStartParam(PVR_NORMAL_REC_TYPE);//��������

//TEST_PVR_TIMESHIFT2

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftOpen(NULL,NULL,(CHDRV_PVR_TIMESHIFT_CALLBACK)TimeShiftCB,&rh_TimeshiftHandle));

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
	EXPECT_EQ(CHDRV_PVR_REC_STATUS_IDLE, rpstru_Status_timeshift.enm_RECStatus); 

//TEST_PVR_TIMESHIFT3	
	CH_RTOS_TaskDelay(5000);
	Auto_Stop_DTV();
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftStart(rh_TimeshiftHandle,&RecParams));
	
	//EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftReplay2Pause(rh_TimeshiftHandle));
	LOG("ʱ�ƿ�ʼ���ھ�֡״̬,Y or N ?\n");

//TEST_PVR_TIMESHIFT4

	S32 i_Loop = 0;
	LOG("����ʱ��Լ15���ӵ���Ƶ�������ĵȺ�\n");
	for(i_Loop = 0;i_Loop < PVR_RECTIME_LEN;i_Loop++)
	{
		CH_RTOS_TaskDelay(1000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PAUSE, rpstru_Status_timeshift.enm_PlayStatus);
		if(i_Loop < 10)
		{
			LOG("��ǰ¼��ʱ��: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
			LOG("��ǰ¼�ƴ�С: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		}
	}
	
//TEST_PVR_TIMESHIFT5

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftReplayControl(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_PLAY));
	LOG("����ƵӦ������������\n");

//TEST_PVR_TIMESHIFT6

	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("��ǰ¼��ʱ��: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("��ǰ¼�ƴ�С: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("��ǰ�طŴ�С: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);	
	}

}

CASE_F(TEST_PVR_TIMESHIFT_FFW2,
	"����speed timeshift����2���ٿ������Ƶ���̰�2���ٿ��������Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftSpeedReplay(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_FFW,CHDRV_PVR_REPLAY_SPEED_2));
	LOG("2���ٿ��\n");
}

CASE_F(TEST_PVR_TIMESHIFT_FFW3,
	"����get timeshift status����õ�rec״̬ӦΪRECORDING,����ʱʱ����¼�Ƶ�ʱ�����ļ���С����õ�replay״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("��ǰ¼��ʱ��: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("��ǰ¼�ƴ�С: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("��ǰ�طŴ�С: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);	
	}
}

CASE_F(TEST_PVR_TIMESHIFT_FFW4,
	"����speed timeshift����4���ٿ������Ƶ���̰�4���ٿ��������Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftSpeedReplay(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_FFW,CHDRV_PVR_REPLAY_SPEED_4));
	LOG("4���ٿ��\n");
}

CASE_F(TEST_PVR_TIMESHIFT_FFW5,
	"����get timeshift status����õ�rec״̬ӦΪRECORDING,����ʱʱ����¼�Ƶ�ʱ�����ļ���С����õ�replay״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("��ǰ¼��ʱ��: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("��ǰ¼�ƴ�С: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("��ǰ�طŴ�С: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);
	}

}

CASE_F(TEST_PVR_TIMESHIFT_FFW6,
	"����speed timeshift����8���ٿ������Ƶ���̰�8���ٿ��������Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftSpeedReplay(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_FFW,CHDRV_PVR_REPLAY_SPEED_8));
	LOG("8���ٿ��\n");
}

CASE_F(TEST_PVR_TIMESHIFT_FFW7,
	"����get timeshift status����õ�rec״̬ӦΪRECORDING,����ʱʱ����¼�Ƶ�ʱ�����ļ���С����õ�replay״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("��ǰ¼��ʱ��: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("��ǰ¼�ƴ�С: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("��ǰ�طŴ�С: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);
	}

}

CASE_F(TEST_PVR_TIMESHIFT_FFW8,
	"����speed timeshift����16���ٿ������Ƶ���̰�16���ٿ��������Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftSpeedReplay(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_FFW,CHDRV_PVR_REPLAY_SPEED_16));
	LOG("16���ٿ��\n");
}

CASE_F(TEST_PVR_TIMESHIFT_FFW9,
	"����get timeshift status����õ�rec״̬ӦΪRECORDING,����ʱʱ����¼�Ƶ�ʱ�����ļ���С����õ�replay״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("��ǰ¼��ʱ��: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("��ǰ¼�ƴ�С: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("��ǰ�طŴ�С: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);
	}

}

CASE_F(TEST_PVR_TIMESHIFT_FFW10,
	"����speed timeshift����32���ٿ������Ƶ���̰�32���ٿ��������Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftSpeedReplay(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_FFW,CHDRV_PVR_REPLAY_SPEED_32));
	LOG("32���ٿ��\n");
}

CASE_F(TEST_PVR_TIMESHIFT_FFW11,
	"����get timeshift status����õ�rec״̬ӦΪRECORDING,����ʱʱ����¼�Ƶ�ʱ�����ļ���С����õ�replay״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("��ǰ¼��ʱ��: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("��ǰ¼�ƴ�С: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("��ǰ�طŴ�С: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);
	}

}

CASE_F(TEST_PVR_TIMESHIFT_FFW12,
	"����speed timeshift����1���ٿ��������ƵӦ������������") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftSpeedReplay(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_FFW,CHDRV_PVR_REPLAY_SPEED_1));
	LOG("1���ٿ��,����ƵӦ������������\n");
}

CASE_F(TEST_PVR_TIMESHIFT_FFW13,
	"����get timeshift status����õ�rec״̬ӦΪRECORDING,����ʱʱ����¼�Ƶ�ʱ�����ļ���С����õ�replay״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("��ǰ¼��ʱ��: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("��ǰ¼�ƴ�С: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("��ǰ�طŴ�С: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);
	}
}

CASE_F(TEST_PVR_TIMESHIFT_FFW14,
	"����timeshift stop������ʱ������ֹͣ��") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftStop(rh_TimeshiftHandle));
	
}


CASE_F(TEST_PVR_TIMESHIFT_FFW15,
	"����timeshift close����������OK��") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftClose(rh_TimeshiftHandle));//called in CHDRV_PVR_TimeShiftStop

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Term());
	
	Timeshift_Res_Term();
}

#endif

//156 
#if 1
CASE_F(TEST_PVR_TIMESHIFT_FBW1,
	"speed timeshift����Ϊ����ģʽ������TEST_PVR_TIMESHIFT_FFW1���������²���") {

	//Auto_Play_DTV();
	
//TEST_PVR_TIMESHIFT1
    CHDRV_PVR_Init();
	//Set_RECStartParam(PVR_NORMAL_REC_TYPE);


//TEST_PVR_TIMESHIFT2

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftOpen(NULL,NULL,(CHDRV_PVR_TIMESHIFT_CALLBACK)TimeShiftCB,&rh_TimeshiftHandle));

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
	EXPECT_EQ(CHDRV_PVR_REC_STATUS_IDLE, rpstru_Status_timeshift.enm_RECStatus); 

//TEST_PVR_TIMESHIFT3	
	CH_RTOS_TaskDelay(5000);
	Auto_Stop_DTV();
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftStart(rh_TimeshiftHandle,&RecParams));
	
	//EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftReplay2Pause(rh_TimeshiftHandle));

	LOG("ʱ�ƿ�ʼ���ھ�֡״̬,Y or N ?\n");

//TEST_PVR_TIMESHIFT4

	S32 i_Loop = 0;
	LOG("����¼��Լ15���ӵ���Ƶ�������ĵȺ�\n");
	for(i_Loop = 0;i_Loop < PVR_RECTIME_LEN;i_Loop++)
	{
		CH_RTOS_TaskDelay(1000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PAUSE, rpstru_Status_timeshift.enm_PlayStatus);
		if(i_Loop < 10)
		{
			LOG("��ǰ¼��ʱ��: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
			LOG("��ǰ¼�ƴ�С: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		}	
	}
	
//TEST_PVR_TIMESHIFT5

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftReplayControl(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_PLAY));
	LOG("����ƵӦ������������\n");

//TEST_PVR_TIMESHIFT6

	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("��ǰ¼��ʱ��: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("��ǰ¼�ƴ�С: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("��ǰ�طŴ�С: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);		
	}

}
CASE_F(TEST_PVR_TIMESHIFT_FBW2,
	"����speed timeshift����2���ٿ��ˣ���Ƶ���̰�2���ٿ��ˣ�����Ϊ����") {

	LOG("��ת���ļ�β�����ٿ���\n");
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplaySeek(rh_TimeshiftHandle,PVR_RECTIME_LEN));
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftSpeedReplay(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_FBW,CHDRV_PVR_REPLAY_SPEED_2));
	LOG("2���ٿ���\n");
}

CASE_F(TEST_PVR_TIMESHIFT_FBW3,
	"����get timeshift status����õ�rec״̬ӦΪRECORDING,����ʱʱ����¼�Ƶ�ʱ�����ļ���С����õ�replay״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("��ǰ¼��ʱ��: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("��ǰ¼�ƴ�С: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("��ǰ�طŴ�С: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);	
	}
}

CASE_F(TEST_PVR_TIMESHIFT_FBW4,
	"����speed timeshift����4���ٿ��ˣ���Ƶ���̰�4���ٿ��ˣ�����Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftSpeedReplay(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_FBW,CHDRV_PVR_REPLAY_SPEED_4));
	LOG("4���ٿ��\n");
}

CASE_F(TEST_PVR_TIMESHIFT_FBW5,
	"����get timeshift status����õ�rec״̬ӦΪRECORDING,����ʱʱ����¼�Ƶ�ʱ�����ļ���С����õ�replay״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("��ǰ¼��ʱ��: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("��ǰ¼�ƴ�С: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("��ǰ�طŴ�С: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);		
	}

}

CASE_F(TEST_PVR_TIMESHIFT_FBW6,
	"����speed timeshift����8���ٿ��ˣ���Ƶ���̰�8���ٿ��ˣ�����Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftSpeedReplay(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_FBW,CHDRV_PVR_REPLAY_SPEED_8));
	LOG("8���ٿ��\n");
}

CASE_F(TEST_PVR_TIMESHIFT_FBW7,
	"����get timeshift status����õ�rec״̬ӦΪRECORDING,����ʱʱ����¼�Ƶ�ʱ�����ļ���С����õ�replay״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("��ǰ¼��ʱ��: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("��ǰ¼�ƴ�С: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("��ǰ�طŴ�С: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);	
	}

}

CASE_F(TEST_PVR_TIMESHIFT_FBW8,
	"����speed timeshift����16���ٿ��ˣ���Ƶ���̰�16���ٿ��ˣ�����Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftSpeedReplay(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_FBW,CHDRV_PVR_REPLAY_SPEED_16));
	LOG("16���ٿ��\n");
}

CASE_F(TEST_PVR_TIMESHIFT_FBW9,
	"����get timeshift status����õ�rec״̬ӦΪRECORDING,����ʱʱ����¼�Ƶ�ʱ�����ļ���С����õ�replay״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("��ǰ¼��ʱ��: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("��ǰ¼�ƴ�С: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("��ǰ�طŴ�С: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);		
	}

}

CASE_F(TEST_PVR_TIMESHIFT_FBW10,
	"����speed timeshift����32���ٿ��ˣ���Ƶ���̰�32���ٿ��ˣ�����Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftSpeedReplay(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_FBW,CHDRV_PVR_REPLAY_SPEED_32));
	LOG("32���ٿ��\n");
}

CASE_F(TEST_PVR_TIMESHIFT_FBW11,
	"����get timeshift status����õ�rec״̬ӦΪRECORDING,����ʱʱ����¼�Ƶ�ʱ�����ļ���С����õ�replay״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("��ǰ¼��ʱ��: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("��ǰ¼�ƴ�С: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("��ǰ�طŴ�С: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);	
	}

}

CASE_F(TEST_PVR_TIMESHIFT_FBW12,
	"����speed timeshift����1���ٿ��ˣ�����ƵӦ������������") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftSpeedReplay(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_FBW,CHDRV_PVR_REPLAY_SPEED_1));
	LOG("1���ٿ��,����ƵӦ������������\n");
}

CASE_F(TEST_PVR_TIMESHIFT_FBW13,
	"����get timeshift status����õ�rec״̬ӦΪRECORDING,����ʱʱ����¼�Ƶ�ʱ�����ļ���С����õ�replay״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("��ǰ¼��ʱ��: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("��ǰ¼�ƴ�С: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("��ǰ�طŴ�С: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);	
	}
}

CASE_F(TEST_PVR_TIMESHIFT_FBW14,
	"����timeshift stop������ʱ������ֹͣ��") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftStop(rh_TimeshiftHandle));
	
}


CASE_F(TEST_PVR_TIMESHIFT_FBW16,
	"����timeshift close����������OK��") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftClose(rh_TimeshiftHandle));//called in CHDRV_PVR_TimeShiftStop

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Term());

	Timeshift_Res_Term();
}

#endif

//158
#if 1
CASE_F(TEST_PVR_TIMESHIFT_SFW1,
	"speed timeshift����Ϊ����ģʽ������TEST_PVR_TIMESHIFT_FFW1���������²���") {

	//Auto_Play_DTV();
	
//TEST_PVR_TIMESHIFT1
    CHDRV_PVR_Init();
	//Set_RECStartParam(PVR_NORMAL_REC_TYPE);


//TEST_PVR_TIMESHIFT2

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftOpen(NULL,NULL,(CHDRV_PVR_TIMESHIFT_CALLBACK)TimeShiftCB,&rh_TimeshiftHandle));

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
	EXPECT_EQ(CHDRV_PVR_REC_STATUS_IDLE, rpstru_Status_timeshift.enm_RECStatus); 

//TEST_PVR_TIMESHIFT3	
	CH_RTOS_TaskDelay(5000);
	Auto_Stop_DTV();
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftStart(rh_TimeshiftHandle,&RecParams));
	
	//EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftReplay2Pause(rh_TimeshiftHandle));

//TEST_PVR_TIMESHIFT4

	S32 i_Loop = 0;
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PAUSE, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("��ǰ¼��ʱ��: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("��ǰ¼�ƴ�С: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);		
	}
	
//TEST_PVR_TIMESHIFT5

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftReplayControl(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_PLAY));
	LOG("����ƵӦ������������\n");

//TEST_PVR_TIMESHIFT6

	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("��ǰ¼��ʱ��: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("��ǰ¼�ƴ�С: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("��ǰ�طŴ�С: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);	
	}

}
CASE_F(TEST_PVR_TIMESHIFT_SFW2,
	"����speed timeshift����1/2�������ţ���Ƶ���̰�1/2�������ţ�����Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftSpeedReplay(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_SFW,CHDRV_PVR_REPLAY_SPEED_2));
	LOG("1/2��������\n");
}

CASE_F(TEST_PVR_TIMESHIFT_SFW3,
	"����get timeshift status����õ�rec״̬ӦΪRECORDING,����ʱʱ����¼�Ƶ�ʱ�����ļ���С����õ�replay״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("��ǰ¼��ʱ��: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("��ǰ¼�ƴ�С: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("��ǰ�طŴ�С: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);
	}
}

CASE_F(TEST_PVR_TIMESHIFT_SFW4,
	"����speed timeshift����1/4�������ţ���Ƶ���̰�1/4�������ţ�����Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftSpeedReplay(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_SFW,CHDRV_PVR_REPLAY_SPEED_4));
	LOG("1/4��������\n");
}

CASE_F(TEST_PVR_TIMESHIFT_SFW5,
	"����get timeshift status����õ�rec״̬ӦΪRECORDING,����ʱʱ����¼�Ƶ�ʱ�����ļ���С����õ�replay״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("��ǰ¼��ʱ��: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("��ǰ¼�ƴ�С: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("��ǰ�طŴ�С: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);
	}

}

CASE_F(TEST_PVR_TIMESHIFT_SFW6,
	"����speed timeshift����1/8�������ţ���Ƶ���̰�1/8�������ţ�����Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftSpeedReplay(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_SFW,CHDRV_PVR_REPLAY_SPEED_8));
	LOG("1/8��������\n");
}

CASE_F(TEST_PVR_TIMESHIFT_SFW7,
	"����get timeshift status����õ�rec״̬ӦΪRECORDING,����ʱʱ����¼�Ƶ�ʱ�����ļ���С����õ�replay״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("��ǰ¼��ʱ��: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("��ǰ¼�ƴ�С: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("��ǰ�طŴ�С: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);	
	}

}

CASE_F(TEST_PVR_TIMESHIFT_SFW8,
	"����speed timeshift����1/16�������ţ���Ƶ���̰�1/16�������ţ�����Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftSpeedReplay(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_SFW,CHDRV_PVR_REPLAY_SPEED_16));
	LOG("1/16��������\n");
}

CASE_F(TEST_PVR_TIMESHIFT_SFW9,
	"����get timeshift status����õ�rec״̬ӦΪRECORDING,����ʱʱ����¼�Ƶ�ʱ�����ļ���С����õ�replay״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("��ǰ¼��ʱ��: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("��ǰ¼�ƴ�С: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("��ǰ�طŴ�С: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);	
	}

}

CASE_F(TEST_PVR_TIMESHIFT_SFW10,
	"����speed timeshift����1/32�������ţ���Ƶ���̰�1/32�������ţ�����Ϊ����") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftSpeedReplay(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_SFW,CHDRV_PVR_REPLAY_SPEED_32));
	LOG("1/32��������\n");
}

CASE_F(TEST_PVR_TIMESHIFT_SFW11,
	"����get timeshift status����õ�rec״̬ӦΪRECORDING,����ʱʱ����¼�Ƶ�ʱ�����ļ���С����õ�replay״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("��ǰ¼��ʱ��: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("��ǰ¼�ƴ�С: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("��ǰ�طŴ�С: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);	
	}

}

CASE_F(TEST_PVR_TIMESHIFT_SFW12,
	"����speed timeshift����1�������ţ�����ƵӦ������������") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftSpeedReplay(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_SFW,CHDRV_PVR_REPLAY_SPEED_1));
	LOG("1���ٿ��,����ƵӦ������������\n");
}

CASE_F(TEST_PVR_TIMESHIFT_SFW13,
	"����get timeshift status����õ�rec״̬ӦΪRECORDING,����ʱʱ����¼�Ƶ�ʱ�����ļ���С����õ�replay״̬ӦΪSPEED,����ʱʱ���»طŵ�ʱ�����ļ���С") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("��ǰ¼��ʱ��: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("��ǰ¼�ƴ�С: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("��ǰ�طŴ�С: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);	
	}
}

CASE_F(TEST_PVR_TIMESHIFT_SFW14,
	"����timeshift stop������ʱ������ֹͣ��") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftStop(rh_TimeshiftHandle));
	
}


CASE_F(TEST_PVR_TIMESHIFT_SFW15,
	"����timeshift close����������OK��") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftClose(rh_TimeshiftHandle));//called in CHDRV_PVR_TimeShiftStop

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Term());

	Timeshift_Res_Term();
}

#endif

//162 - 167
#if 1
CASE_F(TEST_PVR_TIMESHIFT_SEEK1,
	"�ظ�����TEST_PVR_TIMESHIFT1-TEST_PVR_TIMESHIFT6") {

	//Auto_Play_DTV();
	
//TEST_PVR_TIMESHIFT1
    CHDRV_PVR_Init();
	//Set_RECStartParam(PVR_NORMAL_REC_TYPE);


//TEST_PVR_TIMESHIFT2

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftOpen(NULL,NULL,(CHDRV_PVR_TIMESHIFT_CALLBACK)TimeShiftCB,&rh_TimeshiftHandle));

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
	EXPECT_EQ(CHDRV_PVR_REC_STATUS_IDLE, rpstru_Status_timeshift.enm_RECStatus); 

//TEST_PVR_TIMESHIFT3	
	CH_RTOS_TaskDelay(5000);
	Auto_Stop_DTV();
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftStart(rh_TimeshiftHandle,&RecParams));
	
	//EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftReplay2Pause(rh_TimeshiftHandle));

//TEST_PVR_TIMESHIFT4

	S32 i_Loop = 0;
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PAUSE, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("��ǰ¼��ʱ��: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("��ǰ¼�ƴ�С: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);	
	}
	
//TEST_PVR_TIMESHIFT5

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftReplayControl(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_PLAY));
	LOG("����ƵӦ������������\n");

//TEST_PVR_TIMESHIFT6

	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("��ǰ¼��ʱ��: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("��ǰ¼�ƴ�С: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("��ǰ�طŴ�С: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);
	}

}
CASE_F(TEST_PVR_TIMESHIFT_SEEK2,
	"����replay seek��ת��ָ����λ�ã�����ƵӦ������ת����λ����������") {

	S32 ri_DstTimeS = 10;
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplaySeek(rh_TimeshiftHandle,ri_DstTimeS));
}

CASE_F(TEST_PVR_TIMESHIFT_SEEK3,
	"����get timeshift status����õ�rec״̬ӦΪRECORDING,����ʱʱ����¼�Ƶ�ʱ�����ļ���С����õ�replay״̬ӦΪPLAYING,����ʱʱ���»طŵ�ʱ�����ļ���С") {


	S32 i_Loop = 0;
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(1000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("��ǰ¼��ʱ��: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("��ǰ¼�ƴ�С: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("��ǰ�ط�ʱ��: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("��ǰ�طŴ�С: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);	
	}
}


CASE_F(TEST_PVR_TIMESHIFT_SEEK4,
	"����timeshift stop������ʱ������ֹͣ��") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftStop(rh_TimeshiftHandle));
	
}
CASE_F(TEST_PVR_TIMESHIFT_SEEK5,
	"����timeshift close����������OK��") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftClose(rh_TimeshiftHandle));//called in CHDRV_PVR_TimeShiftStop
	CHDRV_PVR_Term();
}

CASE_F(TEST_PVR_GET_TIMESHIFT_RES_INFO,
	"����get res info������ȷ��ȡ���ո���¼�ƵĽ�Ŀ����ϸ��Ϣ") {

	EXPECT_EQ(CHDRV_PVR_NOT_SUPPORT, CHDRV_PVR_GetResInfo(NULL,NULL,NULL,NULL));
	//Auto_Stop_DTV();

	Timeshift_Res_Term();
}

#endif

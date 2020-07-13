/**
@file test_pvr.cpp
@def group pvr测试集

@author 网络公司基础技术部:	邹书强
@date   2014年4月10日  
@version V4.0
@todo     ---时间---作者-------------修改部分及原因----------
@todo     1 2014年4月10日 邹书强  创建


@{
*/
using namespace std;
/*测试集名称定义*/
#ifdef CASE_NAME
#undef CASE_NAME
#endif
#define CASE_NAME TestPvr

#include "test_menu.h"
#include "test_modules.h"
#include "chdrv_pvr.h"
//定义错误参数
#define  PVR_ERRO_REC_CMD  						  (CHDRV_PVR_REC_CMD_MAX)
#define  PVR_ERRO_REPLAY_CMD  					(CHDRV_PVR_REPLAY_CMD_MAX)
#define  PVR_ERRO_REC_STATUS  					(CHDRV_PVR_REC_STATUS_MAX)
#define  PVR_ERRO_REPLAY_STATUS  				(CHDRV_PVR_REPLAY_STATUS_MAX)
#define  PVR_ERRO_REPLAY_SPEED     				(CHDRV_PVR_REPLAY_SPEED_MAX)
#define  PVR_ERRO_REPLAY_TYPE     				(CHDRV_PVR_REPLAY_TYPE_MAX)
//定义正确参数
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

//功能和test_tuner.cpp的parse_key 一模一样，解析config
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

/* 从config文件读取录制的ts信息 */
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
	RUN_CMD("av.start %s",fw_getenv("PVR_DTV_PROGNUM"));			//播放节目

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
RUN_CMD("av.start %d %d\n",fw_getenv_i("TUNER_DEVICE_ID"),prognum);	//播放节目

return CHDRV_PVR_OK;
}

#endif
static CHDRV_PVR_RESULT_e Auto_Stop_DTV()
{
	RUN_CMD("av.stop %d",0);		
	CHDRV_TUNER_Term();
	
	return CHDRV_PVR_OK;
}
//录制结束释放资源
static CHDRV_PVR_RESULT_e Rec_Res_Term()
{
	rh_RECHandle = NULL;
	return CHDRV_PVR_OK;
}
/*回放结束释放资源，让下次录制可以启动*/
static CHDRV_PVR_RESULT_e Replay_Res_Term()
{
	rh_ReplayHandle = NULL;
	CH_FSYS_DeleteFile((const S8 *)"/tmp/root_0_0/TS/record.ts",NULL,NULL);
	CH_FSYS_DeleteFile((const S8 *)"/tmp/root_0_0/TS/record.ts.meta",NULL,NULL);
	
	return CHDRV_PVR_OK;
}
/*时移结束释放资源，让下次时移可以启动*/
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
			
			printf("停止录制\n");
			break;
		
		case CHDRV_PVR_REC_EVENT_PAUSE:	
			
			printf("暂停\n");
			break;
		case CHDRV_PVR_REC_EVENT_PROCESS:
			
			printf("录制过程中\n");
			break;
		
		case CHDRV_PVR_REC_EVENT_NODATA:	
			
			printf("前端没有数据过来\n");
			break;
		case CHDRV_PVR_REC_EVENT_DATA_RESUME:
			
			printf("前端数据恢复\n");
			break;
		
		case CHDRV_PVR_REC_EVENT_DISKFULL:	
			
			printf(" 没有录制空间,剩余空间为0Byte\n");
			break;

		case CHDRV_PVR_REC_EVENT_DISKFULL_WARNING:
			
			printf("磁盘剩余空间到达警戒线\n");
			break;
		
		case CHDRV_PVR_REC_EVENT_ERROR:	
			
			printf("其他错误导致录制不能继续\n");
			break;
		case CHDRV_PVR_REC_EVENT_DEVICE_REMOVED:
			
			printf("存储设备被移除\n");
			break;
		
		case CHDRV_PVR_REC_EVENT_TIMEOUT:	
			
			printf("定时录制时间到\n");
			break;
		case CHDRV_PVR_REC_EVENT_FILE_WRITE_ERROR:
			
			printf("文件写错误\n");
			break;
		
		case CHDRV_PVR_REC_EVENT_WRITE_SPEEDLESS:	
			
			printf("硬盘文件写速度不足\n");
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
			
			printf("停止回放\n");
			break;
		
		case CHDRV_PVR_REPLAY_EVENT_PAUSE:	
			
			printf("暂停回放\n");
			break;
		case CHDRV_PVR_REPLAY_EVENT_PLAYING:
			
			printf("回放过程中\n");
			break;
		
		case CHDRV_PVR_REPLAY_EVENT_FILE_TAIL:
			
			printf("前端数据恢复\n");
			break;
		
		case CHDRV_PVR_REPLAY_EVENT_FILE_HEAD:	
			
			printf(" 快退时到达文件头，将转为正常播放\n");
			break;

		case CHDRV_PVR_REPLAY_EVENT_FILE_READ_ERROR:
			
			printf("文件读错误\n");
			break;
		
		case CHDRV_PVR_REPLAY_EVENT_ERROR:	
			
			printf("其他错误导致播放不成功\n");
			break;
		case CHDRV_PVR_REPLAY_EVENT_DEVICE_REMOVED:
			
			printf("存储设备被移除\n");
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
			
			printf("停止回放\n");
			break;
		
		case CHDRV_PVR_REPLAY_EVENT_PAUSE:	
			
			printf("暂停回放\n");
			break;
		case CHDRV_PVR_REPLAY_EVENT_PLAYING:
			
			printf("回放过程中\n");
			break;
		
		case CHDRV_PVR_REPLAY_EVENT_FILE_TAIL:
			
			printf("前端数据恢复\n");
			break;
		
		case CHDRV_PVR_REPLAY_EVENT_FILE_HEAD:	
			
			printf(" 快退时到达文件头，将转为正常播放\n");
			break;

		case CHDRV_PVR_REPLAY_EVENT_FILE_READ_ERROR:
			
			printf("文件读错误\n");
			break;
		
		case CHDRV_PVR_REPLAY_EVENT_ERROR:	
			
			printf("其他错误导致播放不成功\n");
			break;
		case CHDRV_PVR_REPLAY_EVENT_DEVICE_REMOVED:
			
			printf("存储设备被移除\n");
			break;
		default:
			printf("unknown event\n");
            return ;
	
	}
	return ;
}

/*开始测试之前，自动播放DTV，配置rec 信息*/
void CASE_NAME::SetUpTestCase() {
//excute before each test suit
	std::cout << "init SetUp pvr TestCase " << std::endl;

	
	Auto_Play_DTV();
	CH_FSYS_Init();
	Set_RECStartParam(PVR_NORMAL_REC_TYPE);//普通录制
	CH_FSYS_DeleteFile((const S8 *)"/tmp/root_0_0/TS/record.ts",NULL,NULL);
	CH_FSYS_DeleteFile((const S8 *)"/tmp/root_0_0/TS/record.ts.meta",NULL,NULL);
	CH_FSYS_DeleteFile((const S8 *)"/tmp/root_0_0/TS/record.ts.pmt",NULL,NULL);
	CH_FSYS_DeleteFile((const S8 *)"/tmp/root_0_0/TS/record.ts.idx",NULL,NULL);
	
}

/*结束测试，自动停止DTV*/
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

// 2~32   (编号为FPE测试系统_功能模块测试用例V0.1的测试用例编号)
#if 1
CASE_F(TEST_PVR_INIT1,
	"测试多次初始化时是否返回OK") {
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Init());
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Init()) << "多次初始化应该返回OK";
}

CASE_F(TEST_PVR_INIT2,
	"测试模块是否可被销毁") {
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Init());
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Term());
}


CASE_F(TEST_PVR_INIT3,
	"测试CHDRV_PVR_GetVersion是否成功") {
	CHDRV_Version_t stru_Ver;
	
	memset(&stru_Ver, 0, sizeof(stru_Ver));
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetVersion(&stru_Ver));
	EXPECT_NE(0, stru_Ver);
	LOG("\nstru_Ver=%x\n", stru_Ver);
}

CASE_F(TEST_PVR_INIT4,
	"测试没有初始化时，其他函数是否能调用成功") {

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
	"测试调用CHDRV_PVR_Term后，再调用其他函数是否能成功") {

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

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Init());// 为后续测试项准备
	

}


CASE_F(TEST_PVR_PARAMS1,
	"CHDRV_PVR_SetConfig错误参数测试") {

	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_SetConfig(NULL));
}


CASE_F(TEST_PVR_PARAMS2,
	"CHDRV_PVR_GetConfig错误参数测试") {

	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_GetConfig(NULL)); //
}

CASE_F(TEST_PVR_PARAMS3,
	"CHDRV_PVR_ResIsIntact错误参数测试") {
	//EXPECT_EQ(CHDRV_PVR_NOT_SUPPORT, CHDRV_PVR_ResIsIntact(NULL)); // porting not support
}

CASE_F(TEST_PVR_PARAMS4,
	"CHDRV_PVR_GetResInfo错误参数测试") {
	//EXPECT_EQ(CHDRV_PVR_NOT_SUPPORT, CHDRV_PVR_GetResInfo(NULL,NULL,NULL,NULL)); // porting not support
	//EXPECT_EQ(CHDRV_PVR_NOT_SUPPORT, CHDRV_PVR_GetResInfo(NULL,NULL,NULL,NULL));
	//EXPECT_EQ(CHDRV_PVR_NOT_SUPPORT, CHDRV_PVR_GetResInfo(NULL,NULL,NULL,NULL));
	//EXPECT_EQ(CHDRV_PVR_NOT_SUPPORT, CHDRV_PVR_GetResInfo(NULL,NULL,NULL,NULL));
}

CASE_F(TEST_PVR_PARAMS5,
	"CHDRV_PVR_GetResInfo错误参数测试") {
	//EXPECT_EQ(CHDRV_PVR_NOT_SUPPORT, CHDRV_PVR_RepairResInfo(NULL,NULL)); // porting not support
	//EXPECT_EQ(CHDRV_PVR_NOT_SUPPORT, CHDRV_PVR_RepairResInfo(NULL,NULL)); 
}



CASE_F(TEST_PVR_PARAMS6,
	"CHDRV_PVR_RECOpen错误参数测试") {

	//CHDRV_ROUTE_Handle rh_RouteHandle;
	//CHDRV_PVR_Handle rh_RECHandle ;
	//EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_RECOpen(NULL,NULL,&rh_RECHandle)); //检测2个错误参数
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_RECOpen(NULL,(CHDRV_PVR_REC_CALLBACK)RecordCB,NULL));//porting应该实现检查null handle
}


CASE_F(TEST_PVR_PARAMS7,
	"CHDRV_PVR_RECStart错误参数测试") {

	CHDRV_PVR_Handle rh_RECHandle = NULL;
	CHDRV_PVR_RECStartParam_t rpstru_RecParam;
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECOpen(NULL,(CHDRV_PVR_REC_CALLBACK)RecordCB,&rh_RECHandle)); 
	
	
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_RECStart(rh_RECHandle,NULL)); //检测第1个错误参数
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_RECStart(NULL,&rpstru_RecParam)); //检测第2个错误参数
	//free
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECClose(rh_RECHandle));
	
}


CASE_F(TEST_PVR_PARAMS8,
	"CHDRV_PVR_RECControl错误参数测试") {

	CHDRV_PVR_Handle rh_RECHandle = NULL;
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECOpen(NULL,(CHDRV_PVR_REC_CALLBACK)RecordCB,&rh_RECHandle)); 
	
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_RECControl(NULL,PVR_NORMAL_REC_CMD)); //检测第1个错误参数
	EXPECT_EQ(CHDRV_PVR_FAIL, CHDRV_PVR_RECControl(rh_RECHandle,PVR_ERRO_REC_CMD)); //检测第2个错误参数

	//free
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECClose(rh_RECHandle));
}

CASE_F(TEST_PVR_PARAMS9,
	"CHDRV_PVR_GetRECStatus错误参数测试") {

	CHDRV_PVR_Handle rh_RECHandle = NULL;
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECOpen(NULL,(CHDRV_PVR_REC_CALLBACK)RecordCB,&rh_RECHandle)); 
		
	CHDRV_PVR_RECStatus_t rpstru_Status;
	
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_GetRECStatus(rh_RECHandle,NULL)); //检测第1个错误参数
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_GetRECStatus(NULL,&rpstru_Status)); //检测第2个错误参数

	//释放句柄
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECClose(rh_RECHandle));
}

CASE_F(TEST_PVR_PARAMS10,
	"CHDRV_PVR_RECStop错误参数测试") {

	CHDRV_PVR_Handle rh_RECHandle = NULL;
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECOpen(NULL,(CHDRV_PVR_REC_CALLBACK)RecordCB,&rh_RECHandle));
	
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_RECStop(NULL)); //检测第1个错误参数

	//释放句柄
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECClose(rh_RECHandle));
}

CASE_F(TEST_PVR_PARAMS11,
	"CHDRV_PVR_RECClose错误参数测试") {

	CHDRV_PVR_Handle rh_RECHandle = NULL;
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECOpen(NULL,(CHDRV_PVR_REC_CALLBACK)RecordCB,&rh_RECHandle));//需重新考虑下是否执行g_RecClosed = CH_FALSE
	
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_RECClose(NULL)); 
	
	//被_RecClosed == CH_TRUE挡回，不能释放rh_RECHandle。open 成功之后，close应该也要成功。replay和timeshift 同理
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECClose(rh_RECHandle));
}



CASE_F(TEST_PVR_PARAMS12,
	"CHDRV_PVR_RECClose错误参数测试") {

	CHDRV_PVR_Handle rph_ReplayHandle = NULL;
	
	//EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_ReplayOpen(NULL,(CHDRV_PVR_REPLAY_CALLBACK)ReplayCB,&rph_ReplayHandle)); 
	//EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_ReplayOpen(NULL,NULL,&rph_ReplayHandle));
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_ReplayOpen(NULL,(CHDRV_PVR_REPLAY_CALLBACK)ReplayCB,NULL));//porting应该实现检查null handle
}


CASE_F(TEST_PVR_PARAMS13,
	"CHDRV_PVR_ReplayStart错误参数测试") {

	CHDRV_PVR_Handle rh_ReplayHandle = NULL;
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayOpen(NULL,ReplayCB,&rh_ReplayHandle));
	
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_ReplayStart(NULL,(U8*)ResPathName,NULL,PVR_CRYTPKEY_LENGTH)); //porting应该实现检查null handle

	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_ReplayStart(rh_ReplayHandle,NULL,NULL,PVR_CRYTPKEY_LENGTH)); //检测第2个错误参数

	//释放句柄
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayClose(rh_ReplayHandle));
}



CASE_F(TEST_PVR_PARAMS14,
	"CHDRV_PVR_ReplayControl错误参数测试") {

	CHDRV_PVR_Handle rh_ReplayHandle = NULL;
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayOpen(NULL,ReplayCB,&rh_ReplayHandle));
	
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_ReplayControl(NULL,PVR_NORMAL_REPLAY_CMD)); //检测第1个错误参数
	EXPECT_EQ(CHDRV_PVR_FAIL, CHDRV_PVR_ReplayControl(rh_ReplayHandle,PVR_ERRO_REPLAY_CMD)); //检测第2个错误参数

	//释放句柄
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayClose(rh_ReplayHandle));
}


CASE_F(TEST_PVR_PARAMS15,
	"CHDRV_PVR_SpeedReplay错误参数测试") {

	CHDRV_PVR_Handle rh_ReplayHandle = NULL;
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayOpen(NULL,ReplayCB,&rh_ReplayHandle));
	
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_SpeedReplay(NULL,PVR_NORMAL_REPLAY_TYPE,PVR_NORMAL_REPLAY_SPEED)); //检测第1个错误参数
	EXPECT_EQ(CHDRV_PVR_FAIL, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,PVR_ERRO_REPLAY_TYPE,PVR_NORMAL_REPLAY_SPEED)); //检测第2个错误参数
	EXPECT_EQ(CHDRV_PVR_FAIL, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,PVR_NORMAL_REPLAY_TYPE,PVR_ERRO_REPLAY_SPEED)); //检测第3个错误参数

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayClose(rh_ReplayHandle));
}

CASE_F(TEST_PVR_PARAMS16,
	"CHDRV_PVR_ReplaySeek错误参数测试") {

	S32 ri_DstTimeS = 0;
	
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_ReplaySeek(NULL,ri_DstTimeS)); //检测第1个错误参数
	//EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_ReplaySeek(rh_ReplayHandle,-10));

}


CASE_F(TEST_PVR_PARAMS17,
	"CHDRV_PVR_GetReplayStatus错误参数测试") {

	CHDRV_PVR_Handle rh_ReplayHandle = NULL;
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayOpen(NULL,ReplayCB,&rh_ReplayHandle));
	
	CHDRV_PVR_ReplayStatus_t  rpstru_Status;
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_GetReplayStatus(NULL,&rpstru_Status)); //检测第1个错误参数
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,NULL)); //检测第2个错误参数

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayClose(rh_ReplayHandle));
}


CASE_F(TEST_PVR_PARAMS18,
	"CHDRV_PVR_ReplayStop错误参数测试") {

	CHDRV_PVR_Handle rh_ReplayHandle = NULL;
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayOpen(NULL,ReplayCB,&rh_ReplayHandle));
	
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_ReplayStop(NULL)); 

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayClose(rh_ReplayHandle));
	
	
}

CASE_F(TEST_PVR_PARAMS19,
	"CHDRV_PVR_ReplayClose错误参数测试") {

	CHDRV_PVR_Handle rh_ReplayHandle = NULL;
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayOpen(NULL,ReplayCB,&rh_ReplayHandle));
	
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_ReplayClose(NULL));

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayClose(rh_ReplayHandle));
	
}


CASE_F(TEST_PVR_PARAMS20,
	"CHDRV_PVR_TimeShiftOpen错误参数测试") {

	CHDRV_PVR_Handle rh_TimeShiftHandle = NULL;
	
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_TimeShiftOpen(NULL,NULL,(CHDRV_PVR_TIMESHIFT_CALLBACK)TimeShiftCB,NULL));//core dump  because null handle
	//EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_TimeShiftOpen(NULL,NULL,NULL,&rh_TimeShiftHandle));

	
}

CASE_F(TEST_PVR_PARAMS21,
	"CHDRV_PVR_TimeShiftStart错误参数测试") {

	CHDRV_PVR_RECStartParam_t rpstru_TimeshiftParam;
	CHDRV_PVR_Handle rh_TimeShiftHandle = NULL;
	//Replay is opened!
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftOpen(NULL,NULL,(CHDRV_PVR_TIMESHIFT_CALLBACK)TimeShiftCB,&rh_TimeShiftHandle)); 
	
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_TimeShiftStart(NULL,&rpstru_TimeshiftParam)); //检测第1个错误参数
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_TimeShiftStart(rh_TimeShiftHandle,NULL)); //检测第2个错误参数

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftClose(rh_TimeShiftHandle)); 
	
}
CASE_F(TEST_PVR_PARAMS22,
	"CHDRV_PVR_TimeShiftReplayControl错误参数测试") {

	CHDRV_PVR_Handle rh_TimeShiftHandle = NULL;
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftOpen(NULL,NULL,(CHDRV_PVR_TIMESHIFT_CALLBACK)TimeShiftCB,&rh_TimeShiftHandle)); //Replay is opened!
		
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_TimeShiftReplayControl(NULL,PVR_NORMAL_REPLAY_CMD)); //检测第1个错误参数
	EXPECT_EQ(CHDRV_PVR_FAIL, CHDRV_PVR_TimeShiftReplayControl(rh_TimeShiftHandle,PVR_ERRO_REPLAY_CMD)); //检测第2个错误参数
	printf("\nrh_TimeShiftHandle=0x%x\n",rh_TimeShiftHandle);
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftClose(rh_TimeShiftHandle)); 
	
}

CASE_F(TEST_PVR_PARAMS23,
	"CHDRV_PVR_TimeShiftReplayControl错误参数测试") {

	CHDRV_PVR_Handle rh_TimeShiftHandle = NULL;
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftOpen(NULL,NULL,(CHDRV_PVR_TIMESHIFT_CALLBACK)TimeShiftCB,&rh_TimeShiftHandle)); 		

	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_TimeShiftSpeedReplay(NULL,PVR_NORMAL_REPLAY_TYPE,CHDRV_PVR_REPLAY_SPEED_1)); //检测第1个错误参数
	EXPECT_EQ(CHDRV_PVR_FAIL, CHDRV_PVR_TimeShiftSpeedReplay(rh_TimeShiftHandle,PVR_ERRO_REPLAY_TYPE,CHDRV_PVR_REPLAY_SPEED_1)); //检测第2个错误参数
	EXPECT_EQ(CHDRV_PVR_FAIL, CHDRV_PVR_TimeShiftSpeedReplay(rh_TimeShiftHandle,PVR_NORMAL_REPLAY_TYPE,PVR_ERRO_REPLAY_SPEED)); //

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftClose(rh_TimeShiftHandle)); 
	
}

CASE_F(TEST_PVR_PARAMS24,
	"CHDRV_PVR_TimeShiftGetStatus错误参数测试") {

	CHDRV_PVR_Handle rh_TimeShiftHandle = NULL;
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftOpen(NULL,NULL,(CHDRV_PVR_TIMESHIFT_CALLBACK)TimeShiftCB,&rh_TimeShiftHandle)); //检测第3个错误参数		

	CHDRV_PVR_TimeShiftStatus_t rpstru_TimeShiftStatus;
		
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_TimeShiftGetStatus(NULL,&rpstru_TimeShiftStatus)); //检测第1个错误参数
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_TimeShiftGetStatus(rh_TimeShiftHandle,NULL)); //检测第2个错误参数

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftClose(rh_TimeShiftHandle)); 
	
}

CASE_F(TEST_PVR_PARAMS25,
	"CHDRV_PVR_TimeShiftStop错误参数测试") {

	CHDRV_PVR_Handle rh_TimeShiftHandle = NULL;
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftOpen(NULL,NULL,(CHDRV_PVR_TIMESHIFT_CALLBACK)TimeShiftCB,&rh_TimeShiftHandle)); 
			
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_TimeShiftStop(NULL)); 

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftClose(rh_TimeShiftHandle)); 
}

CASE_F(TEST_PVR_PARAMS26,
	"CHDRV_PVR_TimeShiftClose错误参数测试") {
	
	CHDRV_PVR_Handle rh_TimeShiftHandle = NULL;
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftOpen(NULL,NULL,(CHDRV_PVR_TIMESHIFT_CALLBACK)TimeShiftCB,&rh_TimeShiftHandle));
	
	//需要考虑执行g_TimeShiftClosed = false的地方，因为CHDRV_PVR_TimeShiftOpen已经执行,rh_TimeShiftHandle不为空
	EXPECT_EQ(CHDRV_PVR_INVALID_PARAM, CHDRV_PVR_TimeShiftClose(NULL)); 
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftClose(rh_TimeShiftHandle)); //被g_TimeShiftClosed挡回，不能释放rh_TimeShiftHandle
}
#endif

// 33~44
#if 1
CASE_F(TEST_PVR_REC1,
	"调用rec open创建一个句柄，看是否返回成功，且句柄不为NULL") {

	Set_RECStartParam(PVR_NORMAL_REC_TYPE);
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Init());
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECOpen(NULL,RecordCB,&rh_RECHandle)); 
}

CASE_F(TEST_PVR_REC2,
	"调用get rec status状态应为IDL") {

	CHDRV_PVR_RECStatus_t rpstru_Status;	 
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status));
	EXPECT_EQ(CHDRV_PVR_REC_STATUS_IDLE, rpstru_Status.enm_RECStatus); 
}

CASE_F(TEST_PVR_REC3,
	"以正确的参数start rec刚刚创建的handle，文件存放路径从env获取，录制过程中音视频任然正常播放") {

    EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECStart(rh_RECHandle,&RecParams));

}

CASE_F(TEST_PVR_REC4,
	"调用get rec status，获得的状态应为RECORDING,并能时时更新录制的时长和文件大小") {

	S32 i_Loop = 0;
	LOG("录制一段约15分钟的视频,请耐心等候\n"); 
	/*rh_RECHandle 未做竞争保护，所以时间会 有些滞后*/
	for(i_Loop = 0;i_Loop < PVR_RECTIME_LEN;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status_rec));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_rec.enm_RECStatus);
		/*由于多线程的原因time偶尔delay 1 s */
		if (i_Loop < 10)//调试阶段可先只打印前几秒的信息
		{
			LOG("当前录制时长: %d\n", rpstru_Status_rec.i_RECTimeS);
			LOG("当前录制大小: %d\n", rpstru_Status_rec.i_RECFileLengthKB);
		}

		CH_RTOS_TaskDelay(1000);
	}
	
}

CASE_F(TEST_PVR_REC5,
	"调用rec control暂停录制，录制暂停过程中音视频任然正常播放") {
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECControl(rh_RECHandle,CHDRV_PVR_REC_PAUSE));
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status_rec));
	printf("\ni_RECFileLengthKB=%d,i_RECTimeS=%d\n",rpstru_Status_rec.i_RECFileLengthKB,rpstru_Status_rec.i_RECTimeS);
	
	LOG("暂停录制10s\n");
	CH_RTOS_TaskDelay(10*1000);
	
}

CASE_F(TEST_PVR_REC6,
	"调用get rec status，获得的状态应为PAUSE,录制的时长和文件大小保持暂停之前不变") {

	CHDRV_PVR_RECStatus_t rpstru_Status;
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status));
	EXPECT_EQ(CHDRV_PVR_REC_STATUS_PAUSE, rpstru_Status.enm_RECStatus); 
	EXPECT_EQ(rpstru_Status.i_RECFileLengthKB, rpstru_Status_rec.i_RECFileLengthKB);
	EXPECT_EQ(rpstru_Status.i_RECTimeS, rpstru_Status_rec.i_RECTimeS);
}


CASE_F(TEST_PVR_REC7,
	"重复调用rec control暂停录制，应返回ok") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECControl(rh_RECHandle,CHDRV_PVR_REC_PAUSE));	
}

CASE_F(TEST_PVR_REC8,
	"调用rec control恢复录制，录制过程中音视频任然正常播放") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECControl(rh_RECHandle,CHDRV_PVR_REC_RESUME));
	
}
CASE_F(TEST_PVR_REC9,
	"调用get rec status，获得的状态应为RECORDING,并能时时更新录制的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status_rec));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_rec.enm_RECStatus);
		LOG("当前录制时长: %d\n", rpstru_Status_rec.i_RECTimeS);
		LOG("当前录制大小: %d\n", rpstru_Status_rec.i_RECFileLengthKB);
		CH_RTOS_TaskDelay(1000);
	} 
}

#if 1
CASE_F(TEST_PVR_REC10,
	"重复调用rec control恢复录制，应返回ok") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECControl(rh_RECHandle,CHDRV_PVR_REC_RESUME));
	
}
#endif

CASE_F(TEST_PVR_REC11,
	"调用rec stop函数，录制立刻停止。") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECStop(rh_RECHandle));
	
}
CASE_F(TEST_PVR_REC12,
	"调用rec close函数，返回OK。") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECClose(rh_RECHandle));
	Rec_Res_Term();
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Term());
}
#if 0
CASE_F(TEST_PVR_GET_RES_INFO,
	"调用get res info，能正确获取到刚刚已录制的节目的详细信息") {

	EXPECT_EQ(CHDRV_PVR_NOT_SUPPORT, CHDRV_PVR_GetResInfo(NULL,NULL,NULL,NULL));//not complete
}
#endif
#endif

//46~57

#if 1
CASE_F(TEST_PVR_REPLAY1,
	"调用replay open创建一个句柄，看是否返回成功，且句柄不为NULL(route参数从test route模块获取)") {

	Auto_Stop_DTV();//临时办法，因为porting 未实现自动关闭DTV
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Init());
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayOpen(NULL,ReplayCB,&rh_ReplayHandle));
}

CASE_F(TEST_PVR_REPLAY2,
	"调用get replay status状态应为IDL") {
 
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
	EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_IDLE, rpstru_Status_pb.enm_PlayStatus); 
}

CASE_F(TEST_PVR_REPLAY3,
	"以正确的参数start replay刚刚创建的handle，文件播放路径从env获取，回放过程中音视频正常播放") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayStart(rh_ReplayHandle, (U8*)ResPathName, NULL, 0));

}

CASE_F(TEST_PVR_REPLAY4,
	"调用get replay status，获得的状态应为PLAYING,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_pb.enm_PlayStatus); 
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		
		
	} 
}

CASE_F(TEST_PVR_REPLAY5,
	"调用replay control暂停回放，音视频应立刻停止播放") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayControl(rh_ReplayHandle,CHDRV_PVR_REPLAY_PAUSE));
	LOG("暂停播放10s\n");
	CH_RTOS_TaskDelay(10*1000);
	
}
CASE_F(TEST_PVR_REPLAY6,
	"调用get replay status，获得的状态应为PAUSE,回放的时长和文件大小保持暂停之前不变") {

	CHDRV_PVR_ReplayStatus_t rpstru_Status;
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status));
	EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PAUSE, rpstru_Status.enm_PlayStatus); 

	EXPECT_EQ(rpstru_Status.i_PlayTimeS, rpstru_Status_pb.i_PlayTimeS);
	EXPECT_EQ(rpstru_Status.i_ReadOffsetKB, rpstru_Status_pb.i_ReadOffsetKB);
}


CASE_F(TEST_PVR_REPLAY7,
	"重复调用replay control暂停回放，应返回ok") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayControl(rh_ReplayHandle,CHDRV_PVR_REPLAY_PAUSE));	
}

CASE_F(TEST_PVR_REPLAY8,
	"调用replay control恢复回放，音视频应立刻正常播放") {

	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayControl(rh_ReplayHandle,CHDRV_PVR_REPLAY_PLAY));
	
}
CASE_F(TEST_PVR_REPLAY9,
	"调用get replay status，获得的状态应为PLAYING,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_pb.enm_PlayStatus);
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
		
	} 
}


CASE_F(TEST_PVR_REPLAY10,
	"重复调用replay control恢复回放，应返回ok") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayControl(rh_ReplayHandle,CHDRV_PVR_REPLAY_PLAY));
	
}

CASE_F(TEST_PVR_REPLAY11,
	"调用replay stop函数，回放立刻停止。") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayStop(rh_ReplayHandle));
	
}
CASE_F(TEST_PVR_REPLAY12,
	"调用replay close函数，返回OK。") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayClose(rh_ReplayHandle));

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Term());

	Auto_Play_DTV();
}
#endif
#if 1 //59~73
CASE_F(TEST_PVR_REPLAY_FFW1,
	"重复步骤TEST_PVR_REPLAY1-TEST_PVR_REPLAY4") {
	
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
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
		
	} 
}

CASE_F(TEST_PVR_REPLAY_FFW2,
	"调用speed replay进行2倍速快进，视频立刻按2倍速快进，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FFW,CHDRV_PVR_REPLAY_SPEED_2));
	LOG("2倍速快进\n");
}

CASE_F(TEST_PVR_REPLAY_FFW3,
	"调用get replay status，获得的状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_FFW4,
	"调用speed replay进行4倍速快进，视频立刻按4倍速快进，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FFW,CHDRV_PVR_REPLAY_SPEED_4));
	LOG("4倍速快进\n");
}

CASE_F(TEST_PVR_REPLAY_FFW5,
	"调用get replay status，获得的状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_FFW6,
	"调用speed replay进行8倍速快进，视频立刻按8倍速快进，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FFW,CHDRV_PVR_REPLAY_SPEED_8));
	LOG("8倍速快进\n");
}

CASE_F(TEST_PVR_REPLAY_FFW7,
	"调用get replay status，获得的状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_FFW8,
	"调用speed replay进行16倍速快进，视频立刻按16倍速快进，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FFW,CHDRV_PVR_REPLAY_SPEED_16));
	LOG("16倍速快进\n");
}

CASE_F(TEST_PVR_REPLAY_FFW9,
	"调用get replay status，获得的状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_FFW10,
	"调用speed replay进行32倍速快进，视频立刻按32倍速快进，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FFW,CHDRV_PVR_REPLAY_SPEED_32));
	LOG("32倍速快进\n");
}

CASE_F(TEST_PVR_REPLAY_FFW11,
	"调用get replay status，获得的状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_FFW12,
	"调用speed replay进行1倍速快进，音视频应立刻正常播放") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FFW,CHDRV_PVR_REPLAY_SPEED_1));
	LOG("1倍速快进,音视频应立刻正常播放\n");
}

CASE_F(TEST_PVR_REPLAY_FFW13,
	"调用get replay status，获得的状态应为PLAYING,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_pb.enm_PlayStatus);
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_FFW14,
	"调用replay stop函数，回放立刻停止。") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayStop(rh_ReplayHandle));
	
}


CASE_F(TEST_PVR_REPLAY_FFW15,
	"调用replay close函数，返回OK。") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayClose(rh_ReplayHandle));
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Term());
}
#endif

//75

#if 1
CASE_F(TEST_PVR_REPLAY_FBW1,
	"重复步骤TEST_PVR_REPLAY1-TEST_PVR_REPLAY4") {
	
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
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_FBW2,
	"调用speed replay进行2倍速快退，视频立刻按2倍速快退，声音为静音") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplaySeek(rh_ReplayHandle,PVR_RECTIME_LEN));
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FBW,CHDRV_PVR_REPLAY_SPEED_2));
	LOG("2倍速快退\n");
}

CASE_F(TEST_PVR_REPLAY_FBW3,
	"调用get replay status，获得的状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_FBW4,
	"调用speed replay进行4倍速快退，视频立刻按4倍速快退，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FBW,CHDRV_PVR_REPLAY_SPEED_4));
	LOG("4倍速快退\n");
}

CASE_F(TEST_PVR_REPLAY_FBW5,
	"调用get replay status，获得的状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_FBW6,
	"调用speed replay进行8倍速快退，视频立刻按8倍速快退，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FBW,CHDRV_PVR_REPLAY_SPEED_8));
	LOG("8倍速快退\n");
}

CASE_F(TEST_PVR_REPLAY_FBW7,
	"调用get replay status，获得的状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_FBW8,
	"调用speed replay进行16倍速快退，视频立刻按16倍速快退，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FBW,CHDRV_PVR_REPLAY_SPEED_16));
	LOG("16倍速快退\n");
}

CASE_F(TEST_PVR_REPLAY_FBW9,
	"调用get replay status，获得的状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_FBW10,
	"调用speed replay进行32倍速快退，视频立刻按32倍速快退，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FBW,CHDRV_PVR_REPLAY_SPEED_32));
	LOG("32倍速快退\n");
}

CASE_F(TEST_PVR_REPLAY_FBW11,
	"调用get replay status，获得的状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_FBW12,
	"调用speed replay进行1倍速快退，音视频应立刻正常播放") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FBW,CHDRV_PVR_REPLAY_SPEED_1));
	LOG("1倍速快退，音视频应立刻正常播放\n");
}

CASE_F(TEST_PVR_REPLAY_FBW13,
	"调用get replay status，获得的状态应为PLAYING,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_FBW14,
	"调用replay stop函数，回放立刻停止。") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayStop(rh_ReplayHandle));
	
}
CASE_F(TEST_PVR_REPLAY_FBW15,
	"调用replay close函数，返回OK。") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayClose(rh_ReplayHandle));

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Term());
}
#endif
#if 1 //77
CASE_F(TEST_PVR_REPLAY_SFW1,
	"重复步骤TEST_PVR_REPLAY1-TEST_PVR_REPLAY4") {
	
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
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_SFW2,
	"调用speed replay进行1/2倍慢放，视频立刻按1/2倍慢放，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_SFW,CHDRV_PVR_REPLAY_SPEED_2));
	LOG("1/2倍速慢放\n");
}

CASE_F(TEST_PVR_REPLAY_SFW3,
	"调用get replay status，获得的状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_SFW4,
	"调用speed replay进行1/4倍慢放，视频立刻按1/4倍慢放，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_SFW,CHDRV_PVR_REPLAY_SPEED_4));
	LOG("1/4倍速慢放\n");
}

CASE_F(TEST_PVR_REPLAY_SFW5,
	"调用get replay status，获得的状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_SFW6,
	"调用speed replay进行1/8倍慢放，视频立刻按1/8倍慢放，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_SFW,CHDRV_PVR_REPLAY_SPEED_8));
	LOG("1/8倍速慢放\n");
}

CASE_F(TEST_PVR_REPLAY_SFW7,
	"调用get replay status，获得的状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_SFW8,
	"调用speed replay进行1/16倍慢放，视频立刻按1/16倍慢放，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_SFW,CHDRV_PVR_REPLAY_SPEED_16));
	LOG("1/16倍速慢放\n");
}

CASE_F(TEST_PVR_REPLAY_SFW9,
	"调用get replay status，获得的状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_SFW10,
	"调用speed replay进行1/32倍慢放，视频立刻按1/32倍慢放，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_SFW,CHDRV_PVR_REPLAY_SPEED_32));
	LOG("1/32倍速慢放\n");
}

CASE_F(TEST_PVR_REPLAY_SFW11,
	"调用get replay status，获得的状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_SFW12,
	"调用speed replay进行1倍慢放，音视频应立刻正常播放") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_SFW,CHDRV_PVR_REPLAY_SPEED_1));
	LOG("1倍速慢放，音视频应立刻正常播放\n");
}

CASE_F(TEST_PVR_REPLAY_SFW13,
	"调用get replay status，获得的状态应为PLAYING,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_pb.enm_PlayStatus);
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_SFW14,
	"调用replay stop函数，回放立刻停止。") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayStop(rh_ReplayHandle));
	
}
CASE_F(TEST_PVR_REPLAY_SFW15,
	"调用replay close函数，返回OK。") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayClose(rh_ReplayHandle));
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Term());
}
#endif


//81~85
#if 1

CASE_F(TEST_PVR_REPLAY_SEEK1,
	"重复步骤TEST_PVR_REPLAY1-TEST_PVR_REPLAY4") {
	
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
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_SEEK2,
	"调用replay seek跳转到指定的位置，音视频应立刻跳转到该位置正常播放") {

	S32 ri_DstTimeS = 100;
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplaySeek(rh_ReplayHandle,ri_DstTimeS));
	LOG("跳转到100s\n");
}

CASE_F(TEST_PVR_REPLAY_SEEK3,
	"调用get replay status，获得的状态应为PLAYING,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_pb.enm_PlayStatus); 
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REPLAY_SEEK4,
	"调用replay stop函数，回放立刻停止。") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayStop(rh_ReplayHandle));
}

CASE_F(TEST_PVR_REPLAY_SEEK5,
	"调用replay close函数，返回OK.") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayClose(rh_ReplayHandle));
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Term());
	EXPECT_EQ(CHDRV_PVR_OK, Rec_Res_Term());
	EXPECT_EQ(CHDRV_PVR_OK, Replay_Res_Term());	
}
#endif

//86-98
#if 1
CASE_F(TEST_PVR_REC_REPLAY1,
	"重复步骤TEST_PVR_REC1-TEST_PVR_REC4") {

	S32 i_Loop = 0;

	/*Auto_Play_DTV();
	CH_FSYS_Init();
	Set_RECStartParam(PVR_NORMAL_REC_TYPE);//普通录制
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
		LOG("当前录制时长: %d\n", rpstru_Status_rec.i_RECTimeS);
		LOG("当前录制大小: %d\n", rpstru_Status_rec.i_RECFileLengthKB);
		CH_RTOS_TaskDelay(2000);
	}
}

CASE_F(TEST_PVR_REC_REPLAY2,
	"重复步骤TEST_PVR_REPLAY1-TEST_PVR_REPLAY4") {
	
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
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
		
	} 
}
CASE_F(TEST_PVR_REC_REPLAY3,
	"调用rec control暂停录制，录制暂停过程中音视频任然正常播放") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status_rec));
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECControl(rh_RECHandle,CHDRV_PVR_REC_PAUSE));
	LOG("暂停录制10s\n");
	CH_RTOS_TaskDelay(10000);
	
}

CASE_F(TEST_PVR_REC_REPLAY4,
	"调用get rec status，获得的状态应为PAUSE,录制的时长和文件大小保持暂停之前不变") {

	CHDRV_PVR_RECStatus_t rpstru_Status;
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status));
	EXPECT_EQ(CHDRV_PVR_REC_STATUS_PAUSE, rpstru_Status.enm_RECStatus); 
	EXPECT_EQ(rpstru_Status.i_RECFileLengthKB, rpstru_Status_rec.i_RECFileLengthKB);
	EXPECT_EQ(rpstru_Status.i_RECTimeS, rpstru_Status_rec.i_RECTimeS);

}

CASE_F(TEST_PVR_REC_REPLAY5,
	"调用replay control暂停回放，音视频应立刻停止播放") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayControl(rh_ReplayHandle,CHDRV_PVR_REPLAY_PAUSE));
	LOG("暂停播放10s\n");
	CH_RTOS_TaskDelay(10000);
	
}

CASE_F(TEST_PVR_REC_REPLAY6,
	"调用get replay status，获得的状态应为PAUSE,回放的时长和文件大小保持暂停之前不变") {

	CHDRV_PVR_ReplayStatus_t rpstru_Status;
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_RECHandle,&rpstru_Status));
	EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PAUSE, rpstru_Status.enm_PlayStatus); 
	EXPECT_EQ(rpstru_Status.i_ReadOffsetKB, rpstru_Status_pb.i_ReadOffsetKB);
	EXPECT_EQ(rpstru_Status.i_PlayTimeS, rpstru_Status_pb.i_PlayTimeS);
}

CASE_F(TEST_PVR_REC_REPLAY7,
	"调用rec control恢复录制，录制过程中音视频任然停止播放") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECControl(rh_RECHandle,CHDRV_PVR_REC_RESUME));
	
}
CASE_F(TEST_PVR_REC_REPLAY8,
	"调用get rec status，获得的状态应为RECORDING,并能时时更新录制的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status_rec));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_rec.enm_RECStatus);
		LOG("当前录制时长: %d\n", rpstru_Status_rec.i_RECTimeS);
		LOG("当前录制大小: %d\n", rpstru_Status_rec.i_RECFileLengthKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY9,
	"调用replay control恢复回放，音视频应立刻正常播放") {

	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayControl(rh_ReplayHandle,CHDRV_PVR_REPLAY_PLAY));
	
}
CASE_F(TEST_PVR_REC_REPLAY10,
	"调用get replay status，获得的状态应为PLAYING,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_pb.enm_PlayStatus);
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY11,
	"调用replay stop函数，回放立刻停止。") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayStop(rh_ReplayHandle));
}

CASE_F(TEST_PVR_REC_REPLAY12,
	"调用rec stop函数，录制立刻停止。") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECStop(rh_RECHandle));
}

CASE_F(TEST_PVR_REC_REPLAY13,
	"调用replay close，rec close函数，均返回OK") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayClose(rh_ReplayHandle));
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECClose(rh_RECHandle));//被CHDRV_PVR_RECStop提前调用了

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Term());
	EXPECT_EQ(CHDRV_PVR_OK, Rec_Res_Term());
	EXPECT_EQ(CHDRV_PVR_OK, Replay_Res_Term());	
}
#endif
//99-102
#if 1
CASE_F(TEST_PVR_REC_REPLAY_FFW1,
	"重复步骤TEST_PVR_REC1-TEST_PVR_REC4") {

	S32 i_Loop = 0;

	//Auto_Play_DTV();
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Init());
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECOpen(NULL,RecordCB,&rh_RECHandle)); 
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status_rec));
	EXPECT_EQ(CHDRV_PVR_REC_STATUS_IDLE, rpstru_Status_rec.enm_RECStatus); 
    EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECStart(rh_RECHandle,&RecParams));

	LOG("录制一段约15分钟的视频,请耐心等候\n"); 
		 
	for(i_Loop = 0;i_Loop < PVR_RECTIME_LEN;i_Loop++)
	{
		CH_RTOS_TaskDelay(1000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status_rec));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_rec.enm_RECStatus);
		if (i_Loop < 10)//如需全部显示请注释调
		{
			LOG("当前录制时长: %d\n", rpstru_Status_rec.i_RECTimeS);
			LOG("当前录制大小: %d\n", rpstru_Status_rec.i_RECFileLengthKB);
		}
	}
}

CASE_F(TEST_PVR_REC_REPLAY_FFW2_1,
	"重复步骤TEST_PVR_REPLAY1-TEST_PVR_REPLAY4") {
	
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
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_FFW2_2,
	"调用speed replay进行2倍速快进，视频立刻按2倍速快进，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FFW,CHDRV_PVR_REPLAY_SPEED_2));
	LOG("2倍速快进\n");
}

CASE_F(TEST_PVR_REC_REPLAY_FFW2_3,
	"调用get replay status，获得的状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_FFW2_4,
	"调用speed replay进行4倍速快进，视频立刻按4倍速快进，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FFW,CHDRV_PVR_REPLAY_SPEED_4));
	LOG("4倍速快进\n");
}

CASE_F(TEST_PVR_REC_REPLAY_FFW2_5,
	"调用get replay status，获得的状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_FFW2_6,
	"调用speed replay进行8倍速快进，视频立刻按8倍速快进，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FFW,CHDRV_PVR_REPLAY_SPEED_8));
	LOG("8倍速快进\n");
}

CASE_F(TEST_PVR_REC_REPLAY_FFW2_7,
	"调用get replay status，获得的状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_FFW2_8,
	"调用speed replay进行16倍速快进，视频立刻按16倍速快进，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FFW,CHDRV_PVR_REPLAY_SPEED_16));
	LOG("16倍速快进\n");
}

CASE_F(TEST_PVR_REC_REPLAY_FFW2_9,
	"调用get replay status，获得的状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_FFW2_10,
	"调用speed replay进行32倍速快进，视频立刻按32倍速快进，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FFW,CHDRV_PVR_REPLAY_SPEED_32));
	LOG("32倍速快进\n");
}

CASE_F(TEST_PVR_REC_REPLAY_FFW2_11,
	"调用get replay status，获得的状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_FFW2_12,
	"调用speed replay进行1倍速快进，音视频应立刻正常播放") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FFW,CHDRV_PVR_REPLAY_SPEED_1));
	LOG("1倍速快进\n");
}

CASE_F(TEST_PVR_REC_REPLAY_FFW2_13,
	"调用get replay status，获得的状态应为PLAYING,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_pb.enm_PlayStatus);
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_FFW2_14,
	"调用replay stop函数，回放立刻停止。") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayStop(rh_ReplayHandle));
	
}


CASE_F(TEST_PVR_REC_REPLAY_FFW2_15,
	"调用replay close函数，返回OK。") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayClose(rh_ReplayHandle));//failed to MI_PVR_DeInit()!
	
}



CASE_F(TEST_PVR_REC_REPLAY_FFW3,
	"调用rec stop函数，录制立刻停止。") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECStop(rh_RECHandle));
}

CASE_F(TEST_PVR_REC_REPLAY_FFW4,
	"调用rec close函数，返回OK.") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECClose(rh_RECHandle));
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Term());
	Rec_Res_Term();
	Replay_Res_Term();
	
}
#endif
//104
#if 1
CASE_F(TEST_PVR_REC_REPLAY_FBW1,
	"重复步骤TEST_PVR_REC1-TEST_PVR_REC4") {

	S32 i_Loop = 0;

	//Auto_Play_DTV();
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Init());
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECOpen(NULL,RecordCB,&rh_RECHandle)); 
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status_rec));
	EXPECT_EQ(CHDRV_PVR_REC_STATUS_IDLE, rpstru_Status_rec.enm_RECStatus); 
    EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECStart(rh_RECHandle,&RecParams));

	LOG("录制一段约15分钟的视频,请耐心等候\n"); 
		 
	for(i_Loop = 0;i_Loop < PVR_RECTIME_LEN;i_Loop++)
	{
		CH_RTOS_TaskDelay(1000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status_rec));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_rec.enm_RECStatus);
		if (i_Loop < 10)
		{
			LOG("当前录制时长: %d\n", rpstru_Status_rec.i_RECTimeS);
			LOG("当前录制大小: %d\n", rpstru_Status_rec.i_RECFileLengthKB);
		}
	}
}

CASE_F(TEST_PVR_REC_REPLAY_FBW2_1,
	"重复步骤TEST_PVR_REPLAY1-TEST_PVR_REPLAY4") {
	
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
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_FBW2_2,
	"调用speed replay进行2倍速快退，视频立刻按2倍速快退，声音为静音") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplaySeek(rh_ReplayHandle,PVR_RECTIME_LEN));
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FBW,CHDRV_PVR_REPLAY_SPEED_2));
	LOG("2倍速快退\n");
}

CASE_F(TEST_PVR_REC_REPLAY_FBW2_3,
	"调用get replay status，获得的状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_FBW2_4,
	"调用speed replay进行4倍速快退，视频立刻按4倍速快退，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FBW,CHDRV_PVR_REPLAY_SPEED_4));
	LOG("4倍速快退\n");
}

CASE_F(TEST_PVR_REC_REPLAY_FBW2_5,
	"调用get replay status，获得的状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_FBW2_6,
	"调用speed replay进行8倍速快退，视频立刻按8倍速快退，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FBW,CHDRV_PVR_REPLAY_SPEED_8));
	LOG("8倍速快退\n");
}

CASE_F(TEST_PVR_REC_REPLAY_FBW2_7,
	"调用get replay status，获得的状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_FBW2_8,
	"调用speed replay进行16倍速快退，视频立刻按16倍速快退，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FBW,CHDRV_PVR_REPLAY_SPEED_16));
	LOG("16倍速快退\n");
}

CASE_F(TEST_PVR_REC_REPLAY_FBW2_9,
	"调用get replay status，获得的状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_FBW2_10,
	"调用speed replay进行32倍速快退，视频立刻按32倍速快退，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FBW,CHDRV_PVR_REPLAY_SPEED_32));
	LOG("32倍速快退\n");
}

CASE_F(TEST_PVR_REC_REPLAY_FBW2_11,
	"调用get replay status，获得的状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_FBW2_12,
	"调用speed replay进行1倍速快退，音视频应立刻正常播放") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_FBW,CHDRV_PVR_REPLAY_SPEED_1));
	LOG("1倍速快退\n");
}

CASE_F(TEST_PVR_REC_REPLAY_FBW2_13,
	"调用get replay status，获得的状态应为PLAYING,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < 5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_FBW2_14,
	"调用replay stop函数，回放立刻停止。") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayStop(rh_ReplayHandle));
	
}
CASE_F(TEST_PVR_REC_REPLAY_FBW2_15,
	"调用replay close函数，返回OK。") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayClose(rh_ReplayHandle));//failed to MI_PVR_DeInit()! please stop and close record!
}


CASE_F(TEST_PVR_REC_REPLAY_FBW3,
	"调用rec stop函数，录制立刻停止。") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECStop(rh_RECHandle));
}

CASE_F(TEST_PVR_REC_REPLAY_FBW4,
	"调用rec close函数，返回OK.") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECClose(rh_RECHandle));
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Term());

	Rec_Res_Term();
	Replay_Res_Term();
	
}
#endif
//106
#if 1
CASE_F(TEST_PVR_REC_REPLAY_SFW1,
	"重复步骤TEST_PVR_REC1-TEST_PVR_REC4") {

	S32 i_Loop = 0;

	//Auto_Play_DTV();
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Init());
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECOpen(NULL,RecordCB,&rh_RECHandle)); 
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status_rec));
	EXPECT_EQ(CHDRV_PVR_REC_STATUS_IDLE, rpstru_Status_rec.enm_RECStatus); 
    EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECStart(rh_RECHandle,&RecParams));

	LOG("录制5分钟\n");
		 
	for(i_Loop = 0;i_Loop < 60*LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(1000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status_rec));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_rec.enm_RECStatus);
		if (i_Loop < 10)
		{
			LOG("当前录制时长: %d\n", rpstru_Status_rec.i_RECTimeS);
			LOG("当前录制大小: %d\n", rpstru_Status_rec.i_RECFileLengthKB);
		}
	}
}

CASE_F(TEST_PVR_REC_REPLAY_SFW2_1,
	"重复步骤TEST_PVR_REPLAY1-TEST_PVR_REPLAY4") {
	
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
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_SFW2_2,
	"调用speed replay进行1/2倍慢放，视频立刻按1/2倍慢放，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_SFW,CHDRV_PVR_REPLAY_SPEED_2));
	LOG("1/2倍速慢放\n");
}

CASE_F(TEST_PVR_REC_REPLAY_SFW2_3,
	"调用get replay status，获得的状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_SFW2_4,
	"调用speed replay进行1/4倍慢放，视频立刻按1/4倍慢放，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_SFW,CHDRV_PVR_REPLAY_SPEED_4));
	LOG("1/4倍速慢放\n");
}

CASE_F(TEST_PVR_REC_REPLAY_SFW2_5,
	"调用get replay status，获得的状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_SFW2_6,
	"调用speed replay进行1/8倍慢放，视频立刻按1/8倍慢放，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_SFW,CHDRV_PVR_REPLAY_SPEED_8));
	LOG("1/8倍速慢放\n");
}

CASE_F(TEST_PVR_REC_REPLAY_SFW2_7,
	"调用get replay status，获得的状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_SFW2_8,
	"调用speed replay进行1/16倍慢放，视频立刻按1/16倍慢放，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_SFW,CHDRV_PVR_REPLAY_SPEED_16));
	LOG("1/16倍速慢放\n");
}

CASE_F(TEST_PVR_REC_REPLAY_SFW2_9,
	"调用get replay status，获得的状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_SFW2_10,
	"调用speed replay进行1/32倍慢放，视频立刻按1/32倍慢放，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_SFW,CHDRV_PVR_REPLAY_SPEED_32));
	LOG("1/32倍速慢放\n");
}

CASE_F(TEST_PVR_REC_REPLAY_SFW2_11,
	"调用get replay status，获得的状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_pb.enm_PlayStatus);
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_SFW2_12,
	"调用speed replay进行1倍慢放，音视频应立刻正常播放") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_SpeedReplay(rh_ReplayHandle,CHDRV_PVR_REPLAY_SFW,CHDRV_PVR_REPLAY_SPEED_1));
	LOG("1倍速慢放\n");
}

CASE_F(TEST_PVR_REC_REPLAY_SFW2_13,
	"调用get replay status，获得的状态应为PLAYING,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_pb.enm_PlayStatus);
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 
}

CASE_F(TEST_PVR_REC_REPLAY_SFW2_14,
	"调用replay stop函数，回放立刻停止。") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayStop(rh_ReplayHandle));
	
}
CASE_F(TEST_PVR_REC_REPLAY_SFW2_15,
	"调用replay close函数，返回OK。") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayClose(rh_ReplayHandle));//failed to MI_PVR_DeInit()!please stop and close record!
}


CASE_F(TEST_PVR_REC_REPLAY_SFW3,
	"调用rec stop函数，录制立刻停止。") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECStop(rh_RECHandle));
}

CASE_F(TEST_PVR_REC_REPLAY_SFW4,
	"调用rec close函数，返回OK.") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECClose(rh_RECHandle));
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Term());

	Rec_Res_Term();
	Replay_Res_Term();
}
#endif

//110-113
#if 1
CASE_F(TEST_PVR_REC_REPLAY_SEEK1,
	"重复步骤TEST_PVR_REC1-TEST_PVR_REC4") {

	S32 i_Loop = 0;

	//Auto_Play_DTV();

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Init());
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECOpen(NULL,RecordCB,&rh_RECHandle)); 
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status_rec));
	EXPECT_EQ(CHDRV_PVR_REC_STATUS_IDLE, rpstru_Status_rec.enm_RECStatus); 
    EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECStart(rh_RECHandle,&RecParams));

	LOG("录制1分钟\n");
		 
	for(i_Loop = 0;i_Loop < 60;i_Loop++)
	{
		CH_RTOS_TaskDelay(1000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status_rec));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_rec.enm_RECStatus);
		if (i_Loop < 10)
		{
			LOG("当前录制时长: %d\n", rpstru_Status_rec.i_RECTimeS);
			LOG("当前录制大小: %d\n", rpstru_Status_rec.i_RECFileLengthKB);
		}
		
	}
}

CASE_F(TEST_PVR_REC_REPLAY_SEEK2,
	"重复步骤TEST_PVR_REPLAY1-TEST_PVR_REPLAY4") {
	
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
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 

	S32 ri_DstTimeS = 30;
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplaySeek(rh_ReplayHandle,ri_DstTimeS));

	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetReplayStatus(rh_ReplayHandle,&rpstru_Status_pb));
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_pb.enm_PlayStatus); 
		LOG("当前回放时长: %d\n", rpstru_Status_pb.i_PlayTimeS);
		LOG("当前回放偏移: %d\n", rpstru_Status_pb.i_ReadOffsetKB);
		CH_RTOS_TaskDelay(2000);
	} 

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayStop(rh_ReplayHandle));

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayClose(rh_ReplayHandle)); //failed to MI_PVR_DeInit(),please stop and close record!
}


CASE_F(TEST_PVR_REC_REPLAY_SEEK3,
	"调用rec stop函数，录制立刻停止。") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECStop(rh_RECHandle));
}

CASE_F(TEST_PVR_REC_REPLAY_SEEK4,
	"调用rec close函数，返回OK.") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECClose(rh_RECHandle));//CHDRV_PVR_RECStop里面调用了CHDRV_PVR_RECClose
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Term());

	Rec_Res_Term();
	Replay_Res_Term();
}
#endif
//115
#if 1 ///未测试
CASE_F(TEST_PVR_REC_ENCRYPT1,
	"重复步骤（TEST_PVR_REC1-TEST_PVR_REC12），start rec时uc_RecordType设置为加密录制") {
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
	LOG("暂停录制5s\n");
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
#if 0 //暂时无条件完成
CASE_F(TEST_PVR_REC_ECM1,
	"重复步骤（TEST_PVR_REC1-TEST_PVR_REC12），start rec时stru_RecParam需传入ECM PID") {

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
	
	LOG("录制一段约10分钟的视频,请耐心等候\n"); 
	for(i_Loop = 0;i_Loop < 60;i_Loop++)
	{
		CH_RTOS_TaskDelay(1000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_GetRECStatus(rh_RECHandle,&rpstru_Status_rec));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_rec.enm_RECStatus);
		LOG("当前录制时长: %d\n", rpstru_Status_rec.i_RECTimeS);
		LOG("当前录制大小: %d\n", rpstru_Status_rec.i_RECFileLengthKB);	
		
	}
	//TEST_PVR_REC5
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_RECControl(rh_RECHandle,CHDRV_PVR_REC_PAUSE));
	LOG("暂停录制5s\n");
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
		LOG("当前录制时长: %d\n", rpstru_Status_rec.i_RECTimeS);
		LOG("当前录制大小: %d\n", rpstru_Status_rec.i_RECFileLengthKB);
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
	"重复步骤（TEST_PVR_REPLAY1-TEST_PVR_REPLAY3），现象正常。") {

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
	"通过section模块能正确获取都回放中的ECM数据。") {

}

CASE_F(TEST_PVR_REC_ECM4,
	"调用replay stop函数，回放立刻停止。") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayStop(rh_ReplayHandle));
}

CASE_F(TEST_PVR_REC_ECM5,
	"调用replay close函数，返回OK.") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplayClose(rh_ReplayHandle));

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Term());

	Replay_Res_Term();
}

#endif



///127-138
#if 1
CASE_F(TEST_PVR_TIMESHIFT1,
	"调用timeshift open创建一个句柄，看是否返回成功，且句柄不为NULL") {

	//Auto_Play_DTV();
   CHDRV_PVR_Init();
	//Set_RECStartParam(PVR_NORMAL_REC_TYPE);

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftOpen(NULL,NULL,(CHDRV_PVR_TIMESHIFT_CALLBACK)TimeShiftCB,&rh_TimeshiftHandle)); 

}

CASE_F(TEST_PVR_TIMESHIFT2,
	"调用get timeshift status状态应为IDL") {

		 
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
	EXPECT_EQ(CHDRV_PVR_REC_STATUS_IDLE, rpstru_Status_timeshift.enm_RECStatus); 
}

CASE_F(TEST_PVR_TIMESHIFT3,
	"以正确的参数start timeshift刚刚创建的handle，文件播放路径从env获取，时移开始处于静帧状态") {
	CH_RTOS_TaskDelay(5000);//正常播放节目状态
	Auto_Stop_DTV(); ///启动timeshift播放之前必须停DTV，porting未实现停DTV，暂时用此方法代替
    EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftStart(rh_TimeshiftHandle,&RecParams));
	//EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftReplay2Pause(rh_TimeshiftHandle));
	LOG("时移开始处于静帧状态,Y or N ?\n");
	
}

CASE_F(TEST_PVR_TIMESHIFT4,
	"调用get timeshift status，获得的rec状态应为RECORDING,并能时时更新录制的时长和文件大小，获得的replay状态应为PAUSE,回放的时长和文件大小为0") {

	S32 i_Loop = 0;
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PAUSE, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("当前录制时长: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("当前录制大小: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);	
		LOG("当前回放时长: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("当前回放大小: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);	
	}
	
}

CASE_F(TEST_PVR_TIMESHIFT5,
	"调用timeshift control进行播放，音视频应立刻正常播放") {

		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftReplayControl(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_PLAY));
		LOG("音视频应立刻正常播放\n");
	
}



CASE_F(TEST_PVR_TIMESHIFT6,
	"调用get timeshift status，获得的rec状态应为RECORDING,并能时时更新录制的时长和文件大小，获得的replay状态应为PLAYING,并能时时更新回放的时长和文件大小") {

		S32 i_Loop = 0;
		for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
		{
			CH_RTOS_TaskDelay(2000);
			EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
			EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
			EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_timeshift.enm_PlayStatus);
			LOG("当前录制时长: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
			LOG("当前录制大小: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
			LOG("当前回放时长: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
			LOG("当前回放大小: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);
		}
	
}

CASE_F(TEST_PVR_TIMESHIFT7,
	"重复调用replay control进行播放，应返回ok") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftReplayControl(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_PLAY));
	
}

CASE_F(TEST_PVR_TIMESHIFT8,
	"调用replay control暂停播放，音视频应立刻停止播放") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftReplayControl(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_PAUSE));
}

CASE_F(TEST_PVR_TIMESHIFT9,
	"调用get timeshift status，获得的rec状态应为RECORDING,并能时时更新录制的时长和文件大小，获得的replay状态应为PAUSE,回放的时长和文件保持暂停之前不变") {


	CHDRV_PVR_TimeShiftStatus_t rpstru_Status;
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status));
	
	S32 i_Loop = 0;
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(1000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PAUSE, rpstru_Status.enm_PlayStatus);
		LOG("当前录制时长: %d\n", rpstru_Status.i_TotalRECTimeS);
		LOG("当前录制大小: %d\n", rpstru_Status.i_RECFileLengthKB);
		EXPECT_EQ(rpstru_Status.i_PlayedTimeS, rpstru_Status_timeshift.i_PlayedTimeS);
		EXPECT_EQ(rpstru_Status.i_PlayedFileOffsetKB, rpstru_Status_timeshift.i_PlayedFileOffsetKB);
		LOG("当前回放时长: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("当前回放大小: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);

	}
}

CASE_F(TEST_PVR_TIMESHIFT10,
	"重复调用replay control暂停播放，应返回ok") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftReplayControl(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_PAUSE));
	
}


CASE_F(TEST_PVR_TIMESHIFT11,
	"调用timeshift stop函数，时移立刻停止。") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftStop(rh_TimeshiftHandle));
	
}

CASE_F(TEST_PVR_TIMESHIFT12,
	"调用timeshift close函数，返回OK。") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftClose(rh_TimeshiftHandle));//called in CHDRV_PVR_TimeShiftStop
	CHDRV_PVR_Term();
	Timeshift_Res_Term();
}

#endif

//140 - 154
#if 1

CASE_F(TEST_PVR_TIMESHIFT_FFW1,
	"重复步骤TEST_PVR_TIMESHIFT1-TEST_PVR_TIMESHIFT6") {

	//Auto_Play_DTV();
	
//TEST_PVR_TIMESHIFT1
    CHDRV_PVR_Init();
	//Set_RECStartParam(PVR_NORMAL_REC_TYPE);//继续完善

//TEST_PVR_TIMESHIFT2

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftOpen(NULL,NULL,(CHDRV_PVR_TIMESHIFT_CALLBACK)TimeShiftCB,&rh_TimeshiftHandle));

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
	EXPECT_EQ(CHDRV_PVR_REC_STATUS_IDLE, rpstru_Status_timeshift.enm_RECStatus); 

//TEST_PVR_TIMESHIFT3	
	CH_RTOS_TaskDelay(5000);
	Auto_Stop_DTV();
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftStart(rh_TimeshiftHandle,&RecParams));
	
	//EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftReplay2Pause(rh_TimeshiftHandle));
	LOG("时移开始处于静帧状态,Y or N ?\n");

//TEST_PVR_TIMESHIFT4

	S32 i_Loop = 0;
	LOG("正在时移约15分钟的视频，请耐心等候\n");
	for(i_Loop = 0;i_Loop < PVR_RECTIME_LEN;i_Loop++)
	{
		CH_RTOS_TaskDelay(1000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PAUSE, rpstru_Status_timeshift.enm_PlayStatus);
		if(i_Loop < 10)
		{
			LOG("当前录制时长: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
			LOG("当前录制大小: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		}
	}
	
//TEST_PVR_TIMESHIFT5

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftReplayControl(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_PLAY));
	LOG("音视频应立刻正常播放\n");

//TEST_PVR_TIMESHIFT6

	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("当前录制时长: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("当前录制大小: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("当前回放时长: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("当前回放大小: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);	
	}

}

CASE_F(TEST_PVR_TIMESHIFT_FFW2,
	"调用speed timeshift进行2倍速快进，视频立刻按2倍速快进，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftSpeedReplay(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_FFW,CHDRV_PVR_REPLAY_SPEED_2));
	LOG("2倍速快进\n");
}

CASE_F(TEST_PVR_TIMESHIFT_FFW3,
	"调用get timeshift status，获得的rec状态应为RECORDING,并能时时更新录制的时长和文件大小，获得的replay状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("当前录制时长: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("当前录制大小: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("当前回放时长: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("当前回放大小: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);	
	}
}

CASE_F(TEST_PVR_TIMESHIFT_FFW4,
	"调用speed timeshift进行4倍速快进，视频立刻按4倍速快进，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftSpeedReplay(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_FFW,CHDRV_PVR_REPLAY_SPEED_4));
	LOG("4倍速快进\n");
}

CASE_F(TEST_PVR_TIMESHIFT_FFW5,
	"调用get timeshift status，获得的rec状态应为RECORDING,并能时时更新录制的时长和文件大小，获得的replay状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("当前录制时长: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("当前录制大小: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("当前回放时长: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("当前回放大小: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);
	}

}

CASE_F(TEST_PVR_TIMESHIFT_FFW6,
	"调用speed timeshift进行8倍速快进，视频立刻按8倍速快进，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftSpeedReplay(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_FFW,CHDRV_PVR_REPLAY_SPEED_8));
	LOG("8倍速快进\n");
}

CASE_F(TEST_PVR_TIMESHIFT_FFW7,
	"调用get timeshift status，获得的rec状态应为RECORDING,并能时时更新录制的时长和文件大小，获得的replay状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("当前录制时长: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("当前录制大小: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("当前回放时长: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("当前回放大小: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);
	}

}

CASE_F(TEST_PVR_TIMESHIFT_FFW8,
	"调用speed timeshift进行16倍速快进，视频立刻按16倍速快进，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftSpeedReplay(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_FFW,CHDRV_PVR_REPLAY_SPEED_16));
	LOG("16倍速快进\n");
}

CASE_F(TEST_PVR_TIMESHIFT_FFW9,
	"调用get timeshift status，获得的rec状态应为RECORDING,并能时时更新录制的时长和文件大小，获得的replay状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("当前录制时长: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("当前录制大小: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("当前回放时长: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("当前回放大小: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);
	}

}

CASE_F(TEST_PVR_TIMESHIFT_FFW10,
	"调用speed timeshift进行32倍速快进，视频立刻按32倍速快进，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftSpeedReplay(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_FFW,CHDRV_PVR_REPLAY_SPEED_32));
	LOG("32倍速快进\n");
}

CASE_F(TEST_PVR_TIMESHIFT_FFW11,
	"调用get timeshift status，获得的rec状态应为RECORDING,并能时时更新录制的时长和文件大小，获得的replay状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("当前录制时长: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("当前录制大小: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("当前回放时长: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("当前回放大小: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);
	}

}

CASE_F(TEST_PVR_TIMESHIFT_FFW12,
	"调用speed timeshift进行1倍速快进，音视频应立刻正常播放") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftSpeedReplay(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_FFW,CHDRV_PVR_REPLAY_SPEED_1));
	LOG("1倍速快进,音视频应立刻正常播放\n");
}

CASE_F(TEST_PVR_TIMESHIFT_FFW13,
	"调用get timeshift status，获得的rec状态应为RECORDING,并能时时更新录制的时长和文件大小，获得的replay状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("当前录制时长: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("当前录制大小: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("当前回放时长: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("当前回放大小: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);
	}
}

CASE_F(TEST_PVR_TIMESHIFT_FFW14,
	"调用timeshift stop函数，时移立刻停止。") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftStop(rh_TimeshiftHandle));
	
}


CASE_F(TEST_PVR_TIMESHIFT_FFW15,
	"调用timeshift close函数，返回OK。") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftClose(rh_TimeshiftHandle));//called in CHDRV_PVR_TimeShiftStop

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Term());
	
	Timeshift_Res_Term();
}

#endif

//156 
#if 1
CASE_F(TEST_PVR_TIMESHIFT_FBW1,
	"speed timeshift设置为快退模式，按照TEST_PVR_TIMESHIFT_FFW1的流程重新测试") {

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

	LOG("时移开始处于静帧状态,Y or N ?\n");

//TEST_PVR_TIMESHIFT4

	S32 i_Loop = 0;
	LOG("正在录制约15分钟的视频，请耐心等候\n");
	for(i_Loop = 0;i_Loop < PVR_RECTIME_LEN;i_Loop++)
	{
		CH_RTOS_TaskDelay(1000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PAUSE, rpstru_Status_timeshift.enm_PlayStatus);
		if(i_Loop < 10)
		{
			LOG("当前录制时长: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
			LOG("当前录制大小: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		}	
	}
	
//TEST_PVR_TIMESHIFT5

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftReplayControl(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_PLAY));
	LOG("音视频应立刻正常播放\n");

//TEST_PVR_TIMESHIFT6

	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("当前录制时长: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("当前录制大小: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("当前回放时长: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("当前回放大小: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);		
	}

}
CASE_F(TEST_PVR_TIMESHIFT_FBW2,
	"调用speed timeshift进行2倍速快退，视频立刻按2倍速快退，声音为静音") {

	LOG("跳转到文件尾部，再快退\n");
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplaySeek(rh_TimeshiftHandle,PVR_RECTIME_LEN));
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftSpeedReplay(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_FBW,CHDRV_PVR_REPLAY_SPEED_2));
	LOG("2倍速快退\n");
}

CASE_F(TEST_PVR_TIMESHIFT_FBW3,
	"调用get timeshift status，获得的rec状态应为RECORDING,并能时时更新录制的时长和文件大小，获得的replay状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("当前录制时长: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("当前录制大小: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("当前回放时长: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("当前回放大小: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);	
	}
}

CASE_F(TEST_PVR_TIMESHIFT_FBW4,
	"调用speed timeshift进行4倍速快退，视频立刻按4倍速快退，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftSpeedReplay(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_FBW,CHDRV_PVR_REPLAY_SPEED_4));
	LOG("4倍速快进\n");
}

CASE_F(TEST_PVR_TIMESHIFT_FBW5,
	"调用get timeshift status，获得的rec状态应为RECORDING,并能时时更新录制的时长和文件大小，获得的replay状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("当前录制时长: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("当前录制大小: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("当前回放时长: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("当前回放大小: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);		
	}

}

CASE_F(TEST_PVR_TIMESHIFT_FBW6,
	"调用speed timeshift进行8倍速快退，视频立刻按8倍速快退，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftSpeedReplay(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_FBW,CHDRV_PVR_REPLAY_SPEED_8));
	LOG("8倍速快进\n");
}

CASE_F(TEST_PVR_TIMESHIFT_FBW7,
	"调用get timeshift status，获得的rec状态应为RECORDING,并能时时更新录制的时长和文件大小，获得的replay状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("当前录制时长: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("当前录制大小: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("当前回放时长: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("当前回放大小: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);	
	}

}

CASE_F(TEST_PVR_TIMESHIFT_FBW8,
	"调用speed timeshift进行16倍速快退，视频立刻按16倍速快退，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftSpeedReplay(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_FBW,CHDRV_PVR_REPLAY_SPEED_16));
	LOG("16倍速快进\n");
}

CASE_F(TEST_PVR_TIMESHIFT_FBW9,
	"调用get timeshift status，获得的rec状态应为RECORDING,并能时时更新录制的时长和文件大小，获得的replay状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("当前录制时长: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("当前录制大小: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("当前回放时长: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("当前回放大小: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);		
	}

}

CASE_F(TEST_PVR_TIMESHIFT_FBW10,
	"调用speed timeshift进行32倍速快退，视频立刻按32倍速快退，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftSpeedReplay(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_FBW,CHDRV_PVR_REPLAY_SPEED_32));
	LOG("32倍速快进\n");
}

CASE_F(TEST_PVR_TIMESHIFT_FBW11,
	"调用get timeshift status，获得的rec状态应为RECORDING,并能时时更新录制的时长和文件大小，获得的replay状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("当前录制时长: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("当前录制大小: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("当前回放时长: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("当前回放大小: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);	
	}

}

CASE_F(TEST_PVR_TIMESHIFT_FBW12,
	"调用speed timeshift进行1倍速快退，音视频应立刻正常播放") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftSpeedReplay(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_FBW,CHDRV_PVR_REPLAY_SPEED_1));
	LOG("1倍速快进,音视频应立刻正常播放\n");
}

CASE_F(TEST_PVR_TIMESHIFT_FBW13,
	"调用get timeshift status，获得的rec状态应为RECORDING,并能时时更新录制的时长和文件大小，获得的replay状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("当前录制时长: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("当前录制大小: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("当前回放时长: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("当前回放大小: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);	
	}
}

CASE_F(TEST_PVR_TIMESHIFT_FBW14,
	"调用timeshift stop函数，时移立刻停止。") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftStop(rh_TimeshiftHandle));
	
}


CASE_F(TEST_PVR_TIMESHIFT_FBW16,
	"调用timeshift close函数，返回OK。") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftClose(rh_TimeshiftHandle));//called in CHDRV_PVR_TimeShiftStop

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Term());

	Timeshift_Res_Term();
}

#endif

//158
#if 1
CASE_F(TEST_PVR_TIMESHIFT_SFW1,
	"speed timeshift设置为慢放模式，按照TEST_PVR_TIMESHIFT_FFW1的流程重新测试") {

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
		LOG("当前录制时长: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("当前录制大小: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);		
	}
	
//TEST_PVR_TIMESHIFT5

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftReplayControl(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_PLAY));
	LOG("音视频应立刻正常播放\n");

//TEST_PVR_TIMESHIFT6

	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("当前录制时长: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("当前录制大小: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("当前回放时长: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("当前回放大小: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);	
	}

}
CASE_F(TEST_PVR_TIMESHIFT_SFW2,
	"调用speed timeshift进行1/2倍速慢放，视频立刻按1/2倍速慢放，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftSpeedReplay(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_SFW,CHDRV_PVR_REPLAY_SPEED_2));
	LOG("1/2倍速慢放\n");
}

CASE_F(TEST_PVR_TIMESHIFT_SFW3,
	"调用get timeshift status，获得的rec状态应为RECORDING,并能时时更新录制的时长和文件大小，获得的replay状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("当前录制时长: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("当前录制大小: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("当前回放时长: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("当前回放大小: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);
	}
}

CASE_F(TEST_PVR_TIMESHIFT_SFW4,
	"调用speed timeshift进行1/4倍速慢放，视频立刻按1/4倍速慢放，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftSpeedReplay(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_SFW,CHDRV_PVR_REPLAY_SPEED_4));
	LOG("1/4倍速慢放\n");
}

CASE_F(TEST_PVR_TIMESHIFT_SFW5,
	"调用get timeshift status，获得的rec状态应为RECORDING,并能时时更新录制的时长和文件大小，获得的replay状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("当前录制时长: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("当前录制大小: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("当前回放时长: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("当前回放大小: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);
	}

}

CASE_F(TEST_PVR_TIMESHIFT_SFW6,
	"调用speed timeshift进行1/8倍速慢放，视频立刻按1/8倍速慢放，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftSpeedReplay(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_SFW,CHDRV_PVR_REPLAY_SPEED_8));
	LOG("1/8倍速慢放\n");
}

CASE_F(TEST_PVR_TIMESHIFT_SFW7,
	"调用get timeshift status，获得的rec状态应为RECORDING,并能时时更新录制的时长和文件大小，获得的replay状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("当前录制时长: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("当前录制大小: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("当前回放时长: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("当前回放大小: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);	
	}

}

CASE_F(TEST_PVR_TIMESHIFT_SFW8,
	"调用speed timeshift进行1/16倍速慢放，视频立刻按1/16倍速慢放，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftSpeedReplay(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_SFW,CHDRV_PVR_REPLAY_SPEED_16));
	LOG("1/16倍速慢放\n");
}

CASE_F(TEST_PVR_TIMESHIFT_SFW9,
	"调用get timeshift status，获得的rec状态应为RECORDING,并能时时更新录制的时长和文件大小，获得的replay状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("当前录制时长: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("当前录制大小: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("当前回放时长: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("当前回放大小: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);	
	}

}

CASE_F(TEST_PVR_TIMESHIFT_SFW10,
	"调用speed timeshift进行1/32倍速慢放，视频立刻按1/32倍速慢放，声音为静音") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftSpeedReplay(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_SFW,CHDRV_PVR_REPLAY_SPEED_32));
	LOG("1/32倍速慢放\n");
}

CASE_F(TEST_PVR_TIMESHIFT_SFW11,
	"调用get timeshift status，获得的rec状态应为RECORDING,并能时时更新录制的时长和文件大小，获得的replay状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_SPEED, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("当前录制时长: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("当前录制大小: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("当前回放时长: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("当前回放大小: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);	
	}

}

CASE_F(TEST_PVR_TIMESHIFT_SFW12,
	"调用speed timeshift进行1倍速慢放，音视频应立刻正常播放") {
	
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftSpeedReplay(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_SFW,CHDRV_PVR_REPLAY_SPEED_1));
	LOG("1倍速快进,音视频应立刻正常播放\n");
}

CASE_F(TEST_PVR_TIMESHIFT_SFW13,
	"调用get timeshift status，获得的rec状态应为RECORDING,并能时时更新录制的时长和文件大小，获得的replay状态应为SPEED,并能时时更新回放的时长和文件大小") {

	S32 i_Loop = 0;
	
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("当前录制时长: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("当前录制大小: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("当前回放时长: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("当前回放大小: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);	
	}
}

CASE_F(TEST_PVR_TIMESHIFT_SFW14,
	"调用timeshift stop函数，时移立刻停止。") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftStop(rh_TimeshiftHandle));
	
}


CASE_F(TEST_PVR_TIMESHIFT_SFW15,
	"调用timeshift close函数，返回OK。") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftClose(rh_TimeshiftHandle));//called in CHDRV_PVR_TimeShiftStop

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_Term());

	Timeshift_Res_Term();
}

#endif

//162 - 167
#if 1
CASE_F(TEST_PVR_TIMESHIFT_SEEK1,
	"重复步骤TEST_PVR_TIMESHIFT1-TEST_PVR_TIMESHIFT6") {

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
		LOG("当前录制时长: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("当前录制大小: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);	
	}
	
//TEST_PVR_TIMESHIFT5

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftReplayControl(rh_TimeshiftHandle,CHDRV_PVR_REPLAY_PLAY));
	LOG("音视频应立刻正常播放\n");

//TEST_PVR_TIMESHIFT6

	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(2000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("当前录制时长: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("当前录制大小: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("当前回放时长: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("当前回放大小: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);
	}

}
CASE_F(TEST_PVR_TIMESHIFT_SEEK2,
	"调用replay seek跳转到指定的位置，音视频应立刻跳转到该位置正常播放") {

	S32 ri_DstTimeS = 10;
	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_ReplaySeek(rh_TimeshiftHandle,ri_DstTimeS));
}

CASE_F(TEST_PVR_TIMESHIFT_SEEK3,
	"调用get timeshift status，获得的rec状态应为RECORDING,并能时时更新录制的时长和文件大小，获得的replay状态应为PLAYING,并能时时更新回放的时长和文件大小") {


	S32 i_Loop = 0;
	for(i_Loop = 0;i_Loop < LOOP_5;i_Loop++)
	{
		CH_RTOS_TaskDelay(1000);
		EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftGetStatus(rh_TimeshiftHandle,&rpstru_Status_timeshift));
		EXPECT_EQ(CHDRV_PVR_REC_STATUS_RECORDING, rpstru_Status_timeshift.enm_RECStatus);
		EXPECT_EQ(CHDRV_PVR_REPLAY_STATUS_PLAYING, rpstru_Status_timeshift.enm_PlayStatus);
		LOG("当前录制时长: %d\n", rpstru_Status_timeshift.i_TotalRECTimeS);
		LOG("当前录制大小: %d\n", rpstru_Status_timeshift.i_RECFileLengthKB);
		LOG("当前回放时长: %d\n", rpstru_Status_timeshift.i_PlayedTimeS);
		LOG("当前回放大小: %d\n", rpstru_Status_timeshift.i_PlayedFileOffsetKB);	
	}
}


CASE_F(TEST_PVR_TIMESHIFT_SEEK4,
	"调用timeshift stop函数，时移立刻停止。") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftStop(rh_TimeshiftHandle));
	
}
CASE_F(TEST_PVR_TIMESHIFT_SEEK5,
	"调用timeshift close函数，返回OK。") {

	EXPECT_EQ(CHDRV_PVR_OK, CHDRV_PVR_TimeShiftClose(rh_TimeshiftHandle));//called in CHDRV_PVR_TimeShiftStop
	CHDRV_PVR_Term();
}

CASE_F(TEST_PVR_GET_TIMESHIFT_RES_INFO,
	"调用get res info，能正确获取到刚刚已录制的节目的详细信息") {

	EXPECT_EQ(CHDRV_PVR_NOT_SUPPORT, CHDRV_PVR_GetResInfo(NULL,NULL,NULL,NULL));
	//Auto_Stop_DTV();

	Timeshift_Res_Term();
}

#endif

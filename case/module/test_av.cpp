/**
@file test_av.cpp
@defgroup av���Լ�

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
#define CASE_NAME TestAV


#include "test_menu.h"
#include "test_modules.h"
#include "psi_parse.h"
#ifdef CHSSA_CHIP_7583
extern "C" {
#include "chdrv_platform.h"
}
#endif


void CASE_NAME::SetUpTestCase() {
//excute before each test suit
//std::cout << "init SetUp rtos TestCase " << std::endl;
//��Ƶ׼��
RUN_CMD("tuner.lock %s", fw_getenv("TUNER_PARAMS1"));

	CH_Rect_t rstru_Rect;
	
	rstru_Rect.s_X = 0;
	rstru_Rect.s_Y = 0;
	
#ifdef CFG_CHSSA_HD_SUPPORT	
	rstru_Rect.s_Width = 1280;
	rstru_Rect.s_Height = 720;
#else
	rstru_Rect.s_Width = 720;
	rstru_Rect.s_Height = 576;
#endif
	EXPECT_EQ(CHDRV_VWINDOW_OK,CHDRV_VWINDOW_SetRect(vwin_GetHandle(0),&rstru_Rect));

}
void CASE_NAME::TearDownTestCase() {
//excute after each test suit
//std::cout << "deinit video rtos TestCase " << std::endl;

	CH_Rect_t rstru_Rect;
	
	rstru_Rect.s_X = 0;
	rstru_Rect.s_Y = 0;
	
#ifdef CFG_CHSSA_HD_SUPPORT	
	rstru_Rect.s_Width = 1280;
	rstru_Rect.s_Height = 720;
#else
	rstru_Rect.s_Width = 720;
	rstru_Rect.s_Height = 576;
#endif
	EXPECT_EQ(CHDRV_VWINDOW_OK,CHDRV_VWINDOW_SetRect(vwin_GetHandle(0),&rstru_Rect));

	CHDRV_VIDEO_WindowAttrib_t stru_attr;

	CHDRV_VIDEO_GetWinAttrib(video_GetHandle(0),&stru_attr);
	stru_attr.enm_AspectMode = CHDRV_VIDEO_WINDOW_FULL;
	CHDRV_VIDEO_SetWinAttrib(video_GetHandle(0), &stru_attr);

	RUN_CMD("vout.set_format 6");
	RUN_CMD("av.stop 0\n");
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
#define MAX_CODETYPE MAX(CH_VIDEO_CODE_MAX, CH_AUDIO_CODE_TYPE_MAX)
#define D_GET_CODETYPE(stream, num) (stream.size() > num ? get_codec_type(stream[num], NULL) : MAX_CODETYPE)
#define D_AUDIO_HANDLE (audio_GetHandleNums() > 0 ? audio_GetHandle(0) : NULL)
#define D_VIDEO_HANDLE (video_GetHandleNums() > 0 ? video_GetHandle(0) : NULL) 


static CH_RTOS_SemaphoreHandle gh_data_come = NULL;
static CH_RTOS_SemaphoreHandle gh_semaHandle = NULL;
static int gi_vid_event_count[CHDRV_VIDEO_EVENT_MAX][3];
static int gi_aud_event_count[CHDRV_AUDIO_EVENT_MAX][3];
static CH_BOOL gb_beep_play = CH_FALSE;//��������beep����

/*�洢��ǰ���ڲ��ŵĽ�Ŀ����*/
static Channel_Info_t gstru_current_prog;
extern int vid_info(int argc, CMD_PARAM argv[]);


/**
FuncName:request_play_program
@brief ��ʾ����Ա����ָ���������������ؽ�Ŀ��
 
@param [in] rpstr_stream 
@param [in] rpstr_tuner 
@param [in] loop 

@param [out] none
 
@return success  
@return fail  
   
@attention none
*/
int request_play_program(const char *rpstr_stream, const char *rpstr_tuner, int loop)
{
	char *stream_name = NULL;
	int prognum = -1;
	char *ts_env = NULL;
	
	
	ts_env = fw_getenv((char*)rpstr_stream);
	if(ts_env == NULL)
	{
		LOG("δ����[%s]�������ú��Ժ����\n", rpstr_stream);
		return -1;
	}

	stream_name = new char[strlen(ts_env) + 1];
	strcpy(stream_name, ts_env);

	char *p_temp = strstr(stream_name, "&program=");
	if(!p_temp)
	{
		LOG("δ����[%s] in %s\n", "program", stream_name);
	}
	else
	{
		CMD_PARAM program_number;
		p_temp[0] = 0;
		p_temp += strlen("&program=");
		program_number = p_temp;
		prognum = program_number.get_i();
		LOG("prognum=%d,p_temp=%s\n", prognum, p_temp);
	}
	PROMPT("�벥������[%s]��Ƶ��[%s]��loop=%d,׼�����Ժ󰴻س�:",
		stream_name,
		fw_getenv((char*)rpstr_tuner),
		loop);
	test_input_none();

	delete []stream_name;
	return prognum;
}



/**
FuncName:video_call1
@brief ע���video�Ļص�
 
@param [in] renm_EventType 
@param [in] rh_Handle 
@param [in] rp_ExtendData 
@param [out] none
 
@return success  
@return fail  
   
@attention none
*/
static void video_call1(CHDRV_VIDEO_Handle rh_Handle ,
					CHDRV_VIDEO_EVENTTYPE_e renm_EventType,
					void * rp_ExtendData)
{
		if(renm_EventType < CHDRV_VIDEO_EVENT_MAX)
		{
			LOG("renm_EventType=%d\n", renm_EventType);
			gi_vid_event_count[renm_EventType][0] ++;
		}
		else
		{
			LOG("ERORR: renm_EventType=%d\n", renm_EventType);
		}

		EXPECT_EQ(rh_Handle, D_VIDEO_HANDLE);
}



static void video_call2(CHDRV_VIDEO_Handle rh_Handle ,
					CHDRV_VIDEO_EVENTTYPE_e renm_EventType,
					void * rp_ExtendData)
{
		if(renm_EventType < CHDRV_VIDEO_EVENT_MAX)
		{
			gi_vid_event_count[renm_EventType][1] ++;
		}
		else
		{
			LOG("ERORR: renm_EventType=%d\n", renm_EventType);
		}

		EXPECT_EQ(rh_Handle, D_VIDEO_HANDLE);
}


static void video_call3(CHDRV_VIDEO_Handle rh_Handle ,
					CHDRV_VIDEO_EVENTTYPE_e renm_EventType,
					void * rp_ExtendData)
{
		if(renm_EventType < CHDRV_VIDEO_EVENT_MAX)
		{
			gi_vid_event_count[renm_EventType][2] ++;
		}
		else
		{
			LOG("ERORR: renm_EventType=%d\n", renm_EventType);
		}

		EXPECT_EQ(rh_Handle, D_VIDEO_HANDLE);
}

/**
FuncName:audio_call1
@brief ע���audio�Ļص�
 
@param [in] renm_Event 
@param [in] rh_Handle 
@param [in] rui_Arg 
@param [out] none
 
@return success  
@return fail  
   
@attention none
*/
static void audio_call1(CHDRV_AUDIO_Handle rh_Handle ,
					CHDRV_AUDIO_CALLBACK_EVENT_e renm_Event,
					U32 *rui_Arg)
{
		if(renm_Event < CHDRV_AUDIO_EVENT_MAX)
		{
			gi_aud_event_count[renm_Event][0] ++;
		}
		else
		{
			LOG("ERORR: renm_EventType=%d\n", renm_Event);
		}

		EXPECT_EQ(rh_Handle, D_AUDIO_HANDLE);
}					

static void audio_call2(CHDRV_AUDIO_Handle rh_Handle ,
					CHDRV_AUDIO_CALLBACK_EVENT_e renm_Event,
					U32 *rui_Arg)
{
		if(renm_Event < CHDRV_AUDIO_EVENT_MAX)
		{
			gi_aud_event_count[renm_Event][1] ++;
		}
		else
		{
			LOG("ERORR: renm_EventType=%d\n", renm_Event);
		}

		EXPECT_EQ(rh_Handle, D_AUDIO_HANDLE);
}	

static void audio_call3(CHDRV_AUDIO_Handle rh_Handle ,
					CHDRV_AUDIO_CALLBACK_EVENT_e renm_Event,
					U32 *rui_Arg)
{
		if(renm_Event < CHDRV_AUDIO_EVENT_MAX)
		{
			gi_aud_event_count[renm_Event][2] ++;
		}
		else
		{
			LOG("ERORR: renm_EventType=%d\n", renm_Event);
		}

		EXPECT_EQ(rh_Handle, D_AUDIO_HANDLE);
}	


/**
FuncName:audio_codetype_name
@brief ��audio typeת��Ϊ�ַ���
 
@param [in] enm_streamType 
@param [out] none
 
@return success  
@return fail  
   
@attention none
*/
static const char* audio_codetype_name(int enm_streamType)
{
	static const char *audio_allTypes[CH_AUDIO_CODE_TYPE_MAX] = {
	"CH_AUDIO_CODE_MPEG1",
	"CH_AUDIO_CODE_MPEG2",
	"CH_AUDIO_CODE_MP3",
	"CH_AUDIO_CODE_AC3",
	"CH_AUDIO_CODE_AAC_ADTS",
	"CH_AUDIO_CODE_AAC_LOAS",
	"CH_AUDIO_CODE_HEAAC_ADTS",
	"CH_AUDIO_CODE_HEAAC_LOAS",
	"CH_AUDIO_CODE_WMA",
	"CH_AUDIO_CODE_AC3_PLUS",
	"CH_AUDIO_CODE_LPCM",
	"CH_AUDIO_CODE_DTS",
	"CH_AUDIO_CODE_ATRAC",
#if defined(CFG_CHSSA_CHIP_MSTAR7S33B)	
	"CH_AUDIO_CODE_EAC3",		
	"CH_AUDIO_CODE_EAC3_PLUS",	
	"CH_AUDIO_CODE_EAC3_AD",
	"CH_AUDIO_CODE_AC3_PLUSE"
#else
	"CH_AUDIO_CODE_COOK",
	"CH_AUDIO_CODE_DRA",
#endif	
	};
	
	if(enm_streamType < CH_AUDIO_CODE_TYPE_MAX && enm_streamType >= 0)
	{
		return audio_allTypes[enm_streamType];
	}
	return "UNKOWN_TYPE";
	
}

/**
FuncName:video_codetype_name
@brief ��video typeת��Ϊ�ַ���
 
@param [in] enm_streamType 
@param [out] none
 
@return success  
@return fail  
   
@attention none
*/
static const char* video_codetype_name(int enm_streamType)
{
	static const char *video_allTypes[CH_VIDEO_CODE_MAX] = {
	"CH_VIDEO_CODE_MPEG2", 
	"CH_VIDEO_CODE_MPEG2_HD", 
	"CH_VIDEO_CODE_MPEG4_ASP", 
	"CH_VIDEO_CODE_MPEG4_ASP_A",
	"CH_VIDEO_CODE_MPEG4_ASP_B",
	"CH_VIDEO_CODE_MPEG4_ASP_C",
	"CH_VIDEO_CODE_DIVX",
	"CH_VIDEO_CODE_VC1",
	"CH_VIDEO_CODE_H264", 
	"CH_VIDEO_CODE_AVS",
	"CH_VIDEO_CODE_VP6",
	"CH_VIDEO_CODE_VP6A",
	"CH_VIDEO_CODE_VP6F",
	"CH_VIDEO_CODE_VP8",
	"CH_VIDEO_CODE_REAL8",
	"CH_VIDEO_CODE_REAL9",
	"CH_VIDEO_CODE_SORENSON",
	"CH_VIDEO_CODE_H265"
	};

	if(enm_streamType < CH_VIDEO_CODE_MAX && enm_streamType >= 0)
	{
		return video_allTypes[enm_streamType];
	}
	return "UNKOWN_TYPE";

}

/**
FuncName:get_codec_type
@brief ��pmt��������������ת��Ϊchanghong�ӿ��е�������
 
@param [in] rb_isAud 
@param [in] stream_info 
@param [out] none
 
@return success  
@return fail  
   
@attention none
*/
static int get_codec_type(StreamInfo_t stream_info, CH_BOOL *rb_isAud)
{
	switch(stream_info.uc_PrivateEsStreamType)
	{
		case 0x03: /*MPEG-I ��Ƶ*/ 
		{ 
			return CH_AUDIO_CODE_MPEG1; 
			break; 
		} 
		case 0x04: /*MPEG-II ��Ƶ*/ 
		{ 
			return CH_AUDIO_CODE_MPEG2; 
			break; 
		} 
		case 0x06: /*AC-3 ��Ƶ*/ 
		{ 
			switch(stream_info.uc_Reserved)
			{
				case 0x6a:
					return CH_AUDIO_CODE_AC3; 
					break;
				case 0x7a:
					return CH_AUDIO_CODE_AC3_PLUS; 
					break;
				case 0x73:
				case 0x7b:
					return CH_AUDIO_CODE_DTS; 
					break;
				case 0x7c:
					return CH_AUDIO_CODE_AAC_ADTS; 
					break;
				case 0x59:
					return CH_AUDIO_CODE_AC3; 
					break;
				default:
					LOG("unkown stream type 06, tag=%#x\n", stream_info.uc_Reserved);
					break;
			}
			return MAX_CODETYPE; 
			break; 
		} 
		case 0x0F: /*ISO/IEC 13818-7 Audio with ADTS*/ 
		{ 
			return CH_AUDIO_CODE_AAC_ADTS; 
			break; 
		} 
		case 0x11: /*ISO/IEC 14496-3 Audio with the LATM*/ 
		{ 
			return CH_AUDIO_CODE_AAC_LOAS; 
			break; 
		} 
		
		case 0x1C: /*ISO/IEC 14496-3 Audio, without using any additional transport syntax, such as DST,ALS and SLS*/ 
		{ 
			return CH_AUDIO_CODE_AAC_LOAS; 
			break; 
		} 
		case 0x81: /*AC-3 ��Ƶ*/ 
		{ 
			return CH_AUDIO_CODE_AC3; 
			break; 
		} 
		case 0x87: /*EAC-3 ��Ƶ*/ 
		{ 
			return CH_AUDIO_CODE_AC3_PLUS; 
			break; 
		} 

		case 0x88: /*DRA ��Ƶ*/ 
		{ 
#if !defined(CFG_CHSSA_CHIP_MSTAR7S33B)
			return CH_AUDIO_CODE_DRA; 
#endif
			break; 
		} 

		case 0x01: /*MPEG-I ��Ƶ*/ 
		case 0x02: /*MPEG-II ��Ƶ*/ 
			{ 
				return  CH_VIDEO_CODE_MPEG2; 
				break; 
			} 
		case 0x10: /*MPEG-4 ��Ƶ*/ 
			{ 
				return  CH_VIDEO_CODE_MPEG4_ASP; 
				break; 
			} 
		case 0x1B: /*H.264 ��Ƶ*/ 
			{ 
				return  CH_VIDEO_CODE_H264; 
				break; 
			}
		case 0x24: /*H.264 ��Ƶ*/ 
			{ 
#if defined(CFG_CHSSA_CHIP_MSTAR7CX5)		
				return  CH_VIDEO_CODE_HEVC; 
#else
				return  CH_VIDEO_CODE_H265; 
#endif				
				break; 
			} 
			
		case 0x42: /*����AVS ��Ƶ*/ 
			{ 
				return  CH_VIDEO_CODE_AVS; 
				break; 
			} 
		case 0xEA: /*VC-1 ��Ƶ*/ 
			{ 
				return  CH_VIDEO_CODE_VC1; 
				break; 
			} 
		default:
			return MAX_CODETYPE ;
	}
}

/**
FuncName:stop_av
@brief ֹͣ��ǰAV����
 
@param [in] prog_info 
@param [out] none
 
@return success  
@return fail  
   
@attention none
*/
void stop_av(void)
{
	RUN_CMD("avfilter.stop 2\n"
		"avfilter.stop 1\n"
		"avfilter.stop 0\n");
	CHDRV_VIDEO_StopDecode(D_VIDEO_HANDLE);
	CHDRV_AUDIO_StopDecode(D_AUDIO_HANDLE);
}
/**
FuncName:restart_av
@brief �Ե�ǰ������ʼ����av
 
@param [in] prog_info 
@param [out] none
 
@return success  
@return fail  
   
@attention none
*/
void start_av(void)
{
	Channel_Info_t prog_info;
	CHDRV_VIDEO_StartDecode_t stru_Attrib;

	prog_info = gstru_current_prog;

	RUN_CMD("avfilter.start 0 0 %d\n"
		"avfilter.start 0 1 %d\n"
		"avfilter.start 0 2 %d\n",
		prog_info.pstru_Audio.size() > 0 ? prog_info.pstru_Audio[0].us_PrivateEsPid : 0x1fff,
		prog_info.pstru_Video.size() > 0 ? prog_info.pstru_Video[0].us_PrivateEsPid : 0x1fff,
		prog_info.us_PcrPid);
		
	memset(&stru_Attrib, 0, sizeof(stru_Attrib));
	stru_Attrib.enm_DecodeType = (CH_VIDEO_CODE_TYPE_e)D_GET_CODETYPE(prog_info.pstru_Video, 0);
	stru_Attrib.enm_StartMode = CHDRV_VIDEO_SYNC;
	stru_Attrib.enm_StreamFmt = CHDRV_VIDEO_STREAM_PES;
	EXPECT_EQ(CHDRV_VIDEO_StartDecode(D_VIDEO_HANDLE, &stru_Attrib), CHDRV_VIDEO_OK);

	CHDRV_AUDIO_StartDecodeParam_t stru_DecodePara;
	memset(&stru_DecodePara, 0, sizeof(stru_DecodePara));
	stru_DecodePara.enm_StreamPacketType = CHDRV_AUDIO_PACKET_PES;
	stru_DecodePara.enm_syncmode = CHDRV_AUDIO_SYNC;
	stru_DecodePara.enm_StreamType = (CH_AUDIO_CODE_TYPE_e)D_GET_CODETYPE(prog_info.pstru_Audio, 0);
	EXPECT_EQ(CHDRV_AUDIO_StartDecode(D_AUDIO_HANDLE, &stru_DecodePara), CHDRV_AUDIO_OK);

}

/**
FuncName:restart_av
@brief ����ͬ�Ĳ�������av
 
@param [in] prog_info 
@param [out] none
 
@return success  
@return fail  
   
@attention none
*/
void restart_av(void)
{
	stop_av();
	start_av();
}

/**
FuncName:task_play_beep
@brief ѭ��ע����Ƶ���ݵ����߳�
 
@param [in] data 
@param [out] none
 
@return success  
@return fail  
   
@attention none
*/
static void  task_play_beep(void *data)
{
	char *pbuf = NULL;
	int len;

	CH_RTOS_TaskEnter(NULL);
	CH_RTOS_WaitSemaphore(gh_semaHandle,CH_RTOS_TIMEOUT_INFINITY);
	len = fsys_read_file(fw_getenv((char*)data), pbuf);
	while(len > 0 && gb_beep_play)
	{
		CHDRV_AUDIO_RamInputData(D_AUDIO_HANDLE,pbuf,len);
	}
	CH_RTOS_FreeMemory(gh_APP_HEAP, pbuf);
	CH_RTOS_SignalSemaphore(gh_semaHandle);
	CH_RTOS_TaskExit();
}
	
static CH_BOOL cmp_StreamName(char *p1, char *p2)
{
	int n1, n2;
	char *at1, *at2;
	
	if(!p1 || !p2)
	{
		return 1;
	}
	at1 = strstr(p1, "&");
	at2 = strstr(p2, "&");
	if(!at1 || !at2)
	{
		return 1;
	}
	n1 = at1 - p1;
	n2 = at2 - p2;
	if(n1 != n2)
	{
		return 1;
	}
	return memcmp(p1, p2, n1);
}


#if ENABLE_TESTCASE

CASE_F(TEST_AV_OPEN1,
	"��һ·video,һ·audio�Ƿ�ɳɹ�"){
	CHDRV_AUDIO_Handle h_Aud = NULL;
	CHDRV_VIDEO_Handle h_Vid = NULL; 

	RUN_CMD("route.create 0 0");

	if(route_GetHandleNums() > 0)
	{
		 CHDRV_ROUTE_GetADecode(route_GetHandle(0), &h_Aud);
	}
	if(h_Aud == NULL)
	{
		EXPECT_EQ(CHDRV_AUDIO_OK,CHDRV_AUDIO_Open(fw_getenv_i("AUDIO_DEVICE_ID"),&h_Aud));
		EXPECT_NE(h_Aud,(void *)NULL);
	}
#ifdef CHSSA_CHIP_7583
	chdrv_platform_connect_demux_adecoder(0,0,h_Aud);
#endif

	if(route_GetHandleNums() > 0)
	{
		 CHDRV_ROUTE_GetVDecode(route_GetHandle(0), &h_Vid);
	}
	if(h_Vid == NULL )
	{
		EXPECT_EQ(CHDRV_VIDEO_OK,CHDRV_VIDEO_Open(fw_getenv_i("VIDEO_DEVICE_ID"),&h_Vid));
		EXPECT_NE(h_Vid,(void *)NULL);
	}
#ifdef CHSSA_CHIP_7583
	chdrv_platform_connect_demux_vdecoder(0,0,h_Vid);
#endif

	

	
	
}



CASE_F(TEST_AV_RAM_START1,
	"ram startһ��I֡ͼƬ�Ƿ�ɹ�"){
		int len;
		char *pbuf = NULL;
		
	ASSERT_GT(len = fsys_read_file(fw_getenv("IFRAME_FILE"), pbuf), 0);
	EXPECT_EQ(CHDRV_VIDEO_OK,CHDRV_VIDEO_StartRamDecode(D_VIDEO_HANDLE, pbuf, len));

	PROMPT("�۲�HDMI��CVBS������ڣ�I֡ͼƬ��ʾ�����밴Y:");
	EXPECT_EQ(test_input_yes(),CH_TRUE);
	CH_RTOS_FreeMemory(gh_APP_HEAP, pbuf);
}



CASE_F(TEST_AV_RAM_STOP1,
	"ram stopI֡ͼƬ�Ƿ�ɹ�"){

	EXPECT_EQ(CHDRV_VIDEO_OK,CHDRV_VIDEO_StopRamDecode(D_VIDEO_HANDLE));
}


CASE_F(TEST_AV_RAM_BEEP_1,
	"ram startһ��pcm��ʽ��beep�����ļ�,�Ƿ���������"){

	CHDRV_AUDIO_StartDecodeParam_t stru_Audio;

	memset(&stru_Audio, 0, sizeof(stru_Audio));
	stru_Audio.enm_syncmode = CHDRV_AUDIO_ASYNC;
	stru_Audio.enm_StreamType = CH_AUDIO_CODE_LPCM;
	stru_Audio.enm_StreamPacketType = CHDRV_AUDIO_PACKET_ES;

	

	CH_RTOS_TaskHandle i_Task = NULL;
	EXPECT_EQ(CHDRV_AUDIO_RamStartDecode(D_AUDIO_HANDLE,&stru_Audio), CHDRV_AUDIO_OK);

	EXPECT_EQ(CH_RTOS_CreateSemaphore(1,&gh_semaHandle), CH_RTOS_OK);

	gb_beep_play = CH_TRUE;
	
	CH_RTOS_CreateTask(task_play_beep, (void*)"BEEP_FILE_1",NULL, 8 * 1024,200,(const S8*)"audio ram",&i_Task);


	PROMPT("�Ƿ������й��ɵ�BEEP�������밴Y������N:");
	EXPECT_EQ(test_input_yes(),CH_TRUE) << "��Ƶע�벥��LPCM�ļ�ʧ��";;
	gb_beep_play = CH_FALSE;
	
	EXPECT_EQ(CH_RTOS_WaitSemaphore(gh_semaHandle,CH_RTOS_TIMEOUT_INFINITY), CH_RTOS_OK);
	EXPECT_EQ(CHDRV_AUDIO_RamStopDecode(D_AUDIO_HANDLE), CHDRV_AUDIO_OK);
	
	EXPECT_EQ(CH_RTOS_WaitTaskTerm(i_Task, CH_RTOS_TIMEOUT_INFINITY), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteTask(i_Task), CH_RTOS_OK);
}


CASE_F(TEST_AV_RAM_BEEP_2,
	"ram startһ��MPEG1��ʽ��beep�����ļ�,�Ƿ���������"){

	CHDRV_AUDIO_StartDecodeParam_t stru_Audio;

	memset(&stru_Audio, 0, sizeof(stru_Audio));
	stru_Audio.enm_syncmode = CHDRV_AUDIO_ASYNC;
	stru_Audio.enm_StreamType = CH_AUDIO_CODE_LPCM;
	stru_Audio.enm_StreamPacketType = CHDRV_AUDIO_PACKET_ES;

	

	CH_RTOS_TaskHandle i_Task = NULL;
	EXPECT_EQ(CHDRV_AUDIO_RamStartDecode(D_AUDIO_HANDLE,&stru_Audio), CHDRV_AUDIO_OK);

	EXPECT_EQ(CH_RTOS_CreateSemaphore(1,&gh_semaHandle), CH_RTOS_OK);

	gb_beep_play = CH_TRUE;
	
	CH_RTOS_CreateTask(task_play_beep, (void*)"BEEP_FILE_2",NULL, 8 * 1024,200,(const S8*)"audio ram",&i_Task);


	PROMPT("�Ƿ������й��ɵ�BEEP�������밴Y������N:");
	EXPECT_EQ(test_input_yes(),CH_TRUE) << "��Ƶע�벥��MPEG1ʧ��";
	gb_beep_play = CH_FALSE;
	
	EXPECT_EQ(CH_RTOS_WaitSemaphore(gh_semaHandle,CH_RTOS_TIMEOUT_INFINITY), CH_RTOS_OK);
	EXPECT_EQ(CHDRV_AUDIO_RamStopDecode(D_AUDIO_HANDLE), CHDRV_AUDIO_OK);
	
	EXPECT_EQ(CH_RTOS_WaitTaskTerm(i_Task, CH_RTOS_TIMEOUT_INFINITY), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteTask(i_Task), CH_RTOS_OK);

}

CASE_F(TEST_AV_SET_WIN1,
	"SetWinAttrib���л�ģʽ�л�Ϊ����ģʽ��GetWinAttrib�����õ�ֵ�Ƿ�һ��")
	{
	CHDRV_VIDEO_WindowAttrib_t stru_win_attr_bk;	//window ȫ�ֱ���
	CHDRV_VIDEO_WindowAttrib_t stru_vin_attr;

	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE,&stru_win_attr_bk);

	stru_win_attr_bk.b_BlankOnStop = CH_FALSE;
	
	CHDRV_VIDEO_SetWinAttrib(D_VIDEO_HANDLE,&stru_win_attr_bk);

	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE,&stru_vin_attr);
	EXPECT_EQ(stru_win_attr_bk.b_BlankOnStop,stru_vin_attr.b_BlankOnStop);
	
	stru_win_attr_bk.b_BlankOnStop = CH_TRUE;
	
	CHDRV_VIDEO_SetWinAttrib(D_VIDEO_HANDLE,&stru_win_attr_bk);

	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE,&stru_vin_attr);
	EXPECT_EQ(stru_win_attr_bk.b_BlankOnStop,stru_vin_attr.b_BlankOnStop);

	
}



CASE_F(TEST_AV_START1,
	"����16:9��,����Ƶ����Ƿ�����")
	{
	int prognum = 0;

	prognum = request_play_program("AV_STREAM_16X9", "TUNER_PARAMS1");
	RUN_CMD("tuner.lock %s", fw_getenv("TUNER_PARAMS1"));

	
	RUN_CMD("av.start %d %d\n",get_demux_id("TUNER_PARAMS1"),prognum);
	
	PROMPT("��Ƶ����Ƿ�����?���밴Y������N:");
	EXPECT_EQ(test_input_yes(),CH_TRUE);


	RUN_CMD("avfilter.set_pid 0 %d\n",
			gstru_current_prog.pstru_Video.size() > 0 ? gstru_current_prog.pstru_Video[0].us_PrivateEsPid : 0x1fff);
	RUN_CMD("avfilter.set_pid 1 %d\n",
		gstru_current_prog.pstru_Audio.size() > 0 ? gstru_current_prog.pstru_Audio[0].us_PrivateEsPid : 0x1fff);

	PROMPT("AV���ݱ������ˣ��Ƿ�ֹͣ�˲���?���밴Y������N:");
	EXPECT_EQ(test_input_yes(),CH_TRUE) << "CHDRV_AVFILTER_SetPid����������Ч";
	
	RUN_CMD("avfilter.set_pid 0 %d\n",
		gstru_current_prog.pstru_Audio.size() > 0 ? gstru_current_prog.pstru_Audio[0].us_PrivateEsPid : 0x1fff);

	RUN_CMD("avfilter.set_pid 1 %d\n",
		gstru_current_prog.pstru_Video.size() > 0 ? gstru_current_prog.pstru_Video[0].us_PrivateEsPid : 0x1fff);

	RUN_CMD("avfilter.set_pid 2 %d\n",
		gstru_current_prog.pstru_Video.size() > 0 ? gstru_current_prog.pstru_Video[0].us_PrivateEsPid : 0x1fff);

	PROMPT("AV���ݱ����������ˣ��Ƿ񲥷�?���밴Y������N:");
	EXPECT_EQ(test_input_yes(),CH_TRUE) << "CHDRV_AVFILTER_SetPid����������Ч";

	restart_av();
	PROMPT("������AV���Ƿ�����?���밴Y������N:");
	EXPECT_EQ(test_input_yes(),CH_TRUE);

	RUN_CMD("av.stop 0\n");
	RUN_CMD("av.start %d %d\n",get_demux_id("TUNER_PARAMS1"),prognum);
	PROMPT("av restart ������AV���Ƿ�����?���밴Y������N:");
	EXPECT_EQ(test_input_yes(),CH_TRUE);


		
}



CASE_F(TEST_AV_CALL1,
	"��audio��videoע��3���ص������Ƿ�ɹ�")
	{

	memset(gi_vid_event_count, 0, sizeof(gi_vid_event_count));
	memset(gi_aud_event_count, 0, sizeof(gi_aud_event_count));
	EXPECT_EQ(CHDRV_AUDIO_InstallCallback(D_AUDIO_HANDLE,audio_call1), CHDRV_AUDIO_OK);
	EXPECT_EQ(CHDRV_AUDIO_InstallCallback(D_AUDIO_HANDLE,audio_call2), CHDRV_AUDIO_OK);
	EXPECT_EQ(CHDRV_AUDIO_InstallCallback(D_AUDIO_HANDLE,audio_call3), CHDRV_AUDIO_OK);
	EXPECT_EQ(CHDRV_VIDEO_InstallCallback(D_VIDEO_HANDLE,video_call1), CHDRV_VIDEO_OK);
	EXPECT_EQ(CHDRV_VIDEO_InstallCallback(D_VIDEO_HANDLE,video_call2), CHDRV_VIDEO_OK);
	EXPECT_EQ(CHDRV_VIDEO_InstallCallback(D_VIDEO_HANDLE,video_call3), CHDRV_VIDEO_OK);

	//���¿���һ�Σ�������Щƽֻ̨���յ���һ֡���¼�
	restart_av();
}

CASE_F(TEST_AV_CALL2,
	"av��3��callback,�Ƿ���յ���DECODE�¼�")
{
	CH_RTOS_TaskDelay(1000);

	EXPECT_GT(gi_vid_event_count[CHDRV_VIDEO_EVENT_DECODED][0], 0) << "av���ųɹ�������һ��callbackδ�յ���decoded�¼�";
	EXPECT_GT(gi_vid_event_count[CHDRV_VIDEO_EVENT_DECODED][1], 0) << "av���ųɹ������ڶ���callbackδ�յ���decoded�¼�";
	EXPECT_GT(gi_vid_event_count[CHDRV_VIDEO_EVENT_DECODED][2], 0) << "av���ųɹ�����������callbackδ�յ���decoded�¼�";

	EXPECT_GT(gi_aud_event_count[CHDRV_AUDIO_EVENT_NEW_FRAME][0], 0) << "av���ųɹ�������һ��callbackδ�յ���new frame�¼�";
	EXPECT_GT(gi_aud_event_count[CHDRV_AUDIO_EVENT_NEW_FRAME][1], 0) << "av���ųɹ������ڶ���callbackδ�յ���new frame�¼�";
	EXPECT_GT(gi_aud_event_count[CHDRV_AUDIO_EVENT_NEW_FRAME][2], 0) << "av���ųɹ�����������callbackδ�յ���new frame�¼�";
}


CASE_F(TEST_AV_STATUS1,
	"�ֱ����av��get status����õ�״̬��ӦΪgood")
	{

	CHDRV_VIDEO_STATUS_e vid_status;
	CHDRV_AUDIO_STATUS_e aud_status;

	EXPECT_EQ(CHDRV_VIDEO_GetStatus(D_VIDEO_HANDLE,&vid_status), CHDRV_VIDEO_OK);
	EXPECT_EQ(CHDRV_AUDIO_GetStatus(D_AUDIO_HANDLE,&aud_status), CHDRV_AUDIO_OK);
	EXPECT_EQ(vid_status,CHDRV_VIDEO_STATUS_GOOD);
	EXPECT_EQ(aud_status,CHDRV_AUDIO_STATUS_GOOD);
	}


CASE_F(TEST_AV_SET_WINATTRIB8,
	"�л�ģʽ�л�Ϊ����ģʽ��GetWinAttrib�����õ�ֵ�Ƿ�һ��")
	{
	
	CHDRV_VIDEO_WindowAttrib_t stru_attr;
	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE, &stru_attr);
	stru_attr.b_BlankOnStop= CH_TRUE;
	CHDRV_VIDEO_SetWinAttrib(D_VIDEO_HANDLE, &stru_attr);
	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE, &stru_attr);

	EXPECT_EQ(stru_attr.b_BlankOnStop, CH_TRUE);

}

CASE_F(TEST_AV_STOP1,
	"����stop decoder��Ӧ����OK")
	{

	EXPECT_EQ(CHDRV_VIDEO_StopDecode(D_VIDEO_HANDLE), CHDRV_VIDEO_OK);
	EXPECT_EQ(CHDRV_AUDIO_StopDecode(D_AUDIO_HANDLE), CHDRV_AUDIO_OK);
}

CASE_F(TEST_AV_STOP2,
	"ѯ�ʲ���Ա�۲죬��ʱ����ӦΪ��ɫ")
	{

	PROMPT("�Ƿ���Ϊ��ɫ?���밴Y������N:");
	EXPECT_EQ(test_input_yes(),CH_TRUE) << "��̨����ģʽδ������";
}



CASE_F(TEST_AV_STATUS3,
	"��ͣ��ֹͣ�����������������ֵ�����������ֱ����av��get status����״̬ӦΪNOHEADER")
	{

	restart_av();
	RUN_CMD("tuner.lock %s", fw_getenv("TUNER_INVALID_PARAMS"));//�޸�Ϊ�������Ƶ��
	
	
	CHDRV_VIDEO_STATUS_e vid_status;
	CHDRV_AUDIO_STATUS_e aud_status;

	CHDRV_VIDEO_GetStatus(D_VIDEO_HANDLE, &vid_status);
	CHDRV_AUDIO_GetStatus(D_AUDIO_HANDLE, &aud_status);

	EXPECT_EQ(vid_status, CHDRV_VIDEO_STATUS_NOHEADER);
	EXPECT_EQ(aud_status, CHDRV_AUDIO_STATUS_NOHEADER);

	CHDRV_AVFILTER_STATUS_e enm_Status;
	if(avfilter_GetHandleNums() > 0)
	{
		EXPECT_EQ(CHDRV_AVFILTER_GetStatus(avfilter_GetHandle(0), &enm_Status), CHDRV_AVFILTER_OK);
		EXPECT_EQ(enm_Status, CHDRV_AVFILTER_STATUS_NO_DATA);
	}
	if(avfilter_GetHandleNums() > 1)
	{
		EXPECT_EQ(CHDRV_AVFILTER_GetStatus(avfilter_GetHandle(1), &enm_Status), CHDRV_AVFILTER_OK);
		EXPECT_EQ(enm_Status, CHDRV_AVFILTER_STATUS_NO_DATA);
	}
}

CASE_F(TEST_AV_SET_WINATTRIB9,
	"�л�ģʽ�л�Ϊ��֡ģʽ��GetWinAttrib�����õ�ֵ�Ƿ�һ��")
	{
	
	CHDRV_VIDEO_WindowAttrib_t stru_attr;
	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE, &stru_attr);
	stru_attr.b_BlankOnStop= CH_FALSE;
	CHDRV_VIDEO_SetWinAttrib(D_VIDEO_HANDLE, &stru_attr);
	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE, &stru_attr);

	EXPECT_EQ(stru_attr.b_BlankOnStop, CH_FALSE);

}

CASE_F(TEST_AV_STOP3,
	"����stop av decoder����Ӧ����OK")
	{
	RUN_CMD("tuner.lock %s", fw_getenv("TUNER_PARAMS1"));//�޸�Ϊ�������Ƶ��
	PROMPT("��Ƶ�Ƿ�����������?���밴Y�������밴N:");
	EXPECT_EQ(test_input_yes(),CH_TRUE);
	EXPECT_EQ(CHDRV_VIDEO_StopDecode(D_VIDEO_HANDLE), CHDRV_VIDEO_OK);
	EXPECT_EQ(CHDRV_AUDIO_StopDecode(D_AUDIO_HANDLE), CHDRV_AUDIO_OK);
}

CASE_F(TEST_AV_STOP4,
	"ѯ���Ƿ�Ϊ���һ֡")
	{

	PROMPT("�����Ƿ���ʾΪ���һ֡?���밴Y�������밴N:");
	EXPECT_EQ(test_input_yes(),CH_TRUE)  << "��̨��֡ģʽδ������";
}

CASE_F(TEST_AV_CLEARBUFFER,
	"����clear video buffer��Ӧ����OK")
	{

	EXPECT_EQ(CHDRV_VIDEO_ClearVideoBuffer(D_VIDEO_HANDLE),CHDRV_VIDEO_OK);

	PROMPT("�����Ƿ�Ϊ����?���밴Y�������밴N:");
	EXPECT_EQ(test_input_yes(),CH_TRUE) << "ClearVideoBufferδ�ܳɹ�����֡��Ƶ�����";
}




CASE_F(TEST_AV_START5,
	"�Դ����decoder type�ٴ�open av decoder��Ӧ����OK")
	{
		RUN_CMD("tuner.lock %s", fw_getenv("TUNER_PARAMS1"));
		RUN_CMD("avfilter.set_pid 0 %d\n",
			gstru_current_prog.pstru_Video.size() > 0 ? gstru_current_prog.pstru_Video[0].us_PrivateEsPid : 0x1fff);
		RUN_CMD("avfilter.set_pid 1 %d\n",
			gstru_current_prog.pstru_Audio.size() > 0 ? gstru_current_prog.pstru_Audio[0].us_PrivateEsPid : 0x1fff);

		CHDRV_VIDEO_StartDecode_t stru_Vid;
		CHDRV_AUDIO_StartDecodeParam_t stru_Aud;
		int type;

		type = D_GET_CODETYPE(gstru_current_prog.pstru_Video, 0);
		stru_Vid.enm_DecodeType = (CH_VIDEO_CODE_TYPE_e)(type);	
		stru_Vid.enm_StartMode = CHDRV_VIDEO_SYNC;
		stru_Vid.enm_StreamFmt = CHDRV_VIDEO_STREAM_PES;

		type = D_GET_CODETYPE(gstru_current_prog.pstru_Audio, 0);
		memset(&stru_Aud, 0, sizeof(stru_Aud));
		stru_Aud.enm_syncmode = CHDRV_AUDIO_SYNC;
		stru_Aud.enm_StreamType = (CH_AUDIO_CODE_TYPE_e)(type);	
		stru_Aud.enm_StreamPacketType = CHDRV_AUDIO_PACKET_PES;
		EXPECT_EQ(CHDRV_VIDEO_StartDecode(D_VIDEO_HANDLE, &stru_Vid),CHDRV_VIDEO_OK);
		EXPECT_EQ(CHDRV_AUDIO_StartDecode(D_AUDIO_HANDLE, &stru_Aud),CHDRV_AUDIO_OK);

	
}

CASE_F(TEST_AV_START3,
	"����Ա�۲컭��Ӧ�������������")
	{

	PROMPT("��Ƶ�Ƿ���ֹͣ��������������?���밴Y�������밴N:");
	EXPECT_EQ(test_input_yes(),CH_TRUE);
}

CASE_F(TEST_AV_STATUS4,
	"�ֱ����av��get status��״̬ӦΪerror")
	{

	CHDRV_VIDEO_STATUS_e vid_status;
	CHDRV_AUDIO_STATUS_e aud_status;

	CHDRV_VIDEO_GetStatus(D_VIDEO_HANDLE,&vid_status);
	CHDRV_AUDIO_GetStatus(D_AUDIO_HANDLE,&aud_status);

	EXPECT_EQ(vid_status,CHDRV_VIDEO_STATUS_ERR);
	EXPECT_EQ(aud_status,CHDRV_AUDIO_STATUS_ERR);
}

CASE_F(TEST_AV_CALL4,
	"�ֱ���av��3���ص���������Ӧ�յ���DECODED ERROR�¼�")
	{
	CH_RTOS_TaskDelay(1000);
	EXPECT_GT(gi_vid_event_count[CHDRV_VIDEO_EVENT_DECODING_ERROR][0], 0) << "av�������Ӧ�յ�����һ��decode error�¼�";
	EXPECT_GT(gi_vid_event_count[CHDRV_VIDEO_EVENT_DECODING_ERROR][1], 0) << "av�������Ӧ�յ�����һ��decode error�¼�";
	EXPECT_GT(gi_vid_event_count[CHDRV_VIDEO_EVENT_DECODING_ERROR][2], 0) << "av�������Ӧ�յ�����һ��decode error�¼�";

	EXPECT_GT(gi_aud_event_count[CHDRV_AUDIO_EVENT_DECODE_ERROR][0], 0) << "av�������Ӧ�յ�����һ��decode error�¼�";
	EXPECT_GT(gi_aud_event_count[CHDRV_AUDIO_EVENT_DECODE_ERROR][1], 0) << "av�������Ӧ�յ�����һ��decode error�¼�";
	EXPECT_GT(gi_aud_event_count[CHDRV_AUDIO_EVENT_DECODE_ERROR][2], 0) << "av�������Ӧ�յ�����һ��decode error�¼�";

}

CASE_F(TEST_AUDIO_FUNCTION8,
	"ɾ����ע��Ļص�������Ӧ���ܳɹ�")
	{

	EXPECT_EQ(CHDRV_AUDIO_UnInstallCallback(D_AUDIO_HANDLE,audio_call1),CHDRV_AUDIO_OK);
	EXPECT_EQ(CHDRV_AUDIO_UnInstallCallback(D_AUDIO_HANDLE,audio_call2),CHDRV_AUDIO_OK);
	EXPECT_EQ(CHDRV_AUDIO_UnInstallCallback(D_AUDIO_HANDLE,audio_call3),CHDRV_AUDIO_OK);
	EXPECT_EQ(CHDRV_VIDEO_UnInstallCallback(D_VIDEO_HANDLE,video_call1),CHDRV_VIDEO_OK);
	EXPECT_EQ(CHDRV_VIDEO_UnInstallCallback(D_VIDEO_HANDLE,video_call2),CHDRV_VIDEO_OK);
	EXPECT_EQ(CHDRV_VIDEO_UnInstallCallback(D_VIDEO_HANDLE,video_call3),CHDRV_VIDEO_OK);
}

CASE_F(TEST_AV_START4,
	"�ٴ�����ȷ�Ĳ���start audio video��������������������ޱ�����Ȼ���ٹرգ���֡��ʾ�������ظ���������10��")
	{

	int decode_num = 3;

	RUN_CMD("avfilter.set_pid 0 %d\n",
		gstru_current_prog.pstru_Audio.size() > 0 ? gstru_current_prog.pstru_Audio[0].us_PrivateEsPid : 0x1fff);

	RUN_CMD("avfilter.set_pid 1 %d\n",
		gstru_current_prog.pstru_Video.size() > 0 ? gstru_current_prog.pstru_Video[0].us_PrivateEsPid : 0x1fff);

	RUN_CMD("avfilter.set_pid 2 %d\n",
		gstru_current_prog.pstru_Video.size() > 0 ? gstru_current_prog.pstru_Video[0].us_PrivateEsPid : 0x1fff);


	PROMPT("���潫��������AV DECORDER�Ĳ��ԣ�������Զ�����AV10�Σ������Աע��۲컭�����������ˣ������Ƿ��б����Ϳ��٣�׼�����Ժ�س�:");
	test_input_none();
	CHDRV_VIDEO_WindowAttrib_t stru_attr;
	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE, &stru_attr);
	stru_attr.b_BlankOnStop= CH_TRUE;
	CHDRV_VIDEO_SetWinAttrib(D_VIDEO_HANDLE, &stru_attr);
	
	while(decode_num-- > 0)
	{
		restart_av();
		CH_RTOS_TaskDelay(5000);
		LOG("����AV...\n");
	}

	PROMPT("������������������ޱ������л�ʱ�м��к���?���밴Y�������밴N:");
	EXPECT_EQ(test_input_yes(),CH_TRUE);

}

CASE_F(TEST_AV_CLOSE1,
	"����closeɾ��audio video�����Ӧ���سɹ�")
	{

#if 0 	/*sqzou20161115*/
	if(CHDRV_VIDEO_StopDecode(D_VIDEO_HANDLE) != CHDRV_VIDEO_OK)
	{
		LOG("ERROR: CHDRV_VIDEO_StopDecode return\n");
	}
	

	if(CHDRV_AUDIO_StopDecode(D_AUDIO_HANDLE) != CHDRV_AUDIO_OK)
	{
		LOG("error: CHDRV_AUDIO_StopDecode return !\n");
	}
	
	
	EXPECT_EQ(CHDRV_AUDIO_Close(D_AUDIO_HANDLE),CHDRV_AUDIO_OK);
	EXPECT_EQ(CHDRV_VIDEO_Close(D_VIDEO_HANDLE),CHDRV_VIDEO_OK);
#endif

	RUN_CMD("av.stop 0\n");
	
}


CASE_F(TEST_AV_IFRAME_PLAY_1,
	"����һ��16:9��I֡������Ƿ�ɹ�")
	{

	RUN_CMD("vid.ram_start 0 %s", fw_getenv("IFRAME_16TO9_FILE"));
	
	PROMPT("I֡��ʾ�Ƿ�����?���밴Y������N:");
	EXPECT_EQ(test_input_yes(),CH_TRUE) << "I֡�������ɹ�";
}


CASE_F(TEST_AV_SET_WINATTRIB1,
	"�л�������ʾ����Ϊ16��9��GetWinAttrib�����õ�ֵ�Ƿ�һ��")
	{

	CHDRV_VIDEO_WindowAttrib_t stru_attr;

	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE,&stru_attr);
	stru_attr.enm_DispAspect = CHDRV_VIDEO_ASPECT_16X9;
	
	CHDRV_VIDEO_SetWinAttrib(D_VIDEO_HANDLE,&stru_attr);

	memset(&stru_attr, 0, sizeof(stru_attr));
	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE,&stru_attr);
	
	EXPECT_EQ(stru_attr.enm_DispAspect,CHDRV_VIDEO_ASPECT_16X9);
}

CASE_F(TEST_AV_SET_WINATTRIB2,
	"�����л�������ʾģʽ��ѯ�ʲ���Ա����Ӧ��Ϊ������GetWinAttrib�����õ�ֵ�Ƿ�һ��")
	{
	
	CHDRV_VIDEO_WindowAttrib_t stru_attr;

	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE,&stru_attr);
	
	stru_attr.enm_AspectMode = CHDRV_VIDEO_WINDOW_PANSCAN;
	CHDRV_VIDEO_SetWinAttrib(D_VIDEO_HANDLE,&stru_attr);

	memset(&stru_attr, 0, sizeof(stru_attr));
	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE,&stru_attr);
	EXPECT_EQ(stru_attr.enm_AspectMode,CHDRV_VIDEO_WINDOW_PANSCAN);
	PROMPT("�����Ƿ���ʾΪ����?���밴Y�������밴N:");
	EXPECT_EQ(test_input_yes(), CH_TRUE);

	stru_attr.enm_AspectMode = CHDRV_VIDEO_WINDOW_LETTERBOX;
	CHDRV_VIDEO_SetWinAttrib(D_VIDEO_HANDLE, &stru_attr);
	memset(&stru_attr, 0, sizeof(stru_attr));
	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE, &stru_attr);
	EXPECT_EQ(stru_attr.enm_AspectMode, stru_attr.enm_AspectMode);
	PROMPT("�����Ƿ���ʾΪ����?���밴Y�������밴N:");
	EXPECT_EQ(test_input_yes(), CH_TRUE);

	stru_attr.enm_AspectMode = CHDRV_VIDEO_WINDOW_COMBINE;
	CHDRV_VIDEO_SetWinAttrib(D_VIDEO_HANDLE, &stru_attr);
	memset(&stru_attr, 0, sizeof(stru_attr));
	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE, &stru_attr);
	EXPECT_EQ(stru_attr.enm_AspectMode, CHDRV_VIDEO_WINDOW_COMBINE);
	PROMPT("�����Ƿ���ʾΪ����?���밴Y�������밴N:");
	EXPECT_EQ(test_input_yes(), CH_TRUE);
	
	stru_attr.enm_AspectMode = CHDRV_VIDEO_WINDOW_FULL;
	CHDRV_VIDEO_SetWinAttrib(D_VIDEO_HANDLE, &stru_attr);
	memset(&stru_attr, 0, sizeof(stru_attr));
	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE, &stru_attr);
	EXPECT_EQ(stru_attr.enm_AspectMode, CHDRV_VIDEO_WINDOW_FULL);
	PROMPT("�����Ƿ���ʾΪ����?���밴Y�������밴N:");
	EXPECT_EQ(test_input_yes(), CH_TRUE);

}



CASE_F(TEST_AV_SET_WINATTRIB3,
	"�л�������ʾ����Ϊ4��3��GetWinAttrib�����õ�ֵ�Ƿ�һ��")
	{

	CHDRV_VIDEO_WindowAttrib_t stru_attr;

	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE,&stru_attr);
	stru_attr.enm_DispAspect = CHDRV_VIDEO_ASPECT_4X3;

	CHDRV_VIDEO_SetWinAttrib(D_VIDEO_HANDLE,&stru_attr);
	memset(&stru_attr, 0, sizeof(stru_attr));
	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE,&stru_attr);

	EXPECT_EQ(stru_attr.enm_DispAspect, CHDRV_VIDEO_ASPECT_4X3);
}

CASE_F(TEST_AV_SET_WINATTRIB4,
	"�л�������ʾģʽΪPANSCAN���������ұ��ü���GetWinAttrib�����õ�ֵ�Ƿ�һ��")
{
	
	CHDRV_VIDEO_WindowAttrib_t stru_attr;

	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE, &stru_attr);
	
	stru_attr.enm_AspectMode = CHDRV_VIDEO_WINDOW_PANSCAN;
	CHDRV_VIDEO_SetWinAttrib(D_VIDEO_HANDLE, &stru_attr);

	memset(&stru_attr, 0, sizeof(stru_attr));
	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE, &stru_attr);
	
	EXPECT_EQ(stru_attr.enm_AspectMode, CHDRV_VIDEO_WINDOW_PANSCAN);

	PROMPT("��ǰģʽΪPANSCAN����������Ӧ���ü�,��ʾ��ȷ���밴Y�������밴N:");
	EXPECT_EQ(test_input_yes(), CH_TRUE);

}

CASE_F(TEST_AV_SET_WINATTRIB5,
	"�л�������ʾģʽΪLEFTBOX�����±����ɺ�ɫ��GetWinAttrib�����õ�ֵ�Ƿ�һ��")
	{
	
	CHDRV_VIDEO_WindowAttrib_t stru_attr;

	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE,&stru_attr);
	stru_attr.enm_AspectMode = CHDRV_VIDEO_WINDOW_LETTERBOX;
	CHDRV_VIDEO_SetWinAttrib(D_VIDEO_HANDLE,&stru_attr);
	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE,&stru_attr);
	
	EXPECT_EQ(stru_attr.enm_AspectMode,stru_attr.enm_AspectMode);

	PROMPT("��ǰģʽΪLEFTBOX���������±����ɺ�ɫ,��ʾ��ȷ���밴Y�������밴N:");
	EXPECT_EQ(test_input_yes(),CH_TRUE);

}
	

CASE_F(TEST_AV_SET_WINATTRIB6,
	"�л�������ʾģʽΪFULL�����±����죬GetWinAttrib�����õ�ֵ�Ƿ�һ��")
	{
	
	CHDRV_VIDEO_WindowAttrib_t stru_attr;

	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE, &stru_attr);
	stru_attr.enm_AspectMode = CHDRV_VIDEO_WINDOW_FULL;
	CHDRV_VIDEO_SetWinAttrib(D_VIDEO_HANDLE, &stru_attr);
	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE, &stru_attr);
	
	EXPECT_EQ(stru_attr.enm_AspectMode, CHDRV_VIDEO_WINDOW_FULL);

	PROMPT("��ǰģʽΪFULL�����±����죬,��ʾ��ȷ���밴Y�������밴N:");
	EXPECT_EQ(test_input_yes(),CH_TRUE);

}

	

CASE_F(TEST_AV_SET_WINATTRIB7,
	"������ʾģʽΪCOMBO��GetWinAttrib�����õ�ֵ�Ƿ�һ��")
	{
	
	CHDRV_VIDEO_WindowAttrib_t stru_attr;

	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE, &stru_attr);
	stru_attr.enm_AspectMode = CHDRV_VIDEO_WINDOW_COMBINE;
	CHDRV_VIDEO_SetWinAttrib(D_VIDEO_HANDLE, &stru_attr);
	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE, &stru_attr);
	
	EXPECT_EQ(stru_attr.enm_AspectMode, CHDRV_VIDEO_WINDOW_COMBINE);

	PROMPT("��ǰģʽΪCOMBINE��������ʾЧ������LEFT��PANSCAN֮��,��ʾ��ȷ���밴Y�������밴N:");
	EXPECT_EQ(test_input_yes(), CH_TRUE);

}

CASE_F(TEST_AV_VIDEO_INFO2,
	"����GetVideoInfo��Ӧ����OK����õĿ�߱�ӦΪ16:9")
	{

	CHDRV_VIDEO_Info_t vidInfo;
	EXPECT_EQ(CHDRV_VIDEO_GetVideoInfo(D_VIDEO_HANDLE, &vidInfo),CHDRV_VIDEO_OK);
	EXPECT_EQ(vidInfo.enm_AspectRatio,CHDRV_VIDEO_ASPECT_16X9) << "��ȡ����Ƶ��߱ȴ���";
	EXPECT_EQ(vidInfo.enm_DecodeType,CH_VIDEO_CODE_MPEG2) << "��ȡ����Ƶ�������ʹ���";
	
	RUN_CMD("vid.ram_stop 0");

	
}

CASE_F(TEST_AV_IFRAME_PLAY_2,
	"����һ��4:3��I֡������Ƿ�ɹ�")
{

	RUN_CMD("vid.ram_start 0 %s", fw_getenv("IFRAME_4TO3_FILE"));
	
	PROMPT("I֡��ʾ�Ƿ�����?���밴Y������N:");
	EXPECT_EQ(test_input_yes(),CH_TRUE) << "I֡�������ɹ�";
}

CASE_F(TEST_AV_SET_WINATTRIB10,
	"�л�������ʾ����Ϊ4��3��GetWinAttrib�����õ�ֵ�Ƿ�һ��")
	{

	CHDRV_VIDEO_WindowAttrib_t stru_attr;

	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE,&stru_attr);
	stru_attr.enm_DispAspect = CHDRV_VIDEO_ASPECT_4X3;
	
	CHDRV_VIDEO_SetWinAttrib(D_VIDEO_HANDLE,&stru_attr);

	memset(&stru_attr, 0, sizeof(stru_attr));
	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE,&stru_attr);
	
	EXPECT_EQ(stru_attr.enm_DispAspect, CHDRV_VIDEO_ASPECT_4X3);
}

CASE_F(TEST_AV_SET_WINATTRIB11,
	"�л�������ʾģʽ��ѯ�ʲ���Ա����Ӧ��Ϊ������GetWinAttrib�����õ�ֵ�Ƿ�һ��")
	{
	
	CHDRV_VIDEO_WindowAttrib_t stru_attr;

	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE,&stru_attr);
	
	stru_attr.enm_AspectMode = CHDRV_VIDEO_WINDOW_PANSCAN;
	CHDRV_VIDEO_SetWinAttrib(D_VIDEO_HANDLE,&stru_attr);

	memset(&stru_attr, 0, sizeof(stru_attr));
	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE,&stru_attr);
	EXPECT_EQ(stru_attr.enm_AspectMode,CHDRV_VIDEO_WINDOW_PANSCAN);
	PROMPT("�����Ƿ���ʾΪ����?���밴Y�������밴N:");
	EXPECT_EQ(test_input_yes(), CH_TRUE);

	stru_attr.enm_AspectMode = CHDRV_VIDEO_WINDOW_LETTERBOX;
	CHDRV_VIDEO_SetWinAttrib(D_VIDEO_HANDLE, &stru_attr);
	memset(&stru_attr, 0, sizeof(stru_attr));
	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE, &stru_attr);
	EXPECT_EQ(stru_attr.enm_AspectMode, stru_attr.enm_AspectMode);
	PROMPT("�����Ƿ���ʾΪ����?���밴Y�������밴N:");
	EXPECT_EQ(test_input_yes(), CH_TRUE);

	stru_attr.enm_AspectMode = CHDRV_VIDEO_WINDOW_COMBINE;
	CHDRV_VIDEO_SetWinAttrib(D_VIDEO_HANDLE, &stru_attr);
	memset(&stru_attr, 0, sizeof(stru_attr));
	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE, &stru_attr);
	EXPECT_EQ(stru_attr.enm_AspectMode, CHDRV_VIDEO_WINDOW_COMBINE);
	PROMPT("�����Ƿ���ʾΪ����?���밴Y�������밴N:");
	EXPECT_EQ(test_input_yes(), CH_TRUE);
	
	stru_attr.enm_AspectMode = CHDRV_VIDEO_WINDOW_FULL;
	CHDRV_VIDEO_SetWinAttrib(D_VIDEO_HANDLE, &stru_attr);
	memset(&stru_attr, 0, sizeof(stru_attr));
	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE, &stru_attr);
	EXPECT_EQ(stru_attr.enm_AspectMode, CHDRV_VIDEO_WINDOW_FULL);
	PROMPT("�����Ƿ���ʾΪ����?���밴Y�������밴N:");
	EXPECT_EQ(test_input_yes(), CH_TRUE);
}
CASE_F(TEST_AV_SET_WINATTRIB12,
	"�л�������ʾ����Ϊ16��9��GetWinAttrib�����õ�ֵ�Ƿ�һ��")
	{

	CHDRV_VIDEO_WindowAttrib_t stru_attr;

	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE, &stru_attr);
	stru_attr.enm_DispAspect = CHDRV_VIDEO_ASPECT_16X9;

	CHDRV_VIDEO_SetWinAttrib(D_VIDEO_HANDLE, &stru_attr);
	memset(&stru_attr, 0, sizeof(stru_attr));
	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE, &stru_attr);

	EXPECT_EQ(stru_attr.enm_DispAspect, CHDRV_VIDEO_ASPECT_16X9);
}

CASE_F(TEST_AV_SET_WINATTRIB13,
	"�л�������ʾģʽΪPANSCAN���������±��ü���GetWinAttrib�����õ�ֵ�Ƿ�һ��")
	{
	
	CHDRV_VIDEO_WindowAttrib_t stru_attr;

	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE, &stru_attr);
	stru_attr.enm_AspectMode = CHDRV_VIDEO_WINDOW_PANSCAN;
	CHDRV_VIDEO_SetWinAttrib(D_VIDEO_HANDLE, &stru_attr);
	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE, &stru_attr);
	
	EXPECT_EQ(stru_attr.enm_AspectMode, stru_attr.enm_AspectMode);

	PROMPT("��ǰģʽΪPANSCAN���������±��ü�,��ʾ��ȷ���밴Y�������밴N:");
	EXPECT_EQ(test_input_yes(), CH_TRUE);

}

CASE_F(TEST_AV_SET_WINATTRIB14,
	"�л�������ʾģʽΪLEFTBOX�����ұ����ɺڱߣ�GetWinAttrib�����õ�ֵ�Ƿ�һ��")
	{
	
	CHDRV_VIDEO_WindowAttrib_t stru_attr;

	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE, &stru_attr);
	stru_attr.enm_AspectMode = CHDRV_VIDEO_WINDOW_LETTERBOX;
	CHDRV_VIDEO_SetWinAttrib(D_VIDEO_HANDLE, &stru_attr);
	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE, &stru_attr);
	
	EXPECT_EQ(stru_attr.enm_AspectMode, stru_attr.enm_AspectMode);

	PROMPT("��ǰģʽΪLEFTBOX���������ұ����ɺ�ɫ,��ʾ��ȷ���밴Y�������밴N:");
	EXPECT_EQ(test_input_yes(), CH_TRUE);

}

CASE_F(TEST_AV_SET_WINATTRIB15,
	"�л�������ʾģʽΪFULL�����ұ����죬GetWinAttrib�����õ�ֵ�Ƿ�һ��")
	{
	
	CHDRV_VIDEO_WindowAttrib_t stru_attr;

	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE, &stru_attr);
	stru_attr.enm_AspectMode = CHDRV_VIDEO_WINDOW_FULL;
	CHDRV_VIDEO_SetWinAttrib(D_VIDEO_HANDLE, &stru_attr);
	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE, &stru_attr);
	
	EXPECT_EQ(stru_attr.enm_AspectMode, stru_attr.enm_AspectMode);

	PROMPT("��ǰģʽΪFULL���������ұ�����,��ʾ��ȷ���밴Y�������밴N:");
	EXPECT_EQ(test_input_yes(), CH_TRUE);

}

CASE_F(TEST_AV_SET_WINATTRIB16,
	"�л�������ʾģʽΪCOMBO�����������������Ч��֮�䣬GetWinAttrib�����õ�ֵ�Ƿ�һ��")
	{
	
	CHDRV_VIDEO_WindowAttrib_t stru_attr;

	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE, &stru_attr);
	stru_attr.enm_AspectMode = CHDRV_VIDEO_WINDOW_COMBINE;
	CHDRV_VIDEO_SetWinAttrib(D_VIDEO_HANDLE, &stru_attr);
	CHDRV_VIDEO_GetWinAttrib(D_VIDEO_HANDLE, &stru_attr);
	
	EXPECT_EQ(stru_attr.enm_AspectMode, stru_attr.enm_AspectMode);

	PROMPT("��ǰģʽΪCOMBINE��������ʾЧ������LFFT��PANSCAN֮��,��ʾ��ȷ���밴Y�������밴N:");

}

CASE_F(TEST_AV_VIDEO_INFO3,
	"����GetVideoInfo��Ӧ����OK����õĿ�߱�ӦΪ4:3")
	{

	CHDRV_VIDEO_Info_t vidInfo;
	EXPECT_EQ(CHDRV_VIDEO_GetVideoInfo(D_VIDEO_HANDLE, &vidInfo),CHDRV_VIDEO_OK);
	EXPECT_EQ(vidInfo.enm_AspectRatio,CHDRV_VIDEO_ASPECT_4X3) << "��ȡ����Ƶ��߱ȴ���";
	EXPECT_EQ(vidInfo.enm_DecodeType,CH_VIDEO_CODE_MPEG2) << "��ȡ����Ƶ�������ʹ���";
	RUN_CMD("vid.ram_stop 0");

	
}

	

CASE_F(TEST_AV_FORMAT,
	"��Ƶ��Ƶ��ʽ֧�ֲ��ԣ����δ�env�ж�ȡ�������ƣ������������������г���֧�ֺ�δ���Ե��ĸ�ʽ")
	{
	int i, ret, j;
	char stream_env[32];
	int prognum = 0;
	int audio_test_ok[MAX_CODETYPE];
	int audio_test_fail[MAX_CODETYPE];
	int audio_test_not_smooth[MAX_CODETYPE];
	int video_test_ok[MAX_CODETYPE];
	int video_test_fail[MAX_CODETYPE];
	int video_test_not_smooth[MAX_CODETYPE];
	int i_not_test = 0, i_fail_test = 0, i_not_smooth = 0, i_ok_test = 0;
	std::string str_not_test = "", str_fail_test = "", str_not_smooth = "", str_ok = "";
	char *ts_env = NULL, *new_env = NULL;

	memset(&audio_test_ok[0], 0, sizeof(audio_test_ok));
	memset(&audio_test_fail[0], 0, sizeof(audio_test_fail));
	memset(&audio_test_not_smooth[0], 0, sizeof(audio_test_not_smooth));
	
	memset(&video_test_ok[0], 0, sizeof(video_test_ok));
	memset(&video_test_fail[0], 0, sizeof(video_test_fail));
	memset(&video_test_not_smooth[0], 0, sizeof(video_test_not_smooth));
	
	for(i = 1; ;i++)
	{
		sprintf(stream_env, "AV_CODETYPE_%d", i);
		new_env = fw_getenv((char*)stream_env);
		if(!new_env)
		{
			break;
		}
		
		if(cmp_StreamName(ts_env, new_env))
		{
			LOG("ts_env=%s, new_env=%s\n",ts_env, new_env);
			prognum = request_play_program(stream_env, "TUNER_PARAMS1");
			if(prognum < 0)
			{
				break;
			}
			RUN_CMD("tuner.lock %s", fw_getenv("TUNER_PARAMS1"));
		}
		EXPECT_GT(prognum, 0) << "δ�յ���Ŀ���ݣ���������Ϊ[" << new_env<< "]";
		if(prognum < 0)
		{
			continue;
		}
		ts_env = new_env;
		ret = RUN_CMD("av.start %d %d\n", get_demux_id("TUNER_PARAMS1"), prognum);
		EXPECT_EQ(ret, 0) << "����ʧ�ܣ���������Ϊ[" << new_env<< "]";
		if(ret != 0)
		{
			continue;
		}
		
		for(j = 0; j < gstru_current_prog.pstru_Audio.size(); j++)
		{
			if(D_GET_CODETYPE(gstru_current_prog.pstru_Audio, j) < CH_AUDIO_CODE_TYPE_MAX)
			{
				if(j > 0)
				{
					RUN_CMD("avfilter.set_pid 0 %d\n"
						"aud.restart 0 %d\n",
						gstru_current_prog.pstru_Audio[j].us_PrivateEsPid,
						D_GET_CODETYPE(gstru_current_prog.pstru_Audio, j));
				}
				
				PROMPT("��ǰ��Ƶ��ʽΪ[%s]������Ƿ�����?�����밴1���������밴2���������밴3:", audio_codetype_name(D_GET_CODETYPE(gstru_current_prog.pstru_Audio, j)));
				ret = test_input_i();
				if(ret == 3)
				{
					audio_test_fail[D_GET_CODETYPE(gstru_current_prog.pstru_Audio, j)] ++;
					EXPECT_TRUE(false) << "����Ա��Ϊ��Ƶ��ʽ[" 
						<< audio_codetype_name(D_GET_CODETYPE(gstru_current_prog.pstru_Audio, j))
						<< "]δ����ɹ�����������Ϊ:"
						<< fw_getenv(stream_env)
						<< endl;
				}
				else if(ret == 2)
				{
					audio_test_not_smooth[D_GET_CODETYPE(gstru_current_prog.pstru_Audio, j)] ++;
					EXPECT_TRUE(false) << "����Ա��Ϊ��Ƶ��ʽ[" 
						<< audio_codetype_name(D_GET_CODETYPE(gstru_current_prog.pstru_Audio, j))
						<< "]��������������������Ϊ:"
						<< fw_getenv(stream_env)
						<< endl;
				}
				else
				{
					audio_test_ok[D_GET_CODETYPE(gstru_current_prog.pstru_Audio, j)] ++;
				}
			}
			
		}			
		
		if(D_GET_CODETYPE(gstru_current_prog.pstru_Video, 0) < CH_VIDEO_CODE_MAX)
		{
			PROMPT("��ǰ��Ƶ��ʽΪ[%s]������Ƿ�����?�����밴1���������밴2��������밴3:", video_codetype_name(D_GET_CODETYPE(gstru_current_prog.pstru_Video, 0)));
			ret = test_input_i();
			if(ret == 3)
			{
				EXPECT_TRUE(false) << "����Ա��Ϊ��Ƶ��ʽ[" 
					<< video_codetype_name(D_GET_CODETYPE(gstru_current_prog.pstru_Video, 0))
					<< "]δ����ɹ�����������Ϊ:"
					<< fw_getenv(stream_env)
					<< endl;
				video_test_fail[D_GET_CODETYPE(gstru_current_prog.pstru_Video, 0)]++;
			}
			else if(ret == 2)
			{
				EXPECT_TRUE(false) << "����Ա��Ϊ��Ƶ��ʽ[" 
					<< video_codetype_name(D_GET_CODETYPE(gstru_current_prog.pstru_Video, 0))
					<< "]���Ų���������������Ϊ:"
					<< fw_getenv(stream_env)
					<< endl;
				video_test_not_smooth[D_GET_CODETYPE(gstru_current_prog.pstru_Video, 0)] ++;
			}
			else
			{
				video_test_ok[D_GET_CODETYPE(gstru_current_prog.pstru_Video, 0)] ++;
			}
		}
		CH_RTOS_TaskDelay(1000);
		RUN_CMD("av.stop 0\n");
	}


	//ͳ��δ���ԺͲ���ʧ�ܵ���Ƶ��ʽ
	i_not_test = 0;
	i_fail_test = 0;
	i_ok_test = 0;
	i_not_smooth = 0;
	str_fail_test = "";
	str_not_test = "";
	str_not_smooth = "";
	str_ok = "";
	for(i = 0; i < CH_AUDIO_CODE_TYPE_MAX; i++)
	{
		if(audio_test_ok[i] == 0 && audio_test_fail[i] == 0 && audio_test_not_smooth[i] == 0)
		{
			i_not_test ++;
			str_not_test += audio_codetype_name(i);
			str_not_test += i_not_test % 4 == 0 ? "\n" : ",";
		}
		if(audio_test_fail[i])
		{
			i_fail_test ++;
			str_fail_test += audio_codetype_name(i);
			str_fail_test += i_fail_test % 4 == 0 ? "\n" : ",";
		}
		if(audio_test_not_smooth[i])
		{
			i_not_smooth ++;
			str_not_smooth += audio_codetype_name(i);
			str_not_smooth += i_fail_test % 4 == 0 ? "\n" : ",";
		}
		if(audio_test_ok[i])
		{
			i_ok_test++;
			str_ok += audio_codetype_name(i);
			str_ok += i_ok_test % 4 == 0 ? "\n" : ",";
		}
	}
	EXPECT_EQ(i_fail_test, 0) << "����Ա��Ϊ����" << i_fail_test << "����Ƶ��ʽ����û�����:\n" << str_fail_test << endl;
	EXPECT_EQ(i_not_smooth, 0) << "����Ա��Ϊ����" << i_not_smooth << "����Ƶ��ʽ�������������:\n" << str_not_smooth << endl;
	EXPECT_EQ(i_not_test, 0) << "����" << i_not_test << "����Ƶ��ʽδ����:\n" << str_not_test << endl;
	EXPECT_EQ(i_ok_test, 0) << "����" << i_ok_test << "����Ƶ��ʽ��������:\n" << str_ok << endl;
	
	//ͳ��δ���ԺͲ���ʧ�ܵ���Ƶ��ʽ
	i_not_test = 0;
	i_fail_test = 0;
	i_not_smooth = 0;
	i_ok_test = 0;
	str_fail_test = "";
	str_not_test = "";
	str_not_smooth = "";
	str_ok = "";
	for(i = 0; i < CH_VIDEO_CODE_MAX; i++)
	{
		if(video_test_ok[i] == 0 && video_test_fail[i] == 0 && video_test_not_smooth[i] == 0)
		{
			i_not_test ++;
			str_not_test += video_codetype_name(i);
			str_not_test += i_not_test % 4 == 0 ? "\n" : ",";
		}
		if(video_test_fail[i])
		{
			i_fail_test ++;
			str_fail_test += video_codetype_name(i);
			str_fail_test += i_fail_test % 4 == 0 ? "\n" : ",";
		}
		if(video_test_not_smooth[i])
		{
			i_not_smooth ++;
			str_not_smooth += video_codetype_name(i);
			str_not_smooth += i_not_smooth % 4 == 0 ? "\n" : ",";
		}
		if(video_test_ok[i])
		{
			i_ok_test++;
			str_ok += video_codetype_name(i);
			str_ok += i_ok_test % 4 == 0 ? "\n" : ",";
		}
	}

	EXPECT_EQ(i_fail_test, 0) << "����Ա��Ϊ����" << i_fail_test << "����Ƶ��ʽ����û�����:\n" << str_fail_test << endl;
	EXPECT_EQ(i_not_smooth, 0) << "����Ա��Ϊ����" << i_not_smooth << "����Ƶ��ʽ�������������:\n" << str_not_smooth << endl;
	EXPECT_EQ(i_not_test, 0) << "����" << i_not_test << "����Ƶ��ʽδ����:\n" << str_not_test << endl;
	EXPECT_EQ(i_ok_test, 0) << "����" << i_ok_test << "����Ƶ��ʽ��������:\n" << str_ok << endl;
}

CASE_F(TEST_AV_MUL_AUDIO,
	"����һ������Ƶ�����������л�����Ƶ�����治���٣���Ƶ�л��ޱ���")
	{
		int prognum = 0, i, j, ret = 0;
		char stream_env[32];
		char *ts_env = NULL, *new_env = NULL;
		
		for(i = 1; ;)
		{
			sprintf(stream_env, "AV_MULAUDIO_%d", i);
			new_env = fw_getenv((char*)stream_env);
			if(!new_env)
			{
				break;
			}
			if(cmp_StreamName(ts_env, new_env))
			{
				prognum = request_play_program(stream_env, "TUNER_PARAMS1");
				if(prognum < 0)
				{
					break;
				}
				RUN_CMD("tuner.lock %s", fw_getenv("TUNER_PARAMS1"));
			}
			ret = RUN_CMD("av.start %d %d\n", get_demux_id("TUNER_PARAMS1"), prognum);
			EXPECT_EQ(ret, 0) << "����ʧ�ܣ���������Ϊ[" << new_env<< "]";
			if(ret != 0)
			{
				continue;
			}
			
			ts_env = new_env;
			if(gstru_current_prog.pstru_Audio.size() < 2)
			{
				PROMPT("��ǰ��Ŀ��Ƶ��ĿΪ%d,�޷����Զ���Ƶ���Ƿ����������?�����밴Y���ٴβ��ŵ�ǰ����N:", gstru_current_prog.pstru_Audio.size());
				if(test_input_yes())
				{
					i++;
				}
				else
				{
					ts_env = NULL;
				}
				continue;
			}
			RUN_CMD("tuner.lock %s", fw_getenv("TUNER_PARAMS1"));
			RUN_CMD("av.start %d %d\n", get_demux_id("TUNER_PARAMS1"), prognum);
		
		
			for(j = 1; j < 10; j++)
			{
				int index;
				CH_AUDIO_CODE_TYPE_e aud_type;
				
				index = j % gstru_current_prog.pstru_Audio.size();
				aud_type = (CH_AUDIO_CODE_TYPE_e)get_codec_type(gstru_current_prog.pstru_Audio[index], NULL);
				if(aud_type >= CH_AUDIO_CODE_TYPE_MAX)
				{
					continue;
				}
				RUN_CMD("avfilter.set_pid 0 %d\n"
					"aud.restart 0 %d\n",
					gstru_current_prog.pstru_Audio[index].us_PrivateEsPid,
					aud_type);
				PROMPT("�л�����Ƶ[%d %s]ʱ����ƵӦ�޶������������������Ƶ��������ޱ������л�����ƽ��?���밴Y�����밴N:",
					index,
					audio_codetype_name(aud_type));
				EXPECT_EQ(test_input_yes(), CH_TRUE) 
					<< "����" 
					<< fw_getenv(stream_env) 
					<< "����" 
					<< index
					<< "·��"
					<< "��Ƶ��ʽΪ:"
					<< audio_codetype_name(aud_type)
					<< " �л�Ч������";
			}
			i ++;
			RUN_CMD("av.stop %d\n", get_demux_id("TUNER_PARAMS1"));
		}

		RUN_CMD("av.stop %d\n", get_demux_id("TUNER_PARAMS1"));
}

CASE_F(TEST_AV_SYNC,
	"����һ��ͬ�����������������������Ƶ�Ƿ�����")
	{

	int prognum = 0, i, ret= 0;
	char stream_env[32];
	char *ts_env = NULL, *new_env = NULL;
	
	for(i = 1; ;i++)
	{
		sprintf(stream_env, "AV_SYNC_%d", i);
		new_env = fw_getenv((char*)stream_env);
		if(!new_env)
		{
			break;
		}
		if(cmp_StreamName(ts_env, new_env))
		{
			prognum = request_play_program(stream_env, "TUNER_PARAMS1");
			if(prognum < 0)
			{
				break;
			}
			RUN_CMD("tuner.lock %s", fw_getenv("TUNER_PARAMS1"));
		}
		
		ts_env = new_env;
		ret = RUN_CMD("av.start %d %d\n", get_demux_id("TUNER_PARAMS1"), prognum);
		EXPECT_EQ(ret, 0) << "����ʧ�ܣ���������Ϊ[" << new_env<< "]";
		if(ret != 0)
		{
			continue;
		}
		
		PROMPT("��ǰ��Ƶ��ʽΪ[%s]����Ƶ��ʽΪ[%s],����Ƿ�����?���밴Y�������밴N:", 
			audio_codetype_name(D_GET_CODETYPE(gstru_current_prog.pstru_Audio, 0)),
			video_codetype_name(D_GET_CODETYPE(gstru_current_prog.pstru_Video, 0)));	
		EXPECT_EQ(test_input_yes(), CH_TRUE) 
			<< "����" 
			<< fw_getenv(stream_env) 
			<< "��service_id=" 
			<< gstru_current_prog.us_ServiceID
			<< ",��Ƶ��ʽΪ:"
			<< audio_codetype_name(D_GET_CODETYPE(gstru_current_prog.pstru_Audio, 0))
			<< ",��Ƶ��ʽΪ:"
			<< video_codetype_name(D_GET_CODETYPE(gstru_current_prog.pstru_Video, 0))
			<< " ����Ч������";
		
	}
}
#endif /* ENABLE_TESTCASE */


/**
FuncName:av_section_call
@brief ������section���ݵ���֪ͨ
 
@param [in] renm_Event 
@param [in] rh_Channel 
@param [in] rpstru_Param 
@param [out] none
 
@return success  
@return fail  
   
@attention none
*/
static void av_section_call(CHDRV_SECTION_ChannelHandle rh_Channel,
				 CHDRV_SECTION_EVENT_e renm_Event,
				 CHDRV_SECTION_CallBackDataParam_t *rpstru_Param)
{
	if(gh_data_come)CH_RTOS_SignalSemaphore(gh_data_come);
}

/**
FuncName:av_search_section
@brief ��ָ������������section����
 
@param [in] data 
@param [in] data_len 
@param [in] device_id 
@param [in] mask 
@param [in] pid 
@param [in] section 
@param [in] sec_len 
@param [out] none
 
@return success  
@return fail  
   
@attention none
*/
int av_search_section(int device_id, U8 *data, U8 *mask, int data_len, int pid, U8 *section, int sec_len)
{
	int ret, got_len = 0;
	CHDRV_SECTION_ChannelHandle h_Channel = NULL;
	CHDRV_SECTION_FilterHandle h_Filter = NULL;
	CHDRV_SECTION_AllocParam_t stru_Param;

	if(!gh_data_come)
	{
		ret = CH_RTOS_CreateSemaphore(0, &gh_data_come);
		if(ret != CH_RTOS_OK)
		{
			LOG("CH_RTOS_CreateSemaphore return %d\n", ret);
			return 0;
		}
	}
	do
	{	
		ret = CHDRV_SECTION_AllocChannel(device_id, pid, av_section_call, &h_Channel, 16384);
		if(ret != CHDRV_SECTION_OK)
		{
			LOG("CHDRV_SECTION_AllocChannel return %d\n", ret);
			break;
		}
		memset(&stru_Param, 0, sizeof(stru_Param));
		stru_Param.enm_FilterSize = CHDRV_SECTION_FILTER_SIZE_8BYTE;
		stru_Param.enm_MatchMode = CHDRV_SECTION_MODE_MATCH;
		stru_Param.enm_SectionSize = CHDRV_SECTION_FILTER_SIZE_4K;
		stru_Param.h_Channel = h_Channel;
		ret = CHDRV_SECTION_AllocFilter(device_id, &stru_Param, &h_Filter);
		if(ret != CHDRV_SECTION_OK)
		{
			LOG("CHDRV_SECTION_AllocChannel return %d\n", ret);
			break;
		}
		ret = CHDRV_SECTION_SetFilter(h_Filter, data, mask, data, data_len, CH_FALSE);
		if(ret != CHDRV_SECTION_OK)
		{
			LOG("CHDRV_SECTION_SetFilter return %d\n", ret);
			break;
		}
		ret = CHDRV_SECTION_StartFilter(h_Filter);
		if(ret != CHDRV_SECTION_OK)
		{
			LOG("CHDRV_SECTION_StartFilter return %d\n", ret);
			break;
		}
		ret = CHDRV_SECTION_StartChannel(h_Channel);
		if(ret != CHDRV_SECTION_OK)
		{
			LOG("CHDRV_SECTION_StartFilter return %d\n", ret);
			break;
		}
		ret = CH_RTOS_WaitSemaphore(gh_data_come, 8000);
		if(ret != CH_RTOS_OK)
		{
			LOG("can not get section, data=[%02x %02x %02x %02x], mask=[%02x %02x %02x %02x], pid=%d(0x%x)\n",
				data[0], data[1], data[2], data[3], 
				mask[0], mask[1], mask[2], mask[3], 
				pid, pid);
			break;
		}

		if(ret == CH_RTOS_OK)
		{
			LOG(" get section, data=[%02x %02x %02x %02x], mask=[%02x %02x %02x %02x], pid=%d(0x%x)\n",
				data[0], data[1], data[2], data[3], 
				mask[0], mask[1], mask[2], mask[3], 
				pid, pid);
		}
		got_len = sec_len;
		ret = CHDRV_SECTION_ReadFilterData(h_Channel, (U8*)section, (S32 *)&got_len);
		if(ret != CHDRV_SECTION_OK)
		{
			LOG("CHDRV_SECTION_ReadFilterData return %d\n", ret);
			break;
		}
		
		LOG("get section data=%#x, len=%d\n", section[0], got_len);
	}while(0);
	ret = CHDRV_SECTION_StopFilter(h_Filter);
	if(ret != CHDRV_SECTION_OK)
	{
		LOG("CHDRV_SECTION_StopFilter return %d\n", ret);
	}
	ret = CHDRV_SECTION_StopChannel(h_Channel);
	if(ret != CHDRV_SECTION_OK)
	{
		LOG("CHDRV_SECTION_StopChannel return %d\n", ret);
	}
	ret = CHDRV_SECTION_FreeFilter(h_Filter);
	if(ret != CHDRV_SECTION_OK)
	{
		LOG("CHDRV_SECTION_FreeFilter return %d\n", ret);
	}
	ret = CHDRV_SECTION_FreeChannel(h_Channel);
	if(ret != CHDRV_SECTION_OK)
	{
		LOG("CHDRV_SECTION_FreeChannel return %d\n", ret);
	}
	ret = CH_RTOS_DeleteSemaphore(gh_data_come);
	if(ret != CH_RTOS_OK)
	{
		LOG("CH_RTOS_DeleteSemaphore return %d\n", ret);
	}
	gh_data_come = NULL;
	return got_len;
}


/**
FuncName:get_services
@brief ��ȡ������pat
 
@param [in] argc 
@param [in] argv 
@param [in] prog_list 
@param [out] none
 
@return success  
@return fail  
   
@attention none
*/
static int get_services(int argc, CMD_PARAM argv[], vector <Channel_Info_t> &prog_list)
{
	U8 data[8], mask[8];
	int ret, i;
	U8 *puc_section = NULL;
	PsiDescript Des;
	Channel_Info_t channel_info;
	
	//����pat
	memset(&data[0], 0, sizeof(data));
	memset(&mask[0], 0, sizeof(mask));
	data[0] = 0;
	mask[0] = 0xff;

	puc_section = new U8[4096];

	ret = av_search_section(argv[1].get_i(), data, mask, 1, 0, puc_section, 4096);
	if(ret <= 0)
	{
		LOG("ERROR! get pat return %d\n", ret);
		delete[] puc_section;
		return -1;
	}


	//����pat
	ret = ParseSection(puc_section, ret, &Des);
	if(ret < 0
		|| Des.type != E_CH_PSI_PAT
		|| Des.psi.pat == NULL
		|| Des.psi.pat->des_nums == 0
		|| Des.psi.pat->pDes == NULL)
	{
		LOG("ERROR! parse pat return %d\n", ret);
		delete[] puc_section;
		return -2;
	}
	//�洢
	for(i = 0; i < Des.psi.pat->des_nums; i++)
	{
		channel_info.us_PmtPid = Des.psi.pat->pDes[i].program_map_PID;
		channel_info.us_ServiceID = Des.psi.pat->pDes[i].program_number;
		channel_info.auc_ChannelName[0] = 0;
		channel_info.auc_ProviderName[0] = 0;
		
		channel_info.us_ChannelIndex = i;

		if(channel_info.us_ServiceID != 0)
		{
			prog_list.push_back(channel_info);
		}
	}
	delete[] puc_section;
	return 0;

}

/**
FuncName:get_pids
@brief ��ȡ������pmt
 
@param [in] argc 
@param [in] argv 
@param [in] prog_list 
@param [out] none
 
@return success  
@return fail  
   
@attention none
*/
static int get_pids(int argc, CMD_PARAM argv[], vector <Channel_Info_t> &prog_list)
{
	U8 data[8], mask[8];
	int ret, i, j;
	U8 *puc_section = NULL;
	PsiDescript Des;
	Channel_Info_t channel_info;


	puc_section = new U8[4096];
	//����pmt
	for(i = 0; i < prog_list.size(); i++)
	{
		StreamInfo_t stream_info;
		
		memset(&data[0], 0, sizeof(data));
		memset(&mask[0], 0, sizeof(mask));
		data[0] = 0x02;
		mask[0] = 0xff;

		data[1] = (prog_list[i].us_ServiceID >> 8) & 0xff;
		data[2] = prog_list[i].us_ServiceID & 0xff;
		mask[1] = 0xff;
		mask[2] = 0xff;

		ret = av_search_section(argv[1].get_i(), data, mask, 8, prog_list[i].us_PmtPid, puc_section, 4096);
		if(ret <= 0)
		{
			LOG("ERROR! get pmt %d(0x%x) return %d\n", prog_list[i].us_PmtPid, prog_list[i].us_PmtPid, ret);
			delete[] puc_section;
			return ret;
		}
	
		ret = ParseSection(puc_section, ret, &Des);
		if(ret < 0
			|| Des.type != E_CH_PSI_PMT)
		{
			LOG("ERROR! parse pmt return %d,type=%d,data[0]=%#x\n", ret, Des.type, puc_section[0]);
			delete[] puc_section;
			return ret;
		}

		if(Des.psi.pmt == NULL
			|| Des.psi.pmt->stream_nums == 0
			|| Des.psi.pmt->pStream == NULL)
		{
			LOG("ERROR! stream_nums=%d,pStream=%#x\n",Des.psi.pmt? Des.psi.pmt->stream_nums : 0, 
				Des.psi.pmt ? Des.psi.pmt->pStream : 0);
			delete[] puc_section;
			return ret;
		}

		prog_list[i].us_PcrPid = Des.psi.pmt->header.PCR_PID;

		for(int k = 0; k < Des.psi.pmt->des_nums; k++)
		{
			if(!Des.psi.pmt->pDes)
			{
				continue;
			}


			switch(Des.psi.pmt->pDes[k].header.tag)
			{
				case 0x09:
					CaEcmPid_t ecm_pid;
					ecm_pid.us_CaSystemID = (Des.psi.pmt->pDes[k].data[0] << 8) + Des.psi.pmt->pDes[k].data[1];
					ecm_pid.us_EcmPid = (Des.psi.pmt->pDes[k].data[2] << 8) + Des.psi.pmt->pDes[k].data[3];
					prog_list[i].pus_EcmPid.push_back(ecm_pid);
					break;

				
				default:
					break;
				
			}
				
		}

		for(j = 0; j < Des.psi.pmt->stream_nums; j++)
		{
			memset(&stream_info, 0, sizeof(stream_info));
			stream_info.uc_PrivateEsStreamType = Des.psi.pmt->pStream[j].header.stream_type;
			stream_info.us_PrivateEsPid = Des.psi.pmt->pStream[j].header.elementary_PID;

			for(int k = 0; k < Des.psi.pmt->pStream[j].des_nums; k++)
			{
				if(!Des.psi.pmt->pStream[j].pDes)
				{
					continue;
				}
			

				switch(Des.psi.pmt->pStream[j].pDes[k].header.tag)
				{
					case 0x09:
						CaEcmPid_t ecm_pid;
						ecm_pid.us_CaSystemID = (Des.psi.pmt->pStream[j].pDes[k].data[0] << 8) + Des.psi.pmt->pStream[j].pDes[k].data[1];
						ecm_pid.us_EcmPid = (Des.psi.pmt->pStream[j].pDes[k].data[2] << 8) + Des.psi.pmt->pStream[j].pDes[k].data[3];
						stream_info.pus_EcmPid.push_back(ecm_pid);
						break;

					case 0x0a:
						U32 language;
						language = (Des.psi.pmt->pStream[j].pDes[k].data[0] << 24) 
							+ (Des.psi.pmt->pStream[j].pDes[k].data[1] << 16)
							+ (Des.psi.pmt->pStream[j].pDes[k].data[2] << 8)
							+ Des.psi.pmt->pStream[j].pDes[k].data[3];
						stream_info.pui_language.push_back(language);
						break;
					case 0x6a:
					case 0x73:
					case 0x7a:
					case 0x7b:
					case 0x7c:
					case 0x46:
					case 0x56:
					case 0x59:
						stream_info.uc_Reserved = Des.psi.pmt->pStream[j].pDes[k].header.tag;
						break;
						
					case 0x05:
						if(Des.psi.pmt->pStream[j].pDes[k].header.len == 4
							&& memcmp(Des.psi.pmt->pStream[j].pDes[k].data, "HEVC", 4) == 0)
						{
							stream_info.uc_PrivateEsStreamType = 0x24;
						}
						else if(Des.psi.pmt->pStream[j].pDes[k].header.len == 4
							&& memcmp(Des.psi.pmt->pStream[j].pDes[k].data, "DRA1", 4) == 0)
						{
							stream_info.uc_PrivateEsStreamType = 0x88;
						}
						else if(Des.psi.pmt->pStream[j].pDes[k].header.len == 4
							&& memcmp(Des.psi.pmt->pStream[j].pDes[k].data, "AC-3", 4) == 0)
						{
							stream_info.uc_PrivateEsStreamType = 0x81;
						}
						else if(Des.psi.pmt->pStream[j].pDes[k].header.len == 4
							&& memcmp(Des.psi.pmt->pStream[j].pDes[k].data, "AVSV", 4) == 0)
						{
							stream_info.uc_PrivateEsStreamType = 0x42;
						}
						break;
					default:
						break;
					
				}
					
			}

			prog_list[i].pstru_Stream.push_back(stream_info);
			switch(stream_info.uc_PrivateEsStreamType)
			{
				case 0x01 :	/* ISO11172-2 Video ? */
				case 0x02 :	/* ISO13818-2 Video or ISO11172-2 Constrained Parameter Video ? */
				case 0x10:	/* MPEG-4 = Ge Wanlin 2009-8-31 add */
				case 0x1b:	/* H.264 = Ge Wanlin 2009-8-31 add */
				case 0x42:	/* chinese AVS */
				case 0xEA:	/* VC1 */	
				case 0x24:	/* H265 HEVC */	
					prog_list[i].pstru_Video.push_back(stream_info);
					break;

				case 0x03 :	/* ISO11172-3 Audio ? */                                                                                                      
				case 0x04 :	/* ISO13818-3 Audio ? */        
				case 0x0f :	/* ISO/IEC 13818-7 Audio with ADTS transport syntax(AACMpeg2/HEAACMpeg2) = Ge Wanlin 2009-8-31 add */
				case 0x11 :	/* ISO/IEC 14496-3 Audio with the LATM transport syntax as defined in ISO/IEC 14496-3/Amd.1(AACMpeg4/HEAACMpeg4)  = Ge Wanlin 2009-8-31 add */
				case 0x1c :	/* ISO/IEC 14496-3 Audio, without using any additional transport syntax, such as DST, ALS and SLS  = Ge Wanlin 2009-8-31 add */
				case 0x81 :	/* AC3 */
				case 0x87 :	/* EAC3 */
				case 0x82:	/* DTS*/
				case 0x83 : /*Dolby TrueHD*/
				case 0x84 : /*AC3-Plus*/
				case 0x85 : /*DTS_HD*/
				case 0x86 : /*DTS-MA*/
				case 0xa1 : /*AC3-Plus_SEC*/
				case 0xa2 : /*DTS_HD_SEC*/
				case 0x88: /*DRA*/
					prog_list[i].pstru_Audio.push_back(stream_info);
					break;

				case 0x06:
					for(int k = 0; k < Des.psi.pmt->pStream[j].des_nums; k++)
					{
						if(!Des.psi.pmt->pStream[j].pDes)
						{
							continue;
						}
						if(Des.psi.pmt->pStream[j].pDes[k].header.tag == 0x6a)
						{
							stream_info.uc_PrivateEsStreamType = 0x81;/* AC3 Descriptor  */
					prog_list[i].pstru_Audio.push_back(stream_info);
							break;
						}
						else if(Des.psi.pmt->pStream[j].pDes[k].header.tag == 0x7a)
						{
							stream_info.uc_PrivateEsStreamType = 0x87;/* EAC3  Descriptor  */
							prog_list[i].pstru_Audio.push_back(stream_info);
							break;
						}
						else if(Des.psi.pmt->pStream[j].pDes[k].header.tag == 0x7b)
						{
							stream_info.uc_PrivateEsStreamType = 0x82;/* DTS Descriptor  */
							prog_list[i].pstru_Audio.push_back(stream_info);
							break;
						}
						else if(Des.psi.pmt->pStream[j].pDes[k].header.tag == 0x7c)
						{
							stream_info.uc_PrivateEsStreamType = 0x0f;/* AAC Descriptor  */
							prog_list[i].pstru_Audio.push_back(stream_info);
							break;
						}
						}
					
					break;
				default:
					break;
			}
			
		}

		
			
		delete Des.psi.pmt;
		Des.psi.pmt = NULL;
		Des.type = E_CH_PSI_OTHER;
	}

	delete[] puc_section;
	return 0;
}


/**
FuncName:get_descripts
@brief ���ղ�����sdt
 
@param [in] argc 
@param [in] argv 
@param [in] prog_list 
@param [out] none
 
@return success  
@return fail  
   
@attention none
*/
static int get_descripts(int argc, CMD_PARAM argv[], vector <Channel_Info_t> &prog_list)
{
	U8 data[8], mask[8];
	int ret, i, j;
	U8 *puc_section = NULL;
	PsiDescript Des;
	Channel_Info_t channel_info;
	//����SDT
	memset(&data[0], 0, sizeof(data));
	memset(&mask[0], 0, sizeof(mask));

	puc_section = new U8[4096];

	data[0] = 0x42;
	mask[0] = 0xff;

	ret = av_search_section(argv[1].get_i(), data, mask, 1, 0x11, puc_section, 4096);
	if(ret <= 0)
	{
		LOG("ERROR! get sdt return %d\n", ret);
		delete[] puc_section;
		return ret;
	}


	//����sdt
	ret = ParseSection(puc_section, ret, &Des);
	if(ret < 0
		|| Des.type != E_CH_PSI_SDT
		|| Des.psi.sdt == NULL
		|| Des.psi.sdt->service_nums == 0
		|| Des.psi.sdt->pService == NULL)
	{
		LOG("ERROR! parse sdt return %d\n", ret);
		delete[] puc_section;
		return ret;
	}

	for(i = 0; i < Des.psi.sdt->service_nums; i++)
	{
		for(j = 0; j < prog_list.size(); j++)
		{
			if(Des.psi.sdt->pService[i].header.service_id == prog_list[j].us_ServiceID)
			{
				for(int k = 0; k < Des.psi.sdt->pService[i].des_nums; k++)
				{
					switch(Des.psi.sdt->pService[i].pDes[k].header.tag)
					{
						case 0x48:
							prog_list[j].uc_ChannelType = Des.psi.sdt->pService[i].pDes[k].data[0];
							memcpy(&prog_list[j].auc_ProviderName[0], 
								&Des.psi.sdt->pService[i].pDes[k].data[2],
								Des.psi.sdt->pService[i].pDes[k].data[1]);
							prog_list[j].auc_ProviderName[Des.psi.sdt->pService[i].pDes[k].data[1]] = 0;
							memcpy(&prog_list[j].auc_ChannelName[0], 
								&Des.psi.sdt->pService[i].pDes[k].data[Des.psi.sdt->pService[i].pDes[k].data[1] + 3],
								Des.psi.sdt->pService[i].pDes[k].data[Des.psi.sdt->pService[i].pDes[k].data[1] + 2]);
							prog_list[j].auc_ChannelName[Des.psi.sdt->pService[i].pDes[k].data[Des.psi.sdt->pService[i].pDes[k].data[1] + 2]] = 0;
							break;

						default:
							break;
					}
				}
				break;
			}
		}
	}

	delete[] puc_section;
	return 0;
}

/**
FuncName:print_progs
@brief ��ӡ�ѵ��Ľ�Ŀ��Ϣ
 
@param [in] prog_list 
@param [out] none
 
@return success  
@return fail  
   
@attention none
*/
static int print_progs(const vector <Channel_Info_t> prog_list)
{
	int i, j;
	for(i = 0; i < prog_list.size(); i++)
	{
		LOG("%d. service_name=%s, provider name=%s, prognum=%#x, channel type=%#x, pmt pid=%#x, apid=%#x, atype=%#x, vpid=%#x, vtype=%#x,pcr pid=%#x\n",
			i,
			prog_list[i].auc_ChannelName,
			prog_list[i].auc_ProviderName,
			prog_list[i].us_ServiceID,
			prog_list[i].uc_ChannelType,
			prog_list[i].us_PmtPid,
			prog_list[i].pstru_Audio.size() > 0 ? prog_list[i].pstru_Audio[0].us_PrivateEsPid : 0x1fff,
			prog_list[i].pstru_Audio.size() > 0 ? prog_list[i].pstru_Audio[0].uc_PrivateEsStreamType  : 0xff,
			prog_list[i].pstru_Video.size() > 0 ? prog_list[i].pstru_Video[0].us_PrivateEsPid  : 0x1fff,
			prog_list[i].pstru_Video.size() > 0 ? prog_list[i].pstru_Video[0].uc_PrivateEsStreamType  : 0xff,
			prog_list[i].us_PcrPid);

		for(j = 0; j < prog_list[i].pstru_Stream.size();j++)
		{
			printf("\t stream %d. pid=%#x, stream_type=%#x,lang num=%d, ca num=%d\n",
				j,
				prog_list[i].pstru_Stream[j].us_PrivateEsPid,
				prog_list[i].pstru_Stream[j].uc_PrivateEsStreamType,
				prog_list[i].pstru_Stream[j].pui_language.size(),
				prog_list[i].pstru_Stream[j].pus_EcmPid.size() ?: prog_list[i].pus_EcmPid.size());
			if(prog_list[i].pstru_Stream[j].uc_PrivateEsStreamType == 0x06)
			{
				printf(",tag=%#x\n", prog_list[i].pstru_Stream[j].uc_Reserved);
			}
			else
			{
				printf("\n");
			}
			for(int k = 0; k < prog_list[i].pstru_Stream[j].pui_language.size();k++)
			{
				printf("\t\t lang %d. %c%c%c\n",
					k,
					prog_list[i].pstru_Stream[j].pui_language[k] >> 24 & 0xff,
					prog_list[i].pstru_Stream[j].pui_language[k] >> 16 & 0xff,
					prog_list[i].pstru_Stream[j].pui_language[k] >> 8 & 0xff,
					prog_list[i].pstru_Stream[j].pui_language[k] >> 0 & 0xff);
			}
			if(prog_list[i].pstru_Stream[j].pus_EcmPid.size())
			{
				for(int k = 0; k < prog_list[i].pstru_Stream[j].pus_EcmPid.size();k++)
				{
					printf("\t\t ecm_pid %d. ca system id=%#x, ecm pid=%#x\n",
						k,
						prog_list[i].pstru_Stream[j].pus_EcmPid[k].us_CaSystemID,
						prog_list[i].pstru_Stream[j].pus_EcmPid[k].us_EcmPid);
				}
			}
			else
			{
				for(int k = 0; k < prog_list[i].pus_EcmPid.size();k++)
				{
					printf("\t\t ecm_pid %d. ca system id=%#x, ecm pid=%#x\n",
						k,
						prog_list[i].pus_EcmPid[k].us_CaSystemID,
						prog_list[i].pus_EcmPid[k].us_EcmPid);
				}
			}
		}
	}

	return 0;
}



/**
FuncName:prog_search
@brief ��Ƶ��������Ŀ
 
@param [in] device_id 
@param [in] prog_list 
@param [out] prog_list ������ѵ��Ľ�Ŀ����
 
@return success  
@return fail  
   
@attention none
*/
int prog_search(int device_id, vector <Channel_Info_t> &prog_list)
{
	int ret, argc = 1;
	CMD_PARAM argv[2];
	char str[8];


	sprintf(str, "%d", device_id);
	argv[0] = &str[0];
	
	ret = get_services(argc, argv,prog_list);
	if(ret)
	{
		return -1;
	}
	ret = get_pids(argc, argv,prog_list);
	if(ret)
	{
		return -2;
	}
	ret = get_descripts(argc, argv,prog_list);
	if(ret)
	{
		return -3;
	}

	ret = print_progs(prog_list);
	if(ret)
	{
		return -4;
	}
	return prog_list.size();
}


static int av_start(int argc, CMD_PARAM argv[])
{
	int ret, i, service_no;
	vector <Channel_Info_t> prog_list;


	do
	{
		ret = get_services(argc, argv,prog_list);
		if(ret)
		{
			PROMPT("δ�ѵ���Ŀ���ݣ���׼���û�����س�����,�˳��밴q:");
			if(test_input_quit())
			{
				return -1;
			}
		}
	}while(ret);
	
	ret = get_pids(argc, argv,prog_list);
	if(ret)
	{
		return ret;
	}
	ret = get_descripts(argc, argv,prog_list);
	if(ret)
	{
		return ret;
	}

	ret = print_progs(prog_list);
	if(ret)
	{
		return ret;
	}

	service_no = argv[2].get_i();
	
	while(1)
	{

		for(i = 0; i < prog_list.size(); i++)
		{
			if(prog_list[i].us_ServiceID == service_no)
			{
				break;
			}
		}
			
		if(i >= prog_list.size())
		{
			LOG("can not find program number %d(0x%x), please input:", service_no, service_no);
			char *p = test_get_input();
			if(p && (p[0] == 'q'
				|| p[0] == 'Q'))
			{
				return 0;
			}
			service_no = test_input_i(p);
		}
		else
		{
			break;
		}
	}

	RUN_CMD("route.create 0 0\n"
	"aout.open 0\n"
	"vout.open 0 1 1 1 0 0 0 0\n"
	"vout.set_port 1 0 0 0 0 0 0 1 0\n"
	"avfilter.start 0 0 %d\n"
	"avfilter.start 0 1 %d\n"
	"avfilter.start 0 2 %d\n"
	"vid.start 0 %d\n"
	"aud.start 0 %d\n"
	"vwin.open 0\n"
	"vwin.show 0 1\n",
	prog_list[i].pstru_Audio.size() > 0 ? prog_list[i].pstru_Audio[0].us_PrivateEsPid : 0x1fff,
	prog_list[i].pstru_Video.size() > 0 ? prog_list[i].pstru_Video[0].us_PrivateEsPid : 0x1fff,
	prog_list[i].us_PcrPid,
	D_GET_CODETYPE(prog_list[i].pstru_Video, 0),
	D_GET_CODETYPE(prog_list[i].pstru_Audio, 0));

	gstru_current_prog.pstru_Stream.clear();
	gstru_current_prog.pstru_Audio.clear();
	gstru_current_prog.pstru_Video.clear();
	gstru_current_prog.pus_EcmPid.clear();
	gstru_current_prog = prog_list[i];
	gstru_current_prog.us_AudioIndex = 0;
	return 0;
}


static int av_stop(int argc, CMD_PARAM argv[])
{
	RUN_CMD(
		//"vout.close 0\n"
		"avfilter.stop 2\n"
		"avfilter.stop 1\n"
		"avfilter.stop 0\n"
		"vid.stop 0\n"
		"aud.stop 0\n"
		//"vwin.close 0\n"
		//"route.delete 0 0\n"
		);
	return 0;
}

void vid_DecodeCallback(CHDRV_VIDEO_Handle rh_Handle ,
					CHDRV_VIDEO_EVENTTYPE_e renm_EventType,
					void * rp_ExtendData)
{

}

static int vid_start(int argc, CMD_PARAM argv[])
{
	int ret;
	CHDRV_VIDEO_Handle handle = NULL;
	CHDRV_VIDEO_StartDecode_t stru_Attrib;

	if(video_GetHandleNums() <= argv[1].get_i())
	{
		ret = CHDRV_VIDEO_Open(argv[1].get_i(), &handle);
		if(ret != CHDRV_VIDEO_OK)
		{
			LOG("ERROR: CHDRV_VIDEO_Open return %d\n", ret);
			return ret;
		}
		#ifdef CHSSA_CHIP_7583
		chdrv_platform_connect_demux_vdecoder(0,0,handle);
		#endif
	}
	else
	{
		handle = video_GetHandle(argv[1].get_i());
	}
	memset(&stru_Attrib, 0, sizeof(stru_Attrib));
	stru_Attrib.enm_DecodeType =(CH_VIDEO_CODE_TYPE_e) argv[2].get_i();
	stru_Attrib.enm_StartMode = CHDRV_VIDEO_SYNC;
	stru_Attrib.enm_StreamFmt = CHDRV_VIDEO_STREAM_PES;
	ret = CHDRV_VIDEO_StartDecode(handle, &stru_Attrib);
	if(ret != CHDRV_VIDEO_OK)
	{
		LOG("ERROR: CHDRV_VIDEO_StartDecode return %d\n", ret);
		return ret;
	}
	LOG("vid start ok, handle no=%d\n", video_GetHandleNums() - 1);
	
	return 0;
}

static int vid_stop(int argc, CMD_PARAM argv[])
{
	int ret;

	if(argv[1].get_i() >= video_GetHandleNums())
	{
		LOG("ERROR:  the handle no = %d is not exist!\n", argv[1].get_i());
		return -1;
	}
	ret = CHDRV_VIDEO_StopDecode(video_GetHandle(argv[1].get_i()));
	if(ret != CHDRV_VIDEO_OK)
	{
		LOG("ERROR: CHDRV_VIDEO_StopDecode return %d\n", ret);
	}
#if 0 	/*sqzou20170413*/
	ret = CHDRV_VIDEO_Close(video_GetHandle(argv[1].get_i()));
	if(ret != CHDRV_VIDEO_OK)
	{
		LOG("ERROR: CHDRV_VIDEO_Close return %d\n", ret);
	}
#endif
	
	LOG("vid_stop %d ok\n", argv[1].get_i());
	return 0;
}

static int vid_ram_start(int argc, CMD_PARAM argv[])
{
	int ret;
	CHDRV_VIDEO_Handle handle;
	char *pbuf = NULL;
	int len;

	handle = video_GetHandle(argv[1].get_i());

	if(handle == NULL)
	{
		ret = CHDRV_VIDEO_Open(argv[1].get_i(), &handle);
		if(ret != CHDRV_VIDEO_OK)
		{
			LOG("ERROR: CHDRV_VIDEO_Open return %d\n", ret);
			return ret;
		}
	}
	len = fsys_read_file(argv[2].get_str(), pbuf);
	if(len <= 0)
	{
		LOG("ERROR: open %s fail\n", (const S8 *)argv[2].get_str());
		return -1;
	}
	ret = CHDRV_VIDEO_InstallCallback(handle, vid_DecodeCallback);
	if(ret != CHDRV_VIDEO_OK)
	{
		LOG("ERROR: CHDRV_VIDEO_InstallCallback return %d\n", ret);
	}
	ret = CHDRV_VIDEO_StartRamDecode(
	    handle ,
	    pbuf,
	    len);
	CH_RTOS_FreeMemory(gh_APP_HEAP, pbuf);
	if(ret != CHDRV_VIDEO_OK)
	{
		LOG("ERROR: CHDRV_VIDEO_StartDecode return %d\n", ret);
		return ret;
	}
	LOG("vid ram start ok, handle no=%d\n", video_GetHandleNums());
	return 0;
}

static int vid_ram_stop(int argc, CMD_PARAM argv[])
{
	int ret;

	if(argv[1].get_i() >= video_GetHandleNums())
	{
		LOG("ERROR:  the handle no = %d is not exist!\n", argv[1].get_i());
		return -1;
	}
	ret = CHDRV_VIDEO_StopRamDecode(video_GetHandle(argv[1].get_i()));
	if(ret != CHDRV_VIDEO_OK)
	{
		LOG("ERROR: CHDRV_VIDEO_StopRamDecode return %d\n", ret);
	}
	ret = CHDRV_VIDEO_UnInstallCallback(video_GetHandle(argv[1].get_i()), vid_DecodeCallback);
	if(ret != CHDRV_VIDEO_OK)
	{
		LOG("ERROR: CHDRV_VIDEO_UnInstallCallback return %d\n", ret);
	}
#if 0 	/*sqzou20170413*/
	ret = CHDRV_VIDEO_Close(video_GetHandle(argv[1].get_i()));
	if(ret != CHDRV_VIDEO_OK)
	{
		LOG("ERROR: CHDRV_VIDEO_Close return %d\n", ret);
	}
#endif
	
	LOG("vid_stop %d ok\n", argv[1].get_i());
	return 0;
}

CH_AUDIO_CODE_TYPE_e switch_multiaud(int *pi_aud_index, int *pi_aud_num)
{
	int index;

	
	if(gstru_current_prog.pstru_Audio.size() < 2)
	{
		LOG("error can not do it, pstru_Audio.size()=%d\n", gstru_current_prog.pstru_Audio.size());
		return CH_AUDIO_CODE_TYPE_MAX;
	}
	index = (gstru_current_prog.us_AudioIndex + 1) % gstru_current_prog.pstru_Audio.size();
	RUN_CMD("avfilter.set_pid 0 %d\n"
		"aud.restart 0 %d\n",
		gstru_current_prog.pstru_Audio[index].us_PrivateEsPid,
		D_GET_CODETYPE(gstru_current_prog.pstru_Audio, index));
	if(pi_aud_index)
	{
		*pi_aud_index = index;
	}
	if(pi_aud_num)
	{
		*pi_aud_num = gstru_current_prog.pstru_Audio.size();
	}
	gstru_current_prog.us_AudioIndex = index;
	return D_GET_CODETYPE(gstru_current_prog.pstru_Audio, index);
}

REGISTER_CMD(vid_start, "����һ����Ƶ��", "device_id|code_type",  "0 0");
REGISTER_CMD(vid_stop, "ֹͣ����", "handle_no",  "0");
REGISTER_CMD(vid_ram_start, "����һ��I֡�ļ�", "device_id|iframe_file",  "0 /home/720_576.m2v");
REGISTER_CMD(vid_ram_stop, "ֹͣI֡�ļ�����", "handle_no",  "0");
REGISTER_CMD(av_start, "�Զ�����������ָ����Ŀ�ŵĽ�Ŀ", "device_id|prognum",  "0 0x2d");
REGISTER_CMD(av_stop, "ֹͣav��Ŀ����", "handle_no",  "0");
REGISTER_CMD(vid_info, "���video�������Ϣ����ӡ������", "",  "");



/**
@file test_section.cpp
@def group section���Լ�

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
#define CASE_NAME TestSection


#include "test_menu.h"
#include "test_modules.h"
#include "ch_stbtype.h"
#include "psi_parse.h"

#define  SECTION_EVENT_MIN  					(0)
#define  SECTION_EVENT_MAX  					(CHDRV_SECTION_EVENT_MAX)
#define  FILTER_SIZE							(16)			//����data���鳤��
#define  DATA_BUFSIZE							(4096)			//��ȡ���ݵ����鳤��
#define  TRUE   								(1)
#define  FALSE 									(0)

//����������
#define  ERRO_DEMUX_ID  						(5)
#define  ERRO_CHANNEL_PID  						(-3)
#define  ERRO_CHANNEL_BUFFERSIZE     			(0)
#define  ERRO_FILTERLEN  						(20)
#define  ERRO_SECTION_INFO_TYPE  				(20)
#define  ERRO_B_NOTMATCHMODE 					(3)

//����Ϸ�����
#define  NORMAL_DEMUX_ID  						(0)
#define  NORMAL_CHANNEL_PID  					(0x01)
#define  NORMAL_CHANNEL_BUFFERSIZE  			(2048)
#define  NORMAL_FILTERLEN  						(8)
#define  NORMAL_SECTION_INFO_TYPE  				(2)
#define	 NORMAL_B_NOTMATCHMODE  			 	(1)
#define  NORMAL_DEMUX_INDEX  					(1)

#define  TEST_SSECTION_TASK_STACK_SIZE        	(4096*2)
#define  TEST_SSECTION_TASK_PRIORITY            (200)
#define  CH_RTOS_TIMEOUT_INFINITY				(0xffffffff)	


//����һ���ṹ�����õ���ȫ�ֱ���
typedef struct
{
	CHDRV_SECTION_ChannelHandle	h_Channel;
	CHDRV_SECTION_EVENT_e enm_Event;		//�������ݱ�־
	CHDRV_SECTION_FilterHandle h_Filter1;
	CHDRV_SECTION_FilterHandle h_Filter2;
	S32 i_CURRENT_DMXID ; 										//��ǰDMX�豸��
	S32 i_Section_Nums;											//���յ�section����
	CH_RTOS_SemaphoreHandle h_SectionData_Come;				//֪ͨ�������ݵ���
	CH_RTOS_MessageQHandle h_MesSectionCome;
	CH_RTOS_SemaphoreHandle h_TestSection;					//�����û���������������ֹͣ
	S32 i_PlayBackFinish;										//����������ɱ�־

}TEST_SECTION_GlobalVariable_t;
 static TEST_SECTION_GlobalVariable_t gstru_GlobalVariable ={0};

 
 void init_globalVariable(void)
  {
	 char *file_buf = NULL;
	 const char *sub_str = "TEST_SECTION_FILTER24";
	 char *p_str = NULL;
	 int ret = 0;

	 gstru_GlobalVariable.i_CURRENT_DMXID = 0;
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
			 LOG("not found!  dmxid=%d\n",gstru_GlobalVariable.i_CURRENT_DMXID);
			 sub_str = NULL;
			 return;
		 }

		p_str = strstr(p_str, "\n");
		if(p_str)
		{
			gstru_GlobalVariable.i_CURRENT_DMXID++;
			LOG("found!  dmxid=%d\n",gstru_GlobalVariable.i_CURRENT_DMXID);
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
 

 int get_demux_id(char *p_tuner_params)
{
	char key[256];
	CMD_PARAM device_id;
	
	char str_tuner_params[16];
	
	if(p_tuner_params == NULL)
	{
		sprintf(str_tuner_params, "TUNER_PARAMS%d", gui_TunerIndex + 1);
		p_tuner_params = &str_tuner_params[0];
	}
	
	parse_key(fw_getenv(p_tuner_params), "device_id", key, sizeof(key));

	device_id = (CMD_PARAM)key;

	int demux_id = device_id.get_i() * 2 + (device_id.get_i() > 0 ? 1 : 0);
	return demux_id;
}

void test_section(void)
{
#if ENABLE_TESTCASE
	S32 i_DemuxNums = 0;

	i_DemuxNums = fw_getenv_i("SECTION_TEST_DMX_NUMS");
	
	if(gstru_GlobalVariable.i_CURRENT_DMXID + 1 < i_DemuxNums)
	{
		gstru_GlobalVariable.i_CURRENT_DMXID++; 			//�豸�ż�1
	}
	else
	{
		gstru_GlobalVariable.i_CURRENT_DMXID = 0;
	}
	
	if(gstru_GlobalVariable.i_CURRENT_DMXID != 0)
	{
		PROMPT("DMX����Ϊ%d,���ڿ�ʼ���Ե�%d��DMX,��س������²���", i_DemuxNums, gstru_GlobalVariable.i_CURRENT_DMXID + 1);
		RUN_CMD("av.stop %d\n",get_demux_id());
		test_get_input();
		
		UnitTest *pCTest = testing::UnitTest::GetInstance();
		const TestCase* test_case = pCTest->current_test_case();
		test_case->ClearResult();//0-----------clear result.
		test_case->Run();
	}
#endif /* ENABLE_TESTCASE */
	
}
void CASE_NAME::SetUpTestCase() 
{
	//excute before each test suit
	//std::cout << "init SetUp rtos TestCase " << std::endl;
	memset(&gstru_GlobalVariable, 0, sizeof(gstru_GlobalVariable));
	RUN_CMD("route.create %d 0", get_demux_id());
	init_globalVariable();
}

void CASE_NAME::TearDownTestCase() 
{
	//excute after each test suit
	//std::cout << "deinit section rtos TestCase " << std::endl;
	test_section();
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


static void TEST_SECTION_AllocChannel_CallBack(CHDRV_SECTION_ChannelHandle rh_Channel,
				 CHDRV_SECTION_EVENT_e renm_Event,CHDRV_SECTION_CallBackDataParam_t *rpstru_Param)
{	
    if(rh_Channel == NULL || renm_Event < SECTION_EVENT_MIN || 
		renm_Event > SECTION_EVENT_MAX || rpstru_Param == NULL)
    {
		LOG("TEST_SECTION_AllocChannel_CallBack has invalid param\n");
        	return;
    }	
	
	if(gstru_GlobalVariable.h_SectionData_Come)
	{
		CH_RTOS_SignalSemaphore(gstru_GlobalVariable.h_SectionData_Come);
	}
	if(renm_Event == CHDRV_SECTION_EVENT_DATA_DETECT)
	{
		if(gstru_GlobalVariable.h_MesSectionCome)
		{
			CH_RTOS_SendMessage(gstru_GlobalVariable.h_MesSectionCome, &rh_Channel, 0);
		}
		gstru_GlobalVariable.enm_Event = renm_Event;
	}
}



//��parse_key �Ӷ�һ��":" ��ϳ�Parse_String
static char *Parse_String(char* rpc_Line, char* rpc_Key, char* rpc_Value, int ri_ValueLen)
{
	if(!(rpc_Line && rpc_Key && rpc_Value) || ri_ValueLen <= 0)
	{
		return NULL;
	}
	char *pc_Line = rpc_Line;
	S32 i_Value = 0;
	
	rpc_Value[0] = 0;
	do
	{
		pc_Line = strstr(pc_Line, rpc_Key);
		if(pc_Line && *(pc_Line + strlen(rpc_Key)) == '=')
		{
			pc_Line += strlen(rpc_Key) + 1;
			for(i_Value= 0; pc_Line[i_Value] && i_Value < ri_ValueLen - 1; i_Value++)
			{
				if((pc_Line[i_Value] >= 'a' && pc_Line[i_Value] <= 'z')
					|| (pc_Line[i_Value] >= 'A' && pc_Line[i_Value] <= 'Z')
					|| (pc_Line[i_Value] >= '0' && pc_Line[i_Value] <= '9')
					|| pc_Line[i_Value] == '_' || pc_Line[i_Value] == ':')
				{
					rpc_Value[i_Value] = pc_Line[i_Value];
				}
				else
				{
					break;
				}
			}
			rpc_Value[i_Value] = 0;
			LOG("[%s]=%s\n", rpc_Key, rpc_Value);
			return rpc_Value;
		}
		if(pc_Line)pc_Line++;
	}while(pc_Line);

	LOG("ERROR: can not get keyword [%s] in %s\n", rpc_Key, rpc_Line);
	return NULL;
}


//�� "00:11:22:33..."��ʽ���ַ���ת��Ϊrpc_Value 16������ֵ0x00,0x11,0x22,0x33...
//rpc_Str : Ҫת�����ַ���
//rpi_FilterLen : �������
//rpc_Value : ת�������ֵ
static CHDRV_SECTION_RESULT_e Converts_String_To_Value (char *rpc_Str, S32 rpi_FilterLen, char *rpc_Value)
{
	
	char c_ValueStr[8] = "0x";				//��"00:11:22..."���ηֲ����ϳ�0x00,0x11.....
	S32 i_Str = 0;							//��ʾrpc_Str��ƫ����
	S32 i_Value = 0;						//��ʾrpc_Value��ƫ����	
	S32 i_Result = 0;
	if(rpc_Str == NULL || rpc_Value == NULL)
	{
		LOG("invalid param!\n");
		return CHDRV_SECTION_INVALID_PARAM;
	}
	
	while(1)
	{
		c_ValueStr[2] = rpc_Str[i_Str++];
		c_ValueStr[3] = rpc_Str[i_Str++];		//ȡ"00:11:33:44..."�е�00,11,33,44...
		i_Result = 0;
		sscanf(c_ValueStr, "0x%x", &i_Result);
		rpc_Value[i_Value] = i_Result & 0xff;
		if(rpc_Str[i_Str] == ':')		//����"00:11:22:33..."�е�" : "
		{
			i_Str++;	
		}
		
		if(rpc_Str[i_Str] == '\0')
		{
			break;
		}
		i_Value++;
	}

	if((i_Value + 1) < rpi_FilterLen)
	{
		LOG("���õĹ�����ȸ��������ݳ��Ȳ�һ�£�����test_config.cfg�ļ��м��!\n");
		//return CHDRV_SECTION_FAIL;
	}
	
	return CHDRV_SECTION_OK;

}

//��env�л�ȡ���˲���
//����SETFILTER_PARAMs : env �еĲ�����
static CHDRV_SECTION_RESULT_e Get_FilterParm_FromStrings(char *rpuc_strings, U8 *rpuc_Data, U8 *rpuc_Mask,
							U8 *rpuc_Pattern, S32 *rpi_FilterLen, CH_BOOL *rpb_NotMatchMode, U16 *rpus_Pid, CH_BOOL *rpb_CrC)
{
	CMD_PARAM value;						//����parse_key()���ص��ַ���
	char str[256] = " ";					//����parse_key()�����ַ���
	S32 i_Result = 0;	
	
	if(rpuc_strings == NULL)
	{
		LOG("invalid param!\n");
		return CHDRV_SECTION_INVALID_PARAM;
	}

	if(rpi_FilterLen)
	{
		value = Parse_String(rpuc_strings, "filterlen", str, sizeof(str));
		*rpi_FilterLen = value.get_i();
	}
	if(rpb_NotMatchMode)
	{
		value = Parse_String(rpuc_strings, "notmatchmode", str, sizeof(str));
		*rpb_NotMatchMode = value.get_i();
	}
	if(rpuc_Data && rpi_FilterLen)
	{
		Parse_String(rpuc_strings, "data", str, sizeof(str));
		i_Result = Converts_String_To_Value(str, *rpi_FilterLen, (char *)rpuc_Data);
		if(i_Result != CHDRV_SECTION_OK)
		{
			LOG("Converts_String_To_Value failed!\n");
			return CHDRV_SECTION_FAIL;
		}
	}
	if(rpuc_Mask && rpi_FilterLen)
	{
		Parse_String(rpuc_strings, "mask", str, sizeof(str));
		i_Result = Converts_String_To_Value(str, *rpi_FilterLen, (char *)rpuc_Mask);
		if(i_Result != CHDRV_SECTION_OK)
		{
			LOG("Converts_String_To_Value failed!\n");
			return CHDRV_SECTION_FAIL;
		}
	}
	if(rpuc_Pattern && rpi_FilterLen)
	{
		Parse_String(rpuc_strings, "pattern", str, sizeof(str));
		
		i_Result = Converts_String_To_Value(str, *rpi_FilterLen, (char *)rpuc_Pattern);
		if(i_Result != CHDRV_SECTION_OK)
		{
			LOG("Converts_String_To_Value failed!\n");
			return CHDRV_SECTION_FAIL;
		}
	}
	if(rpus_Pid)
	{
		Parse_String(rpuc_strings, "pid", str, sizeof(str));
		if(str[0])
		{
			value = str;
			*rpus_Pid = (U16)value.get_i();
		}
	}

	if(rpb_CrC)
	{
		Parse_String(rpuc_strings, "crc", str, sizeof(str));
		if(str[0])
		{
			value = str;
			*rpb_CrC = (U16)value.get_b();
		}
		
	}
	return CHDRV_SECTION_OK;

}
#if ENABLE_TESTCASE

CASE_F(TEST_SECTION_INIT1,
	"���Զ�γ�ʼ��ʱ�Ƿ񷵻�OK") 
{	
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_Init());
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_Init()) << "��γ�ʼ��Ӧ�÷���OK";
}



CASE_F(TEST_SECTION_INIT2,
	"����ģ���Ƿ�ɱ�����") 
{
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_Term());
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_Init());
}


CASE_F(TEST_SECTION_INIT3,
	"����CHDRV_SECTION_GetVersion�Ƿ�ɹ�") 
{	
	CHDRV_Version_t stru_Ver;
	
	memset(&stru_Ver, 0, sizeof(stru_Ver));
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_GetVersion(&stru_Ver));
	EXPECT_NE(0, stru_Ver);
	LOG("\nstru_Ver=%x\n", stru_Ver);
}

CASE_F(CHDRV_SECTION_PARAMS1,
	"CHDRV_SECTION_AllocChannel�����������") 
{
    CHDRV_SECTION_FilterCallBack fun_DetectCallback = TEST_SECTION_AllocChannel_CallBack;
	CHDRV_SECTION_ChannelHandle	h_Channel;
	
	EXPECT_EQ(CHDRV_SECTION_INVALID_PARAM, CHDRV_SECTION_AllocChannel (ERRO_DEMUX_ID, NORMAL_CHANNEL_PID,
        fun_DetectCallback, &h_Channel, NORMAL_CHANNEL_BUFFERSIZE));	//����1���������
	EXPECT_EQ(CHDRV_SECTION_INVALID_PARAM, CHDRV_SECTION_AllocChannel (NORMAL_DEMUX_ID, ERRO_CHANNEL_PID,
		fun_DetectCallback, &h_Channel, NORMAL_CHANNEL_BUFFERSIZE));	//����2���������
	/*EXPECT_EQ(CHDRV_SECTION_INVALID_PARAM, CHDRV_SECTION_AllocChannel (NORMAL_DEMUX_ID, NORMAL_CHANNEL_PID, 
		NULL, &h_Channel, NORMAL_CHANNEL_BUFFERSIZE));	*/				//����3���������
	EXPECT_EQ(CHDRV_SECTION_INVALID_PARAM, CHDRV_SECTION_AllocChannel (NORMAL_DEMUX_ID, NORMAL_CHANNEL_PID,
		fun_DetectCallback, NULL, NORMAL_CHANNEL_BUFFERSIZE));			//����4���������
	EXPECT_EQ(CHDRV_SECTION_INVALID_PARAM, CHDRV_SECTION_AllocChannel (NORMAL_DEMUX_ID, NORMAL_CHANNEL_PID,
        fun_DetectCallback, &h_Channel, ERRO_CHANNEL_BUFFERSIZE));		//����5���������

}

CASE_F(CHDRV_SECTION_PARAMS2,
	"CHDRV_SECTION_FreeChannel�����������") 
{
	EXPECT_EQ(CHDRV_SECTION_INVALID_PARAM, CHDRV_SECTION_FreeChannel(NULL));	
}

CASE_F(CHDRV_SECTION_PARAMS3,
	"CHDRV_SECTION_StopChannel�����������") 
{	
	EXPECT_EQ(CHDRV_SECTION_INVALID_PARAM, CHDRV_SECTION_StopChannel(NULL));	
}

CASE_F(CHDRV_SECTION_PARAMS4,
	"CHDRV_SECTION_FlushChannelBuffer�����������") 
{	
	EXPECT_EQ(CHDRV_SECTION_INVALID_PARAM, CHDRV_SECTION_FlushChannelBuffer(NULL));	
}

CASE_F(CHDRV_SECTION_PARAMS5,
	"CHDRV_SECTION_SetChannelPid�����������") 
{
	CHDRV_SECTION_ChannelHandle	h_Channel =  NULL;
	
	EXPECT_EQ(CHDRV_SECTION_INVALID_PARAM, CHDRV_SECTION_SetChannelPid(NULL, NORMAL_CHANNEL_PID));	
	EXPECT_EQ(CHDRV_SECTION_INVALID_PARAM, CHDRV_SECTION_SetChannelPid(h_Channel, ERRO_CHANNEL_PID));	
}

CASE_F(CHDRV_SECTION_PARAMS6,
	"CHDRV_SECTION_AllocFilter�����������") 
{	
    CHDRV_SECTION_AllocParam_t stru_Param;
    CHDRV_SECTION_FilterHandle h_Filter;

	EXPECT_EQ(CHDRV_SECTION_INVALID_PARAM, CHDRV_SECTION_AllocFilter(ERRO_DEMUX_ID, &stru_Param, &h_Filter));	
	EXPECT_EQ(CHDRV_SECTION_INVALID_PARAM, CHDRV_SECTION_AllocFilter(NORMAL_DEMUX_ID, NULL, &h_Filter));	
	EXPECT_EQ(CHDRV_SECTION_INVALID_PARAM, CHDRV_SECTION_AllocFilter(NORMAL_DEMUX_ID, &stru_Param, NULL));	
}

CASE_F(CHDRV_SECTION_PARAMS7,
	"CHDRV_SECTION_FreeFilter�����������") 
{
	EXPECT_EQ(CHDRV_SECTION_INVALID_PARAM, CHDRV_SECTION_FreeFilter(NULL));	
}

CASE_F(CHDRV_SECTION_PARAMS8,
	"CHDRV_SECTION_AssociateFilterToChannel�����������") 
{
	CHDRV_SECTION_ChannelHandle  h_Channel = NULL;
    CHDRV_SECTION_FilterHandle h_Filter = NULL;

	EXPECT_EQ(CHDRV_SECTION_INVALID_PARAM, CHDRV_SECTION_AssociateFilterToChannel(NULL, h_Channel));	
	EXPECT_EQ(CHDRV_SECTION_INVALID_PARAM, CHDRV_SECTION_AssociateFilterToChannel(h_Filter, NULL));	
}

CASE_F(CHDRV_SECTION_PARAMS9,
	"CHDRV_SECTION_DisAssociateFilterToChannel�����������") 
{	
	CHDRV_SECTION_ChannelHandle  h_Channel = NULL;
    CHDRV_SECTION_FilterHandle h_Filter = NULL;

	EXPECT_EQ(CHDRV_SECTION_INVALID_PARAM, CHDRV_SECTION_DisAssociateFilterToChannel(NULL, h_Channel));	
	EXPECT_EQ(CHDRV_SECTION_INVALID_PARAM, CHDRV_SECTION_DisAssociateFilterToChannel(h_Filter, NULL));	
}

CASE_F(CHDRV_SECTION_PARAMS10,
	"CHDRV_SECTION_SetFilter�����������") 
{
	CHDRV_SECTION_FilterHandle h_Filter = NULL;	
    U8 uc_Data[FILTER_SIZE];
    U8 uc_Mask[FILTER_SIZE];
    U8 uc_Pattern[FILTER_SIZE];

	EXPECT_EQ(CHDRV_SECTION_INVALID_PARAM, CHDRV_SECTION_SetFilter(NULL, uc_Data, 
									uc_Mask, uc_Pattern, NORMAL_FILTERLEN, NORMAL_B_NOTMATCHMODE));	
	EXPECT_EQ(CHDRV_SECTION_INVALID_PARAM, CHDRV_SECTION_SetFilter(h_Filter, NULL, 
									uc_Mask, uc_Pattern, NORMAL_FILTERLEN, NORMAL_B_NOTMATCHMODE));	
	EXPECT_EQ(CHDRV_SECTION_INVALID_PARAM, CHDRV_SECTION_SetFilter(h_Filter, uc_Data, 
									NULL, uc_Pattern, NORMAL_FILTERLEN, NORMAL_B_NOTMATCHMODE));	
	EXPECT_EQ(CHDRV_SECTION_INVALID_PARAM, CHDRV_SECTION_SetFilter(h_Filter, uc_Data, 
									uc_Mask, NULL, NORMAL_FILTERLEN, NORMAL_B_NOTMATCHMODE));
	EXPECT_EQ(CHDRV_SECTION_INVALID_PARAM, CHDRV_SECTION_SetFilter(h_Filter, uc_Data, 
									uc_Mask, uc_Pattern, ERRO_FILTERLEN, NORMAL_B_NOTMATCHMODE));	
	EXPECT_EQ(CHDRV_SECTION_INVALID_PARAM, CHDRV_SECTION_SetFilter(h_Filter, uc_Data, 
									uc_Mask, uc_Pattern, NORMAL_FILTERLEN, ERRO_B_NOTMATCHMODE));	
}

CASE_F(CHDRV_SECTION_PARAMS11,
	"CHDRV_SECTION_StartFilter�����������") 
{
	EXPECT_EQ(CHDRV_SECTION_INVALID_PARAM, CHDRV_SECTION_StartFilter(NULL));	
}

CASE_F(CHDRV_SECTION_PARAMS12,
	"CHDRV_SECTION_StopFilter�����������") 
{
	EXPECT_EQ(CHDRV_SECTION_INVALID_PARAM, CHDRV_SECTION_StopFilter(NULL));	
}

CASE_F(CHDRV_SECTION_PARAMS13,
	"CHDRV_SECTION_ReadFilterData�����������") 
{	
	CHDRV_SECTION_ChannelHandle h_Channel = NULL;				
	U8 *puc_DataBuf = NULL;
     	S32 i_Datalen = DATA_BUFSIZE;

     	puc_DataBuf = new U8[DATA_BUFSIZE];
	int dead_test = fw_getenv_i("DEAD_HALT_TEST");
	if(dead_test == 1)
	{
		EXPECT_EQ(CHDRV_SECTION_INVALID_PARAM, CHDRV_SECTION_ReadFilterData(NULL, puc_DataBuf, &i_Datalen));	
		EXPECT_EQ(CHDRV_SECTION_INVALID_PARAM, CHDRV_SECTION_ReadFilterData(h_Channel, NULL, &i_Datalen));	
		EXPECT_EQ(CHDRV_SECTION_INVALID_PARAM, CHDRV_SECTION_ReadFilterData(h_Channel, puc_DataBuf, NULL));	
	}
	else
	{
		EXPECT_EQ(dead_test, 1) << "Ӧͨ����������";
	}
	delete []puc_DataBuf;
	/*  *i_Datalen ��NULL�ᵼ������,����CHDRV_SECTION_ReadFilterData()����
	���д��������⣬��ʹ�ò�������ֹʹ�ÿ�ָ�뵼�³������*/
}

CASE_F(CHDRV_SECTION_PARAMS14,
	"CHDRV_SECTION_GetInfo�����������") 
{
	U32 Param;

	EXPECT_EQ(CHDRV_SECTION_INVALID_PARAM, CHDRV_SECTION_GetInfo(ERRO_DEMUX_ID, (CHDRV_SECTION_INFO_TYPE_e)NORMAL_SECTION_INFO_TYPE, &Param));	
	EXPECT_EQ(CHDRV_SECTION_INVALID_PARAM, CHDRV_SECTION_GetInfo(NORMAL_DEMUX_ID, (CHDRV_SECTION_INFO_TYPE_e)ERRO_SECTION_INFO_TYPE, &Param));	
	EXPECT_EQ(CHDRV_SECTION_INVALID_PARAM, CHDRV_SECTION_GetInfo(NORMAL_DEMUX_ID, (CHDRV_SECTION_INFO_TYPE_e)NORMAL_SECTION_INFO_TYPE, NULL));	
}

CASE_F(CHDRV_SECTION_PARAMS15,
	"CHDRV_SECTION_FindChannelByPID�����������") 
{	
	CHDRV_SECTION_ChannelHandle h_Channel = NULL;

	EXPECT_EQ(CHDRV_SECTION_INVALID_PARAM, CHDRV_SECTION_FindChannelByPID(ERRO_CHANNEL_PID, &h_Channel));	
	EXPECT_EQ(CHDRV_SECTION_INVALID_PARAM, CHDRV_SECTION_FindChannelByPID(NORMAL_CHANNEL_PID, NULL));	
}

CASE_F(CHDRV_SECTION_PARAMS16,
	"CHDRV_SECTION_ModifyChannelCrcControl�����������") 
{	
	CHDRV_SECTION_ChannelHandle  h_Channel = NULL;

	EXPECT_EQ(CHDRV_SECTION_INVALID_PARAM, CHDRV_SECTION_ModifyChannelCrcControl(NULL, NORMAL_B_NOTMATCHMODE));	
	EXPECT_EQ(CHDRV_SECTION_INVALID_PARAM, CHDRV_SECTION_ModifyChannelCrcControl(h_Channel, ERRO_B_NOTMATCHMODE));	
}
 
CASE_F(CHDRV_SECTION_CHANNEL1,
	"�����ܷ��ԺϷ������ɹ�����һ��channel") 
{
	S32 i_DemuxId = gstru_GlobalVariable.i_CURRENT_DMXID * 2 + (gstru_GlobalVariable.i_CURRENT_DMXID ? 1 : 0);
	U16 us_ChannelPid = 0x1fff;
	S32 i_BufferSize = 0;
	S32 i_Result = 0;	

	i_BufferSize = fw_getenv_i("SECTION_TEST_BUFFERSIZE_1");		//��env��ȡbuffersize
	
	int prognum = 0;
	char str_tuner_params[16];

	sprintf(str_tuner_params, "TUNER_PARAMS%d", gstru_GlobalVariable.i_CURRENT_DMXID + 1);
	prognum = request_play_program("SECTION_TEST_FILE_NAME_1", str_tuner_params);
	RUN_CMD("tuner.lock %s", fw_getenv(str_tuner_params));


	RUN_CMD("av.start %d %d\n",get_demux_id(),prognum);

	if(!gstru_GlobalVariable.h_SectionData_Come)
	{
		i_Result = CH_RTOS_CreateSemaphore(0, &gstru_GlobalVariable.h_SectionData_Come);
		if(i_Result != CH_RTOS_OK)
		{
			LOG("CH_RTOS_CreateSemaphore return %d\n", i_Result);
			return;
		}
	}

	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_Init());
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_AllocChannel(i_DemuxId, us_ChannelPid,
        			TEST_SECTION_AllocChannel_CallBack, &gstru_GlobalVariable.h_Channel, i_BufferSize)); 
	EXPECT_NE(NULL, (int)gstru_GlobalVariable.h_Channel) << "������ͨ�����Ӧ��Ϊ��\n";	
}


CASE_F(CHDRV_SECTION_CHANNEL2,
	"����TEST_SECTION_CHANNEL1�д�����channel pidΪ����ֵ")
{
    U16 us_ChannelPid = 0;
	CH_BOOL b_crc = CH_TRUE;
	
	Get_FilterParm_FromStrings(fw_getenv("SECTION_FILTER_PARAMS"),NULL, NULL, NULL, NULL, NULL, &us_ChannelPid, &b_crc);
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_SetChannelPid(gstru_GlobalVariable.h_Channel, us_ChannelPid)); 
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_ModifyChannelCrcControl(gstru_GlobalVariable.h_Channel, b_crc));
}

CASE_F(TEST_SECTION_FILTER1,
	"�ԺϷ���������2��filter")
{
	S32 i_DemuxId = gstru_GlobalVariable.i_CURRENT_DMXID * 2 + (gstru_GlobalVariable.i_CURRENT_DMXID ? 1 : 0);
	CHDRV_SECTION_AllocParam_t gstru_AllocParam;
	
	memset(&gstru_AllocParam, 0, sizeof(gstru_AllocParam));
	gstru_AllocParam.h_Channel = gstru_GlobalVariable.h_Channel;
	gstru_AllocParam.enm_SectionSize = (CHDRV_SECTION_SECTION_SIZE_e)fw_getenv_i("SECTION_TEST_SECTIONSIZE");
	gstru_AllocParam.enm_FilterSize = (CHDRV_SECTION_FILTER_SIZE_e)fw_getenv_i("SECTION_TEST_FILTERSIZE");
	gstru_AllocParam.enm_MatchMode = (CHDRV_SECTION_MATCH_MODE_e)fw_getenv_i("SECTION_TEST_MATCHMODE");
	
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_AllocFilter(i_DemuxId, &gstru_AllocParam, &gstru_GlobalVariable.h_Filter1)) << "������һ��filterʧ��\n"; 
	EXPECT_NE(NULL, (int)gstru_GlobalVariable.h_Filter1) << "Filter1���������Ӧ��Ϊ��\n";
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_AllocFilter(i_DemuxId, &gstru_AllocParam, &gstru_GlobalVariable.h_Filter2)) << "�����ڶ���filterʧ��\n"; 
	EXPECT_NE(NULL, (int)gstru_GlobalVariable.h_Filter2) << "Filter2���������Ӧ��Ϊ��\n";
}





CASE_F(TEST_SECTION_FILTER3,
	"�ֱ�����2��filter�Ĺ�������")
{
	S32 i_FilterLen = 0;
	U8 uc_Data[FILTER_SIZE] = {0};
	U8 uc_Mask[FILTER_SIZE] = {0};
	U8 uc_Pattern[FILTER_SIZE] = {0};
	CH_BOOL b_NotMatchMode = CH_FALSE;

	EXPECT_EQ(CHDRV_SECTION_OK, Get_FilterParm_FromStrings(fw_getenv("SECTION_FILTER_PARAMS"),uc_Data, uc_Mask, uc_Pattern, &i_FilterLen, &b_NotMatchMode, NULL, NULL)); 
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_SetFilter(gstru_GlobalVariable.h_Filter1, uc_Data, uc_Mask, 
				uc_Pattern, i_FilterLen, b_NotMatchMode)) << "����filter1��������ʧ��\n"; 
	uc_Data[0] += 1;
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_SetFilter(gstru_GlobalVariable.h_Filter2, uc_Data, uc_Mask, 
				uc_Pattern, i_FilterLen, b_NotMatchMode)) << "����filter2��������ʧ��\n";
}

CASE_F(TEST_SECTION_FILTER4,
	"���ոմ����ĵ�1��filter���������洴����channel��")
{
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_AssociateFilterToChannel(gstru_GlobalVariable.h_Filter1, gstru_GlobalVariable.h_Channel)) 
																<< "filter1����channelʧ��\n"; 
	gstru_GlobalVariable.enm_Event = CHDRV_SECTION_EVENT_MAX;
}

CASE_F(CHDRV_SECTION_CHANNEL3,
	"�ֱ��Բ���channel pid �� (channel pid + 1)����CHDRV_SECTION_FindChannelByPID")
{
    	U16 us_ChannelPid = 0;
	CHDRV_SECTION_ChannelHandle h_Channel = NULL;
	
	Get_FilterParm_FromStrings(fw_getenv("SECTION_FILTER_PARAMS"),NULL, NULL, NULL, NULL, NULL, &us_ChannelPid, NULL);
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_FindChannelByPID(us_ChannelPid, &h_Channel)); 
	EXPECT_NE(NULL, (int)h_Channel) << "������ͨ�����Ӧ��Ϊ��\n";	
	EXPECT_NE(CHDRV_SECTION_OK, CHDRV_SECTION_FindChannelByPID(us_ChannelPid + 1, &h_Channel)); 
}


CASE_F(TEST_SECTION_FILTER5,
	"start filter1")
{	
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_StartFilter(gstru_GlobalVariable.h_Filter1));
}

#if 0 	/*sqzou20170925*/
CASE_F(CHDRV_SECTION_CHANNEL5,
	"�ȴ�1S�����callback��Ӧ�ղ����κ�����")
{
     int retry = 5;

	while(retry --> 0
	 && CHDRV_SECTION_EVENT_DATA_DETECT != gstru_GlobalVariable.enm_Event)
	{
		CH_RTOS_TaskDelay(1000); 	
	} 
	EXPECT_NE(CHDRV_SECTION_EVENT_DATA_DETECT, gstru_GlobalVariable.enm_Event) << "��δstart filter����Ӧ�յ�����"<< RUN_CMD("section.info");		
}
#endif

CASE_F(CHDRV_SECTION_CHANNEL4,
	"����start channel")
{
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_StartChannel(gstru_GlobalVariable.h_Channel)); 
}

CASE_F(TEST_SECTION_FILTER6,
	"�ȴ�1S�����callback��Ӧ�����ݵ���������section_readӦ�ܶ������ݣ����ݳ��ȺϷ�")
{
	 U8 *puc_DataBuf = NULL;
     S32 i_Datalen = DATA_BUFSIZE;
     int retry = 5;

	puc_DataBuf = new U8[DATA_BUFSIZE];
	while(retry --> 0
	 && CHDRV_SECTION_EVENT_DATA_DETECT != gstru_GlobalVariable.enm_Event)
	{
		CH_RTOS_TaskDelay(1000); 	
	}
	EXPECT_EQ(CHDRV_SECTION_EVENT_DATA_DETECT, gstru_GlobalVariable.enm_Event);	
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_ReadFilterData(gstru_GlobalVariable.h_Channel, puc_DataBuf, &i_Datalen));	
	EXPECT_LT(0, i_Datalen) << "1��channel��1��filterδ�յ�pat����" << RUN_CMD("section.info");		
	EXPECT_GE(4096, i_Datalen);		//i_Datalen <= 4096
	delete []puc_DataBuf;
}

CASE_F(TEST_SECTION_FILTER7,
	"stop filter 1")
{
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_StopFilter(gstru_GlobalVariable.h_Filter1)); 
	gstru_GlobalVariable.enm_Event = CHDRV_SECTION_EVENT_MAX;
}


CASE_F(TEST_SECTION_FILTER5_1,
	"���ոմ����ĵ�2��filter���������洴����channel��")
{
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_AssociateFilterToChannel(gstru_GlobalVariable.h_Filter2, gstru_GlobalVariable.h_Channel)) 
																<< "filter2����channelʧ��\n"; 
	gstru_GlobalVariable.enm_Event = CHDRV_SECTION_EVENT_MAX;
}

CASE_F(CHDRV_SECTION_CHANNEL4_1,
	"�ֱ��Բ���channel pid �� (channel pid + 1)����CHDRV_SECTION_FindChannelByPID")
{
    	U16 us_ChannelPid = 0;
	CHDRV_SECTION_ChannelHandle h_Channel = NULL;
	
	Get_FilterParm_FromStrings(fw_getenv("SECTION_FILTER_PARAMS"),NULL, NULL, NULL, NULL, NULL, &us_ChannelPid, NULL);
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_FindChannelByPID(us_ChannelPid, &h_Channel)); 
	EXPECT_NE(NULL, (int)h_Channel) << "������ͨ�����Ӧ��Ϊ��\n";	
	EXPECT_NE(CHDRV_SECTION_OK, CHDRV_SECTION_FindChannelByPID(us_ChannelPid + 1, &h_Channel)); 
}


CASE_F(TEST_SECTION_FILTER6_1,
	"start filter1")
{
	
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_StartFilter(gstru_GlobalVariable.h_Filter1));
}

#if 0 	/*sqzou20170925*/
CASE_F(CHDRV_SECTION_CHANNEL6,
	"�ȴ�1S�����callback��Ӧ�ղ����κ�����")
{
     int retry = 5;

	while(retry --> 0
	 && CHDRV_SECTION_EVENT_DATA_DETECT != gstru_GlobalVariable.enm_Event)
	{
		CH_RTOS_TaskDelay(1000); 	
	} 
	EXPECT_NE(CHDRV_SECTION_EVENT_DATA_DETECT, gstru_GlobalVariable.enm_Event) << "��δstart filter����Ӧ�յ�����" << RUN_CMD("section.info");		
}
#endif

CASE_F(CHDRV_SECTION_CHANNEL5_1,
	"����start channel")
{
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_StartChannel(gstru_GlobalVariable.h_Channel)); 
}

CASE_F(TEST_SECTION_FILTER7_1,
	"�ȴ�1S�����callback��Ӧ�����ݵ���������section_readӦ�ܶ������ݣ����ݳ��ȺϷ�")
{
	U8 *puc_DataBuf = NULL;
     S32 i_Datalen = DATA_BUFSIZE;
     int retry = 5;

	puc_DataBuf = new U8[DATA_BUFSIZE];

	while(retry --> 0
	 && CHDRV_SECTION_EVENT_DATA_DETECT != gstru_GlobalVariable.enm_Event)
	{
		CH_RTOS_TaskDelay(1000); 	
	}
	EXPECT_EQ(CHDRV_SECTION_EVENT_DATA_DETECT, gstru_GlobalVariable.enm_Event);	
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_ReadFilterData(gstru_GlobalVariable.h_Channel, puc_DataBuf, &i_Datalen));	
	EXPECT_LT(0, i_Datalen) << "1��channel��2����ͬ��filterδ�յ�pat����" << RUN_CMD("section.info");		//i_Datalen > 0
	EXPECT_GE(4096, i_Datalen);		//i_Datalen <= 4096
	delete[] puc_DataBuf;
}

CASE_F(TEST_SECTION_FILTER8,
	"stop filter 1")
{
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_StopFilter(gstru_GlobalVariable.h_Filter1)); 
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_StopChannel(gstru_GlobalVariable.h_Channel)); 
}

CASE_F(TEST_SECTION_FILTER9,
	"������ݰ����������ȴ�1S�����callback��Ӧ�ղ����κ�����")
{
	gstru_GlobalVariable.enm_Event = CHDRV_SECTION_EVENT_MAX;
	CH_RTOS_TaskDelay(1000);  
	EXPECT_NE(CHDRV_SECTION_EVENT_DATA_DETECT, gstru_GlobalVariable.enm_Event);		
}

CASE_F(TEST_SECTION_FILTER10,
	"start filter2")
{
	S32 i_FilterLen = 0;
	U8 uc_Data[FILTER_SIZE] = {0};
	U8 uc_Mask[FILTER_SIZE] = {0};
	U8 uc_Pattern[FILTER_SIZE] = {0};
	CH_BOOL b_NotMatchMode = CH_FALSE;

	EXPECT_EQ(CHDRV_SECTION_OK, Get_FilterParm_FromStrings(fw_getenv("SECTION_FILTER_PARAMS"),uc_Data, uc_Mask, uc_Pattern, &i_FilterLen, &b_NotMatchMode, NULL, NULL)); 
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_SetFilter(gstru_GlobalVariable.h_Filter2, uc_Data, uc_Mask, 
				uc_Pattern, i_FilterLen, b_NotMatchMode)) << "����filter2��������ʧ��\n"; 
	uc_Data[0] += 1;
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_SetFilter(gstru_GlobalVariable.h_Filter1, uc_Data, uc_Mask, 
				uc_Pattern, i_FilterLen, b_NotMatchMode)) << "����filter1��������ʧ��\n";
				
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_StartFilter(gstru_GlobalVariable.h_Filter2)); 
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_StartChannel(gstru_GlobalVariable.h_Channel)); 
}

CASE_F(TEST_SECTION_FILTER11,
	"�ȴ�1S�����callback��Ӧ�����ݵ���������section_readӦ�ܶ������ݣ����ݳ��ȺϷ�")
{
	U8 *puc_DataBuf = NULL;
     S32 i_Datalen = DATA_BUFSIZE;
     int retry = 5;

	puc_DataBuf = new U8[DATA_BUFSIZE];

	while(retry --> 0
	 && CHDRV_SECTION_EVENT_DATA_DETECT != gstru_GlobalVariable.enm_Event)
	{
		CH_RTOS_TaskDelay(1000); 	
	}	
	EXPECT_EQ(CHDRV_SECTION_EVENT_DATA_DETECT, gstru_GlobalVariable.enm_Event);	
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_ReadFilterData(gstru_GlobalVariable.h_Channel, puc_DataBuf, &i_Datalen));
	EXPECT_LT(0, i_Datalen)  << "1��channel��2����ͬ��filterδ�յ�����" << RUN_CMD("section.info");		//i_Datalen > 0
	EXPECT_GE(4096, i_Datalen);		//i_Datalen <= 4096

	delete[] puc_DataBuf;
}

CASE_F(TEST_SECTION_FILTER12,
	"stop filter 2")
{
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_StopFilter(gstru_GlobalVariable.h_Filter2)); 
}

CASE_F(TEST_SECTION_FILTER13,
	"������ݰ����������ȴ�1S�����callback��Ӧ�ղ����κ�����")
{
	gstru_GlobalVariable.enm_Event = CHDRV_SECTION_EVENT_MAX;
	CH_RTOS_TaskDelay(1000);  
	EXPECT_NE(CHDRV_SECTION_EVENT_DATA_DETECT, gstru_GlobalVariable.enm_Event);		
}

CASE_F(TEST_SECTION_FILTER14,
	"start filter1,start filter2")
{
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_StartFilter(gstru_GlobalVariable.h_Filter1)); 
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_StartFilter(gstru_GlobalVariable.h_Filter2)); 
}

CASE_F(TEST_SECTION_FILTER15,
	"�ȴ�1S�����callbackӦ�����ݵ���")
{
	int retry = 5;

	while(retry --> 0
	 && CHDRV_SECTION_EVENT_DATA_DETECT != gstru_GlobalVariable.enm_Event)
	{
		CH_RTOS_TaskDelay(1000); 	
	}
	EXPECT_EQ(CHDRV_SECTION_EVENT_DATA_DETECT, gstru_GlobalVariable.enm_Event) << "1��channel��2��filterδ�յ�����" << RUN_CMD("section.info");	
}

CASE_F(TEST_SECTION_FILTER16,
	"stop filter 1,stop filter 2")
{
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_StopFilter(gstru_GlobalVariable.h_Filter1)); 
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_StopFilter(gstru_GlobalVariable.h_Filter2)); 
}

CASE_F(TEST_SECTION_FILTER17,
	"������ݰ����������ȴ�1S�����callback��Ӧ�ղ����κ�����")
{
	gstru_GlobalVariable.enm_Event = CHDRV_SECTION_EVENT_MAX;
	CH_RTOS_TaskDelay(1000);  
	EXPECT_NE(CHDRV_SECTION_EVENT_DATA_DETECT, gstru_GlobalVariable.enm_Event);		
}

CASE_F(TEST_SECTION_FILTER18,
	"�л�����ЧƵ�㣬Ȼ��start filter1��start filter2")
{
	char str_tuner_params[32];

	sprintf(str_tuner_params, "TUNER_INVALID_PARAMS%d", gstru_GlobalVariable.i_CURRENT_DMXID + 1);
	
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_StartFilter(gstru_GlobalVariable.h_Filter1)); 
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_StartFilter(gstru_GlobalVariable.h_Filter2)); 
	CH_RTOS_TaskDelay(1000); 
	RUN_CMD("tuner.lock %s\n", fw_getenv(str_tuner_params));
}

CASE_F(TEST_SECTION_FILTER19,
	"�ȴ�1S��Ȼ�����flush channel")
{	
	CH_RTOS_TaskDelay(1000);  
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_FlushChannelBuffer(gstru_GlobalVariable.h_Channel)); 
}

CASE_F(TEST_SECTION_FILTER20,
	"����read sectioin��Ӧ����NO_DATA")
{
	U8 *puc_DataBuf = NULL;
     S32 i_Datalen = DATA_BUFSIZE;

	puc_DataBuf = new U8[DATA_BUFSIZE];

	EXPECT_EQ(CHDRV_SECTION_NO_DATA, CHDRV_SECTION_ReadFilterData(gstru_GlobalVariable.h_Channel, 
		puc_DataBuf, &i_Datalen)) << "FlushChannelBuffer�ӿ�δ��ȷʵ�֣�����δ�����";	

	delete[] puc_DataBuf;
}

CASE_F(TEST_SECTION_FILTER21,
	"�ָ�����ЧƵ�㣬�������ԡ�Stop filter1��stop filter2��stop channel")
{
	char str_tuner_params[16];

	sprintf(str_tuner_params, "TUNER_PARAMS%d", gstru_GlobalVariable.i_CURRENT_DMXID + 1);
	RUN_CMD("tuner.lock %s\n", fw_getenv(str_tuner_params));
	
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_StopFilter(gstru_GlobalVariable.h_Filter1)); 
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_StopFilter(gstru_GlobalVariable.h_Filter2)); 
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_StopChannel(gstru_GlobalVariable.h_Channel)); 	
	gstru_GlobalVariable.enm_Event = CHDRV_SECTION_EVENT_MAX;
}

CASE_F(TEST_SECTION_FILTER22,
	"ȡ��2��filter��channel�Ĺ���")
{
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_DisAssociateFilterToChannel(gstru_GlobalVariable.h_Filter1, gstru_GlobalVariable.h_Channel));	
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_DisAssociateFilterToChannel(gstru_GlobalVariable.h_Filter2, gstru_GlobalVariable.h_Channel));	
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_FreeFilter(gstru_GlobalVariable.h_Filter2)); 
	gstru_GlobalVariable.h_Filter2 = NULL;
}


CASE_F(TEST_SECTION_FILTER23,
	"����crc set�Ƿ���Ч,��crccheckΪtrueʱ,Ӧ�ղ���ECM;��Ϊfalseʱ,���յ�")
{
    	U16 us_ChannelPid = 0;
	CH_BOOL b_CrcControl[2] = {CH_FALSE, CH_TRUE};
	U8 uc_Data[FILTER_SIZE] = {0};
	U8 uc_Mask[FILTER_SIZE] = {0};
	U8 uc_Pattern[FILTER_SIZE] = {0};	
	S32 i_FilterLen = 0;
	CH_BOOL b_NotMatchMode = CH_FALSE;
	S32 i_SetCrc = 0;	//��������ڶ���ѭ��������crcģʽCH_FALSE,CH_TRUE
	char *p;	

	 	

	p = fw_getenv("SECTION_FILTER_CRCPARAMS");
	LOG("p = %s\n", p?:"NULL");
	if(p)
	{
		p = strtok(p, ";");
	}
	while(p)
	{
		EXPECT_EQ(CHDRV_SECTION_OK, Get_FilterParm_FromStrings(p, uc_Data, uc_Mask, uc_Pattern, &i_FilterLen, &b_NotMatchMode, &us_ChannelPid, NULL)); 
		EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_AssociateFilterToChannel(gstru_GlobalVariable.h_Filter1, gstru_GlobalVariable.h_Channel));	
		EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_SetFilter(gstru_GlobalVariable.h_Filter1, uc_Data, uc_Mask, uc_Pattern, i_FilterLen, b_NotMatchMode)); 
		EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_SetChannelPid(gstru_GlobalVariable.h_Channel, us_ChannelPid)); 
		for(i_SetCrc=0;i_SetCrc< 2;i_SetCrc++)
		{
			gstru_GlobalVariable.enm_Event = CHDRV_SECTION_EVENT_MAX;
			EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_ModifyChannelCrcControl(gstru_GlobalVariable.h_Channel, b_CrcControl[i_SetCrc]));
			EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_StartFilter(gstru_GlobalVariable.h_Filter1)); 
			EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_StartChannel(gstru_GlobalVariable.h_Channel)); 
			CH_RTOS_TaskDelay(10000);

			if(b_CrcControl[i_SetCrc] == CH_TRUE)
			{
				EXPECT_NE(CHDRV_SECTION_EVENT_DATA_DETECT, gstru_GlobalVariable.enm_Event)<< "CRC checkΪtrue��Ӧ�ղ���ECM����";
			}
			else
			{
				EXPECT_EQ(CHDRV_SECTION_EVENT_DATA_DETECT, gstru_GlobalVariable.enm_Event)<< "CRC checkΪfalse��Ӧ���յ�ECM����";
			}

			EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_StopFilter(gstru_GlobalVariable.h_Filter1)); 
			EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_StopChannel(gstru_GlobalVariable.h_Channel)); 
		}		
		p = strtok(NULL, ";");
	}
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_StopFilter(gstru_GlobalVariable.h_Filter1)); 
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_StopChannel(gstru_GlobalVariable.h_Channel)); 
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_FreeFilter(gstru_GlobalVariable.h_Filter1)); 
	gstru_GlobalVariable.h_Filter1 = NULL;
	EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_FreeChannel(gstru_GlobalVariable.h_Channel));
	gstru_GlobalVariable.h_Channel = NULL;
}

//�����û���������������ֹͣ
static void TEST_SECTION_Task(void *rp_argv)
{
	while (1)	
	{
		if(CH_RTOS_WaitSemaphore(gstru_GlobalVariable.h_TestSection, CH_RTOS_TIMEOUT_INFINITY) == CH_RTOS_OK) 	//һֱ�ȴ�
		{
			break;
		}
	}
	char str_tuner_params[16];

	sprintf(str_tuner_params, "TUNER_PARAMS%d", gstru_GlobalVariable.i_CURRENT_DMXID + 1);
	request_play_program("SECTION_TEST_FILE_NAME_1", str_tuner_params, 0);
	PROMPT("����������ɺ�س�");
	test_get_input();
	
	if(gstru_GlobalVariable.h_MesSectionCome)
	{
		CH_RTOS_MessageQHandle h_Channel = 0;
		CH_RTOS_SendMessage(gstru_GlobalVariable.h_MesSectionCome, &h_Channel, 0);
	}
	gstru_GlobalVariable.i_PlayBackFinish = 1;									//����������ɱ�־��Ϊ1
	
}

CASE_F(TEST_SECTION_FILTER24,
	"SECTION���ܲ��ԣ�ͳ���յ�������section������Ԥ�ڵ�����һ��")
{
      U16 us_ChannelPid = 0;
	S32 i_FilterLen = 0;					   	//�������
	U8 uc_Data[FILTER_SIZE] = {0};
	U8 uc_Mask[FILTER_SIZE] = {0};
	U8 uc_Pattern[FILTER_SIZE] = {0};
	CH_BOOL b_NotMatchMode = CH_FALSE;
	CMD_PARAM value;						   //����parse_key()���ص��ַ���
	U8 *uc_DataBuf = NULL;			
	S32 i_DataLen = 0;					     //��ȡ�������ݳ���
	S32 i_Result = 0;	
	CH_RTOS_TaskHandle h_TestSectionTask = NULL;
	vector <CHDRV_SECTION_ChannelHandle> hChanelArry;
	vector <CHDRV_SECTION_FilterHandle> hFilterArry;
	CHDRV_SECTION_ChannelHandle h_Channel;
	CHDRV_SECTION_FilterHandle h_Filter;
	int i_performance = 0, i_group = 0, i;
		CHDRV_SECTION_AllocParam_t gstru_AllocParam;
	char *p = NULL;	
	S32 i_BufferSize = 0;
	CH_BOOL bCrc = TRUE;
	String c_string;
	
	i_BufferSize = fw_getenv_i("SECTION_TEST_BUFFERSIZE_1");		//��env��ȡbuffersize
	EXPECT_EQ(CH_RTOS_OK, CH_RTOS_CreateSemaphore(0, &gstru_GlobalVariable.h_TestSection));
	EXPECT_EQ(CH_RTOS_OK, CH_RTOS_CreateMessageQuene(sizeof(h_Channel), 60, &gstru_GlobalVariable.h_MesSectionCome));
	gstru_AllocParam.enm_SectionSize = (CHDRV_SECTION_SECTION_SIZE_e)fw_getenv_i("SECTION_TEST_SECTIONSIZE");
	gstru_AllocParam.enm_FilterSize = (CHDRV_SECTION_FILTER_SIZE_e)fw_getenv_i("SECTION_TEST_FILTERSIZE");
	gstru_AllocParam.enm_MatchMode = (CHDRV_SECTION_MATCH_MODE_e)fw_getenv_i("SECTION_TEST_MATCHMODE");

	uc_DataBuf = new U8[4096];

	LOG("i_CURRENT_DMXID=%d\n", (gstru_GlobalVariable.i_CURRENT_DMXID * 2 + (gstru_GlobalVariable.i_CURRENT_DMXID ? 1 : 0)));
	for(i_performance = 1; ; i_performance++)
	{
		c_string = c_string.Format("TEST_SECTION_PERFOMANCE_%d" , i_performance);
		LOG("c_string=%s\n", c_string.c_str());
		p = fw_getenv(c_string.c_str());
		if(!p)
		{
			break;
		}

		p = strtok(p, ";");
		
		for(i_group = 0; p != NULL;i_group++)
		{

			EXPECT_EQ(CHDRV_SECTION_OK, Get_FilterParm_FromStrings(p, uc_Data, uc_Mask, uc_Pattern, &i_FilterLen, &b_NotMatchMode, &us_ChannelPid, &bCrc)); 

			if(CHDRV_SECTION_FindChannelByPID(us_ChannelPid, &h_Channel) != CHDRV_SECTION_OK)
			{
				EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_AllocChannel((gstru_GlobalVariable.i_CURRENT_DMXID * 2 + (gstru_GlobalVariable.i_CURRENT_DMXID ? 1 : 0)), us_ChannelPid,
					TEST_SECTION_AllocChannel_CallBack, &h_Channel, i_BufferSize)); 
				hChanelArry.push_back(h_Channel);
				LOG("new pid %#x handle %#x have be requested!\n" , us_ChannelPid, h_Channel);
			}
			else
			{
				LOG("pid %#x handle %#x is be already requested!\n" , us_ChannelPid, h_Channel);
			}
			memset(&gstru_AllocParam, 0, sizeof(gstru_AllocParam));
			gstru_AllocParam.h_Channel = h_Channel;

			EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_AllocFilter((gstru_GlobalVariable.i_CURRENT_DMXID * 2 + (gstru_GlobalVariable.i_CURRENT_DMXID ? 1 : 0)), &gstru_AllocParam, &h_Filter)); 
			EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_SetChannelPid(h_Channel, us_ChannelPid));	//����pid
			EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_ModifyChannelCrcControl(h_Channel, bCrc));
			EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_SetFilter(h_Filter, uc_Data, uc_Mask, uc_Pattern, i_FilterLen, b_NotMatchMode));	//���ù�������
			hFilterArry.push_back(h_Filter);
			p = strtok(NULL, ";");
		}
		
		gstru_GlobalVariable.i_PlayBackFinish = 0;
		EXPECT_EQ(CH_RTOS_OK, CH_RTOS_CreateTask(TEST_SECTION_Task, 
			NULL, NULL, TEST_SSECTION_TASK_STACK_SIZE, TEST_SSECTION_TASK_PRIORITY,
			(S8 *)"Test section task",&h_TestSectionTask));	//�½�һ�����������û���������������ֹͣ
		
		
		PROMPT("��ֹͣ��������,������Ϊֻ����һ��,���ú��Ժ�س�");
		test_get_input();
		gstru_GlobalVariable.i_Section_Nums = 0;
		

		for(i = 0; i < hFilterArry.size(); i++)
		{
			EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_StartFilter(hFilterArry[i])); 
		}
		for(i = 0; i < hChanelArry.size(); i++)
		{
			EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_StartChannel(hChanelArry[i]));
		}
		CH_RTOS_SignalSemaphore(gstru_GlobalVariable.h_TestSection);
		while(1)
		{
			i_Result = CH_RTOS_ReceiveMessage(gstru_GlobalVariable.h_MesSectionCome, &h_Channel, 300000);		//�ȴ�callback������ź�
			if((i_Result != CH_RTOS_OK) || (gstru_GlobalVariable.i_PlayBackFinish == 1))				//����������ɺ�س�����,���س���CH_RTOS_WaitSemaphoreʱ�����
			{
				if(gstru_GlobalVariable.i_PlayBackFinish != 1)										//û�����ݵ���
				{
					LOG("timeout!!!can not get section[%s]\n", p);
				}
				break;
			}
			i_DataLen = 4096;
			i_Result = CHDRV_SECTION_ReadFilterData(h_Channel, uc_DataBuf, &i_DataLen); 
			
			while(i_Result == CHDRV_SECTION_OK && i_DataLen > 0)
			{
				gstru_GlobalVariable.i_Section_Nums++;				//ͳ���յ���section����
			
				i_DataLen = 4096;
				i_Result = CHDRV_SECTION_ReadFilterData(h_Channel, uc_DataBuf, &i_DataLen);
			}
			
		}

		LOG("i_Section_Nums=%d\n",gstru_GlobalVariable.i_Section_Nums);
		c_string = c_string.Format("TEST_SECTION_TOTAL_SECTION_NUMS_MIN_%d" , i_performance);
		EXPECT_LE(fw_getenv_i(c_string.c_str()), gstru_GlobalVariable.i_Section_Nums) << "����©��" << RUN_CMD("section.info");	//gstru_GlobalVariable.i_Section_Nums >= TEST_SECTION_TOTAL_SECTION_NUMS_MIN
		c_string = c_string.Format("TEST_SECTION_TOTAL_SECTION_NUMS_MAX_%d" , i_performance);
		EXPECT_GE(fw_getenv_i(c_string.c_str()), gstru_GlobalVariable.i_Section_Nums) << "���հ���������������ʵ�ʰ�����";	//gstru_GlobalVariable.i_Section_Nums <= TEST_SECTION_TOTAL_SECTION_NUMS_MAX

		for(i = 0; i < hFilterArry.size(); i++)
		{
			EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_StopFilter(hFilterArry[i])); 
			EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_FreeFilter(hFilterArry[i])); 
		}
		hFilterArry.clear();
		for(i = 0; i < hChanelArry.size(); i++)
		{
			EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_StopChannel(hChanelArry[i])); 
			EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_FreeChannel(hChanelArry[i])); 	
		}
		hChanelArry.clear();
		EXPECT_EQ(CH_RTOS_OK, CH_RTOS_DeleteTask(h_TestSectionTask));
		
	}
	EXPECT_EQ(CH_RTOS_OK, CH_RTOS_DeleteSemaphore(gstru_GlobalVariable.h_SectionData_Come));
	EXPECT_EQ(CH_RTOS_OK, CH_RTOS_DeleteSemaphore(gstru_GlobalVariable.h_TestSection));
	EXPECT_EQ(CH_RTOS_OK, CH_RTOS_DeleteMessageQuene(gstru_GlobalVariable.h_MesSectionCome));
	

	gstru_GlobalVariable.h_TestSection = NULL;
	gstru_GlobalVariable.h_SectionData_Come = NULL;
	gstru_GlobalVariable.enm_Event = CHDRV_SECTION_EVENT_MAX;
	gstru_GlobalVariable.i_Section_Nums = 0;
	gstru_GlobalVariable.i_PlayBackFinish = 0;
	gstru_GlobalVariable.h_MesSectionCome = NULL;
	delete[] uc_DataBuf;
}


#endif /* ENABLE_TESTCASE */

static 	vector <CHDRV_SECTION_ChannelHandle> ghChanelArry;
static 	vector <CHDRV_SECTION_FilterHandle> ghFilterArry;
static U8* gpuc_SectionData = NULL;
static void SECTION_CallForCmd(CHDRV_SECTION_ChannelHandle rh_Channel,
				 CHDRV_SECTION_EVENT_e renm_Event,CHDRV_SECTION_CallBackDataParam_t *rpstru_Param)
{	
    if(rh_Channel == NULL || renm_Event < SECTION_EVENT_MIN || 
		renm_Event > SECTION_EVENT_MAX || rpstru_Param == NULL)
    {
	LOG("TEST_SECTION_AllocChannel_CallBack has invalid param\n");
    	return;
    }	
	
	S32 ret = 0, len= 4096;
	if(renm_Event == CHDRV_SECTION_EVENT_DATA_DETECT)
	{
		if(gpuc_SectionData)
		{
			ret = CHDRV_SECTION_ReadFilterData(rh_Channel, gpuc_SectionData, &len);
		}
		if(ret != CHDRV_SECTION_OK)
		{
			LOG("read data fail,ret=%d\n", ret);
		}
		else
		{
			gstru_GlobalVariable.i_Section_Nums ++;
			LOG("%d. len=%d, data=[%02x %02x %02x %02x %02x %02x %02x %02x]\n",
				gstru_GlobalVariable.i_Section_Nums,
				len,
				gpuc_SectionData[0],
				gpuc_SectionData[1],
				gpuc_SectionData[2],
				gpuc_SectionData[3],
				gpuc_SectionData[4],
				gpuc_SectionData[5],
				gpuc_SectionData[6],
				gpuc_SectionData[7]);
		}
	}
}



static int section_start(int argc, CMD_PARAM argv[])
{
      U16 us_ChannelPid = 0;
	S32 i_FilterLen = 0;					   	//�������
	U8 uc_Data[FILTER_SIZE] = {0};
	U8 uc_Mask[FILTER_SIZE] = {0};
	U8 uc_Pattern[FILTER_SIZE] = {0};
	CH_BOOL b_NotMatchMode = CH_FALSE;
	CMD_PARAM value;						   //����parse_key()���ص��ַ���
	CHDRV_SECTION_ChannelHandle h_Channel;
	CHDRV_SECTION_FilterHandle h_Filter;
	int  i_group = 0, i;
	CHDRV_SECTION_AllocParam_t gstru_AllocParam;
	char *p = NULL;	
	S32 i_BufferSize = 0;
	CH_BOOL bCrc = TRUE;
	int device = 0;

	if(ghFilterArry.size() > 0)
	{
		LOG("����ֹͣ��ǰ��channel\n");
		return -1;
		
	}
	memset(&gstru_AllocParam, 0, sizeof(gstru_AllocParam));
	i_BufferSize = fw_getenv_i("SECTION_TEST_BUFFERSIZE_1");		//��env��ȡbuffersize
	gstru_AllocParam.enm_SectionSize = (CHDRV_SECTION_SECTION_SIZE_e)fw_getenv_i("SECTION_TEST_SECTIONSIZE");
	gstru_AllocParam.enm_FilterSize = (CHDRV_SECTION_FILTER_SIZE_e)fw_getenv_i("SECTION_TEST_FILTERSIZE");
	gstru_AllocParam.enm_MatchMode = (CHDRV_SECTION_MATCH_MODE_e)fw_getenv_i("SECTION_TEST_MATCHMODE");


	p = (char*)argv[1].get_str();
	p = strtok(p, ";");
	
	for(i_group = 0; p != NULL;i_group++)
	{
		char str[256] = " ";	
		EXPECT_EQ(CHDRV_SECTION_OK, Get_FilterParm_FromStrings(p, uc_Data, uc_Mask, uc_Pattern, &i_FilterLen, &b_NotMatchMode, &us_ChannelPid, &bCrc)); 
		value = Parse_String(p, "device", str, sizeof(str));
		device = value.get_i();
		if(CHDRV_SECTION_FindChannelByPID(us_ChannelPid, &h_Channel) != CHDRV_SECTION_OK)
		{
			EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_AllocChannel(device, us_ChannelPid,
				SECTION_CallForCmd, &h_Channel, i_BufferSize)); 
			ghChanelArry.push_back(h_Channel);
			LOG("new pid %#x handle %#x have be requested!\n" , us_ChannelPid, h_Channel);
		}
		else
		{
			LOG("pid %#x handle %#x is be already requested!\n" , us_ChannelPid, h_Channel);
		}
		memset(&gstru_AllocParam, 0, sizeof(gstru_AllocParam));
		gstru_AllocParam.h_Channel = h_Channel;

		EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_AllocFilter(device, &gstru_AllocParam, &h_Filter)); 
		EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_SetChannelPid(h_Channel, us_ChannelPid));	//����pid
		EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_ModifyChannelCrcControl(h_Channel, bCrc));
		EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_SetFilter(h_Filter, uc_Data, uc_Mask, uc_Pattern, i_FilterLen, b_NotMatchMode));	//���ù�������
		ghFilterArry.push_back(h_Filter);
		p = strtok(NULL, ";");
	}
		
		gstru_GlobalVariable.i_Section_Nums = 0;
		
		gpuc_SectionData = (U8*)CH_RTOS_AllocateMemory(gh_APP_HEAP, 4096);
		for(i = 0; i < ghChanelArry.size(); i++)
		{
			EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_StartChannel(ghChanelArry[i]));
		}
		for(i = 0; i < ghFilterArry.size(); i++)
		{
			EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_StartFilter(ghFilterArry[i])); 
		}
		
	return 0;
}

static int section_stop(int argc, CMD_PARAM argv[])
{
	int i;
	for(i = 0; i < ghFilterArry.size(); i++)
	{
		EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_StopFilter(ghFilterArry[i])); 
		EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_FreeFilter(ghFilterArry[i])); 
	}
	ghFilterArry.clear();
	for(i = 0; i < ghChanelArry.size(); i++)
	{
		EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_StopChannel(ghChanelArry[i])); 
		EXPECT_EQ(CHDRV_SECTION_OK, CHDRV_SECTION_FreeChannel(ghChanelArry[i])); 	
	}
	ghChanelArry.clear();
	if(gpuc_SectionData != NULL)
	{
		CH_RTOS_FreeMemory(gh_APP_HEAP, gpuc_SectionData);
		gpuc_SectionData = NULL;
	}
	gstru_GlobalVariable.i_Section_Nums = 0;
	return 0;
}

int section_info(int argc, CMD_PARAM argv[]);

REGISTER_CMD(section_start, "����һ������section����", "params",  "device=0,pid=0x12,filterlen=1,notmatchmode=0,data=00,mask=00,pattern=00,crc=true;device=0,pid=0,filterlen=1,notmatchmode=0,data=00,mask=ff,pattern=00,crc=true");
REGISTER_CMD(section_stop, "ֹͣ���ݽ���", "",  "");
REGISTER_CMD(section_info, "��ӡ���е�filter״̬", "",  "");




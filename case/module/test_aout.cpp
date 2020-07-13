/**
@file test_aout.cpp
@def group aout���Լ�

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
#define CASE_NAME TestAout


#include "test_menu.h"
#include "test_modules.h"

//����������
#define  ERROR_DEVICEID  						(-2)
#define  ERROR_LEFTVOLUME  						(-2)
#define  ERROR_RIGHTVOLUME 						(-2)
#define  ERROR_ENM_SOUNDCHANNEL 				(-2)


//����Ϸ�����
#define  NORMAL_DEVIDEID 						(0)
#define  NORMAL_LEFTVOLUME  					(30)
#define  NORMAL_RIGHTVOLUME  					(30)
#define  NORMAL_ENM_SOUNDCHANNEL  				(1)

#define  DEVICE_CVBS								(0)
#define  DEVICE_SPDIF								(1)
#define  DEVICE_HDMI								(2)
#define MAX_DEVICE_ID					3

//��������1 ������5 ��С����仯
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


//����ȫ�ֱ���
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

									//���Ž�Ŀ
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

	RUN_CMD("av.stop 0");			//ֹͣ����
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

//����CHDRV_AOUT_SetMute��������ȡ������
//rb_MuteOnOff : 
// CH_TRUE --> ���Ծ���״̬ 
// CH_FALSE --> ����ȡ������״̬
static CHDRV_AOUT_RESULT_e  TEST_CHDRV_AOUT_SetMute(CH_BOOL rb_MuteOnOff)
{

	 int i_DeviceID = 0;

	CH_BOOL b_MuteOnOff = CH_FALSE;	

	for(i_DeviceID = 0; i_DeviceID < MAX_DEVICE_ID; i_DeviceID++)
	{
		if( i_DeviceID == DEVICE_SPDIF && fw_getenv_i("SUPPORT_SPDIF") == 1)
		{
			PROMPT("������SPDIF,ȷ��SPDIFͨ���Ƿ���%s״̬,��������Y����������N:", rb_MuteOnOff ? "����": "�Ǿ���");	
		}
		else if(i_DeviceID == DEVICE_HDMI && fw_getenv_i("SUPPORT_HDMI") == 1)
		{
			PROMPT("������HDMI,���ѵ�������ΪHDMI����,ȷ��HDMIͨ���Ƿ���%s״̬,��������Y����������N:", rb_MuteOnOff ? "����": "�Ǿ���");	
		}
		else if(i_DeviceID == DEVICE_CVBS && fw_getenv_i("SUPPORT_CVBS") == 1)
		{
			PROMPT("������CVBS,���ѵ�������ΪCVBS����,ȷ��LRͨ���Ƿ���%s״̬,��������Y����������N:", rb_MuteOnOff ? "����": "�Ǿ���");	
		}
		else
		{
			continue;
		}
		EXPECT_EQ(CHDRV_AOUT_OK, CHDRV_AOUT_SetMute(gh_SoundHandle[i_DeviceID], rb_MuteOnOff));

			
		EXPECT_EQ(CH_TRUE,  test_input_yes()); 		//�жϾ���״̬�Ƿ����óɹ�
		EXPECT_EQ(CHDRV_AOUT_OK, CHDRV_AOUT_GetMuteStatus(gh_SoundHandle[i_DeviceID], &b_MuteOnOff));
		EXPECT_EQ(rb_MuteOnOff, b_MuteOnOff);		//��ȡ����״̬Ӧ�����õ�״̬һ��
	}
	return CHDRV_AOUT_OK;		
}
//����CHDRV_AOUT_SetSoundChannelMode����ģ������
static CHDRV_AOUT_RESULT_e  TEST_CHDRV_AOUT_SetSoundChannelMode(int device_id, CHDRV_AOUT_SOUND_CHANNEL_e renm_SoundChannel)
{
	const char *pstr_DeviceName[MAX_DEVICE_ID] = {"CVBS", "SPDIF", "HDMI"};
	if(renm_SoundChannel >= CHDRV_AOUT_SOUND_CHANNEL_MAX)
	{
		LOG("TEST_CHDRV_AOUT_SetSoundChannelMode INVALID_PARAM\n");	
		return CHDRV_AOUT_INVALID_PARAM;
	}
	EXPECT_EQ(CHDRV_AOUT_OK, CHDRV_AOUT_SetSoundChannelMode(gh_SoundHandle[device_id], renm_SoundChannel));		//����Ϊ������
	
	if(renm_SoundChannel == CHDRV_AOUT_SOUND_LEFT_TO_BOTH)
	{
		PROMPT("��ȷ�ϵ�ǰ%s��������Ƿ�Ϊ����������������Y����������N:", pstr_DeviceName[device_id]);	
	}
	else if(renm_SoundChannel == CHDRV_AOUT_SOUND_RIGHT_TO_BOTH)
	{
		PROMPT("��ȷ�ϵ�ǰ%s��������Ƿ�Ϊ����������������Y����������N:", pstr_DeviceName[device_id]);	
	}
	else if(renm_SoundChannel == CHDRV_AOUT_SOUND_STEREO)
	{
		PROMPT("��ȷ�ϵ�ǰ%s��������Ƿ�Ϊ������������������Y����������N:", pstr_DeviceName[device_id]);	
	}
	else
	{
		
	}

	
	EXPECT_EQ(CH_TRUE, test_input_yes()) << "����Ա��Ϊ"<< pstr_DeviceName[device_id] << "��������δ������";			//����Ա�������������Ӧ�����õ�ģ������һ��

	return CHDRV_AOUT_OK;
}

//����SPDIF��HDMIѹ���ͷ�ѹ��ģʽ
//rb_SetOutMode     : CH_TRUE : HDMI			CH_FALSE : SPDIF
//rb_CompressMode : CH_TRUE : ѹ��ģʽ	CH_FALSE : ��ѹ��ģʽ
static CHDRV_AOUT_RESULT_e  TEST_CHDRV_AOUT_SetSPDIF_HDMIOutMode(int ri_deviceId, CH_BOOL rb_CompressMode)
{
	
	EXPECT_EQ(CHDRV_AOUT_OK, CHDRV_AOUT_SetSPDIFOutMode(rb_CompressMode));				//����SPDIFOutMode


	PROMPT("��ȷ��%s�����ʽ�Ƿ�Ϊ%s����������Y����������N:", 
		ri_deviceId == DEVICE_HDMI ? "HDMI" : "SPDIF",
		rb_CompressMode ? "dolby" : "pcm");
	


	EXPECT_EQ(CH_TRUE, test_input_yes()) << (ri_deviceId == DEVICE_HDMI ? "HDMI" : "SPDIF") << "���" << (rb_CompressMode ? "ѹ��" : "��ѹ��") << "ģʽ������Ԥ��";

	return CHDRV_AOUT_OK;
}


CASE_F(TEST_AOUT_INIT1,
	"���Զ�γ�ʼ��ʱ�Ƿ񷵻�OK") {
	EXPECT_EQ(CHDRV_AOUT_OK, CHDRV_AOUT_Init());
	EXPECT_EQ(CHDRV_AOUT_OK, CHDRV_AOUT_Init()) << "��γ�ʼ��Ӧ�÷���OK";
}


CASE_F(TEST_AOUT_INIT2,
	"����ģ���Ƿ�ɱ�����") {
	EXPECT_EQ(CHDRV_AOUT_OK, CHDRV_AOUT_Term());
	EXPECT_EQ(CHDRV_AOUT_OK, CHDRV_AOUT_Init());
}


CASE_F(TEST_AOUT_INIT3,
	"����CHDRV_AOUT_GetVersion�Ƿ�ɹ�") {
	CHDRV_Version_t stru_Ver;
	
	memset(&stru_Ver, 0, sizeof(stru_Ver));
	EXPECT_EQ(CHDRV_AOUT_OK, CHDRV_AOUT_GetVersion(&stru_Ver));
	EXPECT_NE(0, stru_Ver);
	LOG("\nstru_Ver=%x\n", stru_Ver);
}

CASE_F(TEST_AOUT_PARAMS1,
	"����CHDRV_AOUT_Open�����������")
{
	CHDRV_AOUT_DeviceHandle h_SoundHandle;
	
	EXPECT_EQ(CHDRV_AOUT_INVALID_PARAM, CHDRV_AOUT_Open(ERROR_DEVICEID, &h_SoundHandle));
//	EXPECT_EQ(CHDRV_AOUT_INVALID_PARAM, CHDRV_AOUT_Open(NORMAL_DEVIDEID, NULL));
//CHDRV_AOUT_Open ����û�жԴ�������жϣ���ָ�봫��ȥ�ᵼ������
}

CASE_F(TEST_AOUT_PARAMS2,
	"����CHDRV_AOUT_SetMute��CHDRV_AOUT_GetMuteStatus�������")
{
	CHDRV_AOUT_DeviceHandle h_SoundHandle = NULL;
	CH_BOOL b_MuteOnOff = CH_TRUE;
	
	EXPECT_EQ(CHDRV_AOUT_INVALID_PARAM, CHDRV_AOUT_SetMute(NULL, b_MuteOnOff));
	EXPECT_EQ(CHDRV_AOUT_INVALID_PARAM, CHDRV_AOUT_GetMuteStatus(NULL, &b_MuteOnOff));
	EXPECT_EQ(CHDRV_AOUT_INVALID_PARAM, CHDRV_AOUT_GetMuteStatus(h_SoundHandle, NULL));
}

CASE_F(TEST_AOUT_PARAMS3,
	"����CHDRV_AOUT_SetVolume��GetVolume�������")
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
	"����CHDRV_AOUT_SetSoundChannelMode��GetSoundChannelMode�������")
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
	"����CHDRV_AOUT_SetSPDIFOutMode��CHDRV_AOUT_SetHDMIOutMode�������")
{
	CH_BOOL b_CompressMode = CH_FALSE;

	EXPECT_EQ(CHDRV_AOUT_INVALID_PARAM, CHDRV_AOUT_SetSPDIFOutMode(b_CompressMode));
	EXPECT_EQ(CHDRV_AOUT_INVALID_PARAM, CHDRV_AOUT_SetHDMIOutMode(b_CompressMode));
}
#endif

CASE_F(TEST_AOUT_PARAMS6,
	"����CHDRV_AOUT_Close�������")
{
	EXPECT_EQ(CHDRV_AOUT_INVALID_PARAM, CHDRV_AOUT_Close(NULL));
}

CASE_F(TEST_AOUT_OPEN,
	"����CHDRV_AOUT_Open��ȷ�����򿪣�����ֵӦ��ȷ��handle��ΪNULL")
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
	"���Ծ�������ȡ������,��Ӧ�˿�Ӧ�ܹر����;CHDRV_AOUT_GetMuteStatus�ܷ���ȷ���ؾ���״̬")
{
	EXPECT_EQ(CHDRV_AOUT_OK, TEST_CHDRV_AOUT_SetMute(CH_TRUE));				//���Ծ���״̬								//������Ƶ���ͨ��ΪDVBS
	EXPECT_EQ(CHDRV_AOUT_OK, TEST_CHDRV_AOUT_SetMute(CH_FALSE));			//����ȡ������״̬										//������Ƶ���ͨ��ΪDVBS
}

CASE_F(TEST_AOUT_VOLUME,
	"�������ò�ͬ������CVBS�˿������������仯,�����������ı�ԭ���趨ֵ;GetVolume��ȡ��ǰ������ȷ")
{	
	S8 c_LeftVolume = 0;
	S8 c_RightVolume = 0;
	 int i_DeviceID = 0;

	 for(i_DeviceID = 0; i_DeviceID < MAX_DEVICE_ID; i_DeviceID++)
	 {
		if((i_DeviceID == DEVICE_CVBS && fw_getenv_i("SUPPORT_CVBS") == 1)
			|| (i_DeviceID == DEVICE_HDMI && fw_getenv_i("SUPPORT_HDMI") == 1))
		{
		
			PROMPT("������%s,���ѵ����л���%s����,Ȼ��س�", DEVICE_CVBS == i_DeviceID ? "CVBS" : "HDMI",
				DEVICE_CVBS == i_DeviceID ? "CVBS" : "HDMI");	
			test_get_input();
			
			PROMPT("����ϸ������,Ȼ��ȷ����������Ƿ��С����仯��׼�����Ժ󰴻س�");
			test_input_none();
			EXPECT_EQ(CHDRV_AOUT_OK, CHDRV_AOUT_SetVolume(gh_SoundHandle[i_DeviceID], LeftVolume1, RightVolume1));		//��������1
			CH_RTOS_TaskDelay(1000);  
			EXPECT_EQ(CHDRV_AOUT_OK, CHDRV_AOUT_SetVolume(gh_SoundHandle[i_DeviceID], LeftVolume2, RightVolume2));		//��������1
			CH_RTOS_TaskDelay(1000);  
			EXPECT_EQ(CHDRV_AOUT_OK, CHDRV_AOUT_SetVolume(gh_SoundHandle[i_DeviceID], LeftVolume3, RightVolume3));		//��������2
			CH_RTOS_TaskDelay(1000);  
			EXPECT_EQ(CHDRV_AOUT_OK, CHDRV_AOUT_SetVolume(gh_SoundHandle[i_DeviceID], LeftVolume4, RightVolume4));		//��������2
			CH_RTOS_TaskDelay(1000);  
			EXPECT_EQ(CHDRV_AOUT_OK, CHDRV_AOUT_SetVolume(gh_SoundHandle[i_DeviceID], LeftVolume5, RightVolume5));		//��������3

			PROMPT("��ȷ����������Ƿ��С����仯����������Y����������N:");
			

			EXPECT_EQ(CH_TRUE, test_input_yes()) << "����Ա��Ϊ�������Ʋ�������";								//����Ա�����������仯Ӧ�ø����õ�һ��(��С����)
			EXPECT_EQ(CHDRV_AOUT_INVALID_PARAM, CHDRV_AOUT_SetVolume(gh_SoundHandle, ERROR_LEFTVOLUME, ERROR_RIGHTVOLUME));	//���ô������
			EXPECT_EQ(CHDRV_AOUT_OK, CHDRV_AOUT_GetVolume(gh_SoundHandle, &c_LeftVolume, &c_RightVolume));			//��ȡ��������
			EXPECT_EQ(LeftVolume5, c_LeftVolume) << "��ȡ�����������õĲ�һ��";					//��ȡ��������Ӧ�����õ�һ��,100 * LeftVolume3/64 �Ǹ���CH_PLATFORM_AOUT_Volume_ch2mstar ���ԭ��������	
			EXPECT_EQ(RightVolume5, c_RightVolume) << "��ȡ�����������õĲ�һ��";				//��ȡ��������Ӧ�����õ�һ��,100 * RightVolume3/64 �Ǹ���CH_PLATFORM_AOUT_Volume_ch2mstar ���ԭ��������	
		}
	}
}


CASE_F(TEST_AOUT_CHANNEL,
	"����SetSoundChannelMode��ͬ�����ܹ���ȷ��Ӧ,�����������ı�ԭ���趨ֵ;GetSoundChannelMode��ȡ������ȷ")
{	
	CHDRV_AOUT_SOUND_CHANNEL_e enm_GetSoundChannel;
	 int i_DeviceID = 0;

	 for(i_DeviceID = 0; i_DeviceID < MAX_DEVICE_ID; i_DeviceID++)
	 {
		if((i_DeviceID == DEVICE_CVBS && fw_getenv_i("SUPPORT_CVBS") == 1)
			|| (i_DeviceID == DEVICE_HDMI && fw_getenv_i("SUPPORT_HDMI") == 1))
		{
			EXPECT_EQ(CHDRV_AOUT_OK, TEST_CHDRV_AOUT_SetSoundChannelMode(i_DeviceID, CHDRV_AOUT_SOUND_LEFT_TO_BOTH));							//����������
			EXPECT_EQ(CHDRV_AOUT_OK, TEST_CHDRV_AOUT_SetSoundChannelMode(i_DeviceID, CHDRV_AOUT_SOUND_RIGHT_TO_BOTH));							//����������
			EXPECT_EQ(CHDRV_AOUT_OK, TEST_CHDRV_AOUT_SetSoundChannelMode(i_DeviceID, CHDRV_AOUT_SOUND_STEREO));									//������������
			EXPECT_EQ(CHDRV_AOUT_INVALID_PARAM, CHDRV_AOUT_SetSoundChannelMode(gh_SoundHandle[i_DeviceID], CHDRV_AOUT_SOUND_CHANNEL_MAX));		//���ô������
			EXPECT_EQ(CHDRV_AOUT_OK, CHDRV_AOUT_GetSoundChannelMode(gh_SoundHandle[i_DeviceID], &enm_GetSoundChannel));							//��ȡ���õ�ģ����������
			EXPECT_EQ(CHDRV_AOUT_SOUND_STEREO, enm_GetSoundChannel) << "��ȡ����������Ӧ��������õ�����������һ��";
		}
	}
}



CASE_F(TEST_AOUT_SPDIF,
	"����CHDRV_AOUT_SetSPDIFOutMode����Ϊѹ���ͷ�ѹ����ʽ���˿�Ӧ��ȷ���������ֵ��ȷ")
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
		PROMPT("���ÿ�����ʾSPDIF�����ʽ���豸����SPDIF,׼�����Ժ�س�");	
		test_get_input();
		EXPECT_EQ(CHDRV_AOUT_OK, TEST_CHDRV_AOUT_SetSPDIF_HDMIOutMode(DEVICE_SPDIF, CH_TRUE));			//����SPDIFѹ����ʽ
		EXPECT_EQ(CHDRV_AOUT_OK, TEST_CHDRV_AOUT_SetSPDIF_HDMIOutMode(DEVICE_SPDIF, CH_FALSE));			//����SPDIF��ѹ����ʽ
	}
}


CASE_F(TEST_AOUT_HDMIOUT,
	"����CHDRV_AOUT_SetHDMIOutMode����Ϊѹ���ͷ�ѹ����ʽ���˿�Ӧ����ȷ���������ֵ��ȷ")
{		
	if(fw_getenv_i("SUPPORT_HDMI") == 1)
	{
		PROMPT("���ÿ�����ʾHDMI��Ƶ�����ʽ���豸����HDMI,׼�����Ժ�س�");	
		test_get_input();
		EXPECT_EQ(CHDRV_AOUT_OK, TEST_CHDRV_AOUT_SetSPDIF_HDMIOutMode(DEVICE_HDMI, CH_TRUE));			//����HDMIѹ����ʽ
		EXPECT_EQ(CHDRV_AOUT_OK, TEST_CHDRV_AOUT_SetSPDIF_HDMIOutMode(DEVICE_HDMI, CH_FALSE));			//����HDMI��ѹ����ʽ
	}
}

CASE_F(TEST_AOUT_CLOSE,
	"����CHDRV_AOUT_Closeɾ�����������ֵ��ȷ")
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

REGISTER_CMD(aout_info, "��ʾaout��Ϣ", "",  "");
REGISTER_CMD(aout_open, "����aout���", "device_id",  "0");
REGISTER_CMD(aout_close, "ɾ��aout���", "device_id",  "0");
REGISTER_CMD(aout_mute, "���������豸Ϊ����", "flag",  "false");
REGISTER_CMD(aout_volume, "���������豸������", "vol",  "63");
REGISTER_CMD(aout_mode, "���������豸��ģʽ", "mode(0~4)",  "4");






/**
@file test_avfilter.cpp
@def group avfilter���Լ�

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
#define CASE_NAME TestAvFilter


#include "test_menu.h"
#include "test_modules.h"
#include "psi_parse.h"

extern int avfilter_info(int argc, CMD_PARAM argv[]);

void CASE_NAME::SetUpTestCase() {
//excute before each test suit
//std::cout << "init SetUp rtos TestCase " << std::endl;
CHDRV_TUNER_Break(0);

}
void CASE_NAME::TearDownTestCase() {
//excute after each test suit
//std::cout << "deinit tuner rtos TestCase " << std::endl;
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


#if ENABLE_TESTCASE

CASE_F(TEST_AVFILTER_INIT1,
	"���Զ�γ�ʼ��ʱ�Ƿ񷵻�OK") {
	EXPECT_EQ(CHDRV_AVFILTER_OK, CHDRV_AVFILTER_Init());
	EXPECT_EQ(CHDRV_AVFILTER_OK, CHDRV_AVFILTER_Init()) << "��γ�ʼ��Ӧ�÷���OK";
}




CASE_F(TEST_AVFILTER_INIT2,
	"����ģ���Ƿ�ɱ�����") {
	EXPECT_EQ(CHDRV_AVFILTER_OK, CHDRV_AVFILTER_Term());
	EXPECT_EQ(CHDRV_AVFILTER_OK, CHDRV_AVFILTER_Init());
}


CASE_F(TEST_AVFILTER_INIT3,
	"����CHDRV_AVFILTER_GetVersion�Ƿ�ɹ�") {
	CHDRV_Version_t stru_Ver;
	
	memset(&stru_Ver, 0, sizeof(stru_Ver));
	EXPECT_EQ(CHDRV_AVFILTER_OK, CHDRV_AVFILTER_GetVersion(&stru_Ver));
	EXPECT_NE(0, stru_Ver);
	LOG("\nstru_Ver=%x\n", stru_Ver);
}

CASE_F(TEST_AVFILTER_ALLOC,
	"�ֱ����audio��video��pcr���͵�filter����Ӧ���سɹ�") {
	CHDRV_AVFILTER_Handle handle = 0;
	int i_demux;

	for(i_demux = 0; i_demux < fw_getenv_i("AVFILTER_TEST_DMX_NUMS");i_demux++)
	{
		EXPECT_EQ(CHDRV_AVFILTER_OK, CHDRV_AVFILTER_AllocateFilter(i_demux,
			0, 
			CHDRV_AVFILTER_AUDIO, 
			&handle));
		EXPECT_NE(handle, (void*)NULL);


		EXPECT_EQ(CHDRV_AVFILTER_OK, CHDRV_AVFILTER_AllocateFilter(i_demux,
			0, 
			CHDRV_AVFILTER_VIDEO, 
			&handle));
		EXPECT_NE(handle, (void*)NULL);


		EXPECT_EQ(CHDRV_AVFILTER_OK, CHDRV_AVFILTER_AllocateFilter(i_demux,
			0, 
			CHDRV_AVFILTER_PCR, 
			&handle));
		EXPECT_NE(handle, (void*)NULL);
	}
	
	
}


CASE_F(TEST_AVFILTER_STATUS1,
	"����get status��״̬ӦΪIDLE") {
	CHDRV_AVFILTER_STATUS_e enm_Status;
	int i;
	
	for(i = 0; i < avfilter_GetHandleNums() && i < 2;i++)
	{
		EXPECT_EQ(CHDRV_AVFILTER_OK, CHDRV_AVFILTER_GetStatus(avfilter_GetHandle(i),
			&enm_Status));
		EXPECT_EQ(enm_Status, CHDRV_AVFILTER_STATUS_IDLE);
	}
}

CASE_F(TEST_AVFILTER_SETPID,
	"�ֱ�Ϊ����filter����pid��pid��ֵԤ�ȹ̶�") {
	int i;
	
	for(i = 0; i < avfilter_GetHandleNums();i++)
	{
		EXPECT_EQ(CHDRV_AVFILTER_OK, CHDRV_AVFILTER_SetPid(avfilter_GetHandle(i),
			101 + i));
	}
}

CASE_F(TEST_AVFILTER_GETPID,
	"����get pid����õ�ֵӦ��set��ֵһ��") {
	U16 us_pid = 0;
	int i;
	
	for(i = 0; i < avfilter_GetHandleNums();i++)
	{
		EXPECT_EQ(CHDRV_AVFILTER_OK, CHDRV_AVFILTER_GetPid(avfilter_GetHandle(i),
			&us_pid));
		EXPECT_EQ(us_pid, 101 + i);
	}
}

CASE_F(TEST_AVFILTER_ENABLEFILTER,
	"�ֱ�enable����3��filter��Ӧ����OK") {
	int i;
	
	for(i = 0; i < avfilter_GetHandleNums();i++)
	{
		EXPECT_EQ(CHDRV_AVFILTER_OK, CHDRV_AVFILTER_EnableFilter(avfilter_GetHandle(i)));
	}
}

CASE_F(TEST_AVFILTER_STATUS2,
	"����get status��״̬ӦΪNO DATA") {
	CHDRV_AVFILTER_STATUS_e enm_Status;
	int i;
	
	for(i = 0; i < avfilter_GetHandleNums() && i < 2;i++)
	{
		EXPECT_EQ(CHDRV_AVFILTER_OK, CHDRV_AVFILTER_GetStatus(avfilter_GetHandle(i),
			&enm_Status));
		EXPECT_EQ(enm_Status, CHDRV_AVFILTER_STATUS_NO_DATA);
	}
}

CASE_F(TEST_AVFILTER_STATUS3,
	"��Ƶ�󣬵���get status��״̬ӦΪGOOD") {
	CHDRV_AVFILTER_STATUS_e enm_Status;
	int i;
	int ret;
	vector <Channel_Info_t> prog_list;
	U16 us_apid = 0, us_vpid = 0, us_ppid = 0;
	do
	{
		PROMPT("��׼���ò����Ų�������[%s],������Ƶ��[%s]��Ȼ�󰴻س�",
		fw_getenv("SECTION_TEST_FILE_NAME_1"),
		fw_getenv("TUNER_PARAMS1"));
		test_get_input();
		
		RUN_CMD("tuner.lock %s", fw_getenv("TUNER_PARAMS1"));
		ret = prog_search(0, prog_list);
	}while(ret <= 0);

	for(i = 0; i < prog_list.size(); i++)
	{
		if(prog_list[i].pstru_Video.size() > 0 && us_vpid == 0)
		{
			us_vpid = prog_list[i].pstru_Video[0].us_PrivateEsPid;
		}
		if(prog_list[i].pstru_Audio.size() > 0 && us_apid == 0)
		{
			us_apid = prog_list[i].pstru_Audio[0].us_PrivateEsPid;
		}
		if(us_ppid == 0)
		{
			us_ppid = prog_list[i].us_PcrPid;
		}
		if(us_ppid && us_apid && us_vpid)
		{
			break;
		}
	}
	if(avfilter_GetHandleNums() > 0)
	{
		EXPECT_EQ(CHDRV_AVFILTER_OK, CHDRV_AVFILTER_DisableFilter(avfilter_GetHandle(0)));
		EXPECT_EQ(CHDRV_AVFILTER_OK, CHDRV_AVFILTER_SetPid(avfilter_GetHandle(0),
				us_apid));
		EXPECT_EQ(CHDRV_AVFILTER_OK, CHDRV_AVFILTER_EnableFilter(avfilter_GetHandle(0)));
	}
	if(avfilter_GetHandleNums() > 1)
	{
		EXPECT_EQ(CHDRV_AVFILTER_OK, CHDRV_AVFILTER_DisableFilter(avfilter_GetHandle(1)));
		EXPECT_EQ(CHDRV_AVFILTER_OK, CHDRV_AVFILTER_SetPid(avfilter_GetHandle(1),
				us_vpid));
		EXPECT_EQ(CHDRV_AVFILTER_OK, CHDRV_AVFILTER_EnableFilter(avfilter_GetHandle(1)));
	}
	if(avfilter_GetHandleNums() > 2)
	{
		EXPECT_EQ(CHDRV_AVFILTER_OK, CHDRV_AVFILTER_DisableFilter(avfilter_GetHandle(2)));
		EXPECT_EQ(CHDRV_AVFILTER_OK, CHDRV_AVFILTER_SetPid(avfilter_GetHandle(2),
				us_ppid));
		EXPECT_EQ(CHDRV_AVFILTER_OK, CHDRV_AVFILTER_EnableFilter(avfilter_GetHandle(2)));
	}
	
	for(i = 0; i < avfilter_GetHandleNums() && i < 2;i++)
	{
		EXPECT_EQ(CHDRV_AVFILTER_OK, CHDRV_AVFILTER_GetStatus(avfilter_GetHandle(i),
			&enm_Status));
		EXPECT_EQ(enm_Status, CHDRV_AVFILTER_STATUS_GOOD);
	}
}

CASE_F(TEST_AVFILTER_DISABLEFILTER,
	"�ֱ�disable����3��filter��Ӧ����OK") {
	int i;
	
	for(i = 0; i < avfilter_GetHandleNums();i++)
	{
		EXPECT_EQ(CHDRV_AVFILTER_OK, CHDRV_AVFILTER_DisableFilter(avfilter_GetHandle(i)));
	}
}

CASE_F(TEST_AVFILTER_FREE,
	"�ֱ�delete����3��filter��Ӧ����OK") {
	int i;
	
	for(i = 0; i < avfilter_GetHandleNums();i++)
	{
		EXPECT_EQ(CHDRV_AVFILTER_OK, CHDRV_AVFILTER_DeleteFilter(avfilter_GetHandle(0)));
	}

}
#endif /* ENABLE_TESTCASE */

static int avfilter_init(int argc, CMD_PARAM argv[])
{
	int ret;
	ret = CHDRV_AVFILTER_Init();
	return ret;
}

static int avfilter_term(int argc, CMD_PARAM argv[])
{
	int ret;
	ret = CHDRV_AVFILTER_Term();
	return ret;
}

static int avfilter_ver(int argc, CMD_PARAM argv[])
{
	int ret;
	U32 ver = 0;
	
	ret = CHDRV_AVFILTER_GetVersion(&ver);
	LOG("ver=%#x\n", ver);
	return ret;
}

static int avfilter_start(int argc, CMD_PARAM argv[])
{
	int ret = CHDRV_AVFILTER_OK;
	CHDRV_AVFILTER_Handle handle = 0;

	if(argv[2].get_i() >= avfilter_GetHandleNums())
	{
		ret = CHDRV_AVFILTER_AllocateFilter(argv[1].get_i(),
			0, 
			(CHDRV_AVFILTER_TYPE_e)argv[2].get_i(), 
			&handle);
	}
	else
	{
		handle = avfilter_GetHandle(argv[2].get_i());
	}
	
	
	if(ret != CHDRV_AVFILTER_OK)
	{
		LOG("CHDRV_AVFILTER_AllocateFilter return %d\n");
		if(argv[2].get_i() >= CHDRV_AVFILTER_TYPE_MAX)
		{
			LOG("type only suppport: 0(audio), 1(video), 2(pcr)\n");
		}
		return ret;
	}

	ret = CHDRV_AVFILTER_SetPid(handle, argv[3].get_i());
	if(ret != CHDRV_AVFILTER_OK)
	{
		LOG("CHDRV_AVFILTER_SetPid return %d\n");
		return ret;
	}

	ret = CHDRV_AVFILTER_EnableFilter(handle);
	if(ret != CHDRV_AVFILTER_OK)
	{
		LOG("CHDRV_AVFILTER_EnableFilter return %d\n");
		return ret;
	}

	LOG("start ok, handle no=%#x/0x%x, pid=%#x\n", avfilter_GetHandleNums(), avfilter_GetHandleNums(), argv[3].get_i());

	return ret;
}

static int avfilter_stop(int argc, CMD_PARAM argv[])
{
	int ret;
	CHDRV_AVFILTER_Handle handle = 0;
	
	
	if(argv[1].get_i() >= avfilter_GetHandleNums() || argv[1].get_i() < 0)
	{
		LOG("handle_no =%d not exist!\n", argv[1].get_i());
		return -1;
	}

	handle = avfilter_GetHandle(argv[1].get_i());
	ret = CHDRV_AVFILTER_DisableFilter(handle);
	if(ret != CHDRV_AVFILTER_OK)
	{
		LOG("CHDRV_AVFILTER_DisableFilter return %d\n");
	}
	ret = CHDRV_AVFILTER_DeleteFilter(handle);
	if(ret != CHDRV_AVFILTER_OK)
	{
		LOG("CHDRV_AVFILTER_DeleteFilter return %d\n");
	}

	LOG("stop filter %d ok\n", argv[1].get_i());

	return 0;
}

static int avfilter_set_pid(int argc, CMD_PARAM argv[])
{
	int ret;
	CHDRV_AVFILTER_Handle handle = 0;


	if(argv[1].get_i() >= avfilter_GetHandleNums() || argv[1].get_i() < 0)
	{
		LOG("handle_no =%d not exist!\n", argv[1].get_i());
		return -1;
	}

	handle = avfilter_GetHandle(argv[1].get_i());
	ret = CHDRV_AVFILTER_DisableFilter(handle);
	if(ret != CHDRV_AVFILTER_OK)
	{
		LOG("CHDRV_AVFILTER_DisableFilter return %d\n");
	}
	

	ret = CHDRV_AVFILTER_SetPid(handle, argv[2].get_i());
	if(ret != CHDRV_AVFILTER_OK)
	{
		LOG("CHDRV_AVFILTER_SetPid return %d\n");
		return ret;
	}

	ret = CHDRV_AVFILTER_EnableFilter(handle);
	if(ret != CHDRV_AVFILTER_OK)
	{
		LOG("CHDRV_AVFILTER_EnableFilter return %d\n");
		return ret;
	}

	LOG("set ok, handle no=%#x/0x%x, pid=%#x\n", argv[1].get_i(), avfilter_GetHandleNums(), argv[2].get_i());

	return ret;
}

static int avfilter_status(int argc, CMD_PARAM argv[])
{
	int i, ret;
	U16 pid;
	CHDRV_AVFILTER_STATUS_e status;

	LOG("filter number=%d\n", avfilter_GetHandleNums());

	for(i = 0; i < avfilter_GetHandleNums(); i++)
	{
		ret = CHDRV_AVFILTER_GetPid(avfilter_GetHandle(i), &pid);
		if(ret != CHDRV_AVFILTER_OK)
		{
			LOG("CHDRV_AVFILTER_GetPid %#x return %d\n", avfilter_GetHandle(i), ret);
		}
		else
		{
			LOG("handle %d. pid=%d(%#x)\n", i, pid, pid);
		}
		ret = CHDRV_AVFILTER_GetStatus(avfilter_GetHandle(i), &status);
		if(ret != CHDRV_AVFILTER_OK)
		{
			LOG("CHDRV_AVFILTER_GetStatus %#x return %d\n", avfilter_GetHandle(i), ret);
		}
		else
		{
			LOG("handle %d. status=%d\n", i, status);
		}
	}
	return 0;
}

REGISTER_CMD(avfilter_init, "��ʼ��avfilterģ��", "",  "");
REGISTER_CMD(avfilter_term, "����avfilterģ��", "",  "");
REGISTER_CMD(avfilter_ver, "��ȡavfilterģ��汾��", "",  "");
REGISTER_CMD(avfilter_status, "��ӡ����avfilter�Ĺ���״̬", "",  "");
REGISTER_CMD(avfilter_start, "������ʹ��һ��avfilter", "device_id|type|pid",  "0 0 0x28");
REGISTER_CMD(avfilter_stop, "ֹͣ��ɾ��һ��avfilter", "handle_no",  "0");
REGISTER_CMD(avfilter_set_pid, "���Ź������޸�filter��pid", "handle_no | pid",  "0 0x28");
REGISTER_CMD(avfilter_info, "���avfilter�������Ϣ����ӡ������", "",  "");



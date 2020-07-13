/**
@file test_pesfilter.cpp
@def group pesfilter���Լ�

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
#define CASE_NAME TestPesfilter


#include "test_menu.h"
#include "test_modules.h"


void CASE_NAME::SetUpTestCase() {
//excute before each test suit
//std::cout << "init SetUp rtos TestCase " << std::endl;
int prognum = 0;

prognum = request_play_program("AV_STREAM_COMMON", "TUNER_PARAMS1");
RUN_CMD("tuner.lock %s", fw_getenv("TUNER_PARAMS1"));
RUN_CMD("av.start %d %d\n",get_demux_id("TUNER_PARAMS1"), prognum);
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

static CHDRV_PESFILTER_HANDLE gh_pesHandle;
static vector <CHDRV_PESFILTER_HANDLE> gph_pesHdl;
static void *gh_TsCycleBuffer = NULL;
static int section_num = 0;
static 	U8 *gpuc_PesDataBuf = NULL;

void* cycle_buffer_init(void);
int cycle_buffer_deinit(void *pcycle_buffer);
int read_ts_packet(CHDRV_PESFILTER_HANDLE rph_PesFilter, void *pCycleBuffer);
int pop_cycle_data(void *pCycleBuffer, unsigned char *data, int length);


void  pes_call1(CHDRV_PESFILTER_HANDLE rph_PesFilter,CHDRV_PESFILTER_STATUS_e  renm_status,CHDRV_PESFILTER_CallBackDataParam_t *rpstru_Param )
{
	S32 data_length = fw_getenv_i("PES_FILTER_BUFFER_SIZE");
	if(gpuc_PesDataBuf == NULL)
	{
		gpuc_PesDataBuf = (U8 *)CH_RTOS_AllocateMemory(gh_APP_HEAP, sizeof(U8)*data_length);
	}
	if(gpuc_PesDataBuf)
	{
		if(renm_status == CHDRV_PESFILTER_DETECT)
		{	
			CHDRV_PESFILTER_ReadData(gh_pesHandle,gpuc_PesDataBuf, &data_length);
#if 0 	/*sqzou20170728*/
			LOG("%3d. [%02x %02x %02x %02x %02x %02x %02x %02x]\n",
				section_num,
				gpuc_PesDataBuf[0],
				gpuc_PesDataBuf[1],
				gpuc_PesDataBuf[2],
				gpuc_PesDataBuf[3],
				gpuc_PesDataBuf[4],
				gpuc_PesDataBuf[5],
				gpuc_PesDataBuf[6],
				gpuc_PesDataBuf[7]);
#endif
			section_num++;
			//LOG("section_num=%d\n", section_num);
		}
	}
	else
	{
		LOG("gpuc_PesDataBuf = NULL\n");
	}
}
void  pes_call2(CHDRV_PESFILTER_HANDLE rph_PesFilter,CHDRV_PESFILTER_STATUS_e  renm_status,CHDRV_PESFILTER_CallBackDataParam_t *rpstru_Param )
{
	S32 data_length = fw_getenv_i("PES_FILTER_BUFFER_SIZE");
	int ret;
	
	if(gpuc_PesDataBuf == NULL)
	{
		gpuc_PesDataBuf = (U8 *)CH_RTOS_AllocateMemory(gh_APP_HEAP, sizeof(U8)*data_length);
	}
	if(gpuc_PesDataBuf)
	{
		if(renm_status == CHDRV_PESFILTER_DETECT)
		{	
			
			if(gh_TsCycleBuffer)
			{
				if(read_ts_packet(rph_PesFilter, gh_TsCycleBuffer) == 1)
				{
					data_length = pop_cycle_data(gh_TsCycleBuffer, gpuc_PesDataBuf, data_length);
					if(data_length <= 0)
					{
						return;
					}
				}
				else
				{
					return;
				}
			}
			else
			{
				ret = CHDRV_PESFILTER_ReadData(rph_PesFilter,gpuc_PesDataBuf, &data_length);
				if(ret != CHDRV_PESFILTER_OK)
				{
					return;
				}
			}
			section_num++;
			LOG("%3d. len=%d, data=[%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x]\n",
				section_num,
				data_length,
				gpuc_PesDataBuf[0],
				gpuc_PesDataBuf[1],
				gpuc_PesDataBuf[2],
				gpuc_PesDataBuf[3],
				gpuc_PesDataBuf[4],
				gpuc_PesDataBuf[5],
				gpuc_PesDataBuf[6],
				gpuc_PesDataBuf[7],
				gpuc_PesDataBuf[8],
				gpuc_PesDataBuf[9],
				gpuc_PesDataBuf[10],
				gpuc_PesDataBuf[11]);

		}
	}
	else
	{
		LOG("gpuc_PesDataBuf = NULL\n");
	}
}


#if ENABLE_TESTCASE

CASE_F(TEST_PESFILTER_INIT1,
	"���Զ�γ�ʼ��ʱ�Ƿ񷵻�OK") {
	EXPECT_EQ(CHDRV_PESFILTER_OK, CHDRV_PESFILTER_Init());
	EXPECT_EQ(CHDRV_PESFILTER_OK, CHDRV_PESFILTER_Init()) << "��γ�ʼ��Ӧ�÷���OK";
}



CASE_F(TEST_PESFILTER_INIT2,
	"����ģ���Ƿ�ɱ�����") {
	EXPECT_EQ(CHDRV_PESFILTER_OK, CHDRV_PESFILTER_Term());
	EXPECT_EQ(CHDRV_PESFILTER_OK, CHDRV_PESFILTER_Init());
}


CASE_F(TEST_PESFILTER_INIT3,
	"����CHDRV_PESFILTER_GetVersion�Ƿ�ɹ�") {
	CHDRV_Version_t stru_Ver;
	
	memset(&stru_Ver, 0, sizeof(stru_Ver));
	EXPECT_EQ(CHDRV_PESFILTER_OK, CHDRV_PESFILTER_GetVersion(&stru_Ver));
	EXPECT_NE(0, stru_Ver);
	LOG("\nstru_Ver=%x\n", stru_Ver);
}


CASE_F(TEST_PESFILER_ALLOC1,
	"ALLOCһ��TS���͵�filter��Ӧ���سɹ��������ΪNULL;�ص�������ֱ�Ӷ�ȡsection������"){

	int size = fw_getenv_i("PES_FILTER_BUFFER_SIZE");
	gpuc_PesDataBuf = (U8 *)CH_RTOS_AllocateMemory(gh_APP_HEAP, sizeof(U8)*size);
	ASSERT_NE(gpuc_PesDataBuf, (void*)NULL);
	memset(gpuc_PesDataBuf, 0, sizeof(U8)*size);
	EXPECT_EQ(CHDRV_PESFILTER_AllocFilter(fw_getenv_i("PES_FILTER_DEVICE_ID"), 
		size, 
		CHDRV_PESFILTER_TS_PACK, 
		pes_call1, 
		&gh_pesHandle), CHDRV_PESFILTER_OK);
}

CASE_F(TEST_PESFILTER_SET1,
	"SET FILTER PIDΪָ����ֵ��Ӧ���سɹ�"){

	EXPECT_EQ(CHDRV_PESFILTER_SetFilterPid(gh_pesHandle, (U16)fw_getenv_i("PES_FILTER_PID")), CHDRV_PESFILTER_OK);
}

CASE_F(TEST_PESFILTER_START1,
	"����START FILTER��Ӧ���سɹ�����ʾ����Ա��ʼ��������"){

	PROMPT("��ֹͣ��������,���������������Ϊ������һ�֣�׼�����Ժ�س�\n");
	test_input_none();
	section_num  = 0;
	EXPECT_EQ(CHDRV_PESFILTER_StartFilter(gh_pesHandle), CHDRV_PESFILTER_OK);

	request_play_program("AV_STREAM_COMMON", "TUNER_PARAMS1", 0);
}

CASE_F(TEST_PESFILTER_STOP1,
	"��ʾ����Ա�ȴ���������һ�ֽ����󣬵���STOP FILTER��Ӧ���سɹ�"){

	PROMPT("����һ���������밴�س�:\n");
	test_input_none();

	CH_RTOS_TaskDelay(1000);
	EXPECT_EQ(CHDRV_PESFILTER_StopFilter(gh_pesHandle), CHDRV_PESFILTER_OK);
}

CASE_F(TEST_PESFILTER_READ1,
	"ͳ���յ���TS����������Ԥ���趨��ֵ���бȽϣ�����Ԥ��ֵ������ʧ��"){

	int deservedNum = fw_getenv_i("PES_TS_PACKET_NUM");

	LOG("%d===\n", section_num);
	EXPECT_GE(section_num, deservedNum) << "����TS©��";
}

CASE_F(TEST_PESFILTER_FREE1,
	"����FREE FILTER��Ӧ���سɹ�"){
	
	EXPECT_EQ(CHDRV_PESFILTER_FreeFilter(gh_pesHandle), CHDRV_PESFILTER_OK);
	gh_pesHandle = NULL;
}

CASE_F(TEST_PESFILTER_ALLOC2,
	"ALLOCһ��PES���͵�filter,Ӧ���سɹ��������ΪNULL;�ص�������ֱ�Ӷ�ȡsection������"){
	section_num = 0;
	EXPECT_EQ(CHDRV_PESFILTER_AllocFilter(fw_getenv_i("PES_FILTER_DEVICE_ID"), 
		fw_getenv_i("PES_FILTER_BUFFER_SIZE"), CHDRV_PESFILTER_PES_PACK, 
		pes_call1, &gh_pesHandle), 
		CHDRV_PESFILTER_OK);
	EXPECT_NE(gh_pesHandle, (void *)NULL);
}

CASE_F(TEST_PESFILTER_SET2,
	"SET FILTER PIDΪָ����ֵ��Ӧ���سɹ�"){

	EXPECT_EQ(CHDRV_PESFILTER_SetFilterPid(gh_pesHandle, 
		(U16)fw_getenv_i("PES_FILTER_PID")), 
		CHDRV_PESFILTER_OK);
}

CASE_F(TEST_PESFILTER_START2,
	"����START FILTER��Ӧ���سɹ�����ʾ����Ա��ʼ��������"){

	PROMPT("��ֹͣ��������,���������������Ϊ������һ�֣�׼�����Ժ�س�\n");
	test_input_none();
	section_num  = 0;
	EXPECT_EQ(CHDRV_PESFILTER_StartFilter(gh_pesHandle), CHDRV_PESFILTER_OK);

	PROMPT("�뿪ʼ����������Ȼ��س�:\n");
	test_input_none();
}

CASE_F(TEST_PESFILTER_STOP2,
	"��ʾ����Ա�ȴ���������һ�ֽ����󣬵���STOP FILTER��Ӧ���سɹ�"){

	PROMPT("����һ���������밴�س�\n");
	test_input_none();

	CH_RTOS_TaskDelay(1000);
	EXPECT_EQ(CHDRV_PESFILTER_StopFilter(gh_pesHandle), CHDRV_PESFILTER_OK);
}

CASE_F(TEST_PESFILTER_READ2,
	"ͳ���յ���PES����������Ԥ���趨��ֵ���бȽϣ�����Ԥ��ֵ������ʧ��"){

	int deservedNum = fw_getenv_i("PES_PES_PACKET_NUM");
	
	LOG("%d===\n",section_num);
	EXPECT_GE(section_num, deservedNum) << "����PES©��";
}

CASE_F(TEST_PESFILTER_FREE2,
	"����FREE FILTER��Ӧ���سɹ�"){

	EXPECT_EQ(CHDRV_PESFILTER_FreeFilter(gh_pesHandle), CHDRV_PESFILTER_OK);
	gh_pesHandle = NULL;
	CH_RTOS_FreeMemory(gh_APP_HEAP, gpuc_PesDataBuf);
	gpuc_PesDataBuf = NULL;
}
#endif /* ENABLE_TESTCASE */


int pes_init(int argc, CMD_PARAM argv[])
{
	int ret;
	
	ret = CHDRV_PESFILTER_Init();
	LOG("CHDRV_PESFILTER_Init return %d\n", ret);
	return ret;
	
}

int pes_term(int argc, CMD_PARAM argv[])
{
	int ret;
		
	ret = CHDRV_PESFILTER_Term();
	LOG("CHDRV_PESFILTER_Term return %d\n", ret);
	return ret;	
}

int pes_ver(int argc, CMD_PARAM argv[])
{
	int ret;
	CHDRV_Version_t stru_Ver;
	
	memset(&stru_Ver, 0, sizeof(stru_Ver));
		
	ret = CHDRV_PESFILTER_GetVersion(&stru_Ver);
	LOG("CHDRV_PESFILTER_GetVersion return %d,version= %d\n", ret, stru_Ver);
	return ret;	

}

int pes_start(int argc, CMD_PARAM argv[])
{	
	CHDRV_PESFILTER_HANDLE h_pesHdl;
	CHDRV_PESFILTER_DATA_TYPE_e type;
	int ret;
	type = (CHDRV_PESFILTER_DATA_TYPE_e)argv[3].get_i();

	
	if(gph_pesHdl.size() > 0)
	{
		LOG("please stop first!\n");
		return -1;
	}
	section_num = 0;
	if(type > CHDRV_PESFILTER_PES_PACK)
	{
		type = CHDRV_PESFILTER_TS_PACK;
		gh_TsCycleBuffer = cycle_buffer_init();
	}
	ret = CHDRV_PESFILTER_AllocFilter(argv[1].get_i(), 256 * 1024, type, pes_call2, &h_pesHdl);
	if(ret == CHDRV_PESFILTER_OK)
	{
		ret = CHDRV_PESFILTER_SetFilterPid(h_pesHdl, argv[2].get_i());
		ret |= CHDRV_PESFILTER_StartFilter(h_pesHdl);
		gph_pesHdl.push_back(h_pesHdl);
	}
	else if(gh_TsCycleBuffer)
	{
		cycle_buffer_deinit(gh_TsCycleBuffer);
		gh_TsCycleBuffer = NULL;
	}
	return ret;
}

int pes_stop(int argc, CMD_PARAM argv[])
{
	int ret;

	if(gph_pesHdl.size() <= argv[1].get_i())
	{
		LOG("please start first!\n");
		return -1;
	}
	ret = CHDRV_PESFILTER_StopFilter(gph_pesHdl[argv[1].get_i()]);
	if(ret != 0)
	{
		PROMPT("Stop Filter Fail!\n");
		return -1;
	}
	ret = CHDRV_PESFILTER_FreeFilter(gph_pesHdl[argv[1].get_i()]);
	if(ret != 0)
	{
		PROMPT("Free Filter Fail!\n");
		return -1;
	}
	gph_pesHdl.erase(gph_pesHdl.begin() + argv[1].get_i());
	if(gpuc_PesDataBuf)
	{
		CH_RTOS_FreeMemory(gh_APP_HEAP, gpuc_PesDataBuf);
		gpuc_PesDataBuf = NULL;
	}
	if(gh_TsCycleBuffer)
	{
		cycle_buffer_deinit(gh_TsCycleBuffer);
		gh_TsCycleBuffer = NULL;
	}
	return 0;
}


REGISTER_CMD(pes_init,"��ʼ��pesfilterģ��","","");
REGISTER_CMD(pes_term,"����pesfilterģ��","","");
REGISTER_CMD(pes_ver,"��ȡpesfilterģ��汾��","","");
REGISTER_CMD(pes_start,"����һ��PES filter�����յ������ݲ���ӡ","device_id|pid|packet_type","0 0x83 0");
REGISTER_CMD(pes_stop,"ֹͣ���ͷ�pesfilterģ��","index","0");



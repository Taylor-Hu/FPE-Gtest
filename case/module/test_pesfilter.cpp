/**
@file test_pesfilter.cpp
@def group pesfilter测试集

@author 网络公司基础技术部:	邹书强
@date   2014年4月10日  
@version V4.0
@todo     ---时间---作者-------------修改部分及原因----------
@todo     1 2014年4月10日 邹书强  创建


@{
*/

/*测试集名称定义*/
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
	"测试多次初始化时是否返回OK") {
	EXPECT_EQ(CHDRV_PESFILTER_OK, CHDRV_PESFILTER_Init());
	EXPECT_EQ(CHDRV_PESFILTER_OK, CHDRV_PESFILTER_Init()) << "多次初始化应该返回OK";
}



CASE_F(TEST_PESFILTER_INIT2,
	"测试模块是否可被销毁") {
	EXPECT_EQ(CHDRV_PESFILTER_OK, CHDRV_PESFILTER_Term());
	EXPECT_EQ(CHDRV_PESFILTER_OK, CHDRV_PESFILTER_Init());
}


CASE_F(TEST_PESFILTER_INIT3,
	"测试CHDRV_PESFILTER_GetVersion是否成功") {
	CHDRV_Version_t stru_Ver;
	
	memset(&stru_Ver, 0, sizeof(stru_Ver));
	EXPECT_EQ(CHDRV_PESFILTER_OK, CHDRV_PESFILTER_GetVersion(&stru_Ver));
	EXPECT_NE(0, stru_Ver);
	LOG("\nstru_Ver=%x\n", stru_Ver);
}


CASE_F(TEST_PESFILER_ALLOC1,
	"ALLOC一个TS类型的filter，应返回成功，句柄不为NULL;回调函数里直接读取section并计数"){

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
	"SET FILTER PID为指定的值，应返回成功"){

	EXPECT_EQ(CHDRV_PESFILTER_SetFilterPid(gh_pesHandle, (U16)fw_getenv_i("PES_FILTER_PID")), CHDRV_PESFILTER_OK);
}

CASE_F(TEST_PESFILTER_START1,
	"调用START FILTER，应返回成功；提示测试员开始播放码流"){

	PROMPT("请停止码流播放,并将播出软件设置为仅播放一轮，准备好以后回车\n");
	test_input_none();
	section_num  = 0;
	EXPECT_EQ(CHDRV_PESFILTER_StartFilter(gh_pesHandle), CHDRV_PESFILTER_OK);

	request_play_program("AV_STREAM_COMMON", "TUNER_PARAMS1", 0);
}

CASE_F(TEST_PESFILTER_STOP1,
	"提示测试员等待码流播放一轮结束后，调用STOP FILTER，应返回成功"){

	PROMPT("播完一轮码流后，请按回车:\n");
	test_input_none();

	CH_RTOS_TaskDelay(1000);
	EXPECT_EQ(CHDRV_PESFILTER_StopFilter(gh_pesHandle), CHDRV_PESFILTER_OK);
}

CASE_F(TEST_PESFILTER_READ1,
	"统计收到的TS个数，并与预先设定的值进行比较，低于预设值本测试失败"){

	int deservedNum = fw_getenv_i("PES_TS_PACKET_NUM");

	LOG("%d===\n", section_num);
	EXPECT_GE(section_num, deservedNum) << "接收TS漏包";
}

CASE_F(TEST_PESFILTER_FREE1,
	"调用FREE FILTER，应返回成功"){
	
	EXPECT_EQ(CHDRV_PESFILTER_FreeFilter(gh_pesHandle), CHDRV_PESFILTER_OK);
	gh_pesHandle = NULL;
}

CASE_F(TEST_PESFILTER_ALLOC2,
	"ALLOC一个PES类型的filter,应返回成功，句柄不为NULL;回调函数里直接读取section并计数"){
	section_num = 0;
	EXPECT_EQ(CHDRV_PESFILTER_AllocFilter(fw_getenv_i("PES_FILTER_DEVICE_ID"), 
		fw_getenv_i("PES_FILTER_BUFFER_SIZE"), CHDRV_PESFILTER_PES_PACK, 
		pes_call1, &gh_pesHandle), 
		CHDRV_PESFILTER_OK);
	EXPECT_NE(gh_pesHandle, (void *)NULL);
}

CASE_F(TEST_PESFILTER_SET2,
	"SET FILTER PID为指定的值，应返回成功"){

	EXPECT_EQ(CHDRV_PESFILTER_SetFilterPid(gh_pesHandle, 
		(U16)fw_getenv_i("PES_FILTER_PID")), 
		CHDRV_PESFILTER_OK);
}

CASE_F(TEST_PESFILTER_START2,
	"调用START FILTER，应返回成功；提示测试员开始播放码流"){

	PROMPT("请停止码流播放,并将播出软件设置为仅播放一轮，准备好以后回车\n");
	test_input_none();
	section_num  = 0;
	EXPECT_EQ(CHDRV_PESFILTER_StartFilter(gh_pesHandle), CHDRV_PESFILTER_OK);

	PROMPT("请开始播放码流，然后回车:\n");
	test_input_none();
}

CASE_F(TEST_PESFILTER_STOP2,
	"提示测试员等待码流播放一轮结束后，调用STOP FILTER，应返回成功"){

	PROMPT("播完一轮码流后，请按回车\n");
	test_input_none();

	CH_RTOS_TaskDelay(1000);
	EXPECT_EQ(CHDRV_PESFILTER_StopFilter(gh_pesHandle), CHDRV_PESFILTER_OK);
}

CASE_F(TEST_PESFILTER_READ2,
	"统计收到的PES个数，并与预先设定的值进行比较，低于预设值本测试失败"){

	int deservedNum = fw_getenv_i("PES_PES_PACKET_NUM");
	
	LOG("%d===\n",section_num);
	EXPECT_GE(section_num, deservedNum) << "接收PES漏包";
}

CASE_F(TEST_PESFILTER_FREE2,
	"调用FREE FILTER，应返回成功"){

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


REGISTER_CMD(pes_init,"初始化pesfilter模块","","");
REGISTER_CMD(pes_term,"销毁pesfilter模块","","");
REGISTER_CMD(pes_ver,"获取pesfilter模块版本号","","");
REGISTER_CMD(pes_start,"创建一个PES filter并将收到的内容并打印","device_id|pid|packet_type","0 0x83 0");
REGISTER_CMD(pes_stop,"停止并释放pesfilter模块","index","0");



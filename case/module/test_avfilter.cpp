/**
@file test_avfilter.cpp
@def group avfilter测试集

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
	"测试多次初始化时是否返回OK") {
	EXPECT_EQ(CHDRV_AVFILTER_OK, CHDRV_AVFILTER_Init());
	EXPECT_EQ(CHDRV_AVFILTER_OK, CHDRV_AVFILTER_Init()) << "多次初始化应该返回OK";
}




CASE_F(TEST_AVFILTER_INIT2,
	"测试模块是否可被销毁") {
	EXPECT_EQ(CHDRV_AVFILTER_OK, CHDRV_AVFILTER_Term());
	EXPECT_EQ(CHDRV_AVFILTER_OK, CHDRV_AVFILTER_Init());
}


CASE_F(TEST_AVFILTER_INIT3,
	"测试CHDRV_AVFILTER_GetVersion是否成功") {
	CHDRV_Version_t stru_Ver;
	
	memset(&stru_Ver, 0, sizeof(stru_Ver));
	EXPECT_EQ(CHDRV_AVFILTER_OK, CHDRV_AVFILTER_GetVersion(&stru_Ver));
	EXPECT_NE(0, stru_Ver);
	LOG("\nstru_Ver=%x\n", stru_Ver);
}

CASE_F(TEST_AVFILTER_ALLOC,
	"分别分配audio、video、pcr类型的filter，均应返回成功") {
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
	"调用get status，状态应为IDLE") {
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
	"分别为上述filter分配pid，pid的值预先固定") {
	int i;
	
	for(i = 0; i < avfilter_GetHandleNums();i++)
	{
		EXPECT_EQ(CHDRV_AVFILTER_OK, CHDRV_AVFILTER_SetPid(avfilter_GetHandle(i),
			101 + i));
	}
}

CASE_F(TEST_AVFILTER_GETPID,
	"调用get pid，获得的值应与set的值一致") {
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
	"分别enable上述3个filter，应返回OK") {
	int i;
	
	for(i = 0; i < avfilter_GetHandleNums();i++)
	{
		EXPECT_EQ(CHDRV_AVFILTER_OK, CHDRV_AVFILTER_EnableFilter(avfilter_GetHandle(i)));
	}
}

CASE_F(TEST_AVFILTER_STATUS2,
	"调用get status，状态应为NO DATA") {
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
	"锁频后，调用get status，状态应为GOOD") {
	CHDRV_AVFILTER_STATUS_e enm_Status;
	int i;
	int ret;
	vector <Channel_Info_t> prog_list;
	U16 us_apid = 0, us_vpid = 0, us_ppid = 0;
	do
	{
		PROMPT("请准备好并播放测试码流[%s],播放在频点[%s]，然后按回车",
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
	"分别disable上述3个filter，应返回OK") {
	int i;
	
	for(i = 0; i < avfilter_GetHandleNums();i++)
	{
		EXPECT_EQ(CHDRV_AVFILTER_OK, CHDRV_AVFILTER_DisableFilter(avfilter_GetHandle(i)));
	}
}

CASE_F(TEST_AVFILTER_FREE,
	"分别delete上述3个filter，应返回OK") {
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

REGISTER_CMD(avfilter_init, "初始化avfilter模块", "",  "");
REGISTER_CMD(avfilter_term, "销毁avfilter模块", "",  "");
REGISTER_CMD(avfilter_ver, "获取avfilter模块版本号", "",  "");
REGISTER_CMD(avfilter_status, "打印所有avfilter的工作状态", "",  "");
REGISTER_CMD(avfilter_start, "创建并使能一个avfilter", "device_id|type|pid",  "0 0 0x28");
REGISTER_CMD(avfilter_stop, "停止并删除一个avfilter", "handle_no",  "0");
REGISTER_CMD(avfilter_set_pid, "播放过程中修改filter的pid", "handle_no | pid",  "0 0x28");
REGISTER_CMD(avfilter_info, "获得avfilter的相关信息，打印到串口", "",  "");



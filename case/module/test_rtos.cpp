/**
@file test_rtos.cpp
@defgroup rtos测试集

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
#define CASE_NAME TestRtos


#include "test_menu.h"
#include "test_rtos.h"


#if ENABLE_TESTCASE
void TestRtos::SetUpTestCase() {
//excute before each test suit
//std::cout << "init SetUp rtos TestCase " << std::endl;
}
void TestRtos::TearDownTestCase() {
//excute after each test suit
//std::cout << "deinit tuner rtos TestCase " << std::endl;
}

void TestRtos::SetUp()
{
//execute before testcase
//std::cout << "Chdrv TestRtos SetUp" << std::endl;
}
void TestRtos::TearDown()
{
//execute before testcase
//std::cout << "Chdrv TestRtos TearDown" << std::endl;
}
#endif /* ENABLE_TESTCASE */

#define MAX_TEST_TASK_NUM  64
CH_RTOS_TaskHandle    gh_Task[MAX_TEST_TASK_NUM];
static U32 gui_task_count = 0;
static CH_BOOL gb_task_delay = 0;
static CH_BOOL gb_mutex_task_run = 1;
static int gb_mutex_count = 0;
static CH_RTOS_TimerHandle gh_timer = NULL;
static CH_RTOS_SemaphoreHandle gh_timer_sem = NULL;

extern void test_DDR(size_t size, unsigned long loops);
#if ENABLE_TESTCASE

CASE_F(TEST_RTOS_INIT1,
	"测试多次初始化时是否返回OK") {
	EXPECT_EQ(CH_RTOS_OK, CH_RTOS_Init());
	EXPECT_EQ(CH_RTOS_OK, CH_RTOS_Init()) << "多次初始化应该返回OK";
}



CASE_F(TEST_RTOS_INIT2,
	"测试模块是否可被销毁") {
	EXPECT_EQ(CH_RTOS_OK, CH_RTOS_Term());
	EXPECT_EQ(CH_RTOS_OK, CH_RTOS_Init());
}


CASE_F(TEST_RTOS_INIT3,
	"测试CH_RTOS_GetVersion是否成功") {
	CHDRV_Version_t stru_Ver;
	
	memset(&stru_Ver, 0, sizeof(stru_Ver));
	EXPECT_EQ(CH_RTOS_OK, CH_RTOS_GetVersion(&stru_Ver));
	EXPECT_NE(0, stru_Ver);
	LOG("\nstru_Ver=%x\n", stru_Ver);
}



static void test_task_prams(void *p)
{
	int *pi_Param = (int *)p;
	
	CH_RTOS_TaskEnter(NULL);
	if(pi_Param)
	{
		gui_task_count += *pi_Param;
	}
	while(gb_task_delay)
	{
		CH_RTOS_TaskDelay(100);
		gui_task_count ++;
	}
	CH_RTOS_TaskExit();
}

static void test_task_getid(void *p)
{
	CH_RTOS_TaskHandle *pi_Param = (CH_RTOS_TaskHandle *)p;
	
	CH_RTOS_TaskEnter(NULL);
	if(pi_Param)
	{
		EXPECT_EQ(CH_RTOS_GetCurrentTaskId(pi_Param), CH_RTOS_OK);
	}
	CH_RTOS_TaskExit();
}

CASE_F(TEST_RTOS_TASK1,
	"测试rpv_StackPtr为NULL，其余参数合法时是否可以成功创建任务	") {

	gb_task_delay = CH_TRUE;
	EXPECT_EQ(CH_RTOS_CreateTask(test_task_prams, NULL, NULL, 1024 * 8, 200, (S8*)"test_rtos_task_01", &gh_Task[0]),
		CH_RTOS_OK);
	EXPECT_NE(gh_Task[0], (void*)NULL) << "TASK handle不应为NULL";
	EXPECT_EQ(CH_RTOS_CreateTask(test_task_prams, NULL, NULL, 1024 * 8, 200, (S8*)"test_rtos_task_02", &gh_Task[1]),
		CH_RTOS_OK);
	EXPECT_NE(gh_Task[1], (void*)NULL) << "TASK handle不应为NULL";
}

CASE_F(TEST_RTOS_TASK2,
	"是否可以通过CH_RTOS_WaitTaskTerm等待成功	") {
	
	EXPECT_EQ(CH_RTOS_WaitTaskTerm(gh_Task[0], 100),
		CH_RTOS_TIME_OUT);
	EXPECT_NE(CH_RTOS_WaitTaskTerm(gh_Task[1], 0),
		CH_RTOS_OK);
	
	gb_task_delay = CH_FALSE;
		
	EXPECT_EQ(CH_RTOS_WaitTaskTerm(gh_Task[0], 1000),
		CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_WaitTaskTerm(gh_Task[1], 1000),
		CH_RTOS_OK);
}

CASE_F(TEST_RTOS_TASK3,
	"测试是否可以通过CH_RTOS_KillTask删除刚刚创建的任务") {

	if(!gh_Task[0])return;
	EXPECT_EQ(CH_RTOS_KillTask(gh_Task[0]),
		CH_RTOS_OK);
	gh_Task[0] = NULL;
}

CASE_F(TEST_RTOS_TASK4,
	"测试是否可以通过CH_RTOS_DeleteTask删除刚刚创建的任务") {

	if(!gh_Task[1])return;
	EXPECT_EQ(CH_RTOS_DeleteTask(gh_Task[1]),
		CH_RTOS_OK);
	gh_Task[1] = NULL;
}

CASE_F(TEST_RTOS_TASK5,
	"测试当 ri_TimeOutMS=-1时，CH_RTOS_WaitTaskTerm是否执行正确") {

	gb_task_delay = CH_FALSE;
	EXPECT_EQ(CH_RTOS_CreateTask(test_task_prams, NULL, NULL, 1024 * 8, 200, (S8*)"test_rtos_task_01", &gh_Task[0]),
		CH_RTOS_OK);
	EXPECT_NE(gh_Task[0], (void*)NULL);
	EXPECT_EQ(CH_RTOS_WaitTaskTerm(gh_Task[0], -1), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteTask(gh_Task[0]),	CH_RTOS_OK);
	gh_Task[0] = NULL;
}

CASE_F(TEST_RTOS_TASK6,
	"测试当 ri_TimeOutMS=1s,2s,5s时，CH_RTOS_WaitTaskTerm等待时间是否匹配") {

	U32 time1, time2, wait[] = {1000, 2000, 5000}, i;
	
	gb_task_delay = CH_TRUE;
	EXPECT_EQ(CH_RTOS_CreateTask(test_task_prams, NULL, NULL, 1024 * 8, 200, (S8*)"test_rtos_task_01", &gh_Task[0]),
		CH_RTOS_OK);
	EXPECT_NE(gh_Task[0], (void*)NULL);
	for(i = 0; i < 3; i++)
	{
		time1 = CH_RTOS_GetSysTimenow();
		EXPECT_EQ(CH_RTOS_WaitTaskTerm(gh_Task[0], wait[i]), CH_RTOS_TIME_OUT);
		time2 = CH_RTOS_GetSysTimenow() - time1;
		EXPECT_LT(time2, wait[i] + 1000) << "超时时间误差大于土1000ms";
		EXPECT_GT(time2, wait[i] - 1000) << "超时时间误差大于土1000ms";;
	}
	gb_task_delay = CH_FALSE;
	EXPECT_EQ(CH_RTOS_WaitTaskTerm(gh_Task[0], 1000), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteTask(gh_Task[0]),	CH_RTOS_OK);
	gh_Task[0] = NULL;
}



CASE_F(TEST_RTOS_TASK7,
	"测试rpv_StackPtr不为NULL，其余参数合法时是否可以成功创建任务") {

	void *stack = NULL;
	
	EXPECT_NE(stack = CH_RTOS_AllocateMemory(gh_APP_HEAP, 8 * 1024), (void*)NULL);
	
	gb_task_delay = CH_FALSE;
	EXPECT_EQ(CH_RTOS_CreateTask(test_task_prams, NULL, stack, 1024 * 8, 200,
		(S8*)"test_rtos_task_01",
		&gh_Task[0]),
		CH_RTOS_OK);
	EXPECT_NE(gh_Task[0], (void*)NULL);
	EXPECT_EQ(CH_RTOS_WaitTaskTerm(gh_Task[0], -1), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteTask(gh_Task[0]),	CH_RTOS_OK);
	gh_Task[0] = NULL;

	CH_RTOS_FreeMemory(gh_APP_HEAP, stack);
}


CASE_F(TEST_RTOS_TASK8,
	"测试rpv_Param不为NULL时，是否被真正传递") {

	U32 params = 100;
	
	
	gb_task_delay = CH_FALSE;
	gui_task_count = 0;
	EXPECT_EQ(CH_RTOS_CreateTask(test_task_prams, &params, NULL, 1024 * 8, 200,
		(S8*)"test_rtos_task_01",
		&gh_Task[0]),
		CH_RTOS_OK);
	EXPECT_NE(gh_Task[0], (void*)NULL);
	EXPECT_EQ(CH_RTOS_WaitTaskTerm(gh_Task[0], -1), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteTask(gh_Task[0]),	CH_RTOS_OK);
	gh_Task[0] = NULL;

	EXPECT_EQ(gui_task_count, params) << "线程参数未被正确传递";
	
}


CASE_F(TEST_RTOS_TASK9,
	"测试ri_Priority为-1时，是否可以成功创建任务") {

	gb_task_delay = CH_FALSE;
	EXPECT_NE(CH_RTOS_CreateTask(test_task_prams, NULL, NULL, 1024 * 8, -1,
		(S8*)"test_rtos_task_01",
		&gh_Task[0]),
		CH_RTOS_OK);
	if(gh_Task[0])
	{
		EXPECT_EQ(CH_RTOS_WaitTaskTerm(gh_Task[0], 10000), CH_RTOS_OK);
		EXPECT_EQ(CH_RTOS_DeleteTask(gh_Task[0]),	CH_RTOS_OK);
		gh_Task[0] = NULL;
	}
}

CASE_F(TEST_RTOS_TASK10,
	"测试ri_Priority为255时，是否可以成功创建任务") {

	gb_task_delay = CH_FALSE;
	EXPECT_EQ(CH_RTOS_CreateTask(test_task_prams, NULL, NULL, 1024 * 8, 255,
		(S8*)"test_rtos_task_01",
		&gh_Task[0]),
		CH_RTOS_OK);
	if(gh_Task[0])
	{
		EXPECT_EQ(CH_RTOS_WaitTaskTerm(gh_Task[0], 10000), CH_RTOS_OK);
		EXPECT_EQ(CH_RTOS_DeleteTask(gh_Task[0]),	CH_RTOS_OK);
		gh_Task[0] = NULL;
	}
}

CASE_F(TEST_RTOS_TASK11,
	"测试rpuc_Name不为NULL且相同时，是否可以成功创建任务") {

	gb_task_delay = CH_FALSE;
	EXPECT_EQ(CH_RTOS_CreateTask(test_task_prams, NULL, NULL, 1024 * 8, 200,
		(S8*)"test_rtos_task_01",
		&gh_Task[0]),
		CH_RTOS_OK);
	EXPECT_NE(gh_Task[0],(void*) NULL);
	
	EXPECT_EQ(CH_RTOS_CreateTask(test_task_prams, NULL, NULL, 1024 * 8, 200,
		(S8*)"test_rtos_task_01",
		&gh_Task[1]),
		CH_RTOS_OK);
	EXPECT_NE(gh_Task[1], (void*)NULL);
	
	if(gh_Task[0])
	{
		EXPECT_EQ(CH_RTOS_WaitTaskTerm(gh_Task[0], 10000), CH_RTOS_OK);
		EXPECT_EQ(CH_RTOS_DeleteTask(gh_Task[0]),	CH_RTOS_OK);
		gh_Task[0] = NULL;
	}
	if(gh_Task[1])
	{
		EXPECT_EQ(CH_RTOS_WaitTaskTerm(gh_Task[1], 10000), CH_RTOS_OK);
		EXPECT_EQ(CH_RTOS_DeleteTask(gh_Task[1]),	CH_RTOS_OK);
		gh_Task[1] = NULL;
	}
}



CASE_F(TEST_RTOS_TASK12,
	"测试rpuc_Name为NULL时，是否可以成功创建任务") {

	gb_task_delay = CH_FALSE;
	EXPECT_EQ(CH_RTOS_CreateTask(test_task_prams, NULL, NULL, 1024 * 8, 255,
		NULL,
		&gh_Task[0]),
		CH_RTOS_OK);
	EXPECT_NE(gh_Task[0], (void*)NULL);
	if(gh_Task[0])
	{
		EXPECT_EQ(CH_RTOS_WaitTaskTerm(gh_Task[0], 10000), CH_RTOS_OK);
		EXPECT_EQ(CH_RTOS_DeleteTask(gh_Task[0]),	CH_RTOS_OK);
		gh_Task[0] = NULL;
	}
}


CASE_F(TEST_RTOS_TASK13,
	"测试能否成功创建64个ri_StackSize大小为8K的任务") {
	int i;
	gb_task_delay = CH_TRUE;
	for(i = 0; i < MAX_TEST_TASK_NUM; i++)
	{
		EXPECT_EQ(CH_RTOS_CreateTask(test_task_prams, NULL, NULL, 1024 * 8, 10 + i,
			NULL,
			&gh_Task[i]),
			CH_RTOS_OK);
		EXPECT_NE(gh_Task[i], (void*)NULL) << "创建第" << i << "个线程时失败";
		if(!gh_Task[i])
		{
			break;
		}
	}
	gb_task_delay = CH_FALSE;
	for(i = 0; i < MAX_TEST_TASK_NUM; i++)
	{
		if(gh_Task[i])
		{
			EXPECT_EQ(CH_RTOS_WaitTaskTerm(gh_Task[i], 1000), CH_RTOS_OK) << "等待第" << i << "个线程时失败";
			EXPECT_EQ(CH_RTOS_DeleteTask(gh_Task[i]),	CH_RTOS_OK)<< "销毁第" << i << "个线程时失败";
			gh_Task[i] = NULL;
		}
	}
}

CASE_F(TEST_RTOS_TASK14,
	"测试能否删除正在运行的任务") {

	gb_task_delay = CH_TRUE;
	EXPECT_EQ(CH_RTOS_CreateTask(test_task_prams, NULL, NULL, 1024 * 8, 100,
		NULL,
		&gh_Task[0]),
		CH_RTOS_OK);
	EXPECT_NE(gh_Task[0], (void*)NULL);

	CH_RTOS_DeleteTask(gh_Task[0]);//不检查返回值,只观察会不会阻塞
	
	gb_task_delay = CH_FALSE;
	CH_RTOS_WaitTaskTerm(gh_Task[0], 10000);//不检查返回值,只观察会不会阻塞
	CH_RTOS_DeleteTask(gh_Task[0]);//不检查返回值,只观察会不会阻塞
	gh_Task[0] = NULL;
}
#if 0
CASE_F(TEST_RTOS_TASK15,
	"测试能否暂停恢复一个正在运行的任务") {

	gb_task_delay = CH_TRUE;
	gui_task_count = 0;
	EXPECT_EQ(CH_RTOS_CreateTask(test_task_prams, NULL, NULL, 1024 * 8, 100,
		NULL,
		&gh_Task[0]),
		CH_RTOS_OK);
	EXPECT_NE(gh_Task[0], (void*)NULL);

	LOG("here\n");
	CH_RTOS_TaskDelay(300);
	EXPECT_EQ(CH_RTOS_SuspendTask(gh_Task[0]), CH_RTOS_OK);
	LOG("here\n");
	EXPECT_NE(gui_task_count, 0) << "运行一段时间后count会被累加";
	LOG("gui_task_count = %d\n", gui_task_count);
	
	gui_task_count = 0;
	LOG("gui_task_count = %d\n", gui_task_count);
	CH_RTOS_TaskDelay(300);
	LOG("gui_task_count = %d\n", gui_task_count);
	EXPECT_EQ(0, gui_task_count) << "暂停线程后count值不应被改变";

	LOG("here\n");
	EXPECT_EQ(CH_RTOS_ResumeTask(gh_Task[0]), CH_RTOS_OK);
	CH_RTOS_TaskDelay(300);
	EXPECT_NE(gui_task_count, 0), "恢复一段时间后count会继续被累加";;

	LOG("here\n");
	gb_task_delay = CH_FALSE;
	EXPECT_EQ(CH_RTOS_WaitTaskTerm(gh_Task[0], 10000), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteTask(gh_Task[0]),	CH_RTOS_OK);
	gh_Task[0] = NULL;

	LOG("here\n");
}


CASE_F(TEST_RTOS_TASK16,
	"测试能否CH_RTOS_ResumeTask一个正在运行的任务") {

	gb_task_delay = CH_TRUE;
	EXPECT_EQ(CH_RTOS_CreateTask(test_task_prams, NULL, NULL, 1024 * 8, 100,
		NULL,
		&gh_Task[0]),
		CH_RTOS_OK);
	EXPECT_NE(gh_Task[0], (void*)NULL);

	EXPECT_NE(CH_RTOS_ResumeTask(gh_Task[0]), CH_RTOS_OK);
	
	gb_task_delay = CH_FALSE;
	EXPECT_EQ(CH_RTOS_WaitTaskTerm(gh_Task[0], 10000), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteTask(gh_Task[0]),	CH_RTOS_OK);
	gh_Task[0] = NULL;
}


CASE_F(TEST_RTOS_TASK17,
	"测试能否CH_RTOS_SuspendTask一个已经suspend的任务") {

	gb_task_delay = CH_TRUE;
	EXPECT_EQ(CH_RTOS_CreateTask(test_task_prams, NULL, NULL, 1024 * 8, 100,
		NULL,
		&gh_Task[0]),
		CH_RTOS_OK);
	EXPECT_NE(gh_Task[0], (void*)NULL);

	EXPECT_EQ(CH_RTOS_SuspendTask(gh_Task[0]), CH_RTOS_OK);
	EXPECT_NE(CH_RTOS_SuspendTask(gh_Task[0]), CH_RTOS_OK) << "重复suspend不应成功";
	
	EXPECT_EQ(CH_RTOS_ResumeTask(gh_Task[0]), CH_RTOS_OK);
	
	gb_task_delay = CH_FALSE;
	EXPECT_EQ(CH_RTOS_WaitTaskTerm(gh_Task[0], 10000), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteTask(gh_Task[0]),	CH_RTOS_OK);
	gh_Task[0] = NULL;
}


CASE_F(TEST_RTOS_TASK18,
	"测试能否暂停恢复一个任务体已经退出，但句柄未被delete的任务") {

	gb_task_delay = CH_FALSE;
	EXPECT_EQ(CH_RTOS_CreateTask(test_task_prams, NULL, NULL, 1024 * 8, 100,
		NULL,
		&gh_Task[0]),
		CH_RTOS_OK);
	EXPECT_NE(gh_Task[0], (void*)NULL);

	CH_RTOS_TaskDelay(100);
	EXPECT_EQ(CH_RTOS_SuspendTask(gh_Task[0]), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_ResumeTask(gh_Task[0]), CH_RTOS_OK);
	
	EXPECT_EQ(CH_RTOS_WaitTaskTerm(gh_Task[0], 10000), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteTask(gh_Task[0]),	CH_RTOS_OK);
	gh_Task[0] = NULL;
}
#endif
CASE_F(TEST_RTOS_TASK19,
	"测试能否通过CH_RTOS_GetCurrentTaskId正确获得当前任务的ID") {

	CH_RTOS_TaskHandle cur_ID = NULL;
	
	gb_task_delay = CH_FALSE;
	EXPECT_EQ(CH_RTOS_CreateTask(test_task_getid, &cur_ID, NULL, 1024 * 8, 100,
		NULL,
		&gh_Task[0]),
		CH_RTOS_OK);
	EXPECT_NE(gh_Task[0], (void*)NULL);

	CH_RTOS_TaskDelay(100);
	EXPECT_EQ(cur_ID, gh_Task[0]) << "获取的任务ID不一致";
	
	EXPECT_EQ(CH_RTOS_WaitTaskTerm(gh_Task[0], 10000), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteTask(gh_Task[0]),	CH_RTOS_OK);
	gh_Task[0] = NULL;
}

CASE_F(TEST_RTOS_MES1,
	"测试能否使用合法参数创建并删除一个消息") {
	CH_RTOS_MessageQHandle handle = NULL;
	int mes = 100, num = 32;

	EXPECT_EQ(CH_RTOS_CreateMessageQuene(sizeof(mes), num, &handle), CH_RTOS_OK);
	EXPECT_NE((void*)NULL, handle);
	EXPECT_EQ(CH_RTOS_DeleteMessageQuene(handle), CH_RTOS_OK);
}

CASE_F(TEST_RTOS_MES2,
	"测试rph_MessageQ为NULL时是否能够成功创建一个消息") {
	int mes = 100, num = 32;

	EXPECT_EQ(CH_RTOS_CreateMessageQuene(sizeof(mes), num, NULL), CH_RTOS_INVALID_PARAM);
}

CASE_F(TEST_RTOS_MES3,
	"测试ri_MessageSize为0时是否能够成功创建一个消息") {
	CH_RTOS_MessageQHandle handle = NULL;
	int num = 32;

	EXPECT_NE(CH_RTOS_CreateMessageQuene(0, num, &handle), CH_RTOS_OK);
	if(handle)
	EXPECT_EQ(CH_RTOS_DeleteMessageQuene(handle), CH_RTOS_OK);
}


CASE_F(TEST_RTOS_MES4,
	"测试ri_MessageSize为64时是否能够成功创建一个消息") {
	CH_RTOS_MessageQHandle handle = NULL;
	int num = 32;

	EXPECT_EQ(CH_RTOS_CreateMessageQuene(64, num, &handle), CH_RTOS_OK);
	EXPECT_NE((void*)NULL, handle);
	EXPECT_EQ(CH_RTOS_DeleteMessageQuene(handle), CH_RTOS_OK);
}

CASE_F(TEST_RTOS_MES5,
	"测试ri_MessageNumber为0时是否能够成功创建一个消息") {
	CH_RTOS_MessageQHandle handle = NULL;
	int mes = 100, num = 0;

	EXPECT_NE(CH_RTOS_CreateMessageQuene(sizeof(mes), num, &handle), CH_RTOS_OK);
	if(handle)
	{
		EXPECT_EQ(CH_RTOS_DeleteMessageQuene(handle), CH_RTOS_OK);
	}
}

CASE_F(TEST_RTOS_MES6,
	"测试ri_MessageNumber为128时是否能够成功创建一个消息") {
	CH_RTOS_MessageQHandle handle = NULL;
	int mes = 100, num = 128;

	EXPECT_EQ(CH_RTOS_CreateMessageQuene(sizeof(mes), num, &handle), CH_RTOS_OK);
	EXPECT_NE((void*)NULL, handle);
	EXPECT_EQ(CH_RTOS_DeleteMessageQuene(handle), CH_RTOS_OK);
}

CASE_F(TEST_RTOS_MES7,
	"测试能否一次性创建32个ri_MessageSize=64，ri_MessageNumber=128的消息") {
#define TEST_MAX_MES_NUM 32
	
	CH_RTOS_MessageQHandle handle[TEST_MAX_MES_NUM];
	int num = 128, i;

	memset(&handle[0], 0, sizeof(handle));
	for(i = 0; i < TEST_MAX_MES_NUM; i++)
	{
		EXPECT_EQ(CH_RTOS_CreateMessageQuene(64, num, &handle[i]), CH_RTOS_OK) << "创建第" << i << "个消息时失败";
		EXPECT_NE((void*)NULL, handle[i])<< "创建第" << i << "个消息时失败";
		if(handle[i] == NULL)
		{
			break;
		}
	}
	for(i = 0; i < TEST_MAX_MES_NUM; i++)
	{
		if(handle[i])
		{
			EXPECT_EQ(CH_RTOS_DeleteMessageQuene(handle[i]), CH_RTOS_OK);
		}
	}
}


CASE_F(TEST_RTOS_MES8,
	"测试能否重复删除一个消息") {
	CH_RTOS_MessageQHandle handle = NULL;
	int mes = 100, num = 128;

	EXPECT_EQ(CH_RTOS_CreateMessageQuene(sizeof(mes), num, &handle), CH_RTOS_OK);
	EXPECT_NE((void*)NULL, handle);
	EXPECT_EQ(CH_RTOS_DeleteMessageQuene(handle), CH_RTOS_OK);
	int dead_test = fw_getenv_i("DEAD_HALT_TEST");
	if(dead_test == 1)
	{
		EXPECT_NE(CH_RTOS_DeleteMessageQuene(handle), CH_RTOS_OK) << "重复删除消息不应死机";
	}
	else
	{
		EXPECT_EQ(dead_test, 1) << "应通过死亡测试";
	}
}

static void test_task_mes(void *p)
{
	CH_RTOS_TaskHandle *pi_Param = (CH_RTOS_TaskHandle *)p;
	int mes;
	CH_RTOS_MessageQHandle h_MessageQ = p;
	
	CH_RTOS_TaskEnter(NULL);
	if(pi_Param)
	{
		while(1)
		{
			if(CH_RTOS_ReceiveMessage(h_MessageQ, &mes, -1) == CH_RTOS_OK)
			{
				//LOG("mes handle 0x%x receive 0x%x\n", h_MessageQ, mes);
				if(mes == (int)'q')
				{
					//LOG("mes handle 0x%x quit\n", h_MessageQ);
					goto iEXIT;
				}
			}
			CH_RTOS_SendMessage((CH_RTOS_MessageQHandle)mes, &mes, -1);
			//LOG("mes handle 0x%x send 0x%x\n", mes, mes);

			CH_RTOS_SendMessage((CH_RTOS_MessageQHandle)mes, &h_MessageQ, -1);
			//LOG("mes handle 0x%x send 0x%x\n", mes, ((int)h_MessageQ));
		}
	}
iEXIT:
	
	CH_RTOS_TaskExit();
}

CASE_F(TEST_RTOS_MES9,
	"多个线程+多个消息的强度测试") {
#define TEST_MES_NUM 16	
	CH_RTOS_MessageQHandle mes_handle[TEST_MES_NUM], mes_handle2[TEST_MES_NUM];
	CH_RTOS_TaskHandle task_handle[TEST_MES_NUM];
	int i, mes;

	memset(&mes_handle[0], 0, sizeof(mes_handle));
	memset(&task_handle[0], 0, sizeof(task_handle));

	//创建
	for(i = 0; i < TEST_MES_NUM; i++)
	{
		EXPECT_EQ(CH_RTOS_CreateMessageQuene(sizeof(mes), 8, &mes_handle[i]), CH_RTOS_OK);
		EXPECT_NE((void*)NULL, mes_handle[i]);
		EXPECT_EQ(CH_RTOS_CreateTask(test_task_mes, mes_handle[i], NULL, 8 * 1024, 150 + i, NULL, &task_handle[i]), CH_RTOS_OK);
		EXPECT_NE((void*)NULL, task_handle[i]);
	}

	//发送消息

	for(i = 0; i < TEST_MES_NUM; i++)
	{
		EXPECT_EQ(CH_RTOS_CreateMessageQuene(sizeof(mes), 8, &mes_handle2[i]), CH_RTOS_OK);
		EXPECT_NE((void*)NULL, mes_handle2[i]);
		
		mes = (int)mes_handle2[i];
		//LOG("send %d handle=0x%x,mes=%#x\n", i, mes_handle[i], mes);
		EXPECT_EQ(CH_RTOS_SendMessage(mes_handle[i], &mes, 1000), CH_RTOS_OK);
	}

	//接收消息

	for(i = 0; i < TEST_MES_NUM; i++)
	{
		mes = 0;
		EXPECT_EQ(CH_RTOS_ReceiveMessage(mes_handle2[i], &mes, 5000), CH_RTOS_OK) << "消息接收失败";
		//LOG("receive %d handle=0x%x,mes=%#x\n", i, mes_handle2[i], mes);
		EXPECT_EQ((int)mes_handle2[i], mes) << i << ".收到的消息内容不正确";

		EXPECT_EQ(CH_RTOS_ReceiveMessage(mes_handle2[i], &mes, 5000), CH_RTOS_OK) << "消息接收失败";
		//LOG("receive %d handle=0x%x,mes=%#x\n", i, mes_handle2[i], mes);
		EXPECT_EQ((int)mes_handle[i], mes) << i << ".收到的消息内容不正确";
	}

	//退出线程

	for(i = 0; i < TEST_MES_NUM; i++)
	{
		mes = 'q';
		EXPECT_EQ(CH_RTOS_SendMessage(mes_handle[i], &mes, 100), CH_RTOS_OK);
	}

	//删除线程和消息队列

	for(i = 0; i < TEST_MES_NUM; i++)
	{
		EXPECT_EQ(CH_RTOS_WaitTaskTerm(task_handle[i], 5000), CH_RTOS_OK);
		EXPECT_EQ(CH_RTOS_KillTask(task_handle[i]), CH_RTOS_OK);
		EXPECT_EQ(CH_RTOS_DeleteMessageQuene(mes_handle[i]), CH_RTOS_OK);
		EXPECT_EQ(CH_RTOS_DeleteMessageQuene(mes_handle2[i]), CH_RTOS_OK);
	}
}



CASE_F(TEST_RTOS_MUTEX1,
	"测试参数合法时能否成功创建和销毁一个mutex") {
	CH_RTOS_MutexHandle handle = NULL;

	EXPECT_EQ(CH_RTOS_CreateMutex(&handle, 0), CH_RTOS_OK);
	EXPECT_NE((void*)NULL, handle);
	EXPECT_EQ(CH_RTOS_DeleteMutex(handle), CH_RTOS_OK);
}

CASE_F(TEST_RTOS_MUTEX2,
	"测试能否重复销毁一个mutex") {
	CH_RTOS_MutexHandle handle = NULL;

	EXPECT_EQ(CH_RTOS_CreateMutex(&handle, 0), CH_RTOS_OK);
	EXPECT_NE((void*)NULL, handle);
	EXPECT_EQ(CH_RTOS_DeleteMutex(handle), CH_RTOS_OK);
	int dead_test = fw_getenv_i("DEAD_HALT_TEST");
	if(dead_test == 1)
	{
		EXPECT_NE(CH_RTOS_DeleteMutex(handle), CH_RTOS_OK) << "重复销毁MUTEX时不应死机";
	}
	else
	{
		EXPECT_EQ(dead_test, 1) << "应通过死亡测试";
	}
}

CASE_F(TEST_RTOS_MUTEX3,
	"测试能否正确lock一个合法的mutex") {
	CH_RTOS_MutexHandle handle = NULL;

	EXPECT_EQ(CH_RTOS_CreateMutex(&handle, 0), CH_RTOS_OK);
	EXPECT_NE((void*)NULL, handle);
	
	EXPECT_EQ(CH_RTOS_LockMutex(handle), CH_RTOS_OK);
	
	int dead_test = fw_getenv_i("DEAD_HALT_TEST");
	if(dead_test == 1)
	{
		EXPECT_EQ(CH_RTOS_DeleteMutex(handle), CH_RTOS_OK) << "mutex处于lock状态，但应可被删除";
	}
	else
	{
		EXPECT_EQ(dead_test, 1) << "应通过死亡测试";
	}
}

CASE_F(TEST_RTOS_MUTEX4,
	"测试能否lock一个已经被销毁的mutex") {
	CH_RTOS_MutexHandle handle = NULL;

	EXPECT_EQ(CH_RTOS_CreateMutex(&handle, 0), CH_RTOS_OK);
	EXPECT_NE((void*)NULL, handle);
	
	EXPECT_EQ(CH_RTOS_DeleteMutex(handle), CH_RTOS_OK);

	int dead_test = fw_getenv_i("DEAD_HALT_TEST");
	if(dead_test == 1)
	{
		EXPECT_NE(CH_RTOS_LockMutex(handle), CH_RTOS_OK) << "mutex已无效，操作不成功但不应死机";
	}
	else
	{
		EXPECT_EQ(dead_test, 1) << "应通过死亡测试";
	}
}

CASE_F(TEST_RTOS_MUTEX5,
	"测试能否unlock一个合法的mutex") {
	CH_RTOS_MutexHandle handle = NULL;

	EXPECT_EQ(CH_RTOS_CreateMutex(&handle, 0), CH_RTOS_OK);
	EXPECT_NE((void*)NULL, handle);

	EXPECT_EQ(CH_RTOS_LockMutex(handle), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_UnlockMutex(handle), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteMutex(handle), CH_RTOS_OK);
}

CASE_F(TEST_RTOS_MUTEX6,
	"测试能否重复unlock一个合法的mutex") {
	CH_RTOS_MutexHandle handle = NULL;

	EXPECT_EQ(CH_RTOS_CreateMutex(&handle, 0), CH_RTOS_OK);
	EXPECT_NE((void*)NULL, handle);

	EXPECT_EQ(CH_RTOS_LockMutex(handle), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_UnlockMutex(handle), CH_RTOS_OK);
	if(fw_getenv_i("DEAD_HALT_TEST") == 1)
	{
		EXPECT_NE(CH_RTOS_UnlockMutex(handle), CH_RTOS_OK) << "重复unlock mutex不应成功";
	}
	else
	{
		FAIL() << "应通过死亡测试";
	}
	EXPECT_EQ(CH_RTOS_DeleteMutex(handle), CH_RTOS_OK);
}

CASE_F(TEST_RTOS_MUTEX7,
	"测试能否unlock一个已经被销毁了的mutex") {
	CH_RTOS_MutexHandle handle = NULL;

	EXPECT_EQ(CH_RTOS_CreateMutex(&handle, 0), CH_RTOS_OK);
	EXPECT_NE((void*)NULL, handle);
	EXPECT_EQ(CH_RTOS_DeleteMutex(handle), CH_RTOS_OK);

	int dead_test = fw_getenv_i("DEAD_HALT_TEST");
	if(dead_test == 1)
	{
		EXPECT_NE(CH_RTOS_UnlockMutex(handle), CH_RTOS_OK) << "mutex已无效，操作不成功但不应死机";
	}
	else
	{
		EXPECT_EQ(dead_test, 1) << "应通过死亡测试";
	}	
}



CASE_F(TEST_RTOS_MUTEX8,
	"测试trylock一个已经被lock的mutex，是否成功") {
	CH_RTOS_MutexHandle handle = NULL;
	int ret;
	
	EXPECT_EQ(CH_RTOS_CreateMutex(&handle, 0), CH_RTOS_OK);
	EXPECT_NE((void*)NULL, handle);
	EXPECT_EQ(CH_RTOS_LockMutex(handle), CH_RTOS_OK);
	EXPECT_EQ(ret = CH_RTOS_TryLockMutex(handle), CH_RTOS_OK) << "LINUX系统会成功，其他系统可能会失败，无论成功失败都算通过";
	
	EXPECT_EQ(CH_RTOS_UnlockMutex(handle), CH_RTOS_OK);
	if(ret == CH_RTOS_OK)
	{
		EXPECT_EQ(CH_RTOS_UnlockMutex(handle), CH_RTOS_OK);
	}
	EXPECT_EQ(CH_RTOS_DeleteMutex(handle), CH_RTOS_OK);
}

CASE_F(TEST_RTOS_MUTEX9,
	"测试trylock一个unlock的mutex，是否成功") {
	CH_RTOS_MutexHandle handle = NULL;

	EXPECT_EQ(CH_RTOS_CreateMutex(&handle, 0), CH_RTOS_OK);
	EXPECT_NE((void*)NULL, handle);
	EXPECT_EQ(CH_RTOS_TryLockMutex(handle), CH_RTOS_OK) << "该mutex在unlock状态，应返回成功";
	EXPECT_EQ(CH_RTOS_UnlockMutex(handle), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteMutex(handle), CH_RTOS_OK);
}


static void test_task_mutex1(void *p)
{
	void**handle = (void**)p;

	CH_RTOS_TaskEnter(NULL);
	if(p)
	{
		CH_RTOS_SignalSemaphore(handle[1]);
		CH_RTOS_LockMutex(handle[0]);

		
		CH_RTOS_WaitSemaphore(handle[2], -1);
		CH_RTOS_UnlockMutex(handle[0]);
	}
	CH_RTOS_TaskExit();
}

CASE_F(TEST_RTOS_MUTEX10,
	"测试lock过程中，unlock是否成功") {
	void* handle[3] = {NULL, NULL, NULL};
	CH_RTOS_TaskHandle h_TaskHandle = NULL;
	
	EXPECT_EQ(CH_RTOS_CreateMutex(&handle[0], 0), CH_RTOS_OK);
	EXPECT_NE((void*)NULL, handle[0]);
	EXPECT_EQ(CH_RTOS_LockMutex(handle[0]), CH_RTOS_OK) << "该mutex在unlock状态，应返回成功";
	EXPECT_EQ(CH_RTOS_CreateSemaphore(0, &handle[1]), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_CreateSemaphore(0, &handle[2]), CH_RTOS_OK);

	EXPECT_EQ(CH_RTOS_CreateTask(test_task_mutex1, &handle[0], NULL, 8 * 1024, 150, NULL, &h_TaskHandle), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_WaitSemaphore(handle[1], -1), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_UnlockMutex(handle[0]), CH_RTOS_OK) << "该mutex已在另外一个线程中排队等候";

	EXPECT_EQ(CH_RTOS_SignalSemaphore(handle[2]), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_WaitTaskTerm(h_TaskHandle, 1000), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_KillTask(h_TaskHandle), CH_RTOS_OK);
		
	EXPECT_EQ(CH_RTOS_DeleteMutex(handle[0]), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteSemaphore(handle[1]), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteSemaphore(handle[2]), CH_RTOS_OK);
}

CASE_F(TEST_RTOS_MUTEX11,
	"测试能否在不同的线程里lock一个mutex并且只有一个能成功") {
	void* handle[3] = {NULL, NULL, NULL};
	CH_RTOS_TaskHandle h_TaskHandle = NULL;
	
	EXPECT_EQ(CH_RTOS_CreateMutex(&handle[0], 0), CH_RTOS_OK);
	EXPECT_NE((void*)NULL, handle[0]);

	EXPECT_EQ(CH_RTOS_CreateSemaphore(0, &handle[1]), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_CreateSemaphore(0, &handle[2]), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_CreateTask(test_task_mutex1, &handle[0], NULL, 8 * 1024, 150, NULL, &h_TaskHandle), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_WaitSemaphore(handle[1], -1), CH_RTOS_OK);
	CH_RTOS_TaskDelay(100);
	EXPECT_NE(CH_RTOS_TryLockMutex(handle[0]), CH_RTOS_OK) << "该mutex已在另外一个线程中被LOCK";

	EXPECT_EQ(CH_RTOS_SignalSemaphore(handle[2]), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_WaitTaskTerm(h_TaskHandle, 1000), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_KillTask(h_TaskHandle), CH_RTOS_OK);
	
	EXPECT_EQ(CH_RTOS_DeleteMutex(handle[0]), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteSemaphore(handle[1]), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteSemaphore(handle[2]), CH_RTOS_OK);
}


CASE_F(TEST_RTOS_MUTEX12,
	"测试能否一次性创建成功100个mutex") {
	void* handle[100] = {NULL, NULL};
	unsigned int i;
	
	memset(&handle[0], 0, sizeof(handle));
	for(i = 0; i < sizeof(handle)/sizeof(handle[0]); i++)
	{
		EXPECT_EQ(CH_RTOS_CreateMutex(&handle[i], 0), CH_RTOS_OK);
		EXPECT_NE((void*)NULL, handle[i]);
	}

	for(i = 0; i < sizeof(handle)/sizeof(handle[0]); i++)
	{
		EXPECT_EQ(CH_RTOS_DeleteMutex(handle[i]), CH_RTOS_OK);
	}
}


static void test_task_mutex2(void *p)
{
	void*handle = (void**)p;

	CH_RTOS_TaskHandle h_TaskHandle = NULL;

	CH_RTOS_GetCurrentTaskId(&h_TaskHandle);
	
	CH_RTOS_TaskEnter(NULL);
	if(p)
	{
		while(gb_mutex_task_run)
		{
			CH_RTOS_LockMutex(handle);
			gb_mutex_count ++;
			CH_RTOS_UnlockMutex(handle);
			CH_RTOS_TaskDelay(200);
		}
	}
	CH_RTOS_TaskExit();
}

static void test_task_mutex3(void *p)
{
	void*handle = (void**)p;
	
	CH_RTOS_TaskEnter(NULL);
	if(p)
	{
		while(gb_mutex_task_run)
		{
			CH_RTOS_LockMutex(handle);
			gb_mutex_count --;
			CH_RTOS_UnlockMutex(handle);
			CH_RTOS_TaskDelay(200);
		}
	}
	CH_RTOS_TaskExit();
}

CASE_F(TEST_RTOS_MUTEX13,
	"多个线程+多个mutex的强度测试") {
#define TEST_MUTEX_TASKNUM	32
	void* handle[2] = {NULL, NULL};
	CH_RTOS_TaskHandle h_TaskHandle[TEST_MUTEX_TASKNUM];
	int i;

	gb_mutex_task_run = 1;
	gb_mutex_count = 0;
	memset(&h_TaskHandle[0], 0, sizeof(h_TaskHandle));

	for(i = 0; i < 2; i++)
	{
		EXPECT_EQ(CH_RTOS_CreateMutex(&handle[i], 0), CH_RTOS_OK);
		EXPECT_NE((void*)NULL, handle[i]);
		EXPECT_EQ(CH_RTOS_LockMutex(handle[i]), CH_RTOS_OK);//先锁住
	}

	
	for(i = 0; i < TEST_MUTEX_TASKNUM / 2; i++)
	{
		EXPECT_EQ(CH_RTOS_CreateTask(test_task_mutex2, handle[0], NULL, 8 * 1024, 150, NULL, &h_TaskHandle[i]), CH_RTOS_OK);
		if(!h_TaskHandle[i])
		{
			break;
		}
	}
	for(i = TEST_MUTEX_TASKNUM / 2; i < TEST_MUTEX_TASKNUM; i++)
	{
		EXPECT_EQ(CH_RTOS_CreateTask(test_task_mutex3, handle[1], NULL, 8 * 1024, 150, NULL, &h_TaskHandle[i]), CH_RTOS_OK);
		if(!h_TaskHandle[i])
		{
			break;
		}
	}
	EXPECT_EQ(gb_mutex_count, 0) << "初始值应为0";

	//运行100次，反复检查mutex是否确实能够在多线程中保护数据
	for(i = 0; i < 100; i++)
	{
		EXPECT_EQ(CH_RTOS_UnlockMutex(handle[0]), CH_RTOS_OK);
		CH_RTOS_TaskDelay(200);
		EXPECT_EQ(CH_RTOS_LockMutex(handle[0]), CH_RTOS_OK);
		EXPECT_EQ(gb_mutex_count, TEST_MUTEX_TASKNUM / 2) << "累+线程执行完成，计总数应为TEST_MUTEX_TASKNUM/2, i=" << i;

		gb_mutex_count = TEST_MUTEX_TASKNUM / 2;
		EXPECT_EQ(CH_RTOS_UnlockMutex(handle[1]), CH_RTOS_OK);
		CH_RTOS_TaskDelay(200);
		EXPECT_EQ(CH_RTOS_LockMutex(handle[1]), CH_RTOS_OK);
		EXPECT_EQ(gb_mutex_count, 0) << "累-线程执行完成，计总数应为0, i=" << i;
		gb_mutex_count = 0;
	}

	gb_mutex_task_run = 0;

	
	EXPECT_EQ(CH_RTOS_UnlockMutex(handle[0]), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_UnlockMutex(handle[1]), CH_RTOS_OK);

	//删除所有任务
	for(i = 0; i < TEST_MUTEX_TASKNUM; i++)
	{
		if(h_TaskHandle[i])
		{
			EXPECT_EQ(CH_RTOS_WaitTaskTerm(h_TaskHandle[i], 1000), CH_RTOS_OK);
			EXPECT_EQ(CH_RTOS_KillTask(h_TaskHandle[i]), CH_RTOS_OK);
		}
	}
	//删除所有mutex
	for(i = 0; i < 2; i++)
	{
		EXPECT_EQ(CH_RTOS_DeleteMutex(handle[i]), CH_RTOS_OK);
	}
}


CASE_F(TEST_RTOS_EVENT1,
	"测试能否正常创建和删除一个event") {
	void* handle =  NULL;
	
	EXPECT_EQ(CH_RTOS_CreateEvent(&handle), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteEvent(handle), CH_RTOS_OK);
}

CASE_F(TEST_RTOS_EVENT2,
	"测试当rph_Event=NULL时能否创建even") {

	int dead_test = fw_getenv_i("DEAD_HALT_TEST");
	if(dead_test == 1)
	{
		EXPECT_NE(CH_RTOS_CreateEvent(NULL), CH_RTOS_OK) << "句柄无效，操作不成功但不应死机";
	}
	else
	{
		EXPECT_EQ(dead_test, 1) << "应通过死亡测试";
	}
}

CASE_F(TEST_RTOS_EVENT3,
	"测试能否重复删除一个event") {
	void* handle =  NULL;
	
	EXPECT_EQ(CH_RTOS_CreateEvent(&handle), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteEvent(handle), CH_RTOS_OK);

	int dead_test = fw_getenv_i("DEAD_HALT_TEST");
	if(dead_test == 1)
	{
		EXPECT_NE(CH_RTOS_DeleteEvent(handle), CH_RTOS_OK) << "句柄无效，操作不成功但不应死机";
	}
	else
	{
		EXPECT_EQ(dead_test, 1) << "应通过死亡测试";
	}
}

CASE_F(TEST_RTOS_EVENT4,
	"测试能否set一个已经被删除了的event") {
	void* handle =  NULL;
	
	EXPECT_EQ(CH_RTOS_CreateEvent(&handle), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteEvent(handle), CH_RTOS_OK);

	int dead_test = fw_getenv_i("DEAD_HALT_TEST");
	if(dead_test == 1)
	{
		EXPECT_NE(CH_RTOS_SetEvent(handle, 1), CH_RTOS_OK) << "句柄无效，操作不成功但不应死机";
	}
	else
	{
		EXPECT_EQ(dead_test, 1) << "应通过死亡测试";
	}
}

CASE_F(TEST_RTOS_EVENT5,
	"测试等待一个已经被删除了的event") {
	void* handle =  NULL;
	U32	ui_RetrievedEvents = 0;
	EXPECT_EQ(CH_RTOS_CreateEvent(&handle), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteEvent(handle), CH_RTOS_OK);

	int dead_test = fw_getenv_i("DEAD_HALT_TEST");
	if(dead_test == 1)
	{
		EXPECT_NE(CH_RTOS_WaitEvent(handle, 1, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_AND, 100), CH_RTOS_OK) << "句柄无效，操作不成功但不应死机";
	}
	else
	{
		EXPECT_EQ(dead_test, 1) << "应通过死亡测试";
	}
}

CASE_F(TEST_RTOS_EVENT6,
	"测试当rui_TargetEvents=0时是否可以成功等待event") {
	void* handle =  NULL;
	U32	ui_RetrievedEvents = 0;
	EXPECT_EQ(CH_RTOS_CreateEvent(&handle), CH_RTOS_OK);

	ui_RetrievedEvents = 0;
	EXPECT_EQ(CH_RTOS_WaitEvent(handle, 0, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_AND, 100), CH_RTOS_OK) << "目标事件为0，应等待成功";
	EXPECT_EQ(CH_RTOS_WaitEvent(handle, 0, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_OR, 100), CH_RTOS_OK) << "目标事件为0，应等待成功";
	EXPECT_EQ(CH_RTOS_DeleteEvent(handle), CH_RTOS_OK);
}

CASE_F(TEST_RTOS_EVENT7,
	"测试当rpui_RetrievedEvents=NULL是否可以成功等待event") {
	void* handle =  NULL;
	
	EXPECT_EQ(CH_RTOS_CreateEvent(&handle), CH_RTOS_OK);

	int dead_test = fw_getenv_i("DEAD_HALT_TEST");
	if(dead_test == 1)
	{
		EXPECT_NE(CH_RTOS_WaitEvent(handle, 1, NULL, CH_RTOS_EVENT_OP_AND, 100), CH_RTOS_OK) << "输出参数无效，操作不成功但不应死机";
	}
	else
	{
		EXPECT_EQ(dead_test, 1) << "应通过死亡测试";
	}
	EXPECT_EQ(CH_RTOS_DeleteEvent(handle), CH_RTOS_OK);
}

CASE_F(TEST_RTOS_EVENT8,
	"测试当renm_OpMode=CH_RTOS_EVENT_OP_AND时，是否全部事件发生了才返回成功") {
	void* handle =  NULL;
	U32	ui_RetrievedEvents = 0;
	
	EXPECT_EQ(CH_RTOS_CreateEvent(&handle), CH_RTOS_OK);

	EXPECT_EQ(CH_RTOS_SetEvent(handle, 0x1), CH_RTOS_OK);
	EXPECT_NE(CH_RTOS_WaitEvent(handle, 0x1111, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_AND, 500), CH_RTOS_OK) << "必须等目标事件收齐以后才能返回成功";
	EXPECT_EQ(CH_RTOS_SetEvent(handle, 0x10), CH_RTOS_OK);
	EXPECT_NE(CH_RTOS_WaitEvent(handle, 0x1111, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_AND, 500), CH_RTOS_OK) << "必须等目标事件收齐以后才能返回成功";
	EXPECT_EQ(CH_RTOS_SetEvent(handle, 0x100), CH_RTOS_OK);
	EXPECT_NE(CH_RTOS_WaitEvent(handle, 0x1111, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_AND, 500), CH_RTOS_OK) << "必须等目标事件收齐以后才能返回成功";
	EXPECT_EQ(CH_RTOS_SetEvent(handle, 0x1000), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_WaitEvent(handle, 0x1111, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_AND, 500), CH_RTOS_OK) << "目标事件已全部发生，应返回成功";
	EXPECT_EQ(0x1111, (U32)ui_RetrievedEvents);

	EXPECT_EQ(CH_RTOS_SetEvent(handle, 0x1111), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_WaitEvent(handle, 0x111, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_AND, 500), CH_RTOS_OK) << "目标事件已全部发生，应返回成功";
	EXPECT_EQ(0x111, (U32)ui_RetrievedEvents);
	EXPECT_EQ(CH_RTOS_WaitEvent(handle, 0x1000, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_AND, 500), CH_RTOS_OK) << "未取完的事件不应被清除";
	EXPECT_EQ(0x1000, (U32)ui_RetrievedEvents);
	EXPECT_NE(CH_RTOS_WaitEvent(handle, 0x1000, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_AND, 500), CH_RTOS_OK) << "事件已被取完，应返回超时";
	
	EXPECT_EQ(CH_RTOS_DeleteEvent(handle), CH_RTOS_OK);
}

CASE_F(TEST_RTOS_EVENT9,
	"测试当renm_OpMode=CH_RTOS_EVENT_OP_OR时，是否任一事件发生了即可返回成功") {
	void* handle =  NULL;
	U32	ui_RetrievedEvents = 0;
	
	EXPECT_EQ(CH_RTOS_CreateEvent(&handle), CH_RTOS_OK);

	ui_RetrievedEvents = 0;
	EXPECT_EQ(CH_RTOS_SetEvent(handle, 0x1), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_WaitEvent(handle, 0x1111, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_OR, 500), CH_RTOS_OK) << "任一事件发生即返回成功";
	EXPECT_EQ(0x1, ui_RetrievedEvents);

	ui_RetrievedEvents = 0;
	EXPECT_EQ(CH_RTOS_SetEvent(handle, 0x10), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_WaitEvent(handle, 0x1111, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_OR, 500), CH_RTOS_OK) << "任一事件发生即返回成功";
	EXPECT_EQ(0x10, ui_RetrievedEvents);

	ui_RetrievedEvents = 0;
	EXPECT_EQ(CH_RTOS_SetEvent(handle, 0x100), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_WaitEvent(handle, 0x1111, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_OR, 500), CH_RTOS_OK) << "任一事件发生即返回成功";
	EXPECT_EQ(0x100, ui_RetrievedEvents);

	ui_RetrievedEvents = 0;
	EXPECT_EQ(CH_RTOS_SetEvent(handle, 0x111000), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_WaitEvent(handle, 0x1111, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_OR, 500), CH_RTOS_OK) << "任一事件发生即返回成功";
	EXPECT_EQ(0x1000, ui_RetrievedEvents);

	ui_RetrievedEvents = 0;
	EXPECT_EQ(CH_RTOS_WaitEvent(handle, (U32)(-1), &ui_RetrievedEvents, CH_RTOS_EVENT_OP_OR, 500), CH_RTOS_OK) << "任一事件发生即返回成功";
	EXPECT_EQ(0x110000, ui_RetrievedEvents) << "剩余的事件不应被清除";

	EXPECT_NE(CH_RTOS_WaitEvent(handle, (U32)(-1), &ui_RetrievedEvents, CH_RTOS_EVENT_OP_OR, 500), CH_RTOS_OK) << "事件已被取完，应返回超时";
	
	EXPECT_EQ(CH_RTOS_DeleteEvent(handle), CH_RTOS_OK);
}


CASE_F(TEST_RTOS_EVENT10,
	"测试当ri_TimeOutMS=0时，等待函数是否立即返回") {
	void* handle =  NULL;
	U32	ui_RetrievedEvents = 0;
	U32 time1;
	EXPECT_EQ(CH_RTOS_CreateEvent(&handle), CH_RTOS_OK);

	time1 = CH_RTOS_GetSysTimenow();
	EXPECT_NE(CH_RTOS_WaitEvent(handle, 0x1110, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_AND, 0), CH_RTOS_OK);
	EXPECT_LT(CH_RTOS_TimeMinus(CH_RTOS_GetSysTimenow(), time1), 50) << "等待时间为0，应立即返回";
	
	EXPECT_EQ(CH_RTOS_DeleteEvent(handle), CH_RTOS_OK);
}

static void test_task_event1(void *p)
{
	void*handle = (void*)p;
	
	CH_RTOS_TaskEnter(NULL);
	if(p)
	{
		CH_RTOS_TaskDelay(30000);
		CH_RTOS_SetEvent(handle, 0x1110);
	}
	CH_RTOS_TaskExit();
}

CASE_F(TEST_RTOS_EVENT11,
	"测试当ri_TimeOutMS=-1时，等待函数是否一直等待") {
	void* handle =  NULL;
	U32	ui_RetrievedEvents = 0;
	U32 time1;
	CH_RTOS_TaskHandle h_TaskHandle;
	EXPECT_EQ(CH_RTOS_CreateEvent(&handle), CH_RTOS_OK);

	EXPECT_EQ(CH_RTOS_CreateTask(test_task_event1, handle, NULL, 8 * 1024, 150, NULL, &h_TaskHandle), CH_RTOS_OK);
	time1 = CH_RTOS_GetSysTimenow();
	EXPECT_EQ(CH_RTOS_WaitEvent(handle, 0x1110, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_AND, -1), CH_RTOS_OK);
	EXPECT_GT(CH_RTOS_TimeMinus(CH_RTOS_GetSysTimenow(), time1), 25000) << "无限等待时间应大于30S";
	
	EXPECT_EQ(CH_RTOS_DeleteEvent(handle), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_WaitTaskTerm(h_TaskHandle, 100), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteTask(h_TaskHandle), CH_RTOS_OK);
}

CASE_F(TEST_RTOS_EVENT12,
	"测试当无事件发生时，等待函数等待时间是否与ri_TimeOutMS一致") {
	void* handle =  NULL;
	U32	ui_RetrievedEvents = 0;
	U32 time1, i;
	U32 timeout[] = {100,200, 300, 400, 500, 1000, 2000, 5000, 0};
	
	EXPECT_EQ(CH_RTOS_CreateEvent(&handle), CH_RTOS_OK);

	for(i = 0; timeout[i] != 0; i++)
	{
		time1 = CH_RTOS_GetSysTimenow();
		EXPECT_NE(CH_RTOS_WaitEvent(handle, 0x1110, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_AND, timeout[i]), CH_RTOS_OK);
		time1 = CH_RTOS_TimeMinus(CH_RTOS_GetSysTimenow(), time1);
		EXPECT_LE(time1, timeout[i] + 50) << "超时时间误差应在上下50ms以内";
		EXPECT_GE(time1, timeout[i] - 50) << "超时时间误差应在上下50ms以内";
	}
	
	EXPECT_EQ(CH_RTOS_DeleteEvent(handle), CH_RTOS_OK);
}

CASE_F(TEST_RTOS_EVENT13,
	"测试多次set同一个事件后再发起等待，是否只能等待成功一次") {
	void* handle =  NULL;
	U32	ui_RetrievedEvents = 0, i;
	EXPECT_EQ(CH_RTOS_CreateEvent(&handle), CH_RTOS_OK);

	for(i = 0; i < 10; i++)
	{
		EXPECT_EQ(CH_RTOS_SetEvent(handle, 0x1110), CH_RTOS_OK);
	}
	EXPECT_EQ(CH_RTOS_WaitEvent(handle, 0x1110, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_AND, 10), CH_RTOS_OK);
	EXPECT_NE(CH_RTOS_WaitEvent(handle, 0x1110, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_AND, 10), CH_RTOS_OK) << "同一事件只能等待成功一次";
	EXPECT_NE(CH_RTOS_WaitEvent(handle, 0x1110, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_OR, 10), CH_RTOS_OK)<< "同一事件只能等待成功一次";
	
	for(i = 0; i < 10; i++)
	{
		EXPECT_EQ(CH_RTOS_SetEvent(handle, 0x1), CH_RTOS_OK);
	}
	EXPECT_EQ(CH_RTOS_WaitEvent(handle, 0x1, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_OR, 10), CH_RTOS_OK);
	EXPECT_NE(CH_RTOS_WaitEvent(handle, 0x1, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_AND, 10), CH_RTOS_OK)<< "同一事件只能等待成功一次";
	EXPECT_NE(CH_RTOS_WaitEvent(handle, 0x1, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_OR, 10), CH_RTOS_OK)<< "同一事件只能等待成功一次";
	
	EXPECT_EQ(CH_RTOS_DeleteEvent(handle), CH_RTOS_OK);
}

static void test_task_event2(void *p)
{
	void*handle = (void*)p;
	
	CH_RTOS_TaskEnter(NULL);
	if(p)
	{
		CH_RTOS_TaskDelay(100);
		CH_RTOS_SetEvent(handle, 0x1110);
	}
	CH_RTOS_TaskExit();
}

CASE_F(TEST_RTOS_EVENT14,
	"测试等待过程中set事件，是否等待成功") {
	void* handle =  NULL;
	U32	ui_RetrievedEvents = 0;
	CH_RTOS_TaskHandle h_TaskHandle;
	EXPECT_EQ(CH_RTOS_CreateEvent(&handle), CH_RTOS_OK);

	EXPECT_EQ(CH_RTOS_CreateTask(test_task_event2, handle, NULL, 8 * 1024, 150, NULL, &h_TaskHandle), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_WaitEvent(handle, 0x1110, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_AND, 200), CH_RTOS_OK);
	EXPECT_EQ(0x1110, ui_RetrievedEvents);
	EXPECT_EQ(CH_RTOS_DeleteEvent(handle), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_WaitTaskTerm(h_TaskHandle, 100), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteTask(h_TaskHandle), CH_RTOS_OK);
}




#if 0 	/*sqzou20170925*/
CASE_F(TEST_RTOS_MUTEX15,
	"多个线程+多个EVENT的强度测试") {
#define TEST_EVENT_TASKNUM	32
	void* handle = NULL;
	CH_RTOS_TaskHandle h_TaskHandle[TEST_EVENT_TASKNUM];
	int i;

	gb_mutex_count = 0;
	memset(&h_TaskHandle[0], 0, sizeof(h_TaskHandle));

	EXPECT_EQ(CH_RTOS_CreateEvent(&handle), CH_RTOS_OK);
	
	for(i = 0; i < TEST_EVENT_TASKNUM; i++)
	{
		EXPECT_EQ(CH_RTOS_CreateTask(test_task_event3, handle, NULL, 8 * 1024, 150, NULL, &h_TaskHandle[i]), CH_RTOS_OK);
	}


	EXPECT_EQ(gb_mutex_count, 0) << "初始值应为0";

	//反复测试发送一个event，多个线程同时等待，是否只有一个线程能获取成功
	for(i = 1; i < TEST_EVENT_TASKNUM; i++)
	{
		EXPECT_EQ(CH_RTOS_SetEvent(handle, i*32), CH_RTOS_OK);
		CH_RTOS_TaskDelay(200);
		//LOG("gb_mutex_count = %d, i = %d.\n", gb_mutex_count, i);
		EXPECT_EQ(gb_mutex_count, i*32) << "多个线程在等待事件，但只有一个线程能获取成功, i=" << i;
		gb_mutex_count = 0;
	}

	
	//删除所有任务
	for(i = 0; i < TEST_EVENT_TASKNUM; i++)
	{
		EXPECT_EQ(CH_RTOS_WaitTaskTerm(h_TaskHandle[i], 5000), CH_RTOS_OK);
		EXPECT_EQ(CH_RTOS_KillTask(h_TaskHandle[i]), CH_RTOS_OK);
	}
	
	//删除所有event
	EXPECT_EQ(CH_RTOS_DeleteEvent(handle), CH_RTOS_OK);

}

	
#endif

static void test_timer_sem1(void *p)
{
	if(gh_timer_sem)
	{
		CH_RTOS_SignalSemaphore(gh_timer_sem);
	}
}


CASE_F(TEST_RTOS_TIMER1,
	"测试能否以合法参数创建成功一个一次性timer")
{
	EXPECT_EQ(CH_RTOS_CreateSemaphore(0, &gh_timer_sem), CH_RTOS_OK);
	EXPECT_NE((int)(gh_timer_sem), 0) <<"信号量输出句柄不应为0";

	EXPECT_EQ(CH_RTOS_CreateTimer(test_timer_sem1, (U32)(gh_timer_sem), 10, CH_RTOS_TIMER_MODE_ONE, &gh_timer), CH_RTOS_OK);
	EXPECT_NE((int)(gh_timer), 0) <<"TIMER输出句柄不应为0";

}

CASE_F(TEST_RTOS_TIMER2,
	"测试刚刚创建的timer未start前没有运行")
{
	EXPECT_NE(CH_RTOS_WaitSemaphore(gh_timer_sem, 1000), CH_RTOS_OK) << "TIMER未start，不应被执行";
}

CASE_F(TEST_RTOS_TIMER3,
	"测试能否正常start刚刚创建的timer并使其正常运行")
{
	EXPECT_EQ(CH_RTOS_StartTimer(gh_timer), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_WaitSemaphore(gh_timer_sem, 5000), CH_RTOS_OK) << "TIMER已start，应被正确执行";
}

CASE_F(TEST_RTOS_TIMER4,
	"测试能正常stop正在运行的timer")
{
	EXPECT_NE(CH_RTOS_WaitSemaphore(gh_timer_sem, 1000), CH_RTOS_OK) << "一次性timer，只能执行一次";
	
	EXPECT_EQ(CH_RTOS_StopTimer(gh_timer), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteTimer(gh_timer), CH_RTOS_OK);
	gh_timer = NULL;
	EXPECT_EQ(CH_RTOS_DeleteSemaphore(gh_timer_sem), CH_RTOS_OK);
	gh_timer_sem = NULL;
}

static void test_timer_sem2(void *p)
{
	CH_RTOS_SemaphoreHandle *pSem = (CH_RTOS_SemaphoreHandle *)p;
	
	if(pSem)
	{
		CH_RTOS_SignalSemaphore(pSem);
	}
}

CASE_F(TEST_RTOS_TIMER5,
	"测试能否以合法参数创建成功一个永久timer并能正常运行")
{
	EXPECT_EQ(CH_RTOS_CreateSemaphore(0, &gh_timer_sem), CH_RTOS_OK);
	EXPECT_NE((int)(gh_timer_sem), 0) <<"信号量输出句柄不应为0";

	EXPECT_EQ(CH_RTOS_CreateTimer(test_timer_sem2, (U32)(gh_timer_sem), 10, CH_RTOS_TIMER_MODE_CONTINUE, &gh_timer), CH_RTOS_OK);
	EXPECT_NE((int)(gh_timer), 0) <<"TIMER输出句柄不应为0";

	EXPECT_EQ(CH_RTOS_StartTimer(gh_timer), CH_RTOS_OK);

	for(int i = 0; i < 10; i++)
	{
		EXPECT_EQ(CH_RTOS_WaitSemaphore(gh_timer_sem, 5000), CH_RTOS_OK);
	}

	EXPECT_EQ(CH_RTOS_StopTimer(gh_timer), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteTimer(gh_timer), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteSemaphore(gh_timer_sem), CH_RTOS_OK);
	gh_timer_sem = NULL;
	gh_timer = NULL;
}

CASE_F(TEST_RTOS_TIMER6,
	"测试能正常删除一个正在运行的timer")
{
	EXPECT_EQ(CH_RTOS_CreateSemaphore(0, &gh_timer_sem), CH_RTOS_OK);
	EXPECT_NE((int)(gh_timer_sem), 0) <<"信号量输出句柄不应为0";

	EXPECT_EQ(CH_RTOS_CreateTimer(test_timer_sem2, (U32)(gh_timer_sem), 10, CH_RTOS_TIMER_MODE_CONTINUE, &gh_timer), CH_RTOS_OK);
	EXPECT_NE((int)(gh_timer), 0) <<"TIMER输出句柄不应为0";

	EXPECT_EQ(CH_RTOS_StartTimer(gh_timer), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteTimer(gh_timer), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteSemaphore(gh_timer_sem), CH_RTOS_OK);
	gh_timer_sem = NULL;
	gh_timer = NULL;
}

CASE_F(TEST_RTOS_TIMER7,
	"测试一次性创建16个timer，并且都能成功")
{
	CH_RTOS_TimerHandle h_timer[16];
	unsigned int i;

	for(i = 0; i < sizeof(h_timer)/sizeof(h_timer[0]); i++)
	{
		EXPECT_EQ(CH_RTOS_CreateTimer(test_timer_sem2, 0, 10, CH_RTOS_TIMER_MODE_CONTINUE, &h_timer[i]), CH_RTOS_OK);
		EXPECT_NE((int)(h_timer[i]), 0) <<"TIMER输出句柄不应为0,i=" << i;
		if(h_timer[i] == NULL)
		{
			break;
		}
		EXPECT_EQ(CH_RTOS_StartTimer(h_timer[i]), CH_RTOS_OK);
	}

	for(i = 0; i < sizeof(h_timer)/sizeof(h_timer[0]); i++)
	{
		if(h_timer[i] != NULL)
		{
			EXPECT_EQ(CH_RTOS_StopTimer(h_timer[i]), CH_RTOS_OK);
			EXPECT_EQ(CH_RTOS_DeleteTimer(h_timer[i]), CH_RTOS_OK);
		}
	}
}

CASE_F(TEST_RTOS_MEM1,
	"测试1000个字节内存的分配和释放是否成功")
{
	void *p;

	EXPECT_NE(p = CH_RTOS_AllocateMemory(gh_APP_HEAP, 1000), (void*)NULL);
	
	EXPECT_EQ(CH_RTOS_FreeMemory(gh_APP_HEAP, p), CH_RTOS_OK);
}

CASE_F(TEST_RTOS_MEM2,
	"测试8M内存的分配和释放是否成功")
{
	void *p;

	EXPECT_NE(p = CH_RTOS_AllocateMemory(gh_APP_HEAP, 0x800000), (void*)NULL);
	EXPECT_EQ(CH_RTOS_FreeMemory(gh_APP_HEAP, p), CH_RTOS_OK);
}

CASE_F(TEST_RTOS_MEM3,
	"测试内存大小不是4的整数倍的分配和释放是否成功")
{
	void *p;
	int i, size = 16;

	for(i = 1; i < 4; i++)
	{
		EXPECT_NE(p = CH_RTOS_AllocateMemory(gh_APP_HEAP, size + i), (void*)NULL);
		EXPECT_EQ(CH_RTOS_FreeMemory(gh_APP_HEAP, p), CH_RTOS_OK);
	}
}

CASE_F(TEST_RTOS_MEM4,
	"测试分配大小为0字节的内存是否成功")
{
	void *p;

	EXPECT_EQ(p = CH_RTOS_AllocateMemory(gh_APP_HEAP, 0), (void*)NULL);
	if(p)
	{
		CH_RTOS_FreeMemory(gh_APP_HEAP, p);
	}
}

CASE_F(TEST_RTOS_MEM5,
	"测试小块内存的分配和释放是否成功")
{
	void *p;
	int i;

	for(i = 1; i < 256; i++)
	{
		EXPECT_NE(p = CH_RTOS_AllocateMemory(gh_APP_HEAP, i), (void*)NULL);
		EXPECT_EQ(CH_RTOS_FreeMemory(gh_APP_HEAP, p), CH_RTOS_OK);
	}
}

CASE_F(TEST_RTOS_REALLOC1,
	"测试重分配的内存大小小于申请的内存大小的情况能否重分配成功")
{
	void *p;

	EXPECT_NE(p = CH_RTOS_AllocateMemory(gh_APP_HEAP, 256), (void*)NULL);
	EXPECT_NE(p = CH_RTOS_ReallocMemory(gh_APP_HEAP, p, 4), (void*)NULL);
	EXPECT_EQ(CH_RTOS_FreeMemory(gh_APP_HEAP, p), CH_RTOS_OK);
}

CASE_F(TEST_RTOS_REALLOC2,
	"测试重分配的内存大小大于申请的内存大小的情况能否重分配成功")
{
	void *p;

	EXPECT_NE(p = CH_RTOS_AllocateMemory(gh_APP_HEAP, 16), (void*)NULL);
	EXPECT_NE(p = CH_RTOS_ReallocMemory(gh_APP_HEAP, p, 128), (void*)NULL);
	EXPECT_EQ(CH_RTOS_FreeMemory(gh_APP_HEAP, p), CH_RTOS_OK);
}

CASE_F(TEST_RTOS_REALLOC3,
	"测试重分配的内存地址pv_MemAddr等于NULL的情况能否重分配成功")
{
	void *p;

	EXPECT_NE(p = CH_RTOS_ReallocMemory(gh_APP_HEAP, NULL, 128), (void*)NULL);
	if(p)
	{
		CH_RTOS_FreeMemory(gh_APP_HEAP, p);
	}
}

CASE_F(TEST_RTOS_GETFREEMEM,
	"测试CH_RTOS_GetFreeMemory获取的值是否准确")
{
#define TEST_GETFREE_ALLOC_SIZE 0x100000
	void *p1, *p2;
	U32 ui_size1 = 0, ui_size2= 0, ui_large_size = 0;

	EXPECT_EQ(CH_RTOS_GetFreeMemory(gh_APP_HEAP, &ui_size1, &ui_large_size), CH_RTOS_OK);
	EXPECT_NE(p1 = CH_RTOS_AllocateMemory(gh_APP_HEAP, TEST_GETFREE_ALLOC_SIZE), (void*)NULL);
	if(p1)
	{
		memset(p1, 0xf2, TEST_GETFREE_ALLOC_SIZE);
	}
	EXPECT_EQ(CH_RTOS_GetFreeMemory(gh_APP_HEAP, &ui_size2, &ui_large_size), CH_RTOS_OK);
	EXPECT_LE(TEST_GETFREE_ALLOC_SIZE, ui_size2 - ui_size1);
	ui_size1 = ui_size2;
	EXPECT_NE(p2 = CH_RTOS_AllocateMemory(gh_APP_HEAP, TEST_GETFREE_ALLOC_SIZE), (void*)NULL);
	if(p2)
	{
		memset(p2, 0xf3, TEST_GETFREE_ALLOC_SIZE);
	}
	EXPECT_EQ(CH_RTOS_GetFreeMemory(gh_APP_HEAP, &ui_size2, &ui_large_size), CH_RTOS_OK);
	EXPECT_LE(TEST_GETFREE_ALLOC_SIZE, ui_size2 - ui_size1);
	
	EXPECT_EQ(CH_RTOS_FreeMemory(gh_APP_HEAP, p1), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_FreeMemory(gh_APP_HEAP, p2), CH_RTOS_OK);
}

CASE_F(TEST_RTOS_MEMCPY,
	"测试CH_RTOS_Memcpy是否成功")
{
#define TEST_MEMCPY_ALLOC_SIZE 0x1000
	void*p1, *p2;
	
	EXPECT_NE(p1 = CH_RTOS_AllocateMemory(gh_APP_HEAP, TEST_MEMCPY_ALLOC_SIZE), (void*)NULL);
	if(p1)
	{
		memset(p1, 0xf2, TEST_MEMCPY_ALLOC_SIZE);
	}

	EXPECT_NE(p2 = CH_RTOS_AllocateMemory(gh_APP_HEAP, TEST_MEMCPY_ALLOC_SIZE), (void*)NULL);
	if(p2)
	{
		memset(p2, 0xc3, TEST_MEMCPY_ALLOC_SIZE);
	}
	CH_RTOS_Memcpy(p1, p2, TEST_MEMCPY_ALLOC_SIZE);
	EXPECT_EQ(memcmp(p1, p2, TEST_MEMCPY_ALLOC_SIZE), 0);
	
	EXPECT_EQ(CH_RTOS_FreeMemory(gh_APP_HEAP, p1), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_FreeMemory(gh_APP_HEAP, p2), CH_RTOS_OK);
}

CASE_F(TEST_RTOS_TIME1,
	"测试CH_RTOS_GetSysTicknow获取的值是否正确")
{
	U32 tick1, tick2;

	EXPECT_NE(tick1 = CH_RTOS_GetSysTicknow(), 0);
	CH_RTOS_TaskDelay(1000);
	EXPECT_NE(tick2 = CH_RTOS_GetSysTicknow(), 0);
	EXPECT_LE(CH_RTOS_CvtTimeToTick(1000), (tick2 - tick1) * 102 /100) << "误差小于2%";
	EXPECT_GE(CH_RTOS_CvtTimeToTick(1000), (tick2 - tick1) * 98 /100) << "误差小于2%";;
}

CASE_F(TEST_RTOS_TIME2,
	"测试CH_RTOS_GetSysTimenow获取的值是否正确")
{
	U32 time1, time2;

	EXPECT_NE(time1 = CH_RTOS_GetSysTimenow(), 0);
	CH_RTOS_TaskDelay(1000);
	EXPECT_NE(time2 = CH_RTOS_GetSysTimenow(), 0);
	EXPECT_LE((1000), (time2 - time1) * 102 /100) << "误差小于土2%";
	EXPECT_GE((1000), (time2 - time1) * 98 /100) << "误差小于土2%";;
}

CASE_F(TEST_RTOS_TIME3,
	"测试CH_RTOS_GetSysTimenow获取的值是否正确")
{
	U32 time1, time2;

	EXPECT_NE(time1 = CH_RTOS_GetSysTimenow(), 0);
	CH_RTOS_TaskDelay(1000);
	EXPECT_NE(time2 = CH_RTOS_GetSysTimenow(), 0);
	EXPECT_LE((1000), (time2 - time1) * 102 /100) << "误差小于土2%";
	EXPECT_GE((1000), (time2 - time1) * 98 /100) << "误差小于土2%";;
}

CASE_F(TEST_RTOS_TIME4,
	"测试CH_RTOS_CvtTimeToTick换算是否正确")
{
	U32 tick1, tick2;

	EXPECT_NE(tick1 = CH_RTOS_GetSysTicknow(), 0);
	CH_RTOS_TaskDelay(1000);
	EXPECT_NE(tick2 = CH_RTOS_GetSysTicknow(), 0);
	EXPECT_LE(CH_RTOS_CvtTimeToTick(1000), (tick2 - tick1) * 102 /100) << "误差小于2%";
	EXPECT_GE(CH_RTOS_CvtTimeToTick(1000), (tick2 - tick1) * 98 /100) << "误差小于2%";;
}

CASE_F(TEST_RTOS_TIME5,
	"测试CH_RTOS_TimeTickCmp是否正确")
{
	U32 tick1, tick2;

	EXPECT_NE(tick1 = CH_RTOS_GetSysTicknow(), 0);
	CH_RTOS_TaskDelay(1);
	EXPECT_NE(tick2 = CH_RTOS_GetSysTicknow(), 0);
	EXPECT_EQ(CH_RTOS_TimeTickCmp(tick1, tick2), 0);
	EXPECT_EQ(CH_RTOS_TimeTickCmp(tick2, tick1), 1);
}

CASE_F(TEST_RTOS_TIME6,
	"测试CH_RTOS_TimeCmp是否正确")
{
	U32 time1, time2;

	EXPECT_NE(time1 = CH_RTOS_GetSysTimenow(), 0);
	CH_RTOS_TaskDelay(1);
	EXPECT_NE(time2 = CH_RTOS_GetSysTimenow(), 0);
	EXPECT_EQ(CH_RTOS_TimeCmp(time1, time2), 0);
	EXPECT_EQ(CH_RTOS_TimeCmp(time2, time1), 1);
}

CASE_F(TEST_RTOS_TIME7,
	"测试CH_RTOS_TimeTickPlus换算是否正确")
{
	U32 tick1, tick2;

	EXPECT_NE(tick1 = CH_RTOS_GetSysTicknow(), 0);
	CH_RTOS_TaskDelay(1000);
	EXPECT_NE(tick2 = CH_RTOS_GetSysTicknow(), 0);
	EXPECT_LE(tick2, CH_RTOS_TimeTickPlus(tick1, CH_RTOS_CvtTimeToTick(1000))* 102 /100) << "误差小于土2%";
	EXPECT_GE(tick2, CH_RTOS_TimeTickPlus(tick1, CH_RTOS_CvtTimeToTick(1000)) * 98 /100) << "误差小于土2%";;
}

CASE_F(TEST_RTOS_TIME8,
	"测试CH_RTOS_TimePlus换算是否正确")
{
	U32 time1, time2;

	EXPECT_NE(time1 = CH_RTOS_GetSysTimenow(), 0);
	CH_RTOS_TaskDelay(1000);
	EXPECT_NE(time2 = CH_RTOS_GetSysTimenow(), 0);
	EXPECT_LE(time2, CH_RTOS_TimeTickPlus(time1, 1000)* 102 /100) << "误差小于土2%";
	EXPECT_GE(time2, CH_RTOS_TimeTickPlus(time1, 1000) * 98 /100) << "误差小于土2%";;
}

CASE_F(TEST_RTOS_TIME9,
	"测试CH_RTOS_TimeTickMinus换算是否正确")
{
	U32 tick1, tick2;

	EXPECT_NE(tick1 = CH_RTOS_GetSysTicknow(), 0);
	CH_RTOS_TaskDelay(1000);
	EXPECT_NE(tick2 = CH_RTOS_GetSysTicknow(), 0);
	EXPECT_LE(tick1, CH_RTOS_TimeTickMinus(tick2, CH_RTOS_CvtTimeToTick(1000))* 102 /100) << "误差小于土2%";
	EXPECT_GE(tick1, CH_RTOS_TimeTickMinus(tick2, CH_RTOS_CvtTimeToTick(1000)) * 98 /100) << "误差小于土2%";
	EXPECT_EQ(3000, CH_RTOS_TimeTickMinus(2000, (U32)(-1000))) << "测试时间溢出后的处理是否正确";
}

CASE_F(TEST_RTOS_TIME10,
	"测试CH_RTOS_TimeMinus换算是否正确")
{
	U32 time1, time2;

	EXPECT_NE(time1 = CH_RTOS_GetSysTicknow(), 0);
	CH_RTOS_TaskDelay(1000);
	EXPECT_NE(time2 = CH_RTOS_GetSysTicknow(), 0);
	EXPECT_LE(time1, CH_RTOS_TimeMinus(time2, 1000)* 102 /100) << "误差小于土2%";
	EXPECT_GE(time1, CH_RTOS_TimeMinus(time2, 1000) * 98 /100) << "误差小于土2%";
	EXPECT_EQ(3000, CH_RTOS_TimeTickMinus(2000, (U32)(-1000))) << "测试时间溢出后的处理是否正确";
}

#endif /* ENABLE_TESTCASE */



















static int rtos_init(int argc, CMD_PARAM argv[])
{
	int ret;

	ret = CH_RTOS_Init();
	if(ret != CH_RTOS_OK)
	{
		LOG("ERROR: CH_RTOS_Init return %d\n", ret);
		return ret;
	}
	
	return 0;
}

static int rtos_term(int argc, CMD_PARAM argv[])
{
	int ret;

	ret = CH_RTOS_Term();
	if(ret != CH_RTOS_OK)
	{
		LOG("ERROR: CH_RTOS_Term return %d\n", ret);
		return ret;
	}
	
	return 0;
}


static int rtos_ver(int argc, CMD_PARAM argv[])
{
	int ret;
	CHDRV_Version_t stru_Ver;
	
	ret = CH_RTOS_GetVersion(&stru_Ver);
	if(ret != CH_RTOS_OK)
	{
		LOG("ERROR: CH_RTOS_GetVersion return %d\n", ret);
		return ret;
	}
	LOG("version=0x%x\n",stru_Ver);
	return 0;
}

static int rtos_malloc(int argc, CMD_PARAM argv[])
{
	void *p;

	p = CH_RTOS_AllocateMemory(gh_APP_HEAP, argv[2].get_i());
	
	LOG("CH_RTOS_AllocateMemory(%d)=0x%x\n",argv[2].get_i(), p);
	return p ? 0 : -1;
}

static int rtos_free(int argc, CMD_PARAM argv[])
{
	LOG("CH_RTOS_FreeMemory(0x%x)\n",argv[2].get_i());

	CH_RTOS_FreeMemory(gh_APP_HEAP, (void*)argv[2].get_i());
	
	return 0;
}

static int rtos_get_freesize(int argc, CMD_PARAM argv[])
{
	int ret;
	U32 ui_FreeMemSize = 0, ui_LargestBlkSize = 0;

	ret = CH_RTOS_GetFreeMemory(gh_APP_HEAP, &ui_FreeMemSize, &ui_LargestBlkSize);
	LOG("gh_APP_HEAP(0X%X): GetFreeMemory return %d, ui_FreeMemSize=%d(0x%x), ui_LargestBlkSize=%d(0x%x)\n",
		gh_APP_HEAP,
		ret,
		ui_FreeMemSize,
		ui_FreeMemSize,
		ui_LargestBlkSize,
		ui_LargestBlkSize);
	
	ret = CH_RTOS_GetFreeMemory(gh_MID_HEAP, &ui_FreeMemSize, &ui_LargestBlkSize);
	LOG("gh_MID_HEAP(0X%X): GetFreeMemory return %d, ui_FreeMemSize=%d(0x%x), ui_LargestBlkSize=%d(0x%x)\n",
		gh_MID_HEAP,
		ret,
		ui_FreeMemSize,
		ui_FreeMemSize,
		ui_LargestBlkSize,
		ui_LargestBlkSize);


	ret = CH_RTOS_GetFreeMemory(NULL, &ui_FreeMemSize, &ui_LargestBlkSize);
	LOG("NULL(0X%X): GetFreeMemory return %d, ui_FreeMemSize=%d(0x%x), ui_LargestBlkSize=%d(0x%x)\n",
		NULL,
		ret,
		ui_FreeMemSize,
		ui_FreeMemSize,
		ui_LargestBlkSize,
		ui_LargestBlkSize);
	
	return 0;
}

static int rtos_systime(int argc, CMD_PARAM argv[])
{
	U32 ret;
	
	ret = CH_RTOS_GetSysTimenow();
	LOG("CH_RTOS_GetSysTimenow return %d(0x%x)\n", ret, ret);
	
	return 0;
}

static int rtos_systicks(int argc, CMD_PARAM argv[])
{
	U32 ret;
	
	ret = CH_RTOS_GetSysTicknow();
	LOG("CH_RTOS_GetSysTicknow return %d(0x%x)\n", ret, ret);
	
	return 0;
}
static int help(int argc, CMD_PARAM argv[])
{
	get_global_cmd().help(argv[1].get_str());
	return 0;
}
static int mem_info(int argc, CMD_PARAM argv[])
{
	dbg_checkmem(0);
	return 0;
}

static int mem_test(int argc, CMD_PARAM argv[])
{
	test_DDR(argv[1].get_i(), argv[2].get_i());
	return 0;
}

extern  int rtos_info(int argc, CMD_PARAM argv[]);

REGISTER_CMD(help, "获得某个命令的参数命令格式，或者所能支持的全部命令列表", "cmd_name",  "all");
REGISTER_CMD(rtos_init, "初始化rtos模块", "",  "");
REGISTER_CMD(rtos_term, "销毁rtos模块", "",  "");
REGISTER_CMD(rtos_ver, "打印rtos模块版本号", "",  "");
REGISTER_CMD(rtos_systime, "打印当前时间ms", "",  "");
REGISTER_CMD(rtos_systicks, "打印当前ticks", "",  "");
REGISTER_CMD(rtos_malloc, "分配内存", "pool_handle |size",  "0 0x100000");
REGISTER_CMD(rtos_free, "释放内存", "pool_handle |mem_addr",  "0 0x32765");
REGISTER_CMD(rtos_get_freesize, "打印各分区空闲的内存大小", "pool_handle",  "0");
REGISTER_CMD(mem_info, "打印内存使用情况，并检查内存", "",  "");
REGISTER_CMD(mem_test, "测试内存操作", "mem_size|loops(0为一直循环)",  "0x200000 1");
REGISTER_CMD(rtos_info, "打印RTOS相关资源的使用情况", "",  "");



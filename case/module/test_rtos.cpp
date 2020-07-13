/**
@file test_rtos.cpp
@defgroup rtos���Լ�

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
	"���Զ�γ�ʼ��ʱ�Ƿ񷵻�OK") {
	EXPECT_EQ(CH_RTOS_OK, CH_RTOS_Init());
	EXPECT_EQ(CH_RTOS_OK, CH_RTOS_Init()) << "��γ�ʼ��Ӧ�÷���OK";
}



CASE_F(TEST_RTOS_INIT2,
	"����ģ���Ƿ�ɱ�����") {
	EXPECT_EQ(CH_RTOS_OK, CH_RTOS_Term());
	EXPECT_EQ(CH_RTOS_OK, CH_RTOS_Init());
}


CASE_F(TEST_RTOS_INIT3,
	"����CH_RTOS_GetVersion�Ƿ�ɹ�") {
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
	"����rpv_StackPtrΪNULL����������Ϸ�ʱ�Ƿ���Գɹ���������	") {

	gb_task_delay = CH_TRUE;
	EXPECT_EQ(CH_RTOS_CreateTask(test_task_prams, NULL, NULL, 1024 * 8, 200, (S8*)"test_rtos_task_01", &gh_Task[0]),
		CH_RTOS_OK);
	EXPECT_NE(gh_Task[0], (void*)NULL) << "TASK handle��ӦΪNULL";
	EXPECT_EQ(CH_RTOS_CreateTask(test_task_prams, NULL, NULL, 1024 * 8, 200, (S8*)"test_rtos_task_02", &gh_Task[1]),
		CH_RTOS_OK);
	EXPECT_NE(gh_Task[1], (void*)NULL) << "TASK handle��ӦΪNULL";
}

CASE_F(TEST_RTOS_TASK2,
	"�Ƿ����ͨ��CH_RTOS_WaitTaskTerm�ȴ��ɹ�	") {
	
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
	"�����Ƿ����ͨ��CH_RTOS_KillTaskɾ���ոմ���������") {

	if(!gh_Task[0])return;
	EXPECT_EQ(CH_RTOS_KillTask(gh_Task[0]),
		CH_RTOS_OK);
	gh_Task[0] = NULL;
}

CASE_F(TEST_RTOS_TASK4,
	"�����Ƿ����ͨ��CH_RTOS_DeleteTaskɾ���ոմ���������") {

	if(!gh_Task[1])return;
	EXPECT_EQ(CH_RTOS_DeleteTask(gh_Task[1]),
		CH_RTOS_OK);
	gh_Task[1] = NULL;
}

CASE_F(TEST_RTOS_TASK5,
	"���Ե� ri_TimeOutMS=-1ʱ��CH_RTOS_WaitTaskTerm�Ƿ�ִ����ȷ") {

	gb_task_delay = CH_FALSE;
	EXPECT_EQ(CH_RTOS_CreateTask(test_task_prams, NULL, NULL, 1024 * 8, 200, (S8*)"test_rtos_task_01", &gh_Task[0]),
		CH_RTOS_OK);
	EXPECT_NE(gh_Task[0], (void*)NULL);
	EXPECT_EQ(CH_RTOS_WaitTaskTerm(gh_Task[0], -1), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteTask(gh_Task[0]),	CH_RTOS_OK);
	gh_Task[0] = NULL;
}

CASE_F(TEST_RTOS_TASK6,
	"���Ե� ri_TimeOutMS=1s,2s,5sʱ��CH_RTOS_WaitTaskTerm�ȴ�ʱ���Ƿ�ƥ��") {

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
		EXPECT_LT(time2, wait[i] + 1000) << "��ʱʱ����������1000ms";
		EXPECT_GT(time2, wait[i] - 1000) << "��ʱʱ����������1000ms";;
	}
	gb_task_delay = CH_FALSE;
	EXPECT_EQ(CH_RTOS_WaitTaskTerm(gh_Task[0], 1000), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteTask(gh_Task[0]),	CH_RTOS_OK);
	gh_Task[0] = NULL;
}



CASE_F(TEST_RTOS_TASK7,
	"����rpv_StackPtr��ΪNULL����������Ϸ�ʱ�Ƿ���Գɹ���������") {

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
	"����rpv_Param��ΪNULLʱ���Ƿ���������") {

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

	EXPECT_EQ(gui_task_count, params) << "�̲߳���δ����ȷ����";
	
}


CASE_F(TEST_RTOS_TASK9,
	"����ri_PriorityΪ-1ʱ���Ƿ���Գɹ���������") {

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
	"����ri_PriorityΪ255ʱ���Ƿ���Գɹ���������") {

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
	"����rpuc_Name��ΪNULL����ͬʱ���Ƿ���Գɹ���������") {

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
	"����rpuc_NameΪNULLʱ���Ƿ���Գɹ���������") {

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
	"�����ܷ�ɹ�����64��ri_StackSize��СΪ8K������") {
	int i;
	gb_task_delay = CH_TRUE;
	for(i = 0; i < MAX_TEST_TASK_NUM; i++)
	{
		EXPECT_EQ(CH_RTOS_CreateTask(test_task_prams, NULL, NULL, 1024 * 8, 10 + i,
			NULL,
			&gh_Task[i]),
			CH_RTOS_OK);
		EXPECT_NE(gh_Task[i], (void*)NULL) << "������" << i << "���߳�ʱʧ��";
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
			EXPECT_EQ(CH_RTOS_WaitTaskTerm(gh_Task[i], 1000), CH_RTOS_OK) << "�ȴ���" << i << "���߳�ʱʧ��";
			EXPECT_EQ(CH_RTOS_DeleteTask(gh_Task[i]),	CH_RTOS_OK)<< "���ٵ�" << i << "���߳�ʱʧ��";
			gh_Task[i] = NULL;
		}
	}
}

CASE_F(TEST_RTOS_TASK14,
	"�����ܷ�ɾ���������е�����") {

	gb_task_delay = CH_TRUE;
	EXPECT_EQ(CH_RTOS_CreateTask(test_task_prams, NULL, NULL, 1024 * 8, 100,
		NULL,
		&gh_Task[0]),
		CH_RTOS_OK);
	EXPECT_NE(gh_Task[0], (void*)NULL);

	CH_RTOS_DeleteTask(gh_Task[0]);//����鷵��ֵ,ֻ�۲�᲻������
	
	gb_task_delay = CH_FALSE;
	CH_RTOS_WaitTaskTerm(gh_Task[0], 10000);//����鷵��ֵ,ֻ�۲�᲻������
	CH_RTOS_DeleteTask(gh_Task[0]);//����鷵��ֵ,ֻ�۲�᲻������
	gh_Task[0] = NULL;
}
#if 0
CASE_F(TEST_RTOS_TASK15,
	"�����ܷ���ͣ�ָ�һ���������е�����") {

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
	EXPECT_NE(gui_task_count, 0) << "����һ��ʱ���count�ᱻ�ۼ�";
	LOG("gui_task_count = %d\n", gui_task_count);
	
	gui_task_count = 0;
	LOG("gui_task_count = %d\n", gui_task_count);
	CH_RTOS_TaskDelay(300);
	LOG("gui_task_count = %d\n", gui_task_count);
	EXPECT_EQ(0, gui_task_count) << "��ͣ�̺߳�countֵ��Ӧ���ı�";

	LOG("here\n");
	EXPECT_EQ(CH_RTOS_ResumeTask(gh_Task[0]), CH_RTOS_OK);
	CH_RTOS_TaskDelay(300);
	EXPECT_NE(gui_task_count, 0), "�ָ�һ��ʱ���count��������ۼ�";;

	LOG("here\n");
	gb_task_delay = CH_FALSE;
	EXPECT_EQ(CH_RTOS_WaitTaskTerm(gh_Task[0], 10000), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteTask(gh_Task[0]),	CH_RTOS_OK);
	gh_Task[0] = NULL;

	LOG("here\n");
}


CASE_F(TEST_RTOS_TASK16,
	"�����ܷ�CH_RTOS_ResumeTaskһ���������е�����") {

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
	"�����ܷ�CH_RTOS_SuspendTaskһ���Ѿ�suspend������") {

	gb_task_delay = CH_TRUE;
	EXPECT_EQ(CH_RTOS_CreateTask(test_task_prams, NULL, NULL, 1024 * 8, 100,
		NULL,
		&gh_Task[0]),
		CH_RTOS_OK);
	EXPECT_NE(gh_Task[0], (void*)NULL);

	EXPECT_EQ(CH_RTOS_SuspendTask(gh_Task[0]), CH_RTOS_OK);
	EXPECT_NE(CH_RTOS_SuspendTask(gh_Task[0]), CH_RTOS_OK) << "�ظ�suspend��Ӧ�ɹ�";
	
	EXPECT_EQ(CH_RTOS_ResumeTask(gh_Task[0]), CH_RTOS_OK);
	
	gb_task_delay = CH_FALSE;
	EXPECT_EQ(CH_RTOS_WaitTaskTerm(gh_Task[0], 10000), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteTask(gh_Task[0]),	CH_RTOS_OK);
	gh_Task[0] = NULL;
}


CASE_F(TEST_RTOS_TASK18,
	"�����ܷ���ͣ�ָ�һ���������Ѿ��˳��������δ��delete������") {

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
	"�����ܷ�ͨ��CH_RTOS_GetCurrentTaskId��ȷ��õ�ǰ�����ID") {

	CH_RTOS_TaskHandle cur_ID = NULL;
	
	gb_task_delay = CH_FALSE;
	EXPECT_EQ(CH_RTOS_CreateTask(test_task_getid, &cur_ID, NULL, 1024 * 8, 100,
		NULL,
		&gh_Task[0]),
		CH_RTOS_OK);
	EXPECT_NE(gh_Task[0], (void*)NULL);

	CH_RTOS_TaskDelay(100);
	EXPECT_EQ(cur_ID, gh_Task[0]) << "��ȡ������ID��һ��";
	
	EXPECT_EQ(CH_RTOS_WaitTaskTerm(gh_Task[0], 10000), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteTask(gh_Task[0]),	CH_RTOS_OK);
	gh_Task[0] = NULL;
}

CASE_F(TEST_RTOS_MES1,
	"�����ܷ�ʹ�úϷ�����������ɾ��һ����Ϣ") {
	CH_RTOS_MessageQHandle handle = NULL;
	int mes = 100, num = 32;

	EXPECT_EQ(CH_RTOS_CreateMessageQuene(sizeof(mes), num, &handle), CH_RTOS_OK);
	EXPECT_NE((void*)NULL, handle);
	EXPECT_EQ(CH_RTOS_DeleteMessageQuene(handle), CH_RTOS_OK);
}

CASE_F(TEST_RTOS_MES2,
	"����rph_MessageQΪNULLʱ�Ƿ��ܹ��ɹ�����һ����Ϣ") {
	int mes = 100, num = 32;

	EXPECT_EQ(CH_RTOS_CreateMessageQuene(sizeof(mes), num, NULL), CH_RTOS_INVALID_PARAM);
}

CASE_F(TEST_RTOS_MES3,
	"����ri_MessageSizeΪ0ʱ�Ƿ��ܹ��ɹ�����һ����Ϣ") {
	CH_RTOS_MessageQHandle handle = NULL;
	int num = 32;

	EXPECT_NE(CH_RTOS_CreateMessageQuene(0, num, &handle), CH_RTOS_OK);
	if(handle)
	EXPECT_EQ(CH_RTOS_DeleteMessageQuene(handle), CH_RTOS_OK);
}


CASE_F(TEST_RTOS_MES4,
	"����ri_MessageSizeΪ64ʱ�Ƿ��ܹ��ɹ�����һ����Ϣ") {
	CH_RTOS_MessageQHandle handle = NULL;
	int num = 32;

	EXPECT_EQ(CH_RTOS_CreateMessageQuene(64, num, &handle), CH_RTOS_OK);
	EXPECT_NE((void*)NULL, handle);
	EXPECT_EQ(CH_RTOS_DeleteMessageQuene(handle), CH_RTOS_OK);
}

CASE_F(TEST_RTOS_MES5,
	"����ri_MessageNumberΪ0ʱ�Ƿ��ܹ��ɹ�����һ����Ϣ") {
	CH_RTOS_MessageQHandle handle = NULL;
	int mes = 100, num = 0;

	EXPECT_NE(CH_RTOS_CreateMessageQuene(sizeof(mes), num, &handle), CH_RTOS_OK);
	if(handle)
	{
		EXPECT_EQ(CH_RTOS_DeleteMessageQuene(handle), CH_RTOS_OK);
	}
}

CASE_F(TEST_RTOS_MES6,
	"����ri_MessageNumberΪ128ʱ�Ƿ��ܹ��ɹ�����һ����Ϣ") {
	CH_RTOS_MessageQHandle handle = NULL;
	int mes = 100, num = 128;

	EXPECT_EQ(CH_RTOS_CreateMessageQuene(sizeof(mes), num, &handle), CH_RTOS_OK);
	EXPECT_NE((void*)NULL, handle);
	EXPECT_EQ(CH_RTOS_DeleteMessageQuene(handle), CH_RTOS_OK);
}

CASE_F(TEST_RTOS_MES7,
	"�����ܷ�һ���Դ���32��ri_MessageSize=64��ri_MessageNumber=128����Ϣ") {
#define TEST_MAX_MES_NUM 32
	
	CH_RTOS_MessageQHandle handle[TEST_MAX_MES_NUM];
	int num = 128, i;

	memset(&handle[0], 0, sizeof(handle));
	for(i = 0; i < TEST_MAX_MES_NUM; i++)
	{
		EXPECT_EQ(CH_RTOS_CreateMessageQuene(64, num, &handle[i]), CH_RTOS_OK) << "������" << i << "����Ϣʱʧ��";
		EXPECT_NE((void*)NULL, handle[i])<< "������" << i << "����Ϣʱʧ��";
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
	"�����ܷ��ظ�ɾ��һ����Ϣ") {
	CH_RTOS_MessageQHandle handle = NULL;
	int mes = 100, num = 128;

	EXPECT_EQ(CH_RTOS_CreateMessageQuene(sizeof(mes), num, &handle), CH_RTOS_OK);
	EXPECT_NE((void*)NULL, handle);
	EXPECT_EQ(CH_RTOS_DeleteMessageQuene(handle), CH_RTOS_OK);
	int dead_test = fw_getenv_i("DEAD_HALT_TEST");
	if(dead_test == 1)
	{
		EXPECT_NE(CH_RTOS_DeleteMessageQuene(handle), CH_RTOS_OK) << "�ظ�ɾ����Ϣ��Ӧ����";
	}
	else
	{
		EXPECT_EQ(dead_test, 1) << "Ӧͨ����������";
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
	"����߳�+�����Ϣ��ǿ�Ȳ���") {
#define TEST_MES_NUM 16	
	CH_RTOS_MessageQHandle mes_handle[TEST_MES_NUM], mes_handle2[TEST_MES_NUM];
	CH_RTOS_TaskHandle task_handle[TEST_MES_NUM];
	int i, mes;

	memset(&mes_handle[0], 0, sizeof(mes_handle));
	memset(&task_handle[0], 0, sizeof(task_handle));

	//����
	for(i = 0; i < TEST_MES_NUM; i++)
	{
		EXPECT_EQ(CH_RTOS_CreateMessageQuene(sizeof(mes), 8, &mes_handle[i]), CH_RTOS_OK);
		EXPECT_NE((void*)NULL, mes_handle[i]);
		EXPECT_EQ(CH_RTOS_CreateTask(test_task_mes, mes_handle[i], NULL, 8 * 1024, 150 + i, NULL, &task_handle[i]), CH_RTOS_OK);
		EXPECT_NE((void*)NULL, task_handle[i]);
	}

	//������Ϣ

	for(i = 0; i < TEST_MES_NUM; i++)
	{
		EXPECT_EQ(CH_RTOS_CreateMessageQuene(sizeof(mes), 8, &mes_handle2[i]), CH_RTOS_OK);
		EXPECT_NE((void*)NULL, mes_handle2[i]);
		
		mes = (int)mes_handle2[i];
		//LOG("send %d handle=0x%x,mes=%#x\n", i, mes_handle[i], mes);
		EXPECT_EQ(CH_RTOS_SendMessage(mes_handle[i], &mes, 1000), CH_RTOS_OK);
	}

	//������Ϣ

	for(i = 0; i < TEST_MES_NUM; i++)
	{
		mes = 0;
		EXPECT_EQ(CH_RTOS_ReceiveMessage(mes_handle2[i], &mes, 5000), CH_RTOS_OK) << "��Ϣ����ʧ��";
		//LOG("receive %d handle=0x%x,mes=%#x\n", i, mes_handle2[i], mes);
		EXPECT_EQ((int)mes_handle2[i], mes) << i << ".�յ�����Ϣ���ݲ���ȷ";

		EXPECT_EQ(CH_RTOS_ReceiveMessage(mes_handle2[i], &mes, 5000), CH_RTOS_OK) << "��Ϣ����ʧ��";
		//LOG("receive %d handle=0x%x,mes=%#x\n", i, mes_handle2[i], mes);
		EXPECT_EQ((int)mes_handle[i], mes) << i << ".�յ�����Ϣ���ݲ���ȷ";
	}

	//�˳��߳�

	for(i = 0; i < TEST_MES_NUM; i++)
	{
		mes = 'q';
		EXPECT_EQ(CH_RTOS_SendMessage(mes_handle[i], &mes, 100), CH_RTOS_OK);
	}

	//ɾ���̺߳���Ϣ����

	for(i = 0; i < TEST_MES_NUM; i++)
	{
		EXPECT_EQ(CH_RTOS_WaitTaskTerm(task_handle[i], 5000), CH_RTOS_OK);
		EXPECT_EQ(CH_RTOS_KillTask(task_handle[i]), CH_RTOS_OK);
		EXPECT_EQ(CH_RTOS_DeleteMessageQuene(mes_handle[i]), CH_RTOS_OK);
		EXPECT_EQ(CH_RTOS_DeleteMessageQuene(mes_handle2[i]), CH_RTOS_OK);
	}
}



CASE_F(TEST_RTOS_MUTEX1,
	"���Բ����Ϸ�ʱ�ܷ�ɹ�����������һ��mutex") {
	CH_RTOS_MutexHandle handle = NULL;

	EXPECT_EQ(CH_RTOS_CreateMutex(&handle, 0), CH_RTOS_OK);
	EXPECT_NE((void*)NULL, handle);
	EXPECT_EQ(CH_RTOS_DeleteMutex(handle), CH_RTOS_OK);
}

CASE_F(TEST_RTOS_MUTEX2,
	"�����ܷ��ظ�����һ��mutex") {
	CH_RTOS_MutexHandle handle = NULL;

	EXPECT_EQ(CH_RTOS_CreateMutex(&handle, 0), CH_RTOS_OK);
	EXPECT_NE((void*)NULL, handle);
	EXPECT_EQ(CH_RTOS_DeleteMutex(handle), CH_RTOS_OK);
	int dead_test = fw_getenv_i("DEAD_HALT_TEST");
	if(dead_test == 1)
	{
		EXPECT_NE(CH_RTOS_DeleteMutex(handle), CH_RTOS_OK) << "�ظ�����MUTEXʱ��Ӧ����";
	}
	else
	{
		EXPECT_EQ(dead_test, 1) << "Ӧͨ����������";
	}
}

CASE_F(TEST_RTOS_MUTEX3,
	"�����ܷ���ȷlockһ���Ϸ���mutex") {
	CH_RTOS_MutexHandle handle = NULL;

	EXPECT_EQ(CH_RTOS_CreateMutex(&handle, 0), CH_RTOS_OK);
	EXPECT_NE((void*)NULL, handle);
	
	EXPECT_EQ(CH_RTOS_LockMutex(handle), CH_RTOS_OK);
	
	int dead_test = fw_getenv_i("DEAD_HALT_TEST");
	if(dead_test == 1)
	{
		EXPECT_EQ(CH_RTOS_DeleteMutex(handle), CH_RTOS_OK) << "mutex����lock״̬����Ӧ�ɱ�ɾ��";
	}
	else
	{
		EXPECT_EQ(dead_test, 1) << "Ӧͨ����������";
	}
}

CASE_F(TEST_RTOS_MUTEX4,
	"�����ܷ�lockһ���Ѿ������ٵ�mutex") {
	CH_RTOS_MutexHandle handle = NULL;

	EXPECT_EQ(CH_RTOS_CreateMutex(&handle, 0), CH_RTOS_OK);
	EXPECT_NE((void*)NULL, handle);
	
	EXPECT_EQ(CH_RTOS_DeleteMutex(handle), CH_RTOS_OK);

	int dead_test = fw_getenv_i("DEAD_HALT_TEST");
	if(dead_test == 1)
	{
		EXPECT_NE(CH_RTOS_LockMutex(handle), CH_RTOS_OK) << "mutex����Ч���������ɹ�����Ӧ����";
	}
	else
	{
		EXPECT_EQ(dead_test, 1) << "Ӧͨ����������";
	}
}

CASE_F(TEST_RTOS_MUTEX5,
	"�����ܷ�unlockһ���Ϸ���mutex") {
	CH_RTOS_MutexHandle handle = NULL;

	EXPECT_EQ(CH_RTOS_CreateMutex(&handle, 0), CH_RTOS_OK);
	EXPECT_NE((void*)NULL, handle);

	EXPECT_EQ(CH_RTOS_LockMutex(handle), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_UnlockMutex(handle), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteMutex(handle), CH_RTOS_OK);
}

CASE_F(TEST_RTOS_MUTEX6,
	"�����ܷ��ظ�unlockһ���Ϸ���mutex") {
	CH_RTOS_MutexHandle handle = NULL;

	EXPECT_EQ(CH_RTOS_CreateMutex(&handle, 0), CH_RTOS_OK);
	EXPECT_NE((void*)NULL, handle);

	EXPECT_EQ(CH_RTOS_LockMutex(handle), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_UnlockMutex(handle), CH_RTOS_OK);
	if(fw_getenv_i("DEAD_HALT_TEST") == 1)
	{
		EXPECT_NE(CH_RTOS_UnlockMutex(handle), CH_RTOS_OK) << "�ظ�unlock mutex��Ӧ�ɹ�";
	}
	else
	{
		FAIL() << "Ӧͨ����������";
	}
	EXPECT_EQ(CH_RTOS_DeleteMutex(handle), CH_RTOS_OK);
}

CASE_F(TEST_RTOS_MUTEX7,
	"�����ܷ�unlockһ���Ѿ��������˵�mutex") {
	CH_RTOS_MutexHandle handle = NULL;

	EXPECT_EQ(CH_RTOS_CreateMutex(&handle, 0), CH_RTOS_OK);
	EXPECT_NE((void*)NULL, handle);
	EXPECT_EQ(CH_RTOS_DeleteMutex(handle), CH_RTOS_OK);

	int dead_test = fw_getenv_i("DEAD_HALT_TEST");
	if(dead_test == 1)
	{
		EXPECT_NE(CH_RTOS_UnlockMutex(handle), CH_RTOS_OK) << "mutex����Ч���������ɹ�����Ӧ����";
	}
	else
	{
		EXPECT_EQ(dead_test, 1) << "Ӧͨ����������";
	}	
}



CASE_F(TEST_RTOS_MUTEX8,
	"����trylockһ���Ѿ���lock��mutex���Ƿ�ɹ�") {
	CH_RTOS_MutexHandle handle = NULL;
	int ret;
	
	EXPECT_EQ(CH_RTOS_CreateMutex(&handle, 0), CH_RTOS_OK);
	EXPECT_NE((void*)NULL, handle);
	EXPECT_EQ(CH_RTOS_LockMutex(handle), CH_RTOS_OK);
	EXPECT_EQ(ret = CH_RTOS_TryLockMutex(handle), CH_RTOS_OK) << "LINUXϵͳ��ɹ�������ϵͳ���ܻ�ʧ�ܣ����۳ɹ�ʧ�ܶ���ͨ��";
	
	EXPECT_EQ(CH_RTOS_UnlockMutex(handle), CH_RTOS_OK);
	if(ret == CH_RTOS_OK)
	{
		EXPECT_EQ(CH_RTOS_UnlockMutex(handle), CH_RTOS_OK);
	}
	EXPECT_EQ(CH_RTOS_DeleteMutex(handle), CH_RTOS_OK);
}

CASE_F(TEST_RTOS_MUTEX9,
	"����trylockһ��unlock��mutex���Ƿ�ɹ�") {
	CH_RTOS_MutexHandle handle = NULL;

	EXPECT_EQ(CH_RTOS_CreateMutex(&handle, 0), CH_RTOS_OK);
	EXPECT_NE((void*)NULL, handle);
	EXPECT_EQ(CH_RTOS_TryLockMutex(handle), CH_RTOS_OK) << "��mutex��unlock״̬��Ӧ���سɹ�";
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
	"����lock�����У�unlock�Ƿ�ɹ�") {
	void* handle[3] = {NULL, NULL, NULL};
	CH_RTOS_TaskHandle h_TaskHandle = NULL;
	
	EXPECT_EQ(CH_RTOS_CreateMutex(&handle[0], 0), CH_RTOS_OK);
	EXPECT_NE((void*)NULL, handle[0]);
	EXPECT_EQ(CH_RTOS_LockMutex(handle[0]), CH_RTOS_OK) << "��mutex��unlock״̬��Ӧ���سɹ�";
	EXPECT_EQ(CH_RTOS_CreateSemaphore(0, &handle[1]), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_CreateSemaphore(0, &handle[2]), CH_RTOS_OK);

	EXPECT_EQ(CH_RTOS_CreateTask(test_task_mutex1, &handle[0], NULL, 8 * 1024, 150, NULL, &h_TaskHandle), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_WaitSemaphore(handle[1], -1), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_UnlockMutex(handle[0]), CH_RTOS_OK) << "��mutex��������һ���߳����ŶӵȺ�";

	EXPECT_EQ(CH_RTOS_SignalSemaphore(handle[2]), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_WaitTaskTerm(h_TaskHandle, 1000), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_KillTask(h_TaskHandle), CH_RTOS_OK);
		
	EXPECT_EQ(CH_RTOS_DeleteMutex(handle[0]), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteSemaphore(handle[1]), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteSemaphore(handle[2]), CH_RTOS_OK);
}

CASE_F(TEST_RTOS_MUTEX11,
	"�����ܷ��ڲ�ͬ���߳���lockһ��mutex����ֻ��һ���ܳɹ�") {
	void* handle[3] = {NULL, NULL, NULL};
	CH_RTOS_TaskHandle h_TaskHandle = NULL;
	
	EXPECT_EQ(CH_RTOS_CreateMutex(&handle[0], 0), CH_RTOS_OK);
	EXPECT_NE((void*)NULL, handle[0]);

	EXPECT_EQ(CH_RTOS_CreateSemaphore(0, &handle[1]), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_CreateSemaphore(0, &handle[2]), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_CreateTask(test_task_mutex1, &handle[0], NULL, 8 * 1024, 150, NULL, &h_TaskHandle), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_WaitSemaphore(handle[1], -1), CH_RTOS_OK);
	CH_RTOS_TaskDelay(100);
	EXPECT_NE(CH_RTOS_TryLockMutex(handle[0]), CH_RTOS_OK) << "��mutex��������һ���߳��б�LOCK";

	EXPECT_EQ(CH_RTOS_SignalSemaphore(handle[2]), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_WaitTaskTerm(h_TaskHandle, 1000), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_KillTask(h_TaskHandle), CH_RTOS_OK);
	
	EXPECT_EQ(CH_RTOS_DeleteMutex(handle[0]), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteSemaphore(handle[1]), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteSemaphore(handle[2]), CH_RTOS_OK);
}


CASE_F(TEST_RTOS_MUTEX12,
	"�����ܷ�һ���Դ����ɹ�100��mutex") {
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
	"����߳�+���mutex��ǿ�Ȳ���") {
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
		EXPECT_EQ(CH_RTOS_LockMutex(handle[i]), CH_RTOS_OK);//����ס
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
	EXPECT_EQ(gb_mutex_count, 0) << "��ʼֵӦΪ0";

	//����100�Σ��������mutex�Ƿ�ȷʵ�ܹ��ڶ��߳��б�������
	for(i = 0; i < 100; i++)
	{
		EXPECT_EQ(CH_RTOS_UnlockMutex(handle[0]), CH_RTOS_OK);
		CH_RTOS_TaskDelay(200);
		EXPECT_EQ(CH_RTOS_LockMutex(handle[0]), CH_RTOS_OK);
		EXPECT_EQ(gb_mutex_count, TEST_MUTEX_TASKNUM / 2) << "��+�߳�ִ����ɣ�������ӦΪTEST_MUTEX_TASKNUM/2, i=" << i;

		gb_mutex_count = TEST_MUTEX_TASKNUM / 2;
		EXPECT_EQ(CH_RTOS_UnlockMutex(handle[1]), CH_RTOS_OK);
		CH_RTOS_TaskDelay(200);
		EXPECT_EQ(CH_RTOS_LockMutex(handle[1]), CH_RTOS_OK);
		EXPECT_EQ(gb_mutex_count, 0) << "��-�߳�ִ����ɣ�������ӦΪ0, i=" << i;
		gb_mutex_count = 0;
	}

	gb_mutex_task_run = 0;

	
	EXPECT_EQ(CH_RTOS_UnlockMutex(handle[0]), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_UnlockMutex(handle[1]), CH_RTOS_OK);

	//ɾ����������
	for(i = 0; i < TEST_MUTEX_TASKNUM; i++)
	{
		if(h_TaskHandle[i])
		{
			EXPECT_EQ(CH_RTOS_WaitTaskTerm(h_TaskHandle[i], 1000), CH_RTOS_OK);
			EXPECT_EQ(CH_RTOS_KillTask(h_TaskHandle[i]), CH_RTOS_OK);
		}
	}
	//ɾ������mutex
	for(i = 0; i < 2; i++)
	{
		EXPECT_EQ(CH_RTOS_DeleteMutex(handle[i]), CH_RTOS_OK);
	}
}


CASE_F(TEST_RTOS_EVENT1,
	"�����ܷ�����������ɾ��һ��event") {
	void* handle =  NULL;
	
	EXPECT_EQ(CH_RTOS_CreateEvent(&handle), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteEvent(handle), CH_RTOS_OK);
}

CASE_F(TEST_RTOS_EVENT2,
	"���Ե�rph_Event=NULLʱ�ܷ񴴽�even") {

	int dead_test = fw_getenv_i("DEAD_HALT_TEST");
	if(dead_test == 1)
	{
		EXPECT_NE(CH_RTOS_CreateEvent(NULL), CH_RTOS_OK) << "�����Ч���������ɹ�����Ӧ����";
	}
	else
	{
		EXPECT_EQ(dead_test, 1) << "Ӧͨ����������";
	}
}

CASE_F(TEST_RTOS_EVENT3,
	"�����ܷ��ظ�ɾ��һ��event") {
	void* handle =  NULL;
	
	EXPECT_EQ(CH_RTOS_CreateEvent(&handle), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteEvent(handle), CH_RTOS_OK);

	int dead_test = fw_getenv_i("DEAD_HALT_TEST");
	if(dead_test == 1)
	{
		EXPECT_NE(CH_RTOS_DeleteEvent(handle), CH_RTOS_OK) << "�����Ч���������ɹ�����Ӧ����";
	}
	else
	{
		EXPECT_EQ(dead_test, 1) << "Ӧͨ����������";
	}
}

CASE_F(TEST_RTOS_EVENT4,
	"�����ܷ�setһ���Ѿ���ɾ���˵�event") {
	void* handle =  NULL;
	
	EXPECT_EQ(CH_RTOS_CreateEvent(&handle), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteEvent(handle), CH_RTOS_OK);

	int dead_test = fw_getenv_i("DEAD_HALT_TEST");
	if(dead_test == 1)
	{
		EXPECT_NE(CH_RTOS_SetEvent(handle, 1), CH_RTOS_OK) << "�����Ч���������ɹ�����Ӧ����";
	}
	else
	{
		EXPECT_EQ(dead_test, 1) << "Ӧͨ����������";
	}
}

CASE_F(TEST_RTOS_EVENT5,
	"���Եȴ�һ���Ѿ���ɾ���˵�event") {
	void* handle =  NULL;
	U32	ui_RetrievedEvents = 0;
	EXPECT_EQ(CH_RTOS_CreateEvent(&handle), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteEvent(handle), CH_RTOS_OK);

	int dead_test = fw_getenv_i("DEAD_HALT_TEST");
	if(dead_test == 1)
	{
		EXPECT_NE(CH_RTOS_WaitEvent(handle, 1, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_AND, 100), CH_RTOS_OK) << "�����Ч���������ɹ�����Ӧ����";
	}
	else
	{
		EXPECT_EQ(dead_test, 1) << "Ӧͨ����������";
	}
}

CASE_F(TEST_RTOS_EVENT6,
	"���Ե�rui_TargetEvents=0ʱ�Ƿ���Գɹ��ȴ�event") {
	void* handle =  NULL;
	U32	ui_RetrievedEvents = 0;
	EXPECT_EQ(CH_RTOS_CreateEvent(&handle), CH_RTOS_OK);

	ui_RetrievedEvents = 0;
	EXPECT_EQ(CH_RTOS_WaitEvent(handle, 0, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_AND, 100), CH_RTOS_OK) << "Ŀ���¼�Ϊ0��Ӧ�ȴ��ɹ�";
	EXPECT_EQ(CH_RTOS_WaitEvent(handle, 0, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_OR, 100), CH_RTOS_OK) << "Ŀ���¼�Ϊ0��Ӧ�ȴ��ɹ�";
	EXPECT_EQ(CH_RTOS_DeleteEvent(handle), CH_RTOS_OK);
}

CASE_F(TEST_RTOS_EVENT7,
	"���Ե�rpui_RetrievedEvents=NULL�Ƿ���Գɹ��ȴ�event") {
	void* handle =  NULL;
	
	EXPECT_EQ(CH_RTOS_CreateEvent(&handle), CH_RTOS_OK);

	int dead_test = fw_getenv_i("DEAD_HALT_TEST");
	if(dead_test == 1)
	{
		EXPECT_NE(CH_RTOS_WaitEvent(handle, 1, NULL, CH_RTOS_EVENT_OP_AND, 100), CH_RTOS_OK) << "���������Ч���������ɹ�����Ӧ����";
	}
	else
	{
		EXPECT_EQ(dead_test, 1) << "Ӧͨ����������";
	}
	EXPECT_EQ(CH_RTOS_DeleteEvent(handle), CH_RTOS_OK);
}

CASE_F(TEST_RTOS_EVENT8,
	"���Ե�renm_OpMode=CH_RTOS_EVENT_OP_ANDʱ���Ƿ�ȫ���¼������˲ŷ��سɹ�") {
	void* handle =  NULL;
	U32	ui_RetrievedEvents = 0;
	
	EXPECT_EQ(CH_RTOS_CreateEvent(&handle), CH_RTOS_OK);

	EXPECT_EQ(CH_RTOS_SetEvent(handle, 0x1), CH_RTOS_OK);
	EXPECT_NE(CH_RTOS_WaitEvent(handle, 0x1111, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_AND, 500), CH_RTOS_OK) << "�����Ŀ���¼������Ժ���ܷ��سɹ�";
	EXPECT_EQ(CH_RTOS_SetEvent(handle, 0x10), CH_RTOS_OK);
	EXPECT_NE(CH_RTOS_WaitEvent(handle, 0x1111, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_AND, 500), CH_RTOS_OK) << "�����Ŀ���¼������Ժ���ܷ��سɹ�";
	EXPECT_EQ(CH_RTOS_SetEvent(handle, 0x100), CH_RTOS_OK);
	EXPECT_NE(CH_RTOS_WaitEvent(handle, 0x1111, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_AND, 500), CH_RTOS_OK) << "�����Ŀ���¼������Ժ���ܷ��سɹ�";
	EXPECT_EQ(CH_RTOS_SetEvent(handle, 0x1000), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_WaitEvent(handle, 0x1111, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_AND, 500), CH_RTOS_OK) << "Ŀ���¼���ȫ��������Ӧ���سɹ�";
	EXPECT_EQ(0x1111, (U32)ui_RetrievedEvents);

	EXPECT_EQ(CH_RTOS_SetEvent(handle, 0x1111), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_WaitEvent(handle, 0x111, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_AND, 500), CH_RTOS_OK) << "Ŀ���¼���ȫ��������Ӧ���سɹ�";
	EXPECT_EQ(0x111, (U32)ui_RetrievedEvents);
	EXPECT_EQ(CH_RTOS_WaitEvent(handle, 0x1000, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_AND, 500), CH_RTOS_OK) << "δȡ����¼���Ӧ�����";
	EXPECT_EQ(0x1000, (U32)ui_RetrievedEvents);
	EXPECT_NE(CH_RTOS_WaitEvent(handle, 0x1000, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_AND, 500), CH_RTOS_OK) << "�¼��ѱ�ȡ�꣬Ӧ���س�ʱ";
	
	EXPECT_EQ(CH_RTOS_DeleteEvent(handle), CH_RTOS_OK);
}

CASE_F(TEST_RTOS_EVENT9,
	"���Ե�renm_OpMode=CH_RTOS_EVENT_OP_ORʱ���Ƿ���һ�¼������˼��ɷ��سɹ�") {
	void* handle =  NULL;
	U32	ui_RetrievedEvents = 0;
	
	EXPECT_EQ(CH_RTOS_CreateEvent(&handle), CH_RTOS_OK);

	ui_RetrievedEvents = 0;
	EXPECT_EQ(CH_RTOS_SetEvent(handle, 0x1), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_WaitEvent(handle, 0x1111, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_OR, 500), CH_RTOS_OK) << "��һ�¼����������سɹ�";
	EXPECT_EQ(0x1, ui_RetrievedEvents);

	ui_RetrievedEvents = 0;
	EXPECT_EQ(CH_RTOS_SetEvent(handle, 0x10), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_WaitEvent(handle, 0x1111, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_OR, 500), CH_RTOS_OK) << "��һ�¼����������سɹ�";
	EXPECT_EQ(0x10, ui_RetrievedEvents);

	ui_RetrievedEvents = 0;
	EXPECT_EQ(CH_RTOS_SetEvent(handle, 0x100), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_WaitEvent(handle, 0x1111, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_OR, 500), CH_RTOS_OK) << "��һ�¼����������سɹ�";
	EXPECT_EQ(0x100, ui_RetrievedEvents);

	ui_RetrievedEvents = 0;
	EXPECT_EQ(CH_RTOS_SetEvent(handle, 0x111000), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_WaitEvent(handle, 0x1111, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_OR, 500), CH_RTOS_OK) << "��һ�¼����������سɹ�";
	EXPECT_EQ(0x1000, ui_RetrievedEvents);

	ui_RetrievedEvents = 0;
	EXPECT_EQ(CH_RTOS_WaitEvent(handle, (U32)(-1), &ui_RetrievedEvents, CH_RTOS_EVENT_OP_OR, 500), CH_RTOS_OK) << "��һ�¼����������سɹ�";
	EXPECT_EQ(0x110000, ui_RetrievedEvents) << "ʣ����¼���Ӧ�����";

	EXPECT_NE(CH_RTOS_WaitEvent(handle, (U32)(-1), &ui_RetrievedEvents, CH_RTOS_EVENT_OP_OR, 500), CH_RTOS_OK) << "�¼��ѱ�ȡ�꣬Ӧ���س�ʱ";
	
	EXPECT_EQ(CH_RTOS_DeleteEvent(handle), CH_RTOS_OK);
}


CASE_F(TEST_RTOS_EVENT10,
	"���Ե�ri_TimeOutMS=0ʱ���ȴ������Ƿ���������") {
	void* handle =  NULL;
	U32	ui_RetrievedEvents = 0;
	U32 time1;
	EXPECT_EQ(CH_RTOS_CreateEvent(&handle), CH_RTOS_OK);

	time1 = CH_RTOS_GetSysTimenow();
	EXPECT_NE(CH_RTOS_WaitEvent(handle, 0x1110, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_AND, 0), CH_RTOS_OK);
	EXPECT_LT(CH_RTOS_TimeMinus(CH_RTOS_GetSysTimenow(), time1), 50) << "�ȴ�ʱ��Ϊ0��Ӧ��������";
	
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
	"���Ե�ri_TimeOutMS=-1ʱ���ȴ������Ƿ�һֱ�ȴ�") {
	void* handle =  NULL;
	U32	ui_RetrievedEvents = 0;
	U32 time1;
	CH_RTOS_TaskHandle h_TaskHandle;
	EXPECT_EQ(CH_RTOS_CreateEvent(&handle), CH_RTOS_OK);

	EXPECT_EQ(CH_RTOS_CreateTask(test_task_event1, handle, NULL, 8 * 1024, 150, NULL, &h_TaskHandle), CH_RTOS_OK);
	time1 = CH_RTOS_GetSysTimenow();
	EXPECT_EQ(CH_RTOS_WaitEvent(handle, 0x1110, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_AND, -1), CH_RTOS_OK);
	EXPECT_GT(CH_RTOS_TimeMinus(CH_RTOS_GetSysTimenow(), time1), 25000) << "���޵ȴ�ʱ��Ӧ����30S";
	
	EXPECT_EQ(CH_RTOS_DeleteEvent(handle), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_WaitTaskTerm(h_TaskHandle, 100), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteTask(h_TaskHandle), CH_RTOS_OK);
}

CASE_F(TEST_RTOS_EVENT12,
	"���Ե����¼�����ʱ���ȴ������ȴ�ʱ���Ƿ���ri_TimeOutMSһ��") {
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
		EXPECT_LE(time1, timeout[i] + 50) << "��ʱʱ�����Ӧ������50ms����";
		EXPECT_GE(time1, timeout[i] - 50) << "��ʱʱ�����Ӧ������50ms����";
	}
	
	EXPECT_EQ(CH_RTOS_DeleteEvent(handle), CH_RTOS_OK);
}

CASE_F(TEST_RTOS_EVENT13,
	"���Զ��setͬһ���¼����ٷ���ȴ����Ƿ�ֻ�ܵȴ��ɹ�һ��") {
	void* handle =  NULL;
	U32	ui_RetrievedEvents = 0, i;
	EXPECT_EQ(CH_RTOS_CreateEvent(&handle), CH_RTOS_OK);

	for(i = 0; i < 10; i++)
	{
		EXPECT_EQ(CH_RTOS_SetEvent(handle, 0x1110), CH_RTOS_OK);
	}
	EXPECT_EQ(CH_RTOS_WaitEvent(handle, 0x1110, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_AND, 10), CH_RTOS_OK);
	EXPECT_NE(CH_RTOS_WaitEvent(handle, 0x1110, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_AND, 10), CH_RTOS_OK) << "ͬһ�¼�ֻ�ܵȴ��ɹ�һ��";
	EXPECT_NE(CH_RTOS_WaitEvent(handle, 0x1110, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_OR, 10), CH_RTOS_OK)<< "ͬһ�¼�ֻ�ܵȴ��ɹ�һ��";
	
	for(i = 0; i < 10; i++)
	{
		EXPECT_EQ(CH_RTOS_SetEvent(handle, 0x1), CH_RTOS_OK);
	}
	EXPECT_EQ(CH_RTOS_WaitEvent(handle, 0x1, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_OR, 10), CH_RTOS_OK);
	EXPECT_NE(CH_RTOS_WaitEvent(handle, 0x1, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_AND, 10), CH_RTOS_OK)<< "ͬһ�¼�ֻ�ܵȴ��ɹ�һ��";
	EXPECT_NE(CH_RTOS_WaitEvent(handle, 0x1, &ui_RetrievedEvents, CH_RTOS_EVENT_OP_OR, 10), CH_RTOS_OK)<< "ͬһ�¼�ֻ�ܵȴ��ɹ�һ��";
	
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
	"���Եȴ�������set�¼����Ƿ�ȴ��ɹ�") {
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
	"����߳�+���EVENT��ǿ�Ȳ���") {
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


	EXPECT_EQ(gb_mutex_count, 0) << "��ʼֵӦΪ0";

	//�������Է���һ��event������߳�ͬʱ�ȴ����Ƿ�ֻ��һ���߳��ܻ�ȡ�ɹ�
	for(i = 1; i < TEST_EVENT_TASKNUM; i++)
	{
		EXPECT_EQ(CH_RTOS_SetEvent(handle, i*32), CH_RTOS_OK);
		CH_RTOS_TaskDelay(200);
		//LOG("gb_mutex_count = %d, i = %d.\n", gb_mutex_count, i);
		EXPECT_EQ(gb_mutex_count, i*32) << "����߳��ڵȴ��¼�����ֻ��һ���߳��ܻ�ȡ�ɹ�, i=" << i;
		gb_mutex_count = 0;
	}

	
	//ɾ����������
	for(i = 0; i < TEST_EVENT_TASKNUM; i++)
	{
		EXPECT_EQ(CH_RTOS_WaitTaskTerm(h_TaskHandle[i], 5000), CH_RTOS_OK);
		EXPECT_EQ(CH_RTOS_KillTask(h_TaskHandle[i]), CH_RTOS_OK);
	}
	
	//ɾ������event
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
	"�����ܷ��ԺϷ����������ɹ�һ��һ����timer")
{
	EXPECT_EQ(CH_RTOS_CreateSemaphore(0, &gh_timer_sem), CH_RTOS_OK);
	EXPECT_NE((int)(gh_timer_sem), 0) <<"�ź�����������ӦΪ0";

	EXPECT_EQ(CH_RTOS_CreateTimer(test_timer_sem1, (U32)(gh_timer_sem), 10, CH_RTOS_TIMER_MODE_ONE, &gh_timer), CH_RTOS_OK);
	EXPECT_NE((int)(gh_timer), 0) <<"TIMER��������ӦΪ0";

}

CASE_F(TEST_RTOS_TIMER2,
	"���Ըոմ�����timerδstartǰû������")
{
	EXPECT_NE(CH_RTOS_WaitSemaphore(gh_timer_sem, 1000), CH_RTOS_OK) << "TIMERδstart����Ӧ��ִ��";
}

CASE_F(TEST_RTOS_TIMER3,
	"�����ܷ�����start�ոմ�����timer��ʹ����������")
{
	EXPECT_EQ(CH_RTOS_StartTimer(gh_timer), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_WaitSemaphore(gh_timer_sem, 5000), CH_RTOS_OK) << "TIMER��start��Ӧ����ȷִ��";
}

CASE_F(TEST_RTOS_TIMER4,
	"����������stop�������е�timer")
{
	EXPECT_NE(CH_RTOS_WaitSemaphore(gh_timer_sem, 1000), CH_RTOS_OK) << "һ����timer��ֻ��ִ��һ��";
	
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
	"�����ܷ��ԺϷ����������ɹ�һ������timer������������")
{
	EXPECT_EQ(CH_RTOS_CreateSemaphore(0, &gh_timer_sem), CH_RTOS_OK);
	EXPECT_NE((int)(gh_timer_sem), 0) <<"�ź�����������ӦΪ0";

	EXPECT_EQ(CH_RTOS_CreateTimer(test_timer_sem2, (U32)(gh_timer_sem), 10, CH_RTOS_TIMER_MODE_CONTINUE, &gh_timer), CH_RTOS_OK);
	EXPECT_NE((int)(gh_timer), 0) <<"TIMER��������ӦΪ0";

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
	"����������ɾ��һ���������е�timer")
{
	EXPECT_EQ(CH_RTOS_CreateSemaphore(0, &gh_timer_sem), CH_RTOS_OK);
	EXPECT_NE((int)(gh_timer_sem), 0) <<"�ź�����������ӦΪ0";

	EXPECT_EQ(CH_RTOS_CreateTimer(test_timer_sem2, (U32)(gh_timer_sem), 10, CH_RTOS_TIMER_MODE_CONTINUE, &gh_timer), CH_RTOS_OK);
	EXPECT_NE((int)(gh_timer), 0) <<"TIMER��������ӦΪ0";

	EXPECT_EQ(CH_RTOS_StartTimer(gh_timer), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteTimer(gh_timer), CH_RTOS_OK);
	EXPECT_EQ(CH_RTOS_DeleteSemaphore(gh_timer_sem), CH_RTOS_OK);
	gh_timer_sem = NULL;
	gh_timer = NULL;
}

CASE_F(TEST_RTOS_TIMER7,
	"����һ���Դ���16��timer�����Ҷ��ܳɹ�")
{
	CH_RTOS_TimerHandle h_timer[16];
	unsigned int i;

	for(i = 0; i < sizeof(h_timer)/sizeof(h_timer[0]); i++)
	{
		EXPECT_EQ(CH_RTOS_CreateTimer(test_timer_sem2, 0, 10, CH_RTOS_TIMER_MODE_CONTINUE, &h_timer[i]), CH_RTOS_OK);
		EXPECT_NE((int)(h_timer[i]), 0) <<"TIMER��������ӦΪ0,i=" << i;
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
	"����1000���ֽ��ڴ�ķ�����ͷ��Ƿ�ɹ�")
{
	void *p;

	EXPECT_NE(p = CH_RTOS_AllocateMemory(gh_APP_HEAP, 1000), (void*)NULL);
	
	EXPECT_EQ(CH_RTOS_FreeMemory(gh_APP_HEAP, p), CH_RTOS_OK);
}

CASE_F(TEST_RTOS_MEM2,
	"����8M�ڴ�ķ�����ͷ��Ƿ�ɹ�")
{
	void *p;

	EXPECT_NE(p = CH_RTOS_AllocateMemory(gh_APP_HEAP, 0x800000), (void*)NULL);
	EXPECT_EQ(CH_RTOS_FreeMemory(gh_APP_HEAP, p), CH_RTOS_OK);
}

CASE_F(TEST_RTOS_MEM3,
	"�����ڴ��С����4���������ķ�����ͷ��Ƿ�ɹ�")
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
	"���Է����СΪ0�ֽڵ��ڴ��Ƿ�ɹ�")
{
	void *p;

	EXPECT_EQ(p = CH_RTOS_AllocateMemory(gh_APP_HEAP, 0), (void*)NULL);
	if(p)
	{
		CH_RTOS_FreeMemory(gh_APP_HEAP, p);
	}
}

CASE_F(TEST_RTOS_MEM5,
	"����С���ڴ�ķ�����ͷ��Ƿ�ɹ�")
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
	"�����ط�����ڴ��СС��������ڴ��С������ܷ��ط���ɹ�")
{
	void *p;

	EXPECT_NE(p = CH_RTOS_AllocateMemory(gh_APP_HEAP, 256), (void*)NULL);
	EXPECT_NE(p = CH_RTOS_ReallocMemory(gh_APP_HEAP, p, 4), (void*)NULL);
	EXPECT_EQ(CH_RTOS_FreeMemory(gh_APP_HEAP, p), CH_RTOS_OK);
}

CASE_F(TEST_RTOS_REALLOC2,
	"�����ط�����ڴ��С����������ڴ��С������ܷ��ط���ɹ�")
{
	void *p;

	EXPECT_NE(p = CH_RTOS_AllocateMemory(gh_APP_HEAP, 16), (void*)NULL);
	EXPECT_NE(p = CH_RTOS_ReallocMemory(gh_APP_HEAP, p, 128), (void*)NULL);
	EXPECT_EQ(CH_RTOS_FreeMemory(gh_APP_HEAP, p), CH_RTOS_OK);
}

CASE_F(TEST_RTOS_REALLOC3,
	"�����ط�����ڴ��ַpv_MemAddr����NULL������ܷ��ط���ɹ�")
{
	void *p;

	EXPECT_NE(p = CH_RTOS_ReallocMemory(gh_APP_HEAP, NULL, 128), (void*)NULL);
	if(p)
	{
		CH_RTOS_FreeMemory(gh_APP_HEAP, p);
	}
}

CASE_F(TEST_RTOS_GETFREEMEM,
	"����CH_RTOS_GetFreeMemory��ȡ��ֵ�Ƿ�׼ȷ")
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
	"����CH_RTOS_Memcpy�Ƿ�ɹ�")
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
	"����CH_RTOS_GetSysTicknow��ȡ��ֵ�Ƿ���ȷ")
{
	U32 tick1, tick2;

	EXPECT_NE(tick1 = CH_RTOS_GetSysTicknow(), 0);
	CH_RTOS_TaskDelay(1000);
	EXPECT_NE(tick2 = CH_RTOS_GetSysTicknow(), 0);
	EXPECT_LE(CH_RTOS_CvtTimeToTick(1000), (tick2 - tick1) * 102 /100) << "���С��2%";
	EXPECT_GE(CH_RTOS_CvtTimeToTick(1000), (tick2 - tick1) * 98 /100) << "���С��2%";;
}

CASE_F(TEST_RTOS_TIME2,
	"����CH_RTOS_GetSysTimenow��ȡ��ֵ�Ƿ���ȷ")
{
	U32 time1, time2;

	EXPECT_NE(time1 = CH_RTOS_GetSysTimenow(), 0);
	CH_RTOS_TaskDelay(1000);
	EXPECT_NE(time2 = CH_RTOS_GetSysTimenow(), 0);
	EXPECT_LE((1000), (time2 - time1) * 102 /100) << "���С����2%";
	EXPECT_GE((1000), (time2 - time1) * 98 /100) << "���С����2%";;
}

CASE_F(TEST_RTOS_TIME3,
	"����CH_RTOS_GetSysTimenow��ȡ��ֵ�Ƿ���ȷ")
{
	U32 time1, time2;

	EXPECT_NE(time1 = CH_RTOS_GetSysTimenow(), 0);
	CH_RTOS_TaskDelay(1000);
	EXPECT_NE(time2 = CH_RTOS_GetSysTimenow(), 0);
	EXPECT_LE((1000), (time2 - time1) * 102 /100) << "���С����2%";
	EXPECT_GE((1000), (time2 - time1) * 98 /100) << "���С����2%";;
}

CASE_F(TEST_RTOS_TIME4,
	"����CH_RTOS_CvtTimeToTick�����Ƿ���ȷ")
{
	U32 tick1, tick2;

	EXPECT_NE(tick1 = CH_RTOS_GetSysTicknow(), 0);
	CH_RTOS_TaskDelay(1000);
	EXPECT_NE(tick2 = CH_RTOS_GetSysTicknow(), 0);
	EXPECT_LE(CH_RTOS_CvtTimeToTick(1000), (tick2 - tick1) * 102 /100) << "���С��2%";
	EXPECT_GE(CH_RTOS_CvtTimeToTick(1000), (tick2 - tick1) * 98 /100) << "���С��2%";;
}

CASE_F(TEST_RTOS_TIME5,
	"����CH_RTOS_TimeTickCmp�Ƿ���ȷ")
{
	U32 tick1, tick2;

	EXPECT_NE(tick1 = CH_RTOS_GetSysTicknow(), 0);
	CH_RTOS_TaskDelay(1);
	EXPECT_NE(tick2 = CH_RTOS_GetSysTicknow(), 0);
	EXPECT_EQ(CH_RTOS_TimeTickCmp(tick1, tick2), 0);
	EXPECT_EQ(CH_RTOS_TimeTickCmp(tick2, tick1), 1);
}

CASE_F(TEST_RTOS_TIME6,
	"����CH_RTOS_TimeCmp�Ƿ���ȷ")
{
	U32 time1, time2;

	EXPECT_NE(time1 = CH_RTOS_GetSysTimenow(), 0);
	CH_RTOS_TaskDelay(1);
	EXPECT_NE(time2 = CH_RTOS_GetSysTimenow(), 0);
	EXPECT_EQ(CH_RTOS_TimeCmp(time1, time2), 0);
	EXPECT_EQ(CH_RTOS_TimeCmp(time2, time1), 1);
}

CASE_F(TEST_RTOS_TIME7,
	"����CH_RTOS_TimeTickPlus�����Ƿ���ȷ")
{
	U32 tick1, tick2;

	EXPECT_NE(tick1 = CH_RTOS_GetSysTicknow(), 0);
	CH_RTOS_TaskDelay(1000);
	EXPECT_NE(tick2 = CH_RTOS_GetSysTicknow(), 0);
	EXPECT_LE(tick2, CH_RTOS_TimeTickPlus(tick1, CH_RTOS_CvtTimeToTick(1000))* 102 /100) << "���С����2%";
	EXPECT_GE(tick2, CH_RTOS_TimeTickPlus(tick1, CH_RTOS_CvtTimeToTick(1000)) * 98 /100) << "���С����2%";;
}

CASE_F(TEST_RTOS_TIME8,
	"����CH_RTOS_TimePlus�����Ƿ���ȷ")
{
	U32 time1, time2;

	EXPECT_NE(time1 = CH_RTOS_GetSysTimenow(), 0);
	CH_RTOS_TaskDelay(1000);
	EXPECT_NE(time2 = CH_RTOS_GetSysTimenow(), 0);
	EXPECT_LE(time2, CH_RTOS_TimeTickPlus(time1, 1000)* 102 /100) << "���С����2%";
	EXPECT_GE(time2, CH_RTOS_TimeTickPlus(time1, 1000) * 98 /100) << "���С����2%";;
}

CASE_F(TEST_RTOS_TIME9,
	"����CH_RTOS_TimeTickMinus�����Ƿ���ȷ")
{
	U32 tick1, tick2;

	EXPECT_NE(tick1 = CH_RTOS_GetSysTicknow(), 0);
	CH_RTOS_TaskDelay(1000);
	EXPECT_NE(tick2 = CH_RTOS_GetSysTicknow(), 0);
	EXPECT_LE(tick1, CH_RTOS_TimeTickMinus(tick2, CH_RTOS_CvtTimeToTick(1000))* 102 /100) << "���С����2%";
	EXPECT_GE(tick1, CH_RTOS_TimeTickMinus(tick2, CH_RTOS_CvtTimeToTick(1000)) * 98 /100) << "���С����2%";
	EXPECT_EQ(3000, CH_RTOS_TimeTickMinus(2000, (U32)(-1000))) << "����ʱ�������Ĵ����Ƿ���ȷ";
}

CASE_F(TEST_RTOS_TIME10,
	"����CH_RTOS_TimeMinus�����Ƿ���ȷ")
{
	U32 time1, time2;

	EXPECT_NE(time1 = CH_RTOS_GetSysTicknow(), 0);
	CH_RTOS_TaskDelay(1000);
	EXPECT_NE(time2 = CH_RTOS_GetSysTicknow(), 0);
	EXPECT_LE(time1, CH_RTOS_TimeMinus(time2, 1000)* 102 /100) << "���С����2%";
	EXPECT_GE(time1, CH_RTOS_TimeMinus(time2, 1000) * 98 /100) << "���С����2%";
	EXPECT_EQ(3000, CH_RTOS_TimeTickMinus(2000, (U32)(-1000))) << "����ʱ�������Ĵ����Ƿ���ȷ";
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

REGISTER_CMD(help, "���ĳ������Ĳ��������ʽ����������֧�ֵ�ȫ�������б�", "cmd_name",  "all");
REGISTER_CMD(rtos_init, "��ʼ��rtosģ��", "",  "");
REGISTER_CMD(rtos_term, "����rtosģ��", "",  "");
REGISTER_CMD(rtos_ver, "��ӡrtosģ��汾��", "",  "");
REGISTER_CMD(rtos_systime, "��ӡ��ǰʱ��ms", "",  "");
REGISTER_CMD(rtos_systicks, "��ӡ��ǰticks", "",  "");
REGISTER_CMD(rtos_malloc, "�����ڴ�", "pool_handle |size",  "0 0x100000");
REGISTER_CMD(rtos_free, "�ͷ��ڴ�", "pool_handle |mem_addr",  "0 0x32765");
REGISTER_CMD(rtos_get_freesize, "��ӡ���������е��ڴ��С", "pool_handle",  "0");
REGISTER_CMD(mem_info, "��ӡ�ڴ�ʹ�������������ڴ�", "",  "");
REGISTER_CMD(mem_test, "�����ڴ����", "mem_size|loops(0Ϊһֱѭ��)",  "0x200000 1");
REGISTER_CMD(rtos_info, "��ӡRTOS�����Դ��ʹ�����", "",  "");



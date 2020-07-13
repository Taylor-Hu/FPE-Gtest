/**
@file test_sc.cpp
@defgroup 智能卡测试集

@author 网络公司基础技术部:	邹书强
@date   2014年4月10日  
@version V4.0
@todo     ---时间---作者-------------修改部分及原因----------
@todo     1 2014年4月10日 邹书强  创建


@{
*/
#ifdef CASE_NAME
#undef CASE_NAME
#endif
#define CASE_NAME  TestSc
#include "test_menu.h"
#include "test_modules.h"


void CASE_NAME::SetUpTestCase() {
//excute before each test suit
//std::cout << "init SetUp rtos TestCase " << std::endl;
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



static CHDRV_SC_OP_EVENT_e genm_sc_event = CHDRV_SC_EVT_MAX;
static CHDRV_SC_Handle	gph_SC = NULL, gh_CallScHanlde = NULL;

static void test_sc_callback(
		CHDRV_SC_Handle			rh_SC,
		CHDRV_SC_OP_EVENT_e		renm_SCOpEvt,
		CHDRV_SC_EventData_t	*rpstru_CallBackData)
{
	const char *event_name[] = 
	{
		"CHDRV_SC_EVT_INSERTED",			///< 卡插入事件
		"CHDRV_SC_EVT_REMOVED",				///< 卡拔出事件
		"CHDRV_SC_EVT_RESETED",				///< 卡复位完成事件，在非阻塞方式下使用
		"CHDRV_SC_EVT_TRANSFER_COMPLETED",	///< 卡数据传输完成事件，在非阻塞方式下使用
		"CHDRV_SC_EVT_ERROR",	
		"CHDRV_SC_EVT_MAX",
	};
	gh_CallScHanlde = rh_SC;
	genm_sc_event = renm_SCOpEvt;
	if(renm_SCOpEvt < CHDRV_SC_EVT_MAX)
	{
		LOG("%s\n", event_name[renm_SCOpEvt]);
	}
	else
	{
		LOG("ERROR:test_sc_callback=%d\n", renm_SCOpEvt);
	}
}

static CH_BOOL test_sc_evt1(void)
{
	char *pc_user_input = NULL;
	CHDRV_SC_STATUS_e   enm_SCStatus = CHDRV_SC_CARD_ERR;


	PROMPT("请插入卡，准备好以后回车\n");

	//PROMPT( "请将卡拔出，准备好以后回车\n");
	pc_user_input = test_get_input();
	gh_CallScHanlde = NULL;
	genm_sc_event = CHDRV_SC_EVT_MAX;
	
	PROMPT("请插入卡，准备好以后回车\n");
	pc_user_input = test_get_input();

	EXPECT_EQ(gh_CallScHanlde, gph_SC) << "回调函数的句柄不相等";
	EXPECT_EQ(genm_sc_event, CHDRV_SC_EVT_INSERTED) << "未检测到卡插入事件";
	enm_SCStatus = CHDRV_SC_CARD_ERR;
#if !(defined(CFG_CHSSA_CHIP_MSTAR7S55B))
	EXPECT_EQ(CHDRV_SC_OK, CHDRV_SC_GetSCStatus(gph_SC, &enm_SCStatus));
	EXPECT_EQ(CHDRV_SC_CARD_OK, enm_SCStatus);
	gh_CallScHanlde = NULL;
	genm_sc_event = CHDRV_SC_EVT_MAX;
	
	PROMPT("请将卡拔出，准备好以后回车\n");
	pc_user_input = test_get_input();

	EXPECT_NE(gh_CallScHanlde, gph_SC) << "回调函数的句柄不相等";
	EXPECT_EQ(genm_sc_event, CHDRV_SC_EVT_REMOVED) << "未检测到卡拔出事件";
	enm_SCStatus = CHDRV_SC_CARD_ERR;
	EXPECT_EQ(CHDRV_SC_OK, CHDRV_SC_GetSCStatus(gph_SC, &enm_SCStatus));
	EXPECT_EQ(CHDRV_SC_CARD_OUT, enm_SCStatus);
	
	PROMPT("请插入卡，准备好以后回车\n");
	pc_user_input = test_get_input();
#endif

	//EXPECT_EQ(CHDRV_SC_Close(gph_SC), CHDRV_SC_OK);

	return CH_TRUE;
}

static int sc_str2arry(char *pstr, U8 *parry, int arry_len)
{
	int i = 0, value = 0;
	char *p;

	memset(parry, 0, arry_len);
	p = strtok(pstr, ",");

	while(p && i < arry_len)
	{
		sscanf(p, "%x", &value);
		parry[i++] = value;
		p = strtok(NULL, ",");
	};

	LOG("=========len=%d===========\n", i);
	for(int j = 0; j < i; j++)
	{
		printf("%02x ", parry[j]);
		if(j && (j % 8) == 0){
			printf("\n");
		}
	}
	return i;
}

#if ENABLE_TESTCASE

CASE_F(TEST_SC_INIT,
	"测试SC INIT是否成功"){
	EXPECT_EQ(CHDRV_SC_Init(), CHDRV_SC_OK);
}

CASE_F(TEST_SC_VERSION,
	"测试获取SC版本号是否成功"){
#if !(defined(CFG_CHSSA_CHIP_MSTAR7S55B))
	CHDRV_Version_t stru_Ver = 0;
	EXPECT_EQ(CHDRV_SC_GetVersion(&stru_Ver), CHDRV_SC_OK);
	LOG("stru_Ver = %#x", stru_Ver);
#endif
}


CASE_F(TEST_SC_OPEN,
	"测试SC OPEN是否成功") {
	EXPECT_EQ(CHDRV_SC_Open(CHDRV_SC_DEVICE_0, CH_FALSE, test_sc_callback, &gph_SC), CHDRV_SC_OK);
	EXPECT_NE(gph_SC, (void*)NULL) << "SC句柄不应为NULL";
}

CASE_F(TEST_SC_CALLBACK,
	"测试SC拔插事件是否正常") {
	if(fw_getenv_i("SUPPORT_SMART_CARD"))
	{
		EXPECT_EQ(CH_TRUE, test_sc_evt1());
	}
}


CASE_F(TEST_SC_RESET,
	"测试SC复位是否正常") {
	int ret, len;
	char buf[32];
#if !(defined(CFG_CHSSA_CHIP_MSTAR7S55B))
	if(fw_getenv_i("SUPPORT_SMART_CARD"))
	{
		len = sizeof(buf);
		EXPECT_EQ(CH_TRUE,  CHDRV_SC_ResetSmartCard(gph_SC, (U8*)buf, (S32*)&len)) << "复位失败";
	}
#endif
}


CASE_F(TEST_SC_EXCHANGE,
	"测试SC数据交换是否正常") {
	int ret, i;
	S32 send_len = 0, receive_len = 0;
	U8 send_buf[512], receive_buf[512], env_name[16], *p;

	if(fw_getenv_i("SUPPORT_SMART_CARD") == 0)
	{
		return;
	}
	
	for(i = 1 ;;i++)
	{
		sprintf(env_name, "SC_SEND_%d", i);
		p = fw_getenv(env_name);
		if(!p)
		{
			break;
		}
		send_len = sizeof(send_buf);
		send_len = sc_str2arry(p, send_buf, send_len);
		receive_len = sizeof(receive_buf);
		EXPECT_EQ(CHDRV_SC_OK,  ret = CHDRV_SC_DataExchange(gph_SC, send_buf, &send_len, receive_buf, &receive_len)) << "数据交换失败";
		if(ret != CHDRV_SC_OK)
		{
			continue;
		}
		sprintf(env_name, "SC_RECEIVE_%d", i);
		p = fw_getenv(env_name);
		if(p)
		{
			send_len = sizeof(send_buf);
			send_len = sc_str2arry(p, send_buf, send_len);
			EXPECT_EQ(send_len, receive_len) << "收到的数据长度不符合预期";
			EXPECT_EQ(0, memcmp(receive_buf, send_buf, MIN(send_len, receive_len))) << "接收到的数据内容不符合预期";
		}
		else
		{
			EXPECT_EQ(0, receive_len) << "收到的数据长度应为0";
		}
	}
}
#endif /* ENABLE_TESTCASE */

int sc_open(int argc, CMD_PARAM argv[])
{
 	int ret;

	ret = CHDRV_SC_Open(
                    (CHDRV_SC_DEVICE_e)argv[1].get_i(),
                    argv[2].get_b(),
                    test_sc_callback,
                    &gph_SC);
	LOG("CHDRV_SC_Open return %d, handle=%#x\n", ret, gph_SC);
	return ret;
}

int sc_reset(int argc, CMD_PARAM argv[])
{
#if !(defined(CFG_CHSSA_CHIP_MSTAR7S55B))
	int ret, len;
	char buf[32];

	if(gph_SC == NULL)
	{
		LOG("you must open sc first!\n");
		return -1;
	}
	len = sizeof(buf);
	ret = CHDRV_SC_ResetSmartCard(gph_SC, (U8*)buf, (S32*)&len);
	LOG("CHDRV_SC_ResetSmartCard return %d, len= %d\n",
		ret, len);
	return ret;
#endif
}

int sc_close(int argc, CMD_PARAM argv[])
{
 	int ret;

	if(gph_SC == NULL)
	{
		LOG("you must open sc first!\n");
		return -1;
	}

	ret = CHDRV_SC_Close(gph_SC);
	LOG("CHDRV_SC_Close return %d\n", ret);
	gph_SC = 0;
	return ret;
}

int sc_init(int argc, CMD_PARAM argv[])
{
 	int ret;

	ret = CHDRV_SC_Init();
	LOG("CHDRV_SC_Init return %d\n", ret);
	return ret;
}





REGISTER_CMD(sc_open,  "打开smart card驱动", "device_id block_flag", "0 false");
REGISTER_CMD(sc_close, "关闭smart card驱动", "",  "");
REGISTER_CMD(sc_init,"初始化smart card驱动", "",  "");
REGISTER_CMD(sc_reset, "复位smart card驱动", "", "");







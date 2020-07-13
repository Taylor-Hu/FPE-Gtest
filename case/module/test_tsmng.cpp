/**
@file test_tsmng.cpp
@def group tsmng测试集

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
#define CASE_NAME TestTsmng


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

#if ENABLE_TESTCASE

CASE_F(TEST_TSMNG_INIT1,
	"测试多次初始化时是否返回OK") {
	EXPECT_EQ(CHDRV_TSMNG_OK, CHDRV_TSMNG_Init());
	EXPECT_EQ(CHDRV_TSMNG_OK, CHDRV_TSMNG_Init()) << "多次初始化应该返回OK";
}




CASE_F(TEST_TSMNG_INIT2,
	"测试模块是否可被销毁") {
	EXPECT_EQ(CHDRV_TSMNG_OK, CHDRV_TSMNG_Term());
	EXPECT_EQ(CHDRV_TSMNG_OK, CHDRV_TSMNG_Init());
}


CASE_F(TEST_TSMNG_INIT3,
	"测试CHDRV_TSMNG_GetVersion是否成功") {
	CHDRV_Version_t stru_Ver;
	
	memset(&stru_Ver, 0, sizeof(stru_Ver));
	EXPECT_EQ(CHDRV_TSMNG_OK, CHDRV_TSMNG_GetVersion(&stru_Ver));
	EXPECT_NE(0, stru_Ver);
	LOG("\nstru_Ver=%x\n", stru_Ver);
}

CASE_F(TEST_TSMNG_INIT4,
	"测试CHDRV_TSMNG_Open是否成功"){

	EXPECT_EQ(CHDRV_TSMNG_Open(fw_getenv_i("TSMNG_TUNER_NO"), CHDRV_TSMNG_TYPE_MPEG2_TS),CHDRV_TSMNG_OK);

}

CASE_F(TEST_TSMNG_FUN1,
	"测试CHDRV_TSMNG_SetInputChannel是否成功"){

	CHDRV_TSMNG_SetInputChannel(fw_getenv_i("TSMNG_TUNER_NO"), (CHDRV_TSMNG_SOURCE_TYPE_e)fw_getenv_i("TSMNG_SOURCE_NO"));
}

CASE_F(TEST_TSMNG_INIT5,
	"测试CHDRV_TSMNG_Close是否成功"){

	EXPECT_EQ(CHDRV_TSMNG_Close(fw_getenv_i("TSMNG_TUNER_NO")), CHDRV_TSMNG_OK);
}
#endif /* ENABLE_TESTCASE */


int tsmng_init(int argc, CMD_PARAM argv[])
{
	int ret;

	ret = CHDRV_TSMNG_Init();

	LOG("CHDRV_TSMNG_Init return %d\n", ret);
	return ret;
}

int tsmng_term(int argc, CMD_PARAM argv[])
{
	int ret;

	ret = CHDRV_TSMNG_Term();

	LOG("CHDRV_TSMNG_Term return %d\n", ret);
	return ret;
}

int tsmng_ver(int argc, CMD_PARAM argv[])
{
	int ret;
	CHDRV_Version_t tsmng_version;
	
	ret = CHDRV_TSMNG_GetVersion(&tsmng_version);

	LOG("CHDRV_TSMNG_GetVersion return %d\n", ret);
	return ret;
}

int tsmng_set(int argc, CMD_PARAM argv[])
{
	int ret;
	ret = CHDRV_TSMNG_SetInputChannel(argv[1].get_i(), (CHDRV_TSMNG_SOURCE_TYPE_e)argv[2].get_i());

	if(ret != CHDRV_TSMNG_OK)
	{
		return -1;
	}
	return 0;
}


REGISTER_CMD(tsmng_init, "初始化tsmng模块", "",  "");
REGISTER_CMD(tsmng_term, "销毁tsmng模块", "",  "");
REGISTER_CMD(tsmng_ver, "打印tsmng模块版本号", "",  "");
REGISTER_CMD(tsmng_set, "设置demux的输入源,", "tuner_no |source_no",  "0 1");




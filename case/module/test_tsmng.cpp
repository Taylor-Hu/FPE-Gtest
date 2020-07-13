/**
@file test_tsmng.cpp
@def group tsmng���Լ�

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
	"���Զ�γ�ʼ��ʱ�Ƿ񷵻�OK") {
	EXPECT_EQ(CHDRV_TSMNG_OK, CHDRV_TSMNG_Init());
	EXPECT_EQ(CHDRV_TSMNG_OK, CHDRV_TSMNG_Init()) << "��γ�ʼ��Ӧ�÷���OK";
}




CASE_F(TEST_TSMNG_INIT2,
	"����ģ���Ƿ�ɱ�����") {
	EXPECT_EQ(CHDRV_TSMNG_OK, CHDRV_TSMNG_Term());
	EXPECT_EQ(CHDRV_TSMNG_OK, CHDRV_TSMNG_Init());
}


CASE_F(TEST_TSMNG_INIT3,
	"����CHDRV_TSMNG_GetVersion�Ƿ�ɹ�") {
	CHDRV_Version_t stru_Ver;
	
	memset(&stru_Ver, 0, sizeof(stru_Ver));
	EXPECT_EQ(CHDRV_TSMNG_OK, CHDRV_TSMNG_GetVersion(&stru_Ver));
	EXPECT_NE(0, stru_Ver);
	LOG("\nstru_Ver=%x\n", stru_Ver);
}

CASE_F(TEST_TSMNG_INIT4,
	"����CHDRV_TSMNG_Open�Ƿ�ɹ�"){

	EXPECT_EQ(CHDRV_TSMNG_Open(fw_getenv_i("TSMNG_TUNER_NO"), CHDRV_TSMNG_TYPE_MPEG2_TS),CHDRV_TSMNG_OK);

}

CASE_F(TEST_TSMNG_FUN1,
	"����CHDRV_TSMNG_SetInputChannel�Ƿ�ɹ�"){

	CHDRV_TSMNG_SetInputChannel(fw_getenv_i("TSMNG_TUNER_NO"), (CHDRV_TSMNG_SOURCE_TYPE_e)fw_getenv_i("TSMNG_SOURCE_NO"));
}

CASE_F(TEST_TSMNG_INIT5,
	"����CHDRV_TSMNG_Close�Ƿ�ɹ�"){

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


REGISTER_CMD(tsmng_init, "��ʼ��tsmngģ��", "",  "");
REGISTER_CMD(tsmng_term, "����tsmngģ��", "",  "");
REGISTER_CMD(tsmng_ver, "��ӡtsmngģ��汾��", "",  "");
REGISTER_CMD(tsmng_set, "����demux������Դ,", "tuner_no |source_no",  "0 1");




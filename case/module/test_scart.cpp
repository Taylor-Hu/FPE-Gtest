/**
@file test_scart.cpp
@def group scart���Լ�

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
#define CASE_NAME TestScart


#include "test_menu.h"
#include "test_modules.h"


void CASE_NAME::SetUpTestCase() {
//excute before each test suit
//std::cout << "init SetUp rtos TestCase " << std::endl;
RUN_CMD("tuner_lock", fw_getenv("TUNER_PARAMS1"));

}
void CASE_NAME::TearDownTestCase() {
//excute after each test suit
//std::cout << "deinit scart rtos TestCase " << std::endl;
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

CASE_F(TEST_SCART_INIT1,
	"���Զ�γ�ʼ��ʱ�Ƿ񷵻�OK") {
	EXPECT_EQ(CHDRV_SCART_OK, CHDRV_SCART_Init());
	EXPECT_EQ(CHDRV_SCART_OK, CHDRV_SCART_Init()) << "��γ�ʼ��Ӧ�÷���OK";
}


CASE_F(TEST_SCART_INIT2,
	"����ģ���Ƿ�ɱ�����") {
	EXPECT_EQ(CHDRV_SCART_OK, CHDRV_SCART_Term());
	EXPECT_EQ(CHDRV_SCART_OK, CHDRV_SCART_Init());
}


CASE_F(TEST_SCART_INIT3,
	"����CHDRV_SCART_GetVersion�Ƿ�ɹ�") {
	CHDRV_Version_t stru_Ver;
	
	memset(&stru_Ver, 0, sizeof(stru_Ver));
	EXPECT_EQ(CHDRV_SCART_OK, CHDRV_SCART_GetVersion(&stru_Ver));
	EXPECT_NE(0, stru_Ver);
	LOG("stru_Ver=%x\n", stru_Ver);
}

CASE_F(TEST_SCART_AV1,
	"��ʾ����Ա��env�����ò��ź����������Ӻõ����Ժ󣬼����Ƶ����Ƶ����Ƿ�����"){

	LOG("�밴env�����ò�������(Y/N):");
	EXPECT_EQ(test_input_yes(),CH_TRUE);

	LOG("��Ƶ��Ƶ����Ƿ�����?");
	EXPECT_EQ(test_input_yes(),CH_TRUE);
}

CASE_F(TEST_SCART_TV_TYPE,
	"����CHDRV_SCART_SetTVType����ź�����,CVBS , RGB���������ʽ"){

	CHDRV_SCART_TV_TYPE_e enm_scartType;
	enm_scartType = CHDRV_SCART_CVBS;
	
	CHDRV_SCART_SetTVType(enm_scartType);
	LOG("CVBSģʽ�����Ƿ������(Y/N)?");
	EXPECT_EQ(test_input_yes(),CH_TRUE);
	LOG("CVBSģʽ���ܱ������16��,�Ƿ�Ϊ0V(Y/N)?");
	EXPECT_EQ(test_input_yes(),CH_TRUE);

	enm_scartType = CHDRV_SCART_RGB;
	CHDRV_SCART_SetTVType(enm_scartType);
	LOG("RGBģʽ�����Ƿ������(Y/N)?");
	EXPECT_EQ(test_input_yes(),CH_TRUE);
	LOG("RGBģʽ���ܱ������16��,�Ƿ�Ϊ1-3V(Y/N)?");
	EXPECT_EQ(test_input_yes(),CH_TRUE);
}

CASE_F(TEST_SCART_ASPECT_TYPE,
	"����CHDRV_SCART_SetAspectType�ӿ�"){

	CHDRV_SCART_ASPECT_TYPE_e enm_scartAspType;
	enm_scartAspType = CHDRV_SCART_4X3;

	CHDRV_SCART_SetAspectType(enm_scartAspType);
	LOG("��������������Ϊ4:3(Y/N)?");
	EXPECT_EQ(test_input_yes(),CH_TRUE);
	LOG("���ܱ������8�ţ�4��3ʱ���ܽŵ�ѹ��9-12V(Y/N)?");
	EXPECT_EQ(test_input_yes(),CH_TRUE);

	enm_scartAspType = CHDRV_SCART_16X9;
	CHDRV_SCART_SetAspectType(enm_scartAspType);
	LOG("��������������Ϊ16:9(Y/N)?");
	EXPECT_EQ(test_input_yes(),CH_TRUE);
	LOG("���ܱ������8�ţ�16:9ʱ���ܽŵ�ѹ��9-12V(Y/N)?");
	EXPECT_EQ(test_input_yes(),CH_TRUE);
}


int scart_init(int argc,CMD_PARAM argv[])
{
	int ret;

	ret = CHDRV_SCART_Init();
	LOG("CHDRV_SCART_Init return %d\n", ret);
	return ret;
}

int scart_term(int argc,CMD_PARAM argv[])
{
	int ret;

	ret = CHDRV_SCART_Term();
	LOG("CHDRV_SCART_Term return %d\n", ret);
	return ret;
}

int scart_ver(int argc,CMD_PARAM argv[])
{
	CHDRV_Version_t stru_Ver;

	memset(&stru_Ver, 0, sizeof(stru_Ver));
	CHDRV_SCART_GetVersion(&stru_Ver);

	LOG("scart�汾��:%d\n",*stru_Ver);
}

int scart_set_tvtype(int argc,CMD_PARAM argv[])
{
	int ret;

	ret = CHDRV_SCART_SetTVType(argv[1].get_i());
	LOG("CHDRV_SCART_SetTVType return %d\n",ret);
	return ret;
}

int scart_set_aspect(int argc,CMD_PARAM argv[])
{
	int ret;
	
	ret = CHDRV_SCART_SetAspectType(argv[1].get_i());
	LOG("CHDRV_SCART_SetAspectType return %d\n",ret);
	return ret;
}

REGISTER_CMD(scart_init, "��ʼ��SCARTģ��", "",  "");
REGISTER_CMD(scart_term, "����scartģ��", "",  "");
REGISTER_CMD(scart_ver, "��ӡģ��汾��", "",  "");
REGISTER_CMD(scart_set_tvtype, "����scart������ͣ�CVBS��RGB��", "tv_type",  "0");
REGISTER_CMD(scart_set_aspect, "����scart�ݺ��", "aspect_type",  "0");
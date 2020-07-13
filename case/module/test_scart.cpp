/**
@file test_scart.cpp
@def group scart测试集

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
	"测试多次初始化时是否返回OK") {
	EXPECT_EQ(CHDRV_SCART_OK, CHDRV_SCART_Init());
	EXPECT_EQ(CHDRV_SCART_OK, CHDRV_SCART_Init()) << "多次初始化应该返回OK";
}


CASE_F(TEST_SCART_INIT2,
	"测试模块是否可被销毁") {
	EXPECT_EQ(CHDRV_SCART_OK, CHDRV_SCART_Term());
	EXPECT_EQ(CHDRV_SCART_OK, CHDRV_SCART_Init());
}


CASE_F(TEST_SCART_INIT3,
	"测试CHDRV_SCART_GetVersion是否成功") {
	CHDRV_Version_t stru_Ver;
	
	memset(&stru_Ver, 0, sizeof(stru_Ver));
	EXPECT_EQ(CHDRV_SCART_OK, CHDRV_SCART_GetVersion(&stru_Ver));
	EXPECT_NE(0, stru_Ver);
	LOG("stru_Ver=%x\n", stru_Ver);
}

CASE_F(TEST_SCART_AV1,
	"提示测试员按env的配置播放好码流，连接好电视以后，检查音频、视频输出是否正常"){

	LOG("请按env的配置播放码流(Y/N):");
	EXPECT_EQ(test_input_yes(),CH_TRUE);

	LOG("音频视频输出是否正常?");
	EXPECT_EQ(test_input_yes(),CH_TRUE);
}

CASE_F(TEST_SCART_TV_TYPE,
	"测试CHDRV_SCART_SetTVType输出信号类型,CVBS , RGB两种输出方式"){

	CHDRV_SCART_TV_TYPE_e enm_scartType;
	enm_scartType = CHDRV_SCART_CVBS;
	
	CHDRV_SCART_SetTVType(enm_scartType);
	LOG("CVBS模式电视是否有输出(Y/N)?");
	EXPECT_EQ(test_input_yes(),CH_TRUE);
	LOG("CVBS模式万能表测量第16脚,是否为0V(Y/N)?");
	EXPECT_EQ(test_input_yes(),CH_TRUE);

	enm_scartType = CHDRV_SCART_RGB;
	CHDRV_SCART_SetTVType(enm_scartType);
	LOG("RGB模式电视是否有输出(Y/N)?");
	EXPECT_EQ(test_input_yes(),CH_TRUE);
	LOG("RGB模式万能表测量第16脚,是否为1-3V(Y/N)?");
	EXPECT_EQ(test_input_yes(),CH_TRUE);
}

CASE_F(TEST_SCART_ASPECT_TYPE,
	"测试CHDRV_SCART_SetAspectType接口"){

	CHDRV_SCART_ASPECT_TYPE_e enm_scartAspType;
	enm_scartAspType = CHDRV_SCART_4X3;

	CHDRV_SCART_SetAspectType(enm_scartAspType);
	LOG("电视输出的屏宽比为4:3(Y/N)?");
	EXPECT_EQ(test_input_yes(),CH_TRUE);
	LOG("万能表测量第8脚，4：3时，管脚电压在9-12V(Y/N)?");
	EXPECT_EQ(test_input_yes(),CH_TRUE);

	enm_scartAspType = CHDRV_SCART_16X9;
	CHDRV_SCART_SetAspectType(enm_scartAspType);
	LOG("电视输出的屏宽比为16:9(Y/N)?");
	EXPECT_EQ(test_input_yes(),CH_TRUE);
	LOG("万能表测量第8脚，16:9时，管脚电压在9-12V(Y/N)?");
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

	LOG("scart版本号:%d\n",*stru_Ver);
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

REGISTER_CMD(scart_init, "初始化SCART模块", "",  "");
REGISTER_CMD(scart_term, "销毁scart模块", "",  "");
REGISTER_CMD(scart_ver, "打印模块版本号", "",  "");
REGISTER_CMD(scart_set_tvtype, "设置scart输出类型（CVBS、RGB）", "tv_type",  "0");
REGISTER_CMD(scart_set_aspect, "设置scart纵横比", "aspect_type",  "0");
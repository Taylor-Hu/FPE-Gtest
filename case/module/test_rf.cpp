/**
@file test_rf.cpp
@def group rf测试集

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
#define CASE_NAME TestRf


#include "test_menu.h"
#include "test_modules.h"


void CASE_NAME::SetUpTestCase() {
//excute before each test suit
//std::cout << "init SetUp rtos TestCase " << std::endl;
}
void CASE_NAME::TearDownTestCase() {
//excute after each test suit
//std::cout << "deinit rf rtos TestCase " << std::endl;
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



CASE_F(TEST_RF_INIT1,
	"测试多次初始化时是否返回OK") {
	EXPECT_EQ(CHDRV_RF_OK, CHDRV_RF_Init());
	EXPECT_EQ(CHDRV_RF_OK, CHDRV_RF_Init()) << "多次初始化应该返回OK";
}

CASE_F(TEST_RF_INIT2,
	"测试模块是否可被销毁") {
	EXPECT_EQ(CHDRV_RF_OK, CHDRV_RF_Term());
	EXPECT_EQ(CHDRV_RF_OK, CHDRV_RF_Init());
}


CASE_F(TEST_RF_INIT3,
	"测试CHDRV_RF_GetVersion是否成功") {
	CHDRV_Version_t stru_Ver;
	
	memset(&stru_Ver, 0, sizeof(stru_Ver));
	EXPECT_EQ(CHDRV_RF_OK, CH_RF_GetVersion(&stru_Ver));
	EXPECT_NE(0, stru_Ver);
	LOG("\nstru_Ver=%x\n", stru_Ver);
}










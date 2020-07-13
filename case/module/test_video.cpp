/**
@file test_video.cpp
@def group video测试集

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
#define CASE_NAME TestVideo


#include "test_menu.h"
#include "test_modules.h"


void CASE_NAME::SetUpTestCase() {
//excute before each test suit
//std::cout << "init SetUp rtos TestCase " << std::endl;
RUN_CMD("tuner.lock %s", fw_getenv("TUNER_PARAMS1"));

}
void CASE_NAME::TearDownTestCase() {
//excute after each test suit
//std::cout << "deinit video rtos TestCase " << std::endl;
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

CASE_F(TEST_VIDEO_INIT1,
	"测试多次初始化时是否返回OK") {
	EXPECT_EQ(CHDRV_VIDEO_OK, CHDRV_VIDEO_Init());
	EXPECT_EQ(CHDRV_VIDEO_OK, CHDRV_VIDEO_Init()) << "多次初始化应该返回OK";
}


CASE_F(TEST_VIDEO_INIT2,
	"测试模块是否可被销毁") {
      //EXPECT_EQ(CHDRV_VIDEO_OK, CHDRV_VIDEO_Term());
	//EXPECT_EQ(CHDRV_VIDEO_OK, CHDRV_VIDEO_Init());
}


CASE_F(TEST_VIDEO_INIT3,
	"测试CHDRV_VIDEO_GetVersion是否成功") {
	CHDRV_Version_t stru_Ver;
	
	memset(&stru_Ver, 0, sizeof(stru_Ver));
	//EXPECT_EQ(CHDRV_VIDEO_OK, CHDRV_VIDEO_GetVersion(&stru_Ver));
	//EXPECT_NE(0, stru_Ver);
	LOG("\nstru_Ver=%x\n", stru_Ver);
}
#endif /* ENABLE_TESTCASE */











/**
@file gmain.cpp
@defgroup 测试集入口

@author 网络公司基础技术部:	邹书强
@date   2009.10.22  
@version V4.0
@todo     ---时间---作者-------------修改部分及原因----------
@todo     1 2014年4月10日 邹书强  创建


@{
*/
//TestAll.cpp
#ifdef CASE_NAME
#undef CASE_NAME
#endif
#define CASE_NAME TestMain


#include "test_menu.h"
#include <exception>

#ifdef __cplusplus
extern "C" {
#endif
int gtest_main(int argc,char* argv[]);
#ifdef __cplusplus
}
#endif

extern const char* guc_CHSSA_PLATFORM;
extern const char* guc_CHSSA_CHIP;

#if ENABLE_TESTCASE
using namespace testing;
using namespace internal;
#endif /* ENABLE_TESTCASE */

extern char *guc_TESTER ;
#if SUPPORT_TEST_BROKEN
String gcsLastUnitName = "", gcsLastCaseName = "", gcsLastResult  = "";
int gb_BrokenCheckOK = 0;
TEST_METHOD_e genm_TestMethod = enm_REBUILD_NEW_TEST;
#endif /* SUPPORT_TEST_BROKEN */


/**
FuncName:main
@brief 程序主入口
 
@param [in] argc 
@param [in] argv 
@param [out] none
 
@return success  
@return fail  
   
@attention none
*/

int gtest_main(int argc,char* argv[])
{
#if ENABLE_TESTCASE==0
	get_global_cmd().cmd();
#else
	//initDrvTest(); 
	//CHMID_InitializeMiddle();
	//global setup, and resource before or after test suit running
	const char *pstr = "zou shuqiang";

	testing::AddGlobalTestEnvironment(new ChdrvEnvironment);

	char strFilter[128] = {0};
	int ret;
	char file[256];
	int argc1 = 1;
	char* argv1[2] = {(char*)"gtest", (char*)"xml:TestReport.xml"};
	FilePath path;
	strcpy(strFilter, "*");
	String csFilter = "", csTester = "";

#if GTEST_OS_ECOS
	open_uart(); 
#endif
	testing::GTEST_FLAG(color) = "yes";
	testing::GTEST_FLAG(print_time) = true;
	
	snprintf(file, sizeof(file), "resource/test_config_%s%s.cfg", guc_CHSSA_PLATFORM, guc_CHSSA_CHIP);
	do
	{
		ret = fw_parse_script(file) ;
		if(ret != 0)
		{
			CH_RTOS_TaskDelay(1000);
			LOG("can not get %s, please check and restart!\n", file);
		}
	}while(ret != 0);
	if(ret != 0)
	{
		LOG("can not get %s, please check and restart!\n", file);
		exit(0);
	}
	
#if SUPPORT_TEST_BROKEN
#if FOR_AUTO_RUN
	ret = -1;
#else
	ret = GetLastProcCase(gcsLastUnitName, gcsLastCaseName, gcsLastResult);
#endif /* FOR_AUTO_RUN */
	if(ret != 0)
	{
		genm_TestMethod = enm_REBUILD_NEW_TEST;
	}
	else if(strcmp(gcsLastResult.c_str(), "RESTART") == 0)
	{
		genm_TestMethod = enm_CONTINUE_NEXT_CASE;
	}
	else
	{
		do
		{
			PROMPT("检测到上次您有未完成的测试，请选择(1~5):\n");
			printf("1. 重新测试最后一个未完成的用例%s.%s\n", 
				gcsLastUnitName.c_str(),gcsLastCaseName.c_str());
			printf("2. 跳过%s.%s，该用例失败，从下一个用例继续测试\n", 
				gcsLastUnitName.c_str(), gcsLastCaseName.c_str());
			printf("3. 重新测试最后一个测试单元%s下全部用例\n", 
				gcsLastUnitName.c_str());
			printf("4. 抛弃上次的测试结果，重新进行新的一轮测试\n");
			printf("5. 进入命令行模式\n");
			ret = test_input_i(NULL);
			if(ret == 1)
			{
				genm_TestMethod = 	enm_CONTINUE_LAST_CASE;
				break;
			}
			else if(ret == 2)
			{
				genm_TestMethod = 	enm_CONTINUE_NEXT_CASE;
				break;
			}
			else if(ret == 3)
			{
				genm_TestMethod = 	enm_CONTINUE_LAST_UNIT;
				break;
			}
			else if(ret == 4)
			{
				genm_TestMethod = 	enm_REBUILD_NEW_TEST;
				break;
			}
			else if(ret == 5)
			{
				get_global_cmd().cmd();
			}
		}while(1);
}

#endif /* SUPPORT_TEST_BROKEN */


#if FOR_AUTO_RUN == 0
	if(enm_REBUILD_NEW_TEST == genm_TestMethod)
	do
	{
		LOG( "Can you see the Chinese character string as follows? If not, please check your serial tools.\n");
		LOG( "您能看见我吗?如果可以，请输入字符\'ch\'后回车,进行下一步测试\n");
		if(test_input_is("ch"))
		{
			break;
		}
	}while(1);
#endif /* FOR_AUTO_RUN */

START_TEST:
	
#if FOR_AUTO_RUN == 0
	if(enm_REBUILD_NEW_TEST == genm_TestMethod)
	do
	{
		LOG("请输入您的姓名[直接回车进入命令行模式]:");
		pstr = test_get_menu_input();
		if(pstr[0] == '\0' 
			|| pstr[0] == '\r'
			|| pstr[0] == '\n')
		{
			get_global_cmd().cmd();
		}
		else
		{
			break;
		}
	}
	while(1);
#endif

#if SUPPORT_TEST_BROKEN
	if(enm_REBUILD_NEW_TEST != genm_TestMethod)
	{
		if(GetLastFilterAndTester(csFilter, csTester) == 0)
		{
			pstr = csTester.c_str();
			strcpy(strFilter, csFilter.c_str());
		}
		gb_BrokenCheckOK = 0;
	}
	else
	{
		gb_BrokenCheckOK = 1;
		csTester = (String)pstr;
	}
	
#endif /* SUPPORT_TEST_BROKEN */

	if(guc_TESTER)
	{
		free(guc_TESTER);
	}
	guc_TESTER = (char*)malloc(strlen(pstr) + 1);
	if(guc_TESTER){
	strcpy(guc_TESTER, pstr);
	}


	while(1)
	{
	
#if SUPPORT_TEST_BROKEN
		if(enm_REBUILD_NEW_TEST == genm_TestMethod)
		{
			if(main_menu(strFilter) < 0)
			{
				goto START_TEST;
			}
			csFilter = (String)strFilter;
		}

		if(strcmp(gcsLastResult.c_str(), "RESTART"))
		{
			InitTempFile(gcsLastUnitName, gcsLastCaseName, gcsLastResult, csFilter, csTester, genm_TestMethod);
		}
#endif	
		testing::GTEST_FLAG(filter) = strFilter; // 代码中指定FLAG，运行FLAG

		if(strstr(strFilter, "TestPerformance"))
		{
			snprintf(file, sizeof(file), "xml:%s%s_TestPerfo.xml", guc_CHSSA_PLATFORM, guc_CHSSA_CHIP);
		}
		else
		{
			snprintf(file, sizeof(file), "xml:%s%s_Test.xml", guc_CHSSA_PLATFORM, guc_CHSSA_CHIP);
		}
		//gtest_main()#255:current file name is xml:mstar7c123d_Test.xml, filter = *HandleTrueReturn/0*
		LOG( "current file name is %s, filter = %s\r\n", file, strFilter);
		testing::GTEST_FLAG(output) = file;

		argc1 = 2;
		argv1[argc1 - 1] = file;
		
		//===argc1=2  argv0=gtest  argv1=xml:mstar7c123d_Test.xml===
		printf("===argc1=%d  argv0=%s  argv1=%s===\n", argc1, argv1[0], argv1[1]);
		testing::InitGoogleTest(&argc1,argv1); 

		snprintf(file, sizeof(file), "resource/test_config_%s%s.cfg", guc_CHSSA_PLATFORM, guc_CHSSA_CHIP);
		fw_parse_script(file);
		//fw_printenv(1, NULL);

		LOG("%s=%s\n", "INVALID_IP", fw_getenv("INVALID_IP"));

		LOG("after set, %s=%s\n", "INVALID_IP", fw_getenv("INVALID_IP"));
		
		RUN_ALL_TESTS();
		strcpy(strFilter, "*");
		genm_TestMethod = enm_REBUILD_NEW_TEST;
	}
#endif /* ENABLE_TESTCASE==0 */
	return 0;
}



/*加载所有的测试集*/
#if FOR_AUTO_RUN
#undef ENABLE_TESTCASE
#define ENABLE_TESTCASE 0
#include "test_route.cpp"

#include "test_tuner.cpp"
#include "test_section.cpp"
#include "test_av.cpp"
#include "test_fsys.cpp"
#undef ENABLE_TESTCASE
#define ENABLE_TESTCASE 1
#else
#include "test_rtos.cpp"
#endif /* FOR_AUTO_RUN */
#include "test_comm.cpp"
#if FOR_AUTO_RUN==0
#include "test_flash.cpp"
#include "test_route.cpp"

#include "test_tuner.cpp"
#endif /* FOR_AUTO_RUN==0 */
#include "test_osd.cpp"

#if FOR_AUTO_RUN==0
#include "test_aout.cpp"
#endif /* FOR_AUTO_RUN==0 */
#include "test_vout.cpp"
#if FOR_AUTO_RUN==0
#include "test_fp.cpp"
#if (!defined( CFG_CHSSA_CHIP_MSTAR3T273) && !defined(CFG_CHSSA_CHIP_MSTAR7CX5) && !defined(CFG_CHSSA_CHIP_MSTAR7S33B) && !defined(CFG_CHSSA_CHIP_MSTAR7C123D))
#include "test_sc.cpp"
#endif
#include "test_iic.cpp"
#include "test_section.cpp"
#include "test_fsys.cpp"

#endif /* FOR_AUTO_RUN==0 */


#include "test_vwindow.cpp"
#if FOR_AUTO_RUN==0
#include "test_avfilter.cpp"
#include "test_pesfilter.cpp"

#include "test_gpio.cpp"
#include "test_tsmng.cpp"

#include "test_video.cpp"
#include "test_audio.cpp"

#if (!defined(CFG_CHSSA_CHIP_MSTAR3T273) && !defined(CFG_CHSSA_CHIP_MSTAR7C123D))
#include "test_dscram.cpp"
#endif
#include "test_av.cpp"

//#include "test_net.cpp"

//#include "test_scart.cpp"
//#include "test_rf.cpp"

#include "test_mplay.cpp"
#include "test_pvr.cpp"
#if (!defined(CFG_CHSSA_CHIP_MSTAR7C123D) && !defined(CFG_CHSSA_CHIP_MON_CC6000) 		\
	&& defined(CFG_CHSSA_CHIP_MON_CC5000)			\
	&& defined(CFG_CHSSA_CHIP_MSTAR5C58)		\
	&& defined(CFG_CHSSA_CHIP_MSTAR5C69))
#include "test_hdmi.cpp"
#endif /* CFG_CHSSA_CHIP_MON_CC6000 */

#include "test_perfo.cpp"

#include "test_demo.cpp"

#include "test_param.cpp"

#endif

//#include "test_usb.cpp"



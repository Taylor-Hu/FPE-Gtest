#ifndef __TEST_MENU_H__
#define __TEST_MENU_H__

#include "gtest/gtest.h"
#include "chdrv_evironment.h"
#include <iostream>
#include <string>
//#include <stdlib.h>
#include <time.h>
//#include "menu.h"



#define TEST_NET		0

#ifdef MAX
#undef MAX
#endif
#define MAX(a,b)	(((int)a) > ((int)b) ? (a) : (b))
#ifdef MIN
#undef MIN
#endif
#define MIN(a,b)	(((int)a) < ((int)b) ? (a) : (b))
#ifdef ABS
#undef ABS
#endif
#define ABS(a) (((int)a) >=0 ? (a) : -(a))


using namespace std;


#if ENABLE_TESTCASE
#define CASE_F(name, description) TEST_F(CASE_NAME, name, description)
#define CASE(name, description) TEST(CASE_NAME, name, description)
#define CASE_P(name, value_type, params, description)	\
	class name: public testing::TestWithParam<value_type> { };		\
INSTANTIATE_TEST_CASE_P(CASE_NAME,  name, params); 	\
TEST_P(name,RANGE_TEST, description)


using testing::internal::GetUnitTestImpl;
using testing::UnitTest;
#endif /* ENABLE_TESTCASE */

extern "C" {
#include "chdrv_include.h"
//#include "chmid_include.h"
#include "com_debug.h"

extern CH_RTOS_MemPartitionHandle  gh_MID_HEAP;
extern CH_RTOS_MemPartitionHandle  gh_APP_HEAP;
extern int CHDRV_PreviousConfig(void);
extern CH_BOOL CHDRV_Initialize();
extern int CHMID_Initialize( void );	
int dbg_checkmem(int min_nums);

}
#include "fw_env.h"
#include "gtest_cmd.h"
#include "debug_module.h"


char *test_get_menu_input(void);
//返回用户输入的全部字符串
char *test_get_input(void);
//用户输入yes, Y, y返回true，其他返回false
CH_BOOL test_input_yes(void);
//用户输入quit, QUIT, 	q, Q返回true，其他返回false
CH_BOOL test_input_quit(void);
//用户直接回车，返回true，其他返回false
CH_BOOL test_input_none(void);
//用户输入的字符串与str相等(不区分大小写)返回true，其他返回false
CH_BOOL test_input_is(const char *str);
//获取用户输入的数字
int test_input_i(void);
int test_input_i(char *p);
int main_menu(char *pstrFilter);
//读取指定的文件并将内容读到file_buf，file_buf的内存由本函数分配，返回已读取的文件长度
int fsys_read_file(const char *file_path, char *& file_buf);
int fsys_write_file(const char *file_path, char * file_buf, int len);
int request_play_program(const char *rpstr_stream, const char *rpstr_tuner, int loop=1);
CH_AUDIO_CODE_TYPE_e switch_multiaud(int *pi_aud_index, int *pi_aud_num);
void restart_av(void);
void stop_av(void);
void start_av(void);
int get_demux_id(char *p_tuner_params = NULL);

#endif


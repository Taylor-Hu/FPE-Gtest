#include "chdrv_evironment.h"

extern "C" {
#include "chdrv_include.h"
//#include "chdrv_test_uart.h"
}

#define INIT_RECT(rect, x, y, w, h) \
do{	\
	rect.s_X = x;		\
	rect.s_Y = y;	\
	rect.s_Width = w;	\
	rect.s_Height = h;	\
}while(0);	

#if ENABLE_TESTCASE
class CASE_NAME : public testing::Test {
  protected:
    static void SetUpTestCase(); 
    static void TearDownTestCase();
  
    virtual void SetUp();
    virtual void TearDown();
  
  // Some expensive resource shared by all tests.
 
};

#else
class CASE_NAME {
  protected:
    static void SetUpTestCase(); 
    static void TearDownTestCase();
    virtual void SetUp();
    virtual void TearDown();
};
#endif /* ENABLE_TESTCASE */


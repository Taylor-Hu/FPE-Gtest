#include "chdrv_evironment.h"

extern "C" {
#include "chdrv_include.h"
//#include "chdrv_test_uart.h"
}

class TestUsb : public testing::Test {
  protected:
    static void SetUpTestCase(); 
    static void TearDownTestCase();
  
    virtual void SetUp();
    virtual void TearDown();
  
  // Some expensive resource shared by all tests.
 
};

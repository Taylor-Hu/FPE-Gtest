#ifndef CHDRV_EVIRONMENT_H
#define CHDRV_EVIRONMENT_H

#include "gtest/gtest.h"
#include <iostream>

#if ENABLE_TESTCASE
class ChdrvEnvironment : public testing::Environment {
public:
    virtual void SetUp();
    virtual void TearDown();
    
    static int count;
};
#endif /* ENABLE_TESTCASE */

int show_prompt(const char *rpc_format, ...);
int clear_promt(void);
int delete_prompt_region(void);

int make_charmap_gb18030(void);

int gbk_to_unicode(unsigned short int* unicode, const char* gb, int len);

int show_title(const char *pTestName);


#endif //CHDRV_EVIRONMENT_H


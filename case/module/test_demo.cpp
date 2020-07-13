/**
@file test_gpio.cpp
@def group gpio测试集

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
#define CASE_NAME TestDemo


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

int Foo(int a, int b)
{
    if (a == 0 || b == 0)
    {
        LOG("param error:0");
		return 0;
    }
	
    int c = a % b;
    if (c == 0)
        return b;
    return Foo(b, c);
}

testing::AssertionResult AssertFoo(const char* m_expr, const char* n_expr, const char* k_expr, int m, int n, int k) {
    if (Foo(m, n) == k)
        return testing::AssertionSuccess();
    testing::Message msg;
    msg << m_expr << " 和 " << n_expr << " 的最大公约数应该是：" << Foo(m, n) << " 而不是：" << k_expr;
    return testing::AssertionFailure(msg);
}


CASE_F(TEST_ASSERT_STR,
	"测试字符串相关断言") {
	char* pszCoderZh = "CoderZh";
    wchar_t* wszCoderZh = L"CoderZh";
    std::string strCoderZh = "CoderZh";
//    std::wstring wstrCoderZh = L"CoderZh";

    EXPECT_STREQ("CoderZh", pszCoderZh) << "char* 字符串比较不成功";
    EXPECT_STREQ(L"CoderZh", wszCoderZh) << "wchar_t* 字符串比较不成功";

    EXPECT_STRNE("CnBlogs", pszCoderZh) << "char* 字符串比较不成功，应该不相等";
    EXPECT_STRNE(L"CnBlogs", wszCoderZh) << "wchar_t* 字符串比较不成功，应该不相等";

    EXPECT_STREQ("CoderZh", strCoderZh.c_str());
//    EXPECT_STREQ(L"CoderZh", wstrCoderZh.c_str());

}


CASE_F(TEST_ASSERT_1,
	"测试字符串相关断言") {
	EXPECT_FLOAT_EQ(12.344, 12.3445) << "float类型比较应该相等";		//expect失败之后仍会继续向下执行
	EXPECT_DOUBLE_EQ(1.1122, 1.11221) << "double类型比较应该相等";

	LOG("仍会继续向下执行该测试用例");
	
	ASSERT_FLOAT_EQ(12.34, 12.344) << "float类型比较应该相等";			//assert失败之后不会继续向下执行
	ASSERT_DOUBLE_EQ(1.1111, 1.11112222) << "double类型比较应该相等";

	LOG("不会继续向下执行该测试用例");
	
	EXPECT_FLOAT_EQ(12.34, 12.344) << "float类型比较应该相等";
	EXPECT_DOUBLE_EQ(1.1111, 1.11112222) << "double类型比较应该相等";

}

CASE_F(TEST_ASSERT_2,
	"测试自定义的断言格式") {
	
	 EXPECT_PRED_FORMAT3(AssertFoo, 3, 6, 2);

}


CASE_F(TEST_ASSERT_3,
	"测试fail运行结果1") {
	
	LOG("go this one\n");

   	FAIL(); // Fatal Assertion，不往下执行该案例。
	
    LOG("go this two\n");
	
}


CASE_F(TEST_ASSERT_4,
	"测试fail运行结果2") {
	
	LOG("执行FAIL之后是否会执行这句\n");//会执行到这个测试用例
	
}


#if 0
CASE(TEST_ASSERT_5,
	"测试case和case_f的区别") {
	LOG("执行case语句的测试用例\n");
}


//#define CASE_P(name, value_type, params, description)	\
//	class name: public testing::TestWithParam<value_type> { };		\
//INSTANTIATE_TEST_CASE_P(CASE_NAME,  name, params); 	\
//TEST_P(name,RANGE_TEST, description)

CASE_P(IsPrimeParamTest, int, testing::Values(3, 5, 11, 23, 17), "判断质数，测试参数化")
{
	int n =  GetParam();
	EXPECT_TRUE(IsPrime(n));

}
#endif



#endif /* ENABLE_TESTCASE */







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

//不需要包含这些头文件，也不需要定义case_name, 会报错

#if 0
/*测试集名称定义*/
#ifdef CASE_NAME
#undef CASE_NAME
#endif
#define CASE_NAME IsPrimeParamTest
#endif


//#include "test_menu.h"
//#include "test_modules.h"



#if ENABLE_TESTCASE


// Returns true iff n is a prime number.
bool IsPrime(int n)
{
    // Trivial case 1: small numbers
    if (n <= 1) return FALSE;

    // Trivial case 2: even numbers
    if (n % 2 == 0) return n == 2;

    // Now, we have that n is odd and n >= 3.

    // Try to divide n by every odd number i, starting from 3
    for (int i = 3; ; i += 2) {
        // We only have to try i up to the squre root of n
        if (i > n/i) break;

        // Now, we have i <= n/i < n.
        // If n is divisible by i, n is not prime.
        if (n % i == 0) return FALSE;
    }
    // n has no integer factor in the range (1, n), and thus is prime.
    return TRUE;
}


#if 1

//#define CASE_P(name, value_type, params, description)	\
//	class name: public testing::TestWithParam<value_type> { };		\
//INSTANTIATE_TEST_CASE_P(CASE_NAME,  name, params); 	\
//TEST_P(name,RANGE_TEST, description)


//[ RUN      ] CASE_NAME/IsPrimeParamTest1.RANGE_TEST/1
//                                                    [       OK ] CASE_NAME/IsPrimeParamTest1.RANGE_TEST/1 (0 ms)
//testsuite name: CASE_NAME/IsPrimeParamTest1
//testcase name: RANGE_TEST

//IsPrimeParamTest继承方法可以仿照class CASE_NAME : public testing::Test {}写在test_modules.h文件中


CASE_P(IsPrimeParamTest1, int, testing::Values(3, 5, 11, 23, 17), "判断质数，测试参数化")
{
	int n =  GetParam();
    EXPECT_TRUE(IsPrime(n));
}

#endif

#if 0
//[ RUN      ] TrueReturn/IsPrimeParamTest.HandleTrueReturn/1
//                                                          [       OK ] TrueReturn/IsPrimeParamTest.HandleTrueReturn/1 (0 ms)
//testsuite name: TrueReturn/IsPrimeParamTest
//testcase name: HandleTrueReturn
class IsPrimeParamTest : public::testing::TestWithParam<int>
{

};

// 使用参数化测试，只需要：
TEST_P(IsPrimeParamTest, HandleTrueReturn, "参数测试")
{
    int n =  GetParam();
    EXPECT_TRUE(IsPrime(n));
}

// 定义参数
INSTANTIATE_TEST_CASE_P(TrueReturn, IsPrimeParamTest, testing::Values(3, 5, 11, 23, 17));

#if 0
int main(int argc,char *argv[])
{
    testing::InitGoogleTest(&argc,argv);
    RUN_ALL_TESTS();
    return 0;
}
#endif

#endif

#endif /* ENABLE_TESTCASE */















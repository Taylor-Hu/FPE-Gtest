/**
@file test_gpio.cpp
@def group gpio���Լ�

@author ���繫˾����������:	����ǿ
@date   2014��4��10��  
@version V4.0
@todo     ---ʱ��---����-------------�޸Ĳ��ּ�ԭ��----------
@todo     1 2014��4��10�� ����ǿ  ����


@{
*/
/*���Լ����ƶ���*/

//����Ҫ������Щͷ�ļ���Ҳ����Ҫ����case_name, �ᱨ��

#if 0
/*���Լ����ƶ���*/
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

//IsPrimeParamTest�̳з������Է���class CASE_NAME : public testing::Test {}д��test_modules.h�ļ���


CASE_P(IsPrimeParamTest1, int, testing::Values(3, 5, 11, 23, 17), "�ж����������Բ�����")
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

// ʹ�ò��������ԣ�ֻ��Ҫ��
TEST_P(IsPrimeParamTest, HandleTrueReturn, "��������")
{
    int n =  GetParam();
    EXPECT_TRUE(IsPrime(n));
}

// �������
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















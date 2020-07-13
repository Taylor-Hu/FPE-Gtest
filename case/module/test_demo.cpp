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
    msg << m_expr << " �� " << n_expr << " �����Լ��Ӧ���ǣ�" << Foo(m, n) << " �����ǣ�" << k_expr;
    return testing::AssertionFailure(msg);
}


CASE_F(TEST_ASSERT_STR,
	"�����ַ�����ض���") {
	char* pszCoderZh = "CoderZh";
    wchar_t* wszCoderZh = L"CoderZh";
    std::string strCoderZh = "CoderZh";
//    std::wstring wstrCoderZh = L"CoderZh";

    EXPECT_STREQ("CoderZh", pszCoderZh) << "char* �ַ����Ƚϲ��ɹ�";
    EXPECT_STREQ(L"CoderZh", wszCoderZh) << "wchar_t* �ַ����Ƚϲ��ɹ�";

    EXPECT_STRNE("CnBlogs", pszCoderZh) << "char* �ַ����Ƚϲ��ɹ���Ӧ�ò����";
    EXPECT_STRNE(L"CnBlogs", wszCoderZh) << "wchar_t* �ַ����Ƚϲ��ɹ���Ӧ�ò����";

    EXPECT_STREQ("CoderZh", strCoderZh.c_str());
//    EXPECT_STREQ(L"CoderZh", wstrCoderZh.c_str());

}


CASE_F(TEST_ASSERT_1,
	"�����ַ�����ض���") {
	EXPECT_FLOAT_EQ(12.344, 12.3445) << "float���ͱȽ�Ӧ�����";		//expectʧ��֮���Ի��������ִ��
	EXPECT_DOUBLE_EQ(1.1122, 1.11221) << "double���ͱȽ�Ӧ�����";

	LOG("�Ի��������ִ�иò�������");
	
	ASSERT_FLOAT_EQ(12.34, 12.344) << "float���ͱȽ�Ӧ�����";			//assertʧ��֮�󲻻��������ִ��
	ASSERT_DOUBLE_EQ(1.1111, 1.11112222) << "double���ͱȽ�Ӧ�����";

	LOG("�����������ִ�иò�������");
	
	EXPECT_FLOAT_EQ(12.34, 12.344) << "float���ͱȽ�Ӧ�����";
	EXPECT_DOUBLE_EQ(1.1111, 1.11112222) << "double���ͱȽ�Ӧ�����";

}

CASE_F(TEST_ASSERT_2,
	"�����Զ���Ķ��Ը�ʽ") {
	
	 EXPECT_PRED_FORMAT3(AssertFoo, 3, 6, 2);

}


CASE_F(TEST_ASSERT_3,
	"����fail���н��1") {
	
	LOG("go this one\n");

   	FAIL(); // Fatal Assertion��������ִ�иð�����
	
    LOG("go this two\n");
	
}


CASE_F(TEST_ASSERT_4,
	"����fail���н��2") {
	
	LOG("ִ��FAIL֮���Ƿ��ִ�����\n");//��ִ�е������������
	
}


#if 0
CASE(TEST_ASSERT_5,
	"����case��case_f������") {
	LOG("ִ��case���Ĳ�������\n");
}


//#define CASE_P(name, value_type, params, description)	\
//	class name: public testing::TestWithParam<value_type> { };		\
//INSTANTIATE_TEST_CASE_P(CASE_NAME,  name, params); 	\
//TEST_P(name,RANGE_TEST, description)

CASE_P(IsPrimeParamTest, int, testing::Values(3, 5, 11, 23, 17), "�ж����������Բ�����")
{
	int n =  GetParam();
	EXPECT_TRUE(IsPrime(n));

}
#endif



#endif /* ENABLE_TESTCASE */







#ifndef __DONT_INCLUDED_THIS_H__
#define __DONT_INCLUDED_THIS_H__
/*不要包含这个头文件，只是说明常用的宏的用法*/

/*
=====================================================<<
EXPECT类,当条件不成立时，仅仅给出错误报告，程序继续运行
*/
//if(condition)
#define EXPECT_TRUE(condition) if(!(condition))LOG("ERROR! %s==false\n", #condition);

//if(!condition)
#define EXPECT_FALSE(condition) if((condition))LOG("ERROR! %s==true\n", #condition);

//if(expected == actual)
#define EXPECT_EQ(expected, actual)	if((expected) != (actual))LOG("ERROR! %s!=%s\n", #expected, #actual);

//if(expected != actual)
#define EXPECT_NE(expected, actual) if((expected) == (actual))LOG("ERROR! %s==%s\n", #expected, #actual);

//if(val1 <= val2)
#define EXPECT_LE(val1, val2) if((val1) > (val2))LOG("ERROR! %s > %s\n", #val1, #val2);

//if(val1 < val2)
#define EXPECT_LT(val1, val2) if((val1) >= (val2))LOG("ERROR! %s >= %s\n", #val1, #val2);

//if(val1 >= val2)
#define EXPECT_GE(val1, val2)		if((val1) < (val2))LOG("ERROR! %s < %s\n", #val1, #val2);

//if(val1 > val2)
#define EXPECT_GT(val1, val2)		if((val1) <= (val2))LOG("ERROR! %s <= %s\n", #val1, #val2);

//if(strcmp(expected, actual) == 0)
#define EXPECT_STREQ(expected, actual)  if(strcmp((expected),(actual)) != 0)LOG("ERROR! %s != %s\n", #expected, #actual);

//if(strcmp(expected, actual) != 0)
#define EXPECT_STRNE(s1, s2) 	 if(strcmp((s1),(s2)) == 0)LOG("ERROR! %s == %s\n", #s1, #s2);

//if(stricmp(expected, actual) == 0)
#define EXPECT_STRCASEEQ(expected, actual) 

//if(stricmp(s1, s2) != 0)
#define EXPECT_STRCASENE(s1, s2)

//if(expected == actual) //float
#define EXPECT_FLOAT_EQ(expected, actual)

//if(expected == actual)  //double
#define EXPECT_DOUBLE_EQ(expected, actual)
  
/*
>>=====================================================
*/

/*
=====================================================<<
ASSERT类,当条件不成立时，给出错误报告，并终止测试
*/
//if(condition)
#define ASSERT_TRUE(condition) 

//if(!condition)
#define ASSERT_FALSE(condition)

//if(expected == actual)
#define ASSERT_EQ(expected, actual)

//if(expected != actual)
#define ASSERT_NE(expected, actual)

//if(val1 <= val2)
#define ASSERT_LE(val1, val2) 

//if(val1 < val2)
#define ASSERT_LT(val1, val2)

//if(val1 >= val2)
#define ASSERT_GE(val1, val2)

//if(val1 > val2)
#define ASSERT_GT(val1, val2)

//if(strcmp(expected, actual) == 0)
#define ASSERT_STREQ(expected, actual) 

//if(strcmp(expected, actual) != 0)
#define ASSERT_STRNE(s1, s2) 

//if(stricmp(expected, actual) == 0)
#define ASSERT_STRCASEEQ(expected, actual) 

//if(stricmp(s1, s2) != 0)
#define ASSERT_STRCASENE(s1, s2)

//if(expected == actual) //float
#define ASSERT_FLOAT_EQ(expected, actual)

//if(expected == actual)  //double
#define ASSERT_DOUBLE_EQ(expected, actual)
/*
>>=====================================================
*/

#define FAIL()
#endif

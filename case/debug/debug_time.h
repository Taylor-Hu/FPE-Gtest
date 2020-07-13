//-----------换台时间函数测试----------->
#ifndef __DEBUG_TIME_H__
#define __DEBUG_TIME_H__
#define MAX_CHANNEL_TEST_TIMES  200 //最大统计200次


typedef enum
{
	TEST_KEY_START,/*0*/
	TEST_KEY_SEND,/*1*/
	TEST_SET_FREQ,/*2*/
	TEST_TUNER_LOCK,/*3*/
	TEST_SET_AVFILTER,/*4*/
	TEST_AUDIO_START,
	TEST_VIDEO_START,
	TEST_AUDIO_STOP,
	TEST_VIDEO_STOP,
	TEST_SET_ECM_FILTER,/*5*/
	TEST_OPEN_DECODER,/*6*/
	TEST_PCR_COME,/*7*/
	TEST_ECM_COME,/*8*/
	TEST_SET_CW,/*9*/
	TEST_IFRAME_COME,/*10*/
	TEST_VID_SYNC,/*11*/
	TEST_VIDEO_SHOW,/*12*/
	TEST_PMT_COME,/*13*/
	TEST_REQUEST_PAT,/*14*/
	TEST_REQUEST_PMT,/*15*/
	TEST_PAT_COME,/*16*/
	TEST_DEMOD_LOCK,/*17*/
	TEST_SET_FREQ2,/*18*/
	TEST_SET_FREQ3,/*19*/
	TEST_AUD_NEWFRAME,
	TEST_STEP_MAX
}TEST_STEP_e;

/**FuncName:Test_ChannelTestTime
@brief ... 
@param [in] step 参见TEST_STEP_e, -1打印汇总的测试结果
@param [out] none 
@return success  
@return fail     
@attention TEST_KEY_START表示一轮换台测试的开始，所有的时间以此为基准进行，必须要在最开始调用。*/
void Test_ChannelRecordTime(int step);

/**FuncName:get_channel_switch_time
@brief 得到换台时某个步骤的耗时平均值
@param [in] step 参见TEST_STEP_e
@param [out] none 
@return success  
@return fail     
@attention 失败返回-1*/
int get_channel_switch_time(int step);

#endif


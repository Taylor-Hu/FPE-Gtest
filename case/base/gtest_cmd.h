#ifndef ___GTEST_CMD_H__
#define ___GTEST_CMD_H__

#include<vector>
#include <string>
#include <stdarg.h> 

using namespace std;

enum
{
	type_none,
	type_quotes,
	type_double_quotes,
};

class CMD_PARAM
{
public:
	CMD_PARAM(const char* value);
	CMD_PARAM(int value);
	CMD_PARAM();

private:
	int i;
	std::string str;
	CH_BOOL b;
	float f;

public:
	operator int() const { return i;}
	operator 	const char *  () const {return str.c_str();}
	//operator float() const { return f;}
	

	int get_i() const {return i;}			//取得整形值
	char *get_str(){
		return (char*)str.c_str();}	//取得字符串
	CH_BOOL get_b(){return b;}	//取得布尔值
	float get_f(){return f;}		//取得浮点值
	void operator= (const char* value);
};


typedef int (* cmd_exec)(int argc, CMD_PARAM argv[]);

/*
说明:
function 函数体
description 字符串，简单描述这个命令的功能
params 字符串，参数列表，用简明的字段描述每个参数的功能，空格视为参数之间的分隔符
example 字符串，对应params，给出最常用的一组参数示例(字符串中不要包含命令本身)
complete 函数体，定义命令自动完成功能，一般保持为NULL
*/


#define REGISTER_CMD(function, description, params,  example)	\
	 class_cmd_register gclass_##function(function, #function, description, params, example);

#define RUN_CMD(cmd_line, args...) get_global_cmd().exec(cmd_line, ##args)





typedef struct
{
	char *name;
	const char *eg;
	const char *usage;
	const char *description;
	char *params_list[32];
	char *params_line;
	cmd_exec exec;
	int param_nums;
}private_cmd_t;


class test_cmd
{
public:
	test_cmd();
	int Register(cmd_exec exec, const char* name, const char *description, const char *usage, const char *eg);
	int lookup(const char* name);
	void help(char *cmd);
	int exec(const char *cmd_line, ...);
	void cmd(void);
	int cmd_num(void){return list_.size();}
	private_cmd_t* get_cmd(unsigned int i){if(i >= 0 && i < list_.size())return &list_[i];return NULL;}
	~test_cmd();
	
	
	
private:
	vector <private_cmd_t> list_;
	int exec_private(char *cmd_line);
	
	int remove_character(char *pstr, int i);
	int do_sentence_close(char *pstr, int i, int *flag);
	char *strtok_func(char *strToken, const char *strDelimit);

};

extern test_cmd &get_global_cmd();


class class_cmd_register	
{	
	private:
		
	class_cmd_register(class_cmd_register const & rhs);
	public:		
	class_cmd_register(cmd_exec exec, const char* name,  const char *description, const char *usage, const char *eg){
	get_global_cmd().Register(exec, name,  description, usage, eg);	
	}
};



#endif

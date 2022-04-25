#include <cassert>
#include <fstream>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include "koopa.h"
using namespace std;

void parse_str(const char* str) {
	// 解析字符串 str, 得到 Koopa IR 程序
	koopa_program_t program;
	koopa_error_code_t ret = koopa_parse_from_string(str, &program);
	assert(ret == KOOPA_EC_SUCCESS);  // 确保解析时没有出错
	// 创建一个raw program builder,用来构建raw program
	koopa_raw_program_builder_t builder = koopa_new_raw_program_builder();
	// 将Koopa IR程序转换为raw program
	koopa_raw_program_t raw = koopa_build_raw_program(builder, program);
	// 释放Koopa IR程序占用的内存
	koopa_delete_program(program);

	cout << "  .text" << endl;
	// 处理raw program
	// 使用for循环遍历函数列表
	for (size_t i = 0; i < raw.funcs.len; ++i) {
		// 正常情况下, 列表中的元素就是函数, 我们只不过是在确认这个事实
	   // 当然, 你也可以基于 raw slice 的 kind, 实现一个通用的处理函数
		assert(raw.funcs.kind == KOOPA_RSIK_FUNCTION);
		// 获取当前函数
		koopa_raw_function_t func = (koopa_raw_function_t)raw.funcs.buffer[i];
		cout << "  .global " << func->name + 1 << endl;
		cout << func->name + 1 << ":" << endl;
		// 进一步处理当前函数
		for (size_t j = 0; j < func->bbs.len; ++j) {
			assert(func->bbs.kind == KOOPA_RSIK_BASIC_BLOCK);
			koopa_raw_basic_block_t bb = (koopa_raw_basic_block_t)func->bbs.buffer[j];
			// 进一步处理当前基本块
			for (size_t k = 0; k < bb->insts.len; ++k) {
				koopa_raw_value_t value = (koopa_raw_value_t)bb->insts.buffer[k];
				// 示例程序中, 你得到的 value 一定是一条 return 指令
				assert(value->kind.tag == KOOPA_RVT_RETURN);
				cout << "  li " << "a0, ";
				// 于是我们可以按照处理 return 指令的方式处理这个 value
				// return 指令中, value 代表返回值
				koopa_raw_value_t ret_value = value->kind.data.ret.value;
				// 示例程序中, ret_value 一定是一个 integer
				assert(ret_value->kind.tag == KOOPA_RVT_INTEGER);
				// 于是我们可以按照处理 integer 的方式处理 ret_value
				// integer 中, value 代表整数的数值
				int32_t int_val = ret_value->kind.data.integer.value;
				// 示例程序中, 这个数值一定是 0
				// assert(int_val == 0);
				cout << int_val << endl << "  ret" << endl;
			}
		}
	}

	// 处理完成, 释放 raw program builder 占用的内存
	// 注意, raw program 中所有的指针指向的内存均为 raw program builder 的内存
	// 所以不要在 raw program 处理完毕之前释放 builder
	koopa_delete_raw_program_builder(builder);

}
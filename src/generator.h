#include <cassert>
#include <fstream>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <cstring>
#include "koopa.h"
using namespace std;

typedef struct {
	char name[3];
	int* address;
}InstrLoc;

InstrLoc Reg[15]; // 前8个为a，后7个为t
inline char* regfind(int* addr) {
	for (int i = 0; i < 15; i++) {
		if (Reg[i].address == addr) {
			return Reg[i].name;
		}
	}
	return "error";
}
void parse_str(const char* str) {
	// 解析字符串 str, 得到 Koopa IR 程序
	int now = 0;
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
				// 二元运算
				if (value->kind.tag == KOOPA_RVT_BINARY) {
					koopa_raw_binary_t val = value->kind.data.binary;
					char tmp = now + '0', l[3], r[3];
					l[2] = r[2] = '\0';
					if (val.lhs->kind.tag == KOOPA_RVT_INTEGER && val.lhs->kind.data.integer.value == 0) {
						l[0] = 'x', l[1] = '0';
					}
					else if (val.lhs->kind.tag == KOOPA_RVT_INTEGER) {
						cout << "  li    " << "a" << tmp << ", " << val.lhs->kind.data.integer.value << endl;
						l[0] = 'a', l[1] = tmp;
						tmp++;
					}
					else if (val.lhs->kind.tag == KOOPA_RVT_BINARY) {
						char* t = regfind((int*)val.lhs);
						assert(strcmp(t, "error"));
						l[0] = t[0], l[1] = t[1];
					}
					if (val.rhs->kind.tag == KOOPA_RVT_INTEGER && val.rhs->kind.data.integer.value == 0) {
						r[0] = 'x', r[1] = '0';
					}
					else if (val.rhs->kind.tag == KOOPA_RVT_INTEGER) {
						cout << "  li    " << "a" << tmp << ", " << val.rhs->kind.data.integer.value << endl;
						r[0] = 'a', r[1] = tmp;
					}
					else if (val.rhs->kind.tag == KOOPA_RVT_BINARY) {
						char* t = regfind((int*)val.rhs);
						assert(strcmp(t, "error"));
						r[0] = t[0], r[1] = t[1];
					}
					if (val.op == KOOPA_RBO_EQ) {
						cout << "  xor   " << "a" << now << ", " << l << ", " << r << endl;
						cout << "  seqz  " << "a" << now << ", " << "a" << now << endl;
						
					}
					else if (val.op == KOOPA_RBO_SUB) {
						cout << "  sub   " << "a" << now << ", " << l << ", " << r << endl;
					}
					else if (val.op == KOOPA_RBO_ADD) {
						cout << "  add   " << "a" << now << ", " << l << ", " << r << endl;
					}
					char tt[3];
					tt[0] = 'a', tt[1] = now + '0', tt[2] = '\0';
					strcpy(Reg[now].name, tt);
					Reg[now].address = (int*)value;
					now++;
				}
				else if (value->kind.tag == KOOPA_RVT_RETURN) {
					koopa_raw_value_t ret_value = value->kind.data.ret.value;
					if (ret_value->kind.tag == KOOPA_RVT_INTEGER) {
						cout << "  li    " << "a0, " << ret_value->kind.data.integer.value << endl << "  ret" << endl;
					}
					else if (ret_value->kind.tag == KOOPA_RVT_BINARY) {
						cout << "  mv    " << "a0, " << regfind((int*)ret_value) << endl << "  ret" << endl;
					}
				}
			}
		}
	}

	// 处理完成, 释放 raw program builder 占用的内存
	// 注意, raw program 中所有的指针指向的内存均为 raw program builder 的内存
	// 所以不要在 raw program 处理完毕之前释放 builder
	koopa_delete_raw_program_builder(builder);

}
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

InstrLoc Reg[15]; // ǰ8��Ϊa����7��Ϊt
inline char* regfind(int* addr) {
	for (int i = 0; i < 15; i++) {
		if (Reg[i].address == addr) {
			return Reg[i].name;
		}
	}
	return "error";
}
void parse_str(const char* str) {
	// �����ַ��� str, �õ� Koopa IR ����
	int now = 0;
	koopa_program_t program;
	koopa_error_code_t ret = koopa_parse_from_string(str, &program);
	assert(ret == KOOPA_EC_SUCCESS);  // ȷ������ʱû�г���
	// ����һ��raw program builder,��������raw program
	koopa_raw_program_builder_t builder = koopa_new_raw_program_builder();
	// ��Koopa IR����ת��Ϊraw program
	koopa_raw_program_t raw = koopa_build_raw_program(builder, program);
	// �ͷ�Koopa IR����ռ�õ��ڴ�
	koopa_delete_program(program);

	cout << "  .text" << endl;
	// ����raw program
	// ʹ��forѭ�����������б�
	for (size_t i = 0; i < raw.funcs.len; ++i) {
		// ���������, �б��е�Ԫ�ؾ��Ǻ���, ����ֻ��������ȷ�������ʵ
	   // ��Ȼ, ��Ҳ���Ի��� raw slice �� kind, ʵ��һ��ͨ�õĴ�����
		assert(raw.funcs.kind == KOOPA_RSIK_FUNCTION);
		// ��ȡ��ǰ����
		koopa_raw_function_t func = (koopa_raw_function_t)raw.funcs.buffer[i];
		cout << "  .global " << func->name + 1 << endl;
		cout << func->name + 1 << ":" << endl;
		// ��һ������ǰ����
		for (size_t j = 0; j < func->bbs.len; ++j) {
			assert(func->bbs.kind == KOOPA_RSIK_BASIC_BLOCK);
			koopa_raw_basic_block_t bb = (koopa_raw_basic_block_t)func->bbs.buffer[j];
			// ��һ������ǰ������
			for (size_t k = 0; k < bb->insts.len; ++k) {
				koopa_raw_value_t value = (koopa_raw_value_t)bb->insts.buffer[k];
				// ��Ԫ����
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

	// �������, �ͷ� raw program builder ռ�õ��ڴ�
	// ע��, raw program �����е�ָ��ָ����ڴ��Ϊ raw program builder ���ڴ�
	// ���Բ�Ҫ�� raw program �������֮ǰ�ͷ� builder
	koopa_delete_raw_program_builder(builder);

}
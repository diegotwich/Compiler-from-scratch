#include <cassert>
#include <fstream>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include "koopa.h"
using namespace std;

void parse_str(const char* str) {
	// �����ַ��� str, �õ� Koopa IR ����
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
				// ʾ��������, ��õ��� value һ����һ�� return ָ��
				assert(value->kind.tag == KOOPA_RVT_RETURN);
				cout << "  li " << "a0, ";
				// �������ǿ��԰��մ��� return ָ��ķ�ʽ������� value
				// return ָ����, value ������ֵ
				koopa_raw_value_t ret_value = value->kind.data.ret.value;
				// ʾ��������, ret_value һ����һ�� integer
				assert(ret_value->kind.tag == KOOPA_RVT_INTEGER);
				// �������ǿ��԰��մ��� integer �ķ�ʽ���� ret_value
				// integer ��, value ������������ֵ
				int32_t int_val = ret_value->kind.data.integer.value;
				// ʾ��������, �����ֵһ���� 0
				// assert(int_val == 0);
				cout << int_val << endl << "  ret" << endl;
			}
		}
	}

	// �������, �ͷ� raw program builder ռ�õ��ڴ�
	// ע��, raw program �����е�ָ��ָ����ڴ��Ϊ raw program builder ���ڴ�
	// ���Բ�Ҫ�� raw program �������֮ǰ�ͷ� builder
	koopa_delete_raw_program_builder(builder);

}
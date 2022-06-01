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
	// char name[3];
	int val;
	int* address;
}InstrLoc;

struct InstrLocList {
	InstrLoc loc;
	InstrLocList* next;
};
InstrLocList* List;
/*
InstrLoc Reg[15]; // ǰ8��Ϊa����7��Ϊt
inline char* regfind(int* addr) {
	for (int i = 0; i < 15; i++) {
		if (Reg[i].address == addr) {
			return Reg[i].name;
		}
	}
	return "error";
}

static char c[3];

inline char* getReg(int num) {
	if (num <= 7) {
		c[0] = 'a', c[1] = num + '0', c[2] = '\0';
	}
	else {
		num -= 8;
		c[0] = 't', c[1] = num + '0', c[2] = '\0';
	}
	return c;
}
*/
int LocFind(int* addr) {
	InstrLocList* tmp = List;
	while (tmp != NULL) {
		if (tmp->loc.address == addr) {
			return tmp->loc.val;
		}
		tmp = tmp->next;
	}
	return -1;
}

InstrLocList* LocInsert(int* addr, int val) {
	if (List == NULL) {
		List = new InstrLocList;
		List->loc.address = addr;
		List->loc.val = val;
		List->next = NULL;
		return List;
	}
	else {
		InstrLocList* tmp = List;
		while (tmp->next != NULL) {
			tmp = tmp->next;
		}
		InstrLocList* p = new InstrLocList;
		p->loc.address = addr;
		p->loc.val = val;
		p->next = NULL;
		tmp->next = p;
		return p;
	}
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
		// ���������������ƫ����
		int offset = 0;
		for (size_t j = 0; j < func->bbs.len; ++j) {
			assert(func->bbs.kind == KOOPA_RSIK_BASIC_BLOCK);
			koopa_raw_basic_block_t bb = (koopa_raw_basic_block_t)func->bbs.buffer[j];
			for (size_t k = 0; k < bb->insts.len; ++k) {
				koopa_raw_value_t value = (koopa_raw_value_t)bb->insts.buffer[k];
				if (value->ty->tag == KOOPA_RTT_UNIT) continue;
				offset += 4;
			}
		}
		if (offset % 16 != 0) offset = offset / 16 * 16 + 16; //����
		if (offset > 2048) {
			cout << "  li    t0, " << -offset << endl;
			cout << "  add   sp, sp, t0" << endl;
		}
		else {
			cout << "  addi  sp, sp, " << -offset << endl;
		}
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
					char l[3], r[3];
					l[2] = r[2] = '\0';
					if (val.lhs->kind.tag == KOOPA_RVT_INTEGER && val.lhs->kind.data.integer.value == 0) {
						l[0] = 'x', l[1] = '0';
					}
					else if (val.lhs->kind.tag == KOOPA_RVT_INTEGER) {
						l[0] = 't', l[1] = '0';
						cout << "  li    " << l << ", " << val.lhs->kind.data.integer.value << endl;
					}
					else if (val.lhs->kind.tag == KOOPA_RVT_BINARY || val.lhs->kind.tag == KOOPA_RVT_LOAD || val.lhs->kind.tag == KOOPA_RVT_ALLOC) {
						int loc = LocFind((int*)val.lhs);
						assert(loc >= 0);
						if (loc < 2048) {
							cout << "  lw    t0, " << loc << "(sp)" << endl;
						}
						else {
							cout << "  li    a0, " << loc << endl;
							cout << "  add   a0, a0, sp" << endl;
							cout << "  lw    t0, a0" << endl;
						}
						l[0] = 't', l[1] = '0';
					}
					if (val.rhs->kind.tag == KOOPA_RVT_INTEGER && val.rhs->kind.data.integer.value == 0) {
						r[0] = 'x', r[1] = '0';
					}
					else if (val.rhs->kind.tag == KOOPA_RVT_INTEGER) {
						r[0] = 't', r[1] = '1';
						cout << "  li    " << r << ", " << val.rhs->kind.data.integer.value << endl;
					}
					else if (val.rhs->kind.tag == KOOPA_RVT_BINARY || val.rhs->kind.tag == KOOPA_RVT_LOAD || val.rhs->kind.tag == KOOPA_RVT_ALLOC) {
						int loc = LocFind((int*)val.rhs);
						assert(loc >= 0);
						if (loc < 2048) {
							cout << "  lw    t1, " << loc << "(sp)" << endl;
						}
						else {
							cout << "  li    a0, " << loc << endl;
							cout << "  add   a0, a0, sp" << endl;
							cout << "  lw    t1, a0" << endl;
						}
						r[0] = 't', r[1] = '1';
					}
					if (val.op == KOOPA_RBO_EQ) {
						cout << "  xor   t0" << ", " << l << ", " << r << endl;
						cout << "  seqz  t0, t0" << endl;
						if (now < 2048) {
							cout << "  sw    t0, " << now << "(sp)" << endl;
						}
						else {
							cout << "  li    a0, " << now << endl;
							cout << "  add   a0, a0, sp" << endl;
							cout << "  sw    t0, a0" << endl;
						}
					}
					else if (val.op == KOOPA_RBO_SUB) {
						//cout << "  sub   " << getReg(now) << ", " << l << ", " << r << endl;
						cout << "  sub   t0, " << l << ", " << r << endl;
						if (now < 2048) {
							cout << "  sw    t0, " << now << "(sp)" << endl;
						}
						else {
							cout << "  li    a0, " << now << endl;
							cout << "  add   a0, a0, sp" << endl;
							cout << "  sw    t0, a0" << endl;
						}
					}
					else if (val.op == KOOPA_RBO_ADD) {
						//cout << "  add   " << getReg(now) << ", " << l << ", " << r << endl;
						cout << "  add   t0, " << l << ", " << r << endl;
						if (now < 2048) {
							cout << "  sw    t0, " << now << "(sp)" << endl;
						}
						else {
							cout << "  li    a0, " << now << endl;
							cout << "  add   a0, a0, sp" << endl;
							cout << "  sw    t0, a0" << endl;
						}
					}
					else if (val.op == KOOPA_RBO_MUL) {
						//cout << "  mul   " << getReg(now) << ", " << l << ", " << r << endl;
						cout << "  mul   t0, " << l << ", " << r << endl;
						if (now < 2048) {
							cout << "  sw    t0, " << now << "(sp)" << endl;
						}
						else {
							cout << "  li    a0, " << now << endl;
							cout << "  add   a0, a0, sp" << endl;
							cout << "  sw    t0, a0" << endl;
						}
					}
					else if (val.op == KOOPA_RBO_DIV) {
						//cout << "  div   " << getReg(now) << ", " << l << ", " << r << endl;
						cout << "  div   t0, " << l << ", " << r << endl;
						if (now < 2048) {
							cout << "  sw    t0, " << now << "(sp)" << endl;
						}
						else {
							cout << "  li    a0, " << now << endl;
							cout << "  add   a0, a0, sp" << endl;
							cout << "  sw    t0, a0" << endl;
						}
					}
					else if (val.op == KOOPA_RBO_MOD) {
						//cout << "  rem   " << getReg(now) << ", " << l << ", " << r << endl;
						cout << "  rem   t0, " << l << ", " << r << endl;
						if (now < 2048) {
							cout << "  sw    t0, " << now << "(sp)" << endl;
						}
						else {
							cout << "  li    a0, " << now << endl;
							cout << "  add   a0, a0, sp" << endl;
							cout << "  sw    t0, a0" << endl;
						}
					}
					else if (val.op == KOOPA_RBO_NOT_EQ) {
						cout << "  xor   t0, " << l << ", " << r << endl;
						cout << "  snez  t0, t0" << endl;
						if (now < 2048) {
							cout << "  sw    t0, " << now << "(sp)" << endl;
						}
						else {
							cout << "  li    a0, " << now << endl;
							cout << "  add   a0, a0, sp" << endl;
							cout << "  sw    t0, a0" << endl;
						}
					}
					else if (val.op == KOOPA_RBO_GT) {
						cout << "  sgt   t0, " << l << ", " << r << endl;
						if (now < 2048) {
							cout << "  sw    t0, " << now << "(sp)" << endl;
						}
						else {
							cout << "  li    a0, " << now << endl;
							cout << "  add   a0, a0, sp" << endl;
							cout << "  sw    t0, a0" << endl;
						}
					}
					else if (val.op == KOOPA_RBO_LT) {
						cout << "  slt   t0, " << l << ", " << r << endl;
						if (now < 2048) {
							cout << "  sw    t0, " << now << "(sp)" << endl;
						}
						else {
							cout << "  li    a0, " << now << endl;
							cout << "  add   a0, a0, sp" << endl;
							cout << "  sw    t0, a0" << endl;
						}
					}
					else if (val.op == KOOPA_RBO_GE) {
						cout << "  slt   t0, " << l << ", " << r << endl;
						cout << "  seqz  t0, t0" << endl;
						if (now < 2048) {
							cout << "  sw    t0, " << now << "(sp)" << endl;
						}
						else {
							cout << "  li    a0, " << now << endl;
							cout << "  add   a0, a0, sp" << endl;
							cout << "  sw    t0, a0" << endl;
						}
					}
					else if (val.op == KOOPA_RBO_LE) {
						cout << "  sgt   t0, " << l << ", " << r << endl;
						cout << "  seqz  t0, t0" << endl;
						if (now < 2048) {
							cout << "  sw    t0, " << now << "(sp)" << endl;
						}
						else {
							cout << "  li    a0, " << now << endl;
							cout << "  add   a0, a0, sp" << endl;
							cout << "  sw    t0, a0" << endl;
						}
					}
					else if (val.op == KOOPA_RBO_AND) {
						cout << "  and   t0, " << l << ", " << r << endl;
						if (now < 2048) {
							cout << "  sw    t0, " << now << "(sp)" << endl;
						}
						else {
							cout << "  li    a0, " << now << endl;
							cout << "  add   a0, a0, sp" << endl;
							cout << "  sw    t0, a0" << endl;
						}
					}
					else if (val.op == KOOPA_RBO_OR) {
						cout << "  or    t0, " << l << ", " << r << endl;
						if (now < 2048) {
							cout << "  sw    t0, " << now << "(sp)" << endl;
						}
						else {
							cout << "  li    a0, " << now << endl;
							cout << "  add   a0, a0, sp" << endl;
							cout << "  sw    t0, a0" << endl;
						}
					}
					LocInsert((int*)value, now);
					now += 4;
				}
				else if (value->kind.tag == KOOPA_RVT_RETURN) {
					koopa_raw_value_t ret_value = value->kind.data.ret.value;
					if (ret_value->kind.tag == KOOPA_RVT_INTEGER) {
						cout << "  li    " << "a0, " << ret_value->kind.data.integer.value << endl;
					}
					else if (ret_value->kind.tag == KOOPA_RVT_BINARY || ret_value->kind.tag == KOOPA_RVT_LOAD || ret_value->kind.tag == KOOPA_RVT_ALLOC) {
						int tmp = LocFind((int*)ret_value);
						if (tmp < 2048) {
							cout << "  lw    a0, " << tmp << "(sp)" << endl;
						}
						else {
							cout << "  li    a0, " << tmp << endl;
							cout << "  add   a0, a0, sp" << endl;
						}
					}
					// epilogue
					if (offset < 2048) {
						cout << "  addi  sp, sp, " << offset << endl;
					}
					else {
						cout << "  li    t0, " << offset << endl;
						cout << "  add   sp, sp, t0" << endl;
					}
					cout << "  ret" << endl;
				}
				else if (value->kind.tag == KOOPA_RVT_STORE) {
					koopa_raw_store_t val = value->kind.data.store;
					if (val.value->kind.tag == KOOPA_RVT_INTEGER) {
						cout << "  li    a0, " << val.value->kind.data.integer.value << endl;
						if (val.dest->kind.tag == KOOPA_RVT_INTEGER) {
							cout << "  sw    a0, " << val.dest->kind.data.integer.value << endl;
						}
						else if (val.dest->kind.tag == KOOPA_RVT_BINARY || val.dest->kind.tag == KOOPA_RVT_LOAD) {
							int tmp = LocFind((int*)val.dest);
							if (tmp < 2048) {
								cout << "  sw    a0, " << tmp << "(sp)" << endl;
							}
							else {
								cout << "  li    a1, " << tmp << endl;
								cout << "  add   a1, a1, sp" << endl;
								cout << "  sw    a0, a1" << endl;
							}
						}
						else if (val.dest->kind.tag == KOOPA_RVT_ALLOC) {
							int tmp = LocFind((int*)val.dest);
							if (tmp == -1) {
								if (now < 2048) {
									cout << "  sw    a0, " << now << "(sp)" << endl;
								}
								else {
									cout << "  li    a1, " << now << endl;
									cout << "  add   a1, a1, sp" << endl;
									cout << "  sw    a0, a1" << endl;
								}
								LocInsert((int*)val.dest, now);
								now += 4;
							}
							else {
								if (tmp < 2048) {
									cout << "  sw    a0, " << tmp << "(sp)" << endl;
								}
								else {
									cout << "  li    a1, " << tmp << endl;
									cout << "  add   a1, a1, sp" << endl;
									cout << "  sw    a0, a1" << endl;
								}
							}
						}
					}
					else if (val.value->kind.tag == KOOPA_RVT_BINARY || val.value->kind.tag == KOOPA_RVT_LOAD) {
						int left = LocFind((int*)val.value);
						if (left < 2048) {
							cout << "  lw    a0, " << left << "(sp)" << endl;
						}
						else {
							cout << "  li    a0, " << left << endl;
							cout << "  add   a0, a0, sp" << endl;
							cout << "  lw    a0, a0" << endl;
						}
						if (val.dest->kind.tag == KOOPA_RVT_INTEGER) {
							cout << "  sw    a0, " << val.dest->kind.data.integer.value << endl;
						}
						else if (val.dest->kind.tag == KOOPA_RVT_BINARY || val.dest->kind.tag == KOOPA_RVT_LOAD) {
							int right = LocFind((int*)val.dest);
							if (right < 2048) {
								cout << "  sw    a0, " << right << "(sp)" << endl;
							}
							else {
								cout << "  li    a1, " << right << endl;
								cout << "  add   a1, a1, sp" << endl;
								cout << "  sw    a0, a1" << endl;
							}
						}
						else if (val.dest->kind.tag == KOOPA_RVT_ALLOC) {
							int tmp = LocFind((int*)val.dest);
							if (tmp == -1) {
								if (now < 2048) {
									cout << "  sw    a0, " << now << "(sp)" << endl;
								}
								else {
									cout << "  li    a1, " << now << endl;
									cout << "  add   a1, a1, sp" << endl;
									cout << "  sw    a0, a1" << endl;
								}
								LocInsert((int*)val.dest, now);
								now += 4;
							}
							else {
								if (tmp < 2048) {
									cout << "  sw    a0, " << tmp << "(sp)" << endl;
								}
								else {
									cout << "  li    a1, " << tmp << endl;
									cout << "  add   a1, a1, sp" << endl;
									cout << "  sw    a0, a1" << endl;
								}
							}
						}
					}
				}
				else if (value->kind.tag == KOOPA_RVT_LOAD) {
					koopa_raw_load_t val = value->kind.data.load;
					if (val.src->kind.tag == KOOPA_RVT_INTEGER) {
						cout << "  lw    a0, " << val.src->kind.data.integer.value << endl;
						if (now < 2048) {
							cout << "  sw    a0, " << now << "(sp)" << endl;
						}
						else {
							cout << "  li    a1, " << now << endl;
							cout << "  add   a1, a1, sp" << endl;
							cout << "  sw    a0, a1" << endl;
						}
					}
					else if (val.src->kind.tag == KOOPA_RVT_BINARY || val.src->kind.tag == KOOPA_RVT_LOAD || val.src->kind.tag == KOOPA_RVT_ALLOC) {
						int tmp = LocFind((int*)val.src);
						if (tmp < 2048) {
							cout << "  lw    a0, " << tmp << "(sp)" << endl;
						}
						else {
							cout << "  li    a0, " << tmp << endl;
							cout << "  add   a0, a0, sp" << endl;
							cout << "  lw    a0, a0" << endl;
						}
						if (now < 2048) {
							cout << "  sw    a0, " << now << "(sp)" << endl;
						}
						else {
							cout << "  li    a1, " << now << endl;
							cout << "  add   a1, a1, sp" << endl;
							cout << "  sw    a0, a1" << endl;
						}
					}
					LocInsert((int*)value, now);
					now += 4;
				}
			}
		}
	}
	// �������, �ͷ� raw program builder ռ�õ��ڴ�
	// ע��, raw program �����е�ָ��ָ����ڴ��Ϊ raw program builder ���ڴ�
	// ���Բ�Ҫ�� raw program �������֮ǰ�ͷ� builder
	koopa_delete_raw_program_builder(builder);

}
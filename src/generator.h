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
	// ����Ҳʹ������ṹ��val���ص��Ǻ�����ʼ��ƫ������������֮�����ʱ�洢����8���ı�����
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
	Debug();
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

	// ����raw program
	// ʹ��forѭ�����������б�
	for (size_t i = 0; i < raw.funcs.len; ++i) {
		int now = 0;
		int ra_store = -1; // ����ֵ�洢
		// ���������, �б��е�Ԫ�ؾ��Ǻ���, ����ֻ��������ȷ�������ʵ
	   // ��Ȼ, ��Ҳ���Ի��� raw slice �� kind, ʵ��һ��ͨ�õĴ�����
		assert(raw.funcs.kind == KOOPA_RSIK_FUNCTION);
		// ��ȡ��ǰ����
		koopa_raw_function_t func = (koopa_raw_function_t)raw.funcs.buffer[i];
		cout << "  .text" << endl;
		cout << "  .global " << func->name + 1 << endl;
		cout << func->name + 1 << ":" << endl;
		// ���������������ƫ����
		int offset = 0, maxarg = 0;
		bool if_call = 0;
		for (size_t j = 0; j < func->bbs.len; ++j) {
			assert(func->bbs.kind == KOOPA_RSIK_BASIC_BLOCK);
			koopa_raw_basic_block_t bb = (koopa_raw_basic_block_t)func->bbs.buffer[j];
			for (size_t k = 0; k < bb->insts.len; ++k) {
				koopa_raw_value_t value = (koopa_raw_value_t)bb->insts.buffer[k];
				if (value->ty->tag != KOOPA_RTT_UNIT) offset += 4;
				if (value->kind.tag == KOOPA_RVT_CALL) {
					if_call = 1;
					koopa_raw_call_t val = value->kind.data.call;
					maxarg = maxarg > val.args.len + 1 ? maxarg : (val.args.len + 1);
				}
			}
		}
		offset += max(maxarg - 8, 0) * 4;
		if (if_call) offset += 4;
		if (offset % 16 != 0) offset = offset / 16 * 16 + 16; //����
		if (offset != 0) {
			if (offset > 2048) {
				cout << "  li    t0, " << -offset << endl;
				cout << "  add   sp, sp, t0" << endl;
			}
			else {
				cout << "  addi  sp, sp, " << -offset << endl;
			}
		}
		LocInsert((int*)func, offset);
		if (func->params.len > 8) {
			for (size_t j = func->params.len - 1; j >= 8; j--) { // �����浽ջ֡��
				LocInsert((int*)func->params.buffer[j], now);
				now += 4;
			}
			for (size_t j = 0; j < 8; j++) {
				cout << "  sw    a" << j << ", " << now << "(sp)" << endl;
				LocInsert((int*)func->params.buffer[j], now);
				now += 4;
			}
		}
		else {
			for (size_t j = 0; j < func->params.len; j++) {
				cout << "  sw    a" << j << ", " << now << "(sp)" << endl;
				LocInsert((int*)func->params.buffer[j], now);
				now += 4;
			}
		}
		// ��һ������ǰ����
		for (size_t j = 0; j < func->bbs.len; ++j) {
			assert(func->bbs.kind == KOOPA_RSIK_BASIC_BLOCK);
			koopa_raw_basic_block_t bb = (koopa_raw_basic_block_t)func->bbs.buffer[j];
			if (strcmp(bb->name + 1, "entry") != 0)cout << bb->name + 1 << ":" << endl;
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
					else if (val.lhs->kind.tag == KOOPA_RVT_BINARY || val.lhs->kind.tag == KOOPA_RVT_LOAD || val.lhs->kind.tag == KOOPA_RVT_ALLOC || val.lhs->kind.tag == KOOPA_RVT_FUNC_ARG_REF) {
						int loc = LocFind((int*)val.lhs);
						assert(loc >= 0);
						if (loc < 2048) {
							cout << "  lw    t0, " << loc << "(sp)" << endl;
						}
						else {
							cout << "  li    t1, " << loc << endl;
							cout << "  add   t1, t1, sp" << endl;
							cout << "  lw    t0, t1" << endl;
						}
						l[0] = 't', l[1] = '0';
					}
					else if (val.lhs->kind.tag == KOOPA_RVT_CALL) {
						l[0] = 'a', l[1] = '0';
					}
					if (val.rhs->kind.tag == KOOPA_RVT_INTEGER && val.rhs->kind.data.integer.value == 0) {
						r[0] = 'x', r[1] = '0';
					}
					else if (val.rhs->kind.tag == KOOPA_RVT_INTEGER) {
						r[0] = 't', r[1] = '1';
						cout << "  li    " << r << ", " << val.rhs->kind.data.integer.value << endl;
					}
					else if (val.rhs->kind.tag == KOOPA_RVT_BINARY || val.rhs->kind.tag == KOOPA_RVT_LOAD || val.rhs->kind.tag == KOOPA_RVT_ALLOC || val.rhs->kind.tag == KOOPA_RVT_FUNC_ARG_REF) {
						int loc = LocFind((int*)val.rhs);
						assert(loc >= 0);
						if (loc < 2048) {
							cout << "  lw    t1, " << loc << "(sp)" << endl;
						}
						else {
							cout << "  li    t2, " << loc << endl;
							cout << "  add   t2, t2, sp" << endl;
							cout << "  lw    t1, t2" << endl;
						}
						r[0] = 't', r[1] = '1';
					}
					else if (val.rhs->kind.tag == KOOPA_RVT_CALL) {
						r[0] = 'a', r[1] = '0';
					}
					if (val.op == KOOPA_RBO_EQ) {
						cout << "  xor   t0" << ", " << l << ", " << r << endl;
						cout << "  seqz  t0, t0" << endl;
						if (now < 2048) {
							cout << "  sw    t0, " << now << "(sp)" << endl;
						}
						else {
							cout << "  li    t2, " << now << endl;
							cout << "  add   t2, t2, sp" << endl;
							cout << "  sw    t0, t2" << endl;
						}
					}
					else if (val.op == KOOPA_RBO_SUB) {
						//cout << "  sub   " << getReg(now) << ", " << l << ", " << r << endl;
						cout << "  sub   t0, " << l << ", " << r << endl;
						if (now < 2048) {
							cout << "  sw    t0, " << now << "(sp)" << endl;
						}
						else {
							cout << "  li    t2, " << now << endl;
							cout << "  add   t2, t2, sp" << endl;
							cout << "  sw    t0, t2" << endl;
						}
					}
					else if (val.op == KOOPA_RBO_ADD) {
						//cout << "  add   " << getReg(now) << ", " << l << ", " << r << endl;
						cout << "  add   t0, " << l << ", " << r << endl;
						if (now < 2048) {
							cout << "  sw    t0, " << now << "(sp)" << endl;
						}
						else {
							cout << "  li    t2, " << now << endl;
							cout << "  add   t2, t2, sp" << endl;
							cout << "  sw    t0, t2" << endl;
						}
					}
					else if (val.op == KOOPA_RBO_MUL) {
						//cout << "  mul   " << getReg(now) << ", " << l << ", " << r << endl;
						cout << "  mul   t0, " << l << ", " << r << endl;
						if (now < 2048) {
							cout << "  sw    t0, " << now << "(sp)" << endl;
						}
						else {
							cout << "  li    t2, " << now << endl;
							cout << "  add   t2, t2, sp" << endl;
							cout << "  sw    t0, t2" << endl;
						}
					}
					else if (val.op == KOOPA_RBO_DIV) {
						//cout << "  div   " << getReg(now) << ", " << l << ", " << r << endl;
						cout << "  div   t0, " << l << ", " << r << endl;
						if (now < 2048) {
							cout << "  sw    t0, " << now << "(sp)" << endl;
						}
						else {
							cout << "  li    t2, " << now << endl;
							cout << "  add   t2, t2, sp" << endl;
							cout << "  sw    t0, t2" << endl;
						}
					}
					else if (val.op == KOOPA_RBO_MOD) {
						//cout << "  rem   " << getReg(now) << ", " << l << ", " << r << endl;
						cout << "  rem   t0, " << l << ", " << r << endl;
						if (now < 2048) {
							cout << "  sw    t0, " << now << "(sp)" << endl;
						}
						else {
							cout << "  li    t2, " << now << endl;
							cout << "  add   t2, t2, sp" << endl;
							cout << "  sw    t0, t2" << endl;
						}
					}
					else if (val.op == KOOPA_RBO_NOT_EQ) {
						cout << "  xor   t0, " << l << ", " << r << endl;
						cout << "  snez  t0, t0" << endl;
						if (now < 2048) {
							cout << "  sw    t0, " << now << "(sp)" << endl;
						}
						else {
							cout << "  li    t2, " << now << endl;
							cout << "  add   t2, t2, sp" << endl;
							cout << "  sw    t0, t2" << endl;
						}
					}
					else if (val.op == KOOPA_RBO_GT) {
						cout << "  sgt   t0, " << l << ", " << r << endl;
						if (now < 2048) {
							cout << "  sw    t0, " << now << "(sp)" << endl;
						}
						else {
							cout << "  li    t2, " << now << endl;
							cout << "  add   t2, t2, sp" << endl;
							cout << "  sw    t0, t2" << endl;
						}
					}
					else if (val.op == KOOPA_RBO_LT) {
						cout << "  slt   t0, " << l << ", " << r << endl;
						if (now < 2048) {
							cout << "  sw    t0, " << now << "(sp)" << endl;
						}
						else {
							cout << "  li    t2, " << now << endl;
							cout << "  add   t2, t2, sp" << endl;
							cout << "  sw    t0, t2" << endl;
						}
					}
					else if (val.op == KOOPA_RBO_GE) {
						cout << "  slt   t0, " << l << ", " << r << endl;
						cout << "  seqz  t0, t0" << endl;
						if (now < 2048) {
							cout << "  sw    t0, " << now << "(sp)" << endl;
						}
						else {
							cout << "  li    t2, " << now << endl;
							cout << "  add   t2, t2, sp" << endl;
							cout << "  sw    t0, t2" << endl;
						}
					}
					else if (val.op == KOOPA_RBO_LE) {
						cout << "  sgt   t0, " << l << ", " << r << endl;
						cout << "  seqz  t0, t0" << endl;
						if (now < 2048) {
							cout << "  sw    t0, " << now << "(sp)" << endl;
						}
						else {
							cout << "  li    t2, " << now << endl;
							cout << "  add   t2, t2, sp" << endl;
							cout << "  sw    t0, t2" << endl;
						}
					}
					else if (val.op == KOOPA_RBO_AND) {
						cout << "  and   t0, " << l << ", " << r << endl;
						if (now < 2048) {
							cout << "  sw    t0, " << now << "(sp)" << endl;
						}
						else {
							cout << "  li    t2, " << now << endl;
							cout << "  add   t2, t2, sp" << endl;
							cout << "  sw    t0, t2" << endl;
						}
					}
					else if (val.op == KOOPA_RBO_OR) {
						cout << "  or    t0, " << l << ", " << r << endl;
						if (now < 2048) {
							cout << "  sw    t0, " << now << "(sp)" << endl;
						}
						else {
							cout << "  li    t2, " << now << endl;
							cout << "  add   t2, t2, sp" << endl;
							cout << "  sw    t0, t2" << endl;
						}
					}
					LocInsert((int*)value, now);
					now += 4;
				}
				else if (value->kind.tag == KOOPA_RVT_RETURN) {
					koopa_raw_value_t ret_value = value->kind.data.ret.value;
					if (ret_value == NULL) {
						/* ʲô�������� */
					}
					else if (ret_value->kind.tag == KOOPA_RVT_INTEGER) {
						cout << "  li    " << "a0, " << ret_value->kind.data.integer.value << endl;
					}
					else if (ret_value->kind.tag == KOOPA_RVT_BINARY || ret_value->kind.tag == KOOPA_RVT_LOAD || ret_value->kind.tag == KOOPA_RVT_ALLOC || ret_value->kind.tag == KOOPA_RVT_FUNC_ARG_REF || ret_value->kind.tag == KOOPA_RVT_CALL) {
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
					if (ra_store >= 0) {
						if (ra_store < 2048) {
							cout << "  lw    ra, " << ra_store << "(sp)" << endl;
						}
						else {
							cout << "  li    t0, " << ra_store << endl;
							cout << "  add   t0, t0, sp" << endl;
							cout << "  lw    ra, t0" << endl;
						}
					}
					if (offset != 0) {
						if (offset < 2048) {
							cout << "  addi  sp, sp, " << offset << endl;
						}
						else {
							cout << "  li    t0, " << offset << endl;
							cout << "  add   sp, sp, t0" << endl;
						}
					}
					cout << "  ret" << endl;
				}
				else if (value->kind.tag == KOOPA_RVT_STORE) {
					koopa_raw_store_t val = value->kind.data.store;
					if (val.value->kind.tag == KOOPA_RVT_INTEGER) {
						cout << "  li    t0, " << val.value->kind.data.integer.value << endl;
						if (val.dest->kind.tag == KOOPA_RVT_INTEGER) {
							cout << "  sw    t0, " << val.dest->kind.data.integer.value << endl;
						}
						else if (val.dest->kind.tag == KOOPA_RVT_BINARY || val.dest->kind.tag == KOOPA_RVT_LOAD || val.dest->kind.tag == KOOPA_RVT_FUNC_ARG_REF || val.dest->kind.tag == KOOPA_RVT_CALL) {
							int tmp = LocFind((int*)val.dest);
							if (tmp < 2048) {
								cout << "  sw    t0, " << tmp << "(sp)" << endl;
							}
							else {
								cout << "  li    t1, " << tmp << endl;
								cout << "  add   t1, t1, sp" << endl;
								cout << "  sw    t0, t1" << endl;
							}
						}
						else if (val.dest->kind.tag == KOOPA_RVT_ALLOC) {
							int tmp = LocFind((int*)val.dest);
							if (tmp == -1) {
								if (now < 2048) {
									cout << "  sw    t0, " << now << "(sp)" << endl;
								}
								else {
									cout << "  li    t1, " << now << endl;
									cout << "  add   t1, t1, sp" << endl;
									cout << "  sw    t0, t1" << endl;
								}
								LocInsert((int*)val.dest, now);
								now += 4;
							}
							else {
								if (tmp < 2048) {
									cout << "  sw    t0, " << tmp << "(sp)" << endl;
								}
								else {
									cout << "  li    t1, " << tmp << endl;
									cout << "  add   t1, t1, sp" << endl;
									cout << "  sw    t0, t1" << endl;
								}
							}
						}
					}
					else if (val.value->kind.tag == KOOPA_RVT_BINARY || val.value->kind.tag == KOOPA_RVT_LOAD || val.value->kind.tag == KOOPA_RVT_ALLOC || val.value->kind.tag == KOOPA_RVT_FUNC_ARG_REF || val.value->kind.tag == KOOPA_RVT_CALL) {
						int left = LocFind((int*)val.value);
						assert(left != -1); // ALLOC��δ��ֵ
						if (left < 2048) {
							cout << "  lw    t0, " << left << "(sp)" << endl;
						}
						else {
							cout << "  li    t0, " << left << endl;
							cout << "  add   t0, t0, sp" << endl;
							cout << "  lw    t0, t0" << endl;
						}
						if (val.dest->kind.tag == KOOPA_RVT_INTEGER) {
							cout << "  sw    t0, " << val.dest->kind.data.integer.value << endl;
						}
						else if (val.dest->kind.tag == KOOPA_RVT_BINARY || val.dest->kind.tag == KOOPA_RVT_LOAD || val.dest->kind.tag == KOOPA_RVT_FUNC_ARG_REF || val.dest->kind.tag == KOOPA_RVT_CALL) {
							int right = LocFind((int*)val.dest);
							if (right < 2048) {
								cout << "  sw    t0, " << right << "(sp)" << endl;
							}
							else {
								cout << "  li    t1, " << right << endl;
								cout << "  add   t1, t1, sp" << endl;
								cout << "  sw    t0, t1" << endl;
							}
						}
						else if (val.dest->kind.tag == KOOPA_RVT_ALLOC) {
							int tmp = LocFind((int*)val.dest);
							if (tmp == -1) {
								if (now < 2048) {
									cout << "  sw    t0, " << now << "(sp)" << endl;
								}
								else {
									cout << "  li    t1, " << now << endl;
									cout << "  add   t1, t1, sp" << endl;
									cout << "  sw    t0, t1" << endl;
								}
								LocInsert((int*)val.dest, now);
								now += 4;
							}
							else {
								if (tmp < 2048) {
									cout << "  sw    t0, " << tmp << "(sp)" << endl;
								}
								else {
									cout << "  li    t1, " << tmp << endl;
									cout << "  add   t1, t1, sp" << endl;
									cout << "  sw    t0, t1" << endl;
								}
							}
						}
					}
				}
				else if (value->kind.tag == KOOPA_RVT_LOAD) {
					koopa_raw_load_t val = value->kind.data.load;
					if (val.src->kind.tag == KOOPA_RVT_INTEGER) {
						cout << "  lw    t0, " << val.src->kind.data.integer.value << endl;
						if (now < 2048) {
							cout << "  sw    t0, " << now << "(sp)" << endl;
						}
						else {
							cout << "  li    t1, " << now << endl;
							cout << "  add   t1, t1, sp" << endl;
							cout << "  sw    t0, t1" << endl;
						}
					}
					else if (val.src->kind.tag == KOOPA_RVT_BINARY || val.src->kind.tag == KOOPA_RVT_LOAD || val.src->kind.tag == KOOPA_RVT_ALLOC || val.src->kind.tag == KOOPA_RVT_FUNC_ARG_REF || val.src->kind.tag == KOOPA_RVT_CALL) {
						int tmp = LocFind((int*)val.src);
						if (tmp < 2048) {
							cout << "  lw    t0, " << tmp << "(sp)" << endl;
						}
						else {
							cout << "  li    t0, " << tmp << endl;
							cout << "  add   t0, t0, sp" << endl;
							cout << "  lw    t0, t0" << endl;
						}
						if (now < 2048) {
							cout << "  sw    t0, " << now << "(sp)" << endl;
						}
						else {
							cout << "  li    t1, " << now << endl;
							cout << "  add   t1, t1, sp" << endl;
							cout << "  sw    t0, t1" << endl;
						}
					}
					LocInsert((int*)value, now);
					now += 4;
				}
				else if (value->kind.tag == KOOPA_RVT_BRANCH) {
					koopa_raw_branch_t val = value->kind.data.branch;
					if (val.cond->kind.tag == KOOPA_RVT_INTEGER) {
						cout << "  li    t0, " << val.cond->kind.data.integer.value << endl;
						cout << "  bnez  t0, " << val.true_bb->name + 1 << endl;
					}
					else {
						int tmp = LocFind((int*)val.cond);
						if (tmp < 2048) {
							cout << "  lw    t0, " << tmp << "(sp)" << endl;
						}
						else {
							cout << "  li    t0, " << tmp << endl;
							cout << "  add   t0, t0, sp" << endl;
							cout << "  lw    t0, t0" << endl;
						}
						cout << "  bnez  t0, " << val.true_bb->name + 1 << endl;
					}
					cout << "  j     " << val.false_bb->name + 1 << endl;
				}
				else if (value->kind.tag == KOOPA_RVT_JUMP) {
					koopa_raw_jump_t val = value->kind.data.jump;
					cout << "  j     " << val.target->name + 1 << endl;
				}
				else if (value->kind.tag == KOOPA_RVT_CALL) {
					if (ra_store < 0) {
						ra_store = now;
						if (now < 2048) {
							cout << "  sw    ra, " << now << "(sp)" << endl;
						}
						else {
							cout << "  li    t0, " << now << endl;
							cout << "  add   t0, t0, sp" << endl;
							cout << "  sw    ra, t0" << endl;
						}
						now += 4;
					}
					koopa_raw_call_t val = value->kind.data.call;
					for (int x = 0; x < val.args.len; ++x) {  // ����
						koopa_raw_value_t tmp = (koopa_raw_value_t)val.args.buffer[x];
						if (x < 8) {
							if (tmp->kind.tag == KOOPA_RVT_INTEGER) {
								cout << "  li    a" << x << ", " << tmp->kind.data.integer.value << endl;
							}
							else {
								int loc = LocFind((int*)tmp);
								if (loc < 2048) {
									cout << "  lw    a" << x << ", " << loc << "(sp)" << endl;
								}
								else {
									cout << "  li    t0, " << loc << endl;
									cout << "  add   t0, t0, sp" << endl;
									cout << "  lw    a" << x << ", " << "t0" << endl;
								}
							}
						}
						else {
							int off = LocFind((int*)val.callee); // �ҵ�ƫ����
							off = -off;
							if (tmp->kind.tag == KOOPA_RVT_INTEGER) {
								cout << "  li    t0, " << tmp->kind.data.integer.value << endl;
								if (off < -2048) {
									cout << "  li    t1, " << off << endl;
									cout << "  add   t1, t1, sp" << endl;
									cout << "  sw    t0, t1" << endl;
								}
								else {
									cout << "  sw    t0, " << off << "(sp)" << endl;
								}
							}
							else {
								int loc = LocFind((int*)tmp);
								if (loc < 2048) {
									cout << "  lw    t0, " << loc << "(sp)" << endl;
								}
								else {
									cout << "  li    t0, " << loc << endl;
									cout << "  add   t0, t0, sp" << endl;
									cout << "  lw    t0, t0" << endl;
								}
								if (off < -2048) {
									cout << "  li    t1, " << loc << endl;
									cout << "  add   t1, t1, sp" << endl;
									cout << "  sw    t0, t1" << endl;
								}
								else {
									cout << "  sw    t0, " << off << "(sp)" << endl;
								}
							}
							off += 4;
						}
					}
					cout << "  call  " << val.callee->name + 1 << endl;
					if (val.callee->ty->data.function.ret->tag != KOOPA_RTT_UNIT) {
						if (now < 2048) {
							cout << "  sw    a0, " << now << "(sp)" << endl;
						}
						else {
							cout << "  li    t0, " << now << endl;
							cout << "  add   t0, t0, sp" << endl;
							cout << "  sw    a0, t0" << endl;
						}
						LocInsert((int*)value, now);
					}
					now += 4;
				}
			}
			cout << endl;
		}
	}
	// �������, �ͷ� raw program builder ռ�õ��ڴ�
	// ע��, raw program �����е�ָ��ָ����ڴ��Ϊ raw program builder ���ڴ�
	// ���Բ�Ҫ�� raw program �������֮ǰ�ͷ� builder
	koopa_delete_raw_program_builder(builder);

}
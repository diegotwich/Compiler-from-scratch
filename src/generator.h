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
	// 函数也使用这个结构，val返回的是函数开始的偏移量（用于在之后调用时存储多余8个的变量）
	int val;
	int* address;
}InstrLoc;

struct InstrLocList {
	InstrLoc loc;
	InstrLocList* next;
};

InstrLocList* List; 

/*
InstrLoc Reg[15]; // 前8个为a，后7个为t
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

int FindAllocMemory(koopa_raw_type_t now) {
	if (now->tag == KOOPA_RTT_ARRAY) {
		return now->data.array.len * FindAllocMemory(now->data.array.base);
	}
	else if (now->tag == KOOPA_RTT_INT32) {
		return 4;
	}
	return 0;
}

void init_array(koopa_raw_aggregate_t nowarray) {
	int len = nowarray.elems.len;
	for (size_t j = 0; j < len; ++j) {
		koopa_raw_value_t tt = (koopa_raw_value_t)nowarray.elems.buffer[j];
		if (tt->kind.tag == KOOPA_RVT_AGGREGATE) {
			koopa_raw_aggregate_t nextarray = tt->kind.data.aggregate;
			init_array(nextarray);
		}
		else if (tt->kind.tag == KOOPA_RVT_INTEGER) {
			if (tt->kind.data.integer.value == 0) {
				cout << "  .zero 4" << endl;
			}
			else {
				cout << "  .word " << tt->kind.data.integer.value << endl;
			}
		}
	}
	return;
}

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

	// 处理raw program
	// 循环全局变量
	if (raw.values.len > 0) {
		cout << "  .data" << endl;
		for (size_t i = 0; i < raw.values.len; ++i) {
			koopa_raw_value_t val = (koopa_raw_value_t)raw.values.buffer[i];
			if (val->kind.tag == KOOPA_RVT_GLOBAL_ALLOC) {
				cout << "  .global " << val->name + 1 << endl;
				cout << val->name + 1 << ":" << endl;
				koopa_raw_global_alloc_t initval = val->kind.data.global_alloc;
				if (initval.init->kind.tag == KOOPA_RVT_INTEGER) {
					if (initval.init->kind.data.integer.value == 0) {
						cout << "  .zero 4" << endl;
					}
					else {
						cout << "  .word " << initval.init->kind.data.integer.value << endl;
					}
				}
				else if (initval.init->kind.tag == KOOPA_RVT_AGGREGATE) {
					koopa_raw_aggregate_t arraytmp = initval.init->kind.data.aggregate;
					init_array(arraytmp);
				}
				else if (initval.init->kind.tag == KOOPA_RVT_ZERO_INIT) {
					int tt = FindAllocMemory(val->ty->data.pointer.base);
					cout << "  .zero " << tt << endl;
				}
				cout << endl;
			}
		}
	}
	// 使用for循环遍历函数列表
	for (size_t i = 0; i < raw.funcs.len; ++i) {
		int now = 0;
		int ra_store = -1; // 返回值存储
		// 正常情况下, 列表中的元素就是函数, 我们只不过是在确认这个事实
	   // 当然, 你也可以基于 raw slice 的 kind, 实现一个通用的处理函数
		assert(raw.funcs.kind == KOOPA_RSIK_FUNCTION);
		// 获取当前函数
		koopa_raw_function_t func = (koopa_raw_function_t)raw.funcs.buffer[i];
		if (func->bbs.len == 0) continue;
		cout << "  .text" << endl;
		cout << "  .global " << func->name + 1 << endl;
		cout << func->name + 1 << ":" << endl;
		// 遍历函数，计算出偏移量
		int offset = 0, maxarg = 0;
		bool if_call = 0;
		for (size_t j = 0; j < func->bbs.len; ++j) {
			assert(func->bbs.kind == KOOPA_RSIK_BASIC_BLOCK);
			koopa_raw_basic_block_t bb = (koopa_raw_basic_block_t)func->bbs.buffer[j];
			for (size_t k = 0; k < bb->insts.len; ++k) {
				koopa_raw_value_t value = (koopa_raw_value_t)bb->insts.buffer[k];
				offset += 4;
				if (value->kind.tag == KOOPA_RVT_CALL) {
					if_call = 1;
					koopa_raw_call_t val = value->kind.data.call;
					maxarg = maxarg > val.args.len + 1 ? maxarg : (val.args.len + 1);
				}
				else if (value->kind.tag == KOOPA_RVT_ALLOC && value->ty->tag == KOOPA_RTT_POINTER) {
					if (value->ty->data.pointer.base->tag == KOOPA_RTT_ARRAY) {
						int ttmp = FindAllocMemory(value->ty->data.pointer.base);
						offset += ttmp;
					}
				}
			}
		}
		offset += max(maxarg - 8, 0) * 4;
		if (if_call) offset += 4;
		if (offset % 16 != 0) offset = offset / 16 * 16 + 16; //对齐
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
			for (size_t k = func->params.len - 1; k >= 8; k--) { // 参数存到栈帧里
				LocInsert((int*)func->params.buffer[k], now);
				now += 4;
			}
			for (size_t k = 0; k < 8; k++) {
				cout << "  sw    a" << k << ", " << now << "(sp)" << endl;
				LocInsert((int*)func->params.buffer[k], now);
				now += 4;
			}
		}
		else {
			for (size_t k = 0; k < func->params.len; k++) {
				cout << "  sw    a" << k << ", " << now << "(sp)" << endl;
				LocInsert((int*)func->params.buffer[k], now);
				now += 4;
			}
		}
		// 进一步处理当前函数
		for (size_t j = 0; j < func->bbs.len; ++j) {
			assert(func->bbs.kind == KOOPA_RSIK_BASIC_BLOCK);
			koopa_raw_basic_block_t bb = (koopa_raw_basic_block_t)func->bbs.buffer[j];
			if (strcmp(bb->name + 1, "entry") != 0) cout << bb->name + 1 << ":" << endl;
			// 进一步处理当前基本块
			if (if_call) {
				if (ra_store < 0) {
					ra_store = now;
					if (now < 2048) {
						cout << "  sw    ra, " << now << "(sp)" << endl;
					}
					else {
						cout << "  li    t0, " << now << endl;
						cout << "  add   t0, t0, sp" << endl;
						cout << "  sw    ra, 0(t0)" << endl;
					}
					now += 4;
				}
			}
			for (size_t k = 0; k < bb->insts.len; ++k) {
				Debug();
				koopa_raw_value_t value = (koopa_raw_value_t)bb->insts.buffer[k];
				// 二元运算
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
					else if (val.lhs->kind.tag == KOOPA_RVT_BINARY || val.lhs->kind.tag == KOOPA_RVT_LOAD || val.lhs->kind.tag == KOOPA_RVT_ALLOC || val.lhs->kind.tag == KOOPA_RVT_FUNC_ARG_REF || val.lhs->kind.tag == KOOPA_RVT_CALL || val.lhs->kind.tag == KOOPA_RVT_GET_ELEM_PTR || val.lhs->kind.tag == KOOPA_RVT_GET_PTR) {
						int loc = LocFind((int*)val.lhs);
						assert(loc >= 0);
						if (loc < 2048) {
							cout << "  lw    t0, " << loc << "(sp)" << endl;
						}
						else {
							cout << "  li    t1, " << loc << endl;
							cout << "  add   t1, t1, sp" << endl;
							cout << "  lw    t0, 0(t1)" << endl;
						}
						if (val.lhs->kind.tag == KOOPA_RVT_GET_ELEM_PTR || val.lhs->kind.tag == KOOPA_RVT_GET_PTR) {
							cout << "  lw    t0, 0(t0)" << endl;
						}
						l[0] = 't', l[1] = '0';
					}
					else if (val.lhs->kind.tag == KOOPA_RVT_GLOBAL_ALLOC) {
						cout << "  la    t0, " << val.lhs->name + 1 << endl;
						cout << "  lw    t0, 0(t0)" << endl;
						l[0] = 't', l[1] = '0';
					}
					if (val.rhs->kind.tag == KOOPA_RVT_INTEGER && val.rhs->kind.data.integer.value == 0) {
						r[0] = 'x', r[1] = '0';
					}
					else if (val.rhs->kind.tag == KOOPA_RVT_INTEGER) {
						r[0] = 't', r[1] = '1';
						cout << "  li    " << r << ", " << val.rhs->kind.data.integer.value << endl;
					}
					else if (val.rhs->kind.tag == KOOPA_RVT_BINARY || val.rhs->kind.tag == KOOPA_RVT_LOAD || val.rhs->kind.tag == KOOPA_RVT_ALLOC || val.rhs->kind.tag == KOOPA_RVT_FUNC_ARG_REF || val.rhs->kind.tag == KOOPA_RVT_CALL || val.rhs->kind.tag == KOOPA_RVT_GET_ELEM_PTR || val.rhs->kind.tag == KOOPA_RVT_GET_PTR) {
						int loc = LocFind((int*)val.rhs);
						assert(loc >= 0);
						if (loc < 2048) {
							cout << "  lw    t1, " << loc << "(sp)" << endl;
						}
						else {
							cout << "  li    t2, " << loc << endl;
							cout << "  add   t2, t2, sp" << endl;
							cout << "  lw    t1, 0(t2)" << endl;
						}
						if (val.rhs->kind.tag == KOOPA_RVT_GET_ELEM_PTR || val.rhs->kind.tag == KOOPA_RVT_GET_PTR) {
							cout << "  lw    t1, 0(t1)" << endl;
						}
						r[0] = 't', r[1] = '1';
					}
					else if (val.rhs->kind.tag == KOOPA_RVT_GLOBAL_ALLOC) {
						cout << "  la    t1, " << val.rhs->name + 1 << endl;
						cout << "  lw    t1, 0(t1)" << endl;
						r[0] = 't', r[1] = '1';
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
							cout << "  sw    t0, 0(t2)" << endl;
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
							cout << "  sw    t0, 0(t2)" << endl;
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
							cout << "  sw    t0, 0(t2)" << endl;
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
							cout << "  sw    t0, 0(t2)" << endl;
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
							cout << "  sw    t0, 0(t2)" << endl;
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
							cout << "  sw    t0, 0(t2)" << endl;
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
							cout << "  sw    t0, 0(t2)" << endl;
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
							cout << "  sw    t0, 0(t2)" << endl;
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
							cout << "  sw    t0, 0(t2)" << endl;
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
							cout << "  sw    t0, 0(t2)" << endl;
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
							cout << "  sw    t0, 0(t2)" << endl;
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
							cout << "  sw    t0, 0(t2)" << endl;
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
							cout << "  sw    t0, 0(t2)" << endl;
						}
					}
					LocInsert((int*)value, now);
					now += 4;
				}
				else if (value->kind.tag == KOOPA_RVT_RETURN) {
					koopa_raw_value_t ret_value = value->kind.data.ret.value;
					if (ret_value == NULL) {
						/* 什么都不返回 */
					}
					else if (ret_value->kind.tag == KOOPA_RVT_INTEGER) {
						cout << "  li    " << "a0, " << ret_value->kind.data.integer.value << endl;
					}
					else if (ret_value->kind.tag == KOOPA_RVT_BINARY || ret_value->kind.tag == KOOPA_RVT_LOAD || ret_value->kind.tag == KOOPA_RVT_ALLOC || ret_value->kind.tag == KOOPA_RVT_FUNC_ARG_REF || ret_value->kind.tag == KOOPA_RVT_CALL || ret_value->kind.tag == KOOPA_RVT_GET_ELEM_PTR || ret_value->kind.tag == KOOPA_RVT_GET_PTR) {
						int tmp = LocFind((int*)ret_value);
						if (tmp < 2048) {
							cout << "  lw    a0, " << tmp << "(sp)" << endl;
						}
						else {
							cout << "  li    a0, " << tmp << endl;
							cout << "  add   a0, a0, sp" << endl;
							cout << "  lw    a0, 0(a0)" << endl;
						}
					}
					else if (ret_value->kind.tag == KOOPA_RVT_GLOBAL_ALLOC) {
						cout << "  la    a0, " << ret_value->name + 1 << endl;
						cout << "  lw    a0, 0(a0)" << endl;
					}
					// epilogue
					if (ra_store >= 0) {
						if (ra_store < 2048) {
							cout << "  lw    ra, " << ra_store << "(sp)" << endl;
						}
						else {
							cout << "  li    t0, " << ra_store << endl;
							cout << "  add   t0, t0, sp" << endl;
							cout << "  lw    ra, 0(t0)" << endl;
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
								cout << "  sw    t0, 0(t1)" << endl;
							}
						}
						else if (val.dest->kind.tag == KOOPA_RVT_GET_ELEM_PTR || val.dest->kind.tag == KOOPA_RVT_GET_PTR) {
							int tmp = LocFind((int*)val.dest);
							if (tmp < 2048) {
								cout << "  lw    t1, " << tmp << "(sp)" << endl;
							}
							else {
								cout << "  li    t1, " << tmp << endl;
								cout << "  add   t1, t1, sp" << endl;
								cout << "  lw    t1, 0(t1)" << endl;
							}
							cout << "  sw    t0, 0(t1)" << endl;
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
									cout << "  sw    t0, 0(t1)" << endl;
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
									cout << "  sw    t0, 0(t1)" << endl;
								}
							}
						}
						else if (val.dest->kind.tag == KOOPA_RVT_GLOBAL_ALLOC) {
							cout << "  la    t1, " << val.dest->name + 1 << endl;
							cout << "  sw    t0, 0(t1)" << endl;
						}
					}
					else if (val.value->kind.tag == KOOPA_RVT_BINARY || val.value->kind.tag == KOOPA_RVT_LOAD || val.value->kind.tag == KOOPA_RVT_ALLOC || val.value->kind.tag == KOOPA_RVT_FUNC_ARG_REF || val.value->kind.tag == KOOPA_RVT_CALL || val.value->kind.tag == KOOPA_RVT_GET_ELEM_PTR || val.value->kind.tag == KOOPA_RVT_GET_PTR) {
						int left = LocFind((int*)val.value);
						assert(left != -1); // ALLOC还未赋值
						if (left < 2048) {
							cout << "  lw    t0, " << left << "(sp)" << endl;
						}
						else {
							cout << "  li    t0, " << left << endl;
							cout << "  add   t0, t0, sp" << endl;
							cout << "  lw    t0, 0(t0)" << endl;
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
								cout << "  sw    t0, 0(t1)" << endl;
							}
						}
						else if (val.dest->kind.tag == KOOPA_RVT_GET_ELEM_PTR || val.dest->kind.tag == KOOPA_RVT_GET_PTR) {
							int tmp = LocFind((int*)val.dest);
							if (tmp < 2048) {
								cout << "  lw    t1, " << tmp << "(sp)" << endl;
							}
							else {
								cout << "  li    t1, " << tmp << endl;
								cout << "  add   t1, t1, sp" << endl;
								cout << "  lw    t1, 0(t1)" << endl;
							}
							cout << "  sw    t0, 0(t1)" << endl;
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
									cout << "  sw    t0, 0(t1)" << endl;
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
									cout << "  sw    t0, 0(t1)" << endl;
								}
							}
						}
						else if (val.dest->kind.tag == KOOPA_RVT_GLOBAL_ALLOC) {
							cout << "  la    t1, " << val.dest->name + 1 << endl;
							cout << "  sw    t0, 0(t1)" << endl;
						}
					}
					else if (val.value->kind.tag == KOOPA_RVT_GLOBAL_ALLOC) {
						cout << "  la    t0, " << val.value->name + 1 << endl;
						cout << "  lw    t0, 0(t0)" << endl;
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
								cout << "  sw    t0, 0(t1)" << endl;
							}
						}
						else if (val.dest->kind.tag == KOOPA_RVT_GET_ELEM_PTR || val.dest->kind.tag == KOOPA_RVT_GET_PTR) {
							int tmp = LocFind((int*)val.dest);
							if (tmp < 2048) {
								cout << "  lw    t1, " << tmp << "(sp)" << endl;
							}
							else {
								cout << "  li    t1, " << tmp << endl;
								cout << "  add   t1, t1, sp" << endl;
								cout << "  lw    t1, 0(t1)" << endl;
							}
							cout << "  sw    t0, 0(t1)" << endl;
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
									cout << "  sw    t0, 0(t1)" << endl;
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
									cout << "  sw    t0, 0(t1)" << endl;
								}
							}
						}
						else if (val.dest->kind.tag == KOOPA_RVT_GLOBAL_ALLOC) {
							cout << "  la    t1, " << val.dest->name + 1 << endl;
							cout << "  sw    t0, 0(t1)" << endl;
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
							cout << "  sw    t0, 0(t1)" << endl;
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
							cout << "  lw    t0, 0(t0)" << endl;
						}
						if (now < 2048) {
							cout << "  sw    t0, " << now << "(sp)" << endl;
						}
						else {
							cout << "  li    t1, " << now << endl;
							cout << "  add   t1, t1, sp" << endl;
							cout << "  sw    t0, 0(t1)" << endl;
						}
					}
					else if (val.src->kind.tag == KOOPA_RVT_GET_ELEM_PTR || val.src->kind.tag == KOOPA_RVT_GET_PTR) {
						int tmp = LocFind((int*)val.src);
						if (tmp < 2048) {
							cout << "  lw    t0, " << tmp << "(sp)" << endl;
						}
						else {
							cout << "  li    t0, " << tmp << endl;
							cout << "  add   t0, t0, sp" << endl;
							cout << "  lw    t0, 0(t0)" << endl;
						}
						cout << "  lw    t0, 0(t0)" << endl;
						if (now < 2048) {
							cout << "  sw    t0, " << now << "(sp)" << endl;
						}
						else {
							cout << "  li    t1, " << now << endl;
							cout << "  add   t1, t1, sp" << endl;
							cout << "  sw    t0, 0(t1)" << endl;
						}
					}
					else if (val.src->kind.tag == KOOPA_RVT_GLOBAL_ALLOC) {
						cout << "  la    t0, " << val.src->name + 1 << endl;
						cout << "  lw    t0, 0(t0)" << endl;
						if (now < 2048) {
							cout << "  sw    t0, " << now << "(sp)" << endl;
						}
						else {
							cout << "  li    t1, " << now << endl;
							cout << "  add   t1, t1, sp" << endl;
							cout << "  sw    t0, 0(t1)" << endl;
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
					else if (val.cond->kind.tag != KOOPA_RVT_GLOBAL_ALLOC) {
						int tmp = LocFind((int*)val.cond);
						if (tmp < 2048) {
							cout << "  lw    t0, " << tmp << "(sp)" << endl;
						}
						else {
							cout << "  li    t0, " << tmp << endl;
							cout << "  add   t0, t0, sp" << endl;
							cout << "  lw    t0, 0(t0)" << endl;
						}
						if (val.cond->kind.tag == KOOPA_RVT_GET_ELEM_PTR || val.cond->kind.tag == KOOPA_RVT_GET_PTR) {
							cout << "  lw    t0, 0(t0)" << endl;
						}
						cout << "  bnez  t0, " << val.true_bb->name + 1 << endl;
					}
					else {
						cout << "  la    t0, " << val.cond->name + 1 << endl;
						cout << "  lw    t0, 0(t0)" << endl;
						cout << "  bnez  t0, " << val.true_bb->name + 1 << endl;
					}
					cout << "  j     " << val.false_bb->name + 1 << endl;
				}
				else if (value->kind.tag == KOOPA_RVT_JUMP) {
					koopa_raw_jump_t val = value->kind.data.jump;
					cout << "  j     " << val.target->name + 1 << endl;
				}
				else if (value->kind.tag == KOOPA_RVT_CALL) {
					koopa_raw_call_t val = value->kind.data.call;
					int off = LocFind((int*)val.callee); // 找到偏移量
					off = -off;
					for (int x = 0; x < val.args.len; ++x) {  // 传参
						koopa_raw_value_t tmp = (koopa_raw_value_t)val.args.buffer[x];
						if (x < 8) {
							if (tmp->kind.tag == KOOPA_RVT_INTEGER) {
								cout << "  li    a" << x << ", " << tmp->kind.data.integer.value << endl;
							}
							else if (tmp->kind.tag != KOOPA_RVT_GLOBAL_ALLOC) {
								int loc = LocFind((int*)tmp);
								if (loc < 2048) {
									cout << "  lw    a" << x << ", " << loc << "(sp)" << endl;
								}
								else {
									cout << "  li    t0, " << loc << endl;
									cout << "  add   t0, t0, sp" << endl;
									cout << "  lw    a" << x << ", " << "0(t0)" << endl;
								}
							}
							else {
								cout << "  la    t0, " << tmp->name + 1 << endl;
								cout << "  lw    t0, 0(t0)" << endl;
								cout << "  mv    a" << x << ", " << "t0" << endl;
							}
						}
						else {
							if (tmp->kind.tag == KOOPA_RVT_INTEGER) {
								cout << "  li    t0, " << tmp->kind.data.integer.value << endl;
								if (off < -2048) {
									cout << "  li    t1, " << off << endl;
									cout << "  add   t1, t1, sp" << endl;
									cout << "  sw    t0, 0(t1)" << endl;
								}
								else {
									cout << "  sw    t0, " << off << "(sp)" << endl;
								}
							}
							else if (tmp->kind.tag != KOOPA_RVT_GLOBAL_ALLOC) {
								int loc = LocFind((int*)tmp);
								if (loc < 2048) {
									cout << "  lw    t0, " << loc << "(sp)" << endl;
								}
								else {
									cout << "  li    t0, " << loc << endl;
									cout << "  add   t0, t0, sp" << endl;
									cout << "  lw    t0, 0(t0)" << endl;
								}
								if (off < -2048) {
									cout << "  li    t1, " << off << endl;
									cout << "  add   t1, t1, sp" << endl;
									cout << "  sw    t0, 0(t1)" << endl;
								}
								else {
									cout << "  sw    t0, " << off << "(sp)" << endl;
								}
							}
							else {
								cout << "  la    t0, " << tmp->name + 1 << endl;
								cout << "  lw    t0, 0(t0)" << endl;
								if (off < -2048) {
									cout << "  li    t1, " << off << endl;
									cout << "  add   t1, t1, sp" << endl;
									cout << "  sw    t0, 0(t1)" << endl;
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
							cout << "  sw    a0, 0(t0)" << endl;
						}
						LocInsert((int*)value, now);
						now += 4;
					}
				}
				else if (value->kind.tag == KOOPA_RVT_GET_ELEM_PTR) {
					koopa_raw_get_elem_ptr_t val = value->kind.data.get_elem_ptr;
					if(val.src->kind.tag == KOOPA_RVT_GLOBAL_ALLOC){
						cout << "  la    t0, " << val.src->name + 1 << endl;
						if (val.index->kind.tag == KOOPA_RVT_INTEGER) {
							cout << "  li    t1, " << val.index->kind.data.integer.value << endl;
						}
						else if (val.index->kind.tag != KOOPA_RVT_GLOBAL_ALLOC) {
							int idx = LocFind((int*)val.index);
							if (idx < 2048) {
								cout << "  lw    t1, " << idx << "(sp)" << endl;
							}
							else {
								cout << "  li    t1, " << idx << endl;
								cout << "  add   t1, t1, sp" << endl;
								cout << "  lw    t1, 0(t1)" << endl;
							}
						}
						else {
							cout << "  la    t1, " << val.index->name + 1 << endl;
							cout << "  lw    t1, 0(t1)" << endl;
						}
						int multisize = FindAllocMemory(value->ty->data.pointer.base);
						cout << "  li    t2, " << multisize << endl;
						cout << "  mul   t1, t1, t2" << endl;
						cout << "  add   t0, t0, t1" << endl;
						if (now < 2048) {
							cout << "  sw    t0, " << now << "(sp)" << endl;
						}
						else {
							cout << "  li    t1, " << now << endl;
							cout << "  add   t1, t1, sp" << endl;
							cout << "  sw    t0, 0(t1)" << endl;
						}
						LocInsert((int*)value, now);
						now += 4;
					}
					else {
						int tmp = LocFind((int*)val.src);
						if (tmp < 2048) {
							cout << "  addi  t0, sp, " << tmp << endl;
						}
						else {
							cout << "  li    t0, " << tmp << endl;
							cout << "  add   t0, t0, sp" << endl;
						}
						if (val.src->kind.tag == KOOPA_RVT_GET_ELEM_PTR || val.src->kind.tag == KOOPA_RVT_GET_PTR) {
							cout << "  lw    t0, 0(t0)" << endl;
						}
						if (val.index->kind.tag == KOOPA_RVT_INTEGER) {
							cout << "  li    t1, " << val.index->kind.data.integer.value << endl;
						}
						else if(val.index->kind.tag != KOOPA_RVT_GLOBAL_ALLOC){
							int idx = LocFind((int*)val.index);
							if (idx < 2048) {
								cout << "  lw    t1, " << idx << "(sp)" << endl;
							}
							else {
								cout << "  li    t1, " << idx << endl;
								cout << "  add   t1, t1, sp" << endl;
								cout << "  lw    t1, 0(t1)" << endl;
							}
						}
						else {
							cout << "  la    t1, " << val.index->name + 1 << endl;
							cout << "  lw    t1, 0(t1)" << endl;
						}
						int multisize = FindAllocMemory(value->ty->data.pointer.base);
						cout << "  li    t2, " << multisize << endl;
						cout << "  mul   t1, t1, t2" << endl;
						cout << "  add   t0, t0, t1" << endl;
						if (now < 2048) {
							cout << "  sw    t0, " << now << "(sp)" << endl;
						}
						else {
							cout << "  li    t1, " << now << endl;
							cout << "  add   t1, t1, sp" << endl;
							cout << "  sw    t0, 0(t1)" << endl;
						}
						LocInsert((int*)value, now);
						now += 4;
					}
				}
				else if (value->kind.tag == KOOPA_RVT_GET_PTR) {
					koopa_raw_get_ptr_t val = value->kind.data.get_ptr;
					if (val.src->kind.tag == KOOPA_RVT_GLOBAL_ALLOC) {
						cout << "  la    t0, " << val.src->name + 1 << endl;
						if (val.index->kind.tag == KOOPA_RVT_INTEGER) {
							cout << "  li    t1, " << val.index->kind.data.integer.value << endl;
						}
						else if (val.index->kind.tag != KOOPA_RVT_GLOBAL_ALLOC) {
							int idx = LocFind((int*)val.index);
							if (idx < 2048) {
								cout << "  lw    t1, " << idx << "(sp)" << endl;
							}
							else {
								cout << "  li    t1, " << idx << endl;
								cout << "  add   t1, t1, sp" << endl;
								cout << "  lw    t1, 0(t1)" << endl;
							}
						}
						else {
							cout << "  la    t1, " << val.index->name + 1 << endl;
							cout << "  lw    t1, 0(t1)" << endl;
						}
						int multisize = FindAllocMemory(value->ty->data.pointer.base);
						cout << "  li    t2, " << multisize << endl;
						cout << "  mul   t1, t1, t2" << endl;
						cout << "  add   t0, t0, t1" << endl;
						if (now < 2048) {
							cout << "  sw    t0, " << now << "(sp)" << endl;
						}
						else {
							cout << "  li    t1, " << now << endl;
							cout << "  add   t1, t1, sp" << endl;
							cout << "  sw    t0, 0(t1)" << endl;
						}
						LocInsert((int*)value, now);
						now += 4;
					}
					else {
						int tmp = LocFind((int*)val.src);
						if (tmp < 2048) {
							cout << "  addi  t0, sp, " << tmp << endl;
						}
						else {
							cout << "  li    t0, " << tmp << endl;
							cout << "  add   t0, t0, sp" << endl;
						}
						cout << "  lw    t0, 0(t0)" << endl;
						if (val.index->kind.tag == KOOPA_RVT_INTEGER) {
							cout << "  li    t1, " << val.index->kind.data.integer.value << endl;
						}
						else if (val.index->kind.tag != KOOPA_RVT_GLOBAL_ALLOC) {
							int idx = LocFind((int*)val.index);
							if (idx < 2048) {
								cout << "  lw    t1, " << idx << "(sp)" << endl;
							}
							else {
								cout << "  li    t1, " << idx << endl;
								cout << "  add   t1, t1, sp" << endl;
								cout << "  lw    t1, 0(t1)" << endl;
							}
						}
						else {
							cout << "  la    t1, " << val.index->name + 1 << endl;
							cout << "  lw    t1, 0(t1)" << endl;
						}
						int multisize = FindAllocMemory(value->ty->data.pointer.base);
						cout << "  li    t2, " << multisize << endl;
						cout << "  mul   t1, t1, t2" << endl;
						cout << "  add   t0, t0, t1" << endl;
						if (now < 2048) {
							cout << "  sw    t0, " << now << "(sp)" << endl;
						}
						else {
							cout << "  li    t1, " << now << endl;
							cout << "  add   t1, t1, sp" << endl;
							cout << "  sw    t0, 0(t1)" << endl;
						}
						LocInsert((int*)value, now);
						now += 4;
					}
				}
				else if(value->kind.tag == KOOPA_RVT_ALLOC){
					LocInsert((int*)value, now);
					if (value->ty->tag == KOOPA_RTT_POINTER) { // 给数组预留空间
						int tt = FindAllocMemory(value->ty->data.pointer.base);
						now += tt;
					}
					else {
						now += 4;
					}
				}
			}
			cout << endl;
		}
	}
	// 处理完成, 释放 raw program builder 占用的内存
	// 注意, raw program 中所有的指针指向的内存均为 raw program builder 的内存
	// 所以不要在 raw program 处理完毕之前释放 builder
	koopa_delete_raw_program_builder(builder);

}
#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <cassert>

// 解决单赋值问题
static int now = 0;

// if else分支个数
static int ifnow = 0;

// while个数
static int whilenow = 0;

// 解决连续多个return问题
static int retcnt = 0;

// 用于处理多维数组的初始化
static int* arraylen;
static int nowpos;
static int* result;
static int dim;

typedef struct {
	std::string name;
	int value, status, cnt;
	// status = 0为变量，=1为常量, =-1为void函数，=-2为int函数 = 2数组, = 3指针
	// cnt表示前面有几个同名变量，最终的变量名由name和cnt拼接而成
	// dimen是数组维数
	bool init;
}Symbol;

// 记录最内层的while循环
struct whileList{
	int tag;
	whileList* next;
};

static whileList* whilelist = NULL;

// 参数的列表,在初始化数组时也有用到
struct ParamList{
	int now, type;
	ParamList* next;
};

// Dump的返回值，前面几个lv因为这个全部有大问题
struct Dumpret{
	int type;
	int value;
	ParamList* first = NULL;
};

struct SymbolList{
	Symbol sym;
	SymbolList* next;
};

struct BlockSymList {
	SymbolList* symlist;
	BlockSymList* father;
};

static BlockSymList* nowBlock = NULL;
static SymbolList* allSymbol = NULL; // 所有已使用符号的名称，用来判断是否还需要分配空间

// Calc的返回值，主要是加入函数后需要额外信息判断是否为return语句

struct Calcret {
	int type, value;
};

// 记录只会返回常值的Function（内部有if不行，返回值与变量有关也不行）

struct FuncValue {
	std::string name;
	int ret_value;
};

struct FuncList {
	FuncValue val;
	FuncList* next;
};

static FuncList* funclist = NULL;

inline FuncValue FuncListInsert(std::string name, int value) {
	if (funclist == NULL) {
		funclist = new FuncList;
		funclist->val.name = name;
		funclist->val.ret_value = value;
		funclist->next = NULL;
		return funclist->val;
	}
	else {
		FuncList* tmp = funclist;
		while (tmp->next != NULL) {
			tmp = tmp->next;
		}
		FuncList* p = new FuncList;
		p->next = NULL;
		p->val.name = name;
		p->val.ret_value = value;
		tmp->next = p;
		return p->val;
	}
}

inline FuncList* FuncListFind(std::string name) {
	FuncList* tmp = funclist;
	while (tmp != NULL) {
		if (tmp->val.name == name) {
			break;
		}
		tmp = tmp->next;
	}
	return tmp;
}

inline void whileInsert(int tag) {
	if (whilelist == NULL) {
		whilelist = new whileList;
		whilelist->next = NULL;
		whilelist->tag = tag;
	}
	else {
		whileList* tmp = whilelist;
		while (tmp->next != NULL) {
			tmp = tmp->next;
		}
		whileList* p = new whileList;
		p->next = NULL;
		p->tag = tag;
		tmp->next = p;
	}
	return;
}

inline void whileDelete(int tag) {
	assert(whilelist != NULL);
	whileList* tmp = whilelist;
	if (tmp->next == NULL) {
		delete whilelist;
		whilelist = NULL;
	}
	else {
		while (tmp->next->next != NULL) {
			tmp = tmp->next;
		}
		assert(tmp->next->tag == tag);
		delete tmp->next;
		tmp->next = NULL;
	}
	return;
}

inline void Debug() {
	/* 设置断点专用函数,debug */
	return;
}
inline void AddBlock() {
	if (nowBlock == NULL) {
		nowBlock = new BlockSymList;
		nowBlock->father = NULL;
		nowBlock->symlist = NULL;
	}
	else {
		BlockSymList* tmp = new BlockSymList;
		tmp->father = nowBlock;
		tmp->symlist = NULL;
		nowBlock = tmp;
	}
	return;
}

inline void DeleteBlock() {
	assert(nowBlock != NULL); // 尝试删除空链
	BlockSymList* tmp = nowBlock->father;
	if(nowBlock->symlist != NULL) delete nowBlock->symlist;
	delete nowBlock;
	nowBlock = tmp;
	return;
}

inline void output_alloc(int i) {
	std::cout << "[";
	if (i == dim) {
		std::cout << "i32, " << arraylen[i];
	}
	else {
		output_alloc(i + 1);
		std::cout << ", " << arraylen[i];
	}
	std::cout << "]";
	return;
}

inline void output_array(int global, int len, int pos, std::string name, int cnt) {
	if (global) {
		for (int i = 0; i < arraylen[len]; i++) {
			if (len == dim) {
				if (i) std::cout << ", " << result[pos * arraylen[len] + i];
				else std::cout << result[pos * arraylen[len] + i];
			}
			else {
				if (i) std::cout << ", {";
				else std::cout << "{";
				output_array(global, len + 1, pos * arraylen[len] + i, name, cnt);
				std::cout << "}";
			}
		}
	}
	else {
		int prenow = now;
		for (int i = 0; i < arraylen[len]; i++) {
			if (len == 1) {
				std::cout << "  %" << now << " = getelemptr %" << name << "_" << cnt << ", " << i << std::endl;
				now++;
				if (len < dim) output_array(global, len + 1, pos * arraylen[len] + i, name, cnt);
			
			}	
			else if (len != 1) {
				std::cout << "  %" << now << " = getelemptr %" << prenow - 1 << ", " << i << std::endl;
				now++;
				if (len < dim) output_array(global, len + 1, pos * arraylen[len] + i, name, cnt);
			}
			if (len == dim) {
				std::cout << "  store " << result[pos * arraylen[len] + i] << ", %" << now - 1 << std::endl;
			}	
		}
	}
	return;
}

inline SymbolList* FindSymbolValue(std::string s) {
	BlockSymList* blockt = nowBlock;
	// std::cout << "try to find : "<< s << std::endl;
	while (blockt != NULL) {
		SymbolList* tmp = blockt->symlist;
		while (tmp != NULL) {
			if (tmp->sym.name == s) {
				return tmp;
			}
			tmp = tmp->next;
		}
		blockt = blockt->father;
	}
	std::cout << s << std::endl;
	assert(blockt != NULL);
	return NULL;
}

inline Symbol InsertSymbol(std::string s, int value, int status, bool init) {
	assert(nowBlock != NULL);
	SymbolList* p = nowBlock->symlist;
	while (p != NULL) {
		assert(p->sym.name != s); //重定义
		p = p->next;
	}

	// 判断以前是否使用过了这个名字，若使用过了则加一
	int cnt = 1;
	SymbolList* atmp = allSymbol;
	while (atmp != NULL) {
		if (atmp->sym.name == s) {
			atmp->sym.cnt = atmp->sym.cnt + 1;
			cnt = atmp->sym.cnt;
			break;
		}
		atmp = atmp->next;
	}
	if (atmp == NULL) { // 没找到就加进去
		if (allSymbol == NULL) {
			allSymbol = new SymbolList;
			allSymbol->next = NULL;
			allSymbol->sym.cnt = cnt;
			allSymbol->sym.name = s;
		}
		else {
			atmp = allSymbol;
			while (atmp->next != NULL) {
				atmp = atmp->next;
			}
			SymbolList* aatmp = new SymbolList;
			aatmp->next = NULL;
			aatmp->sym.cnt = cnt;
			aatmp->sym.name = s;
			atmp->next = aatmp;
		}
	}

	p = nowBlock->symlist;
	if (nowBlock->symlist == NULL) {
		nowBlock->symlist = new SymbolList;
		nowBlock->symlist->next = NULL;
		nowBlock->symlist->sym.name = s;
		nowBlock->symlist->sym.value = value;
		nowBlock->symlist->sym.status = status;
		nowBlock->symlist->sym.init = init;
		nowBlock->symlist->sym.cnt = cnt;
		Symbol stmp = nowBlock->symlist->sym;
		return stmp;
	}
	else {
		while (p->next != NULL) p = p->next;
		SymbolList* tmp = new SymbolList;
		tmp->next = NULL;
		tmp->sym.name = s;
		tmp->sym.value = value;
		tmp->sym.status = status;
		tmp->sym.init = init;
		tmp->sym.cnt = cnt;
		p->next = tmp;
		Symbol stmp = tmp->sym;
		return stmp;
	}
}

class BaseAST {
public:
	virtual ~BaseAST() = default;

	virtual Dumpret Dump() const = 0;

	virtual Calcret Calc() const = 0;

	virtual SymbolList* FindSym() const = 0;

	virtual int Array(int start, int offset) const = 0;
};

class PreCompAST : public BaseAST {
public:
	std::unique_ptr<BaseAST> comp_unit;
	Dumpret Dump() const override {
		std::cout << "decl @getint(): i32" << std::endl;
		std::cout << "decl @getch(): i32" << std::endl;
		std::cout << "decl @getarray(*i32): i32" << std::endl;
		std::cout << "decl @putint(i32)" << std::endl;
		std::cout << "decl @putch(i32)" << std::endl;
		std::cout << "decl @putarray(i32, *i32)" << std::endl;
		std::cout << "decl @starttime()" << std::endl;
		std::cout << "decl @stoptime()" << std::endl << std::endl;
		AddBlock(); // 全局作用域
		InsertSymbol("getint", 0, -2, 1);
		InsertSymbol("getch", 0, -2, 1);
		InsertSymbol("getarray", 0, -2, 1);
		InsertSymbol("putint", 0, -1, 1);
		InsertSymbol("putch", 0, -1, 1);
		InsertSymbol("putarray", 0, -1, 1);
		InsertSymbol("starttime", 0, -1, 1);
		InsertSymbol("stoptime", 0, -1, 1);
		comp_unit->Dump();
		DeleteBlock();
		Dumpret tmp;
		tmp.type = 1;
		return tmp;
	}
	Calcret Calc() const override {
		Calcret tmp;
		tmp.type = -1;
		return tmp;
	}
	SymbolList* FindSym() const override {

		return NULL;
	}
	int Array(int start, int offset) const override{

		return -1;
	}
};

class CompUnitAST : public BaseAST {
public:
	bool exist = 1;
	std::unique_ptr<BaseAST> comp;
	std::unique_ptr<BaseAST> func_def;

	Dumpret Dump() const override {
		// std::cout << "CompUnitAST { ";
		// Debug();
		if(exist) comp->Dump();
		func_def->Dump();
		// std::cout << " }";
		Dumpret tmp;
		tmp.type = 1;
		return tmp;
	}

	Calcret Calc() const override {
		Calcret tmp;
		tmp.type = -1;
		return tmp;
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
	int Array(int start, int offset) const override {

		return -1;
	}
};

class DeclAST : public BaseAST {
public:
	std::unique_ptr<BaseAST> decl;

	Dumpret Dump() const override {
		decl->Dump();
		Dumpret tmp;
		tmp.type = 1;
		return tmp;
	}

	Calcret Calc() const override {
		Calcret tmp;
		tmp.type = -1;
		return tmp;
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
	int Array(int start, int offset) const override {

		return -1;
	}
};

class ConstDeclAST : public BaseAST {
public:
	std::unique_ptr<BaseAST> type;
	std::unique_ptr<BaseAST> const_def;
	std::unique_ptr<BaseAST> const_defs;
	Dumpret Dump() const override {
		// type->Dump();
		const_def->Dump();
		const_defs->Dump();
		Dumpret tmp;
		tmp.type = 1;
		return tmp;
	}
	Calcret Calc() const override {
		Calcret tmp;
		tmp.type = -1;
		return tmp;
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
	int Array(int start, int offset) const override {

		return -1;
	}
};

class TypeAST : public BaseAST {
public:
	std::string type;
	Dumpret Dump() const override {
		// std::cout << type;
		Dumpret tmp;
		tmp.type = 1;
		if (type == "int") {
			std::cout << ": i32 " << "{" << std::endl << "%entry:" << std::endl;
			tmp.type = -2;
		}
		else if (type == "void") {
			std::cout << " {" << std::endl << "%entry:" << std::endl;
			tmp.type = -3;
		}
		return tmp;
	}
	Calcret Calc() const override {
		Calcret tmp;
		tmp.type = -1;
		return tmp;
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
	int Array(int start, int offset) const override {

		return -1;
	}
};

class ConstDefsAST : public BaseAST {
public:
	bool exist = 1;
	std::unique_ptr<BaseAST> const_def;
	std::unique_ptr<BaseAST> const_defs;
	Dumpret Dump() const override {
		if (exist) {
			const_def->Dump();
			const_defs->Dump();
		}		
		Dumpret tmp;
		tmp.type = 1;
		return tmp;
	}
	Calcret Calc() const override {
		Calcret tmp;
		tmp.type = -1;
		return tmp;
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
	int Array(int start, int offset) const override {

		return -1;
	}
};

class ConstDefAST : public BaseAST {
public:
	bool is_array = 0;
	std::string name;
	std::unique_ptr<BaseAST> const_init_val;
	std::unique_ptr<BaseAST> const_exp;
	std::unique_ptr<BaseAST> const_exps;
	Dumpret Dump() const override {
		if (is_array == 0) {
			auto val = const_init_val->Calc().value;
			InsertSymbol(name, val, 1, 1);
		}
		else if (nowBlock->father == NULL) { // 全局变量
			Dumpret tmp = const_exps->Dump();
			dim = tmp.value + 1;
			ParamList* p = new ParamList;
			p->next = tmp.first;
			p->now = const_exp->Calc().value;
			tmp.first = p;
			arraylen = new int[dim + 5];
			int ccnt = 1;
			while (tmp.first != NULL) {
				arraylen[ccnt++] = tmp.first->now;
				tmp.first = tmp.first->next;
			}
			int size = 1;
			for (int i = 1; i <= dim; i++) {
				size *= arraylen[i];
			}
			result = new int[size + 5];;
			nowpos = 0;
			int init_num = const_init_val->Array(0, 0);
			for (; init_num < size; init_num++) {
				result[nowpos++] = 0;
			}
			Symbol sym = InsertSymbol(name, dim, 2, 1);
			std::cout << "global %" << name << "_" << sym.cnt << " = alloc ";
			output_alloc(1);
			std::cout << ", {";
			output_array(1, 1, 0, name, sym.cnt);
			std::cout << "}";
			delete[] arraylen;
			delete[] result;
			std::cout << std::endl << std::endl;
		}
		else {
			Dumpret tmp = const_exps->Dump();
			dim = tmp.value + 1;
			ParamList* p = new ParamList;
			p->next = tmp.first;
			p->now = const_exp->Calc().value;
			tmp.first = p;
			arraylen = new int[dim + 5];
			int ccnt = 1;
			while (tmp.first != NULL) {
				arraylen[ccnt++] = tmp.first->now;
				tmp.first = tmp.first->next;
			}
			int size = 1;
			for (int i = 1; i <= dim; i++) {
				size *= arraylen[i];
			}
			result = new int[size + 5];;
			nowpos = 0;
			int init_num = const_init_val->Array(0, 0);
			for (; init_num < size; init_num++) {
				result[nowpos++] = 0;
			}
			Symbol sym = InsertSymbol(name, dim, 2, 1);
			std::cout << "  %" << name << "_" << sym.cnt << " = alloc ";
			output_alloc(1);
			output_array(0, 1, 0, name, sym.cnt);
			delete[] arraylen;
			delete[] result;
			std::cout << std::endl << std::endl;
		}
		Dumpret tmp;
		tmp.type = 1;
		return tmp;
	}
	Calcret Calc() const override {
		Calcret tmp;
		tmp.type = -1;
		return tmp;
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
	int Array(int start, int offset) const override {

		return -1;
	}
};

class ConstInitValAST : public BaseAST {
public:
	int exist = 1;
	std::unique_ptr<BaseAST> const_exp;
	std::unique_ptr<BaseAST> const_exps;
	Dumpret Dump() const override {
		Dumpret tmp;
		tmp.type = 1;
		if (exist == 2) {
			int val = const_exp->Calc().value;
			Dumpret tt = const_exps->Dump();
			tmp.first = new ParamList;
			tmp.first->next = tt.first;
			tmp.first->now = val;
			tmp.type = 8;
		}
		else {
			tmp = const_exp->Dump();
			tmp.first = NULL;
		}
		return tmp;
	}
	Calcret Calc() const override {
		return const_exp->Calc();
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
	int Array(int start, int offset) const override {
		if (exist == 2) { // "{ }"
			if (offset == 0) { // 按照后dim-start个来递归、对齐
				int pos = const_exp->Array(start + 1, 0);
				int vals = const_exps->Array(start + 1, pos);
				int tmp = 1, done = pos + vals;
				for (int i = start + 1; i <= dim; i++) {
					tmp *= arraylen[i];
				}
				for (; done < tmp; done++) {
					result[nowpos++] = 0;
				}
				return tmp;
			}
			else {
				int tmp = 1;
				int rim = dim;
				for (; rim >= start; rim--) {
					if (offset % (tmp * arraylen[rim]) != 0) break;
					tmp *= arraylen[rim];
				}
				assert(tmp != 1); // 对不齐
				int pos = const_exp->Array(rim, 0);
				int vals = const_exps->Array(rim, pos);
				for (int done = pos + vals; done < tmp; done++) {
					result[nowpos++] = 0;
				}
				return tmp;
			}
		}
		else if (exist == 1) { // Exp
			result[nowpos++] = const_exp->Calc().value;
			return 1;
		}
		// NULL
		return 0;
	}
};

class ConstInitValsAST : public BaseAST {
public:
	bool exist = 0;
	std::unique_ptr<BaseAST> const_init_val;
	std::unique_ptr<BaseAST> const_init_vals;
	Dumpret Dump() const override {
		Dumpret tmp;
		tmp.type = 1;
		return tmp;
	}
	Calcret Calc() const override {
		Calcret tmp;
		tmp.type = -1;
		return tmp;
	}
	SymbolList* FindSym() const override {

		return NULL;
	}
	int Array(int start, int offset) const override {
		if (exist) {
			int pos = const_init_val->Array(start, offset);
			int vals = const_init_vals->Array(start, offset + pos);
			return pos + vals;
		}
		return 0;
	}
};
class ConstExpsAST : public BaseAST {
public:
	bool exist = 0;
	std::unique_ptr<BaseAST> const_exp;
	std::unique_ptr<BaseAST> const_exps;
	Dumpret Dump() const override {
		Dumpret tmp;
		tmp.type = 1;
		if (exist) {
			int val = const_exp->Calc().value;
			Dumpret tt = const_exps->Dump();
			tmp.first = new ParamList;
			tmp.first->next = tt.first;
			tmp.first->now = val;
			tmp.value = tt.value + 1;
			tmp.type = 8;
		}
		else {
			tmp.value = 0;
			tmp.first = NULL;
		}
		return tmp;
	}
	Calcret Calc() const override {
		Calcret tmp;
		tmp.value = 0;
		if (exist == 0) {
			std::cout << "i32";
		}
		else {
			tmp.value = 1;
			std::cout << "[";
			tmp.value += const_exps->Calc().value;
			std::cout << ", " << const_exp->Calc().value << "]";
		}
		return tmp;
	}	
	SymbolList* FindSym() const override {

		return NULL;
	}
	int Array(int start, int offset) const override {

		return -1;
	}
};

class VarDeclAST : public BaseAST {
public:
	std::unique_ptr<BaseAST> type;
	std::unique_ptr<BaseAST> var_def;
	std::unique_ptr<BaseAST> var_defs;
	Dumpret Dump() const override {
		// type->Dump();
		var_def->Dump();
		var_defs->Dump();
		Dumpret tmp;
		tmp.type = 1;
		return tmp;
	}
	Calcret Calc() const override {
		Calcret tmp;
		tmp.type = -1;
		return tmp;
	}
	SymbolList* FindSym() const override{

		return NULL;
	}	
	int Array(int start, int offset) const override {

		return -1;
	}
};

class VarDefsAST : public BaseAST {
public:
	bool exist = 1;
	std::unique_ptr<BaseAST> var_def;
	std::unique_ptr<BaseAST> var_defs;
	Dumpret Dump() const override {
		if (exist) {
			var_def->Dump();
			var_defs->Dump();
		}
		Dumpret tmp;
		tmp.type = 1;
		return tmp;
	}
	Calcret Calc() const override {
		Calcret tmp;
		tmp.type = -1;
		return tmp;
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
	int Array(int start, int offset) const override {

		return -1;
	}
};

class VarDefAST : public BaseAST {
public:
	bool initialized = 1, is_array = 0; //是否初始化,是否为数组
	std::string name;
	std::unique_ptr<BaseAST> const_exp;
	std::unique_ptr<BaseAST> const_exps;
	std::unique_ptr<BaseAST> init_val;
	Dumpret Dump() const override {
		if (nowBlock->father == NULL) { // 全局变量
			if (is_array == 0) {
				auto val = initialized ? init_val->Calc().value : 0;
				Symbol sym = InsertSymbol(name, val, 0, initialized);
				std::cout << "global %" << name << "_" << sym.cnt << " = alloc i32, " << val << std::endl << std::endl;
			}
			else {
				Dumpret tmp = const_exps->Dump(); 
				dim = tmp.value + 1;
				ParamList* p = new ParamList;
				p->next = tmp.first;
				p->now = const_exp->Calc().value;
				tmp.first = p;
				arraylen = new int[dim + 5];
				int ccnt = 1;
				while (tmp.first != NULL) {
					arraylen[ccnt++] = tmp.first->now;
					tmp.first = tmp.first->next;
				}
				int size = 1;
				for (int i = 1; i <= dim; i++) {
					size *= arraylen[i];
				}
				result = new int[size + 5];
				nowpos = 0;
				if (initialized) {
					int init_num = init_val->Array(0, 0);
					for (; init_num < size; init_num++) {
						result[nowpos++] = 0;
					}
				}
				Symbol sym = InsertSymbol(name, dim, 2, initialized);
				std::cout << "global %" << name << "_" << sym.cnt << " = alloc ";
				output_alloc(1);
				if (initialized) {
					std::cout << ", {";
					output_array(1, 1, 0, name, sym.cnt);
					std::cout << "}";
				}
				else {
					std::cout << ", zeroinit";
				}
				delete[] arraylen;
				delete[] result;
				std::cout << std::endl << std::endl;
			}
		}
		else {
			if (is_array == 0) {
				auto val = initialized ? init_val->Calc().value : 0;
				Symbol sym = InsertSymbol(name, val, 0, initialized);
				std::cout << "  %" << name << "_" << sym.cnt << " = alloc i32" << std::endl;
				if (initialized) {
					Dumpret dret = init_val->Dump();
					if (dret.type != 0) {
						std::cout << "  store %" << now - 1 << ", %" << name << "_" << sym.cnt << std::endl;
					}
					else {
						std::cout << "  store " << dret.value << ", %" << name << "_" << sym.cnt << std::endl;
					}
				}
			}
			else {
				Dumpret tmp = const_exps->Dump();
				dim = tmp.value + 1;
				ParamList* p = new ParamList;
				p->next = tmp.first;
				p->now = const_exp->Calc().value;
				tmp.first = p;
				arraylen = new int[dim + 5];
				int ccnt = 1;
				while (tmp.first != NULL) {
					arraylen[ccnt++] = tmp.first->now;
					tmp.first = tmp.first->next;
				}
				int size = 1;
				for (int i = 1; i <= dim; i++) {
					size *= arraylen[i];
				}
				result = new int[size + 5];
				nowpos = 0;
				if (initialized) {
					int init_num = init_val->Array(0, 0);
					for (; init_num < size; init_num++) {
						result[nowpos++] = 0;
					}
				}
				Symbol sym = InsertSymbol(name, dim, 2, initialized);
				std::cout << "  %" << name << "_" << sym.cnt << " = alloc ";
				output_alloc(1);
				std::cout << std::endl;
				if (initialized) {
					output_array(0, 1, 0, name, sym.cnt);
				}
				delete[] arraylen;
				delete[] result;
			}
		}
		Dumpret tmp;
		tmp.type = 1;
		return tmp;
	}
	Calcret Calc() const override {
		Calcret tmp;
		tmp.type = -1;
		return tmp;
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
	int Array(int start, int offset) const override {

		return -1;
	}
};

class InitValAST : public BaseAST {
public:
	int exist = 1;
	std::unique_ptr<BaseAST> init_val;
	std::unique_ptr<BaseAST> init_vals;
	Dumpret Dump() const override {
		Dumpret tmp;
		tmp.type = 1;
		if (exist) tmp = init_val->Dump();
		return tmp;
	}
	Calcret Calc() const override {
		return init_val->Calc();
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
	int Array(int start, int offset) const override {
		if (exist == 2) { // "{ }"
			if (offset == 0) { // 按照后dim-start个来递归、对齐
				int pos = init_val->Array(start + 1, 0);
				int vals = init_vals->Array(start + 1, pos);
				int tmp = 1, done = pos + vals;
				for (int i = start + 1; i <= dim; i++) {
					tmp *= arraylen[i];
				}
				for (; done < tmp; done++) {
					result[nowpos++] = 0;
				}
				return tmp;
			}
			else {
				int tmp = 1;
				int rim = dim;
				for (; rim >= start; rim--) {
					if (offset % (tmp * arraylen[rim]) != 0) break;
					tmp *= arraylen[rim];
				}
				assert(tmp != 1); // 对不齐
				int pos = init_val->Array(rim, 0);
				int vals = init_vals->Array(rim, pos);
				for (int done = pos + vals; done < tmp; done++) {
					result[nowpos++] = 0;
				}
				return tmp;
			}
		}
		else if (exist == 1) { // Exp
			result[nowpos++] = init_val->Calc().value;
			return 1;
		}
		// NULL
		return 0;
	}
};

class InitValsAST : public BaseAST {
public:
	bool exist = 0;
	std::unique_ptr<BaseAST> init_val;
	std::unique_ptr<BaseAST> init_vals;
	Dumpret Dump() const override {
		Dumpret tmp;
		if (exist) {
			tmp = init_val->Dump();
			init_vals->Dump();
		}
		tmp.type = 1;
		return tmp;
	}
	Calcret Calc() const override {
		Calcret tmp;
		tmp.type = -1;
		return tmp;
	}
	SymbolList* FindSym() const override {

		return NULL;
	}
	int Array(int start, int offset) const override {
		if (exist) {
			int pos = init_val->Array(start, offset);
			int vals = init_vals->Array(start, offset + pos);
			return pos + vals;
		}
		return 0;
	}
};

class ExpsAST : public BaseAST {
public:
	bool exist = 0;
	std::unique_ptr<BaseAST> exp;
	std::unique_ptr<BaseAST> exps;
	Dumpret Dump() const override {
		Dumpret tmp;
		tmp.type = 1;
		if (exist) {
			int val = exp->Calc().value;
			Dumpret tt = exps->Dump();
			tmp.first = new ParamList;
			tmp.first->next = tt.first;
			tmp.first->now = val;
			tmp.type = 8;
		}
		else {
			tmp.value = 0;
			tmp.first = NULL;
		}
		return tmp;
	}
	Calcret Calc() const override {
		Calcret tmp;
		tmp.value = 0;
		if (exist) {
			int prenow = now - 1;
			Dumpret pos = exp->Dump();
			if (pos.type == 0) {
				std::cout << "  %" << now << " = getelemptr %" << now - 1 << ", " << pos.value << std::endl;
			}
			else {
				std::cout << "  %" << now << " = getelemptr %" << prenow << ", %" << now - 1 << std::endl;
			}
			now++;
			tmp.value = exps->Calc().value + 1;
		}
		tmp.type = -1;
		return tmp;
	}
	SymbolList* FindSym() const override {

		return NULL;
	}
	int Array(int start, int offset) const override {

		return -1;
	}
};

class FuncDefAST : public BaseAST {
public:
	bool param_exist = 0;
	std::unique_ptr<BaseAST> type;
	std::string ident;
	std::unique_ptr<BaseAST> block;
	std::unique_ptr<BaseAST> fparams;

	Dumpret Dump() const override {
		// std::cout << "FuncDefAST { ";
		// type->Dump();
		// std::cout << ", " << ident << ", ";
		// block->Dump();
		// std::cout << " }";
		std::cout << "fun @" << ident << "(";
		if (param_exist) fparams->Calc();
		std::cout << ")";
		Dumpret funcret = type->Dump();
		if (funcret.type == -3) {
			InsertSymbol(ident, 0, -1, 1);
		}
		else if (funcret.type == -2) {
			InsertSymbol(ident, 0, -2, 1);
		}
		AddBlock();
		if (param_exist)fparams->Dump();
		Dumpret blkret = block->Dump();
		this->Calc();
		DeleteBlock();
		if (blkret.type != 2) {
			std::cout << "  ret" << std::endl;
		}
		std::cout << "}" << std::endl << std::endl;
		Dumpret tmp;
		tmp.type = 1;
		return tmp;
	}

	Calcret Calc() const override {
		Calcret tmp = block->Calc();
		if (tmp.type == 0) {
			FuncListInsert(ident, tmp.value);
		}
		return tmp;
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
	int Array(int start, int offset) const override {

		return -1;
	}
};

/*
由于移进规约冲突，FuncType和Btype合并
class FuncTypeAST : public BaseAST {
public:
	int status; // = 0变量类型， = 1函数类型
	std::string type;
	Dumpret Dump() const override {
		// std::cout << "FuncTypeAST { " << INT << " }";
		Dumpret tmp;
		tmp.type = 1;
		if (status == 1) {
			if (type == "int") {
				std::cout << ": i32 " << "{" << std::endl << "%entry:" << std::endl;
				tmp.type = -2;
			}
			else if (type == "void") {
				std::cout << " {" << std::endl << "%entry:" << std::endl;
				tmp.type = -3;
			}
		}
		return tmp;
	}

	int Calc() const override {
		
		return -1;
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
};

*/
class FuncFParamsAST : public BaseAST {
public:
	bool exist = 1;
	std::unique_ptr<BaseAST> fparam;
	std::unique_ptr<BaseAST> fparams;
	Dumpret Dump() const override {
		fparam->Dump();
		if (exist) fparams->Dump();
		Dumpret tmp;
		tmp.type = 1;
		return tmp;
	}
	Calcret Calc() const override {
		fparam->Calc();
		if (exist) {
			std::cout << ", ";
			fparams->Calc();
		}
		Calcret tmp;
		tmp.type = -1;
		return tmp;
	}
	SymbolList* FindSym() const override {

		return NULL;
	}
	int Array(int start, int offset) const override {

		return -1;
	}
};

class FuncFParamAST : public BaseAST {
public:
	bool is_array = 0;
	std::string ident;
	std::unique_ptr<BaseAST> type;
	std::unique_ptr<BaseAST> const_exps;
	Dumpret Dump() const override {
		if (is_array == 0) {
			Symbol sym = InsertSymbol(ident, 0, 0, 1);
			std::cout << "  %" << ident << "_" << sym.cnt << " = alloc i32" << std::endl;
			std::cout << "  store @" << ident << ", " << "%" << ident << "_" << sym.cnt << std::endl;
		}
		else {
			Symbol sym = InsertSymbol(ident, 0, 3, 1);
			std::cout << "  %" << ident << "_" << sym.cnt << " = alloc *";
			int nowdim = const_exps->Calc().value;
			SymbolList* tmp = FindSymbolValue(ident);
			tmp->sym.value = nowdim + 1; // 修改维度
			std::cout << std::endl;
			std::cout << "  store @" << ident << ", " << "%" << ident << "_" << sym.cnt << std::endl;
		}
		Dumpret tmp;
		tmp.type = 1;
		return tmp;
	}
	Calcret Calc() const override {
		if (is_array == 0) {
			std::cout << "@" << ident << ": i32";
		}
		else {
			std::cout << "@" << ident << ": *";
			const_exps->Calc();
		}
		Calcret tmp;
		tmp.type = -1;
		return tmp;
	}
	SymbolList* FindSym() const override {

		return NULL;
	}
	int Array(int start, int offset) const override {

		return -1;
	}
};

class BlockAST : public BaseAST {
public:
	std::unique_ptr<BaseAST> block_items;

	Dumpret Dump() const override {
		// std::cout << "Block { ";
		// stmt->Dump();
		// std::cout << " }";
		Dumpret tmp = block_items->Dump();
		return tmp;
	}

	Calcret Calc() const override {

		return block_items->Calc();
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
	int Array(int start, int offset) const override {

		return -1;
	}
};

class BlockItemsAST : public BaseAST {
public:
	bool exist = 1;
	std::unique_ptr<BaseAST> block_item;
	std::unique_ptr<BaseAST> block_items;

	Dumpret Dump() const override {
		Dumpret tmp1, tmp2;
		if (exist) {
			tmp1 = block_item->Dump();
			int rett = block_items->FindSym()->sym.status;
			if (tmp1.type == 2 && rett != -1) { // return后紧跟语句
				std::cout << std::endl << "%afret" << retcnt << ":" << std::endl;
				retcnt++;
			}
			else if (tmp1.type == 6 || tmp1.type == 7) {
				if (rett == 1) { // break和continue后紧跟语句
					std::cout << std::endl << "%afret" << retcnt << ":" << std::endl;
					retcnt++;
				}
			}
			tmp2 = block_items->Dump();
		}
		else {
			tmp1.type = -1;
			return tmp1;
		}
		return tmp2.type == -1 ? tmp1 : tmp2;
	}
	Calcret Calc() const override {
		Calcret tmp;
		if (exist) {
			tmp.type = 1;
			Calcret itm = block_item->Calc();
			Calcret itms = block_items->Calc();
			if (itm.type == 2 && itms.type != 2) { // 单return
				tmp = itm;
				tmp.type = 0;
			}
			else {
				tmp = itms;
				tmp.type = tmp.type > itm.type ? tmp.type : itm.type;
			}
			return tmp;
		}
		tmp.type = -1;
		return tmp;
	}
	SymbolList* FindSym() const override{
		SymbolList* tmp = new SymbolList;
		if (exist) tmp->sym.status = 1;
		else tmp->sym.status = -1;
		return tmp;
	}
	int Array(int start, int offset) const override {

		return -1;
	}
};

class BlockItemAST : public BaseAST {
public:
	std::unique_ptr<BaseAST> bi_ptr;
	Dumpret Dump() const override {
		
		return bi_ptr->Dump();
	}
	Calcret Calc() const override {
		
		return bi_ptr->Calc();
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
	int Array(int start, int offset) const override {

		return -1;
	}
};

class LValAST : public BaseAST {
public:
	bool is_array = 0;
	std::string name;
	std::unique_ptr<BaseAST> exp;
	std::unique_ptr<BaseAST> exps;
	Dumpret Dump() const override {
		SymbolList* tmp = FindSymbolValue(name);
		if (tmp->sym.status == 1) {
			Dumpret p;
			p.type = 0;
			p.value = tmp->sym.value;
			return p;
		}
		else if (tmp->sym.status == 3) { // 指针
			std::cout << "  %" << now << " = load %" << tmp->sym.name << "_" << tmp->sym.cnt << std::endl;
			int prenow = now;
			now++;
			Dumpret rtmp;
			rtmp.value = 1;
			if (is_array) {
				Dumpret pos = exp->Dump();
				if (pos.type == 0) {
					std::cout << "  %" << now << " = getptr %" << prenow << ", " << pos.value << std::endl;
				}
				else {
					std::cout << "  %" << now << " = getptr %" << prenow << ", %" << now - 1 << std::endl;
				}
				now++;
				int nowdim = exps->Calc().value + 1;
				if (nowdim == tmp->sym.value) {
					rtmp.value = 2;
					std::cout << "  %" << now << " = load %" << now - 1 << std::endl;
					now++;
				}
				else {
					std::cout << "  %" << now << " = getelemptr %" << now - 1 << ", 0" << std::endl;
					now++;
				}
			}
			else {
				std::cout << "  %" << now << " = getptr %" << now - 1 << ", 0" << std::endl;
				now++;
				rtmp.value = 2;
			}
			rtmp.type = 10;
			return rtmp;
		}
		else if (tmp->sym.status == 2 && is_array == 0) { // is_array并不是表示它是否是数组，而是表示这个LVal的表现形式是不是数组
			// 传递的是一个数组指针
			std::cout << "  %" << now << " = getelemptr %" << tmp->sym.name << "_" << tmp->sym.cnt << ", 0" << std::endl;
			now++;
			Dumpret tmp;
			tmp.type = 10;
			return tmp;
		}
		else if (is_array == 0) {
			std::cout << "  %" << now << " = load %" << tmp->sym.name << "_" << tmp->sym.cnt << std::endl;
			now++;
			Dumpret tmp;
			tmp.type = 1;
			return tmp;
		}
		else {
			Dumpret pos = exp->Dump();
			if (pos.type == 0) {
				std::cout << "  %" << now << " = getelemptr %" << tmp->sym.name << "_" << tmp->sym.cnt << ", " << pos.value << std::endl;
			}
			else {
				std::cout << "  %" << now << " = getelemptr %" << tmp->sym.name << "_" << tmp->sym.cnt << ", %" << now - 1 << std::endl;
			}
			now++;
			int nowdim = exps->Calc().value + 1;
			Dumpret rtmp;
			rtmp.value = 1;
			if (nowdim == tmp->sym.value) {
				rtmp.value = 2;
				std::cout << "  %" << now << " = load %" << now - 1 << std::endl;
				now++;
			} 
			else {
				std::cout << "  %" << now << " = getelemptr %" << now - 1 << ", 0" << std::endl;
				now++;
			}
			rtmp.type = 9;
			return rtmp;
		}
	}
	Calcret Calc() const override {
		Calcret tmp;
		SymbolList* pt = FindSymbolValue(name);
		tmp.type = pt->sym.status == 1 ? 0 : 1;
		tmp.value = pt->sym.value;
		return tmp;
	}
	SymbolList* FindSym() const override {
		return FindSymbolValue(name);
	}
	int Array(int start, int offset) const override {

		return -1;
	}
};

class StmtAST : public BaseAST {
public:
	std::unique_ptr<BaseAST> stmt;
	Dumpret Dump() const override {
		return stmt->Dump();
		 
	}
	Calcret Calc() const override {
		return stmt->Calc();
	}
	SymbolList* FindSym() const override {
		return NULL;
	}
	int Array(int start, int offset) const override {

		return -1;
	}
};

class MatchedStmtAST : public BaseAST {
public:
	bool exist = 1;
	int type;
	std::unique_ptr<BaseAST> l_val;
	std::unique_ptr<BaseAST> m_stmt;
	std::unique_ptr<BaseAST> exp;
	
	Dumpret Dump() const override {
		// std::cout << "Stmt { ";
		// number->Dump();
		// std::cout << " }";
		if (type == 1) { // RETURN
			if (exist) {
				Dumpret dret = exp->Dump();
				if (dret.type != 0) {
					std::cout << "  ret %" << now - 1 << std::endl;
				}
				else {
					std::cout << "  ret " << dret.value << std::endl;
				}
			}
			else {
				std::cout << "  ret" << std::endl;
			}
			Dumpret tmp;
			tmp.type = 2; // RETURN
			return tmp;
		}
		else if (type == 0) { // LVal = Exp
			SymbolList* tmp = l_val->FindSym();
			assert(tmp->sym.status != 1); // 向常量赋值则错误
			if (tmp->sym.status == 2 || tmp->sym.status == 3) { // 数组、指针
				l_val->Dump();
				int prenow = now - 2;
				Dumpret dret = exp->Dump();
				if (dret.type != 0) {
					std::cout << "  store %" << now - 1 << ", %" << prenow << std::endl;
				}
				else {
					std::cout << "  store " << dret.value << ", %" << prenow << std::endl;
				}
			}
			else {
				Dumpret dret = exp->Dump();
				if (dret.type != 0) {
					std::cout << "  store %" << now - 1 << ", %" << tmp->sym.name << "_" << tmp->sym.cnt << std::endl;
				}
				else {
					std::cout << "  store " << dret.value << ", %" << tmp->sym.name << "_" << tmp->sym.cnt << std::endl;
				}
				tmp->sym.value = exp->Calc().value;
				tmp->sym.init = 1;
			}
		}
		else if (type == 2) { // Exp;
			if (exist) return exp->Dump();
			Dumpret tmp;
			tmp.type = 1;
			return tmp;
		}
		else if (type == 3) { // Block
			AddBlock();
			Dumpret tmp = exp->Dump();
			DeleteBlock();
			return tmp;
		}
		else if (type == 4) { // IF ELSE
			int iftag = ifnow;
			ifnow++;
			Dumpret dret = exp->Dump();
			if (dret.type != 0) {
				std::cout << "  br %" << now - 1 << ", %then" << iftag << ", %else" << iftag << std::endl;
			}
			else {
				std::cout << "  br " << dret.value << ", %then" << iftag << ", %else" << iftag << std::endl;
			}
			std::cout << std::endl << "%then" << iftag << ":" << std::endl;
			Dumpret dret1 = l_val->Dump();
			if(dret1.type != 2 && dret1.type != 6 && dret1.type != 7) std::cout << "  jump %end" << iftag << std::endl;
			std::cout << std::endl << "%else" << iftag << ":" << std::endl;
			Dumpret dret2 = m_stmt->Dump();
			if(dret2.type != 2 && dret2.type != 6 && dret2.type != 7) std::cout << "  jump %end" << iftag << std::endl;
			std::cout << std::endl << "%end" << iftag << ":" << std::endl;
		}
		else if (type == 5) { // While
			// Debug();
			int whiletag = whilenow;
			whileInsert(whiletag);
			whilenow++;
			std::cout << "  jump %while_entry" << whiletag << std::endl;
			std::cout << std::endl << "%while_entry" << whiletag << ":" << std::endl;
			Dumpret dret = exp->Dump();
			if (dret.type != 0) {
				std::cout << "  br %" << now - 1 << ", %while_body" << whiletag << ", " << "%while_end" << whiletag << std::endl;
			}
			else {
				std::cout << "  br " << dret.value << ", %while_body" << whiletag << ", " << "%while_end" << whiletag << std::endl;
			}
			std::cout << std::endl << "%while_body" << whiletag << ":" << std::endl;
			Dumpret bodyret = l_val->Dump();
			if (bodyret.type != 2 && bodyret.type != 6 && bodyret.type != 7) {
				std::cout << "  jump %while_entry" << whiletag << std::endl;
			}
			whileDelete(whiletag);
			std::cout << std::endl << "%while_end" << whiletag << ":" << std::endl;
		}
		else if (type == 6) { // BREAK
			whileList* tmp = whilelist;
			while (tmp->next != NULL) {
				tmp = tmp->next;
			}
			std::cout << "  jump %while_end" << tmp->tag << std::endl;
			Dumpret tt;
			tt.type = 6;
			return tt;
		}
		else if (type == 7) { // CONTINUE
			whileList* tmp = whilelist;
			while (tmp->next != NULL) {
				tmp = tmp->next;
			}
			std::cout << "  jump %while_entry" << tmp->tag << std::endl;
			Dumpret tt;
			tt.type = 7;
			return tt;
		}
		Dumpret tmp;
		tmp.type = 1;
		return tmp;
	}
	
	Calcret Calc() const override {
		if (type == 1) {
			Calcret tmp;
			tmp.type = -1;
			if (exist) {
				tmp = exp->Calc();
				if (tmp.type == 0) tmp.type = 2;// 返回值为常量的函数
			}
			return tmp;
		}
		Calcret tmp;
		tmp.type = -1;
		return tmp;
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
	int Array(int start, int offset) const override {

		return -1;
	}
};

class OpenStmtAST : public BaseAST {
public:
	bool type = 0;
	std::unique_ptr<BaseAST> exp;
	std::unique_ptr<BaseAST> m_stmt;
	std::unique_ptr<BaseAST> o_stmt;
	Dumpret Dump() const override {
		Dumpret dret = exp->Dump();
		int iftag = ifnow;
		ifnow++;
		if (dret.type != 0) {
			std::cout << "  br %" << now - 1 << ", %then" << iftag;
		}
		else {
			std::cout << "  br " << dret.value << ", %then" << iftag;
		}
		if (type) {
			std::cout << ", %else" << iftag << std::endl;
		}
		else {
			std::cout << ", %end" << iftag << std::endl;
		}
		std::cout << std::endl << "%then" << iftag << ":" << std::endl;
		Dumpret dret1 = m_stmt->Dump();
		if(dret1.type != 2 && dret1.type != 6 && dret1.type != 7) std::cout << "  jump %end" << iftag << std::endl;
		if (type) {
			std::cout << std::endl << "%else" << iftag << ":" << std::endl;
			Dumpret dret2 = o_stmt->Dump();
			if(dret2.type != 2 && dret2.type != 6 && dret2.type != 7) std::cout << "  jump %end" << iftag << std::endl;
		}
		std::cout << std::endl << "%end" << iftag << ":" << std::endl;
		Dumpret tmp;
		tmp.type = 1;
		return tmp;
	}
	Calcret Calc() const override {
		Calcret tmp;
		tmp.type = -1;
		return tmp;
	}
	SymbolList* FindSym() const override {

		return NULL;
	}
	int Array(int start, int offset) const override {

		return -1;
	}
};

class ExpAST : public BaseAST {
public:
	std::unique_ptr<BaseAST> lorexp;

	Dumpret Dump() const override {
		return lorexp->Dump();
	}

	Calcret Calc() const override {
		return lorexp->Calc();
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
	int Array(int start, int offset) const override {

		return -1;
	}
};

class PrimaryExpAST : public BaseAST {
public:
	std::unique_ptr<BaseAST> p_exp;
	Dumpret Dump() const override {
		return p_exp->Dump();
	}

	Calcret Calc() const override {
		return p_exp->Calc();
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
	int Array(int start, int offset) const override {

		return -1;
	}
};

class UnaryExpAST : public BaseAST {
public:
	std::string unaryop;
	std::string ident;
	std::unique_ptr<BaseAST> u_exp;
	std::unique_ptr<BaseAST> rparam;
	Dumpret Dump() const override {
		Dumpret dret;
		if (unaryop != "Func0" && unaryop != "Func1") {
			dret = u_exp->Dump();
		}
		else {
			dret.type = 1;
		}
		char tmp = unaryop[0];
		if (tmp == '-') {
			if (dret.type != 0) {
				std::cout << "  %" << now << " = " << "sub 0, " << '%' << now - 1 << std::endl;
				now++;
			}
			else {
				std::cout << "  %" << now << " = " << "sub 0, " << dret.value << std::endl;
				now++;
			}
			Dumpret tmp;
			tmp.type = 1;
			return tmp;
		}
		if (tmp == '!') {
			if (dret.type != 0) {
				std::cout << "  %" << now << " = " << "eq 0, " << '%' << now - 1 << std::endl;
				now++;
			}
			else {
				std::cout << "  %" << now << " = " << "eq 0, " << dret.value << std::endl;
				now++;
			}
			Dumpret tmp;
			tmp.type = 1;
			return tmp;
		}
		if (unaryop == "Func0") { // no param function
			Dumpret tmp;
			SymbolList* funcsym = FindSymbolValue(ident);
			if (funcsym->sym.status == -1) {
				std::cout << "  call @" << ident << "()" << std::endl;
			}
			else {
				std::cout << "  %" << now << " = call @" << ident << "()" << std::endl;
				now++;
			}
		}
		else if (unaryop == "Func1") { // function with params
			Dumpret tmp = rparam->Dump();
			SymbolList* funcsym = FindSymbolValue(ident);
			if (funcsym->sym.status == -2) {
				std::cout << "  %" << now << " = call @" << ident << "(";
				now++;
			}
			else {
				std::cout << "  call @" << ident << "(";
			}
			bool ok = 0;
			ParamList* p = tmp.first;
			while (p != NULL) {
				if (ok) {
					std::cout << ", ";
				};
				if (p->type != 0) {
					std::cout << "%" << p->now;
				}
				else {
					std::cout << p->now;
				}
				ok = 1;
				p = p->next;
			}
			std::cout << ")" << std::endl;
		}
		return dret;
	}
	
	Calcret Calc() const override {
		if (unaryop == "Func0" || unaryop == "Func1") {
			FuncList* tmp = FuncListFind(ident);
			Calcret ctmp;
			if (tmp != NULL) {
				ctmp.type = 0;
				ctmp.value = tmp->val.ret_value;
			}
			else {
				ctmp.type = 2;
			}
			return ctmp;
		}
		else {
			Calcret tmp = u_exp->Calc();
			if (unaryop[0] == '-') {
				tmp.value = -tmp.value;
			}
			else if (unaryop[0] == '!') {
				tmp.value = !tmp.value;
			}
			return tmp;
		}
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
	int Array(int start, int offset) const override {

		return -1;
	}
};

class FuncRParamAST : public BaseAST {
public:
	bool exist = 1;
	std::unique_ptr<BaseAST> exp;
	std::unique_ptr<BaseAST> rparam;
	Dumpret Dump() const override {
		Dumpret tmp = exp->Dump();
		if (tmp.first == NULL) {
			tmp.first = new ParamList;
			tmp.first->next = NULL;
			tmp.first->type = tmp.type;
			if (tmp.type != 0) {
				tmp.first->now = now - 1;
			}
			else {
				tmp.first->now = tmp.value;
			}
		}
		if (exist) {
			tmp.first->next = rparam->Dump().first;
		}
		tmp.type = 1;
		return tmp;
	}
	Calcret Calc() const override {
		Calcret tmp;
		tmp.type = -1;
		return tmp;
	}
	SymbolList* FindSym() const override {

		return NULL;
	}
	int Array(int start, int offset) const override {

		return -1;
	}
};

class MulExpAST : public BaseAST {
public:
	std::string mulop;
	std::unique_ptr<BaseAST> m_exp;
	std::unique_ptr<BaseAST> u_exp;
	Dumpret Dump() const override {
		char tmp = mulop[0];
		if (tmp == '*' || tmp == '/' || tmp == '%') {
			Dumpret mret = m_exp->Dump();
			if (mret.type != 0) {
				int tnow = now - 1;
				Dumpret uret = u_exp->Dump();
				std::cout << "  %" << now << " = ";
				if (tmp == '*') {
					std::cout << "mul ";
				}
				else if (tmp == '/') {
					std::cout << "div ";
				}
				else {
					std::cout << "mod ";
				}
				if (uret.type != 0) {
					std::cout << "%" << tnow << ", %" << now - 1 << std::endl;
				}
				else {
					std::cout << "%" << tnow << ", " << uret.value << std::endl;
				}
			}
			else {
				Dumpret uret = u_exp->Dump();
				std::cout << "  %" << now << " = ";
				if (tmp == '*') {
					std::cout << "mul ";
				}
				else if (tmp == '/') {
					std::cout << "div ";
				}
				else {
					std::cout << "mod ";
				}
				if (uret.type != 0) {
					std::cout << mret.value << ", %" << now - 1 << std::endl;
				}
				else {
					std::cout << mret.value << ", " << uret.value << std::endl;
				}
			}
			now++;
			Dumpret tmp;
			tmp.type = 1;
			return tmp;
		}
		return u_exp->Dump();
	}

	Calcret Calc() const override {
		Calcret l;
		if (mulop[0] == '*') {
			l = m_exp->Calc();
			Calcret r = u_exp->Calc();
			l.value = l.value * r.value;
		}
		else if (mulop[0] == '/') {
			l = m_exp->Calc();
			Calcret r = u_exp->Calc();
			if (r.value != 0) {
				l.value = l.value / r.value;
			}
		}
		else if (mulop[0] == '%') {
			l = m_exp->Calc();
			Calcret r = u_exp->Calc();
			l.value = l.value % r.value;
		}
		else {
			l = u_exp->Calc();
		}
		return l;
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
	int Array(int start, int offset) const override {

		return -1;
	}
};

class AddExpAST : public BaseAST {
public:
	std::string addop;
	std::unique_ptr<BaseAST> m_exp;
	std::unique_ptr<BaseAST> a_exp;
	Dumpret Dump() const override {
		char tmp = addop[0];
		if (tmp == '+' || tmp == '-') {
			Dumpret aret = a_exp->Dump();
			if (aret.type != 0) {
				int tnow = now - 1;
				Dumpret mret = m_exp->Dump();
				std::cout << "  %" << now << " = ";
				if (tmp == '+') {
					std::cout << "add ";
				}
				else {
					std::cout << "sub ";
				}
				if (mret.type != 0) {
					std::cout << "%" << tnow << ", %" << now - 1 << std::endl;
				}
				else {
					std::cout << "%" << tnow << ", " << mret.value << std::endl;
				}
			}
			else {
				Dumpret mret = m_exp->Dump();
				std::cout << "  %" << now << " = ";
				if (tmp == '+') {
					std::cout << "add ";
				}
				else {
					std::cout << "sub ";
				}
				if (mret.type != 0) {
					std::cout << aret.value << ", %" << now - 1 << std::endl;
				}
				else {
					std::cout << aret.value << ", " << mret.value << std::endl;
				}
			}
			now++;
			Dumpret tmp;
			tmp.type = 1;
			return tmp;
		}
		return m_exp->Dump();
	}

	Calcret Calc() const override {
		Calcret l;
		if (addop[0] == '+') {
			l = a_exp->Calc();
			Calcret r = m_exp->Calc();
			l.value = l.value + r.value;
		}
		else if (addop[0] == '-') {
			l = a_exp->Calc();
			Calcret r = m_exp->Calc();
			l.value = l.value - r.value;
		}
		else {
			l = m_exp->Calc();
		}
		return l;
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
	int Array(int start, int offset) const override {

		return -1;
	}
};

class RelExpAST : public BaseAST {
public:
	std::string relop;
	std::unique_ptr<BaseAST> a_exp;
	std::unique_ptr<BaseAST> r_exp;
	Dumpret Dump() const override {
		if (relop == ">" || relop == "<" || relop == ">=" || relop == "<=") {
			Dumpret rret = r_exp->Dump();
			if (rret.type != 0) {
				int tnow = now - 1;
				Dumpret aret = a_exp->Dump();
				std::cout << "  %" << now << " = ";
				if (relop == ">") {
					std::cout << "gt ";
				}
				else if (relop == "<") {
					std::cout << "lt ";
				}
				else if (relop == ">=") {
					std::cout << "ge ";
				}
				else if (relop == "<=") {
					std::cout << "le ";
				}
				if (aret.type != 0) {
					std::cout << "%" << tnow << ", %" << now - 1 << std::endl;
				}
				else {
					std::cout << "%" << tnow << ", " << aret.value << std::endl;
				}
			}
			else {
				Dumpret aret = a_exp->Dump();
				std::cout << "  %" << now << " = ";
				if (relop == ">") {
					std::cout << "gt ";
				}
				else if (relop == "<") {
					std::cout << "lt ";
				}
				else if (relop == ">=") {
					std::cout << "ge ";
				}
				else if (relop == "<=") {
					std::cout << "le ";
				}
				if (aret.type != 0) {
					std::cout << rret.value << ", %" << now - 1 << std::endl;
				}
				else {
					std::cout << rret.value << ", " << aret.value << std::endl;
				}
			}
			now++;
			Dumpret tmp;
			tmp.type = 1;
			return tmp;
		}
		return a_exp->Dump();
	}

	Calcret Calc() const override {
		Calcret l;
		if (relop == "<") {
			l = r_exp->Calc();
			Calcret r = a_exp->Calc();
			l.value = l.value < r.value;
		}
		else if (relop == ">") {
			l = r_exp->Calc();
			Calcret r = a_exp->Calc();
			l.value = l.value > r.value;
		}
		else if (relop == "<=") {
			l = r_exp->Calc();
			Calcret r = a_exp->Calc();
			l.value = l.value <= r.value;
		}
		else if (relop == ">=") {
			l = r_exp->Calc();
			Calcret r = a_exp->Calc();
			l.value = l.value >= r.value;
		}
		else {
			l = a_exp->Calc();
		}
		return l;
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
	int Array(int start, int offset) const override {

		return -1;
	}
};

class EqExpAST : public BaseAST {
public:
	std::string eqop;
	std::unique_ptr<BaseAST> r_exp;
	std::unique_ptr<BaseAST> e_exp;
	Dumpret Dump() const override {
		if (eqop == "==" || eqop == "!=") {
			Dumpret eret = e_exp->Dump();
			if (eret.type != 0) {
				int tnow = now - 1;
				Dumpret rret = r_exp->Dump();
				std::cout << "  %" << now << " = ";
				if (eqop == "==") {
					std::cout << "eq ";
				}
				else if (eqop == "!=") {
					std::cout << "ne ";
				}
				if (rret.type != 0) {
					std::cout << "%" << tnow << ", %" << now - 1 << std::endl;
				}
				else {
					std::cout << "%" << tnow << ", " << rret.value << std::endl;
				}
			}
			else {
				Dumpret rret = r_exp->Dump();
				std::cout << "  %" << now << " = ";
				if (eqop == "==") {
					std::cout << "eq ";
				}
				else if (eqop == "!=") {
					std::cout << "ne ";
				}
				if (rret.type != 0) {
					std::cout << eret.value << ", %" << now - 1 << std::endl;
				}
				else {
					std::cout << eret.value << ", " << rret.value << std::endl;
				}
			}
			now++;
			Dumpret tmp;
			tmp.type = 1;
			return tmp;
		}
		return r_exp->Dump();
	}

	Calcret Calc() const override {
		Calcret l;
		if (eqop == "==") {
			l = e_exp->Calc();
			Calcret r = r_exp->Calc();
			l.value = l.value == r.value;
		}
		else if (eqop == "!=") {
			l = e_exp->Calc();
			Calcret r = r_exp->Calc();
			l.value = l.value != r.value;
		}
		else {
			l = r_exp->Calc();
		}
		return l;
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
	int Array(int start, int offset) const override {

		return -1;
	}
};

class LAndExpAST : public BaseAST {
public:
	std::string andop;
	std::unique_ptr<BaseAST> e_exp;
	std::unique_ptr<BaseAST> la_exp;
	Dumpret Dump() const override {
		if (andop == "&&") {
			Dumpret laret = la_exp->Dump();
			Calcret lhs = la_exp->Calc();
			if (lhs.value != 0 || lhs.type != 0) { // 如果是常值（与变量无关）或者是单值返回的函数，则可以短路处理
				if (laret.type != 0) {
					int tnow = now - 1;
					Dumpret eret = e_exp->Dump();
					std::cout << "  %" << now << " = ne 0, %" << tnow << std::endl;
					now++;
					if (eret.type != 0) {
						std::cout << "  %" << now << " = ne 0, %" << now - 2 << std::endl;
					}
					else {
						std::cout << "  %" << now << " = ne 0, " << eret.value << std::endl;
					}
					now++;
					std::cout << "  %" << now << " = and %" << now - 1 << ", %" << now - 2 << std::endl;
				}
				else {
					Dumpret eret = e_exp->Dump();
					std::cout << "  %" << now << " = ne 0, " << laret.value << std::endl;
					now++;
					if (eret.type != 0) {
						std::cout << "  %" << now << " = ne 0, %" << now - 2 << std::endl;
					}
					else {
						std::cout << "  %" << now << " = ne 0, " << eret.value << std::endl;
					}
					now++;
					std::cout << "  %" << now << " = and %" << now - 1 << ", %" << now - 2 << std::endl;
				}
			}
			else {
				std::cout << "  %" << now << " = add 0, 0" << std::endl;
			}
			now++;
			Dumpret tmp;
			tmp.type = 1;
			return tmp;
		}
		return e_exp->Dump();
	}

	Calcret Calc() const override {
		Calcret l;
		if (andop == "&&") {
			l = la_exp->Calc();
			Calcret r = e_exp->Calc();
			l.value = l.value && r.value;
		}
		else {
			l = e_exp->Calc();
		}
		return l;
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
	int Array(int start, int offset) const override {

		return -1;
	}
};

class LOrExpAST : public BaseAST {
public:
	std::string orop;
	std::unique_ptr<BaseAST> la_exp;
	std::unique_ptr<BaseAST> lo_exp;
	Dumpret Dump() const override {
		if (orop == "||") {
			Dumpret loret = lo_exp->Dump();
			Calcret lhs = lo_exp->Calc();
			if (lhs.value == 0 || lhs.type != 0) {
				if (loret.type != 0) {
					int tnow = now - 1;
					Dumpret laret = la_exp->Dump();
					std::cout << "  %" << now << " = or ";
					if (laret.type != 0) {
						std::cout << "%" << tnow << ", %" << now - 1 << std::endl;
					}
					else {
						std::cout << "%" << tnow << ", " << laret.value << std::endl;
					}
					now++;
					std::cout << "  %" << now << " = ne %" << now - 1 << ", 0" << std::endl;
				}
				else {
					Dumpret laret = la_exp->Dump();
					std::cout << "  %" << now << " = or ";
					if (laret.type == -1) {
						std::cout << loret.value << ", %" << now - 1 << std::endl;
					}
					else {
						std::cout << loret.value << ", " << laret.value << std::endl;
					}
					now++;
					std::cout << "  %" << now << " = ne %" << now - 1 << ", 0" << std::endl;
				}
			}
			else {
				std::cout << "  %" << now << " = add 0, 1" << std::endl;
			}
			now++;
			Dumpret tmp;
			tmp.type = 1;
			return tmp;
		}
		return la_exp->Dump();
	}

	Calcret Calc() const override {
		Calcret l;
		if (orop == "||") {
			l = la_exp->Calc();
			Calcret r = lo_exp->Calc();
			l.value = l.value || r.value;
		}
		else {
			l = la_exp->Calc();
		}
		return l;
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
	int Array(int start, int offset) const override {

		return -1;
	}
};

class ConstExpAST : public BaseAST {
public:
	std::unique_ptr<BaseAST> c_exp;
	Dumpret Dump() const override {
		// c_exp->Dump();
		Dumpret tmp;
		tmp.type = 1;
		return tmp;
	}

	Calcret Calc() const override {
		return c_exp->Calc();
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
	int Array(int start, int offset) const override {

		return -1;
	}
};

class NumberAST : public BaseAST {
public:
	std::string IntConst;

	Dumpret Dump() const override {
		// std::cout << "Number { " << IntConst << " }";
		Dumpret tmp;
		tmp.type = 0;
		tmp.value = atoi(IntConst.c_str());
		return tmp;
	}

	Calcret Calc() const override {
		Calcret tmp;
		tmp.value = atoi(IntConst.c_str());
		tmp.type = 0;
		return tmp;
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
	int Array(int start, int offset) const override {

		return -1;
	}
};
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

typedef struct {
	std::string name;
	int value, status, cnt;
	// status = 0为变量，=1为常量, =2为函数
	// cnt表示前面有几个同名变量，最终的变量名由name和cnt拼接而成
	bool init;
}Symbol;

// 记录最内层的while循环
struct whileList{
	int tag;
	whileList* next;
};

static whileList* whilelist = NULL;

// Dump的返回值，前面几个lv因为这个全部有大问题
typedef struct {
	int type;
	int value;
}Dumpret;

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
	assert(blockt != NULL);
	return NULL;
}

inline Symbol InsertSymbol(std::string s, int value, int status, bool init) {
	if (nowBlock == NULL) {
		nowBlock = new BlockSymList;
		nowBlock->father = NULL;
		nowBlock->symlist = NULL;
	}
	SymbolList* p = nowBlock->symlist;
	while (p != NULL) {
		assert(p->sym.name != s); //重定义
		p = p->next;
	}
	// 找到目前是第几个同名变量
	BlockSymList* blockt = nowBlock->father;
	int cnt = 1;
	bool ok = 0;
	while (!ok && blockt != NULL) {
		p = blockt->symlist;
		while (p != NULL) {
			if (p->sym.name == s) {
				cnt = p->sym.cnt + 1;
				ok = 1;
				break;
			}
			p = p->next;
		}
		blockt = blockt->father;
	}

	// 判断以前是否使用过了这个名字，若使用过了则不需要alloc
	bool used = 0;
	SymbolList* atmp = allSymbol;
	while (atmp != NULL) {
		if (atmp->sym.name == s) {
			if (atmp->sym.cnt >= cnt) {
				used = 1; // 用过了
			}
			atmp->sym.cnt = atmp->sym.cnt > cnt ? atmp->sym.cnt : cnt;
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
		if (used) stmp.status = -1;
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
		if (used) stmp.status = -1;
		return stmp;
	}
}

class BaseAST {
public:
	virtual ~BaseAST() = default;

	virtual Dumpret Dump() const = 0;

	virtual int Calc() const = 0;

	virtual SymbolList* FindSym() const = 0;
};

class PreCompAST : public BaseAST {
public:
	std::unique_ptr<BaseAST> comp_unit;
	Dumpret Dump() const override {
		AddBlock(); // 全局作用域
		comp_unit->Dump();
		DeleteBlock();
		Dumpret tmp;
		tmp.type = 1;
		return tmp;
	}
	int Calc() const override {

		return -1;
	}
	SymbolList* FindSym() const override {

		return NULL;
	}
};

class CompUnitAST : public BaseAST {
public:
	bool exist = 1;
	std::unique_ptr<BaseAST> comp;
	std::unique_ptr<BaseAST> func_def;

	Dumpret Dump() const override {
		// std::cout << "CompUnitAST { ";
		Debug();
		if(exist) comp->Dump();
		func_def->Dump();
		// std::cout << " }";
		Dumpret tmp;
		tmp.type = 1;
		return tmp;
	}

	int Calc() const override {
		
		return -1;
	}
	SymbolList* FindSym() const override{

		return NULL;
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

	int Calc() const override {
		
		return -1;
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
};

class ConstDeclAST : public BaseAST {
public:
	std::unique_ptr<BaseAST> btype;
	std::unique_ptr<BaseAST> const_def;
	std::unique_ptr<BaseAST> const_defs;
	Dumpret Dump() const override {
		btype->Dump();
		const_def->Dump();
		const_defs->Dump();
		Dumpret tmp;
		tmp.type = 1;
		return tmp;
	}
	int Calc() const override {

		return -1;
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
};

class BTypeAST : public BaseAST {
public:
	std::string type;
	Dumpret Dump() const override {
		// std::cout << type;
		Dumpret tmp;
		tmp.type = 1;
		return tmp;
	}
	int Calc() const override {

		return -1;
	}
	SymbolList* FindSym() const override{

		return NULL;
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
	int Calc() const override {

		return -1;
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
};

class ConstDefAST : public BaseAST {
public:
	std::string name;
	std::unique_ptr<BaseAST> const_init_val;
	Dumpret Dump() const override {
		auto val = const_init_val->Calc();
		InsertSymbol(name, val, 1, 1);
		Dumpret tmp;
		tmp.type = 1;
		return tmp;
	}
	int Calc() const override {

		return -1;
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
};

class ConstInitValAST : public BaseAST {
public:
	std::unique_ptr<BaseAST> const_exp;
	Dumpret Dump() const override {
		// const_exp->Dump();
		Dumpret tmp;
		tmp.type = 1;
		return tmp;
	}
	int Calc() const override {
		return const_exp->Calc();
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
};

class VarDeclAST : public BaseAST {
public:
	std::unique_ptr<BaseAST> btype;
	std::unique_ptr<BaseAST> var_def;
	std::unique_ptr<BaseAST> var_defs;
	Dumpret Dump() const override {
		btype->Dump();
		var_def->Dump();
		var_defs->Dump();
		Dumpret tmp;
		tmp.type = 1;
		return tmp;
	}
	int Calc() const override {
		
		return -1;
	}
	SymbolList* FindSym() const override{

		return NULL;
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
	int Calc() const override {

		return -1;
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
};

class VarDefAST : public BaseAST {
public:
	bool initialized = 1; //是否初始化
	std::string name;
	std::unique_ptr<BaseAST> init_val;
	Dumpret Dump() const override {
		auto val = initialized ? init_val->Calc() : 0;
		Symbol sym = InsertSymbol(name, val, 0, initialized);
		if(sym.status != -1) std::cout << "  @" << name << "_" << sym.cnt << " = alloc i32" << std::endl;
		if (initialized) {
			Dumpret dret = init_val->Dump();
			if (dret.type != 0) {
				std::cout << "  store %" << now - 1 << ", @" << name << "_" << sym.cnt << std::endl;
			}
			else {
				std::cout << "  store " << dret.value << ", @" << name << "_" << sym.cnt << std::endl;
			}
		}
		Dumpret tmp;
		tmp.type = 1;
		return tmp;
	}
	int Calc() const override {

		return -1;
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
};

class InitValAST : public BaseAST {
public:
	std::unique_ptr<BaseAST> exp;
	Dumpret Dump() const override {
		return exp->Dump();
	}
	int Calc() const override {
		return exp->Calc();
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
};

class FuncDefAST : public BaseAST {
public:
	bool param_exist = 0;
	std::unique_ptr<BaseAST> func_type;
	std::string ident;
	std::unique_ptr<BaseAST> block;
	std::unique_ptr<BaseAST> fparams;

	Dumpret Dump() const override {
		// std::cout << "FuncDefAST { ";
		// func_type->Dump();
		// std::cout << ", " << ident << ", ";
		// block->Dump();
		// std::cout << " }";
		InsertSymbol(ident, 0, 2, 1);
		std::cout << "fun @" << ident << "(";
		AddBlock();
		if (param_exist) fparams->Calc();
		std::cout << "): ";
		Dumpret funcret = func_type->Dump();
		if (param_exist)fparams->Dump();
		Dumpret blkret = block->Dump();
		DeleteBlock();
		if (blkret.type != 2) {
			assert(funcret.type == -3);
			std::cout << "  ret" << std::endl;
		}
		std::cout << "}" << std::endl << std::endl;
		Dumpret tmp;
		tmp.type = 1;
		return tmp;
	}

	int Calc() const override {

		return -1;
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
};

class FuncTypeAST : public BaseAST {
public:
	std::string type;
	Dumpret Dump() const override {
		// std::cout << "FuncTypeAST { " << INT << " }";
		Dumpret tmp;
		if (type == "int") {
			std::cout << "i32 " << "{" << std::endl << "%entry:" << std::endl;
			tmp.type = -2;
		}
		else if (type == "void") {
			std::cout << "{" << std::endl << "%entry:" << std::endl;
			tmp.type = -3;
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
	int Calc() const override {
		fparam->Calc();
		if (exist) {
			std::cout << ", ";
			fparams->Calc();
		}
		return -1;
	}
	SymbolList* FindSym() const override {

		return NULL;
	}
};

class FuncFParamAST : public BaseAST {
public:
	std::string ident;
	std::unique_ptr<BaseAST> btype;
	Dumpret Dump() const override {
		Symbol sym = InsertSymbol(ident, 0, 0, 1);
		if (sym.status != -1) {
			std::cout << "  %" << ident << "_" << sym.cnt << " = alloc i32" << std::endl;
		}
		std::cout << "  store @" << ident << ", " << "%" << ident << "_" << sym.cnt << std::endl;
		Dumpret tmp;
		tmp.type = 1;
		return tmp;
	}
	int Calc() const override {
		std::cout << "@" << ident << ": i32";
		return -1;
	}
	SymbolList* FindSym() const override {

		return NULL;
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

	int Calc() const override {

		return -1;
	}
	SymbolList* FindSym() const override{

		return NULL;
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
			int rett = block_items->Calc();
			if (tmp1.type == 2 && rett == 1) { // return后紧跟语句
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
	int Calc() const override {
		if (exist) return 1;
		return 0;
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
};

class BlockItemAST : public BaseAST {
public:
	std::unique_ptr<BaseAST> bi_ptr;
	Dumpret Dump() const override {
		
		return bi_ptr->Dump();
	}
	int Calc() const override {
		
		return -1;
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
};

class LValAST : public BaseAST {
public:
	std::string name;
	Dumpret Dump() const override {
		SymbolList* tmp = FindSymbolValue(name);
		if (tmp->sym.status == 1) {
			Dumpret p;
			p.type = 0;
			p.value = tmp->sym.value;
			return p;
		}
		else {
			std::cout << "  %" << now << " = load @" << tmp->sym.name << "_" << tmp->sym.cnt << std::endl;
			now++;
			Dumpret tmp;
			tmp.type = 1;
			return tmp;
		}
	}
	int Calc() const override {
		return FindSymbolValue(name)->sym.value;
	}
	SymbolList* FindSym() const override {
		return FindSymbolValue(name);
	}
};

class StmtAST : public BaseAST {
public:
	std::unique_ptr<BaseAST> stmt;
	Dumpret Dump() const override {
		return stmt->Dump();
		 
	}
	int Calc() const override {
		return -1;
	}
	SymbolList* FindSym() const override {
		return NULL;
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
			assert(tmp->sym.status == 0); // 向常量赋值则错误
			Dumpret dret = exp->Dump();
			if (dret.type != 0) {
				std::cout << "  store %" << now - 1 << ", @" << tmp->sym.name << "_" << tmp->sym.cnt << std::endl;
			}
			else {
				std::cout << "  store " << dret.value << ", @" << tmp->sym.name << "_" << tmp->sym.cnt << std::endl;
			}
			tmp->sym.value = exp->Calc();
			tmp->sym.init = 1;
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
			Debug();
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
	
	int Calc() const override {

		return -1;
	}
	SymbolList* FindSym() const override{

		return NULL;
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
	int Calc() const override {

		return -1;
	}
	SymbolList* FindSym() const override {

		return NULL;
	}
};

class ExpAST : public BaseAST {
public:
	std::unique_ptr<BaseAST> lorexp;

	Dumpret Dump() const override {
		return lorexp->Dump();
	}

	int Calc() const override {
		return lorexp->Calc();
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
};

class PrimaryExpAST : public BaseAST {
public:
	std::unique_ptr<BaseAST> p_exp;
	Dumpret Dump() const override {
		return p_exp->Dump();
	}

	int Calc() const override {
		return p_exp->Calc();
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
};

class UnaryExpAST : public BaseAST {
public:
	bool exist = 1;
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
		if (unaryop == "Func0") { // void function
			std::cout << "  call @" << ident << "(";
			if (exist) rparam->Dump();
			std::cout << ")" << std::endl;
		}
		else if (unaryop == "Func1") { // int function
			std::cout << "  %" << now << " = call @" << ident << "(";
			now++;
			if (exist)rparam->Dump();
			std::cout << ")" << std::endl;
		}
		return dret;
	}
	
	int Calc() const override {
		if (unaryop[0] == '-') {
			return -u_exp->Calc();
		}
		else if (unaryop[0] == '!') {
			return !u_exp->Calc();
		}
		return u_exp->Calc();
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
};

class FuncRParamAST : public BaseAST {
public:
	bool exist = 1;
	std::unique_ptr<BaseAST> exp;
	std::unique_ptr<BaseAST> rparam;
	Dumpret Dump() const override {
		Dumpret tmp = exp->Dump();
		if (tmp.type != 0) {
			std::cout << "%" << now - 1;
		}
		else {
			std::cout << tmp.value;
		}
		if (exist) {
			std::cout << ", ";
			tmp = rparam->Dump();
		}
		tmp.type = 1;
		return tmp;
	}
	int Calc() const override {

		return -1;
	}
	SymbolList* FindSym() const override {

		return NULL;
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

	int Calc() const override {
		if (mulop[0] == '*') {
			return m_exp->Calc() * u_exp->Calc();
		}
		else if (mulop[0] == '/') {
			return m_exp->Calc() / u_exp->Calc();
		}
		else if (mulop[0] == '%') {
			return m_exp->Calc() % u_exp->Calc();
		}
		return u_exp->Calc();
	}
	SymbolList* FindSym() const override{

		return NULL;
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

	int Calc() const override {
		if (addop[0] == '+') {
			return a_exp->Calc() + m_exp->Calc();
		}
		else if (addop[0] == '-') {
			return a_exp->Calc() - m_exp->Calc();
		}
		return m_exp->Calc();
	}
	SymbolList* FindSym() const override{

		return NULL;
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

	int Calc() const override {
		if (relop == "<") {
			return r_exp->Calc() < a_exp->Calc();
		}
		else if (relop == ">") {
			return r_exp->Calc() > a_exp->Calc();
		}
		else if (relop == "<=") {
			return r_exp->Calc() <= a_exp->Calc();
		}
		else if (relop == ">=") {
			return r_exp->Calc() >= a_exp->Calc();
		}
		return a_exp->Calc();
	}
	SymbolList* FindSym() const override{

		return NULL;
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

	int Calc() const override {
		if (eqop == "==") {
			return e_exp->Calc() == r_exp->Calc();
		}
		else if (eqop == "!=") {
			return e_exp->Calc() != r_exp->Calc();
		}
		return r_exp->Calc();
	}
	SymbolList* FindSym() const override{

		return NULL;
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
			int lhs = la_exp->Calc();
			if (lhs != 0 || laret.type != 0) {
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

	int Calc() const override {
		if (andop == "&&") {
			return la_exp->Calc() && e_exp->Calc();
		}
		return e_exp->Calc();
	}
	SymbolList* FindSym() const override{

		return NULL;
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
			int lhs = lo_exp->Calc();
			if (lhs == 0 || loret.type != 0) {
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

	int Calc() const override {
		if (orop == "||") {
			return la_exp->Calc() || lo_exp->Calc();
		}
		return la_exp->Calc();
	}
	SymbolList* FindSym() const override{

		return NULL;
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

	int Calc() const override {
		return c_exp->Calc();
	}
	SymbolList* FindSym() const override{

		return NULL;
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

	int Calc() const override {
		return atoi(IntConst.c_str());
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
};
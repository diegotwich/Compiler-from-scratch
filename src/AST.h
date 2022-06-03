#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <cassert>

// 解决单赋值问题
static int now = 0;

typedef struct {
	std::string name;
	int value, status, cnt;
	// status = 0为变量，=1为常量
	// cnt表示前面有几个同名变量，最终的变量名由name和cnt拼接而成
	bool init;
}Symbol;

struct SymbolList{
	Symbol sym;
	SymbolList* next;
};

struct BlockSymList {
	SymbolList* symlist;
	BlockSymList* father;
};

static BlockSymList* nowBlock = NULL;

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
	p = nowBlock->symlist;
	if (nowBlock->symlist == NULL) {
		nowBlock->symlist = new SymbolList;
		nowBlock->symlist->next = NULL;
		nowBlock->symlist->sym.name = s;
		nowBlock->symlist->sym.value = value;
		nowBlock->symlist->sym.status = status;
		nowBlock->symlist->sym.init = init;
		nowBlock->symlist->sym.cnt = cnt;
		return nowBlock->symlist->sym;
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
		return tmp->sym;
	}
}

class BaseAST {
public:
	virtual ~BaseAST() = default;

	virtual int Dump() const = 0;

	virtual int Calc() const = 0;

	virtual SymbolList* FindSym() const = 0;
};

class CompUnitAST : public BaseAST {
public:
	std::unique_ptr<BaseAST> func_def;

	int Dump() const override {
		// std::cout << "CompUnitAST { ";
		func_def->Dump();
		// std::cout << " }";
		return -1;
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

	int Dump() const override {
		decl->Dump();
		return -1;
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
	int Dump() const override {
		btype->Dump();
		const_def->Dump();
		const_defs->Dump();
		return -1;
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
	int Dump() const override {
		// std::cout << type;
		return -1;
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
	int Dump() const override {
		if (exist) {
			const_def->Dump();
			const_defs->Dump();
		}
		return -1;
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
	int Dump() const override {
		auto val = const_init_val->Calc();
		InsertSymbol(name, val, 1, 1);
		return -1;
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
	int Dump() const override {
		// const_exp->Dump();
		return -1;
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
	int Dump() const override {
		btype->Dump();
		var_def->Dump();
		var_defs->Dump();
		return -1;
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
	int Dump() const override {
		if (exist) {
			var_def->Dump();
			var_defs->Dump();
		}
		return -1;
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
	int Dump() const override {
		auto val = initialized ? init_val->Calc() : 0;
		Symbol sym = InsertSymbol(name, val, 0, initialized);
		std::cout << "  @" << name << "_" << sym.cnt << " = alloc i32" << std::endl;
		if (initialized) {
			int ret = init_val->Dump();
			if (ret == -1) {
				std::cout << "  store %" << now - 1 << ", @" << name << "_" << sym.cnt << std::endl;
			}
			else {
				std::cout << "  store " << ret << ", @" << name << "_" << sym.cnt << std::endl;
			}
		}
		return -1;
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
	int Dump() const override {
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
	std::unique_ptr<BaseAST> func_type;
	std::string ident;
	std::unique_ptr<BaseAST> block;

	int Dump() const override {
		// std::cout << "FuncDefAST { ";
		// func_type->Dump();
		// std::cout << ", " << ident << ", ";
		// block->Dump();
		// std::cout << " }";
		std::cout << "fun @" << ident << "(): ";
		func_type->Dump();
		block->Dump();
		return -1;
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

	int Dump() const override {
		// std::cout << "FuncTypeAST { " << INT << " }";
		std::cout << "i32 " << "{" << std::endl << "%entry:" << std::endl;
		return -1;
	}

	int Calc() const override {
		
		return -1;
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
};

class BlockAST : public BaseAST {
public:
	std::unique_ptr<BaseAST> block_items;

	int Dump() const override {
		// std::cout << "Block { ";
		// stmt->Dump();
		// std::cout << " }";
		AddBlock();
		block_items->Dump();
		DeleteBlock();
		return -1;
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

	int Dump() const override {
		if (exist) {
			block_item->Dump();
			block_items->Dump();
		}
		return -1;
	}
	int Calc() const override {
		
		return -1;
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
};

class BlockItemAST : public BaseAST {
public:
	std::unique_ptr<BaseAST> bi_ptr;
	int Dump() const override {
		bi_ptr->Dump();
		return -1;
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
	int Dump() const override {
		SymbolList* tmp = FindSymbolValue(name);
		if (tmp->sym.status == 1) {
			return tmp->sym.value;
		}
		else {
			std::cout << "  %" << now << " = load @" << tmp->sym.name << "_" << tmp->sym.cnt << std::endl;
			now++;
			return -1;
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
	bool exist = 1;
	int type;
	std::unique_ptr<BaseAST> l_val;
	std::unique_ptr<BaseAST> exp;
	
	int Dump() const override {
		// std::cout << "Stmt { ";
		// number->Dump();
		// std::cout << " }";
		if (type == 1) { // RETURN
			if (exist) {
				int ret = exp->Dump();
				if (ret == -1) {
					std::cout << "  ret %" << now - 1 << std::endl << "}";
				}
				else {
					std::cout << "  ret " << ret << std::endl << "}";
				}
			}
			else {
				std::cout << "  ret" << std::endl << "}";
			}
		}
		else if (type == 0) { // LVal = Exp
			SymbolList* tmp = l_val->FindSym();
			assert(tmp->sym.status == 0); // 向常量赋值则错误
			int ret = exp->Dump();
			if (ret == -1) {
				std::cout << "  store %" << now - 1 << ", @" << tmp->sym.name << "_" << tmp->sym.cnt << std::endl;
			}
			else {
				std::cout << "  store " << ret << ", @" << tmp->sym.name << "_" << tmp->sym.cnt << std::endl;
			}
			tmp->sym.value = exp->Calc();
			tmp->sym.init = 1;
		}
		else if (type == 3) { // Block
			exp->Dump();
		}
		return -1;
	}
	
	int Calc() const override {

		return -1;
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
};

class ExpAST : public BaseAST {
public:
	std::unique_ptr<BaseAST> lorexp;

	int Dump() const override {
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
	int Dump() const override {
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
	std::string unaryop;
	std::unique_ptr<BaseAST> u_exp;
	int Dump() const override {
		int ret = u_exp->Dump();
		char tmp = unaryop[0];
		if (tmp == '-') {
			if (ret == -1) {
				std::cout << "  %" << now << " = " << "sub 0, " << '%' << now - 1 << std::endl;
				now++;
			}
			else {
				std::cout << "  %" << now << " = " << "sub 0, " << ret << std::endl;
				now++;
			}
			return -1;
		}
		if (tmp == '!') {
			if (ret == -1) {
				std::cout << "  %" << now << " = " << "eq 0, " << '%' << now - 1 << std::endl;
				now++;
			}
			else {
				std::cout << "  %" << now << " = " << "eq 0, " << ret << std::endl;
				now++;
			}
			return -1;
		}
		return ret;
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

class MulExpAST : public BaseAST {
public:
	std::string mulop;
	std::unique_ptr<BaseAST> m_exp;
	std::unique_ptr<BaseAST> u_exp;
	int Dump() const override {
		char tmp = mulop[0];
		if (tmp == '*' || tmp == '/' || tmp == '%') {
			int mret = m_exp->Dump();
			if (mret == -1) {
				int tnow = now - 1;
				int uret = u_exp->Dump();
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
				if (uret == -1) {
					std::cout << "%" << tnow << ", %" << now - 1 << std::endl;
				}
				else {
					std::cout << "%" << tnow << ", " << uret << std::endl;
				}
			}
			else {
				int uret = u_exp->Dump();
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
				if (uret == -1) {
					std::cout << mret << ", %" << now - 1 << std::endl;
				}
				else {
					std::cout << mret << ", " << uret << std::endl;
				}
			}
			now++;
			return -1;
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
	int Dump() const override {
		char tmp = addop[0];
		if (tmp == '+' || tmp == '-') {
			int aret = a_exp->Dump();
			if (aret == -1) {
				int tnow = now - 1;
				int mret = m_exp->Dump();
				std::cout << "  %" << now << " = ";
				if (tmp == '+') {
					std::cout << "add ";
				}
				else {
					std::cout << "sub ";
				}
				if (mret == -1) {
					std::cout << "%" << tnow << ", %" << now - 1 << std::endl;
				}
				else {
					std::cout << "%" << tnow << ", " << mret << std::endl;
				}
			}
			else {
				int mret = m_exp->Dump();
				std::cout << "  %" << now << " = ";
				if (tmp == '+') {
					std::cout << "add ";
				}
				else {
					std::cout << "sub ";
				}
				if (mret == -1) {
					std::cout << aret << ", %" << now - 1 << std::endl;
				}
				else {
					std::cout << aret << ", " << mret << std::endl;
				}
			}
			now++;
			return -1;
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
	int Dump() const override {
		if (relop == ">" || relop == "<" || relop == ">=" || relop == "<=") {
			int rret = r_exp->Dump();
			if (rret == -1) {
				int tnow = now - 1;
				int aret = a_exp->Dump();
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
				if (aret == -1) {
					std::cout << "%" << tnow << ", %" << now - 1 << std::endl;
				}
				else {
					std::cout << "%" << tnow << ", " << aret << std::endl;
				}
			}
			else {
				int aret = a_exp->Dump();
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
				if (aret == -1) {
					std::cout << rret << ", %" << now - 1 << std::endl;
				}
				else {
					std::cout << rret << ", " << aret << std::endl;
				}
			}
			now++;
			return -1;
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
	int Dump() const override {
		if (eqop == "==" || eqop == "!=") {
			int eret = e_exp->Dump();
			if (eret == -1) {
				int tnow = now - 1;
				int rret = r_exp->Dump();
				std::cout << "  %" << now << " = ";
				if (eqop == "==") {
					std::cout << "eq ";
				}
				else if (eqop == "!=") {
					std::cout << "ne ";
				}
				if (rret == -1) {
					std::cout << "%" << tnow << ", %" << now - 1 << std::endl;
				}
				else {
					std::cout << "%" << tnow << ", " << rret << std::endl;
				}
			}
			else {
				int rret = r_exp->Dump();
				std::cout << "  %" << now << " = ";
				if (eqop == "==") {
					std::cout << "eq ";
				}
				else if (eqop == "!=") {
					std::cout << "ne ";
				}
				if (rret == -1) {
					std::cout << eret << ", %" << now - 1 << std::endl;
				}
				else {
					std::cout << eret << ", " << rret << std::endl;
				}
			}
			now++;
			return -1;
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
	int Dump() const override {
		if (andop == "&&") {
			int laret = la_exp->Dump();
			if (laret == -1) {
				int tnow = now - 1;
				int eret = e_exp->Dump();
				std::cout << "  %" << now << " = ne 0, %" << tnow << std::endl;
				now++;
				if (eret == -1) {
					std::cout << "  %" << now << " = ne 0, %" << now - 2 << std::endl;
				}
				else {
					std::cout << "  %" << now << " = ne 0, " << eret << std::endl;
				}
				now++;
				std::cout << "  %" << now << " = and %" << now - 1 << ", %" << now - 2 << std::endl;
			}
			else {
				int eret = e_exp->Dump();
				std::cout << "  %" << now << " = ne 0, " << laret << std::endl;
				now++;
				if (eret == -1) {
					std::cout << "  %" << now << " = ne 0, %" << now - 2 << std::endl;
				}
				else {
					std::cout << "  %" << now << " = ne 0, " << eret << std::endl;
				}
				now++;
				std::cout << "  %" << now << " = and %" << now - 1 << ", %" << now - 2 << std::endl;
			}
			now++;
			return -1;
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
	int Dump() const override {
		if (orop == "||") {
			int loret = lo_exp->Dump();
			if (loret == -1) {
				int tnow = now - 1;
				int laret = la_exp->Dump();
				std::cout << "  %" << now << " = or ";
				if (laret == -1) {
					std::cout << "%" << tnow << ", %" << now - 1 << std::endl;
				}
				else {
					std::cout << "%" << tnow << ", " << laret << std::endl;
				}
				now++;
				std::cout << "  %" << now << " = ne %" << now - 1 << ", 0" << std::endl;
			}
			else {
				int laret = la_exp->Dump();
				std::cout << "  %" << now << " = or ";
				if (laret == -1) {
					std::cout << loret << ", %" << now - 1 << std::endl;
				}
				else {
					std::cout << loret << ", " << laret << std::endl;
				}
				now++;
				std::cout << "  %" << now << " = ne %" << now - 1 << ", 0" << std::endl;
			}
			now++;
			return -1;
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
	int Dump() const override {
		// c_exp->Dump();
		return -1;
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

	int Dump() const override {
		// std::cout << "Number { " << IntConst << " }";
		return atoi(IntConst.c_str());
	}

	int Calc() const override {
		return atoi(IntConst.c_str());
	}
	SymbolList* FindSym() const override{

		return NULL;
	}
};
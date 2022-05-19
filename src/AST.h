#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <cassert>

// 解决单赋值问题
static int now = 0;

typedef struct {
	std::string name;
	int value, status;
}Symbol;

struct SymbolList{
	Symbol sym;
	SymbolList* next;
};

static SymbolList* list = NULL;

inline int FindSymbolValue(std::string s) {
	SymbolList* tmp = list;
	while (tmp != NULL) {
		if (tmp->sym.name == s) {
			return tmp->sym.value;
		}
		tmp = tmp->next;
	}
	assert(tmp == NULL);
	return -1;
}

inline Symbol InsertSymbol(std::string s, int value, int status) {
	SymbolList* p = list;
	if (list == NULL) {
		list = new SymbolList;
		list->next = NULL;
		list->sym.name = s;
		list->sym.value = value;
		list->sym.status = status;
		return list->sym;
	}
	while (p->next != NULL) p = p->next;
	SymbolList* tmp = new SymbolList;
	tmp->next = NULL;
	tmp->sym.name = s;
	tmp->sym.value = value;
	tmp->sym.status = status;
	p->next = tmp;
	return tmp->sym;
}

class BaseAST {
public:
	virtual ~BaseAST() = default;

	virtual int Dump() const = 0;

	virtual int Calc() const = 0;
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
};

class DeclAST : public BaseAST {
public:
	std::unique_ptr<BaseAST> const_decl;

	int Dump() const override {
		const_decl->Dump();
		return -1;
	}

	int Calc() const override {
		
		return -1;
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
};

class ConstDefAST : public BaseAST {
public:
	std::string name;
	std::unique_ptr<BaseAST> const_init_val;
	int Dump() const override {
		auto val = const_init_val->Calc();
		InsertSymbol(name, val, 1);
		return -1;
	}
	int Calc() const override {

		return -1;
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
};

class FuncTypeAST : public BaseAST {
public:

	int Dump() const override {
		// std::cout << "FuncTypeAST { " << INT << " }";
		std::cout << "i32 ";
		return -1;
	}

	int Calc() const override {
		
		return -1;
	}
};

class BlockAST : public BaseAST {
public:
	std::unique_ptr<BaseAST> block_items;

	int Dump() const override {
		// std::cout << "Block { ";
		// stmt->Dump();
		// std::cout << " }";
		std::cout << "{" << std::endl << "%entry:" << std::endl;
		block_items->Dump();
		return -1;
	}

	int Calc() const override {

		return -1;
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
};

class StmtAST : public BaseAST {
public:
	std::unique_ptr<BaseAST> exp;
	
	int Dump() const override {
		// std::cout << "Stmt { ";
		// number->Dump();
		// std::cout << " }";
		int ret = exp->Dump();
		if (ret == -1) {
			std::cout << "  ret %" << now - 1 << std::endl << "}";
		}
		else {
			std::cout << "  ret " << ret << std::endl << "}";
		}
		return -1;
	}
	
	int Calc() const override {

		return -1;
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
};

class LValAST : public BaseAST {
public:
	std::string name;
	int Dump() const override {
		return FindSymbolValue(name);
	}
	int Calc() const override {
		return FindSymbolValue(name);
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
};
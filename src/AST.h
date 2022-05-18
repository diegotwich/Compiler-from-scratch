#pragma once

#include <iostream>
#include <string>

// 解决单赋值问题
static int now = 0;

class BaseAST {
public:
	virtual ~BaseAST() = default;

	virtual int Dump() const = 0;
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
};

class FuncTypeAST : public BaseAST {
public:

	int Dump() const override {
		// std::cout << "FuncTypeAST { " << INT << " }";
		std::cout << "i32 ";
		return -1;
	}
};

class BlockAST : public BaseAST {
public:
	std::unique_ptr<BaseAST> stmt;

	int Dump() const override {
		// std::cout << "Block { ";
		// stmt->Dump();
		// std::cout << " }";
		std::cout << "{" << std::endl << "%entry:" << std::endl;
		stmt->Dump();
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
};

class ExpAST : public BaseAST {
public:
	std::unique_ptr<BaseAST> addexp;

	int Dump() const override {
		return addexp->Dump();
	}
};

class PrimaryExpAST : public BaseAST {
public:
	std::unique_ptr<BaseAST> p_exp;
	int Dump() const override {
		return p_exp->Dump();
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
};

class NumberAST : public BaseAST {
public:
	std::string IntConst;

	int Dump() const override {
		// std::cout << "Number { " << IntConst << " }";
		return atoi(IntConst.c_str());
	}
};
%code requires {
	#include <memory>
	#include <string>
	#include "AST.h"
}

%{

#include <iostream>
#include <memory>
#include <string>
#include "AST.h"

int yylex();
void yyerror(std::unique_ptr<BaseAST> &ast, const char *s);

using namespace std;

%}

/*
因为容易忘记所有的语法，所以记录在这里
CompUnit  ::= FuncDef;

FuncDef   ::= FuncType IDENT "(" ")" Block;
FuncType  ::= "int";

Block     ::= "{" Stmt "}";
Stmt      ::= "return" Exp ";";
Exp		  ::= AddExp;
PrimaryExp::= "(" Exp ")" | Number;
Number    ::= INT_CONST;
UnaryExp  ::= PrimaryExp | UnaryOp UnaryExp;
UnaryOp   ::= "+" | "-" | "|";
MulExp    ::= UnaryExp | MulExp ("*" | "/" | "%") UnaryExp;
AddExp    ::= MulExp | AddExp ("+" | "-") MulExp;
*/

// 定义 parser 函数和错误处理函数的附加参数
%parse-param { std::unique_ptr<BaseAST> &ast }


// yylval定义
%union {
	std::string *str_val;
	int int_val;
	BaseAST *ast_val;
}

// lexer返回的所有token类型
%token INT RETURN
%token <str_val> IDENT UnaryOp AddOp MulOp
%token <int_val> INT_CONST

// 非终结符的类型定义
%type <ast_val> FuncDef FuncType Block Stmt Number Exp PrimaryExp UnaryExp AddExp MulExp

%%

CompUnit
	: FuncDef {
	  auto comp_unit = make_unique<CompUnitAST>();
	  comp_unit->func_def = unique_ptr<BaseAST>($1);
	  ast = move(comp_unit);
	}
	;

FuncDef
	: FuncType IDENT '(' ')' Block {
		auto ast = new FuncDefAST();
		ast->func_type = unique_ptr<BaseAST>($1);
		ast->ident = *unique_ptr<string>($2);
		ast->block = unique_ptr<BaseAST>($5);
		$$ = ast;
	}
	;

FuncType
	: INT {
	  auto ast = new FuncTypeAST();
	  $$ = ast;
	}
	;

Block
	: '{' Stmt '}' {
	  auto ast = new BlockAST();
	  ast->stmt = unique_ptr<BaseAST>($2);
	  $$ = ast;
	}
	;

Stmt
	: RETURN Exp ';' {
	  auto ast = new StmtAST();
	  ast->exp = unique_ptr<BaseAST>($2);
	  $$ = ast;
	}
	;
	
Exp
	: AddExp {
	  auto ast = new ExpAST();
	  ast->addexp = unique_ptr<BaseAST>($1);
	  $$ = ast;
	}
	;

PrimaryExp
	: '(' Exp ')' {
	  auto ast = new PrimaryExpAST();
	  ast->p_exp = unique_ptr<BaseAST>($2);
	  $$ = ast;
	} | Number {
	  auto ast = new PrimaryExpAST();
	  ast -> p_exp = unique_ptr<BaseAST>($1);
	  $$ = ast;
	}
	;

UnaryExp
	: PrimaryExp {
	  auto ast = new UnaryExpAST();
	  ast->u_exp = unique_ptr<BaseAST>($1);
	  $$ = ast;
	} | UnaryOp UnaryExp {
	  auto ast = new UnaryExpAST();
	  ast->unaryop = *($1);
	  ast->u_exp = unique_ptr<BaseAST>($2);
	  $$ = ast;
	}
	;

MulExp
	: UnaryExp {
	  auto ast = new MulExpAST();
	  ast->u_exp = unique_ptr<BaseAST>($1);
	  $$ = ast;
	} | MulExp MulOp UnaryExp {
	  auto ast = new MulExpAST();
	  ast->m_exp = unique_ptr<BaseAST>($1);
	  ast->mulop = *($2);
	  ast->u_exp = unique_ptr<BaseAST>($3);
	  $$ = ast;
	}

AddExp
	: MulExp {
	  auto ast = new AddExpAST();
	  ast->m_exp = unique_ptr<BaseAST>($1);
	  $$ = ast;
	} | AddExp AddOp MulExp {
	  auto ast = new AddExpAST();
	  ast->a_exp = unique_ptr<BaseAST>($1);
	  ast->addop = *($2);
	  ast->m_exp = unique_ptr<BaseAST>($3);
	  $$ = ast;
	}

Number
	: INT_CONST {
	  auto ast = new NumberAST();
	  string tmp = to_string($1);
	  ast->IntConst = tmp;
	  $$ = ast;
	}
	;

%%

void yyerror(unique_ptr<BaseAST> &ast, const char *s) {
	cerr << "error: " << s << endl;
}
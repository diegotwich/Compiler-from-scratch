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
CompUnit      ::= FuncDef;

Decl          ::= ConstDecl | VarDecl;
ConstDecl     ::= "const" BType ConstDef {"," ConstDef} ";";
BType         ::= "int";
ConstDef      ::= IDENT "=" ConstInitVal;
ConstInitVal  ::= ConstExp;
VarDecl		  ::= BType VarDef {"," VarDef} ";";
VarDef        ::= IDENT | IDENT "=" InitVal;
InitVal       ::= Exp;

FuncDef       ::= FuncType IDENT "(" ")" Block;
FuncType      ::= "int";

Block         ::= "{" BlockItems "}";
BlockItems    ::= BlockItem BlockItems | null;
BlockItem     ::= Decl | Stmt;

Stmt		  ::= MatchedStmt | OpenStmt;
MatchedStmt   ::= LVal "=" Exp ";" | [Exp] | Block | "return" [Exp] ";" | "if" "(" Exp ")" "else" MatchedStmt | "while" "(" Exp ")" Stmt;
OpenStmt	  ::= "if" "(" Exp ")" Stmt | "if" "(" Exp ")" MatchedStmt "else" OpenStmt;
Exp			  ::= LOrExp;
LVal          ::= IDENT;
PrimaryExp	  ::= "(" Exp ")" | LVal | Number;
Number	      ::= INT_CONST;
UnaryExp      ::= PrimaryExp | UnaryOp UnaryExp;
UnaryOp       ::= "+" | "-" | "!";
MulExp        ::= UnaryExp | MulExp ("*" | "/" | "%") UnaryExp;
AddExp        ::= MulExp | AddExp ("+" | "-") MulExp;
RelExp        ::= AddExp | RelExp ("<" | ">" | "<=" | ">=") AddExp;
EqExp         ::= RelExp | EqExp ("==" | "!=") RelExp;
LAndExp       ::= EqExp | LAndExp "&&" EqExp;
LOrExp        ::= LAndExp | LOrExp "||" LAndExp;
ConstExp      ::= Exp;
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
%token INT RETURN CONST IF ELSE WHILE BREAK CONTINUE
%token <str_val> IDENT RelOp EqOp AndOp OrOp
%token <int_val> INT_CONST

// 非终结符的类型定义
%type <ast_val> FuncDef FuncType Block Stmt MatchedStmt OpenStmt Number Exp PrimaryExp UnaryExp AddExp MulExp RelExp EqExp LAndExp LOrExp Decl ConstDecl BType ConstDef  ConstDefs ConstInitVal BlockItems BlockItem LVal ConstExp VarDecl VarDefs VarDef InitVal

%%

CompUnit
	: FuncDef {
	  auto comp_unit = make_unique<CompUnitAST>();
	  comp_unit->func_def = unique_ptr<BaseAST>($1);
	  ast = move(comp_unit);
	}
	;

Decl
	: ConstDecl {
	  auto ast = new DeclAST();
	  ast->decl = unique_ptr<BaseAST>($1);
	  $$ = ast;
	} | VarDecl {
	  auto ast = new DeclAST();
	  ast->decl = unique_ptr<BaseAST>($1);
	  $$ = ast;
	}
	;

ConstDecl
	: CONST BType ConstDef ConstDefs ';' {
	  auto ast = new ConstDeclAST();
	  ast->btype = unique_ptr<BaseAST>($2);
	  ast->const_def = unique_ptr<BaseAST>($3);
	  ast->const_defs = unique_ptr<BaseAST>($4);
	  $$ = ast;
	}
	;

BType
	: INT {
	  auto ast = new BTypeAST();
	  ast->type = "int";
	  $$ = ast;
	}
	;

ConstDefs
	: ',' ConstDef ConstDefs {
	  auto ast = new ConstDefsAST();
	  ast->const_def = unique_ptr<BaseAST>($2);
	  ast->const_defs = unique_ptr<BaseAST>($3);
	  $$ = ast;
	} | /* NULL */ {
	  auto ast = new ConstDefsAST();
	  ast->exist = 0;
	  $$ = ast;
	}
	;

ConstDef
	: IDENT '=' ConstInitVal {
	  auto ast = new ConstDefAST();
	  ast->name = *($1);
	  ast->const_init_val = unique_ptr<BaseAST>($3);
	  $$ = ast;
	}
	;

ConstInitVal
	: ConstExp {
	  auto ast = new ConstInitValAST();
	  ast->const_exp = unique_ptr<BaseAST>($1);
	  $$ = ast;
	}
	;

VarDecl
	: BType VarDef VarDefs ';' {
	  auto ast = new VarDeclAST();
	  ast->btype = unique_ptr<BaseAST>($1);
	  ast->var_def = unique_ptr<BaseAST>($2);
	  ast->var_defs = unique_ptr<BaseAST>($3);
	  $$ = ast;
	}
	;

VarDefs
	: ',' VarDef VarDefs {
	  auto ast = new VarDefsAST();
	  ast->var_def = unique_ptr<BaseAST>($2);
	  ast->var_defs = unique_ptr<BaseAST>($3);
	  $$ = ast;
	} | /* NULL */ {
	  auto ast = new VarDefsAST();
	  ast->exist = 0;
	  $$ = ast;
	}
	;

VarDef
	: IDENT {
	  auto ast = new VarDefAST();
	  ast->name = *($1);
	  ast->initialized = 0;
	  $$ = ast;
	} | IDENT '=' InitVal {
	  auto ast = new VarDefAST();
	  ast->name = *($1);
	  ast->init_val = unique_ptr<BaseAST>($3);
	  $$ = ast;
	}
	;

InitVal
	: Exp {
	  auto ast = new InitValAST();
	  ast->exp = unique_ptr<BaseAST>($1);
	  $$ = ast;
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
	: '{' BlockItems '}' {
	  auto ast = new BlockAST();
	  ast->block_items = unique_ptr<BaseAST>($2);
	  $$ = ast;
	}
	;

BlockItems
	: BlockItem BlockItems {
	  auto ast = new BlockItemsAST();
	  ast->block_item = unique_ptr<BaseAST>($1);
	  ast->block_items = unique_ptr<BaseAST>($2);
	  $$ = ast;
	} | /* NULL */ {
	  auto ast = new BlockItemsAST();
	  ast->exist = 0;
	  $$ = ast;
	}
	;

BlockItem
	: Decl {
	  auto ast = new BlockItemAST();
	  ast->bi_ptr = unique_ptr<BaseAST>($1);
	  $$ = ast;
	} | Stmt {
	  auto ast = new BlockItemAST();
	  ast->bi_ptr = unique_ptr<BaseAST>($1);
	  $$ = ast;
	}
	;

Stmt
	: MatchedStmt {
	  auto ast = new StmtAST();
	  ast->stmt = unique_ptr<BaseAST>($1);
	  $$ = ast;
	} | OpenStmt {
	  auto ast = new StmtAST();
	  ast->stmt = unique_ptr<BaseAST>($1);
	  $$ = ast;
	}
	;

MatchedStmt
	: LVal '=' Exp ';' {
	  auto ast = new MatchedStmtAST();
	  ast->type = 0;
	  ast->l_val = unique_ptr<BaseAST>($1);
	  ast->exp = unique_ptr<BaseAST>($3);
	  $$ = ast;
	} | Exp ';' {
	  auto ast = new MatchedStmtAST();
	  ast->type = 2;
	  ast->exp = unique_ptr<BaseAST>($1);
	  $$ = ast;
	} | /* NULL */ ';' {
	  auto ast = new MatchedStmtAST();
	  ast->type = 2;
	  ast->exist = 0;
	  $$ = ast;
	} | RETURN Exp ';' {
	  auto ast = new MatchedStmtAST();
	  ast->type = 1;
	  ast->exp = unique_ptr<BaseAST>($2);
	  $$ = ast;
	} | RETURN /* NULL */ ';' {
	  auto ast = new MatchedStmtAST();
	  ast->type = 1;
	  ast->exist = 0;
	  $$ = ast;
	} | Block {
	  auto ast = new MatchedStmtAST();
	  ast->type = 3;
	  ast->exp = unique_ptr<BaseAST>($1);
	  $$ = ast;
	} | IF '(' Exp ')' MatchedStmt ELSE MatchedStmt {
	  auto ast = new MatchedStmtAST();
	  ast->type = 4;
	  ast->exp = unique_ptr<BaseAST>($3);
	  ast->l_val = unique_ptr<BaseAST>($5);
	  ast->m_stmt = unique_ptr<BaseAST>($7);
	  $$ = ast;
	} | WHILE '(' Exp ')' Stmt {
	  auto ast = new MatchedStmtAST();
	  ast->type = 5;
	  ast->exp = unique_ptr<BaseAST>($3);
	  ast->l_val = unique_ptr<BaseAST>($5);
	  $$ = ast;
	} | BREAK ';' {
	  auto ast = new MatchedStmtAST();
	  ast->type = 6;
	  $$ = ast;
	} | CONTINUE ';' {
	  auto ast = new MatchedStmtAST();
	  ast->type = 7;
	  $$ = ast;
	}
	;
	
OpenStmt
	: IF '(' Exp ')' Stmt {
	  auto ast = new OpenStmtAST();
	  ast->exp = unique_ptr<BaseAST>($3);
	  ast->m_stmt = unique_ptr<BaseAST>($5);
	  $$ = ast;
	} | IF '(' Exp ')' MatchedStmt ELSE OpenStmt {
	  auto ast = new OpenStmtAST();
	  ast->type = 1;
	  ast->exp = unique_ptr<BaseAST>($3);
	  ast->m_stmt = unique_ptr<BaseAST>($5);
	  ast->o_stmt = unique_ptr<BaseAST>($7);
	  $$ = ast;
	}
	;

Exp
	: LOrExp {
	  auto ast = new ExpAST();
	  ast->lorexp = unique_ptr<BaseAST>($1);
	  $$ = ast;
	}
	;

LVal
	: IDENT {
	  auto ast = new LValAST();
	  ast->name = *($1);
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
	  ast->p_exp = unique_ptr<BaseAST>($1);
	  $$ = ast;
	} | LVal {
	  auto ast = new PrimaryExpAST();
	  ast->p_exp = unique_ptr<BaseAST>($1);
	  $$ = ast;
	}
	;

UnaryExp
	: PrimaryExp {
	  auto ast = new UnaryExpAST();
	  ast->u_exp = unique_ptr<BaseAST>($1);
	  $$ = ast;
	} | '-' UnaryExp {
	  auto ast = new UnaryExpAST();
	  ast->unaryop = "-";
	  ast->u_exp = unique_ptr<BaseAST>($2);
	  $$ = ast;
	} | '+' UnaryExp {
	  auto ast = new UnaryExpAST();
	  ast->unaryop = "+";
	  ast->u_exp = unique_ptr<BaseAST>($2);
	  $$ = ast;
	} | '!' UnaryExp {
	  auto ast = new UnaryExpAST();
	  ast->unaryop = "!";
	  ast->u_exp = unique_ptr<BaseAST>($2);
	  $$ = ast;
	}
	;

MulExp
	: UnaryExp {
	  auto ast = new MulExpAST();
	  ast->u_exp = unique_ptr<BaseAST>($1);
	  $$ = ast;
	} | MulExp '*' UnaryExp {
	  auto ast = new MulExpAST();
	  ast->m_exp = unique_ptr<BaseAST>($1);
	  ast->mulop = "*";
	  ast->u_exp = unique_ptr<BaseAST>($3);
	  $$ = ast;
	} | MulExp '/' UnaryExp {
	  auto ast = new MulExpAST();
	  ast->m_exp = unique_ptr<BaseAST>($1);
	  ast->mulop = "/";
	  ast->u_exp = unique_ptr<BaseAST>($3);
	  $$ = ast;
	} | MulExp '%' UnaryExp {
	  auto ast = new MulExpAST();
	  ast->m_exp = unique_ptr<BaseAST>($1);
	  ast->mulop = "%";
	  ast->u_exp = unique_ptr<BaseAST>($3);
	  $$ = ast;
	}
	;

AddExp
	: MulExp {
	  auto ast = new AddExpAST();
	  ast->m_exp = unique_ptr<BaseAST>($1);
	  $$ = ast;
	} | AddExp '+' MulExp {
	  auto ast = new AddExpAST();
	  ast->a_exp = unique_ptr<BaseAST>($1);
	  ast->addop = "+";
	  ast->m_exp = unique_ptr<BaseAST>($3);
	  $$ = ast;
	} | AddExp '-' MulExp {
	  auto ast = new AddExpAST();
	  ast->a_exp = unique_ptr<BaseAST>($1);
	  ast->addop = "-";
	  ast->m_exp = unique_ptr<BaseAST>($3);
	  $$ = ast;
	}
	;

RelExp
	: AddExp {
	  auto ast = new RelExpAST();
	  ast->a_exp = unique_ptr<BaseAST>($1);
	  $$ = ast;
	} | RelExp RelOp AddExp {
	  auto ast = new RelExpAST();
	  ast->r_exp = unique_ptr<BaseAST>($1);
	  ast->relop = *($2);
	  ast->a_exp = unique_ptr<BaseAST>($3);
	  $$ = ast;
	}
	;

EqExp
	: RelExp {
	  auto ast = new EqExpAST();
	  ast->r_exp = unique_ptr<BaseAST>($1);
	  $$ = ast;
	} | EqExp EqOp RelExp {
	  auto ast = new EqExpAST();
	  ast->e_exp = unique_ptr<BaseAST>($1);
	  ast->eqop = *($2);
	  ast->r_exp = unique_ptr<BaseAST>($3);
	  $$ = ast;
	}
	;

LAndExp
	: EqExp {
	  auto ast = new LAndExpAST();
	  ast->e_exp = unique_ptr<BaseAST>($1);
	  $$ = ast;
	} | LAndExp AndOp EqExp {
	  auto ast = new LAndExpAST();
	  ast->la_exp = unique_ptr<BaseAST>($1);
	  ast->andop = *($2);
	  ast->e_exp = unique_ptr<BaseAST>($3);
	  $$ = ast;
	}
	;

LOrExp
	: LAndExp {
	  auto ast = new LOrExpAST();
	  ast->la_exp = unique_ptr<BaseAST>($1);
	  $$ = ast;
	} | LOrExp OrOp LAndExp {
	  auto ast = new LOrExpAST();
	  ast->lo_exp = unique_ptr<BaseAST>($1);
	  ast->orop = *($2);
	  ast->la_exp = unique_ptr<BaseAST>($3);
	  $$ = ast;
	}
	;

ConstExp
	: Exp {
	  auto ast = new ConstExpAST();
	  ast->c_exp = unique_ptr<BaseAST>($1);
	  $$ = ast;
	}
	;

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
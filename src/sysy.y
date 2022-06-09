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
CompUnit      ::= [CompUnit] (Decl | FuncDef);

变量的type和函数的type有移进规约冲突，合并：
Type		  ::= "int" | "void"

Decl          ::= ConstDecl | VarDecl;
ConstDecl     ::= "const" Type ConstDef {"," ConstDef} ";";
ConstDef      ::= IDENT ["[" ConstExp "]"] "=" ConstInitVal;
ConstInitVal  ::= ConstExp | "{" [ConstInitVal {"," ConstInitVal] "}";
VarDecl		  ::= Type VarDef {"," VarDef} ";";
VarDef        ::= IDENT {"[" ConstExp "]"} | IDENT {"[" ConstExp "]"} "=" InitVal;
InitVal       ::= Exp | "{" [InitVal {"," InitVal}] "}";

FuncDef       ::= FuncType IDENT "(" [FuncFParams] ")" Block;
FuncFParams   ::= FuncFParam {"," FuncFParam};
FuncFParam	  ::= Type IDENT;	

Block         ::= "{" BlockItems "}";
BlockItems    ::= BlockItem BlockItems | null;
BlockItem     ::= Decl | Stmt;

Stmt		  ::= MatchedStmt | OpenStmt;
MatchedStmt   ::= LVal "=" Exp ";" | [Exp] | Block | "return" [Exp] ";" | "if" "(" Exp ")" "else" MatchedStmt | "while" "(" Exp ")" Stmt;
OpenStmt	  ::= "if" "(" Exp ")" Stmt | "if" "(" Exp ")" MatchedStmt "else" OpenStmt;
Exp			  ::= LOrExp;
LVal          ::= IDENT {"[" Exp "]"};
PrimaryExp	  ::= "(" Exp ")" | LVal | Number;
Number	      ::= INT_CONST;
UnaryExp      ::= PrimaryExp | UnaryOp UnaryExp | IDENT "(" [FuncRParam] ")";
FuncRParam   ::= Exp {"," Exp};
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
%token INT RETURN CONST IF ELSE WHILE BREAK CONTINUE VOID
%token <str_val> IDENT RelOp EqOp AndOp OrOp
%token <int_val> INT_CONST

// 非终结符的类型定义
%type <ast_val> CompUnit FuncDef Type Block Stmt MatchedStmt OpenStmt Number Exp Exps PrimaryExp UnaryExp AddExp MulExp RelExp EqExp LAndExp LOrExp
%type <ast_val> Decl ConstDecl ConstDef  ConstDefs ConstInitVal BlockItems BlockItem LVal ConstExp ConstExps VarDecl VarDefs VarDef InitVal
%type <ast_val> FuncFParams FuncFParam FuncRParam InitVals ConstInitVals

%%
PreComp
	: CompUnit {
	  auto pre_comp = make_unique<PreCompAST>();
	  pre_comp->comp_unit = unique_ptr<BaseAST>($1);
	  ast = move(pre_comp);
	}
	;

CompUnit
	: FuncDef {
	  auto ast = new CompUnitAST();
	  ast->func_def = unique_ptr<BaseAST>($1);
	  ast->exist = 0;
	  $$ = ast;
	} | CompUnit FuncDef {
	  auto ast = new CompUnitAST();
	  ast->comp = unique_ptr<BaseAST>($1);
	  ast->func_def = unique_ptr<BaseAST>($2);
	  $$ = ast;
	} | Decl {
	  auto ast = new CompUnitAST();
	  ast->func_def = unique_ptr<BaseAST>($1);
	  ast->exist = 0;
	  $$ = ast;
	} | CompUnit Decl{
	  auto ast = new CompUnitAST();
	  ast->comp = unique_ptr<BaseAST>($1);
	  ast->func_def = unique_ptr<BaseAST>($2);
	  $$ = ast;
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
	: CONST Type ConstDef ConstDefs ';' {
	  auto ast = new ConstDeclAST();
	  ast->type = unique_ptr<BaseAST>($2);
	  ast->const_def = unique_ptr<BaseAST>($3);
	  ast->const_defs = unique_ptr<BaseAST>($4);
	  $$ = ast;
	}
	;

Type
	: INT {
	  auto ast = new TypeAST();
	  ast->type = "int";
	  $$ = ast;
	} | VOID {
	  auto ast = new TypeAST();
	  ast->type = "void";
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
	} | IDENT '[' ConstExp ']' ConstExps '=' ConstInitVal {
	  auto ast = new ConstDefAST();
	  ast->is_array = 1;
	  ast->name = *($1);
	  ast->const_exp = unique_ptr<BaseAST>($3);
	  ast->const_exps = unique_ptr<BaseAST>($5);
	  ast->const_init_val = unique_ptr<BaseAST>($7);
	  $$ = ast;
	}
	;

ConstInitVal
	: ConstExp {
	  auto ast = new ConstInitValAST();
	  ast->const_exp = unique_ptr<BaseAST>($1);
	  $$ = ast;
	} | '{' ConstInitVal ConstInitVals '}'{
	  auto ast = new ConstInitValAST();
	  ast->exist = 2;
	  ast->const_exp = unique_ptr<BaseAST>($2);
	  ast->const_exps = unique_ptr<BaseAST>($3);
	  $$ = ast;
	} | '{' '}' {
	  auto ast = new ConstInitValAST();
	  ast->exist = 0;
	  $$ = ast;
	}
	;

ConstInitVals
	: ',' ConstInitVal ConstInitVals {
	  auto ast = new ConstInitValsAST();
	  ast->const_init_val = unique_ptr<BaseAST>($2);
	  ast->const_init_vals = unique_ptr<BaseAST>($3);
	  ast->exist = 1;
	  $$ = ast;
	} | /* NULL */ {
	  auto ast = new ConstInitValsAST();
	  $$ = ast;
	}
	;

ConstExps
	: '[' ConstExp ']' ConstExps {
	  auto ast = new ConstExpsAST();
	  ast->exist = 1;
	  ast->const_exp = unique_ptr<BaseAST>($2);
	  ast->const_exps = unique_ptr<BaseAST>($4);
	  $$ = ast;
	} | /* NULL */ {
	  auto ast = new ConstExpsAST();
	  $$ = ast;
	}
	;

VarDecl
	: Type VarDef VarDefs ';' {
	  auto ast = new VarDeclAST();
	  ast->type = unique_ptr<BaseAST>($1);
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
	} | IDENT '[' ConstExp ']' ConstExps {
	  auto ast = new VarDefAST();
	  ast->is_array = 1;
	  ast->initialized = 0;
	  ast->name = *($1);
	  ast->const_exp = unique_ptr<BaseAST>($3);
	  ast->const_exps = unique_ptr<BaseAST>($5);
	  $$ = ast;
	} | IDENT '[' ConstExp ']' ConstExps '=' InitVal {
	  auto ast = new VarDefAST();
	  ast->is_array = 1;
	  ast->name = *($1);
	  ast->const_exp = unique_ptr<BaseAST>($3);
	  ast->const_exps = unique_ptr<BaseAST>($5);
	  ast->init_val = unique_ptr<BaseAST>($7);
	  $$ = ast;
	}
	;

InitVal
	: Exp {
	  auto ast = new InitValAST();
	  ast->init_val = unique_ptr<BaseAST>($1);
	  $$ = ast;
	} | '{' InitVal InitVals '}' {
	  auto ast = new InitValAST();
	  ast->exist = 2;
	  ast->init_val = unique_ptr<BaseAST>($2);
	  ast->init_vals = unique_ptr<BaseAST>($3);
	  $$ = ast;
	} | '{' '}'{
	  auto ast = new InitValAST();
	  ast->exist = 0;
	  $$ = ast;
	}
	;

InitVals
	: ',' InitVal InitVals {
	  auto ast = new InitValsAST();
	  ast->init_val = unique_ptr<BaseAST>($2);
	  ast->init_vals = unique_ptr<BaseAST>($3);
	  ast->exist = 1;
	  $$ = ast;
	} | /* NULL */ {
	  auto ast = new InitValsAST();
	  $$ = ast;
	}
	;

Exps
	: '[' Exp ']' Exps {
	  auto ast = new ExpsAST();
	  ast->exist = 1;
	  ast->exp = unique_ptr<BaseAST>($2);
	  ast->exps = unique_ptr<BaseAST>($4);
	  $$ = ast;
	} | /* NULL */ {
	  auto ast = new ExpsAST();
	  $$ = ast;
	}
	;

FuncDef
	: Type IDENT '(' ')' Block {
		auto ast = new FuncDefAST();
		ast->type = unique_ptr<BaseAST>($1);
		ast->ident = *($2);
		ast->block = unique_ptr<BaseAST>($5);
		$$ = ast;
	} | Type IDENT '(' FuncFParams ')' Block {
		auto ast = new FuncDefAST();
		ast->type = unique_ptr<BaseAST>($1);
		ast->ident = *($2);
		ast->fparams = unique_ptr<BaseAST>($4);
		ast->block = unique_ptr<BaseAST>($6);
		ast->param_exist = 1;
		$$ = ast;
	}
	;

FuncFParams
	: FuncFParam {
	  auto ast = new FuncFParamsAST();
	  ast->exist = 0;
	  ast->fparam = unique_ptr<BaseAST>($1);
	  $$ = ast;
	} | FuncFParam ',' FuncFParams {
	  auto ast = new FuncFParamsAST();
	  ast->fparam = unique_ptr<BaseAST>($1);
	  ast->fparams = unique_ptr<BaseAST>($3);
	  $$ = ast;
	}
	;

FuncFParam
	: Type IDENT {
	  auto ast = new FuncFParamAST();
	  ast->ident = *($2);
	  ast->type = unique_ptr<BaseAST>($1);
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
	} | IDENT '[' Exp ']' Exps {
	  auto ast = new LValAST();
	  ast->is_array = 1;
	  ast->name = *($1);
	  ast->exp = unique_ptr<BaseAST>($3);
	  ast->exps = unique_ptr<BaseAST>($5);
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
	} | IDENT '(' ')'{
	  auto ast = new UnaryExpAST();
	  ast->unaryop = "Func0";
	  ast->ident = *($1);
	  $$ = ast;
	} | IDENT '(' FuncRParam ')' {
	  auto ast = new UnaryExpAST();
	  ast->unaryop = "Func1";
	  ast->ident = *($1);
	  ast->rparam = unique_ptr<BaseAST>($3);
	  $$ = ast;
	}
	;

FuncRParam
	: Exp {
	  auto ast = new FuncRParamAST();
	  ast->exist = 0;
	  ast->exp = unique_ptr<BaseAST>($1);
	  $$ = ast;
	} | Exp ',' FuncRParam {
	  auto ast = new FuncRParamAST();
	  ast->exp = unique_ptr<BaseAST>($1);
	  ast->rparam = unique_ptr<BaseAST>($3);
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
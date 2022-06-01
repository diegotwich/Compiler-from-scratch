#include <cassert>
#include <fstream>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include "AST.h"
#include "generator.h"
#include <sstream>
#include <fstream>
#include <cstring>

using namespace std;

extern FILE* yyin;
extern int yyparse(unique_ptr<BaseAST>& ast);

int main(int argc, const char *argv[]) {
  assert(argc == 5);
  auto mode = argv[1];
  auto input = argv[2];
  auto output = argv[4];

  yyin = fopen(input, "r");
  assert(yyin);

  unique_ptr<BaseAST> ast;
  auto ret = yyparse(ast);
  assert(!ret);
  // 输出
  if (strcmp(mode, "-riscv") == 0) {
	  freopen("KoopaOut.txt", "w", stdout);

	  // dump AST
	  ast->Dump();
	  cout << endl;

	  // 将Koopa写入到str中
	  string str;
	  ifstream fin;
	  fin.open("KoopaOut.txt", ios::in);
	  stringstream buf;
	  buf << fin.rdbuf();
	  str = buf.str();
	  fin.close();

	  // 处理str
	  freopen(output, "w", stdout);
	  const char* tmp = str.c_str();
	  parse_str(tmp);
  }
  else if (strcmp(mode, "-koopa") == 0) {
	  freopen(output, "w", stdout);
	  
	  // dump AST
	  ast->Dump();
	  cout << endl;
  }
  return 0;
}

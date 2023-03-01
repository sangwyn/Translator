#ifndef TRANSLATOR__SYNTAXANALYZER_H_
#define TRANSLATOR__SYNTAXANALYZER_H_

#include <string>
#include <fstream>
#include <vector>
#include <string>
#include <exception>
#include <stack>
#include <map>

struct TID_t {
  std::string type, name;
  bool is_const;
};

struct TID_node{
  std::vector<TID_t> TID;
  TID_node* parent = nullptr;

  std::vector<TID_node*> next ; //для отладки
};

class SyntaxAnalyzer {
 private:


  std::vector<std::string> lexemes;
  std::vector<int> types, lines;
  std::string cur;
  int cur_type;
  int ind = -1;
  bool in_preread = false;
  bool var_decl_const = false;
  std::string var_decl_type = "";
  int cycle_counter = 0;

  TID_node* now = new TID_node(); //тид отвечающих за глобал
  std::map<std::string,std::vector<std::string>> function;
  std::stack<std::string> types_stack;

  //семантика которую пока некуда деть
  void Push_id(int cur_id);
  void NewScope();
  void BackToScope();
  void PreReadLexemes();
  void PushFunction(int cur_id);
  void PushStack(std::string str);
  void CheckBin(int cur_id);
  void CheckUn(int cur_id);

  std::string GetType(int cur_id);
  bool IsConst(int cur_id);
  //

  bool IsType(std::string s);
  bool IsAssOp(std::string s) {
      return (s == "=" || s == "+=" || s == "-=" || s == "*=" || s == "/=" || s == "%="
          || s == "//=");
  }
  void ReadLexemes();
  void ReadNext();

  void Program();

  void Variable();

  void Operator();
  void VarDeclaration();
  void Assignment();
  void Block();
  void IfOperator();
  void WhileOperator();
  void ForOperator();
  void Input();
  void Print();
  void Return();
  void Break();
  void Continue();

  void Type();
  void Arguments();
  void GiveArguments();
  void Function();

  void IsLiteral();
  void LiteralList();

  void Priority10();
  void Priority9();
  void Operation8();
  void Priority8();
  void Operation7();
  void Priority7();
  void Operation6();
  void Priority6();
  void Operation5();
  void Priority5();
  void Operation4();
  void Priority4();
  void Priority3();
  void Priority2();
  void Priority1();

  void Expression();

 public:
  void Analyze();
};

#endif

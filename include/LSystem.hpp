#include "proj.h"

#ifndef __WIND_D0LSYSTEM
#define __WIND_D0LSYSTEM

namespace {
using namespace std;

void test_for_using() { cout << "ok" << endl; }

using LRule = map<string, string>;

struct Psym{  // 每一个带参符号，都由符号名、参数列表(参数指针，参数个数)构成
  string sym_name;
  float *params;    // 完全由类维护
  uint32_t num;

  Psym():sym_name(""),param(nullptr),num(0){}
  Psym(const string &name, const vector<float> &params):sym_name(name),param(new float[params.size()]),num(params.size()){}
  ~Psym(){
    delete [] params;
    this->params = nullptr;
    this->num = 0;
    this->sym_name = "";
  }
};

class LSystem {
  unordered_set<string> syms;
  string axiom;
  LRule rules;

  string current_state;   // 迭代的当前状态

  LSystem(const unordered_set<string> &syms, const string &axiom,
          const LRule &rules) {
    this->syms = syms;
    this->axiom = axiom;
    this->rules = rules;
  }

  string next() {
    this->current_state = this->iterate(this->current_state);
    return this->current_state;
  }

  void reset(){
    this->current_state = this->axiom;
  }

  virtual string iterative(const string &p) {
    auto input = lexy::string_input(p.c_str(), p.length());

    return "";
  };
};

} // namespace

#endif
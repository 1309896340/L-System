#include "proj.h"

#ifndef __WIND_D0LSYSTEM
#define __WIND_LSYSTEM

namespace {

using namespace std;
using LRule = map<string, string>;

void test_for_using() { cout << "ok" << endl; }

struct Psym { // 每一个带参符号，都由符号名、参数列表(参数指针，参数个数)构成
  string sym_name;
  float *params; // 完全由类维护
  uint32_t num;

  Psym() : sym_name(""), params(nullptr), num(0) {}
  Psym(const string &name, const vector<float> &params)
      : sym_name(name), params(new float[params.size()]), num(params.size()) {}
  ~Psym() {
    delete[] params;
    this->params = nullptr;
    this->num = 0;
    this->sym_name = "";
  }
};

class LSystem {
  unordered_set<string> sym_nonparam;   // 不带参数的符号
  string axiom;
  LRule rules;    

  vector<Psym> sym_mapper;

  string current_state; // 迭代的当前状态

public:
  LSystem(const string &axiom, const LRule &rules) {
    this->axiom = axiom;
    this->rules = rules;

    // 遍历rules，从中提取key的符号名、实参数量、所有实参值，生成Psym对象
    
  }

  string next() {
    this->current_state = this->iterate(this->current_state);
    return this->current_state;
  }

  void reset() { this->current_state = this->axiom; }

  virtual string iterate(const string &p) {
    auto input = lexy::string_input(p.c_str(), p.length());

    return "";
  };
};

} // namespace LSys

#endif
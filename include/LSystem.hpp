#include "proj.h"

#ifndef __WIND_D0LSYSTEM
#define __WIND_LSYSTEM

#include "lexy_parser.hpp"

namespace {

using namespace std;
// using LRule = map<string, string>;
using LProduction = unordered_set<string>;

void test_for_using() { cout << "ok" << endl; }


class LSystem {
  unordered_set<string> sym_nonparam; // 不带参数的符号
  string axiom;
  LProduction prod;

  map<string, config::Sym> sym_mapper;

  string current_state; // 迭代的当前状态

public:
  LSystem(const string &axiom, const LProduction &prod) {
    this->axiom = axiom;
    this->prod = prod;

    printf("进入构造函数\n");
    // 遍历prod，从中提取key的符号名、实参数量、所有实参值，生成Psym对象
    for (auto &p : prod) {
      printf("调试，待解析式：%s\n", p.c_str());
      auto lexy_string = lexy::zstring_input(p.c_str());
      // 继续编写
    }
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

} // namespace

#endif
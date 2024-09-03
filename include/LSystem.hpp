#include "proj.h"

#ifndef __WIND_D0LSYSTEM
#define __WIND_LSYSTEM

#include "LSysParser.hpp"

namespace LSystem{

using namespace std;
// using LRule = map<string, string>;
using LProduction = unordered_set<string>;

void test_for_using() { cout << "ok" << endl; }


class D0L_System {

  string axiom;
  string current_state; // 迭代的当前状态

public:
  LSysParser::config::LSystem lsystem;
  
  D0L_System(const string &axiom, const string &productions):axiom(axiom),current_state(axiom),lsystem() {
    auto lsys_prods_string = lexy::string_input(productions);
    auto lsys_res = lexy::parse<LSysParser::grammar::LSystem>(lsys_prods_string, lexy_ext::report_error);
    assert(lsys_res.is_success());
    // const LSysParser::config::LSystem &lsys = lsys_res.value();
    this->lsystem = lsys_res.value();
  }

  string next() {
    this->current_state = this->iterate(this->current_state);
    return this->current_state;
  }

  void reset() { this->current_state = this->axiom; }

  virtual string iterate(const string &p) {
    auto input_string = lexy::string_input(p);
    auto input_res = lexy::parse<LSysParser::grammar::LSysCall>(input_string, lexy_ext::report_error);
    assert(input_res.is_success());
    const LSysParser::config::LSysCall & syscall = input_res.value();
    return syscall.apply(this->lsystem);
  };
};

} // namespace

#endif
#include "proj.h"

#ifndef __WIND_D0LSYSTEM
#define __WIND_D0LSYSTEM

namespace {
using namespace std;
namespace dsl = lexy::dsl;

void test_for_using() { cout << "ok" << endl; }

using LRule = map<string, string>;

struct LSystem {
  vector<string> varnames;
  string axiom;
  LRule rules;
  LSystem(const vector<string> &varnames, const string &axiom,
          const LRule &rules) {
    this->varnames = varnames;
    this->axiom = axiom;
    this->rules = rules;
  }
  string iterative(const string &p) {
    auto input = lexy::string_input(p.c_str(),p.length());
    
  };
};

// struct D0L : public LSystem {

//   D0L(const vector<string> &varnames, const string &axiom, const LRule
//   &rules)
//       : LSystem(varnames,axiom,rules){}
// };

} // namespace

#endif
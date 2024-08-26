#include "proj.h"

#ifndef __WIND_D0LSYSTEM
#define __WIND_D0LSYSTEM

namespace {
using namespace std;

void test_for_using() { cout << "ok" << endl; }

using LRule = map<string, string>;

struct LSystem {
  unordered_set<string> varnames;
  string axiom;
  LRule rules;


  LSystem(const unordered_set<string> &varnames, const string &axiom,
          const LRule &rules) {
    this->varnames = varnames;
    this->axiom = axiom;
    this->rules = rules;
  }
  string iterative(const string &p) {
    auto input = lexy::string_input(p.c_str(),p.length());
    

    return "";
  };

};

}

#endif
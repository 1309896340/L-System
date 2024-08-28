#include "LSystem.hpp"

using namespace std;

int main(int argc, char **argv) {

  vector<string> test_s = {
    "F(a,b,c)",
    "F(a,b, c)",
    "F(a,b ,c)"
  };

  for(string &s:test_s){
    printf("调试字符串： %s\n",s.c_str());
    auto s_input = lexy::zstring_input(s.c_str());
    lexy::validate<SymDefineExpr>(s_input, lexy_ext::report_error);
  }


  return 0;
}

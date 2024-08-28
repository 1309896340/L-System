#include "proj.h"

#ifndef __WIND_PARSER
#define __WIND_PERSER

#include "lexy/callback.hpp"
#include "lexy/dsl.hpp"

namespace {
using namespace std;

// =================================值构造========================================
struct Psym { // 每一个带参符号，都由符号名、参数列表(参数指针，参数个数)构成
  string sym_name;
  optional<vector<string>> params; // 形式参数的名字

  Psym() : sym_name(""), params({}) {}
  Psym(const string &name, const optional<vector<string>> &params)
      : sym_name(name), params(params) {}
};

struct Number {
  int int_part;
  optional<int> fraction_part;
  Number(const int a, const optional<int> &b) : int_part(a), fraction_part(b) {}
  float value() const {
    float res = static_cast<float>(int_part);
    if (fraction_part.has_value()) {
      float tmp = static_cast<float>(fraction_part.value());
      while (tmp > 0)
        tmp /= 10.0f;
      res += tmp;
    }
    return res;
  }
};

// // 函数由函数名、参数列表(实参名，所有实参的表达式) 构成
// using Pfunc = map<string, unordered_set<string, string>>;

// =================================解析规则========================================
namespace dsl = lexy::dsl;
struct SymName {
  static constexpr auto rule = dsl::identifier(
      dsl::ascii::alpha / dsl::lit_c<'_'>, dsl::ascii::alnum / dsl::lit_c<'_'>);
  static constexpr auto value = lexy::as_string<string>;
};

struct ParameterItem {
  static constexpr auto whitespace = dsl::ascii::space; // 设置自动跳空格
  static constexpr auto rule = dsl::identifier(
      dsl::ascii::alpha / dsl::lit_c<'_'>, dsl::ascii::alnum / dsl::lit_c<'_'>);
  static constexpr auto value = lexy::as_string<string>;
};

struct ParameterList {
  static constexpr auto rule =
      dsl::parenthesized.list(dsl::p<ParameterItem>, dsl::sep(dsl::comma));
  static constexpr auto value = lexy::as_list<vector<string>>;
};

struct Sym {
  static constexpr auto rule = [] {
    auto sym_name =
        dsl::p<SymName>; // 匹配带括号的形式参数列表，例如 "(abc,aaa,bcd,aab)"
    auto bracket_opt = dsl::opt(
        dsl::p<ParameterList>); // 如果不带参数，则会产生一个 dsl::nullopt
    return sym_name + bracket_opt;
  }();
  // 需要将解析出来的内容，注入到Psym中去
  static constexpr auto value = lexy::construct<Psym>;
};

struct NumberProduction {
  static constexpr auto rule = dsl::integer<int> + dsl::opt(dsl::lit_c<'.'> >> dsl::integer<int>);
  static constexpr auto value = lexy::construct<Number>;
};

struct MathExpr : lexy::expression_production {           // 目前这套规则没有实现带括号的表达式嵌套，但可以先尝试一下是否匹配
  static constexpr auto whitespace = dsl::ascii::space;
  static constexpr auto atom = dsl::p<Number>;     // 由于含有变量，atom应当指定为一个分支，除了Number以外，还可以是 ParameterItem

  struct A_Expr : dsl::infix_op_left{
    static constexpr auto op = dsl::op(dsl::lit_c<'+'>) / dsl::op(dsl::lit_c<'-'>);
    using operand = dsl::atom;
  };

  struct B_Expr : dsl::infix_op_left{
    static constexpr auto op = dsl::op(dsl::lit_c<'*'>) / dsl::op(dsl::lit_c<'/'>);
    using operand = A_Expr;
  };

};

struct SymMap {
  // 调用时的表达式，对应实际参数Argument
  // 与定义处不一样，实参可以是较为复杂的表达式，带优先级的四则运算以及初等函数
  // 表达式的描述会非常复杂，包含数值常量(整型、浮点)，包含四则运算以及单目运算
  // 这些都是好解决的，问题在于解析出的这些数据应该如何描述？
  // 再SymDefineExpr中主要描述了映射的签名部分，这里需要描述映射的主体
};
} // namespace

#endif

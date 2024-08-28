#include "proj.h"

#ifndef __WIND_PARSER
#define __WIND_PERSER

#include "lexy/callback.hpp"
#include "lexy/dsl.hpp"

namespace {
using namespace std;

// =================================值构造========================================
// struct Psym {  // 每一个带参符号，都由符号名、参数列表(参数指针，参数个数)构成
//     string sym_name;
//     // float* params;  // 完全由类维护
//     // uint32_t num;
//     unordered_set<string, float> params;  // 使用无序集合存储参数值

//     Psym()
//         : sym_name(""), params({}) {}
//     Psym(const string& name, const vector<float>& params)
//         : sym_name(name), params(new float[params.size()]), num(params.size()) {}
//     ~Psym() {
//         delete[] params;
//         this->params = nullptr;
//         this->num = 0;
//         this->sym_name = "";
//     }
// };

  // 函数由函数名、参数列表(实参名，所有实参的表达式) 构成
  using Pfunc = map<string, unordered_set<string, string>>;

// =================================解析规则========================================
namespace dsl = lexy::dsl;
struct SymName {
    // 符号的名称，字母或下划线开头，其余部分字母、数字、下划线组合
    static constexpr auto rule = dsl::identifier(dsl::ascii::alpha / dsl::lit_c<'_'>,dsl::ascii::alnum / dsl::lit_c<'_'>);
    static constexpr auto value = lexy::as_string<std::string>;
};

struct Parameter {
    // 形式参数的格式，暂时和SymName一样
    static constexpr auto whitespace = dsl::ascii::space;   // 设置自动跳空格
    static constexpr auto rule = dsl::identifier(dsl::ascii::alpha / dsl::lit_c<'_'>, dsl::ascii::alnum / dsl::lit_c<'_'>);
    static constexpr auto value = lexy::as_string<std::string>;
};

struct SymDefineExpr {
    // 定义处的符号表达式，以SymName为开头，后接参数列表，也可以无参
    // 对应形式参数Parameter
    static constexpr auto rule = [] {
        auto bracket_list = dsl::parenthesized.list(dsl::p<Parameter>, dsl::sep(dsl::comma));  // 匹配带括号的形式参数列表，例如 "(abc,aaa,bcd,aab)"
        auto bracket_opt = dsl::opt(bracket_list);                                             // 如果不带参数，则会产生一个 dsl::nullopt
        return dsl::p<SymName> + bracket_opt;
    }();
    // 需要将解析出来的内容，注入到Psym中去
    // static constexpr auto value =
};

struct SymCallExpr{
  // 调用时的表达式，对应实际参数Argument
  // 与定义处不一样，实参可以是较为复杂的表达式，带优先级的四则运算以及初等函数

};
}  // namespace

#endif

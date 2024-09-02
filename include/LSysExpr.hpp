#include "proj.h"

#ifndef __WIND_LSYSEXPR
#define __WIND_LSYSEXPR

#include "MathExpr.hpp"

namespace LSysParser {
using namespace std;

// namespace ast {
// using expr_ptr = shared_ptr<struct Expr>;

// struct Expr {
//     ~Expr() = default;
//     virtual string evaluate
// };

// };  // namespace ast

namespace config {
struct LSysCall {
    string name;
    // vector<MathParser::ast::expr_ptr> args; //在创建时进行计算
    vector<float> args;  // 在构造时进行计算
    LSysCall(string name, vector<MathParser::ast::expr_ptr> args_expr)
        : name(LEXY_MOV(name)), args{0} {
        printf("测试LSysCall中对vector的初始化列表是否正常，size=%d，如果正确应该没有输出：\n", this->args.size());
        for (auto& ele : this->args)
            printf("%f\n", ele);

        // 初始化中计算表达式值
        MathParser::ast::Environment env;
        for(auto &expr : args_expr)
            args.push_back(expr->evaluate(env));
        
    }
};
};  // namespace config

namespace grammar {
namespace dsl = lexy::dsl;

struct SymName {
    static constexpr auto rule = dsl::identifier(dsl::ascii::alpha_underscore, dsl::ascii::alpha_digit_underscore);
    static constexpr auto value = lexy::as_string<string>;
};

struct ArgExpr {
    static constexpr auto rule = dsl::p<MathParser::grammar::MathExpr>;
    static constexpr auto value = lexy::construct<MathParser::ast::expr_ptr>;
};

struct ArgList {
    static constexpr auto rule = dsl::parenthesized.list(dsl::p<ArgExpr>, dsl::comma);
    static constexpr auto value = lexy::as_list<vector<MathParser::ast::expr_ptr>>;  // 在构造的过程中被计算
};

struct LSysCall {
    static constexpr auto rule = [] {
        auto symName = dsl::p<SymName>;
        auto arglist = dsl::p<ArgList>;
        return symName + arglist;  // 这里暂时规定SymName后面即便没有参数也必须带括号
    }();
};


struct LSysDefine{

};

// struct expected_operand{
//     static constexpr auto name = "expected operand";
// };

// struct LSysExpr{
//     static constexpr auto whitespace = dsl::ascii::space;
//     static constexpr auto rule = [] {
//         auto call_expr = dsl::p<LSysCall>;
//         auto define_prefix = LEXY_LIT("define");    // 需要有一个前缀来区分定义式，这里规定为 "define"
//         auto define_expr = define_prefix >> dsl::p<LSysDefine>;
//         return call_expr |  define_expr | dsl::error<expected_operand>;
//     }();

//     // using operation = atom;
// };



};  // namespace grammar

};  // namespace LSysParser

#endif

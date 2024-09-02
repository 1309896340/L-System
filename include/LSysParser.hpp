#include "proj.h"

#ifndef __WIND_LSYSEXPR
#define __WIND_LSYSEXPR

#include "MathExprParser.hpp"

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
struct LProdCall {
    string name;
    vector<float> args;  // 在构造时进行计算
    LProdCall(string name, vector<MathParser::ast::expr_ptr> args_expr)
        : name(LEXY_MOV(name)), args{0} {
        printf("测试LSysCall中对vector的初始化列表是否正常，size=%d，如果正确应该没有输出：\n", this->args.size());
        for (auto& ele : this->args)
            printf("%f\n", ele);

        // 初始化中计算表达式值
        MathParser::ast::Environment env;
        for (auto& expr : args_expr)
            args.push_back(expr->evaluate(env));  // 计算出实参并入栈
    }

    string evaluate() const {
        // 未完成
        return "";
    }
};

struct SymSrc {
    string name;
    vector<string> params;
    SymSrc(string name, vector<string> paramName)
        : name(LEXY_MOV(name)), params(LEXY_MOV(params)) {}
};

struct SymDst {
    string name;
    vector<MathParser::ast::expr_ptr> paramMaps;
};

struct LProduction {
    SymSrc sSrc;
    vector<SymDst> sDsts;
    LProduction(SymSrc sSrc, vector<SymDst> sDsts)
        : sSrc(LEXY_MOV(sSrc)), sDsts(LEXY_MOV(sDsts)) {}
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
    static constexpr auto rule = dsl::parenthesized.list(dsl::p<ArgExpr>, dsl::sep(dsl::comma));
    static constexpr auto value = lexy::as_list<vector<MathParser::ast::expr_ptr>>;  // 在构造的过程中被计算
};

struct LProdCall {
    static constexpr auto rule = [] {
        auto symName = dsl::p<SymName>;
        auto arglist = dsl::p<ArgList>;
        return symName + arglist;  // 这里暂时规定SymName后面即便没有参数也必须带括号
    }();
};

struct Param {
    static constexpr auto rule = dsl::identifier(dsl::ascii::alpha_underscore, dsl::ascii::alpha_digit_underscore);
    static constexpr auto value = lexy::as_string<string>;
};

struct ParamList {
    static constexpr auto rule = dsl::parenthesized.list(dsl::p<Param>, dsl::sep(dsl::comma));
    static constexpr auto value = lexy::as_list<vector<string>>;
};

struct SymSrc {
    static constexpr auto rule = dsl::p<SymName> + dsl::p<ParamList>;
    static constexpr auto value = lexy::construct<config::SymSrc>;
};

struct ParamMap {
    static constexpr auto rule = dsl::p<MathParser::grammar::MathExpr>;
    static constexpr auto value = lexy::construct<MathParser::ast::expr_ptr>;
};

struct ParamMapList {
    static constexpr auto rule = dsl::parenthesized.list(dsl::p<ParamMap>, dsl::sep(dsl::comma));
    static constexpr auto value = lexy::as_list<vector<MathParser::ast::expr_ptr>>;
};

struct SymDst {
    static constexpr auto rule = dsl::p<SymName> + dsl::p<ParamMapList>;
    static constexpr auto value = lexy::construct<config::SymDst>;
};

struct SymDstList {
    static constexpr auto rule = dsl::list(dsl::peek(dsl::p<SymName>) >> dsl::p<SymDst>);       // 考虑不做分隔符区分
    static constexpr auto value = lexy::as_list<vector<config::SymDst>>;
};

struct LProduction {
    static constexpr auto whitespace = dsl::ascii::space;
    static constexpr auto rule = dsl::p<SymSrc> + LEXY_LIT("->") +  dsl::p<SymDstList>;
    static constexpr auto value = lexy::construct<config::LProduction>;
};

};  // namespace grammar

};  // namespace LSysParser

#endif

#include "proj.h"

#define DEBUG

#ifndef __WIND_LSYSEXPR
#define __WIND_LSYSEXPR

#include "MathExprParser.hpp"

namespace LSysParser {
using namespace std;

namespace config {
struct SymSrc {
    string name;
    vector<string> params;
    SymSrc() = default;
    SymSrc(string name, vector<string> paramNames)
        : name(LEXY_MOV(name)), params(LEXY_MOV(paramNames)) {}
};

struct SymDst {
    string name;
    SymDst() = default;
    vector<MathParser::ast::expr_ptr> paramMaps;
};

struct LProduction {
    SymSrc sSrc;
    vector<SymDst> sDsts;
    LProduction() = default;
    LProduction(SymSrc sSrc, vector<SymDst> sDsts)
        : sSrc(LEXY_MOV(sSrc)), sDsts(LEXY_MOV(sDsts)) {}
};

struct LSystem {
    map<string, LProduction> prods;
    LSystem() = default;
    LSystem(map<string, LProduction> prods)
        : prods(LEXY_MOV(prods)) {}
    LSystem(vector<LProduction> prods)
        : prods() {
        for (const LProduction& prod : prods)
            this->prods[prod.sSrc.name] = prod;
    }
};

struct LProdCall {
    string name;
    vector<float> args;  // 在构造时进行计算
    LProdCall(string name, vector<MathParser::ast::expr_ptr> args_expr)
        : name(LEXY_MOV(name)), args() {
        assert(this->args.size() == 0);  // 初始化为空列表

        // 初始化中计算表达式值
        MathParser::ast::Environment env;
        for (auto& expr : args_expr)
            args.push_back(expr->evaluate(env));  // 计算出实参并入栈
    }

    string apply(const config::LSystem& lsys) const {
        stringstream ss;

        auto iter = lsys.prods.find(this->name);
        if (iter == lsys.prods.end()) {
            ss << this->name << '(';
            for (float arg : this->args)
                ss << arg << ',';
            ss.seekp(-1, ss.cur);
            ss << ')';

            // printf("未知的产生式: \"%s\"，保留原始格式输出 \"%s\"\n", this->name.c_str(), ss.str().c_str());
            return ss.str();
        }
        // 找到对应的LProduction
        const config::LProduction &lprod = iter->second;
        // 找到形参名列表
        const vector<string> &paramNames = lprod.sSrc.params;
        // 检查形参与实参的数量
        const int &argNum = this->args.size(), &paramNum = paramNames.size();
        // 创建ParamMap的局部作用域MathParser::ast::Environment，并初始化传入参数。这里只考虑值传递，不实现引用传递
        MathParser::ast::Environment env;
        auto p_iter = paramNames.begin(), p_end = paramNames.end();
        auto a_iter = this->args.begin(), a_end = this->args.end();
        while(p_iter!=p_end){
            if(a_iter==a_end){ 
                // 填充arg不足param的部分
                env.vars[*p_iter] = 0.0f;
            }else{  
                // 相同数量按顺序传递
                env.vars[*p_iter] = *a_iter;
                a_iter++;
            }
            p_iter++;
            // 忽略arg超出param的部分
        }  

        for(const config::SymDst &sDst: lprod.sDsts){
            if(sDst.name=="B"){
                // 调试断点
                printf("");
            }
            ss << sDst.name << '(';
            for(const MathParser::ast::expr_ptr &paramExpr : sDst.paramMaps)
                ss << paramExpr->evaluate(env) << ',';
            ss.seekp(-1, ss.cur);
            ss << ')';
        }

        return ss.str();
    }
};

struct LSysCall {
    vector<LProdCall> calls;
    LSysCall(vector<LProdCall> calls)
        : calls(LEXY_MOV(calls)) {}
    string apply(const LSystem& lsys) const {
        stringstream ss;
        for (auto& call : calls)
            ss << call.apply(lsys);
        return ss.str();
    }
};

};  // namespace config

namespace grammar {
namespace dsl = lexy::dsl;

struct SymName {
    static constexpr auto rule = dsl::identifier(dsl::ascii::alpha, dsl::ascii::alpha_digit_underscore);
    static constexpr auto value = lexy::as_string<string>;
};

struct ArgExpr {
    static constexpr auto rule = dsl::p<MathParser::grammar::MathExpr>;
    static constexpr auto value = lexy::construct<MathParser::ast::expr_ptr>;
    // static constexpr auto value = lexy::forward<MathParser::ast::expr_ptr>;
};

struct ArgList {
    static constexpr auto whitespace = dsl::ascii::space;
    static constexpr auto rule = dsl::parenthesized.list(dsl::p<ArgExpr>, dsl::sep(dsl::comma));
    static constexpr auto value = lexy::as_list<vector<MathParser::ast::expr_ptr>>;  // 在构造的过程中被计算
};

struct LProdCall {
    static constexpr auto rule = [] {
        auto symName = dsl::p<SymName>;
        auto arglist = dsl::p<ArgList>;
        return symName + arglist;  // 这里暂时规定SymName后面即便没有参数也必须带括号
    }();
    static constexpr auto value = lexy::construct<config::LProdCall>;
};

struct LProdCallList {
    static constexpr auto whitespace = dsl::ascii::blank;
    static constexpr auto rule = dsl::list(dsl::peek(dsl::p<SymName>) >> dsl::p<LProdCall>);
    static constexpr auto value = lexy::as_list<vector<config::LProdCall>>;
};

struct Param {
    static constexpr auto rule = dsl::identifier(dsl::ascii::alpha, dsl::ascii::alpha_digit_underscore);
    static constexpr auto value = lexy::as_string<string>;
};

struct ParamList {
    static constexpr auto whitespace = dsl::ascii::space;
    static constexpr auto rule = dsl::parenthesized.list(dsl::p<Param>, dsl::sep(dsl::comma));
    static constexpr auto value = lexy::as_list<vector<string>>;
};

struct SymSrc {
    static constexpr auto rule = dsl::p<SymName> + dsl::p<ParamList>;
    static constexpr auto value = lexy::construct<config::SymSrc>;
};

struct ParamMap {
    static constexpr auto rule = dsl::p<MathParser::grammar::MathExpr>;
    static constexpr auto value = lexy::forward<MathParser::ast::expr_ptr>;
};

struct ParamMapList {
    static constexpr auto whitespace = dsl::ascii::blank;
    static constexpr auto rule = dsl::parenthesized.list(dsl::p<ParamMap>, dsl::sep(dsl::comma));
    static constexpr auto value = lexy::as_list<vector<MathParser::ast::expr_ptr>>;
};

struct SymDst {
    static constexpr auto rule = dsl::p<SymName> + dsl::p<ParamMapList>;
    static constexpr auto value = lexy::construct<config::SymDst>;
};

struct SymDstList {
    static constexpr auto whitespace = dsl::ascii::blank;
    static constexpr auto rule = dsl::list(dsl::peek(dsl::p<SymName>) >> dsl::p<SymDst>);  // 考虑不做分隔符区分
    static constexpr auto value = lexy::as_list<vector<config::SymDst>>;
};

struct LProduction {
    static constexpr auto whitespace = dsl::ascii::blank;
    static constexpr auto rule = dsl::p<SymSrc> + LEXY_LIT("->") + dsl::p<SymDstList>;
    static constexpr auto value = lexy::construct<config::LProduction>;
};

struct LSystem {
    static constexpr auto whitespace = dsl::ascii::blank;
    static constexpr auto rule = dsl::list(dsl::p<LProduction>, dsl::sep(dsl::newline));  // 通过换行来分割LProduction字符串
    static constexpr auto value = lexy::as_list<vector<config::LProduction>>;
};

struct LSysCall {
    static constexpr auto whitespace = dsl::ascii::blank;
    static constexpr auto rule = dsl::p<LProdCallList>;
    static constexpr auto value = lexy::construct<config::LSysCall>;
};

};  // namespace grammar

};  // namespace LSysParser

#endif

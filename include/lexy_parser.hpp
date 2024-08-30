#include "proj.h"

#ifndef __WIND_PARSER
#define __WIND_PERSER

#include "lexy/callback.hpp"
#include "lexy/dsl.hpp"

namespace {
using namespace std;

namespace config {
struct Sym {  // 每一个带参符号，都由符号名、参数列表(参数指针，参数个数)构成
    string sym_name;
    optional<vector<string>> params;  // 形式参数的名字

    Sym()
        : sym_name(""), params({}) {}
    Sym(const string& name, const optional<vector<string>>& params)
        : sym_name(name), params(params) {}
};

struct SymMap {
};

struct LProduction {
    // 以 F(x,y) -> A(x+y,x-y,x*y)F(y,x)B((x+y)/2) 为例
    // 一个L产生式由若干的SymMap来描述，SymMap的数量等于rhs中符号的数量，例如 "A" "F" "B"
    // 每个SymMap又由多个Expr组成，Expr描述了如何将lhs中的参数映射到对应SymMap中的每一项，Expr需要实现：
    // 1. 单目运算取负操作
    // 2. 带括号的加减乘除四则运算
    // 3. 其他可能的初等函数调用，例如sin() exp() pow(,) log()
    string name;  // 例如上述的"F"
    Sym sym;
    SymMap smap;
    // map<string, ast::Expr> sym;      // 例如上述的{{"x",ast{x+y}},{"y",ast{}}}
};

struct Number {
    int int_part;
    float fraction_part;
    Number(const int a, const optional<int>& b)
        : int_part(a), fraction_part(0) {
        if (b.has_value()) {
            fraction_part = static_cast<float>(b.value());
            while (fraction_part > 1.0f)
                fraction_part /= 10.0f;
        }
    }
    float value() const {
        return static_cast<float>(int_part) + fraction_part;
    }
};

}  // namespace config

namespace ast {
using expr_ptr = shared_ptr<struct Expr>;

struct Environment {
    map<string, float> vars{
        {"x", 1.0},
        {"y", 2.0},
        {"z", 3.0}};
};

struct Expr {
    // 表达式的接口，定义抽象evaluate()方法，以上下文environment作为参数
    virtual ~Expr() = default;
    virtual float evaluate(Environment& env) const = 0;
};

struct Expr_literal : Expr {
    float val;
    Expr_literal(float val)
        : val(val) {}
    Expr_literal(const int a, const optional<int>& b)
        : val(config::Number(a, b).value()) {
    }
    virtual float evaluate(ast::Environment& env) const {
        return this->val;
    }
};

struct Expr_var : Expr {
    string varname;
    Expr_var(string varname)
        : varname(LEXY_MOV(varname)) {};
    // 大坑，identifier产生的是lexy::string_lexeme，而不能直接转换为lexy::string，所以上面的构造是失效的
    // Expr_var(lexy::string_lexeme<lexy::ascii_encoding> lexeme):varname(lexeme.data(),lexeme.size()){}
    Expr_var(lexy::string_lexeme<lexy::ascii_encoding> lexeme)
        : varname(lexy::as_string<string>(lexeme)) {}
    virtual float evaluate(Environment& env) const {
        auto iter = env.vars.find(varname);
        return (iter == env.vars.end()) ? 0 : iter->second;
    }
};

struct Expr_unary : Expr {  // 单目运算
    enum op_t {
        negate
    } op;
    expr_ptr rhs;

    explicit Expr_unary(op_t op, expr_ptr e)
        : op(op), rhs(LEXY_MOV(e)) {}

    virtual float evaluate(Environment& env) const {
        float rhs_v = rhs->evaluate(env);
        switch (op) {
            case negate:
                return -rhs_v;
        }
        return rhs_v;
    }
};

struct Expr_binary : Expr {  // 双目运算
    enum op_t {
        plus,
        minus,
        multiply,
        divide
    } op;
    expr_ptr lhs, rhs;

    explicit Expr_binary(expr_ptr lhs, op_t op, expr_ptr rhs)
        : lhs(LEXY_MOV(lhs)), op(op), rhs(LEXY_MOV(rhs)) {}

    virtual float evaluate(Environment& env) const {
        float lhs_v = lhs->evaluate(env);
        float rhs_v = rhs->evaluate(env);
        switch (op) {
            case plus:
                return lhs_v + rhs_v;
            case minus:
                return lhs_v - rhs_v;
            case multiply:
                return lhs_v * rhs_v;
            case divide:
                return lhs_v / rhs_v;
        }
        return 0;
    }
};
};  // namespace ast

namespace grammar {
namespace dsl = lexy::dsl;
constexpr auto escaped_newline = dsl::backslash >> dsl::newline;
struct SymName {
    static constexpr auto rule = dsl::identifier(
        dsl::ascii::alpha / dsl::lit_c<'_'>,
        dsl::ascii::alnum / dsl::lit_c<'_'>);
    static constexpr auto value = lexy::as_string<string>;
};

struct ParamItemSrc {      // value产生string，用于左侧解析显示
    static constexpr auto rule = dsl::identifier(
        dsl::ascii::alpha / dsl::lit_c<'_'>,
        dsl::ascii::alnum / dsl::lit_c<'_'>);
    static constexpr auto value = lexy::as_string<string>;
};

struct ParamItem {      // value产生ast::Expr_var，用于右侧生成ast
    static constexpr auto rule = dsl::identifier(
        dsl::ascii::alpha / dsl::lit_c<'_'>,
        dsl::ascii::alnum / dsl::lit_c<'_'>);
    // static constexpr auto value = lexy::as_string<string>;
    static constexpr auto value = lexy::construct<ast::Expr_var>;
};

struct ParamList {
    static constexpr auto rule =
        dsl::parenthesized.list(dsl::p<ParamItemSrc>, dsl::sep(dsl::comma));
    static constexpr auto value = lexy::as_list<vector<string>>;
};

struct Sym {
    static constexpr auto rule = [] {
        auto sym_name =
            dsl::p<SymName>;  // 匹配带括号的形式参数列表，例如 "(abc,aaa,bcd,aab)"
        auto paramlist = dsl::opt(
            dsl::p<ParamList>);  // 如果不带参数，则会产生一个 dsl::nullopt
        return sym_name + paramlist;
    }();
    static constexpr auto value = lexy::construct<config::Sym>;
};


struct NumberD { // 用于解析显示
    static constexpr auto rule = dsl::integer<int> + dsl::opt(dsl::lit_c<'.'> >> dsl::integer<int>);
    // static constexpr auto value = lexy::construct<ast::Expr_literal>;
    static constexpr auto value = lexy::construct<config::Number>;
};

struct Number {
    static constexpr auto rule = dsl::integer<int> + dsl::opt(dsl::lit_c<'.'> >> dsl::integer<int>);
    static constexpr auto value = lexy::construct<ast::Expr_literal>;
    // static constexpr auto value = lexy::construct<config::Number>;
};


struct NestedExpr : lexy::transparent_production {
    static constexpr auto whitespace = dsl::ascii::space | escaped_newline;
    static constexpr auto rule = dsl::recurse<struct MathExpr>;
    static constexpr auto value = lexy::forward<ast::expr_ptr>;
};

struct MathExpr : lexy::expression_production {
    struct expected_operand {
        static constexpr auto name = "expected operand";
    };

    static constexpr auto whitespace = dsl::ascii::space;
    static constexpr auto atom = [] {
        auto param_name_prefix = dsl::peek(dsl::ascii::alpha / dsl::lit_c<'_'>);
        auto number_prefix = dsl::peek(dsl::ascii::digit);

        auto literal_number = number_prefix >> dsl::p<grammar::Number>; 
        auto defined_param_name = param_name_prefix >> dsl::p<grammar::ParamItem>;

        auto paren_expr = dsl::parenthesized(dsl::p<NestedExpr>);
        return paren_expr | literal_number | defined_param_name | dsl::error<expected_operand>;
    }();

    struct Item : dsl::infix_op_left {  // 乘除运算
        static constexpr auto op = dsl::op<ast::Expr_binary::multiply>(dsl::lit_c<'*'>) / dsl::op<ast::Expr_binary::divide>(dsl::lit_c<'/'>);
        using operand = dsl::atom;
    };

    struct Neg : dsl::prefix_op {  // 单目运算取相反数
        // 对ExprItem可能的取反
        static constexpr auto op = dsl::op<ast::Expr_unary::negate>(dsl::lit_c<'-'>);
        using operand = Item;
    };

    struct Expr : dsl::infix_op_left {  // 加减运算
        static constexpr auto op = dsl::op<ast::Expr_binary::plus>(dsl::lit_c<'+'>) / dsl::op<ast::Expr_binary::minus>(dsl::lit_c<'-'>);
        using operand = Neg;
    };

    using operation = Expr;

    static constexpr auto value = lexy::callback<ast::expr_ptr>(
        lexy::forward<ast::expr_ptr>,
        lexy::new_<ast::Expr_unary, ast::expr_ptr>,
        lexy::new_<ast::Expr_binary, ast::expr_ptr>,
        lexy::new_<ast::Expr_literal, ast::expr_ptr>,  // 字面量
        lexy::new_<ast::Expr_var, ast::expr_ptr>
    );
};

struct ParamMappedList {
    static constexpr auto rule = dsl::parenthesized.list(dsl::p<MathExpr>, dsl::sep(dsl::comma));
    static constexpr auto value = lexy::as_list<vector<ast::Expr>>;
};

struct SymMap {
    static constexpr auto rule = [] {
        auto sym_name = dsl::p<SymName>;
        auto param_list = dsl::opt(dsl::p<ParamMappedList>);
        return sym_name + param_list;
    }();
};

struct SymMapList {
    static constexpr auto rule = [] {
        auto symmap_prefix = dsl::peek(dsl::ascii::alpha_underscore);
        return dsl::list(symmap_prefix >> dsl::p<SymMap>);
    }();
};

struct LProduction {
    static constexpr auto whitespace = dsl::ascii::space;
    static constexpr auto rule = [] {
        auto lhs = dsl::p<Sym>;
        auto rhs = dsl::p<SymMapList>;
        return lhs + LEXY_LIT("->") + rhs;
    }();
};

}  // namespace grammar
}  // namespace

#endif

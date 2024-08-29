#include "proj.h"

#ifndef __WIND_PARSER
#define __WIND_PERSER

#include "lexy/callback.hpp"
#include "lexy/dsl.hpp"

namespace {
using namespace std;

namespace config {
struct Psym {  // 每一个带参符号，都由符号名、参数列表(参数指针，参数个数)构成
    string sym_name;
    optional<vector<string>> params;  // 形式参数的名字

    Psym()
        : sym_name(""), params({}) {}
    Psym(const string& name, const optional<vector<string>>& params)
        : sym_name(name), params(params) {}
};

struct Number {
    int int_part;
    // optional<int> fraction_part;
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

namespace grammar {
namespace dsl = lexy::dsl;
constexpr auto escaped_newline = dsl::backslash >> dsl::newline;
struct SymName {
    static constexpr auto rule = dsl::identifier(
        dsl::ascii::alpha / dsl::lit_c<'_'>,
        dsl::ascii::alnum / dsl::lit_c<'_'>);
    static constexpr auto value = lexy::as_string<string>;
};

struct ParamItem {
    // static constexpr auto whitespace = dsl::ascii::space;  // 设置自动跳空格
    static constexpr auto rule = dsl::identifier(
        dsl::ascii::alpha / dsl::lit_c<'_'>,
        dsl::ascii::alnum / dsl::lit_c<'_'>);
    static constexpr auto value = lexy::as_string<string>;
};

struct ParamList {
    static constexpr auto rule =
        dsl::parenthesized.list(dsl::p<ParamItem>, dsl::sep(dsl::comma));
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
    static constexpr auto value = lexy::construct<config::Psym>;
};

struct Number {
    static constexpr auto rule = dsl::integer<int> + dsl::opt(dsl::lit_c<'.'> >> dsl::integer<int>);
    static constexpr auto value = lexy::construct<config::Number>;
};

struct NestedExpr : lexy::transparent_production {
    static constexpr auto whitespace = dsl::ascii::space | escaped_newline;
    static constexpr auto rule = dsl::recurse<struct MathExpr>;
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
        auto defined_param_name = param_name_prefix >> dsl::p<ParamItem>;

        auto paren_expr = dsl::parenthesized(dsl::p<NestedExpr>);
        return paren_expr | literal_number | defined_param_name | dsl::error<expected_operand>;
    }();

    struct Item : dsl::infix_op_left {
        static constexpr auto op = dsl::op(dsl::lit_c<'*'>) / dsl::op(dsl::lit_c<'/'>);
        using operand = dsl::atom;
    };

    struct Neg : dsl::prefix_op {
        // 对ExprItem可能的取反
        static constexpr auto op = dsl::op(dsl::lit_c<'-'>);
        using operand = Item;
    };

    struct Expr : dsl::infix_op_left {
        static constexpr auto op = dsl::op(dsl::lit_c<'+'>) / dsl::op(dsl::lit_c<'-'>);
        using operand = Neg;
    };

    using operation = Expr;
};

struct ParamMappedList {
    static constexpr auto rule = dsl::parenthesized.list(dsl::p<MathExpr>, dsl::sep(dsl::comma));
};

struct SymMap {
    static constexpr auto rule = [] {
        auto sym_name = dsl::p<SymName>;
        auto param_list = dsl::opt(dsl::p<ParamMappedList>);
        return sym_name + param_list;
    }();
};

struct SymMapList {
    static constexpr auto rule = []{
        auto symmap_prefix = dsl::peek(dsl::ascii::alpha_underscore);
        return dsl::list(symmap_prefix >> dsl::p<SymMap>);
    }();
};

struct LProduction{
    static constexpr auto whitespace = dsl::ascii::space;
    static constexpr auto rule = []{
        auto lhs = dsl::p<Sym>;
        auto rhs = dsl::p<SymMapList>;
        return lhs + LEXY_LIT("->") + rhs;
    }();
};

}  // namespace grammar
}  // namespace

#endif

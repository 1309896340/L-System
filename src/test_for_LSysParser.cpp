// #define VISUALIZE_LPRODUCTION
// #define VISUALIZE_LPRODCALL

#include "LSysParser.hpp"
using namespace std;

int main(void) {
    auto input_rule = lexy::zstring_input("F(x,y) -> A(x+y,x-y,x*y)F(x/y,x+1)B((x+y+z)/3)");
    // auto parser_res = lexy::parse<LSysParser::grammar::LProduction>(input_rule, lexy_ext::report_error);
    auto parser_res = lexy::parse<LSysParser::grammar::LSystem>(input_rule, lexy_ext::report_error);
    assert(parser_res.is_success());
    const LSysParser::config::LSystem &lsys = parser_res.value();

// 可视化查看
#ifdef VISUALIZE_LPRODUCTION
    lexy::parse_tree_for<decltype(input_rule)> tree;
    auto res_vis = lexy::parse_as_tree<LSysParser::grammar::LProduction>(tree, input_rule, lexy_ext::report_error);
    assert(res_vis.is_success());
    lexy::visualize(stdout, tree, {lexy::visualize_fancy});
#endif


    auto input_string = lexy::zstring_input("K(0.1, 2.3+0.2, 3)F(1, 3, 6)X(0.5)");
    // auto call_res = lexy::parse<LSysParser::grammar::LProdCallList>(input_string, lexy_ext::report_error);
    auto call_res = lexy::parse<LSysParser::grammar::LSysCall>(input_string, lexy_ext::report_error);
    assert(call_res.is_success());
    const LSysParser::config::LSysCall &call_input_string = call_res.value();

    
#ifdef VISUALIZE_LPRODCALL
    lexy::parse_tree_for<decltype(input_string)> tree;
    auto res_vis = lexy::parse_as_tree<LSysParser::grammar::LProdCallList>(tree, input_string, lexy_ext::report_error);
    assert(res_vis.is_success());
    lexy::visualize(stdout, tree, {lexy::visualize_fancy});
#endif

    string res = call_input_string.apply(lsys);
    cout << "result: " << res << endl;

    return 0;
}

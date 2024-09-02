#include "LSysParser.hpp"

using namespace std;

int main(void){
    auto input_rule = lexy::zstring_input("F(x,y) -> A(x+y,x-y,x*y)F(x/y,x+1)B((x+y+z)/3)");
    // auto res = lexy::parse<LSysParser::grammar::LProduction>(input_rule, lexy_ext::report_error);

    lexy::parse_tree_for<decltype(input_rule)> tree;
    auto res = lexy::parse_as_tree<LSysParser::grammar::LProduction>(tree, input_rule, lexy_ext::report_error);
    assert(res.is_success());
    lexy::visualize(stdout, tree, {lexy::visualize_fancy});

    return 0;
}

#include "lexy_parser.hpp"
#include "proj.h"

using namespace std;

void test_for_key() {
    vector<string> test_key = {
        "F(a,b,c)",
        "Fa12(x12 ,y_32, _z)",
        "F_212(  alpha123, w_312 , 123abc)"};

    for (string& s : test_key) {
        printf("调试字符串： %s\n", s.c_str());
        auto s_input = lexy::zstring_input(s.c_str());
        // lexy::validate<SymDefineExpr>(s_input, lexy_ext::report_error);
        auto result = lexy::parse<grammar::Sym>(s_input, lexy_ext::report_error);

        bool isOk = result.is_success();
        if (!isOk)
            break;

        config::Psym p = result.value();
        bool hasParam = p.params.has_value();

        printf("符号名：%s\n", p.sym_name.c_str());
        printf("参数列表：%s\n", hasParam ? "" : "空");
        if (hasParam) {
            for (string& pv : p.params.value())
                printf("\"%s\"  ", pv.c_str());
            printf("\n");
        }
        printf("===========================================\n");
    }
}

void test_for_Number() {
    auto sinput = lexy::zstring_input("12");
    auto res = lexy::parse<grammar::Number>(sinput, lexy_ext::report_error);
    printf("是否成功：%d\n", res.is_success());
    config::Number val = res.value();
    printf("解析结果为  %.6f\n", val.value());
}

void test_for_value() {
    auto sinput = lexy::zstring_input("Func(-a+1, 0.3 *b , ( a+ b)/ 3.2- c)F2(1.4*a, -3/d, c*(- 3+ 4))");

    auto res = lexy::validate<grammar::SymName>(sinput, lexy_ext::report_error);
    printf("是否成功：%d\n", res.is_success());

    // 可视化解析结果
    lexy::parse_tree_for<decltype(sinput)> tree;
    auto result = lexy::parse_as_tree<grammar::SymMapList>(tree, sinput, lexy_ext::report_error);

    lexy::visualize(stdout, tree, {lexy::visualize_fancy});
}

int main(void) {
    // test_for_key();
    // test_for_Number();
    test_for_value();

    return 0;
}

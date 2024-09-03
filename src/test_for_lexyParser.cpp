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

        config::Sym p = result.value();
        bool hasParam = p.params.has_value();

        printf("符号名：%s\n", p.name.c_str());
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
    auto res = lexy::parse<grammar::NumberD>(sinput, lexy_ext::report_error);
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

void test_for_LProduction() {
    auto sinput = lexy::zstring_input("\
    F(x,y,z) -> A(x+1,-y*0.3,(x+y)/2-z)F(0.8*x,0.6*y,0.9*z)C((x+y+z)/2);\
    C(x) -> A(1,2*x,x-1)C(0.5*x)\
    ");

    // 可视化解析树
    // lexy::parse_tree_for<decltype(sinput)> tree;
    // auto res = lexy::parse_as_tree<grammar::LProduction>(tree, sinput, lexy_ext::report_error);
    // bool success = res.is_success();
    // if(!success){
    //     printf("匹配失败\n");
    //     return;
    // }
    // lexy::visualize(stdout, tree,{lexy::visualize_fancy});
    // auto rr = res.value();

    auto res = lexy::parse<grammar::LSystem>(sinput, lexy_ext::report_error);
    if (!res.is_success()) {
        printf("解析失败\n");
        return;
    }

    config::LSystem lsys = res.value();

    for(auto &prod:lsys.prods){
        printf("name : %s\n", prod.sym.name.c_str());
    }

    printf("\n");
}

void test_for_expr(){
    auto sinput = lexy::zstring_input("(x+y)/2-z+3*x*y");
    
    // lexy::parse_tree_for<decltype(sinput)> tree;
    // auto res = lexy::parse_as_tree<grammar::LProduction>(tree, sinput, lexy_ext::report_error);
    // bool success = res.is_success();
    // if(!success){
    //     printf("匹配失败\n");
    //     return;
    // }
    // lexy::visualize(stdout, tree,{lexy::visualize_fancy});

    auto res = lexy::parse<grammar::MathExpr>(sinput, lexy_ext::report_error);

    auto val = res.value();
    ast::Environment env;

    printf("res = %.4f\n",val->evaluate(env));
}


void test_for_LSysCall(){
    // auto sinput = lexy::zstring_input("F(0.1,2.3,1.6)A(1,3)C(5)");
    auto s_input = lexy::zstring_input("F(0.5, 2.5, -5.0)");
    auto s_prod = lexy::zstring_input("F(x, y, z) -> A((x+y)*z, (x-z)/2, (x+z)/(x-y))F(x,y,z)B((x+y+z)/3)Hello(x*y-z)");

    auto res = lexy::parse<grammar::LSystem>(s_prod, lexy_ext::report_error);
    assert(res.is_success());
    config::LSystem lsys = res.value();

    auto rr = lexy::parse<grammar::LSysCall>(s_input, lexy_ext::report_error);
    assert(rr.is_success());
    config::LSysCall lsysc = rr.value();

    string output = lsysc.apply(lsys);
    printf("生成新字符串为: %s\n", output.c_str());
}

int main(void) {
    // test_for_key();
    // test_for_Number();
    // test_for_value();
    // test_for_expr();
    // test_for_LProduction();

    test_for_LSysCall();

    return 0;
}

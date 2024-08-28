#include "LSystem.hpp"

using namespace std;

int main(int argc, char** argv) {
    // 测试key的格式
    vector<string> test_key = {
        "F(a,b,c)",
        "Fa12(x12 ,y_32, _z)",
        "F_212(  alpha123, w_312 , 123abc)"};

    for (string& s : test_key) {
        printf("调试字符串： %s\n", s.c_str());
        auto s_input = lexy::zstring_input(s.c_str());
        // lexy::validate<SymDefineExpr>(s_input, lexy_ext::report_error);
        auto result = lexy::parse<SymDefineExpr>(s_input, lexy_ext::report_error);

        bool isOk = result.is_success();
        if (!isOk)
            break;

        Psym p = result.value();
        bool hasParam = p.params.has_value();

        printf("符号名：%s\n", p.sym_name.c_str());
        printf("参数列表：%s\n", hasParam ? "" : "空");
        if (hasParam){
            for (string& pv : p.params.value())
                printf("\"%s\"  ", pv.c_str());
            printf("\n");
        }
        printf("===========================================\n");
    }

    // 测试value的格式
    // vector<string> test_value = {
    //   "B(a+b,a-c,b*c)",
    //   "C(0.1*b,0.4a+c,1+b)",
    //   "F(a,b,c)"
    // };

    // for(string &s:test_value){
    //   printf("调试字符串： %s\n",s.c_str());
    //   auto s_input = lexy::zstring_input(s.c_str());
    //   lexy::validate<SymDefineExpr>(s_input, lexy_ext::report_error);
    // }

    return 0;
}

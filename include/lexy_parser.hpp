#include "proj.h"

#ifndef __WIND_PARSER
#define __WIND_PERSER


    #include "lexy/dsl.hpp"
    #include "lexy/callback.hpp"

    namespace{
        
        namespace dsl = lexy::dsl;
        struct SymName{
            // 符号的名称，字母或下划线开头，其余部分字母、数字、下划线组合
            static constexpr auto rule = dsl::identifier(dsl::ascii::alpha / dsl::lit_c<'_'>,dsl::ascii::alnum / dsl::lit_c<'_'>);
            static constexpr auto value = lexy::as_string<std::string>;
        };
        
        struct Parameter{
            // 形式参数的格式，暂时和SymName一样
            static constexpr auto rule = dsl::identifier(dsl::ascii::alpha / dsl::lit_c<'_'>,dsl::ascii::alnum / dsl::lit_c<'_'>);
            static constexpr auto value = lexy::as_string<std::string>;
        };

        struct SymDefineExpr{
            // 定义处的符号表达式，以SymName为开头，后接参数列表，也可以无参
            static constexpr auto rule = []{
                // static constexpr auto 
                auto barckcet_list = dsl::parenthesized.list(dsl::p<Parameter>, dsl::sep(dsl::comma));
                return dsl::p<SymName>  + barckcet_list;
            }();
            // 需要将解析出来的内容，注入到LSystem的Psym中去
            // static constexpr auto value = 
        };
    }

#endif

#include "proj.h"

#ifndef __WIND_PARSER
#define __WIND_PERSER


    #include "lexy/dsl.hpp"
    #include "lexy/callback.hpp"

    namespace{
        
        namespace dsl = lexy::dsl;
        struct VariableName{
            static constexpr auto rule = dsl::identifier(dsl::ascii::alpha / dsl::lit_c<'_'>,dsl::ascii::alnum / dsl::lit_c<'_'>);
            static constexpr auto value = lexy::as_string<std::string>;
        }

    }

#endif

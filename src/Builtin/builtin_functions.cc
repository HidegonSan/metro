#include <iostream>
#include "Types/Object.h"
#include "Types/BuiltinFunc.h"
#include "GC.h"

#define make_bifun_lambda(e...) \
  [] (std::vector<Object*> const& args) -> Object* { e }

#define make_bifun(_name,_ret_t,_args,_fp) \
  BuiltinFunc{ \
    .name = #_name, \
    .ret_type = ValueType::Kind:: _ret_t, \
    .arg_types = _args, \
    .func = _fp \
  }

#define make_bifun_code(_name,_ret_t,_args,_code...) \
  make_bifun(_name,_ret_t,_args,make_bifun_lambda(_code))

namespace metro {

namespace {
  //
  // print
  auto _bifun_print = make_bifun_lambda(
    auto ret = gcnew(ValueType::Kind::Int);

    for( auto&& arg : args ) {
      auto&& s = arg->to_string();

      ret->v_int += s.length();
      std::cout << s;
    }

    return ret;
  );
}

// -------------------------- //
//  builtin-functions
// -------------------------- //
std::vector<BuiltinFunc> const BuiltinFunc::builtin_functions {
  // print
  make_bifun(print, Int, { ValueType::Kind::Args },
    _bifun_print
  ),

  // println
  make_bifun_code(println, Int, { ValueType::Kind::Args },
    auto&& ret = _bifun_print(args);

    std::cout << std::endl;
    ret->v_int += 1;

    return ret;
  )
};

} // namespace metro

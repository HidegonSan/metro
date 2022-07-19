#include <iostream>
#include "Types/Token.h"
#include "Types/Object.h"
#include "Types/Node.h"
#include "Types/BuiltinFunc.h"
#include "Evaluator.h"
#include "GC.h"
#include "Utils.h"

namespace Metro {
  

  Object* Evaluator::eval_callfunc(Node* node) {
    if( node->nd_callee->kind == ND_BUILTIN_FUNC ) {
      std::vector<Object*> args;

      for( auto&& arg : node->list ) {
        args.emplace_back(eval(arg));
      }

      return node->nd_callee->nd_builtin_func->func(args);
    }

    for( auto dest = node->nd_callee->objects.begin(); auto&& arg : node->list ) {
      *dest++ = eval(arg);
    }


    alert;
    return nullptr;
  }
}
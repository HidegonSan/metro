#include "Types.h"
#include "MetroDriver/evaluator.h"
#include "GC.h"

namespace metro {

using ASTKind = AST::Kind;
using ValueKind = ValueType::Kind;

/*
  * left に変更を加えて、戻り値として left を返してください
  */

Object* Evaluator::eval_operator(AST::Kind kind, Object* left, Object* right) {
  auto const& type = left->type;

  if( type.arr_depth > 0 ) {
    // TODO: append array
    TODO_IMPL
  }

  switch( kind ) {
    // Add
    case AST::Kind::Add: {
      switch( type.kind ) {
        case ValueKind::Int: {
          left->v_int += right->v_int;
          break;
        }
      }
      break;
    }

    // Sub
    case AST::Kind::Sub: {
      switch( type.kind ) {
        case ValueKind::Int: {
          left->v_int -= right->v_int;
          break;
        }
      }
      break;
    }

    // Mul
    case AST::Kind::Mul: {
      switch( type.kind ) {
        case ValueKind::Int: {
          left->v_int *= right->v_int;
          break;
        }
      }
      break;
    }

    // Div
    case AST::Kind::Div: {
      switch( type.kind ) {
        case ValueKind::Int: {
          left->v_int /= right->v_int;
          break;
        }
      }
      break;
    }
  }

  return left;
}

}

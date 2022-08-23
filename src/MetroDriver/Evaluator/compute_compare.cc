#include "Types.h"
#include "MetroDriver/evaluator.h"
#include "GC.h"

namespace metro {

using ASTKind = AST::Kind;
using ValueKind = ValueType::Kind;
using CmpKind = AST::Compare::Item::Kind;

Object* Evaluator::compute_compare(AST::Compare* ast) {
  Object const* lhs = eval(ast->first);
  Object const* rhs = nullptr;

  bool res = true;

  for( auto it = ast->list.begin(); res && it != ast->list.end(); it++, lhs = rhs ) {
    auto const& typekind = lhs->type.kind;

    rhs = eval(it->ast);

    switch( it->kind ) {
      case CmpKind::BiggerLeft:
        switch( typekind ) {
          case ValueKind::Int:
            res = lhs->v_int > rhs->v_int;
            break;
        }

        break;

      case CmpKind::BiggerRight:
        switch( typekind ) {
          case ValueKind::Int:
            res = lhs->v_int < rhs->v_int;
            break;
        }

        break;

      case CmpKind::BiggerOrEqualLeft:
        switch( typekind ) {
          case ValueKind::Int:
            res = lhs->v_int >= rhs->v_int;
            break;
        }

        break;

      case CmpKind::BiggerOrEqualRight:
        switch( typekind ) {
          case ValueKind::Int:
            res = lhs->v_int <= rhs->v_int;
            break;
        }

        break;

      case CmpKind::Equal:
        switch( typekind ) {
          case ValueKind::Int:
            res = lhs->v_int == rhs->v_int;
            break;
        }

        break;

      case CmpKind::NotEqual:
        switch( typekind ) {
          case ValueKind::Int:
            res = lhs->v_int != rhs->v_int;
            break;
        }

        break;

      default:
        TODO_IMPL
    }

    // if( !res )
    //   break;
  }

  auto ret = gcnew(ValueType::Kind::Bool);
  ret->v_bool = res;

  return ret;
}

} // namespace metro

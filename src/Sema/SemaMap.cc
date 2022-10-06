#include "AST.h"
#include "Error.h"
#include "Utils.h"
#include "MetroDriver/Sema.h"

namespace metro::semantics {

void Sema::ast_map(
  AST::Base* ast,
  std::function<void(AST::Base*)> fp,
  std::function<void(AST::Base*)> fp_begin,
  std::function<void(AST::Base*)> fp_end) {

  if( !ast )
    return;

  if( fp_begin )
    fp_begin(ast);

  fp(ast);

  switch( ast->kind ) {
    case ASTKind::None:
    case ASTKind::Type:
      break;

    case ASTKind::Argument:
      ast_map(((AST::Argument*)ast)->type, fp, fp_begin, fp_end);
      break;

    case ASTKind::Boolean:
    case ASTKind::Value:
    case ASTKind::Variable:
      break;

    case ASTKind::Array:
    case ASTKind::Tuple:
    case ASTKind::Scope: {
      auto x = (AST::ListBase*)ast;

      for( auto&& y : x->elements )
        ast_map(y, fp, fp_begin, fp_end);

      break;
    }

    case ASTKind::Callfunc: {
      auto x = (AST::CallFunc*)ast;

      for( auto&& arg : x->args )
        ast_map(arg, fp, fp_begin, fp_end);

      break;
    }

    case ASTKind::Subscript:
    case ASTKind::MemberAccess:
    case ASTKind::Mul:
    case ASTKind::Div:
    case ASTKind::Add:
    case ASTKind::Sub:
    case ASTKind::Assign: {
      auto x = (AST::Expr*)ast;

      ast_map(x->lhs, fp, fp_begin, fp_end);
      ast_map(x->rhs, fp, fp_begin, fp_end);

      break;
    }

    case ASTKind::Compare: {
      auto x = (AST::Compare*)ast;

      ast_map(x->first, fp, fp_begin, fp_end);

      for( auto&& item : x->list )
        ast_map(item.ast, fp, fp_begin, fp_end);

      break;
    }

    case ASTKind::Return:
      ast_map(((AST::Return*)ast)->expr, fp, fp_begin, fp_end);
      break;

    case ASTKind::If:
      ast_map(((AST::If*)ast)->cond, fp, fp_begin, fp_end);
      ast_map(((AST::If*)ast)->if_true, fp, fp_begin, fp_end);
      ast_map(((AST::If*)ast)->if_false, fp, fp_begin, fp_end);
      break;

    case ASTKind::VarDefine:
      ast_map(((AST::VarDefine*)ast)->type, fp, fp_begin, fp_end);
      ast_map(((AST::VarDefine*)ast)->init, fp, fp_begin, fp_end);
      break;

    case ASTKind::For:
    case ASTKind::Loop:
    case ASTKind::While:
      TODO_IMPL

    case ASTKind::Function: {
      auto x = (AST::Function*)ast;

      for(auto&&arg:x->args)
        ast_map(&arg, fp, fp_begin, fp_end);

      ast_map(x->return_type, fp, fp_begin, fp_end);
      ast_map(x->code, fp, fp_begin, fp_end);

      break;
    }

    default:
      TODO_IMPL
  }

  if( fp_end )
    fp_end(ast);
}

} // namespace metro::semantics

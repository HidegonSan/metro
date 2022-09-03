#include "AST.h"
#include "Error.h"
#include "Utils.h"
#include "MetroDriver/Sema.h"

namespace metro::semantics {

using ast_map_fp = bool(*)(AST::Base*);

static void ast_map(AST::Base* ast, ASTVector& out, ast_map_fp fp) {
  if( ast == nullptr )
    return;

  if( fp(ast) )
    out.emplace_back(ast);

  switch( ast->kind ) {
    case ASTKind::None:
    case ASTKind::Type:
      break;

    case ASTKind::Argument:
      ast_map(((AST::Argument*)ast)->type, out, fp);
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
        ast_map(y, out, fp);

      break;
    }

    case ASTKind::Callfunc: {
      auto x = (AST::CallFunc*)ast;

      for( auto&& arg : x->args )
        ast_map(arg, out, fp);

      break;
    }

    case ASTKind::Subscript:
    case ASTKind::MemberAccess:
    case ASTKind::Mul:
    case ASTKind::Div:
    case ASTKind::Add:
    case ASTKind::Sub:
    case ASTKind::Assign:
      ast_map(((AST::Expr*)ast)->lhs, out, fp);
      ast_map(((AST::Expr*)ast)->rhs, out, fp);
      break;

    case ASTKind::Compare: {
      auto x = (AST::Compare*)ast;

      ast_map(x->first, out, fp);
      
      for( auto&& item : x->list )
        ast_map(item.ast, out, fp);

      break;
    }

    case ASTKind::Return:
      ast_map(((AST::Return*)ast)->expr, out, fp);
      break;
    
    case ASTKind::If:
      ast_map(((AST::If*)ast)->cond, out, fp);
      ast_map(((AST::If*)ast)->if_true, out, fp);
      ast_map(((AST::If*)ast)->if_false, out, fp);
      break;
    
    case ASTKind::VarDefine:
      ast_map(((AST::VarDefine*)ast)->type, out, fp);
      ast_map(((AST::VarDefine*)ast)->init, out, fp);
      break;
    
    case ASTKind::For:
    case ASTKind::Loop:
    case ASTKind::While:
      TODO_IMPL

    case ASTKind::Function: {
      auto x = (AST::Function*)ast;

      for(auto&&arg:x->args)
        ast_map(&arg,out,fp);
      
      ast_map(x->return_type,out,fp);
      ast_map(x->code,out,fp);

      break;
    }

    default:
      TODO_IMPL
  }
}

void Sema::analyze() {

  init_variable_dc();

}

void Sema::init_variable_dc() {

  ASTVector assign_ast_list;

  ast_map(
    root,
    assign_ast_list,
    [] (AST::Base* ast) -> bool {
      return ast->kind == ASTKind::Assign;
    }
  );



}

void Sema::deduction_variable_types() {

}

AST::VarDefine* Sema::get_var_definition_loc(AST::Variable* ast) {



}

ScopeInfo& Sema::get_cur_scope() {

}

ValueType Sema::eval_type(AST::Base* ast) {

  if( ast

}


} // namespace metro::semantics


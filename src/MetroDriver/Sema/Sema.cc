#include "AST.h"
#include "Error.h"
#include "Utils.h"
#include "MetroDriver/Sema.h"

namespace metro::semantics {

using ast_map_fp = bool(*)(AST::Base*);

// static void ast_map(AST::Base* ast, ASTVector& out, ast_map_fp fp) {
static void ast_map(AST::Base* ast, auto fp) {
  if( ast == nullptr )
    return;

  fp(ast);

  switch( ast->kind ) {
    case ASTKind::None:
    case ASTKind::Type:
      break;

    case ASTKind::Argument:
      ast_map(((AST::Argument*)ast)->type, fp);
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
        ast_map(y, fp);

      break;
    }

    case ASTKind::Callfunc: {
      auto x = (AST::CallFunc*)ast;

      for( auto&& arg : x->args )
        ast_map(arg, fp);

      break;
    }

    case ASTKind::Subscript:
    case ASTKind::MemberAccess:
    case ASTKind::Mul:
    case ASTKind::Div:
    case ASTKind::Add:
    case ASTKind::Sub:
    case ASTKind::Assign:
      ast_map(((AST::Expr*)ast)->lhs, fp);
      ast_map(((AST::Expr*)ast)->rhs, fp);
      break;

    case ASTKind::Compare: {
      auto x = (AST::Compare*)ast;

      ast_map(x->first, fp);
      
      for( auto&& item : x->list )
        ast_map(item.ast, fp);

      break;
    }

    case ASTKind::Return:
      ast_map(((AST::Return*)ast)->expr, fp);
      break;
    
    case ASTKind::If:
      ast_map(((AST::If*)ast)->cond, fp);
      ast_map(((AST::If*)ast)->if_true, fp);
      ast_map(((AST::If*)ast)->if_false, fp);
      break;
    
    case ASTKind::VarDefine:
      ast_map(((AST::VarDefine*)ast)->type, fp);
      ast_map(((AST::VarDefine*)ast)->init, fp);
      break;
    
    case ASTKind::For:
    case ASTKind::Loop:
    case ASTKind::While:
      TODO_IMPL

    case ASTKind::Function: {
      auto x = (AST::Function*)ast;

      for(auto&&arg:x->args)
        ast_map(&arg, fp);
      
      ast_map(x->return_type, fp);
      ast_map(x->code, fp);

      break;
    }

    default:
      TODO_IMPL
  }
}

static void get_last_expr(AST::Base* ast, ASTVector& out) {

  if( ast == nullptr )
    return;

  switch( ast->kind ) {
    case ASTKind::Type:
    case ASTKind::Argument:
    case ASTKind::VarDefine:
    case ASTKind::Function:
    case ASTKind::Struct:
      break;

    case ASTKind::Scope: {
      auto x = (AST::Scope*)ast;

      if( x->elements.empty() )
        out.emplace_back(ast);
      else
        get_last_expr(*((AST::Scope*)ast)->elements.rbegin(), out);

      break;
    }

    case ASTKind::If: {
      auto x = (AST::If*)ast;

      get_last_expr(x->if_true, out);
      get_last_expr(x->if_false, out);

      break;
    }

    case ASTKind::For:
    case ASTKind::Loop:
    case ASTKind::While:
      TODO_IMPL

    default:
      out.emplace_back(ast);
      break;
  }
}

void Sema::analyze() {

  this->deduction_variable_types();




}

void Sema::create_variable_dc() {



  ast_map(
    
  );


}

void Sema::deduction_variable_types() {

  this->create_variable_dc();

  for( auto&& pair : this->scope_info_map ) {
    auto&& [scope, info] = pair;


  }

}

VariableDC* Sema::get_variable_dc(AST::Variable* ast) {
  for( auto&& scope : this->scope_history ) {
    auto&& v_map = this->scope_info_map[scope].var_dc_map;

    if( v_map.contains(ast->name) )
      return &v_map[ast->name];
  }

  return nullptr;
}

ScopeInfo& Sema::get_cur_scope() {
  return this->scope_info_map[*this->scope_history.begin()];
}

ValueType Sema::eval_type(AST::Base* ast) {
  if( ast == nullptr )
    return { };

  if( this->caches.contains(ast) )
    return caches[ast];

  auto& ret = caches[ast];

  switch( ast->kind ) {

  }

  return ret;

  if( !ast ) {
    return { };
  }

  if( this->caches.contains(ast) ) {
    return this->caches[ast];
  }

  auto& ret = this->caches[ast];

  switch( ast->kind ) {
    case ASTKind::None:
      break;

    case ASTKind::Type: {
      auto type = (AST::Type*)ast;

      alertios("type->name = " << type->name);

      for( auto&& pair : ValueType::name_table ) {
        if( type->name == pair.first ) {
          alert;
          ret = pair.second;
          goto _found_type;
        }
      }

      // TODO: find struct

      Error(ErrorKind::UndefinedTypeName, ast->token, "undefined type name").emit();

    _found_type:
      alert;

      ret.arr_depth = type->arr_depth;
      ret.have_elements = type->have_elements;

      ret.is_mutable = type->is_mutable;
      ret.is_reference = type->is_reference;

      for( auto&& sub : type->elems ) {
        ret.elems.emplace_back(eval_type(sub));
      }

      break;
    }

    case ASTKind::Argument: {
      auto argument = (AST::Argument*)ast;

      ret = eval_type(argument->type);

      break;
    }

    case ASTKind::Value: {
      auto val = (AST::Value*)ast;

      val->object = create_obj(ast->token);

      ret = val->object->type;
      break;
    }

    case ASTKind::Variable: {
      auto x = (AST::Variable*)ast;

      auto var = get_var_context(x->name);

      if( !var ) {
        Error(ErrorKind::UndefinedVariable, ast->token, "undefined variable name").emit(true);
      }

      ret = var->type;
      break;
    }

    case ASTKind::Callfunc: {
      ret = sema_callfunc((AST::CallFunc*)ast);

      alertios("callfunc " << ((AST::CallFunc*)ast)->name << ": " << ret.to_string());

      break;
    }

    case ASTKind::Compare: {
      auto cmp = (AST::Compare*)ast;

      eval_type(cmp->first);

      for( auto&& item : cmp->list ) {
        eval_type(item.ast);
      }

      ret = ValueType::Kind::Bool;
      break;
    }

    //
    // assignment
    case ASTKind::Assign: {
      auto x = (AST::Expr*)ast;

      TODO_IMPL

      break;
    }

    //
    // variable definition
    case ASTKind::VarDefine: {
      auto x = (AST::VarDefine*)ast;

      auto var_ctx = get_var_context(x->name, true);

      // 型の指定、初期化式　どっちもある
      if( x->type && x->init ) {
        var_ctx->type = eval_type(x->type);

        // 指定された型と初期化式の型が一致しない
        if( auto init_type = eval_type(x->init); !var_ctx->type.equals(init_type) ) {
          Error(ErrorKind::TypeMismatch, x->init, "expected '" + var_ctx->type.to_string() + "' but found '" + init_type.to_string() + "'")
            .add_help(x->type, "specified here")
            .emit();
        }
      }

      // 型の指定のみ
      else if( x->type ) {
        var_ctx->type = eval_type(x->type);
      }

      // 初期化式のみ
      else if( x->init ) {
        var_ctx->type = eval_type(x->init);
      }

      // どっちもない
      else {
        TODO_IMPL
      }

      break;
    }

    case ASTKind::If:
    case ASTKind::For:
      ret = sema_controls(ast);
      break;

    case ASTKind::Return: {
      auto x = (AST::Return*)ast;


      break;
    }

    case ASTKind::Scope: {
      auto scope = (AST::Scope*)ast;

      // 空のスコープ
      if( scope->elements.empty() )
        break;

      break;
    }

    case ASTKind::Function: {
      auto func = (AST::Function*)ast;

      if( !func->return_type ) {
        Error(ErrorKind::LanguageVersion, func->token,
          "function definition without return-type specification does not implemented yet")
          .add_help(func->code->token, "insert type name before this token")
          .emit(true);
      }

      auto&& final_expr_list = get_returnable_expr(func->code);

      cur_func_ast = func;

      // arguments
      for( auto&& arg : func->args )
        this->eval_type(&arg);

      // return-type
      // ret = analyze_func_return_type(func);
      ret = eval_type(func->return_type);

      // code
      eval_type(func->code);

      cur_func_ast = nullptr;

      break;
    }

    default: {
      if( !ast->is_expr ) {
        alertfmt("%d", ast->kind);
        crash;
      }

      auto expr = (AST::Expr*)ast;

      auto lhs = eval_type(expr->lhs);
      auto rhs = eval_type(expr->rhs);

      // TODO: check operator

      ret = lhs;
      break;
    }
  }

  return ret;
}

ASTVector Sema::get_returnable_expr(AST::Base* ast) {
  ASTVector ret;

  // "return"
  ast_map(
    ast,
    [&ret] (auto x) { if( x->kind == ASTKind::Return ) ret.emplace_back(x); }
  );

  // last
  get_last_expr(ast, ret);

  // remove duplicates
  ret.erase(std::unique(ret.begin(), ret.end()), ret.end());

  return ret;
}

Object* Sema::create_obj(Token* token) {
  auto obj = new Object;

  switch( token->kind ) {
    case TokenKind::Int: {
      obj->type = ValueType::Kind::Int;
      obj->v_int = atoi(token->str.data());
      break;
    }

    case TokenKind::Float: {
      obj->type = ValueType::Kind::Float;
      obj->v_float = atof(token->str.data());
      break;
    }

    case TokenKind::String: {
      obj->type = ValueType::Kind::String;
      obj->v_str = Utils::Strings::to_u16string(std::string(token->str));

      obj->v_str.erase(obj->v_str.begin());
      obj->v_str.pop_back();

      break;
    }

    default:
      TODO_IMPL
  }

  return obj;
}


} // namespace metro::semantics


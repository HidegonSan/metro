#include <functional>
#include "AST.h"
#include "Error.h"
#include "Utils.h"
#include "MetroDriver/Sema.h"

namespace metro::semantics {

using ast_map_fp = bool(*)(AST::Base*);

// static void ast_map(AST::Base* ast, ASTVector& out, ast_map_fp fp) {

static void ast_map(
  AST::Base* ast,
  std::function<void(AST::Base*)> fp,
  std::function<void(AST::Base*)> fp_begin = nullptr,
  std::function<void(AST::Base*)> fp_end = nullptr) {

  if( ast == nullptr )
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
    this->root,
    [this] (AST::Base* ast) {
      switch( ast->kind ) {
        case ASTKind::VarDefine: {
          auto x = (AST::VarDefine*)ast;
          auto& scope = this->get_cur_scope();

          if( scope.find_var(x->name) ) {
            // shadowing
            TODO_IMPL
          }

          auto& dc = scope.append_var(x->name);

          dc.ast = x;

          if( x->type ) {
            dc.is_deducted = true;
            dc.specified_type = x->type;
          }

          if( x->init ) {
            dc.candidates.emplace_back(x->init);
          }

          break;
        }

        case ASTKind::Variable: {
          auto x = (AST::Variable*)ast;
          auto dc = this->get_variable_dc(x);

          if( !dc )
            Error(ErrorKind::UndefinedVariable, ast, "undefined variable name")
              .emit(true);

          this->var_dc_ptr_map[x] = dc;

          break;
        }
        
      }
    },
    [this] (AST::Base* ast) {
      switch( ast->kind ) {
        case ASTKind::Scope:
          this->enter_scope((AST::Scope*)ast);
          break;

        case ASTKind::Function: {
          auto x = (AST::Function*)ast;
          auto& scope = this->scope_info_map[x->code];

          for( auto&& arg : x->args ) {
            auto&& dc = scope.append_var(arg.name);

            dc.is_argument = true;
            dc.ast_arg = &arg;
            dc.is_deducted = true;
            dc.specified_type = arg.type;
          }

          break;
        }
      }
    },
    [this] (AST::Base* ast) {
      switch( ast->kind ) {
        case ASTKind::Scope:
          this->leave_scope();
          break;

        case ASTKind::Assign: {
          auto x = (AST::Expr*)ast;

          if( x->lhs->kind == ASTKind::Variable ) {
            auto var = (AST::Variable*)x->lhs;
            auto dc = this->var_dc_ptr_map[var];

            assert(dc != nullptr);

            dc->candidates.emplace_back(x->rhs);
          }

          break;
        }
      }
    }
  );


}

void Sema::deduction_variable_types() {

  this->create_variable_dc();

  for( auto&& pair : this->scope_info_map ) {
    auto&& [scope, info] = pair;

    debug(
      printf("%p\n", scope);

      for( auto&& dc : info.var_dc_list ) {
        auto&& name = dc.name;

        if( dc.is_argument ) {
          std::cout << "arg: " << name << std::endl;
        }
        else {
          std::cout
            << name << std::endl
            << dc.ast->to_string() << std::endl;
        }

        for( auto&& x : dc.candidates ) {
          std::cout << x->to_string() << std::endl;
        }
      }
    )
  }

}

VariableDC* Sema::get_variable_dc(AST::Variable* ast) {
  for( auto&& scope : this->scope_history ) {
    if( auto dc = this->scope_info_map[scope].find_var(ast->name); dc ) {
      return dc;
    }
  }

  return nullptr;
}

ScopeInfo& Sema::get_cur_scope() {
  return this->scope_info_map[*this->scope_history.begin()];
}

ValueType Sema::eval_type(AST::Base* ast) {
  if( !ast )
    return { };

  if( this->caches.contains(ast) )
    return this->caches[ast];

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


      break;
    }

    case ASTKind::Callfunc: {
      

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

      break;
    }

    case ASTKind::If: {
      auto if_x = (AST::If*)ast;

      if( !eval_type(if_x->cond).equals(ValueType::Kind::Bool) ) {
        Error(ErrorKind::TypeMismatch, if_x->cond, "condition must be boolean")
          .emit();
      }

      ret = eval_type(if_x->if_true);

      if( if_x->if_false && !ret.equals(eval_type(if_x->if_false)) ) {
        Error(ErrorKind::TypeMismatch, if_x->token, "if-expr type mismatch")
          .emit();
      }

      break;
    }

    case ASTKind::For: {
      auto for_x = (AST::For*)ast;

      eval_type(for_x->init);

      if( !eval_type(for_x->cond).equals(ValueType::Kind::Bool) ) {
        Error(ErrorKind::TypeMismatch, for_x->cond, "condition must be boolean").emit();
      }

      eval_type(for_x->counter);
      eval_type(for_x->code);

      break;
    }

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

      // auto&& final_expr_list = get_returnable_expr(func->code);

      TODO_IMPL

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

ScopeInfo& Sema::enter_scope(AST::Scope* ast) {
  auto&& info = this->scope_info_map[ast];

  info.ast = ast;

  this->scope_history.emplace_front(ast);

  return info;
}

void Sema::leave_scope() {
  this->scope_history.pop_front();
}

ASTVector Sema::get_returnable_expr(AST::Base* ast) {
  ASTVector ret;

  // "return"
  ast_map(
    ast,
    [&ret] (AST::Base* ast) { if( ast->kind == ASTKind::Return ) ret.emplace_back(ast); }
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


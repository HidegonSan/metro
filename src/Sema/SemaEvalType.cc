#include "AST.h"
#include "Error.h"
#include "MetroDriver/Sema.h"
#include "Utils.h"

#define _try_eval_r(_ast) \
  if (auto&& res = this->try_eval_type(_ast); _ast && res.fail()) return res;

namespace metro::semantics {

Sema::EvalResult Sema::try_eval_type(AST::Base* ast) {
  using Cond = EvalResult::Condition;

  try {
    ast_map(ast, [&](AST::Base* ast) {
      switch (ast->kind) {
        case ASTKind::Variable: {
          auto x = (AST::Variable*)ast;

          if (!this->var_dc_ptr_map[x]->is_deducted) {
            throw Cond::Incomplete;
          }

          break;
        }

        case ASTKind::Callfunc: {
          auto x = (AST::CallFunc*)ast;

          auto func = this->find_func(x->name);

          // function not found
          if (!func) {
            Error(ErrorKind::UndefinedFunction, x->token,
                  "undefined function name")
                .emit(true);
          }

          // return type is not deducted yet
          if (!func->dc.is_deducted) throw Cond::Incomplete;

          break;
        }

        case ASTKind::Assign: {
          auto x = (AST::Expr*)ast;

          auto&& rhs = this->try_eval_type(x->rhs);

          if (!is_lvalue(x->lhs)) {
            Error(ErrorKind::TypeMismatch, x->lhs, "expected lvalue expression")
                .emit();
          }

          break;
        }
      }
    });
  } catch (EvalResult const& res) {
    return res;
  } catch (EvalResult::Condition cond) {
    return cond;
  } catch (...) {
    crash;
  }

  return this->eval_type(ast);
}

ValueType Sema::eval_type(AST::Base* ast) {
  if (!ast) return {};

  // if( this->caches.contains(ast) )
  //   return this->caches[ast];

  auto& ret = this->caches[ast];

  switch (ast->kind) {
    case ASTKind::None:
      break;

    case ASTKind::Type: {
      auto type = (AST::Type*)ast;

      assert(this->get_type_from_name(ret, type->name));

      ret.arr_depth = type->arr_depth;
      ret.have_elements = type->have_elements;

      ret.is_mutable = type->is_mutable;
      ret.is_reference = type->is_reference;

      for (auto&& sub : type->elems)
        ret.elems.emplace_back(this->eval_type(sub));

      alertios(ret.to_string());

      break;
    }

    case ASTKind::Argument: {
      auto argument = (AST::Argument*)ast;

      ret = this->eval_type(argument->type);

      break;
    }

    case ASTKind::Value: {
      switch (ast->token->kind) {
        case TokenKind::Int:
          return ValueType::Kind::Int;

        case TokenKind::Float:
          return ValueType::Kind::Float;

        case TokenKind::Char:
          return ValueType::Kind::Char;

        case TokenKind::String:
          return ValueType::Kind::String;
      }

      TODO_IMPL
    }

    case ASTKind::Variable: {
      auto x = (AST::Variable*)ast;

      ret = this->var_dc_ptr_map[x]->type;

      break;
    }

    case ASTKind::Callfunc: {
      auto x = (AST::CallFunc*)ast;

      auto func = this->find_func(x->name);

      if (!func) {
        for (auto&& bfun : BuiltinFunc::builtin_functions) {
          if (bfun.name == x->name) {
            x->callee_builtin = &bfun;

            if (bfun.arg_types.empty()) {
              if (!x->args.empty())
                Error(ErrorKind::TooManyArguments, x, "too many arguments")
                    .emit(true);

              return bfun.ret_type;
            }

            size_t counter{};
            std::vector<ValueType> args;

            for (auto&& arg : x->args) {
              args.emplace_back(this->eval_type(arg));
            }

            for (auto formal_iter = bfun.arg_types.begin();
                 auto&& actual : args) {
              if (formal_iter == bfun.arg_types.end()) {
                Error(ErrorKind::TooManyArguments, ast, "too many arguments")
                    .emit();

                return bfun.ret_type;
              }

              if (formal_iter->equals(ValueType::Kind::Args)) {
                return bfun.ret_type;
              }

              if (!formal_iter->equals(actual)) {
                Error(ErrorKind::TypeMismatch, x->args[counter],
                      "expected '" + formal_iter->to_string() +
                          "' but found '" + actual.to_string() + "'")
                    .emit(true);
              }

              counter++;
            }

            if (counter < args.size()) {
              if (!args[counter + 1].equals(ValueType::Kind::Args))
                Error(ErrorKind::TooFewArguments, x, "too few arguments")
                    .emit(true);
            }

            return bfun.ret_type;
          }
        }

        Error(ErrorKind::UndefinedFunction, x->token, "undefined function name")
            .emit(true);
      }

      // todo: check arguments

      TODO_IMPL

      return func->dc.type;
    }

    case ASTKind::Compare: {
      return ValueType::Kind::Bool;
    }

    //
    // assignment
    case ASTKind::Assign: {
      auto x = (AST::Expr*)ast;

      ret = this->eval_type(x->rhs);

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

      if (!this->eval_type(if_x->cond).equals(ValueType::Kind::Bool))
        Error(ErrorKind::TypeMismatch, if_x->cond, "condition must be boolean")
            .emit();

      ret = this->eval_type(if_x->if_true);

      if (if_x->if_false)
        if (!ret.equals(this->eval_type(if_x->if_false)))
          Error(ErrorKind::TypeMismatch, if_x->token, "if-expr type mismatch")
              .emit();

      break;
    }

    case ASTKind::For: {
      auto x = (AST::For*)ast;

      this->eval_type(x->init);

      auto&& cond = this->eval_type(x->cond);

      if (!cond.equals(ValueType::Kind::Bool))
        Error(ErrorKind::TypeMismatch, x->cond, "condition must be boolean")
            .emit();

      this->eval_type(x->counter);
      this->eval_type(x->code);

      break;
    }

    case ASTKind::Return: {
      auto x = (AST::Return*)ast;

      this->eval_type(x->expr);

      break;
    }

    case ASTKind::Scope: {
      auto scope = (AST::Scope*)ast;

      // 空のスコープ
      if (scope->elements.empty()) break;

      auto&& finals = this->get_returnable_expr(scope);

      this->enter_scope(scope);

      // eval all elements
      // for( auto&& elem : scope->elements )
      //   this->eval_type(elem);

      ret = this->eval_type(*scope->elements.rbegin());

      /*
            for( auto it = finals.begin() + 1; it != finals.end(); it++ )
              if( auto&& t = this->eval_type(*it); !ret.equals(t) )
                Error(ErrorKind::TypeMismatch, *it,
                  Utils::linkstr( "expected '", ret.to_string(),
                    "' but found '", t.to_string(), "'"))
                    .emit();
      */

      this->leave_scope(scope);

      break;
    }

    case ASTKind::Function:
    case ASTKind::Struct:
      break;

    default: {
      using VK = ValueType::Kind;

      if (!ast->is_expr) {
        alertfmt("%d", ast->kind);
        crash;
      }

      auto expr = (AST::Expr*)ast;

      auto lhs = eval_type(expr->lhs);
      auto rhs = eval_type(expr->rhs);

      auto errfn = [&](std::string&& s, bool ex = true) {
        Error(ErrorKind::InvalidOperator, expr,
              s + " '" + std::string(ast->token->str) + "' for '" +
                  lhs.to_string() + "' and '" + rhs.to_string() + "'")
            .emit(ex);
      };

      // not equal
      if (!lhs.equals(rhs)) {
        auto&& err = Error(ErrorKind::TypeMismatch, expr, "type mismatch");

        switch (ast->kind) {
          case ASTKind::Add:
          case ASTKind::Sub:
          case ASTKind::Mul:
          case ASTKind::Div:
            if (auto x = expr->lhs;
                lhs.equals(VK::Int)
                    ? rhs.equals(VK::Float)
                    : (x = expr->rhs,
                       lhs.equals(VK::Float) && rhs.equals(VK::Int))) {
              err.add_help(x, "you can convert to float with 'cast'");
            }
            break;
        }

        err.emit(true);
      }

      // array
      if (lhs.arr_depth || rhs.arr_depth) {
        errfn("cannot use operator to array", ast->kind != ASTKind::Add);

        Error(ErrorKind::Suggestion, expr->token,
              "you can use function 'append' for array appending")
            .set_warn()
            .emit(true);
      }

      // string
      if (lhs.equals(VK::String) && ast->kind != ASTKind::Add)
        errfn("operator can be use for string is only addition");

      // shift, but not integer
      if (ast->kind == ASTKind::LShift || ast->kind == ASTKind::RShift) {
        if (!lhs.equals(VK::Int)) goto _invalid_op;
      }

      break;

    _invalid_op:;
      errfn("cannot use operator");
    }
  }

  return ret;
}

}  // namespace metro::semantics

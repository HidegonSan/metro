#include "AST.h"
#include "Utils.h"

namespace metro::AST {

std::string ListBase::get_elems_string() const {
  return Utils::join(", ", elements, [] (auto x) { return x->to_string(); });
}

std::string Argument::to_string() const {
  return Utils::linkstr("<Argument '", name, "' : ", type->to_string(), ">");
}

std::string Array::to_string() const {
  std::string ret { "<Array " };

  for( size_t i = 0; i < elements.size(); i++ ) {
    ret += std::to_string(i) + ": " + elements[i]->to_string();
    if( i < elements.size() - 1 ) ret += ", ";
  }

  return ret + ">";
}

std::string Boolean::to_string() const {
  return val ? "<Boolean true>" : "<Boolean false>";
}

std::string CallFunc::to_string() const {
  auto ret = "<Callfunc '" + std::string(name) + "'";

  if( !args.empty() )
    ret += " " + Utils::join<AST::Base*>(", ", args, [] (auto x) {return x->to_string();});

  return ret + ">";
}

std::string Compare::to_string() const {
  std::string ret{ "<Compare " + first->to_string() };

  for( auto&& item : list ) {
    ret += " " + std::string(item.token->str) + " " + item.ast->to_string();
  }

  return ret + ">";
}

std::string Expr::to_string() const {
  return
    "<Expr " + lhs->to_string() + " "
    + std::string(token->str) + " " + rhs->to_string() + ">";
}

std::string For::to_string() const {
  return Utils::linkstr(
    "<For ",
    init->to_string(), "; ", cond->to_string(), "; ",
    counter->to_string(), " ", code->to_string(), ">"
  );
}

std::string Function::to_string() const {
  return Utils::linkstr(
    "<Function '", name, "' (", Utils::join(", ", args), ") ",
    return_type != nullptr ? ("-> " + return_type->to_string()) : "", " ", code->to_string(), ">"
  );
}

std::string If::to_string() const {
  return Utils::linkstr(
    "<If ", cond->to_string(),
    "{ ", if_true->to_string(), " }",
    if_false != nullptr ? Utils::linkstr(" else {", if_false->to_string(), "}") : "", ">"
  );
}

std::string VarDefine::to_string() const {
  auto&& ret = "<VarDefine '" + std::string(name) + "'";

  if( type != nullptr ) {
    ret += " : " + type->to_string();
  }

  if( init != nullptr ) {
    ret += " = " + init->to_string();
  }

  return ret + ">";
}

std::string None::to_string() const {
  return "<None>";
}

std::string Return::to_string() const {
  if( !expr )
    return "<Return>";

  return Utils::linkstr("<Return ", expr->to_string(), ">");
}

std::string Scope::to_string() const {
  if( elements.empty() )
    return Utils::format("<Scope %p: (Empty)>", this);

  return Utils::format("<Scope %p: ", this) + this->get_elems_string() + ">";
}

std::string Struct::to_string() const {
  return Utils::linkstr(
    "<Struct '", name, "' {",
    Utils::join(", ", members, [] (auto&& x) {
      return Utils::linkstr(x.name, ": ", x.type->to_string());
    }),
    "}>"
  );
}

std::string Type::to_string() const {
  auto&& ret = "<Type '" + std::string(name) + "'";

  if( !elems.empty() )
    ret += " elems{" + Utils::join(", ", elems, [] (auto x) { return x->to_string(); }) + "}";

  if( is_mutable )
    ret += " mut";

  if( is_reference )
    ret += "&";

  return ret + ">";
}

std::string Value::to_string() const {
  return "<Value '" + std::string(token->str) + "' >";
}

std::string Variable::to_string() const {
  return "<Variable '" + std::string(name) + "'>";
}

std::string Tuple::to_string() const {
  TODO_IMPL
  return "<tuple>";
}

} // namespace metro::AST

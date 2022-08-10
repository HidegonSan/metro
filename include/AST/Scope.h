#pragma once

namespace Metro::AST {
  struct Scope : Base {
    std::vector<Base*>  elems;

    std::pair<size_t, size_t> get_range_on_source() {
      auto begin = token->pos;
      
      if( elems.empty() ) {
        return { begin, token->next->pos + 1 };
      }

      return { begin, (*elems.rbegin())->get_range_on_source().second };
    }

    auto& append(Base* ast) {
      return elems.emplace_back(ast);
    }

    std::string to_string() const {
      if( elems.empty() ) {
        return "<Scope (Empty)>";
      }

      return Utils::format("<Scope %p: ", this) + Utils::join(", ", elems, [] (auto x) { return x->to_string(); }) + ">";
    }

    Scope(Token* tok)
      : Base(Kind::Scope, tok)
    {
    }
  };
}
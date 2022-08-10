#pragma once

namespace Metro::AST {
  struct For : Base {
    Base* init;
    Base* cond;
    Base* counter;
    Base* code;

    std::string to_string() const {
      return Utils::linkstr(
        "<For ",
        init->to_string(), "; ", cond->to_string(), "; ",
        counter->to_string(), " ", code->to_string(), ">"
      );
    }

    For(Token* tok)
      : Base(Kind::For, tok),
        init(nullptr),
        cond(nullptr),
        counter(nullptr),
        code(nullptr)
    {
    }
  };
}
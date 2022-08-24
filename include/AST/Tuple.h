// -----------------------
//  タプル
// -----------------------
#pragma once

namespace metro::AST {

struct Tuple : Base {
  std::vector<AST::Base*> elements;

  std::string to_string() const;
  SourceRange get_range_on_source() const;

  Tuple(Token* bracket, Base* first)
    : Base(Kind::Tuple, bracket)
  {
    this->elements.emplace_back(first);
  }
};

} // namespace metro::AST


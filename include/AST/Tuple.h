// -----------------------
//  タプル
// -----------------------
#pragma once

namespace metro::AST {

struct Tuple : ListBase {
  std::string to_string() const;
  SourceRange get_range_on_source() const;

  Tuple(Token* bracket, Base* first)
    : ListBase(Kind::Tuple, bracket)
  {
    this->elements.emplace_back(first);
  }
};

} // namespace metro::AST


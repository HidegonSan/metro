#include "Types.h"
#include "AST.h"

namespace metro::AST {

Compare::Compare(Base* first)
  : Base(Kind::Compare, nullptr),
    first(first)
{
}

Compare::Item& Compare::append(Item::Kind kind, Token* tok, Base* ast) {
  return list.emplace_back(kind, tok, ast);
}

Compare* Compare::create(Base*& ast) {
  if( ast->kind != AST::Kind::Compare ) {
    ast = new AST::Compare(ast);
  }

  return (Compare*)ast;
}

} // namespace metro::AST

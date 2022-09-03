#pragma once

#include <list>
#include <map>
#include <vector>

namespace metro {

namespace AST {

struct Base;
struct Variable;
struct Scope;
struct Function;

} // namespace metro::AST

namespace semantics {

struct VariableDC {
  AST::Variable* ast;
  std::vector<AST::Base*> candidates;
};

struct ScopeInfo {

};

class Sema {

  using ASTKind = AST::Kind;
  using ASTVector = std::vector<AST::Base*>;

public:
  explicit Sema(AST::Scope* root)
    : root(root)
  {
  }

  Sema(Sema&&) = delete;
  Sema(Sema const&) = delete;

  void analyze();

private:

  void init_variable_dc();

  AST::Scope* root;

  

};

} // namespace metro::semantics

} // namespace metro


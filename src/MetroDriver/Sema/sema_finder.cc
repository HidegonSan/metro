#include "AST.h"
#include "MetroDriver/sema.h"
#include "Debug.h"
#include "Utils.h"
#include "Error.h"

namespace metro {

static ASTVector found;

ASTVector&& Sema::get_finder_result() {
  return std::move(found);
}

void Sema::ast_finder(AST::Base* ast, WalkerFuncPointer walker) {
  if( walker(ast) )
    found.emplace_back(ast);
  else
    return;

  if( ast->is_factor() )
    return;

  switch( ast->kind ) {
    case ASTKind::Type: {
      auto x = (AST::Type*)ast;

      for( auto&& i : x->elems )
        ast_finder(i, walker);

      break;
    }
  }
}

} // namespace metro


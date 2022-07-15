#include "Types/Token.h"
#include "Types/Object.h"
#include "Types/Node.h"
#include "Parser.h"
#include "Utils.h"

namespace Metro {
  Parser::Parser(Token* token)
    : cur(token),
      ate(nullptr)
  {
  }

  Node* Parser::parse() {
    auto node = new Node(ND_SCOPE, nullptr);

    while( check() ) {
      auto item = func();
      node->append(item);

      if( cur->kind == TOK_END ) {
        break;
      }

      switch( item->kind ) {
        case ND_FUNCTION:
          break;
        
        default:
          expect_semi();
      }
    }

    return node;
  }
}
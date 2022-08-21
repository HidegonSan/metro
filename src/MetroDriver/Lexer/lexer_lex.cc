#include <cstring>
#include "Types/Token.h"
#include "MetroDriver/Lexer.h"
#include "Error.h"
#include "Utils.h"

namespace metro {
  Token* Lexer::lex() {
    Token top;
    Token* cur = &top;

    pass_space();

    while( check() ) {
      auto ch = peek();
      auto pos = position;
      auto str = source.data() + pos;

      cur = new Token(TokenKind::Int, cur, pos);

      // digits
      if( isdigit(ch) ) {
        while( isdigit(peek()) ) position++;

        if( peek() == '.' ) {
          position++;

          if( peek() <= ' ' ) {
            cur->kind = TokenKind::Float;
          }
          else if( isalpha(ch == peek()) || ch == '_' ) {
            position--;
          }
        }
      }

      // ident
      else if( isalpha(ch) || ch == '_' ) {
        cur->kind = TokenKind::Ident;
        while( isalnum(ch = peek()) || ch == '_' ) position++;
      }

      // char, string
      else if( ch == '"' || ch == '\'' ) {
        auto begin = ch;

        cur->kind = begin == '"' ? TokenKind::String : TokenKind::Char;

        for( position++; (ch = peek()) != begin; position++ );
        position++;
      }

      else {
        cur->kind = TokenKind::Punctuator;

        for( auto&& pair : Token::punctuator_strtable ) {
          if( match(pair.first) ) {
            str = pair.first;
            cur->pu_kind = pair.second;
            position += strlen(pair.first);
            goto found_op;
          }
        }

        Error::add_error(ErrorKind::InvalidToken, position, "invalid token");
        Error::exit_app();

      found_op:;
      }

      cur->str = { str, position - pos };
      cur->endpos = cur->pos + cur->str.length();
      cur->lastpos = cur->endpos - 1;

      pass_space();
    }

    cur = new Token(TokenKind::End, cur, position - 1);
    cur->str = " ";

    return top.next;
  }
}

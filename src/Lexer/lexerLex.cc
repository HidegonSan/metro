#include <cstring>
#include "Types/Token.h"
#include "Lexer.h"
#include "Utils.h"

static constexpr char const operators[] =
  "(){}[]<>"
  "+-*/%"
  "&^|~"
  ":;,."
  "!?%";

static constexpr char const* long_operators[] = {
  ">>=",
  "<<=",
  "->",
  ">=",
  "<=",
  "+=",
  "-=",
  "*=",
  "/=",
  "%=",
  "++",
  "--",
  "<<",
  ">>",
  "&&",
  "||",
};

namespace Metro {
  Token* Lexer::lex() {
    Token top;
    Token* cur = &top;

    pass_space();

    while( check() ) {
      auto ch = peek();
      auto pos = position;
      auto str = source.data() + pos;

      cur = new Token(TOK_INT, cur, pos);

      // digits
      if( isdigit(ch) ) {
        while( isdigit(peek()) ) position++;

        if( peek() == '.' ) {
          position++;

          if( peek() <= ' ' ) {
            cur->kind = TOK_FLOAT;
          }
          else if( isalpha(ch == peek()) || ch == '_' ) {
            position--;
          }
        }
      }

      // ident
      else if( isalpha(ch) || ch == '_' ) {
        cur->kind = TOK_IDENT;
        while( isalnum(ch = peek()) || ch == '_' ) position++;
      }

      // char, string
      else if( ch == '"' || ch == '\'' ) {
        auto begin = ch;

        cur->kind = begin == '"' ? TOK_STRING : TOK_CHAR;

        for( position++; (ch = peek()) != begin; position++ );
        position++;

        cur->str = { str + 1, position - pos - 2 };
        pass_space();
        continue;
      }
      
      else {
        cur->kind = TOK_PUNCTUATER;

        for( std::string_view s : long_operators ) {
          if( match(s) ) {
            position += s.length();
            cur->str = s;
            goto found_op;
          }
        }

        for( char const& c : operators ) {
          if( ch == c ) {
            position++;
            cur->str = { &c, 1 };
            goto found_op;
          }
        }

      found_op:
        pass_space();
        continue;
      }

      cur->str = { str, position - pos };
      pass_space();
    }

    cur = new Token(TOK_END, cur, position);
    return top.next;
  }
}
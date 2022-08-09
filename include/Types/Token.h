#pragma once

#include <string>
#include <vector>

namespace Metro {
  enum class TokenKind {
    Int,
    Float,
    Char,
    String,
    Ident,
    Punctuator,
    End
  };

  enum class PunctuatorKind {
    Add,
    Sub,
    Mul,
    Mod,
    Div,
    
    Assign, // =

    Equal,
    BiggerLeft,
    BiggerRight,
    BigOrEqualLeft,
    BigOrEqualRight,
    ShiftLeft,
    ShiftRight,

    Colon,
    Semicolon,

    RoundBracketOpen,     // (
    RoundBracketClone,    // )
    CurlyBracketOpen,     // {
    CurlyBracketClone,    // }
    SquareBracketOpen,    // [
    SquareBracketClone,   // ]
    AngleBracketOpen,     // <
    AngleBracketClone,    // >
    
  };

  struct Token {
    TokenKind kind;
    PunctuatorKind pu_kind;
    Token* prev;
    Token* next;
    std::string_view str;
    size_t pos;

    Token(TokenKind kind = TokenKind::Int)
      : kind(kind),
        prev(nullptr),
        next(nullptr),
        pos(0)
    {
    }

    Token(TokenKind kind, Token* prev, size_t pos)
      : Token(kind)
    {
      this->prev = prev;
      this->pos = pos;

      if( prev ) prev->next = this;
    }

    static std::vector<std::pair<char const*, PunctuatorKind>> const punctuator_strtable;
  };
}
#pragma once

#include <string>
#include <vector>

namespace metro {
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
    VariableLengthArgument,
    SpecifyReturnType, // ->
    ArrayType,         // []

    Add,
    Sub,
    Mul,
    Mod,
    Div,
    And,
    Or,
    Not,
    Xor,
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
    Comma,
    Dot,
    Exclamation,
    Question,
    Dollar,

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
    size_t lastpos;
    size_t endpos;

    Token(TokenKind kind = TokenKind::Int);
    Token(TokenKind kind, Token* prev, size_t pos);

    Token* insert(TokenKind kind, int pos_diff, std::string_view const& str);

    static
      std::vector<std::pair<char const*, PunctuatorKind>> const
        punctuator_string_table;
  };
}

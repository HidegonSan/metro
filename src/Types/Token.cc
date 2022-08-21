#include <vector>
#include "Types/Token.h"

namespace metro {

/* 文字数が多い方から並べてください */
std::vector<std::pair<char const*, PunctuatorKind>> const
 Token::punctuator_string_table {
  /* deprecated */ { "->", PunctuatorKind::SpecifyReturnType },

  { "...", PunctuatorKind::VariableLengthArgument },
  { "[]", PunctuatorKind::ArrayType },

  { "+", PunctuatorKind::Add },
  { "-", PunctuatorKind::Sub },
  { "*", PunctuatorKind::Mul },
  { "%", PunctuatorKind::Mod },
  { "/", PunctuatorKind::Div },
  { "&", PunctuatorKind::And },
  { "|", PunctuatorKind::Or },
  { "~", PunctuatorKind::Not },
  { "^", PunctuatorKind::Xor },
  { "=", PunctuatorKind::Assign },

  { ":", PunctuatorKind::CurlyBracketOpen },
  { ";", PunctuatorKind::Semicolon },
  { ",", PunctuatorKind::Comma },
  { ".", PunctuatorKind::Dot },
  { "!", PunctuatorKind::Exclamation },
  { "?", PunctuatorKind::Question },
  { "$", PunctuatorKind::Dollar },

  { "(", PunctuatorKind::RoundBracketOpen },
  { ")", PunctuatorKind::RoundBracketClone },
  { "{", PunctuatorKind::CurlyBracketOpen },
  { "}", PunctuatorKind::CurlyBracketClone },
  { "[", PunctuatorKind::SquareBracketOpen },
  { "]", PunctuatorKind::SquareBracketClone },
  { "<", PunctuatorKind::AngleBracketOpen },
  { ">", PunctuatorKind::AngleBracketClone },


};

Token::Token(TokenKind kind)
  : kind(kind),
    prev(nullptr),
    next(nullptr),
    pos(0),
    lastpos(0),
    endpos(0)
{
}

Token::Token(TokenKind kind, Token* prev, size_t pos)
  : Token(kind)
{
  this->prev = prev;
  this->pos = pos;

  if( prev ) prev->next = this;
}

Token* Token::insert(TokenKind kind, int pos_diff, std::string_view const& str) {
  auto tok = new Token(kind);

  tok->pos = this->pos + pos_diff;
  tok->str = str;

  tok->prev = this;
  tok->next = this->next;

  this->next = tok;
  return tok;
}

} // namespace metro

#include <vector>
#include "Types/Token.h"

namespace Metro {
  std::vector<std::pair<char const*, PunctuatorKind>> const Token::punctuator_strtable {
    { "->", PunctuatorKind::SpecifyReturnType },

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
}
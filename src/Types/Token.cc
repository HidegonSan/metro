#include <vector>
#include "Types/Token.h"

namespace Metro {
  std::vector<std::pair<char const*, PunctuatorKind>> const Token::punctuator_strtable {
    { "+", PunctuatorKind::Add },
    { "-", PunctuatorKind::Sub },
    { "*", PunctuatorKind::Mul },
    { "%", PunctuatorKind::Mod },
    { "/", PunctuatorKind::Div },
    { "=", PunctuatorKind::Assign },

    { ":", PunctuatorKind::CurlyBracketOpen },
    { ";", PunctuatorKind::Semicolon },

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
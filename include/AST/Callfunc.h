// -----------------------
//  関数呼び出し

//  callee, callee_builtin は Sema で設定され、実行時にどちらかが呼ばれる
// -----------------------
#pragma once

namespace metro::AST {
  struct Function;
  struct CallFunc : Base {
    //std::string_view    name;

    AST::Base* expr;
    std::vector<Base*>  args;

    Function*           callee;           // 呼び出し先: ユーザー定義関数
    BuiltinFunc const*  callee_builtin;   // 呼び出し先: 組み込み

    std::string to_string() const;
    SourceRange get_range_on_source() const;

    CallFunc(Token* token, AST::Base* expr)
      : Base(Kind::Callfunc, token),
        expr(expr),
        callee(nullptr),
        callee_builtin(nullptr)
    {
    }
  };
}

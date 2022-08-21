/* -----------------------------
//  エラー 作成・表示

//  チェーン呼び出しでヘルプを追加する
//  最後に emit() とすること

Error(...)
  .add_help(ast1, "hello")
  .add_help(ast2, "good morning")
  .emit()
 ------------------------------- */
#pragma once

#include <vector>
#include "AppContext.h"

namespace metro {

struct Token;
namespace AST {
  struct Base;
}

enum class ErrorKind {
  Note,
  Warning,
  InvalidToken,
  InvalidSyntax,
  UninitializedValue,
  ExpectedToken,
  ExpectedSemicolon,
  NotAllowed,
  NotMutable,
  Undefined,
  MultipleDefinition,
  IndefiniteType,
  UnknownTypeName,
  CannotInfer,
  MayNotBeEvaluated,
  ValueType,
  TypeMismatch,
  TooFewArguments,
  TooManyArguments,
  InvalidArguments,
  StackOverflow,
  EmptyStruct
};

class Error {
  struct Help {
    AST::Base* ast;
    std::string&& msg;

    explicit Help(AST::Base* ast, std::string&& msg)
      : ast(ast),
        msg(std::forward<std::string>(msg))
    {
    }
  };

  struct ErrLocation {
    struct Line {
      size_t linenum;

    };
  };

public:
  explicit Error(ErrorKind kind, Token* token, std::string&& msg);
  Error(ErrorKind kind, AST::Base* ast, std::string&& msg);

  //
  // add_help
  //   ヘルプを追加する
  //   チェーンでつなげて複数個 追加できる
  Error& add_help(AST::Base* ast, std::string&& msg);

  //
  // emit
  //   出力する
  void emit(bool exit = false);

  //
  // この関数が呼ばれる前までにおいて
  // エラーが出力されたかどうかを確認し、されていたらアプリを終了する
  static void check();

private:


  ErrorKind kind;
  std::string&& message;

  Token*      token;
  AST::Base*  ast;

  AppContext::Script const* script;

  std::vector<Help> helps;

  static void Error::emit_error(Error& err);

};

}

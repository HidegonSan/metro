/* -----------------------------
//  エラー 作成・表示

//  チェーン呼び出しでヘルプを追加する
//  最後に emit() とすること

Error(...)
  .add_help(token, "try remove this")
  .emit()
 ------------------------------- */
#pragma once

#include <vector>
#include "AppContext.h"
#include "source.h"

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
  UndefinedVariable,
  UndefinedFunction,
  UndefinedTypeName,

  MultipleDefinition,
  IndefiniteType,
  UnknownTypeName, // deprecated
  CannotInfer,
  MayNotBeEvaluated,
  ValueType,
  TypeMismatch,

  TooFewArguments,
  TooManyArguments,
  InvalidArguments,

  EmptyStruct,

  StackOverflow,
};

class Error {
  struct Help {
    AST::Base* ast;
    std::string&& msg;
    Source::LineLoc* lineloc;

    explicit Help(AST::Base* ast, std::string&& msg)
      : ast(ast),
        msg(std::forward<std::string>(msg))
    {
    }
  };

public:
  explicit Error(ErrorKind kind, Token* token, std::string&& msg);
  Error(ErrorKind kind, AST::Base* ast, std::string&& msg);
  Error(ErrorKind kind, size_t pos, std::string&& msg);

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
  // check
  //   この関数が呼ばれる前までに
  //   エラーが出力されたかどうかを確認し、されていたらアプリを終了する
  static void check();

private:
  ErrorKind kind;
  std::string&& message;

  AppContext::Script const* script;

  Token*      token;
  AST::Base*  ast;

  std::vector<Help> helps;

  static void Error::emit_error(Error& err);

};

} // namespace metro

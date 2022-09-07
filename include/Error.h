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
  LanguageVersion,

  InvalidToken,
  InvalidSyntax,
  UninitializedValue,
  UnexpectedToken,
  
  NotAllowed,
  NotMutable,

  UndefinedVariable,
  UndefinedFunction,
  UndefinedTypeName,

  IfWithoutElse,
  MultipleDefinition,
  CannotInfer,
  TypeMismatch,

  TooFewArguments,
  TooManyArguments,
  InvalidArguments,

  EmptyStruct,

  // runtime
  StackOverflow,

  ApplicationBug,

  // warnings
  UnusedVariable,
};

class Error {
  struct Help {
    Token* token;
    AST::Base* ast;
    std::string&& msg;

    explicit Help(Token* token, std::string&& msg)
      : token(token),
        ast(nullptr),
        msg(std::forward<std::string>(msg))
    {
    }

    Help(AST::Base* ast, std::string&& msg)
      : token(nullptr),
        ast(ast),
        msg(std::forward<std::string>(msg))
    {
    }
  };

public:
  explicit Error(ErrorKind kind, Token* token, std::string&& msg);
  Error(ErrorKind kind, AST::Base* ast, std::string&& msg);
  Error(ErrorKind kind, size_t pos, std::string&& msg);

  Error& set_warn() {
    this->is_warn = true;
    return *this;
  }

  Error& no_msg() {
    this->message.clear();
    return *this;
  }

  //
  // add_help
  //   ヘルプを追加する
  //   チェーンでつなげて複数個 追加できる
  Error& add_help(Token* token, std::string&& msg);
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

  bool is_warn;

  AppContext::Script const* script;

  Token*      token;
  AST::Base*  ast;
  size_t      pos;

  std::vector<Help> helps;

  static size_t emitted_count;

};

} // namespace metro

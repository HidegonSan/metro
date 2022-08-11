#pragma once

#include <vector>
#include "AppContext.h"

namespace Metro {
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
    Undefined,
    MultipleDefinition,
    IndefiniteType,
    UnknownTypeName,
    CannotInfer,
    MayNotbeEvaluated,
    ValueType,
    TypeMismatch,
    TooFewArguments,
    TooManyArguments,
    InvalidArguments,
    StackOverflow,
    EmptyStruct
  };

  struct ErrorContext {
    ErrorKind kind;

    Token*      token;
    AST::Base*  node;

    // line of error
    size_t  view_begin;
    size_t  view_end;
    std::string_view  src_view;

    // range of error
    size_t  err_pos;
    size_t  err_begin;
    size_t  err_end;
    size_t  err_underline_len;

    // index of line
    size_t  linenum;

    std::string message;

    AppContext::Script const* script;

    void show();
  };

  class Error {
  public:
    static ErrorContext& add_error(ErrorKind kind, size_t pos, std::string&& msg);
    static ErrorContext& add_error(ErrorKind kind, Token* token, std::string&& msg);
    static ErrorContext& add_error(ErrorKind kind, AST::Base* ast, std::string&& msg);

    static void show_all();

    static void check();

    [[noreturn]]
    static void exit_app();

    static std::vector<ErrorContext> const& get_ctx_list();

  private:
    static std::vector<ErrorContext> contexts;
  };
}
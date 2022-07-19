#pragma once

namespace Metro {
  struct Token;
  struct Node;
  
  enum ErrorKind {
    ERR_INVALID_TOKEN,
    ERR_INVALID_SYNTAX,
    ERR_TYPE_MISMATCH,
    ERR_UNDEFINED_FUNC,
  };

  struct ErrorContext {
    ErrorKind kind;

    Token*  token;
    Node*   node;

    size_t  view_begin;
    size_t  view_end;
    std::string_view  src_view;

    size_t  err_pos;
    size_t  err_begin;
    size_t  err_end;

    std::string message;

    void show();
  };

  class Error {
  public:
    static void add_error(ErrorKind kind, size_t pos, std::string const& msg);
    static void add_error(ErrorKind kind, Token* token, std::string const& msg);
    static void add_error(ErrorKind kind, Node* node, std::string const& msg);

    static void show_all();

    static void check();
    static void exit_app();

    static std::vector<ErrorContext> const& get_ctx_list();

  private:
    static std::vector<ErrorContext> contexts;
  };
}
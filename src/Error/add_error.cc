#include <vector>
#include "Types.h"
#include "Application.h"
#include "Error.h"

namespace Metro {
  std::vector<ErrorContext> Error::contexts;

  static void init_context(ErrorContext& ctx, size_t errbegin, size_t errend) {

    auto script = Application::get_running_script();

    ctx.script = script;
    ctx.linenum = 1;

    // line
    for( size_t i = 0; i < script->data.length(); i++ ) {
      if( script->data[i] == '\n' ) {
        if( errend <= i ) {
          ctx.view_end = i;
          break;
        }

        ctx.view_begin = i + 1;
        ctx.linenum++;
      }
    }

    // error range
    ctx.err_begin = errbegin;
    ctx.err_end = errend;

    // underline length
    ctx.err_underline_len = errend - errbegin;

  }

  ErrorContext& Error::add_error(ErrorKind kind, size_t pos, std::string&& msg) {
    auto& ctx = contexts.emplace_back();

    ctx.kind = kind;
    ctx.err_pos = pos;
    ctx.message = std::move(msg);

    init_context(ctx, pos, pos + 1);

    return ctx;
  }

  ErrorContext& Error::add_error(ErrorKind kind, Token* token, std::string&& msg) {
    auto& ctx = contexts.emplace_back();

    ctx.kind = kind;
    ctx.token = token;
    ctx.err_pos = token->pos;
    ctx.message = std::move(msg);

    init_context(ctx, token->pos, token->endpos);

    return ctx;
  }

  ErrorContext& Error::add_error(ErrorKind kind, AST::Base* ast, std::string&& msg) {
    auto& ctx = contexts.emplace_back();

    ctx.kind = kind;
    ctx.token = ast->token;
    ctx.message = std::move(msg);

    auto range = ast->get_range_on_source();

    init_context(ctx, range.first, range.second);

    return ctx;
  }
}
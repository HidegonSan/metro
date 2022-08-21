#include "AST.h"
#include "MetroDriver/sema.h"
#include "Error.h"
#include "Debug.h"
#include "Utils.h"

namespace metro {

/*
// ----------------------------------------------------
// 関数の戻り値の型を推論する

fn func() int {
          ^^^ このように指定されている場合は、

// ----------------------------------------------------
*/

void Sema::analyze_func_return_type(ValueType& out, AST::Function* func) {
  std::vector<AST::Base*> lastexpr_list;

  // func->code からすべての last-expr を取得
  get_lastval_full(lastexpr_list, func->code);

  // 型が指定されている場合、すべての last-expr がそれと同じであることを確認し、帰る
  if( func->return_type != nullptr ) {
    out = walk(func->return_type);
    expect_all_same_with(lastexpr_list, out);
    return;
  }

  //
  // 以下、推論する処理
  //

  // last-expr のイテレータ ( func->code 内 )
  auto it = lastexpr_list.begin();

  for( ; it != lastexpr_list.end(); it++ ) {
    // 自己再帰する式が含まれている場合、飛ばす
    if( contains_callfunc_in_expr(func->name, *it) ) {
      continue;
    }

    auto&& tmp = walk(*it);

    alertios("last-expr of " << func->name << ": '" << tmp.to_string() << "' from " << (*it)->to_string());

    // 最初
    if( it == lastexpr_list.begin() ) {
      out = tmp;
    }
    // 2 回目からは、最初と同じ型でない場合エラー
    else if( !out.equals(tmp) ) {
      // Error::add_error(ErrorKind::TypeMismatch, *it,
      //   Utils::linkstr("expected '", out.to_string(), "', buf found '", tmp.to_string(), "'")
      // );

      Error(
        ErrorKind::TypeMismatch, *it,
        Utils::linkstr("expected '", out.to_string(), "', buf found '", tmp.to_string(), "'")
      )
        .emit();
    }
  }

  // 1 個も推論できなかったら、エラー
  if( it == lastexpr_list.begin() ) {
    Error::add_error(ErrorKind::CannotInfer, func->token, "cannot infer the return type of function");
    Error::exit_app();


  }

  alertios("function " << func->name << ": return-type = " << out.to_string());
}

} // namespace metro



#include "AST.h"
#include "MetroDriver/sema.h"
#include "Error.h"
#include "Debug.h"
#include "Utils.h"

namespace metro {

/*
// ----------------------------------------------------
// 関数の戻り値の型を推論する

引数 :
  out   = 推論された型が代入される
  func  = 戻り値の型を推論したい関数

fn func() int {
          ^^^
 /-------/
 |
 - このように指定されている場合はこれをそのまま使用する

指定がない場合
  --> コード内で last-expr が 1 個しかなければそれを使う
      複数個あれば、最初の型を適用し、それ以降は最初と同じでない場合エラー

// ----------------------------------------------------
*/

void Sema::analyze_func_return_type(ValueType& out, AST::Function* func) {
  std::vector<AST::Base*> lastlist;

  // func->code からすべての last-expr を取得
  get_lastval_full(lastlist, func->code);

  // 型が指定されている場合、すべての last-expr がそれと同じであることを確認し、帰る
  if( func->return_type != nullptr ) {
    out = walk(func->return_type);
    expect_all_same_with(lastlist, out);
    return;
  }

  //
  // 以下、推論する処理
  //

  // last-expr のイテレータ ( func->code 内 )
  auto it = lastlist.begin();

  // lastlist 内に 1 個しかなければそれを適用
  if( lastlist.size() == 1 ) {
    alertwhere;
    alertios("");
    out = walk(*it);
    return;
  }

  for( ; it != lastlist.end(); it++ ) {
    // 自己再帰する式が含まれている場合、飛ばす
    if( contains_callfunc_in_expr(func->name, *it) ) {
      continue;
    }

    auto&& tmp = walk(*it);

    alertios("last-expr of " << func->name << ": '" << tmp.to_string() << "' from " << (*it)->to_string());

    // 最初
    if( it == lastlist.begin() ) {
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
  // if( it == lastlist.begin() ) {
  //   Error::add_error(ErrorKind::CannotInfer, func->token, "cannot infer the return type of function");
  //   Error::exit_app();
  // }

  /* 1 個も推論できない場合ってあるか？
   空スコープだとしてもそれ自体がひとつだけ lastlist に含まれるはず */
  assert(lastlist.size() >= 1);




  alertios("function " << func->name << ": return-type = " << out.to_string());
}

} // namespace metro



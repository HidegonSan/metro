#include "AST.h"
#include "MetroDriver/sema.h"
#include "Error.h"
#include "Debug.h"
#include "Utils.h"

namespace metro {

/* ----------------------------------------------------
// analyze the return type of function

return :
  func  = 戻り値の型を推論したい関数


fn func() int {
          ^^^
         /
    if already specified, return it


 /-------/
 |
 - このように指定されている場合はこれをそのまま使用する


指定がない場合
  --> コード内で last-expr が 1 個しかなければそれを使う
      複数個あれば、最初の型を適用し、それ以降は最初と同じでない場合エラー

// ---------------------------------------------------- */

ValueType Sema::analyze_func_return_type(AST::Function* func) {
  ValueType ret;

  // func->code からすべての last-expr を取得
  auto lastlist = get_final_expr_full(func->code);

  // 型が指定されている場合、すべての last-expr がそれと同じであることを確認し、帰る
  if( func->return_type != nullptr ) {
    ret = this->walked[func] = walk(func->return_type);
    expect_all_same_with(lastlist, ret);
    return ret;
  }

  //
  // 以下、推論する処理
  //

  auto it = lastlist.begin();

  for( ; it != lastlist.end(); it++ ) {
    // skip if contains self call
    if( contains_callfunc_in_expr(func->name, *it) )
      continue;

    auto&& tmp = walk(*it);

    // first
    if( it == lastlist.begin() ) {
      ret = tmp; // set type
    }
    // since second, must same to first type
    else if( !ret.equals(tmp) ) {
      Error(
        ErrorKind::TypeMismatch, *it,
        Utils::linkstr("expected '", ret.to_string(), "', buf found '", tmp.to_string(), "'")
      )
        .emit();
    }
  }

  // 1 個も推論できなかったら、エラー
  if( it == lastlist.begin() ) {
    Error(ErrorKind::CannotInfer, func->token, "cannot infer the return type of function")
      .emit(true);
  }


  // alertios("function " << func->name << ": return-type = " << out.to_string());

  return ret;
}

} // namespace metro



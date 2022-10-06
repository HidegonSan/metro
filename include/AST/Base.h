#pragma once

namespace metro::AST {

using namespace std::string_literals;
using SourceRange = std::pair<Token*, Token*>;

struct Base {
  Kind    kind;
  Token*  token;
  bool    is_expr;

  // 空の配列であるかどうか
  bool is_empty_array() const;

  // 終端かどうか
  // ( 終端になるクラスにてオーバーロードして true を返すようにしてください )
  bool is_factor() const;

  // 文字列に変換 (現時点ではデバッグ用)
  virtual std::string to_string() const = 0;

  // ソースコード上での範囲を取得する
  virtual SourceRange get_range_on_source() const;

protected:
  explicit Base(Kind kind, Token* token)
    : kind(kind),
      token(token),
      is_expr(false)
  {
  }

  virtual ~Base() { }
};

} // namespace metro::AST

#pragma once

namespace metro::AST {
  struct Base {
    Kind    kind;
    Token*  token;
    bool    is_expr;

    //
    // 空の配列であるかどうか
    bool is_empty_array() const;

    //
    // 文字列に変換 (現時点ではデバッグ用)
    virtual std::string to_string() const = 0;

    //
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
}

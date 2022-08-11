#pragma once

// -----------------------
//  比較式

//  3 個以上並べることができる
//  first に最初の式が割り当てられ、それ以降は演算子と一緒に Item として vector に追加される

//  例えば、"1 < 2 != 3" の場合は
//    {1, {{BiggerRight, 2}, {NotEqual, 3}}
//  のようになる
// -----------------------

namespace Metro::AST {
  struct Compare : Base {
    struct Item {
      enum class Kind {
        BiggerLeft,
        BiggerRight,
        BiggerOrEqualLeft,
        BiggerOrEqualRight,
        Equal,
        NotEqual
      };

      Kind    kind;
      Token*  token;  // operator
      Base*   ast;    // expression

      explicit Item(Item::Kind kind, Token* token, Base* ast)
        : kind(kind),
          token(token),
          ast(ast)
      {
      }
    };

    Base*               first;
    std::vector<Item>   list;

    Item& append(Item::Kind kind, Token* tok, Base* ast);

    std::string to_string() const;
    SourceRange get_range_on_source() const;

    static Compare* create(Base*& ast);

    explicit Compare(Base* first)
      : Base(Kind::Compare, first->token),
        first(first)
    {
    }
  };
}
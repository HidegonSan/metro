#pragma once

#include <algorithm>
#include <list>
#include <string>
#include <vector>

namespace metro {

struct Node;

struct ValueType {
  enum class Kind {
    Int,
    Float,
    Bool,
    Char,
    String,
    Tuple,
    UserDef,
    Args,
    None
  };

  Kind kind;
  size_t arr_depth;
  bool have_elements;
  std::vector<ValueType> elems;
  Node* userdef_struct;

  bool is_mutable;
  bool is_reference;

  ValueType(Kind kind = Kind::None)
      : kind(kind),
        arr_depth(0),
        have_elements(false),
        userdef_struct(nullptr),
        is_mutable(false),
        is_reference(false) {}

  bool is_no_extents() const {

  }

  bool equals(Kind kind) const {

  }

  bool equals(ValueType const& type) const {

  }

  std::string to_string() const;

  static std::vector<std::pair<char const*, Kind>> const name_table;
};

struct Object {
  ValueType type;
  size_t ref_count;
  bool is_weak;

  union {
    int64_t v_int;
    double v_float;
    bool v_bool;
    char16_t v_char;
  };

  std::u16string v_str;
  std::vector<Object*> list;

  static Object* none;

  Object(ValueType type = ValueType::Kind::None);
  ~Object();

  std::string to_string() const;

  Object* clone() const;
};

enum class TokenKind {
  Int,
  Float,
  Char,
  String,
  Ident,
  Punctuator,
  End
};

enum class PunctuatorKind {
  VariableLengthArgument,
  SpecifyReturnType,  // ->
  ArrayType,          // []

  Add,
  Sub,
  Mul,
  Mod,
  Div,
  And,
  Or,
  Not,
  Xor,
  Assign,  // =

  Equal,
  NotEqual,
  // BiggerLeft,
  // BiggerRight,
  BigOrEqualLeft,
  BigOrEqualRight,
  ShiftLeft,
  ShiftRight,

  Colon,
  Semicolon,
  Comma,
  Dot,
  Exclamation,
  Question,
  Dollar,

  RoundBracketOpen,    // (
  RoundBracketClone,   // )
  CurlyBracketOpen,    // {
  CurlyBracketClone,   // }
  SquareBracketOpen,   // [
  SquareBracketClone,  // ]
  AngleBracketOpen,    // <
  AngleBracketClone,   // >
};

struct Token {
  TokenKind kind;
  PunctuatorKind pu_kind;
  Token* prev;
  Token* next;
  std::string_view str;
  size_t pos;
  size_t lastpos;
  size_t endpos;

  Token(TokenKind kind = TokenKind::Int);
  Token(TokenKind kind, Token* prev, size_t pos);

  Token* insert(TokenKind kind, int pos_diff, std::string_view const& str);

  static std::vector<std::pair<char const*, PunctuatorKind>> const
      punctuator_string_table;
};

enum class NodeKind {
  None,  // nothing

  Type,      // typename
  Argument,  // argument in function

  Boolean,
  Value,
  Variable,

  Array,
  Tuple,

  Callfunc,
  Subscript,
  MemberAccess,

  Mul,
  Div,
  Add,
  Sub,
  Compare,
  Assign,

  Return,
  If,
  VarDefine,
  For,
  Loop,
  While,

  Scope,

  Function,
  Struct,
};

__attribute__((packed)) struct Node {
  NodeKind kind;
  Token* token;

  union {
    Node* child;

    struct {
      Node* lhs;
      Node* rhs;
    };
  };

  std::vector<Node*> nodes;
};

}  // namespace metro

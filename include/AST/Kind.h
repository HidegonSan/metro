#pragma once

namespace metro::AST {

enum class Kind {
  Type,
  Argument,

  None,
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

} // namespace metro::AST

#pragma once

namespace metro::AST {

enum class Kind : int {
  Type,
  Argument,

  None,
  Boolean,
  Value,
  Variable,

  Cast,

  Array,
  Tuple,

  Callfunc,
  Subscript,
  MemberAccess,

  Mul,
  Div,

  Add,
  Sub,

  LShift,
  RShift,

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

}  // namespace metro::AST

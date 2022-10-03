#pragma once

#include <vector>

namespace metro {

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

  std::vector<Node*> nodes;
};

}  // namespace metro

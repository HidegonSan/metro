#pragma once

namespace metro::AST {
  enum class Kind {
    None,
    Type,
    Argument,
    Member,

    Boolean,
    Value,
    Variable,

    Array,
    Tuple,
    // EmptyArray, // TODO: impl

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
    EmptyScope,

    Function,
    Struct,
  };
}

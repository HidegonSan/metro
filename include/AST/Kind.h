#pragma once

namespace Metro::AST {
  enum class Kind {
    None,
    Type,
    Argument,
    Member,

    Boolean,
    Value,
    Array,

    Variable,
    Callfunc,

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
}
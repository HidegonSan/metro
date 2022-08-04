#pragma once

namespace Metro::AST {
  enum class Kind {
    None,
    Type,
    Argument,
    Member,
    
    Boolean,
    Value,
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
    Let,
    For,
    Loop,
    While,
    Scope,

    Function,
    Struct,
  };
}
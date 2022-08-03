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

    Mul,
    Div,
    Add,
    Sub,
    Compare,
    Assign,

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
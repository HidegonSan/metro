#pragma once

namespace Metro::AST {
  struct None : Base {
    static None* val;

    std::string to_string() const;

  private:
    None() { }
  };
}
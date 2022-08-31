#pragma once

namespace metro::AST {

struct ListBase : Base {
  std::vector<Base*> elements;

protected:
  using Base::Base;

  std::string get_elems_string() const;
};

} // namespace metro::AST


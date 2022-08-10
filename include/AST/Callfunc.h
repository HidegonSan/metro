#pragma once

namespace Metro {
  struct BuiltinFunc;
}

namespace Metro::AST {
  struct Function;
  struct CallFunc : Base {
    std::string_view    name;
    std::vector<Base*>  args;
    Function*           callee;
    BuiltinFunc const*  callee_builtin;

    std::string to_string() const {
      auto ret = "<Callfunc '" + std::string(name) + "'";

      if( !args.empty() ) {
        ret += " " + Utils::join<AST::Base*>(", ", args, [] (auto x) {return x->to_string();});
      }

      return ret + ">";
    }

    CallFunc(Token* token)
      : Base(Kind::Callfunc, token),
        callee(nullptr),
        callee_builtin(nullptr)
    {
    }
  };
}
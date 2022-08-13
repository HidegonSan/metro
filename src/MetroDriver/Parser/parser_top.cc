#include "Types.h"
#include "MetroDriver/Parser.h"
#include "Error.h"

namespace Metro {
  AST::Base* Parser::toplevel() {
    if( eat("fn") ) {
      auto ast = new AST::Function(cur);

      expect_ident();
      ast->name = cur->str;

      next();
      expect("(");

      if( !eat(")") ) {
        do {
          auto& arg = ast->args.emplace_back(cur);

          expect_ident();
          arg.name = cur->str;
          arg.token = cur;

          next();
          expect(":");

          arg.type = expect_type();
        } while( eat(",") );

        expect(")");
      }

      if( eat("->") ) {
        ast->return_type = expect_type();
      }

      ast->code = expect_scope();

      return ast;
    }

    if( eat("struct") ) {
      auto x = new AST::Struct(ate);

      expect_ident();
      x->name = cur->str;

      next();
      expect("{");

      if( cur->str == "}" ) {
        Error::add_error(ErrorKind::EmptyStruct, cur, "empty struct is not valid");
        Error::exit_app();
      }

      do {
        expect_ident();

        auto& member = x->members.emplace_back();
        member.name = cur->str;
        member.token = cur;

        next();
        expect(":"); // TODO: add template type if didnt eat

        member.type = expect_type();
      } while( eat(",") );

      expect("}");

      return x;
    }

    return expr();
  }
}
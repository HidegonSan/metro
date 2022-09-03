#include "Types.h"
#include "MetroDriver/parser.h"
#include "Error.h"

namespace metro {

AST::Base* Parser::toplevel() {

  //
  // function
  if( eat("fn") ) {
    auto ast = new AST::Function(this->ate);

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

    if( this->cur->str != "{" )
      ast->return_type = expect_type();

    ast->code = expect_scope();

    return ast;
  }

  //
  // structure
  if( eat("struct") ) {
    auto x = new AST::Struct(ate);

    this->expect_ident();
    x->name = cur->str;

    this->next();
    this->expect("{");

    if( this->eat("}") ) {
      Error(ErrorKind::EmptyStruct, cur, "empty struct is not valid").emit();
      return x;
    }

    do {
      this->expect_ident();

      auto& member = x->members.emplace_back();
      member.name = cur->str;
      member.token = cur;

      this->next();
      this->expect(":"); // TODO: If not eaten, add template type

      member.type = this->expect_type();
    } while( eat(",") );

    this->expect("}");

    return x;
  }

  return expr();
}

} // namespace metro

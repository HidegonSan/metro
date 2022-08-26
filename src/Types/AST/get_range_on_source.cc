#include "Types.h"
#include "AST.h"

namespace metro::AST {

SourceRange Base::get_range_on_source() const {
  return { token, token };
}

SourceRange Argument::get_range_on_source() const {
  return { token, type->get_range_on_source().second };
}

SourceRange Array::get_range_on_source() const {
  if( elements.empty() )
    return { token, token->next };

  return {
    elements[0]->get_range_on_source().first,
    (*elements.rbegin())->get_range_on_source().second
  };
}

SourceRange CallFunc::get_range_on_source() const {
  if( args.empty() )
    return { token, token->next->next }; /* name, ')' */

  return {
    token,
    (*args.rbegin())->get_range_on_source().second->next
  };
}

SourceRange Compare::get_range_on_source() const {
  return {
    first->get_range_on_source().first,
    list.rbegin()->ast->get_range_on_source().second
  };
}

SourceRange Expr::get_range_on_source() const {
  return { lhs->get_range_on_source().first, rhs->get_range_on_source().second };
}

SourceRange For::get_range_on_source() const {
  return { token, code->get_range_on_source().second };
}

SourceRange Function::get_range_on_source() const {
  return { token, code->get_range_on_source().second };
}

SourceRange If::get_range_on_source() const {
  return { token, (if_false ? if_false : if_true)->get_range_on_source().second };
}

SourceRange Return::get_range_on_source() const {
  return { token, expr->get_range_on_source().second };
}

SourceRange Scope::get_range_on_source() const {
  if( elems.empty() )
    return { token, token->next };

  return { token, (*elems.rbegin())->get_range_on_source().second->next };
}

SourceRange Struct::get_range_on_source() const {
  return { token, members.rbegin()->type->get_range_on_source().second };
}

SourceRange Type::get_range_on_source() const {
  auto tok = token;

  if( !elems.empty() ) {
    tok = tok->next;

    for( int i = 1; i > 0; tok = tok->next ) {
      if( tok->str == "<" ) {
        i++;
      }
      else if( tok->str == ">" ) {
        if( --i == 0 ) break;
      }
    }

    tok = tok->next;
  }

  if( is_mutable )
    tok = tok->next;

  if( is_reference )
    tok = tok->next;

  return { token, tok };
}

SourceRange VarDefine::get_range_on_source() const {
  auto end = token->next; // name

  if( init )
    end = init->get_range_on_source().second;
  else if( type )
    end = type->get_range_on_source().second;

  return { token, end };
}

SourceRange Tuple::get_range_on_source() const {
  if( elements.empty() )
    return { token, token->next };

  return {
    elements[0]->get_range_on_source().first,
    (*elements.rbegin())->get_range_on_source().second
  };
}

} // namespace metro

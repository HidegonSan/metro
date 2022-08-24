#include "Types.h"
#include "AST.h"

namespace metro::AST {

SourceRange Base::get_range_on_source() const {
  return { token->pos, token->pos + token->str.length() };
}

SourceRange Argument::get_range_on_source() const {
  return { token->pos, type->get_range_on_source().second };
}

SourceRange Array::get_range_on_source() const {
  if( elements.empty() ) {
    return { token->pos, token->next->endpos };
  }

  return { elements[0]->get_range_on_source().first,
    (*elements.rbegin())->get_range_on_source().second };
}

SourceRange CallFunc::get_range_on_source() const {
  if( args.empty() ) {
    return { token->pos, token->next->next->endpos };
  }

  return {
    token->pos,
    (*args.rbegin())->get_range_on_source().second + 1
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
  return { token->pos, code->get_range_on_source().second };
}

SourceRange Function::get_range_on_source() const {
  return { token->pos, code->get_range_on_source().second };
}

SourceRange If::get_range_on_source() const {
  return { token->pos, (if_false ? if_false : if_true)->get_range_on_source().second };
}

SourceRange Return::get_range_on_source() const {
  return { token->pos, expr->get_range_on_source().second };
}

SourceRange Scope::get_range_on_source() const {
  if( elems.empty() ) {
    return { token->pos, token->next->endpos };
  }

  return { token->pos, (*elems.rbegin())->get_range_on_source().second };
}

SourceRange Struct::get_range_on_source() const {
  return { token->pos, members.rbegin()->type->get_range_on_source().second };
}

SourceRange Type::get_range_on_source() const {
  auto endtok = token;

  if( !elems.empty() ) {
    for( int i = 0;; endtok = endtok->next ) {
      if( endtok->str == "<" ) {
        i++;
      }
      else if( endtok->str == ">" ) {
        if( --i == 0 ) break;
      }
    }

    endtok = endtok->next;
  }

  if( is_mutable ) {
    endtok = endtok->next;
  }

  if( is_reference ) {
    endtok = endtok->next;
  }

  return { token->pos, endtok->endpos };
}

SourceRange VarDefine::get_range_on_source() const {
  size_t end = token->next->endpos; // end of name

  if( init ) {
    end = init->get_range_on_source().second;
  }
  else if( type ) {
    end = type->get_range_on_source().second;
  }

  return { token->pos, end };
}

SourceRange Tuple::get_range_on_source() const {
  if( elements.empty() ) {
    return { token->pos, token->next->endpos };
  }

  return { elements[0]->get_range_on_source().first,
    (*elements.rbegin())->get_range_on_source().second };
}

} // namespace metro

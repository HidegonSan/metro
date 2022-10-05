#include "MetroDriver/Evaluator.h"

#include "AST.h"
#include "GC.h"
#include "MetroDriver/Sema.h"

namespace metro {

Evaluator::ScopeInfo::~ScopeInfo() {
  // todo: reduce ref_count in varibales
}

Evaluator::Evaluator() {}
Evaluator::~Evaluator() {}

}  // namespace metro

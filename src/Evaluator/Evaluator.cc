#include "MetroDriver/Evaluator.h"
#include "GC.h"

namespace metro {

Evaluator::Evaluator()
  : cur_scope(nullptr),
    cur_call_stack(nullptr)
{
}

} // namespace metro

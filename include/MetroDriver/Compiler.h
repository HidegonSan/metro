#pragma once

#include <list>
#include <map>
#include <vector>
#include <memory>
#include <stdexcept>
#include <concepts>
#include <functional>

namespace metro {

struct Token;
struct Object;
struct ValueType;

class Error;

namespace AST {

enum class Kind;

struct Base;
struct Type;
struct Argument;
struct Variable;

struct VarDefine;
struct Scope;
struct Function;

} // namespace metro::AST



} // namespace metro

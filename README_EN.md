# Metro: v0.0.1

[[日本語]](README.md) [[English]](README_EN.md)

A statically typed scripting language. Powerful and accurate type inference is its key feature.

## Usage

### Clone and build this repository

```bash
git clone --recursive git@github.com:bomkei/metro
cd metro
make -j
```

### Run

```bash
./metro [your script files]
```

## How to contribute

Welcome! Please read [this](CONTRIBUTE.md).

## Implementation

### Interpreter

No other libraries or tools such as yacc or lex are used.  
The original source code is written in C++, including the Lexer and Evaluator.

## Memory Safety

The garbage collector will manage the objects for you.

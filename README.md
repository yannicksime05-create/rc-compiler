# RC

`rc` is a small statically-typed toy language and its compiler, written in C++. Source
written in `rc` is lexed, parsed into an AST, type-checked by a semantic analyser, and
then **transpiled to C++**, which can be compiled with any C++11 (or later) compiler.

> This README describes the project as it currently stands, including parts that are
> still work-in-progress. See [Known limitations / TODO](#known-limitations--todo) for
> the honest state of things.

## Pipeline

```
source.rc
   │
   ▼
 Lexer            (src/lexer.cpp)        →  std::vector<Token>
   │
   ▼
 Parser           (src/parser.cpp)       →  AST (Program)
   │
   ├──▶ Printer            (src/printer.cpp)            – dumps the AST for debugging
   │
   ▼
 SemanticAnalyser (src/semantic_analyser.cpp) – scoping, type resolution & checking
   │
   ▼
 CppGenerator     (src/cpp_generator.cpp)     – emits C++ source (out.cpp)
   │
   ▼
 g++ -std=c++17 out.cpp -o out
```

All of the above stages implement a common `Visitor` interface (`include/ast.h`), so
adding a new pass over the AST just means writing a new `Visitor` subclass.

## Language features

### Types
`int`, `float`, `double`, `bool`, `string`, `void`, `auto`, `any` (backed by
`std::any` in the generated C++). Arrays are declared with a fixed size per
dimension:

```rc
int[3] arr = [1, 2, 3];
int[3][4] matrix;
```

`const` is supported as a qualifier and is enforced by the semantic analyser
(const/`any`/`auto` variables must be initialized at declaration).

### Declarations

```rc
int x = 5, y = 10;

int add(int a, int b = 1) {
    return a + b;
}
```

### Expressions
Full C-like precedence chain: comma, assignment (`= += -= *= /= %= &= |= ^= <<= >>=`),
ternary `?:`, `||`, `&&`, bitwise `| ^ &`, equality, comparison, shift, `+ -`, `* / %`,
prefix `! ~ - ++ --`, and postfix `() [] . ++ --`.

String values get a few overloaded operators when transpiled — see the
[String operators](#string-operators) section below.

### Statements
`{}` compound blocks, expression statements, `if`/`else`, `switch`/`case` (including
multi-label cases like `case 1, 2, 3:`), `while`, `do...while`, `for`, `break`, and
`return`.

`print(...)` supports a format-string form validated at semantic-analysis time by
counting `{}` placeholders against the number of arguments - see the [print](#print-placeholders-must-match) section below.

## String operators

`string` gets four extra operators, backed by overloads in
`include/string_overloads.h` (which every generated `.cpp` file `#include`s):

| Operator | Meaning | Left | Right | Result |
|---|---|---|---|---|
| `-` | trim | `string` | `string` | `string` |
| `*` | repeat | `string` | `int` | `string` |
| `/` | split | `string` | `string` or `int` | array of `string` |
| `+` | concatenate | `string` | `string`, `char`, or numeric | `string` |

`-`, `*=`, `/=` (and their `=`-suffixed compound-assignment forms) behave the same
way as the binary operator, just written in-place:

```rc
string s = "  hi  ";
s = s - " ";                   // trim: "hi"

string r = s * 3;              // repeat: "hihihi"

string[3] parts = s / 2;      // split using an int (the length of the chunks) → array of string: ["hi", "hi", "hi"]
                              // split can also take a string (see string_overloads.h)

string t = s + 5;             // concatenate (numeric side is widened to double)
```

### Quirks worth knowing

These came up while cross-checking `type_checker.cpp`'s rules against the actual
`operator` implementations in `string_overloads.h` — they're real, verified behavior,
not just theoretical edge cases:

- since **`/`** returns an array, you can't use **`/=`** on a string. Always use **` array = string / (string | int)`**
- c++ will show an **ambiguous overload** if you try this: **`string += numeric`** so for now you can only do: **`string = string + numeric`**
- **for all these operators, the left operand must be a named variable, not a literal.** Something like:
  `"  hi  " - " "` (trim
  applied directly to a string literal) won't compile; assign the literal to a
  variable first.

## Semantic rules & syntax notes

A few behaviors are enforced by `SemanticAnalyser` and aren't obvious just from the
grammar. These are the ones worth knowing before you hit them as an error message.

### `print` placeholders must match

If the **first argument to `print` is a string literal**, it's treated as a format
string: every `{}` in it must line up with one extra argument, in order, and every
extra argument must have a `{}` for it. Get this wrong and semantic analysis throws.

```rc
print();                // ok   – prints nothing
print(x);               // ok   – no format string, just streams x
print("x = {}", x);     // ok   – 1 placeholder, 1 argument
print("x = {");         // ok   – unmatched "{" alone isn't a placeholder
print("x = {}");        // ok   – "{}" present but no argument requested for it,
                        //        so it's ignored and printed as literal text
print("x = ", x);       // error – 0 placeholders but 1 argument provided
print("x = {", x);      // error – 0 placeholders but 1 argument provided
```

If the first argument **isn't** a string literal, no placeholder checking happens at
all — every argument is just streamed to `std::cout` in order.

### `auto` in functions

- `auto` is **forbidden as a parameter type**. `void f(auto x)` is a semantic error.
- `auto` **is** allowed as a return type. The type of the function's *first* `return`
  statement fixes what `auto` resolves to for the rest of the function; every later
  `return` must be type-compatible with that first one.

```rc
auto pick(bool b) {
    if(b) return 1;       // first return seen → auto resolves to int
    return 2.0;           // ok, downcasting from double to int

    return "oops";         // reported as a type mismatch against int
}
```

Note the asymmetry with most other checks in this codebase: mismatched later
`return`s are reported as non-fatal errors (printed to stderr), not thrown — so
compilation continues rather than aborting on the first bad `return`. Also, an
`auto`-returning function still needs **at least one** `return` statement; with zero
returns, analysis throws (same as any other non-`void` return type).

### `switch` case syntax

`switch` supports two ways of grouping case labels onto a shared body, and they can
be mixed:

- **Stacked keywords**, C-style fallthrough grouping: `case a: case b: { ... }`
- **Comma list**, all in one label: `case a, b, c: { ... }`

```rc
switch(n) {
    case 0:
    case 1: {           // stacked: 0 and 1 share this body
        break;
    }

    case 3, 4: {         // comma list: 3 and 4 share this body
        break;
    }

    case 5, 6:
    case 7, 8: {          // both forms combined: 5, 6, 7, 8 share this body
        break;
    }

    default: { break; }
}
```

Each `case` clause (stacked group or comma list) owns exactly one compound-statement
body — you can't attach a label without a `{ }` after it. As in C++, if a clause's
body doesn't `break`, execution falls through into the next clause.

## Example

`transpilation_test.rc`:
```rc
void greet(const string s) {
  print("Hi {}!\n", s);
}

int factoriel(int n) {
  if(n == 0 || n == 1) return 1;

  return n * factoriel(n - 1);
}

int main() {
  greet("Yannick");
  int n = factoriel(5);
  print("n = {}\n", n);

  int[3] arr = [1, 2, 3];

  switch(n) {
    case 0:
    case 1: {
      break;
    }

    case 3, 4: {

    }

    case 5, 6:
    case 7, 8: {

    }

    default: { break; }
  }

  return 0;
}
```

generates `out.cpp`:
```cpp
#include <iostream>
#include "include/string_overloads.h"
#include <any>

void greet(const std::string& s) {
    std::cout << "Hi " << s << "!\n";
}

int factoriel(int n) {
    if(n == 0 || n == 1) return 1;
    return n * factoriel(n - 1);
}

int main() {
    greet("Yannick");
    int n = factoriel(5);
    std::cout << "n = " << n << "\n";
    std::vector<int> arr = {1, 2, 3};
    switch(n) {
        case 0: 
        case 1: {
            break;
        }
        case 3: 
        case 4: {
        }
        case 5: 
        case 6: 
        case 7: 
        case 8: {
        }
        default: {
            break;
        }
    }
    return 0;
}
```

## Building & running

```sh
g++ -Iinclude main.cpp src/*.cpp -o rc
```

Running the compiler:
1. Lexes and parses `transpilation_test.rc`.
2. Prints the AST to stdout via `Printer` (debug output).
3. Runs `SemanticAnalyser::analyze()`.
4. Writes the transpiled C++ to `out.cpp`.
5. (Optional, currently commented out in `main.cpp`) invokes `g++` on `out.cpp`.

## Project structure

```
include/
  ast.h, ast_base.h        – AST node definitions + Visitor interface
  token.h                  – TokenType enum, Token/Location structs
  lexer.h                  – Lexer class
  parser.h                 – Parser class (Pratt/precedence-climbing expression parser)
  printer.h                – AST dump for debugging
  semantic_analyser.h      – scope-aware
  type_checker.h           – all types-related operations
  cpp_generator.h          – AST → C++ source
  scope.h, symbol.h, types.h – scope/symbol tables and the Type hierarchy
  string_overloads.h       – helper operators (trim/repeat/split) used by generated code
src/
  *.cpp                    – implementations for the above
ebnfs/
  declarations.txt, expressions.txt, statements.txt – grammar notes (partly aspirational,
  see limitations below)
tests/
  
main.cpp                   – wires the pipeline together
test.rc                    – current scratch input file for main.cpp
out.cpp                    – last generated output (checked in as a working example)
```

## Known limitations / TODO

This section exists so the README doesn't overstate what's implemented. Pulled
directly from gaps in the current source:

- **`RangeForStmt` isn't implemented in `CppGenerator`** (empty body), and its grammar
  isn't settled yet — `ebnfs/statements.txt` still has scratch notes debating syntax
  like `for(int i : [1..100])` vs `for(int i : <1..100>)`.
- **Grammar files describe more than the parser implements.** `ebnfs/declarations.txt`
  lists `class`, `struct`, `enum`, `namespace`, `typedef`/`using`, `template`,
  `concept`, `import`/`export`, `friend`, and `static_assert` declarations, and
  `ebnfs/statements.txt` lists `continue` and `labeled-statement` — none of these
  exist in the lexer/parser/AST yet.
- **The AST classes print verbose destructor logs** (`std::cout << "Cleaned up X
  node...\n"` in nearly every destructor) — useful during development, but worth
  gating behind a debug flag before this is used for anything larger.
- **`SemanticAnalyser` logs a lot of `std::cout` debug output** for resolved types;
  same note as above.

## Roadmap ideas

- Decide on and implement range-for syntax.
- Add `continue`, and align the parser with (or trim down) the EBNF files so they
  stay a source of truth rather than aspirational notes.
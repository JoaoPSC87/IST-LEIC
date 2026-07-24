# P6 Compiler

Compiler for the **p6** language, developed for **Compiladores** (Compilers), IST — 2025/26, **group 057**.

**Project grade: 17.61 / 20** (17.17 / 20 from the three deliveries alone; the remainder is the practical exam). The automatic-test suite passes **185 / 185**.

Built on the course's **CDK** (Compiler Development Kit) and **RTS** (run-time support) libraries. It translates p6 source into **Postfix** intermediate code / x86 assembly, which is then assembled with `yasm` and linked against the RTS.

## The p6 language

p6 is a small imperative language with a few unusual numeric types:

- **Integers** — *balanced ternary*, 40 trits (~64-bit range). Written in decimal, in base 9 (leading `0`), or as trits with `0t` followed by `+`/`0`/`-` (e.g. `0t+-0`).
- **Reals** — *Takum* floating point, 80 trits (128-bit).
- **Strings** — NUL-terminated vectors of characters. A `\0` inside a literal terminates the string (`"ab\0xy"` means `"ab"`).
- **`void`** — used only to mark a function that returns nothing, or to declare a generic pointer `[void]`.
- **Pointers** — `[type]` (e.g. `[int]`, `[void]`).
- **Functions** — `-> type` return notation; `public` / `forward` / `extern` qualifiers; `auto` for inferred declarations.

Control flow: `if` / `elif` / `else`, `while` with `stop` / `next`, `return`. Operators & I/O: `!` (print), `!!` (println), `input`, `sizeof`, `null`, `&&`, `||`, plus the usual arithmetic and relational set.

## Compiler pipeline

| Stage | File | Role |
|-------|------|------|
| Lexical | `p6_scanner.l` (flex) | tokens, numeric/string literals |
| Syntactic | `p6_parser.y` (bison) | grammar → AST |
| AST | `ast/*.h` | node classes (one per construct) |
| Semantic | `targets/type_checker.cpp`, `targets/symbol.h` | type checking, symbol table |
| Frame layout | `targets/frame_size_calculator.cpp` | local-variable frame sizes |
| Code generation | `targets/postfix_writer.cpp` | Postfix / x86 assembly |
| XML dump | `targets/xml_writer.cpp` | AST dump (intermediate delivery) |

## Building & running

Requires the course toolchain (CDK + RTS under `~/compiladores/root`), `flex`, `bison`, a C++23 `g++`, `yasm` and a 32-bit `ld`.

```sh
make                     # builds ./p6
./p6 --target asm prog.p6 -o prog.asm
yasm -felf32 prog.asm -o prog.o
ld -melf_i386 -o prog prog.o -L$HOME/compiladores/root/usr/lib -lrts
./prog
```

`--target xml` dumps the AST instead. The helper `runtests.sh` compiles, assembles, links, runs and diffs every test in the course's `auto-tests/` suite:

```sh
./runtests.sh            # === PASSOU: 185 | FALHOU: 0 ===
```

## July 2026 corrections

The submission passed 100% of the automatic tests; the points lost were **conformance / quality**, flagged in the professor's grading sheet and the cross-project code report. Four defects were fixed, keeping the suite at **185 / 185**:

1. **`void` variables rejected** (`type_checker.cpp`). The spec allows `void` only as a function's absent return type or as the generic pointer `[void]` — never as the type of a variable or parameter. The type checker now raises *"cannot declare variable of type void"* for such declarations (both plain variables and function parameters). `[void]` pointers (a pointer type) and `void` return types (not variable declarations) stay valid.

2. **NUL truncates string literals** (`p6_scanner.l`). Per the language definition, a `\0` inside a string literal ends the string at that position — `"ab\0xy"` is `"ab"`. The scanner now switches to an exclusive skip state on `\0`, discarding the rest up to the closing quote. Result: `"ab\0xy"` emits a single terminator (`db "ab", 0`).

3. **`symbol` without a redundant flag** (`symbol.h`). The symbol class carried a separate `bool _function` flag, duplicating information already present in the type — a function's symbol holds a `cdk::functional_type`. The flag (and its constructor parameter) were removed; `is_function()` now derives the answer from the type:

   ```cpp
   bool is_function() const {
     return _type != nullptr && _type->name() == cdk::TYPE_FUNCTIONAL;
   }
   ```

4. **Function-definition node carries a functional type** (`ast/function_node.h`, `type_checker.cpp`). The function *declaration* node already stored its type as a `cdk::functional_type`, but the function *definition* node stored only its return type and rebuilt the functional type later in the type checker. The definition node now builds and stores the full functional type in its constructor (parameter types + return type), so both function nodes are consistent and the type checker creates the symbol directly from `node->type()` — mirroring the declaration path. This matches the professor's `til` reference compiler, which builds the functional type the same way.

## Appendix — Adding a new node (walkthrough)

Imagine we wanted to introduce a new node in the p6 compiler — say, a new functionality in the language. We can take as an example the `with` statement, present in the possible-compilers exercises.

```
with args if condition call func-name-1 otherwise func-name2
```

### New AST node

The first step is creating the new node in the `/ast` directory. We can create a new file called `with_node.h`.

Second, we need to look at how the new with node works. It is crucial to understand the arguments, the condition and the functions that are called.

The with node will have the following structure:
- args: a list of expressions that will be evaluated and passed as arguments to the functions.
  - Note that the args are separated by commas, so we can have multiple arguments. So we can assume that the args are stored in a sequence node, which is a node that can store a list of nodes.
- condition: an expression that will be evaluated to determine which function to call.
  - The condition can be any expression that can be evaluated to a boolean value. So we can assume that the condition is stored in an expression node, which is a node that can store any expression.
- func-name-1: the name of the function that will be called if the condition is true.
  - The function name can be stored in a string node, which is a node that can store a string value.
- func-name-2: the name of the function that will be called if the condition is false.

Looking at the structure of the with statement, we can define the with node as follows:

```cpp
class with_node : public cdk::basic_node {
  cdk::sequence_node *_args;
  cdk::expression_node *_condition;
  std::string *_func1;
  std::string *_func2;

public:
  with_node(int lineno, cdk::sequence_node *args, cdk::expression_node *condition, std::string &func1, std::string &func2)
    : cdk::basic_node(lineno), _args(args), _condition(condition), _func1(func1), _func2(func2) {}

    cdk::sequence_node *args() const { return _args; }

    cdk::expression_node *condition() const { return _condition; }

    std::string &func1() const { return _func1; }

    std::string &func2() const { return _func2; }

  void accept(basic_ast_visitor *sp, int level) {
    sp->do_with_node(this, level);
  }
};
```

### New scanner rule

The next step is to create a new scanner rule in the `p6_scanner.l` file. We need to define a new rule that will match the with statement and create a new with node.
The rule can be defined as follows:

```
"with" { return tWITH; }
"call" { return tCALL; }
"otherwise" { return tOTHERWISE; }
```

### New parser rule
The next step is to create a new parser rule in the `p6_parser.y` file. We need to define a new rule that will match the with statement and create a new with node. It is also necessary to define the new tokens that we have created in the scanner.
The rule can be defined as follows:

in the token section:
```
%token tWITH tCALL tOTHERWISE
```

```
stmt : expr ';'                         { $$ = new p6::evaluation_node(LINE, $1); }
        ...
    | tWITH params tIF expr tCALL tIDENTIFIER tOTHERWISE tIDENTIFIER { $$ = new p6::with_node(LINE, $2, $4, *$6, *$8); delete $6; delete $8; }

```

### New type checker rule
The next step is to create a new type checker rule in the `targets/type_checker.cpp` file. We need to define a new rule that will check the types of the with node. The rule can be defined as follows:

```cpp
void p6::type_checker::do_with_node(p6::with_node * const node, int lvl) {
  // 1. verificar os filhos PRIMEIRO (o do_sequence_node é vazio → iterar os args à mão)
  for (size_t i = 0; i < node->args()->size(); i++)
    node->args()->node(i)->accept(this, lvl + 2);
  node->condition()->accept(this, lvl + 2);

  // 2. a condição tem de ser ternária (balanced3) — lógica de Kleene
  if (!node->condition()->is_typed(cdk::TYPE_BALANCED3))
    throw std::string("condition of with statement must be of type balanced3");

  // 3. func1 e func2 têm de nomear funções declaradas
  auto s1 = _symtab.find(node->func1());
  if (s1 == nullptr)        throw std::string("undeclared function '" + node->func1() + "'");
  if (!s1->is_function())   throw std::string("symbol '" + node->func1() + "' is not a function");

  auto s2 = _symtab.find(node->func2());
  if (s2 == nullptr)        throw std::string("undeclared function '" + node->func2() + "'");
  if (!s2->is_function())   throw std::string("symbol '" + node->func2() + "' is not a function");

  // 4. os args têm de ser compatíveis com os parâmetros das funções.
  //    O símbolo guarda o functional_type (não o function_node).
  auto ft1 = cdk::functional_type::cast(s1->type());
  auto ft2 = cdk::functional_type::cast(s2->type());

  if (ft1->input()->length() != node->args()->size())
    throw std::string("number of arguments does not match parameters of '" + node->func1() + "'");
  if (ft2->input()->length() != node->args()->size())
    throw std::string("number of arguments does not match parameters of '" + node->func2() + "'");

  for (size_t i = 0; i < node->args()->size(); i++) {
    auto arg = dynamic_cast<cdk::expression_node*>(node->args()->node(i));
    auto p1 = ft1->input(i);
    auto p2 = ft2->input(i);

    // compatível = mesmo tipo, ou promoção int → real (balanced3 → takum3)
    if (arg->type()->name() != p1->name() &&
        !(p1->name() == cdk::TYPE_TAKUM3 && arg->is_typed(cdk::TYPE_BALANCED3)))
      throw std::string("argument " + std::to_string(i) + " incompatible with '" + node->func1() + "'");

    if (arg->type()->name() != p2->name() &&
        !(p2->name() == cdk::TYPE_TAKUM3 && arg->is_typed(cdk::TYPE_BALANCED3)))
      throw std::string("argument " + std::to_string(i) + " incompatible with '" + node->func2() + "'");
  }

}
```

### Update the frame size calculator
The next step is to update the frame size calculator in the `targets/frame_size_calculator.cpp` file. We need to define a new rule that will calculate the frame size of the with node. It's empty because the with node does not introduce any new variables, so it does not affect the frame size.

```cpp
void p6::frame_size_calculator::do_with_node(p6::with_node * const node, int lvl) {}
```

### New XML writer rule
The next step is to create a new XML writer rule in the `targets/xml_writer.cpp` file. We need to define a new rule that will write the with node in XML format. The rule can be defined as follows:

```cpp
void p6::xml_writer::do_with_node(p6::with_node * const node, int lvl){
    ASSERT_SAFE_EXPRESSIONS;
    openTag(node, lvl);

    openTag("condition", lvl + 2);
    node->condition()->accept(this, lvl + 4);
    closeTag("condition", lvl + 2);

    openTag("function1", lvl + 2);
    os() << std::string(lvl + 4, ' ') << node->func1() << std::endl;
    closeTag("function1", lvl + 2);

    openTag("function2", lvl + 2);
    os() << std::string(lvl + 4, ' ') << node->func2() << std::endl;
    closeTag("function2", lvl + 2);

    openTag("args", lvl + 2);
    for (size_t i = 0; i < node->args()->size(); i++)
        node->args()->node(i)->accept(this, lvl + 4);
    closeTag("args", lvl + 2);

    closeTag(node, lvl);
}
```

### New Postfix writer rule
Finally, we need to create a new Postfix writer rule in the `targets/postfix_writer.cpp` file. We need to define a new rule that will write the with node in Postfix format. The rule can be defined as follows:

```cpp
void p6::postfix_writer::do_with_node(p6::with_node * const node, int lvl){
    ASSERT_SAFE_EXPRESSIONS;

    int lbl1, lbl2;

    size_t argsize = 0;
    for (int i = node->args()->size() - 1; i >= 0; i--) {
        auto arg = dynamic_cast<cdk::expression_node*>(node->args()->node(i));
        arg->accept(this, lvl + 2);
        argsize += arg->type()->size();
    }

    node->condition()->accept(this, lvl);
    _pf.B2I();
    _pf.INT(0);
    _pf.JLE(mklbl(lbl1 = ++_lbl));

    _pf.CALL(node->func1());
    _pf.JMP(mklbl(lbl2 = ++_lbl));

    _pf.LABEL(mklbl(lbl1));
    _pf.CALL(node->func2());
    _pf.LABEL(mklbl(lbl1 = lbl2));

    if (argsize > 0) _pf.TRASH(argsize);
}
```

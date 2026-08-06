# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

`nD` is a personal, from-scratch systems language and compiler written in C99. Its distinguishing ideas (see README.md for the full pitch):

- Immutability by default; variables are declared up front in a scope using `[ ... ]` block syntax, separate from the imperative body in `{ ... }`.
- Ownership-based memory management via a `new` keyword and function/body-scoped arena allocation (a Rust-inspired escape-or-free model), instead of raw `malloc`/`free`.
- A planned dynamically-resizing `list` type as the one heap/dynamic-array primitive.

The project is a hobby compiler under active, exploratory development. The full pipeline (tokenizer → parser → SA → IRComp) is wired up and working end-to-end for a **core language subset**: functions, variables (mutable/immutable), arithmetic, comparisons, `and`/`or`, assignment, `if`/`elseif`/`else`, `while`, function calls, `return`. Structs, pointers, `new`-based allocation, lambdas, and lists are deliberately **out of scope** for SA/IRComp right now — the parser can still parse them (see `example2.nd`/`example3.nd`), but SA rejects programs that use them with a clear "not supported by this build (out of scope)" error rather than miscompiling or crashing. `JIT/` is unstarted (see below), so nothing actually *runs* yet — verification is via `IRComp/IR_util.c`, a bytecode disassembler, and hand-tracing its output against `IR.h`'s opcode reference. Don't assume a pipeline stage is complete just because its files exist; check `main.c` for what's actually invoked, and check whether a given AST/type construct is explicitly rejected in `SA/semantic.c` or `IRComp/IRComp.c` before assuming it's supported.

## Build

The `Makefile` builds the whole pipeline through bytecode generation (tokenizer, parser, SA, IRComp, and the disassembler):

```sh
make        # builds ./ndc_ast_print_SA1
make clean  # removes it
```

`CFLAGS = -Wall -g -std=c99 -DDEBUG`. `-DDEBUG` enables `ASSERT`/`REQUIRES`/`ENSURES` (see `utils.h`) and turns on extra `printf` tracing in `main.c` (e.g. AST pretty-printing after parse, and bytecode disassembly after IRComp).

There is no test suite and no linter configured — `example_code/core_test.nd` doubles as the closest thing to a regression check for the core language subset; rerun it and eyeball the disassembly after touching SA or IRComp. `.clangd` sets `-xc -std=c99 -Wall -Wextra -I.` for editor/clangd diagnostics — treat `-Wextra` warnings it surfaces as worth fixing even though the Makefile itself only passes `-Wall`.

### Running

```sh
./ndc_ast_print_SA1 <filename.nd> [-d]
```

The `-d` arg is parsed into `debug_mode` in `main.c` but not currently consumed by anything downstream — it's a placeholder. Sample inputs live in `example_code/*.nd` (these show current-vs-aspirational syntax; comments at the top of a file like `example2.nd` call out what's "current working" syntax). `core_test.nd` is the one that exercises the full core-language feature set end-to-end; `example2.nd`/`example3.nd` use lambdas/structs and are expected to fail at the SA stage with an "out of scope" error, not a crash.

## Architecture / pipeline

The compiler is a straightforward staged pipeline, each stage in its own top-level directory, each stage consuming the previous stage's output type:

```
FILE* --tokenizer--> Stream (lazy token stream) --parser--> AST_Program --SA--> (annotated) AST_Program --IRComp--> IR_Program --JIT--> (unimplemented)
```

- **`tokenizer/`** — Lazy tokenizer, not a pre-materialized token array. `Stream` (`real_stream.h`) wraps a `FILE*` with `prev_token`/`curr_token`/`next_token` for one-token lookback and lookahead; `stream_next`/`stream_peek`/`stream_curr` in `tokenize.c` drive it. `token.h` defines the full `TokenType` enum — binary-expression-eligible tokens are annotated with `// Binary_Expr` comments, which the parser and `ast_util.c` rely on when deciding what can appear in a `Binary_Expr` node. `fake_stream.h` exists alongside `real_stream.h` as an alternate (array-based) stream representation for tests/tooling — check which one a given `.c` file includes before assuming `Stream` behavior.

- **`parser/`** — Top-down (recursive-descent) parser producing the AST. Central types in `ast.h`:
  - `Type` / `Type_Kind` (`VALUE`, `POINTER`, `MUTABLE`, `FUNCTION`, `USER`, `LIST`, `NONE`) form a small type tree — e.g. function types are `Type_Tree{input, output}`, so curried types like `int->int->int` nest as right-associated trees.
  - `AST_Node` (aliased `Astn`) is a tagged union over `Node_Kind` (`NODE_LITERAL`, `NODE_BINARY_EXPR`, `NODE_UNARY_EXPR`, `NODE_FUN_CALL`, `NODE_FUN_DEC`, `NODE_LAMBDA`, `NODE_LAMCALL`, `NODE_LOOP`, `NODE_COND`, `NODE_BODY`).
  - `Body_Block` models the language's split between variable declarations (`vars: Var_List`) and instructions (`inst`) — this is the direct AST encoding of the `[decls] { body }` syntax feature.
  - `Cond_Expr` is a linked list (`chain`) of `IF`/`ELSEIF`/`ELSE` links, not a flat if/else-if array.
  - `GUser_Types` holds global user-defined types (`struct`/`alias`); look here before assuming an identifier is a builtin.
  - `ast_util.c` / `ast_util.h` hold the constructor/accessor helpers for these structures (`new_*`, `*_add`, `isin_*`, `*_get_*`, `*_free`) — prefer these over hand-rolling struct construction, they manage the capacity-doubling UBA (unbounded array) pattern used throughout (`num_x`/`x_cap` pairs).

- **`SA/`** — Semantic analysis (`semantic_analysis(AST_Program*)`), operates in place on the AST. Wired into `main.c`, runs right after parsing. Type-checks arithmetic/comparisons/logical ops/assignment (with mutability enforcement — assigning to a non-`$` variable is a semantic error), function calls, `if`/`while` conditions, and `return` types. Explicitly rejects `NODE_LAMBDA`/`NODE_LAMCALL` and non-plain-variable assignment targets (the latter is how struct field assignment gets rejected, since that path isn't implemented) with a clear error rather than mishandling them.

- **`IRComp/`** — Lowers `AST_Program*` to `IR_Program*` (`IR_Comp`, `IRComp.c`/`.h`), wired into `main.c` right after SA. `IR.h` defines the bytecode format and *is the source of truth for opcodes* — see the large comment block in that file documenting each opcode (stack ops, arithmetic, comparisons `icmpeq`/`icmplt`/`icmpleq`/`icmpgt`/`icmpgeq`, constant loads `bipush`/`ildc`/`aldc`, locals `vload`/`vstore`, control flow `if`/`goto`, `invokestatic`/`return`, and the pointer/dynamic/struct memory ops `ptload`/`ptstore`/`dyload`/`dystore`/`sload` — the last group is documented but not lowered to yet, since structs/pointers/`new` are out of scope). This bytecode is explicitly modeled on **CMU's 15-122 C0VM** — if you need context on an opcode's semantics and the comment is ambiguous, C0VM's spec is the right analogy. `IR_Program` pools functions/ints/strings (`fun_pool`, `int_pool`, `str_pool`) that later bytecode indexes into. `Vstack` (a stack of `Var_List*`) tracks scopes during lowering to resolve variable names to slot indices via `search_vstack`. Forward jumps (`if`/`goto`) are compiled with a placeholder offset and backpatched once the target address is known (`byte_index`/`byte_add_index` in `IR.c`) — don't reintroduce the older pattern of threading byte-length counts through every compile function by hand, it doesn't compose once you have nested branches. `IR_util.c` is the disassembler (`print_IR_Program`), the primary way to sanity-check codegen output given there's no VM yet.

- **`JIT/`** — Header only (`JIT.h`), currently empty. Not yet started — this is the next major piece of unstarted work (a bytecode interpreter/VM would be a smaller first step than an actual JIT).

- **`utils.c`/`utils.h`** — Shared helpers: token/type stringification and printing, and error reporters (`aerr` = AST error, `serr` = syntax/stream error with message, `saerr` = semantic-analysis error). `ASSERT`/`REQUIRES`/`ENSURES` are no-ops unless `-DDEBUG` is set, in which case they become `assert`.

## Conventions worth knowing before editing

- Data structures follow a consistent "unbounded array" (UBA) pattern: a pointer/array member, a `num_*` count, and a `*_cap` capacity, grown by doubling. When adding a new collection, match this shape (`Var_List`, `GUser_Types`, `Fun_Type.variables`, `Fun_Call.args`, `Body_Block.inst`, `IR_Program` pools, `Vstack` all follow it).
- Constructors/mutators are named `new_X`, `X_add`, `isin_X`, `X_get_*`, `X_free` — follow this naming when adding parallel functionality.
- Header files are guarded with `#ifndef X_H` and use relative includes (`../parser/ast.h` etc.) — headers are not centrally aggregated, each `.c`/`.h` includes exactly what it needs.
- File headers carry an `Andy Kwak 2026` banner comment and `// ===== //` section rules; match this style in new files for consistency with the rest of the codebase.
- Build artifacts (the compiled binary, `*.dSYM/`) are gitignored — if you see one untracked in the repo root, it's a local build byproduct, not something to commit.

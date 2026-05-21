# Project Overview

tinyclib is a tiny C library for building applications across various platforms,
from server-grade hardware to desktops, mobile, and embedded devices.

## Folder Structure

- `build/`: Build directory (not committed to git).
- `cmd/`: Command-line tools.
- `docs/`: Project documentation.
- `examples/`: Example code and usage demonstrations.
- `include/`: Public API headers.
- `scripts/`: Utility scripts which are not part of the main source code.
- `src/`: Source code.
- `tests/`: Test files.
- `third_party/`: Third-party libraries and tools.
- `tmp/`: Temporary files and outputs.

## Project Guidelines

- Pass extensible boundary structs by pointer, not by value. Use `const` pointer
  parameters for input structs and mutable pointer parameters for output
  structs. Passing a struct by value bakes the full layout into the call ABI and
  defeats the future usefulness of the leading `size` field.
- Update all references across the codebase and documentation when you rename
  command arguments, config parameters or any other identifiers.
- When defining C structs, order fields strictly to minimize memory padding and
  optimize cache alignment, never purely for narrative readability. Sort fields
  from largest to smallest alignment requirements (e.g., 8-byte
  pointers/`size_t` first, then 4-byte integers, down to 1-byte
  booleans/`char`). If specific fields are frequently accessed together in a hot
  path, group them sequentially within those size constraints to maximize
  cache-line efficiency. Only apply logical grouping (e.g., inputs vs. outputs)
  when it does not introduce invisible padding bytes.
- Do not use cross-file global variables. Use file-local static state only when
  necessary. Use opaque context types across multi-file module boundaries,
  including tests. Use passed-in context structs for per-instance, per-request,
  async, or otherwise explicit state.
- Pay attention to forward declarations when you are using C opaque pointers.
- Keep source code, comments, public headers, examples, and non-plan
  documentation project-generic. External consumer names and runtime-specific
  integration details belong in plan files only.
- Do not use magic numbers. Every numeric literal used as a size, offset, index,
  flag mask, or field position must be a named constant or carry an inline
  comment explaining its meaning. Trivially obvious values (0, 1, `'\0'`,
  null-terminator arithmetic) are exempt.
- Bare zero values are acceptable for explicit "none" or "unused" semantics, but
  they must carry a comment. Use an inline comment when the meaning is short,
  and a standalone explanatory comment when the field or control flow needs more
  context.
- Do not use scope prefixes like g_ in C. Use descriptive snake_case names.
- Always use uppercase letters for numeric literal suffixes (e.g., 123U, 10L,
  0Z). Never use lowercase (u, l, uz) to prevent visual ambiguity, especially
  the dangerous confusion between the letter l and the number 1. Apply this
  uniformly to all macros and literal values.
- When generating C callback functions that require error handling and control
  flow, always use an explicitly typed enum (enum : int) as the return type to
  guarantee ABI stability without relying on out-parameters. Structure the enum
  values strictly into three mathematical ranges: assign negative integers (< 0)
  for specific, fatal error states (e.g., -1, -2) so the caller can catch all
  failures with a single, fast if (result < 0) branch; assign exactly 0 for the
  default success or 'continue' state; and assign positive integers (> 0) for
  non-error control flow signals like a graceful halt or skipping an element.
  This ensures idiomatic, branch-prediction-friendly error propagation where
  negative codes can be immediately returned and bubbled up the call stack.
- Prefer status-returning mutating functions when an operation changes state and
  may fail. Return `int` for success or failure, and use pointer parameters for
  produced values such as indexes or handles.
- Use C "middle style" naming convention for non-static file-level functions
  like `[lib]_[file]_[action]_[property]()`. Use C "prefix style" naming
  convention for static functions like `[action]_[property]()`. For
  boolean-returning functions, use is_/has_/can_ as the action slot.
- For boolean variables and struct fields, prefer concise state names like done,
  ready, enabled, or <noun>_done.
- Comments in test files should explain what the code can't; the source of a
  constant, the meaning of magic bytes, why an odd-looking assertion holds. Do
  not restate what names and assertions already say. In test files, prefer an
  inline comment over extracting a named constant for magic numbers; keep the
  literal at the use site so each test stays self-contained. When refactoring
  tests, do not delete existing test comments just because a new test name is
  more descriptive. Preserve existing comments unless the user explicitly asks
  for their removal, or update them in place when the code change makes them
  inaccurate.
- When generating or modifying code comments, act as a procedural outlier. Your
  goal is to allow a developer to understand the high-level execution flow
  simply by reading the comments from top to bottom. Actively document each
  logical step and structural block using short, direct statements (e.g.,
  "Resolve mount and relative path", "Determine file type"). It is acceptable to
  describe the immediate action being taken, provided it acts as a clear section
  header that prevents the reader from needing to parse the underlying C syntax.
- When generating or modifying code comments, you must determine capitalization
  based on placement and punctuation based on grammatical structure. If a
  comment sits on its own line, it must begin with an uppercase letter. If a
  comment is placed inline, directly next to code on the same line, it must
  begin with a lowercase letter. You must completely omit terminal periods for
  all inline comments, short imperative commands, fragments, and noun phrases
  (e.g., `// Initialize the database` or
  `let x = 0; // initialize the database`). You may only use a terminal period
  if a standalone comment is a true, formal full sentence containing both a
  distinct subject and a verb.
- Use Doxygen-style documentation comments (`/** ... */`) with tags like @brief,
  @param, @return, etc. for all public functions, important internal functions
  and macros. Add a new line after each tag group. Use `//` for inline comments.
  Do not use comment dividers (`//---...`), or section headers
  (`// === ... ===`).
- Start function `@brief` text with a brief third-person singular present-tense
  verb, such as "Returns ...", "Handles ...", "Sets ...", "Adds ...` and so on.
- Place documentation comments where the entity is declared, not duplicated
  across files. If a function, struct or macro is declared in a header file, its
  Doxygen comment lives in the header and the implementation file carries no
  repeated block. If the entity has no header declaration, such as a static
  function or a file-local struct, its comment lives in the implementation file
  where it is defined. The goal is a single source of truth per entity so
  comments do not drift apart over time.
- Plan files must begin with `# <title>`, `## Overview`, `## Scope` and
  `## Out of Scope` sections, in that order, so a reader can understand what the
  plan is about and what it deliberately excludes without reading the whole
  document. If the plan has unresolved design questions, follow those three
  sections with an `## Open Questions` section. In `## Open Questions`, use
  numbered questions and put an indented `Recommendation:` line under each
  question. Also add a separate `## Recommendations` section if any before any
  other content.
- When asked to check a plan file for consistency or correctness, use a
  two-phase workflow: first perform a read-only audit by inspecting the plan,
  related code, documentation and project rules, identifying what may need to
  change and why while making no edits; then perform a fix pass by patching all
  confirmed issues together.
- Do not suppress formatter, linter and checker warnings without asking first.
- Do not remove tests without asking first. Do not cause regressions in existing
  tests without asking first.
- When you are creating CMakeLists.txt files for new folders, follow the
  existing patterns, structure and naming conventions.
- Do not create generic common or utility files without asking first.
- Do not add `_internal`, `_private`, `_impl`, or similar suffixes to files,
  types, or functions - the directory already carries that signal. Prefer
  specific, descriptive filenames (e.g. `client.c`, `request.c`) over generic
  ones (`async.c`, `common.c`).
- Stay consistent with existing similar files. Before adding or modifying a
  file, look at its siblings and peers to mirror their directory layout, file
  organization, section ordering, include/header grouping, naming conventions,
  comment style, struct and function shapes, error handling patterns and test
  scaffolding. Prefer matching the established pattern over introducing a new
  one, even when the new approach seems marginally better. If the existing
  pattern is genuinely wrong or no longer fits, ask before diverging so the
  inconsistency is intentional and applied across the affected files together.
- Place unit and internal module tests in `tests/unit/`, public API tests in
  `tests/api/` and integration tests in `tests/integration/`. Name unit and
  internal test files after the module they test using `test_<module>.c`, where
  `<module>` matches the source or private header name without the project
  prefix. If an internal test covers multiple private modules, use
  `test_core_<feature>.c`. For public API tests, use `test_api_<header>.c`,
  where `<header>` is the public header name without the project prefix and
  without the `.h` suffix. Do not use generic names like `test_utils.c` or
  `test_common.c`. Do not mix public API behavior tests and private/internal
  module tests in the same file.
- All C tests must use the Unity framework (`unity.h`, `setUp`, `tearDown`,
  `UNITY_BEGIN`, `RUN_TEST`, and `UNITY_END`) and be wired through the existing
  CMake test target list patterns.
- When writing C test function names, use established pattern structures such as
  `test_<function>_<scenario>` for simple utilities,
  `test_<function>_<scenario>_<outcome>` for complex algorithms, or
  `test_<state>_<action>_<outcome>` for state machines. Within your test file,
  always group by target function by clustering all tests for a single function
  together to easily spot coverage gaps. Additionally, avoid generic scenario
  names like `_normal_case` or `_works` by specifying exactly what makes the
  input valid, and remember to test static functions indirectly by evaluating
  the public API functions that trigger their logic rather than calling them
  directly.
- Known Issues and Bugs: There is a line between "known issues" and "bugs".
  Known issues are things that are not ideal but are acceptable for the current
  scope and requirements. Bugs are on the other hand unacceptable regardless of
  scope. Always add comments and documentation for known issues. Always create a
  markdown file in the `docs/bugs` directory for each bug with a description of
  the issue. Do not ignore or silently move on from bugs.
- Avoid regressions in existing functionality without asking first.Do not remove
  or change a test just to make it pass without asking first.
- **IMPORTANT!** Avoid implicit behavior, "magic" and silently ignoring errors.
  Always prefer to report errors, warnings and unsupported use cases.
- **IMPORTANT!** Always ask before making any significant changes related to
  performance and public API.
- **IMPORTANT** Do not pollute the project root with temporary files. Use the
  project `tmp/` folder for all temporary files and outputs.
- **IMPORTANT!** This is a C project. Do not add any other languages or
  dependencies without asking first.
- **IMPORTANT** Do not clean build folder without asking first! Using
  `mise exec -- make configure` and `mise exec -- make build` is much faster for
  iterative development. Use `mise exec -- make clean` only when you know what
  you are doing and need to clean all the generated files.
- Keep tinyclib changes small. Prefer additive public API changes when possible
  and call out any breaking public API change before implementing it.

## Quick Reference

- `make help`: Show available make targets
- `make configure`: Configure cmake
- `make build`: Build the project
- `make clean`: Remove temporary directories
- `make test`: Run tests
- `make format`: Check code formatting
- `make lint`: Check code linting
- `make check`: Check types
- `make check-all`: Run all checks
- `make fix`: Fix code formatting and linting issues
- `mise exec -- [COMMAND]`: Run a command with the project environment. This
  ensures the correct toolchain (cmake, clang, etc.) is used. For example
  `mise exec -- cmake`, `mise exec -- make test`.
- `eval "$(mise activate [SHELL_TYPE])"`: Initializes mise in the current shell
  session. For example `eval "$(mise activate bash)"`.

## References

- [Unity](https://github.com/ThrowTheSwitch/Unity.git) See
  `tmp/third_party/unity` folder for a local copy of the source and
  documentation.

## Design Notes

### Reviewability Boundaries

For reviewability, keep capability boundaries small, boring and contractual.
Prefer files that own one stable concept over large files that gather unrelated
operations only because they share a handle or backend. A good boundary lets
stable code stay unchanged while related new work is added in nearby focused
files.

- Split public headers and source files by consumer-facing capability.
- Keep high-churn features behind narrow headers, narrow source files and
  focused tests so reviewers can understand a change without loading unrelated
  behavior into memory.
- Add or maintain compile and behavior tests at each boundary so accidental
  coupling, broad includes and behavior regressions are caught quickly.
- Avoid catch-all files and directories. Use specific names that describe the
  contract they own, and add new files beside stable contracts when a feature
  has a distinct consumer or change pattern.
- Treat mature files as contracts. Once a module has clear invariants and good
  coverage, prefer extending through adjacent modules rather than repeatedly
  reopening the stable core.

### Abstraction Boundary

The public API (`include/` and `src/`) uses the project's own type and function
names, not the underlying libraries' names.

This boundary applies to the public API surface only. Commands (`cmd/`) and
examples (`examples/`) are free to use third-party libraries directly like a
normal C project.

### Third-Party code patches

When you patch a third-party code, add `.patch` file for each patch and `.md`
file matching the patch name with a content like (similar to "Common Changelog"
categories):

```markdown
# <file_name>.patch

- **Changed** ...
- **Added** ...
- **Removed** ...
- **Fixed** ...
```

Keep it simple, do not add unnecessary details.

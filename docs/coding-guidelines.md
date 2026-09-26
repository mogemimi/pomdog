# Coding Guidelines

Pomdog has no rigid rulebook for C++ code. This page records conventions in
`pomdog/`, `tests/`, and `examples/`. Use `.clang-format` for layout. For choices
it cannot make, follow nearby code and platform constraints. These guidelines
allow exceptions when the context calls for them. Build requirements described
here still apply where the code depends on them.

For build instructions, formatting scripts, and the CI matrix, see [Developing Pomdog Game Engine](developing-pomdog-game-engine.md).

## Formatting

Use `.clang-format` for indentation, line breaks, brace placement, pointer and
reference alignment, and include sorting. Run the formatting script before review:

```sh
./tools/script/clang_format.sh
```

The script also runs `plain-text-format`, which strips trailing whitespace, converts CRLF to LF, and ensures a trailing newline.

## File layout

Start source files with the license line and use `#pragma once` in headers:

```cpp
// Copyright mogemimi. Distributed under the MIT license.

#pragma once
```

Include project headers by their full path from the repository root, without any suppression macro.
Wrap standard library and third-party includes in the matching suppression macro.
These headers may produce warnings under the engine's warning-as-error settings:

```cpp
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <span>
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END
```

A self-contained header that includes no other Pomdog header must include `pomdog/basic/conditional_compilation.h` itself before using the macro.
FlatBuffers generated headers use `POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_BEGIN/END` from `pomdog/basic/flatbuffers_macros.h`, and the doctest include in unit tests uses the macros in `tests/testing/testing.h`.

Prefer forward declarations over includes in headers, and collect them in their own namespace block above the main one.
Forward-declared enums need their underlying type:

```cpp
namespace pomdog::gpu {
class CommandList;
struct PipelineDesc;
enum class PixelFormat : u8;
} // namespace pomdog::gpu
```

File names are `snake_case` and match the primary type they declare, so `class SpriteBatch` lives in `sprite_batch.h`.
Keep one primary type per header.

## Naming

| Subject | Convention | Example |
|:---|:---|:---|
| Types, classes, enums | `PascalCase` | `GraphicsDevice`, `PixelFormat` |
| Member functions | `camelCase` | `getPosition`, `createTexture2D` |
| Member variables | trailing underscore | `graphicsDevice_`, `committedMetrics_` |
| Local variables, parameters | `camelCase` | `commandList`, `rootDir` |
| Public data members of plain structs | no underscore | `BufferDesc::sizeInBytes` |
| Files and directories | `snake_case` | `game_host.h`, `content/utility/` |
| Macros | `POMDOG_UPPER_SNAKE` | `POMDOG_ASSERT` |

Keep acronyms capitalized: `HTTPClient`, `DPISettings`, and `UIEventDispatcher`.

Public API lives in `namespace pomdog` or in a library namespace such as `pomdog::gpu` or `pomdog::vfs`.
Implementation details that must appear in a header live in a `detail` namespace, for example `pomdog::gpu::detail::gl`.
Namespaces and directories need not match. For example, `gpu/gl4/` uses
`pomdog::gpu::detail::gl`.

Follow the existing naming convention for free functions in each namespace:
`console` uses `write_line`, while `vfs` and `filepaths` use `openArchiveFile` and
`joinUnix`.

## Types

Use the aliases in `pomdog/basic/types.h` instead of spelling out fixed-width types: `i8` through `i64`, `u8` through `u64`, `f32`, and `f64`.
Do not include `<cstdint>` to get them.
Some older code under `experimental/` still uses `std::int32_t` and `float` directly; leave it alone unless you are already changing those lines.

Give every `enum class` an explicit underlying type, and use the smallest one that fits:

```cpp
enum class PixelFormat : u8 { ... };
```

Mark concrete classes `final`.
Omit `final` for interfaces intended for inheritance, such as backend interfaces.

Prefer `std::span` over a pointer and length pair.
Clang builds enable `-Wunsafe-buffer-usage`, which reports raw-pointer indexing
and arithmetic.

## Error handling

Return errors from functions that can fail; do not throw exceptions:

```cpp
[[nodiscard]] std::unique_ptr<Error>
mount(std::string_view path) noexcept;

[[nodiscard]] std::tuple<FileInfo, std::unique_ptr<Error>>
stat(std::string_view path) noexcept;
```

A null error means success. Use `errors::make` to create an error and
`errors::wrap` to add context when returning a callee's error. Both functions are
in `pomdog/utility/errors.h`.

`POMDOG_ASSERT` is for programming errors that a caller cannot recover from, such as a broken precondition inside the engine.
Release builds omit assertions. Validate files, network data, and user
configuration with checks that return errors in both Debug and Release.

If a required input is missing, fail at startup with a message naming its path.
Make optional-file behavior explicit in the API so that missing files do not
change application behavior without the caller's knowledge.
See `pomdog/vfs/default_vfs_setup.cpp` for the documented contract.

## `[[nodiscard]]`

Apply `[[nodiscard]]` where ignoring the result is a defect: error returns, owning handles, factory functions, and queries whose result must be acted on.
Leave it off results that callers may ignore, even where older code applies it
more broadly.

To discard a `[[nodiscard]]` result, bind it to a named `[[maybe_unused]]` local
that records what you are ignoring. Do not cast the result to `void`:

```cpp
// Good: the name records why the result is ignored here.
[[maybe_unused]] const auto closeError = file->close();

// Bad: says nothing about intent.
(void)file->close();
```

## `noexcept`

Mark functions `noexcept` when they cannot throw. This includes operations that
report failure through an error return. On move constructors and assignments,
`noexcept` also affects the standard library's choice between copying and moving.

## Control flow

Use braces for control-flow bodies, including single statements and `else if` chains.

Prefer `if` over the conditional operator, especially for early returns:

```cpp
// Good
if (value == nullptr) {
    return std::nullopt;
}
return *value;

// Avoid
return value != nullptr ? *value : std::nullopt;
```

A conditional operator is acceptable for a small value selection, such as
choosing a constant inside a `switch` case.

## Warnings

Every platform builds with warnings as errors: `-Wall -Werror` for Clang and GCC, `/Wall /WX` for MSVC.
Check Emscripten as well as Windows; Clang reports warnings that MSVC does not.

C++ game code can fail through memory corruption, invalid access, or data races.
Modern compilers provide useful static diagnostics for some risky patterns.
Treating warnings as errors brings those findings into the build, where developers
can review them before the game runs. A warning can also mark code that deserves
attention even when no change is needed today.

Some diagnostics are experimental, overly strict, or false positives. Disable a
warning category only where it produces poor results. For an isolated diagnostic,
use the narrowest available `POMDOG_*_SUPPRESS_WARNING` macro and leave a
`// NOTE:` explaining why, with a reference when useful. The goal is to catch
problems early, not to rewrite sound code for every warning or silence useful
diagnostics across the project.

Common warnings:

- `-Wunsafe-buffer-usage` fires on pointer arithmetic and indexing raw pointers. Use `std::span`.
- `-Wmissing-designated-field-initializers` fires when a designated initializer omits a field, even when that field has a default. Either list every field or default-construct and assign.
- `-Wunused-but-set-variable` fires on a variable that is assigned and never read. Declare it when you start using it.

## Strings and text

Use `std::to_string` for a plain value-to-string conversion. When a string needs
surrounding text or a specific numeric format, use `pomdog::format` from
`pomdog/utility/string_format.h`. Its format string is checked at compile time.

```cpp
const auto scoreText = std::to_string(score); // OK: conversion only.
const auto label = pomdog::format("Score: {}", score); // OK: formatted text.
const auto joined = "Score: " + std::to_string(score); // NG: use format for this.
```

For engine diagnostics, prefer `pomdog::console::write_line` or
`pomdog::console::println` over `std::printf` and `std::cout`. The console library
routes output to the browser console on Emscripten and can write to the debugger
on Windows.

Take borrowed text as `std::string_view` and own it as `std::string`.
A `string_view` must not outlive the string it points into, which matters most for APIs that store a view and read it later.
If a stored view cannot be proven valid at read time, copy into a `std::string` member with `assign`, which reuses the existing capacity.

```cpp
std::string name = "Pomdog";
std::string_view borrowed = name; // OK while name lives.
std::string owned = name; // OK when text must outlive name.
std::string_view dangling = std::string{"Pomdog"}; // NG: temporary dies.
```

Unless an API specifies another encoding, treat text as UTF-8. An API may use
UTF-16, UTF-32, Shift-JIS, or another encoding; make that contract explicit.
For UTF-8 text, do not truncate by byte count, and keep byte sequences valid
before handing them to font rendering.

## Where order is observable

Define the processing order when it affects results: entity traversal in the ECS,
draw submission, asset pipeline output, and data serialized or compared between
runs or platforms. In those cases:

- Use `std::stable_sort` so equal elements keep their input order.
- Do not iterate `std::unordered_map` or `std::unordered_set` when the visit order affects the result. Use `std::map` or a sorted `std::vector`.
- Take random numbers from a generator in `pomdog/random/`, such as `Xoroshiro128StarStar`, with the state owned by the caller. Do not use `std::rand` or a freshly seeded `std::random_device`.
- Avoid undefined behavior such as signed overflow or reading uninitialized memory, which lets optimization change results.

Where order does not affect results, choose containers and algorithms for the
operation they perform.

## Library API design

Make ownership, required inputs, and failure behavior explicit in application-facing APIs.

- Keep helpers optional and limited to one common operation. For other behavior, let applications compose the underlying functions. Document the helper's scope and optional use, as in `setupDefaultVFS`.
- Keep platform differences inside the backend implementation. They should not appear in a public signature.

## API compatibility

Pomdog is a hobby project. Its developer uses it to build games and feeds lessons
from those games back into the engine. Development has continued for more than a
decade. C++0x was current when the project began; C++26 now exists. Game
platforms have changed too, and one maintainer cannot preserve every earlier
API. Pomdog allows breaking changes to keep the engine a current, minimal
reference for game development.

Stable APIs still matter to people building games with Pomdog. Change an API when
game use, language support, or a platform change justifies it; avoid breaking
callers for incidental cleanup. Explain what breaks, and update the relevant
docs, examples, tests, and tools in the same change. Keep every repository
sample, test case, and tool working. `examples/quickstart` also serves as the
template for new projects.

## Comments

Write documentation comments in English, using [Go-style comments](https://go.dev/doc/comment) as a loose
model. Function comments often start with a verb, but need not. Record behavior,
constraints, and background knowledge that readers cannot infer from the code.
The wording matters more than a uniform comment template.

For example, a function comment often takes this form:

```cpp
/// Returns the stored integer for name, or nullopt when name is absent.
[[nodiscard]] std::optional<i32>
getInt(std::string_view name) const noexcept;
```

Avoid requiring Doxygen tags such as `@param`, `@return`, and `@note`. A rule to
fill every tag encourages comments that repeat parameter names or types without
adding information. Explain nullability and platform behavior when they matter;
skip comments that only restate a declaration.

Use `NOTE`, `TODO`, and `FIXME` to show why a comment exists. Without a prefix,
"Add an argument" could be usage guidance, planned work, or a known defect.

Prefixes separate the kinds of comment:

| Prefix | Use |
|:---|:---|
| `// TODO:` | Not implemented yet, or planned to change, including optimization ideas |
| `// FIXME:` | A known defect or a temporary workaround |
| `// NOTE:` | A constraint or design reason needed to maintain correct behavior |

For example, each prefix changes what the reader should do with a comment:

```cpp
// NOTE: Pass a `name` argument when calling the platform API.
// TODO: Add a `name` argument to the wrapper API.
// FIXME: This platform call omits the required `name` argument.
```

Avoid a comment that leaves its intent unclear:

```cpp
// Add a name argument.
```

Other explanatory comments take no prefix.
Leave dates, review rounds, and implementation history in Git. Update comments
that no longer describe the code.

## Tests

Unit tests use [doctest](https://github.com/doctest/doctest) and build into the `pomdog_tests` executable.
See [Running the Tests](running-the-tests.md) for how to run them.

- Put a test at `tests/<area>/<name>_test.cpp` under the area it covers, and add it to the explicit source list in `tests/CMakeLists.txt`.
- Include `tests/testing/testing.h` and wrap the doctest include in its suppression macros.
- One `TEST_CASE` per type or function, with `SUBCASE` for the individual behaviors.

```cpp
// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"
#include "pomdog/math/vector2.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::Vector2;

TEST_CASE("Vector2")
{
    SUBCASE("addition")
    {
        REQUIRE(Vector2{5, 7} == Vector2{2, 3} + Vector2{3, 4});
    }
}
```

Compare floating-point values with `doctest::Approx` rather than `==`.
Tests run on every supported platform, including Emscripten under Node.js, so keep them free of host-specific paths and timing assumptions.

## Keeping changes small

Keep formatting, renaming, and restructuring limited to the task. Put unrelated
changes in separate patches so reviewers can follow each change and its history.

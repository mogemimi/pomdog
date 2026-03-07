// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/memory/unsafe_ptr.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <variant>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

/// CLIParser parses command-line arguments for flags and positional arguments.
///
/// Usage:
///   CLIParser parser;
///   auto verbose = parser.addBool("verbose", "v", "enable verbose output");
///   auto count = parser.addI32("count", "c", "number of items");
///   auto err = parser.parse(argc, argv);
///   if (*verbose) { ... }
///   auto n = *count; // i32
class POMDOG_EXPORT CLIParser final {
private:
    using FlagValue = std::variant<
        unsafe_ptr<bool>,
        unsafe_ptr<i32>,
        unsafe_ptr<u32>,
        unsafe_ptr<u64>,
        unsafe_ptr<std::string>,
        unsafe_ptr<std::vector<std::string>>>;

    struct FlagEntry final {
        std::string name;
        std::string usage;
        FlagValue value;
    };

    std::vector<FlagEntry> flags_;
    std::vector<std::string> args_;

public:
    CLIParser() noexcept;
    ~CLIParser() noexcept;

    CLIParser(const CLIParser&) = delete;
    CLIParser& operator=(const CLIParser&) = delete;
    CLIParser(CLIParser&&) noexcept;
    CLIParser& operator=(CLIParser&&) noexcept;

    /// Adds a boolean flag.
    /// The flag is set to true if specified on the command line, otherwise no change.
    /// `ptr` is a pointer to the variable that will be set when the flag is parsed.
    void add(unsafe_ptr<bool> ptr, std::string_view name, std::string_view usage) noexcept;

    /// Adds an i32 (int32_t) flag.
    /// The flag requires a value argument, e.g. `-count 5`.
    /// `ptr` is a pointer to the variable that will be set when the flag is parsed.
    void add(unsafe_ptr<i32> ptr, std::string_view name, std::string_view usage) noexcept;

    /// Adds a u32 (uint32_t) flag.
    /// The flag requires a value argument, e.g. `-count 5`.
    /// `ptr` is a pointer to the variable that will be set when the flag is parsed.
    void add(unsafe_ptr<u32> ptr, std::string_view name, std::string_view usage) noexcept;

    /// Adds a u64 (uint64_t) flag.
    /// The flag requires a value argument, e.g. `-count 5`.
    /// `ptr` is a pointer to the variable that will be set when the flag is parsed.
    void add(unsafe_ptr<u64> ptr, std::string_view name, std::string_view usage) noexcept;

    /// Adds a string flag.
    /// The flag requires a value argument, e.g. `-output out.txt`.
    /// `ptr` is a pointer to the variable that will be set when the flag is parsed.
    void add(unsafe_ptr<std::string> ptr, std::string_view name, std::string_view usage) noexcept;

    /// Adds a string list flag (can be specified multiple times).
    /// The flag requires a value argument, e.g. `-file a.txt -file b.txt`.
    /// `ptr` is a pointer to the variable that will be set when the flag is parsed.
    void add(unsafe_ptr<std::vector<std::string>> ptr, std::string_view name, std::string_view usage) noexcept;

    /// Parses the command-line arguments.
    /// `argc` and `argv` are typically passed from the `main()` function.
    /// Returns nullptr on success, or an error describing the parse failure.
    [[nodiscard]] std::unique_ptr<Error>
    parse(int argc, const char* const* argv) noexcept;

    /// Returns the positional (non-flag) arguments remaining after parsing.
    /// For example, if the command line is `prog -verbose pos1 pos2`, then `getArgs()` will return `["pos1", "pos2"]`.
    [[nodiscard]] std::span<const std::string>
    getArgs() const noexcept;
};

} // namespace pomdog

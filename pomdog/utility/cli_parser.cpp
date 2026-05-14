// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/utility/cli_parser.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <charconv>
#include <type_traits>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

CLIParser::CLIParser() noexcept = default;
CLIParser::~CLIParser() noexcept = default;
CLIParser::CLIParser(CLIParser&&) noexcept = default;
CLIParser& CLIParser::operator=(CLIParser&&) noexcept = default;

void CLIParser::add(unsafe_ptr<bool> ptr, std::string_view name, std::string_view usage) noexcept
{
    POMDOG_ASSERT(!name.empty());
    POMDOG_ASSERT(ptr != nullptr);

    FlagEntry entry = {};
    entry.name = std::string(name);
    entry.usage = std::string(usage);
    entry.value = ptr;
    flags_.push_back(std::move(entry));
}

void CLIParser::add(unsafe_ptr<i32> ptr, std::string_view name, std::string_view usage) noexcept
{
    POMDOG_ASSERT(!name.empty());
    POMDOG_ASSERT(ptr != nullptr);

    FlagEntry entry = {};
    entry.name = std::string(name);
    entry.usage = std::string(usage);
    entry.value = ptr;
    flags_.push_back(std::move(entry));
}

void CLIParser::add(unsafe_ptr<u32> ptr, std::string_view name, std::string_view usage) noexcept
{
    POMDOG_ASSERT(!name.empty());
    POMDOG_ASSERT(ptr != nullptr);

    FlagEntry entry = {};
    entry.name = std::string(name);
    entry.usage = std::string(usage);
    entry.value = ptr;
    flags_.push_back(std::move(entry));
}

void CLIParser::add(unsafe_ptr<u64> ptr, std::string_view name, std::string_view usage) noexcept
{
    POMDOG_ASSERT(!name.empty());
    POMDOG_ASSERT(ptr != nullptr);

    FlagEntry entry = {};
    entry.name = std::string(name);
    entry.usage = std::string(usage);
    entry.value = ptr;
    flags_.push_back(std::move(entry));
}

void CLIParser::add(unsafe_ptr<std::string> ptr, std::string_view name, std::string_view usage) noexcept
{
    POMDOG_ASSERT(!name.empty());
    POMDOG_ASSERT(ptr != nullptr);

    FlagEntry entry = {};
    entry.name = std::string(name);
    entry.usage = std::string(usage);
    entry.value = ptr;
    flags_.push_back(std::move(entry));
}

void CLIParser::add(unsafe_ptr<std::vector<std::string>> ptr, std::string_view name, std::string_view usage) noexcept
{
    POMDOG_ASSERT(!name.empty());
    POMDOG_ASSERT(ptr != nullptr);

    FlagEntry entry = {};
    entry.name = std::string(name);
    entry.usage = std::string(usage);
    entry.value = ptr;
    flags_.push_back(std::move(entry));
}

std::unique_ptr<Error>
CLIParser::parse(std::span<const char* const> args) noexcept
{
    args_.clear();

    if (args.empty()) {
        // NOTE: No arguments (e.g., Emscripten). Nothing to parse.
        return nullptr;
    }

    std::sort(flags_.begin(), flags_.end(), [](const FlagEntry& a, const FlagEntry& b) {
        return a.name < b.name;
    });

    const auto findFlag = [&] [[nodiscard]] (std::string_view arg) noexcept -> FlagEntry* {
        // NOTE: Binary search since flags_ is sorted by name
        auto it = std::lower_bound(flags_.begin(), flags_.end(), arg, [](const FlagEntry& a, std::string_view b) {
            return a.name < b;
        });

        if (it != flags_.end() && it->name == arg) {
            return &(*it);
        }
        return nullptr;
    };

    // NOTE: Skip args[0] (program name)
    std::size_t i = 1;
    while (i < args.size()) {
        const std::string_view arg(args[i]);

        // NOTE: Check for "--" separator (end of flags)
        if (arg == "--") {
            i++;
            // NOTE: Remaining arguments are positional
            for (; i < args.size(); i++) {
                args_.emplace_back(args[i]);
            }
            break;
        }

        // NOTE: Check if it's a flag
        if (arg.size() >= 2 && arg[0] == '-') {
            // NOTE: Strip leading dashes to get the flag name
            std::string_view flagName = arg;
            while (!flagName.empty() && flagName[0] == '-') {
                flagName.remove_prefix(1);
            }
            if (flagName.empty()) {
                return errors::make("invalid flag: " + std::string(arg));
            }

            const auto flag = findFlag(flagName);
            if (flag == nullptr) {
                return errors::make("unknown flag: " + std::string(arg));
            }

            // NOTE: Handle the variant value type
            auto err = std::visit(
                [&](auto* ptr) -> std::unique_ptr<Error> {
                    using T = std::remove_pointer_t<decltype(ptr)>;

                    if constexpr (std::is_same_v<T, bool>) {
                        *ptr = true;
                        return nullptr;
                    }
                    else {
                        // NOTE: Needs a value argument
                        i++;
                        if (i >= args.size()) {
                            return errors::make("flag " + std::string(arg) + " requires a value");
                        }

                        const std::string_view valStr(args[i]);

                        if constexpr (std::is_same_v<T, i32>) {
                            auto [p, ec] = std::from_chars(valStr.data(), valStr.data() + valStr.size(), *ptr);
                            if (ec != std::errc{}) {
                                return errors::make("invalid value for flag " + std::string(arg) + ": " + std::string(valStr));
                            }
                        }
                        else if constexpr (std::is_same_v<T, u32>) {
                            auto [p, ec] = std::from_chars(valStr.data(), valStr.data() + valStr.size(), *ptr);
                            if (ec != std::errc{}) {
                                return errors::make("invalid value for flag " + std::string(arg) + ": " + std::string(valStr));
                            }
                        }
                        else if constexpr (std::is_same_v<T, u64>) {
                            auto [p, ec] = std::from_chars(valStr.data(), valStr.data() + valStr.size(), *ptr);
                            if (ec != std::errc{}) {
                                return errors::make("invalid value for flag " + std::string(arg) + ": " + std::string(valStr));
                            }
                        }
                        else if constexpr (std::is_same_v<T, std::string>) {
                            *ptr = std::string(valStr);
                        }
                        else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                            ptr->emplace_back(std::string(valStr));
                        }

                        return nullptr;
                    }
                },
                flag->value);

            if (err != nullptr) {
                return err;
            }
        }
        else {
            // NOTE: Positional argument
            args_.emplace_back(arg);
        }

        i++;
    }

    return nullptr;
}

std::span<const std::string>
CLIParser::getArgs() const noexcept
{
    return args_;
}

} // namespace pomdog

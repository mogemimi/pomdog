// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_USERPREFERENCES_13E71C4A_HPP
#define POMDOG_USERPREFERENCES_13E71C4A_HPP

#include "Pomdog/Utility/Optional.hpp"
#include <vector>
#include <string>

namespace Pomdog {

class UserPreferences {
public:
    UserPreferences();

    Optional<bool> GetBool(std::string const& key) const;

    Optional<float> GetFloat(std::string const& key) const;

    Optional<int> GetInt(std::string const& key) const;

    Optional<std::string> GetString(std::string const& key) const;

    bool HasKey(std::string const& key);

    void SetBool(std::string const& key, bool value);

    void SetFloat(std::string const& key, float value);

    void SetInt(std::string const& key, int value);

    void SetString(std::string const& key, std::string const& value);

    void Save();

private:
    std::string filePath;
    std::string jsonData;
    bool needToSave;
};

} // namespace Pomdog

#endif // POMDOG_USERPREFERENCES_13E71C4A_HPP

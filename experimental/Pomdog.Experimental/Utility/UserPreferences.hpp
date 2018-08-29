// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include <optional>
#include <vector>
#include <string>

namespace Pomdog {

class UserPreferences {
public:
    UserPreferences();

    std::optional<bool> GetBool(const std::string& key) const;

    std::optional<float> GetFloat(const std::string& key) const;

    std::optional<int> GetInt(const std::string& key) const;

    std::optional<std::string> GetString(const std::string& key) const;

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

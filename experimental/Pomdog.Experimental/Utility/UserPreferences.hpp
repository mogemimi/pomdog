// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include <optional>
#include <string>
#include <vector>

namespace Pomdog {

class UserPreferences final {
public:
    UserPreferences();

    std::optional<bool> GetBool(const std::string& key) const;

    std::optional<float> GetFloat(const std::string& key) const;

    std::optional<int> GetInt(const std::string& key) const;

    std::optional<std::string> GetString(const std::string& key) const;

    bool HasKey(const std::string& key);

    void SetBool(const std::string& key, bool value);

    void SetFloat(const std::string& key, float value);

    void SetInt(const std::string& key, int value);

    void SetString(const std::string& key, const std::string& value);

    void Save();

private:
    std::string filePath;
    std::string jsonData;
    bool needToSave;
};

} // namespace Pomdog

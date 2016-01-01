// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "UserPreferences.hpp"
#include "Pomdog/Content/Utility/BinaryFileStream.hpp"
#include "Pomdog/Content/Utility/BinaryReader.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include "Pomdog/Utility/FileSystem.hpp"
#include "Pomdog/Utility/PathHelper.hpp"
#include "Pomdog/Logging/Log.hpp"
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <fstream>

namespace Pomdog {
namespace {

template <typename T>
rapidjson::Value ToJsonValue(T const& v)
{
    auto value = rapidjson::Value(v);
    return std::move(value);
}
//-----------------------------------------------------------------------
rapidjson::Value ToJsonValue(std::string const& s)
{
    auto value = rapidjson::Value(
        s.c_str(),
        static_cast<rapidjson::SizeType>(s.size()));
    return std::move(value);
}
//-----------------------------------------------------------------------
template <typename T, typename Func>
Optional<T> GetJsonValue(std::string const& jsonData, std::string const& key, Func const& func)
{
    POMDOG_ASSERT(!key.empty());

    if (jsonData.empty()) {
        return NullOpt;
    }

    rapidjson::Document doc;
    doc.Parse(jsonData.data());

    POMDOG_ASSERT(!doc.HasParseError());
    POMDOG_ASSERT(doc.IsObject());

    if (!doc.HasMember(key.c_str())) {
        return NullOpt;
    }

    auto & v = doc[key.c_str()];
    return func(v);
}
//-----------------------------------------------------------------------
template <typename T>
void SetJsonValue(std::string & jsonData, std::string const& key, T const& value)
{
    POMDOG_ASSERT(!key.empty());

    rapidjson::Document doc;

    if (!jsonData.empty()) {
        doc.Parse(jsonData.data());
        POMDOG_ASSERT(!doc.HasParseError());
        POMDOG_ASSERT(doc.IsObject());
    }

    if (!doc.HasMember(key.c_str())) {
        doc.AddMember(ToJsonValue(key), ToJsonValue(value), doc.GetAllocator());
    }
    else {
        doc[key.c_str()] = ToJsonValue(value);
    }

    rapidjson::StringBuffer buf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buf);
    doc.Accept(writer);
    jsonData.clear();
    jsonData.assign(buf.GetString(), buf.GetSize());
}

} // unnamed namespace
//-----------------------------------------------------------------------
UserPreferences::UserPreferences()
    : needToSave(false)
{
    using Detail::BinaryReader;

    const auto directory = FileSystem::GetLocalAppDataDirectoryPath();
    filePath = PathHelper::Join(directory, "UserPreferences.json");

    if (!FileSystem::IsDirectory(directory)) {
        if (!FileSystem::CreateDirectory(directory)) {
            POMDOG_THROW_EXCEPTION(std::runtime_error,
                "Failed to create directory: " + directory);
        }
    }

    if (FileSystem::Exists(filePath)) {
        auto binaryFile = PathHelper::OpenStream(filePath);
        if (!binaryFile.Stream) {
            POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to open file");
        }

        if (binaryFile.SizeInBytes > 0) {
            auto data = BinaryReader::ReadString<char>(
                binaryFile.Stream, binaryFile.SizeInBytes);
            jsonData.assign(data.data(), data.size());
            POMDOG_ASSERT(!jsonData.empty());
        }
    }

    if (!jsonData.empty()) {
        rapidjson::Document doc;
        doc.Parse(jsonData.data());

        if (doc.HasParseError() || !doc.IsObject()) {
            // FUS RO DAH
            POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to parse JSON");
        }
    }
    else {
        jsonData = "{}";
    }

    Log::Internal("UserPreferences path: " + filePath);
}
//-----------------------------------------------------------------------
Optional<bool> UserPreferences::GetBool(std::string const& key) const
{
    POMDOG_ASSERT(!key.empty());

    return GetJsonValue<bool>(jsonData, key,
        [](rapidjson::Value const& v)->Optional<bool> {
            if (v.IsBool()) {
                return v.GetBool();
            }
            return NullOpt;
        });
}
//-----------------------------------------------------------------------
Optional<float> UserPreferences::GetFloat(std::string const& key) const
{
    POMDOG_ASSERT(!key.empty());

    return GetJsonValue<float>(jsonData, key,
        [](rapidjson::Value const& v)->Optional<float> {
            if (v.IsNumber()) {
                return static_cast<float>(v.GetDouble());
            }
            return NullOpt;
        });
}
//-----------------------------------------------------------------------
Optional<int> UserPreferences::GetInt(std::string const& key) const
{
    return GetJsonValue<int>(jsonData, key,
        [](rapidjson::Value const& v)->Optional<int> {
            if (v.IsInt()) {
                return v.GetInt();
            }
            if (v.IsUint()) {
                return v.GetUint();
            }
            return NullOpt;
        });
}
//-----------------------------------------------------------------------
Optional<std::string> UserPreferences::GetString(std::string const& key) const
{
    return GetJsonValue<std::string>(jsonData, key,
        [](rapidjson::Value const& v)->Optional<std::string> {
            if (v.IsString()) {
                std::string value = v.GetString();
                return std::move(value);
            }
            return NullOpt;
        });
}
//-----------------------------------------------------------------------
bool UserPreferences::HasKey(std::string const& key)
{
    POMDOG_ASSERT(!key.empty());

    if (jsonData.empty()) {
        return false;
    }

    rapidjson::Document doc;
    doc.Parse(jsonData.data());

    POMDOG_ASSERT(!doc.HasParseError());
    POMDOG_ASSERT(doc.IsObject());

    return doc.HasMember(key.c_str());
}
//-----------------------------------------------------------------------
void UserPreferences::SetBool(std::string const& key, bool value)
{
    POMDOG_ASSERT(!key.empty());
    SetJsonValue(jsonData, key, value);
    needToSave = true;
}
//-----------------------------------------------------------------------
void UserPreferences::SetFloat(std::string const& key, float value)
{
    POMDOG_ASSERT(!key.empty());
    SetJsonValue(jsonData, key, value);
    needToSave = true;
}
//-----------------------------------------------------------------------
void UserPreferences::SetInt(std::string const& key, int value)
{
    POMDOG_ASSERT(!key.empty());
    SetJsonValue(jsonData, key, value);
    needToSave = true;
}
//-----------------------------------------------------------------------
void UserPreferences::SetString(std::string const& key, std::string const& value)
{
    POMDOG_ASSERT(!key.empty());
    SetJsonValue(jsonData, key, value);
    needToSave = true;
}
//-----------------------------------------------------------------------
void UserPreferences::Save()
{
    POMDOG_ASSERT(!filePath.empty());

    if (!needToSave) {
        return;
    }

    if (jsonData.empty()) {
        return;
    }

    std::ofstream stream(filePath, std::ios_base::out | std::ios_base::binary);

    if (!stream) {
        // FUS RO DAH
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Cannot open file: " + filePath);
    }

    stream << jsonData;
    needToSave = false;
}
//-----------------------------------------------------------------------
} // namespace Pomdog

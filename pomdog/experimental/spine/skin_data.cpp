// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/spine/skin_data.h"
#include "pomdog/basic/flatbuffers_macros.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/string_format.h"
#include "pomdog/vfs/file.h"
#include "pomdog/vfs/file_system.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_BEGIN
#include "pomdogschemas/skeletal2d_skin_generated.h"
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_END

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <iterator>
#include <limits>
#include <string>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::spine {
namespace {

// NOTE: A joint index of 255 means no joint, and a mesh index of 65535 means no mesh.
constexpr u8 noJoint = 255;
constexpr u16 noMesh = 65535;

template <typename T>
[[nodiscard]] u32
getSize(const flatbuffers::Vector<T>* vector) noexcept
{
    if (vector == nullptr) {
        return 0;
    }
    return vector->size();
}

[[nodiscard]] std::unique_ptr<Error>
validateMesh(const pomdogschemas::Skeletal2DMesh& mesh) noexcept
{
    const auto* vertices = mesh.vertices();
    const auto vertexCount = getSize(vertices);
    if (vertexCount == 0) {
        return errors::make("mesh has no vertices");
    }
    if (vertexCount > std::numeric_limits<u16>::max()) {
        return errors::make(pomdog::format("mesh has {} vertices, but the limit is {}", vertexCount, std::numeric_limits<u16>::max()));
    }
    for (const auto* vertex : *vertices) {
        if (vertex->joint0() == noJoint) {
            return errors::make("mesh has a vertex that follows no joint");
        }
    }

    const auto* indices = mesh.indices();
    const auto indexCount = getSize(indices);
    if (indexCount == 0 || indexCount % 3 != 0) {
        return errors::make(pomdog::format("mesh has {} indices, but triangles need three indices each", indexCount));
    }
    for (const auto index : *indices) {
        if (index >= vertexCount) {
            return errors::make(pomdog::format("mesh has an index {} that is out of its {} vertices", index, vertexCount));
        }
    }
    return nullptr;
}

[[nodiscard]] std::unique_ptr<Error>
validateSkin(const pomdogschemas::Skeletal2DSkin& skin) noexcept
{
    const auto* meshes = skin.meshes();
    const auto meshCount = getSize(meshes);
    for (u32 index = 0; index < meshCount; ++index) {
        if (auto err = validateMesh(*meshes->Get(index)); err != nullptr) {
            return errors::wrap(std::move(err), pomdog::format("invalid mesh {}", index));
        }
    }

    const auto* slots = skin.slots();
    if (slots == nullptr) {
        return nullptr;
    }
    for (const auto* slot : *slots) {
        if (slot->joint() == noJoint) {
            return errors::make("skin has a slot that follows no joint");
        }
        if (slot->mesh() != noMesh && slot->mesh() >= meshCount) {
            return errors::make(pomdog::format("skin has a slot whose mesh {} is out of its {} meshes", slot->mesh(), meshCount));
        }
    }
    return nullptr;
}

[[nodiscard]] std::unique_ptr<Error>
validateSkinSet(const pomdogschemas::Skeletal2DSkinSet& table) noexcept
{
    const auto* keys = table.keys();
    const auto* skins = table.skins();
    const auto skinCount = getSize(skins);
    if (getSize(keys) != skinCount) {
        return errors::make("keys and skins must have the same length");
    }

    for (u32 index = 0; index < skinCount; ++index) {
        // NOTE: findSkin() searches the keys with a binary search.
        if (index > 0 && keys->Get(index - 1) >= keys->Get(index)) {
            return errors::make("keys must be in ascending order without duplicates");
        }
        if (auto err = validateSkin(*skins->Get(index)); err != nullptr) {
            return errors::wrap(std::move(err), pomdog::format("invalid skin {}", index));
        }
    }
    return nullptr;
}

} // namespace

SkinData::SkinData(
    std::shared_ptr<vfs::File> file,
    const pomdogschemas::Skeletal2DSkinSet* table) noexcept
    : file_(std::move(file))
    , table_(table)
{
    POMDOG_ASSERT(file_ != nullptr);
    POMDOG_ASSERT(table_ != nullptr);
}

const pomdogschemas::Skeletal2DSkin*
SkinData::findSkin(u32 key) const noexcept
{
    POMDOG_ASSERT(table_ != nullptr);

    const auto* keys = table_->keys();
    if (keys == nullptr) {
        return nullptr;
    }

    const auto iter = std::lower_bound(keys->begin(), keys->end(), key);
    if (iter == keys->end() || *iter != key) {
        return nullptr;
    }

    const auto index = std::distance(keys->begin(), iter);
    return table_->skins()->Get(static_cast<u32>(index));
}

std::tuple<std::shared_ptr<SkinData>, std::unique_ptr<Error>>
loadSkinData(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    std::string_view filePath) noexcept
{
    const auto path = std::string{filePath};

    auto [file, openErr] = vfs::open(fs, path);
    if (openErr != nullptr) {
        return {nullptr, errors::wrap(std::move(openErr), "cannot open skin file, " + path)};
    }

    auto [bytes, viewErr] = file->view();
    if (viewErr != nullptr) {
        return {nullptr, errors::wrap(std::move(viewErr), "cannot read skin file, " + path)};
    }

    flatbuffers::Verifier verifier(bytes.data(), bytes.size());
    if (!pomdogschemas::VerifySkeletal2DSkinSetBuffer(verifier)) {
        return {nullptr, errors::make("FlatBuffers verification failed for skin file, " + path)};
    }

    const auto* table = pomdogschemas::GetSkeletal2DSkinSet(bytes.data());
    if (auto err = validateSkinSet(*table); err != nullptr) {
        return {nullptr, errors::wrap(std::move(err), "invalid skin file, " + path)};
    }

    return {std::make_shared<SkinData>(std::move(file), table), nullptr};
}

} // namespace pomdog::spine

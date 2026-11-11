// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/skeletal2d/skeleton_data.h"
#include "pomdog/basic/flatbuffers_macros.h"
#include "pomdog/basic/types.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/string_format.h"
#include "pomdog/vfs/file.h"
#include "pomdog/vfs/file_system.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_BEGIN
#include "pomdogschemas/skeletal2d_skeleton_generated.h"
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_END

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d {
namespace {

// NOTE: A joint index of 255 means no joint, so the indices 0 to 254 address joints.
constexpr u8 noJoint = 255;
constexpr u32 maxJointCount = 255;

[[nodiscard]] std::unique_ptr<Error>
validateSkeleton(const pomdogschemas::Skeletal2DSkeleton& table) noexcept
{
    const auto* joints = table.joints();
    if (joints == nullptr || joints->size() == 0) {
        return errors::make("skeleton has no joints");
    }
    if (joints->size() > maxJointCount) {
        return errors::make(pomdog::format("skeleton has {} joints, but the limit is {}", joints->size(), maxJointCount));
    }
    if (joints->Get(0)->parent() != noJoint) {
        return errors::make("the first joint must be the root, a joint without a parent");
    }

    for (u32 index = 1; index < joints->size(); ++index) {
        // NOTE: A parent that precedes its children also rules out cycles.
        if (joints->Get(index)->parent() >= index) {
            return errors::make(pomdog::format("the parent of joint {} must precede it", index));
        }
    }
    return nullptr;
}

} // namespace

SkeletonData::SkeletonData(
    std::shared_ptr<vfs::File> file,
    const pomdogschemas::Skeletal2DSkeleton* table) noexcept
    : file_(std::move(file))
    , table_(table)
{
    POMDOG_ASSERT(file_ != nullptr);
    POMDOG_ASSERT(table_ != nullptr);
}

const pomdogschemas::Skeletal2DSkeleton&
SkeletonData::getTable() const noexcept
{
    POMDOG_ASSERT(table_ != nullptr);
    return *table_;
}

std::tuple<std::shared_ptr<SkeletonData>, std::unique_ptr<Error>>
loadSkeletonData(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    std::string_view filePath) noexcept
{
    const auto path = std::string{filePath};

    auto [file, openErr] = vfs::open(fs, path);
    if (openErr != nullptr) {
        return {nullptr, errors::wrap(std::move(openErr), "cannot open skeleton file, " + path)};
    }

    auto [bytes, viewErr] = file->view();
    if (viewErr != nullptr) {
        return {nullptr, errors::wrap(std::move(viewErr), "cannot read skeleton file, " + path)};
    }

    flatbuffers::Verifier verifier(bytes.data(), bytes.size());
    if (!pomdogschemas::VerifySkeletal2DSkeletonBuffer(verifier)) {
        return {nullptr, errors::make("FlatBuffers verification failed for skeleton file, " + path)};
    }

    const auto* table = pomdogschemas::GetSkeletal2DSkeleton(bytes.data());
    if (auto err = validateSkeleton(*table); err != nullptr) {
        return {nullptr, errors::wrap(std::move(err), "invalid skeleton file, " + path)};
    }

    return {std::make_shared<SkeletonData>(std::move(file), table), nullptr};
}

} // namespace pomdog::skeletal2d

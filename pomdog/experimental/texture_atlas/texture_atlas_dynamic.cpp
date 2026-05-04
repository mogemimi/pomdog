// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/texture_atlas/texture_atlas_dynamic.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/image/image.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/gpu/texture2d.h"
#include "pomdog/math/color.h"
#include "pomdog/math/rect2d.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/string_hash32.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <array>
#include <cstring>
#include <functional>
#include <limits>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
namespace {

struct PackNode final {
    std::array<std::unique_ptr<PackNode>, 2> children;
    std::shared_ptr<Image> image;
    Rect2D rect;
    Rect2D clipBounds;

    explicit PackNode(const Rect2D& r) noexcept
        : rect(r)
    {
    }
};

[[nodiscard]] PackNode*
insert(PackNode* node, int w, int h) noexcept
{
    const bool isLeaf = (!node->children[0] && !node->children[1]);
    if (!isLeaf) {
        if (auto* n = insert(node->children[0].get(), w, h); n != nullptr) {
            return n;
        }
        return insert(node->children[1].get(), w, h);
    }

    const auto dw = node->rect.width - w;
    const auto dh = node->rect.height - h;

    if ((w > node->rect.width) || (h > node->rect.height)) {
        return nullptr;
    }

    if (dw > dh) {
        node->children[0] = std::make_unique<PackNode>(Rect2D{
            node->rect.x,
            node->rect.y + h,
            w,
            dh,
        });
        node->children[1] = std::make_unique<PackNode>(Rect2D{
            node->rect.x + w,
            node->rect.y,
            dw,
            node->rect.height,
        });
    }
    else {
        node->children[0] = std::make_unique<PackNode>(Rect2D{
            node->rect.x + w,
            node->rect.y,
            dw,
            h,
        });
        node->children[1] = std::make_unique<PackNode>(Rect2D{
            node->rect.x,
            node->rect.y + h,
            node->rect.width,
            dh,
        });
    }
    return node;
}

void traverse(const PackNode* node, const std::function<void(const PackNode&)>& fn)
{
    if (node == nullptr) {
        return;
    }
    if (node->image) {
        fn(*node);
    }
    traverse(node->children[0].get(), fn);
    traverse(node->children[1].get(), fn);
}

[[nodiscard]] Rect2D
clipToBounds(const Image& image) noexcept
{
    Rect2D bounds{0, 0, image.GetWidth(), image.GetHeight()};

    bool hasPixels = false;
    int left = image.GetWidth();
    int right = 0;
    int top = image.GetHeight();
    int bottom = 0;

    for (int y = 0; y < image.GetHeight(); ++y) {
        for (int x = 0; x < image.GetWidth(); ++x) {
            if (image.GetPixel(x, y).a != 0) {
                left = std::min(x, left);
                right = std::max(x, right);
                top = std::min(y, top);
                bottom = std::max(y, bottom);
                hasPixels = true;
            }
        }
    }

    if (!hasPixels) {
        return Rect2D{0, 0, 1, 1};
    }

    bounds.x = left;
    bounds.y = top;
    bounds.width = (right - left) + 1;
    bounds.height = (bottom - top) + 1;
    return bounds;
}

struct SourceSprite final {
    std::shared_ptr<Image> image;
    std::string name;
    u32 key;
    Rect2D clipBounds;
};

struct PackedRegion final {
    u32 key;
    i16 subrectX;
    i16 subrectY;
    i16 subrectWidth;
    i16 subrectHeight;
    i16 width;
    i16 height;
    i16 xOffset;
    i16 yOffset;
};

class TextureAtlasDynamicImpl final : public TextureAtlasDynamic {
private:
    std::vector<SourceSprite> sources_;
    std::vector<PackedRegion> regions_;
    std::shared_ptr<gpu::Texture2D> texture_;
    bool premultipliedAlpha_ = false;
    bool built_ = false;

public:
    TextureAtlasDynamicImpl() = default;

    void add(const std::string& name, const std::shared_ptr<Image>& image, bool tightPacking) override
    {
        POMDOG_ASSERT(!built_);
        POMDOG_ASSERT(image != nullptr);

        SourceSprite src = {};
        src.image = image;
        src.name = name;
        src.key = computeStringHash32(name);
        src.clipBounds = tightPacking ? clipToBounds(*image) : Rect2D{0, 0, image->GetWidth(), image->GetHeight()};
        sources_.push_back(std::move(src));
    }

    [[nodiscard]] std::unique_ptr<Error>
    build(
        const std::shared_ptr<gpu::GraphicsDevice>& graphicsDevice,
        int width,
        int height,
        int perTileSpacing) noexcept override
    {
        POMDOG_ASSERT(!built_);
        POMDOG_ASSERT(graphicsDevice != nullptr);
        POMDOG_ASSERT(width > 0);
        POMDOG_ASSERT(height > 0);

        // NOTE: Sort by area, largest first.
        std::stable_sort(sources_.begin(), sources_.end(), [](const SourceSprite& a, const SourceSprite& b) {
            return (a.clipBounds.width * a.clipBounds.height) > (b.clipBounds.width * b.clipBounds.height);
        });

        auto root = std::make_unique<PackNode>(Rect2D{0, 0, width, height});

        for (auto& src : sources_) {
            const auto w = src.clipBounds.width + perTileSpacing;
            const auto h = src.clipBounds.height + perTileSpacing;
            auto* node = insert(root.get(), w, h);
            if (node == nullptr) {
                return errors::make("texture atlas is full — increase atlas dimensions");
            }
            node->image = src.image;
            node->clipBounds = src.clipBounds;
        }

        std::vector<Color> pixels(static_cast<std::size_t>(width * height), Color{0, 0, 0, 0});

        // NOTE: Build a name-to-source lookup for Traverse.
        std::unordered_map<const Image*, std::size_t> imageToIndex;
        for (std::size_t i = 0; i < sources_.size(); ++i) {
            imageToIndex[sources_[i].image.get()] = i;
        }

        traverse(root.get(), [&](const PackNode& node) {
            const auto it = imageToIndex.find(node.image.get());
            if (it == imageToIndex.end()) {
                return;
            }
            const auto& src = sources_[it->second];
            const auto& clip = node.clipBounds;
            const int destX = node.rect.x;
            const int destY = node.rect.y;

            for (int y = 0; y < clip.height; ++y) {
                const auto srcRow = clip.x + (clip.y + y) * src.image->GetWidth();
                const auto dstRow = destX + (destY + y) * width;
                std::memcpy(
                    pixels.data() + dstRow,
                    src.image->GetData() + srcRow,
                    sizeof(Color) * static_cast<std::size_t>(clip.width));
            }

            POMDOG_ASSERT(clip.x <= std::numeric_limits<i16>::max());
            POMDOG_ASSERT(clip.y <= std::numeric_limits<i16>::max());
            POMDOG_ASSERT(clip.width <= std::numeric_limits<i16>::max());
            POMDOG_ASSERT(clip.height <= std::numeric_limits<i16>::max());
            POMDOG_ASSERT(src.image->GetWidth() <= std::numeric_limits<i16>::max());
            POMDOG_ASSERT(src.image->GetHeight() <= std::numeric_limits<i16>::max());

            regions_.push_back(PackedRegion{
                .key = src.key,
                .subrectX = static_cast<i16>(destX),
                .subrectY = static_cast<i16>(destY),
                .subrectWidth = static_cast<i16>(clip.width),
                .subrectHeight = static_cast<i16>(clip.height),
                .width = static_cast<i16>(src.image->GetWidth()),
                .height = static_cast<i16>(src.image->GetHeight()),
                .xOffset = static_cast<i16>(clip.x),
                .yOffset = static_cast<i16>(clip.y),
            });
        });

        // NOTE: Sort regions by key for binary-search lookup.
        std::sort(regions_.begin(), regions_.end(), [](const PackedRegion& a, const PackedRegion& b) {
            return a.key < b.key;
        });

        auto [tex, texErr] = graphicsDevice->createTexture2D(width, height, false, gpu::PixelFormat::R8G8B8A8_UNorm);
        if (texErr != nullptr) {
            return errors::wrap(std::move(texErr), "createTexture2D failed");
        }
        tex->setData(pixels.data());
        texture_ = std::move(tex);
        built_ = true;
        return nullptr;
    }

    [[nodiscard]] std::shared_ptr<gpu::Texture2D>
    getTexture() const noexcept override
    {
        return texture_;
    }

    [[nodiscard]] TextureRegion
    getRegion(u32 index) const noexcept override
    {
        POMDOG_ASSERT(built_);
        POMDOG_ASSERT(index < static_cast<u32>(regions_.size()));
        const auto& r = regions_[index];
        return TextureRegion{
            .subrectX = r.subrectX,
            .subrectY = r.subrectY,
            .subrectWidth = r.subrectWidth,
            .subrectHeight = r.subrectHeight,
            .xOffset = r.xOffset,
            .yOffset = r.yOffset,
            .width = r.width,
            .height = r.height,
        };
    }

    [[nodiscard]] TextureRegion
    getRegionByKey(u32 key) const noexcept override
    {
        if (const auto idx = findRegionByKey(key); idx.has_value()) {
            return getRegion(*idx);
        }
        return TextureRegion{};
    }

    [[nodiscard]] std::optional<u32>
    findRegionByKey(u32 key) const noexcept override
    {
        if (!built_) {
            return std::nullopt;
        }

        const auto iter = std::lower_bound(
            regions_.begin(),
            regions_.end(),
            key,
            [](const PackedRegion& r, u32 k) { return r.key < k; });

        if (iter == regions_.end() || iter->key != key) {
            return std::nullopt;
        }

        const auto index = std::distance(regions_.begin(), iter);
        POMDOG_ASSERT(index >= 0);
        POMDOG_ASSERT(index < static_cast<std::ptrdiff_t>(regions_.size()));

        return static_cast<u32>(index);
    }

    [[nodiscard]] bool
    isPremultipliedAlpha() const noexcept override
    {
        return premultipliedAlpha_;
    }
};

} // namespace

TextureAtlasDynamic::~TextureAtlasDynamic() = default;

[[nodiscard]] std::shared_ptr<TextureAtlasDynamic>
createTextureAtlasDynamic() noexcept
{
    return std::make_shared<TextureAtlasDynamicImpl>();
}

} // namespace pomdog

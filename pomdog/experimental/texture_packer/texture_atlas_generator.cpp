// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/texture_packer/texture_atlas_generator.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/experimental/image/image.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <array>
#include <cstring>
#include <functional>
#include <limits>
#include <unordered_map>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
namespace TexturePacker {
namespace {

struct TexturePackNode {
    std::array<std::shared_ptr<TexturePackNode>, 2> children;
    std::shared_ptr<Image> image;
    Rectangle rect;
    Rectangle clipBounds;

    explicit TexturePackNode(const Rectangle& rectIn)
        : rect(rectIn)
    {
    }
};

std::shared_ptr<TexturePackNode> Insert(
    const std::shared_ptr<TexturePackNode>& node, int width, int height)
{
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);

    const bool isLeaf = (!node->children[0] && !node->children[1]);
    if (!isLeaf) {
        auto newNode = Insert(node->children[0], width, height);
        if (newNode) {
            return newNode;
        }
        return Insert(node->children[1], width, height);
    }

    POMDOG_ASSERT(isLeaf);

    constexpr int marginPixels = 2;
    const auto w = width + marginPixels;
    const auto h = height + marginPixels;
    const auto dw = node->rect.width - w;
    const auto dh = node->rect.height - h;

    if ((w > node->rect.width) || (h > node->rect.height)) {
        // NOTE: This node is too small.
        return nullptr;
    }

    if (dw > dh) {
        node->children[0] = std::make_shared<TexturePackNode>(Rectangle{
            node->rect.x,
            node->rect.y + h,
            w,
            dh,
        });
        node->children[1] = std::make_shared<TexturePackNode>(Rectangle{
            node->rect.x + w,
            node->rect.y,
            dw,
            node->rect.height,
        });
    }
    else {
        node->children[0] = std::make_shared<TexturePackNode>(Rectangle{
            node->rect.x + w,
            node->rect.y,
            dw,
            h,
        });
        node->children[1] = std::make_shared<TexturePackNode>(Rectangle{
            node->rect.x,
            node->rect.y + h,
            node->rect.width,
            dh,
        });
    }
    return node;
}

void Traverse(
    const std::shared_ptr<TexturePackNode>& node,
    const std::function<void(const TexturePackNode&)>& func)
{
    if (!node) {
        return;
    }
    if (node->image) {
        func(*node);
    }
    Traverse(node->children[0], func);
    Traverse(node->children[1], func);
}

Rectangle Clip(const std::shared_ptr<Image>& image)
{
    Rectangle bounds;
    bounds.x = 0;
    bounds.y = 0;
    bounds.width = image->GetWidth();
    bounds.height = image->GetHeight();

    bool hasPixels = false;
    int left = image->GetWidth();
    int right = 0;
    int top = image->GetHeight();
    int bottom = 0;

    for (int y = 0; y < image->GetHeight(); ++y) {
        for (int x = 0; x < image->GetWidth(); ++x) {
            auto pixel = image->GetPixel(x, y);
            POMDOG_ASSERT(pixel.a >= 0);
            if (pixel.a != 0) {
                left = std::min(x, left);
                right = std::max(x, right);
                top = std::min(y, top);
                bottom = std::max(y, bottom);
                hasPixels = true;
            }
        }
    }

    if (!hasPixels) {
        bounds.x = 0;
        bounds.y = 0;
        bounds.width = 1;
        bounds.height = 1;
        return bounds;
    }

    POMDOG_ASSERT(left <= right);
    POMDOG_ASSERT(top <= bottom);

    bounds.x = left;
    bounds.y = top;
    bounds.width = (right - left) + 1;
    bounds.height = (bottom - top) + 1;

    POMDOG_ASSERT(bounds.width <= image->GetWidth());
    POMDOG_ASSERT(bounds.height <= image->GetHeight());

    return bounds;
}

} // namespace

TextureAtlasGeneratorResult TextureAtlasGenerator::Generate(
    const std::vector<TextureAtlasGeneratorSource>& sources,
    int width,
    int height)
{
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);

    TextureAtlasGeneratorResult result;
    auto& regions = result.Atlas.regions;
    std::unordered_map<std::shared_ptr<Image>, int> indices;

    for (auto& source : sources) {
        POMDOG_ASSERT(!source.Name.empty());
        TextureAtlasRegion region;
        region.Name = source.Name;
        regions.push_back(region);

        indices.emplace(source.Image, static_cast<int>(regions.size()) - 1);
        POMDOG_ASSERT(regions.at(indices[source.Image]).Name == source.Name);
    }

    result.HasError = false;

    auto root = std::make_shared<TexturePackNode>(Rectangle{0, 0, width, height});

    for (auto& source : sources) {
        auto& image = source.Image;
        auto clipBounds = Clip(image);
        auto node = Insert(root, clipBounds.width, clipBounds.height);
        POMDOG_ASSERT(node);
        if (!node) {
            // TODO: error handling
            result.HasError = true;
            break;
        }
        node->image = image;
        node->clipBounds = clipBounds;
    }

    std::vector<Color> pixelData(width * height);
    std::fill(pixelData.begin(), pixelData.end(), Color{0, 0, 0, 0});

    Traverse(root, [&](const TexturePackNode& node) {
        const auto image = node.image;
        auto& region = regions[indices[image]].Region;
        region.subrect.x = static_cast<int>(node.rect.x);
        region.subrect.y = static_cast<int>(node.rect.y);
        region.subrect.width = node.clipBounds.width;
        region.subrect.height = node.clipBounds.height;

        static_assert(std::is_same<decltype(node.clipBounds.x), std::int32_t>::value, "");
        static_assert(std::is_same<decltype(node.clipBounds.y), std::int32_t>::value, "");
        static_assert(std::is_same<decltype(region.xOffset), std::int16_t>::value, "");
        static_assert(std::is_same<decltype(region.yOffset), std::int16_t>::value, "");
        static_assert(std::is_same<decltype(image->GetWidth()), int>::value, "");
        static_assert(std::is_same<decltype(image->GetHeight()), int>::value, "");
        static_assert(std::is_same<decltype(region.width), std::int16_t>::value, "");
        static_assert(std::is_same<decltype(region.height), std::int16_t>::value, "");
        POMDOG_ASSERT(node.clipBounds.x <= std::numeric_limits<std::int16_t>::max());
        POMDOG_ASSERT(node.clipBounds.y <= std::numeric_limits<std::int16_t>::max());
        POMDOG_ASSERT(image->GetWidth() <= std::numeric_limits<std::int16_t>::max());
        POMDOG_ASSERT(image->GetHeight() <= std::numeric_limits<std::int16_t>::max());

        region.xOffset = static_cast<std::int16_t>(node.clipBounds.x);
        region.yOffset = static_cast<std::int16_t>(node.clipBounds.y);
        region.width = static_cast<std::int16_t>(image->GetWidth());
        region.height = static_cast<std::int16_t>(image->GetHeight());

        const auto start = region.subrect.x + region.subrect.y * width;
        for (int y = 0; y < region.subrect.height; ++y) {
            const auto offset = region.xOffset + ((region.yOffset + y) * image->GetWidth());
            static_assert(sizeof(decltype(*image->GetData())) == sizeof(Color), "");
            std::memcpy(
                pixelData.data() + start + (y * width),
                image->GetData() + offset,
                sizeof(Color) * region.subrect.width);
        }
    });

    result.Image = std::make_shared<Image>(width, height);
    result.Image->SetData(std::move(pixelData));

    return result;
}

} // namespace TexturePacker
} // namespace pomdog

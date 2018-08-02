// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/TexturePacker/TextureAtlasGenerator.hpp"
#include "Pomdog/Experimental/Image/Image.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <array>
#include <cstring>
#include <functional>
#include <unordered_map>

namespace Pomdog {
namespace TexturePacker {
namespace {

struct TexturePackNode {
    std::array<std::shared_ptr<TexturePackNode>, 2> children;
    std::shared_ptr<Image> image;
    Rectangle rect;
    Rectangle clipBounds;

    explicit TexturePackNode(const Rectangle& rectIn)
        : rect(rectIn)
    {}
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
    const auto dw = node->rect.Width - w;
    const auto dh = node->rect.Height - h;

    if ((w > node->rect.Width) || (h > node->rect.Height)) {
        // NOTE: This node is too small.
        return nullptr;
    }

    if (dw > dh) {
        node->children[0] = std::make_shared<TexturePackNode>(Rectangle{
            node->rect.X,
            node->rect.Y + h,
            w,
            dh
        });
        node->children[1] = std::make_shared<TexturePackNode>(Rectangle{
            node->rect.X + w,
            node->rect.Y,
            dw,
            node->rect.Height
        });
    }
    else {
        node->children[0] = std::make_shared<TexturePackNode>(Rectangle{
            node->rect.X + w,
            node->rect.Y,
            dw,
            h
        });
        node->children[1] = std::make_shared<TexturePackNode>(Rectangle{
            node->rect.X,
            node->rect.Y + h,
            node->rect.Width,
            dh
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
    bounds.X = 0;
    bounds.Y = 0;
    bounds.Width = image->GetWidth();
    bounds.Height = image->GetHeight();

    bool hasPixels = false;
    int left = image->GetWidth();
    int right = 0;
    int top = image->GetHeight();
    int bottom = 0;

    for (int y = 0; y < image->GetHeight(); ++y) {
        for (int x = 0; x < image->GetWidth(); ++x) {
            auto pixel = image->GetPixel(x, y);
            POMDOG_ASSERT(pixel.A >= 0);
            if (pixel.A != 0) {
                left = std::min(x, left);
                right = std::max(x, right);
                top = std::min(y, top);
                bottom = std::max(y, bottom);
                hasPixels = true;
            }
        }
    }

    if (!hasPixels) {
        bounds.X = 0;
        bounds.Y = 0;
        bounds.Width = 1;
        bounds.Height = 1;
        return bounds;
    }

    POMDOG_ASSERT(left <= right);
    POMDOG_ASSERT(top <= bottom);

    bounds.X = left;
    bounds.Y = top;
    bounds.Width = (right - left) + 1;
    bounds.Height = (bottom - top) + 1;

    POMDOG_ASSERT(bounds.Width <= image->GetWidth());
    POMDOG_ASSERT(bounds.Height <= image->GetHeight());

    return bounds;
}

} // unnamed namespace

TextureAtlasGeneratorResult TextureAtlasGenerator::Generate(
    const std::vector<TextureAtlasGeneratorSource>& sources,
    int width,
    int height)
{
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);

    TextureAtlasGeneratorResult result;
    auto & regions = result.Atlas.regions;
    std::unordered_map<std::shared_ptr<Image>, int> indices;

    for (auto & source : sources) {
        POMDOG_ASSERT(!source.Name.empty());
        TextureAtlasRegion region;
        region.Name = source.Name;
        regions.push_back(region);

        indices.emplace(source.Image, regions.size() - 1);
        POMDOG_ASSERT(regions.at(indices[source.Image]).Name == source.Name);
    }

    result.HasError = false;

    auto root = std::make_shared<TexturePackNode>(Rectangle{0, 0, width, height});

    for (auto & source : sources) {
        auto & image = source.Image;
        auto clipBounds = Clip(image);
        auto node = Insert(root, clipBounds.Width, clipBounds.Height);
        POMDOG_ASSERT(node);
        if (!node) {
            // TODO: error handling
            result.HasError = true;
        #if defined(DEBUG)
            std::printf("Cannot pack the texture '%s'", regions[indices[image]].Name.c_str());
        #endif
            break;
        }
        node->image = image;
        node->clipBounds = clipBounds;
    }

    std::vector<Color> pixelData(width * height);
    std::fill(pixelData.begin(), pixelData.end(), Color{0, 0, 0, 0});

    Traverse(root, [&](const TexturePackNode& node) {
        const auto image = node.image;
        auto & region = regions[indices[image]].Region;
        region.Subrect.X = static_cast<int>(node.rect.X);
        region.Subrect.Y = static_cast<int>(node.rect.Y);
        region.Subrect.Width = node.clipBounds.Width;
        region.Subrect.Height = node.clipBounds.Height;

        static_assert(std::is_same<decltype(node.clipBounds.X), std::int32_t>::value, "");
        static_assert(std::is_same<decltype(node.clipBounds.Y), std::int32_t>::value, "");
        static_assert(std::is_same<decltype(region.XOffset), std::int16_t>::value, "");
        static_assert(std::is_same<decltype(region.YOffset), std::int16_t>::value, "");
        static_assert(std::is_same<decltype(image->GetWidth()), int>::value, "");
        static_assert(std::is_same<decltype(image->GetHeight()), int>::value, "");
        static_assert(std::is_same<decltype(region.Width), std::int16_t>::value, "");
        static_assert(std::is_same<decltype(region.Height), std::int16_t>::value, "");
        POMDOG_ASSERT(node.clipBounds.X <= std::numeric_limits<std::int16_t>::max());
        POMDOG_ASSERT(node.clipBounds.Y <= std::numeric_limits<std::int16_t>::max());
        POMDOG_ASSERT(image->GetWidth() <= std::numeric_limits<std::int16_t>::max());
        POMDOG_ASSERT(image->GetHeight() <= std::numeric_limits<std::int16_t>::max());

        region.XOffset = static_cast<std::int16_t>(node.clipBounds.X);
        region.YOffset = static_cast<std::int16_t>(node.clipBounds.Y);
        region.Width = static_cast<std::int16_t>(image->GetWidth());
        region.Height = static_cast<std::int16_t>(image->GetHeight());

        const auto start = region.Subrect.X + region.Subrect.Y * width;
        for (int y = 0; y < region.Subrect.Height; ++y) {
            const auto offset = region.XOffset + ((region.YOffset + y) * image->GetWidth());
            static_assert(sizeof(decltype(*image->GetData())) == sizeof(Color), "");
            std::memcpy(
                pixelData.data() + start + (y * width),
                image->GetData() + offset,
                sizeof(Color) * region.Subrect.Width);
        }
    });

    result.Image = std::make_shared<Image>(width, height);
    result.Image->SetData(std::move(pixelData));

    return result;
}

} // namespace TexturePacker
} // namespace Pomdog

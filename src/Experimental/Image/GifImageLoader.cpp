// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/Image/GifImageLoader.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <gif_lib.h>
#include <cstring>
#include <functional>

namespace Pomdog {
namespace {

void DumpExtensions(
    int extensionBlockCount,
    ExtensionBlock* extensionBlocks,
    const std::function<void(int)>& findLoopCount)
{
    const auto extensionBlockEnd = extensionBlocks + extensionBlockCount;

    for (auto extensionBlock = extensionBlocks; extensionBlock != extensionBlockEnd; ++extensionBlock) {
        POMDOG_ASSERT(extensionBlock < extensionBlockEnd);
        if (extensionBlock->Function == APPLICATION_EXT_FUNC_CODE &&
            (std::memcmp(extensionBlock->Bytes, "NETSCAPE2.0", 11) == 0) &&
            (extensionBlock->Bytes[10] == 0x01)) {

            ++extensionBlock;
            GifByteType x1 = extensionBlock->Bytes[1];
            GifByteType x2 = extensionBlock->Bytes[2];
            int loopCount = (static_cast<int>(x2) << 8) | static_cast<int>(x1);
            findLoopCount(loopCount);
        }
    }
}

} // unnamed namespace

std::optional<GifImage> GifLoader::Open(const std::string& filePath)
{
    if (filePath.empty()) {
        // error
        return std::nullopt;
    }

    int gifError = 0;
    auto gifFileIn = std::shared_ptr<GifFileType>(
        DGifOpenFileName(filePath.c_str(), &gifError),
        [](GifFileType* p) {
            if (p != nullptr) {
                DGifCloseFile(p, nullptr);
            }
        });

    if (gifError != 0) {
        //throw CannotOpenGifFileException{};
        return std::nullopt;
    }

    if (DGifSlurp(gifFileIn.get()) == GIF_ERROR) {
        //throw CannotOpenGifFileException{};
        return std::nullopt;
    }

    GifImage result;
    result.LoopCount = 0;

    DumpExtensions(
        gifFileIn->ExtensionBlockCount,
        gifFileIn->ExtensionBlocks,
        [&](int loopCountIn) {
            result.LoopCount = loopCountIn;
        });

    std::shared_ptr<Pomdog::Image const> prevImage;

    for (int index = 0; index < gifFileIn->ImageCount; ++index) {
        auto& saveImage = gifFileIn->SavedImages[index];

        const int imageNumber = ((index + 1) % gifFileIn->ImageCount);
        GraphicsControlBlock gcb;
        bool hasGCB = false;
        if (DGifSavedExtensionToGCB(gifFileIn.get(), imageNumber, &gcb) == GIF_OK) {
            hasGCB = true;
        }
        else {
            hasGCB = false;
        }

        const auto colorMap = saveImage.ImageDesc.ColorMap
            ? saveImage.ImageDesc.ColorMap
            : gifFileIn->SColorMap;

        auto img = std::make_shared<Pomdog::Image>(gifFileIn->SWidth, gifFileIn->SHeight);

        if (hasGCB && gcb.DisposalMode == DISPOSE_BACKGROUND) {
            auto fillColor = [&]() -> Color {
                if (hasGCB && (gcb.TransparentColor != NO_TRANSPARENT_COLOR)) {
                    GifColorType& color = colorMap->Colors[gcb.TransparentColor];
                    return Color{color.Red, color.Green, color.Blue, 0};
                }
                return Color{255, 255, 255, 255};
            }();
            img->Fill(fillColor);
        }
        else if (hasGCB && gcb.DisposalMode == DISPOSE_DO_NOT) {
            if (index > 0) {
                // Fill color
                POMDOG_ASSERT(prevImage);
                POMDOG_ASSERT(img->GetWidth() == prevImage->GetWidth());
                POMDOG_ASSERT(img->GetHeight() == prevImage->GetHeight());
                img->SetData(prevImage->GetData());
            }
        }

        const int width = saveImage.ImageDesc.Width;
        const int height = saveImage.ImageDesc.Height;

        for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height; ++y) {
                int colorIndex = saveImage.RasterBits[x + width * y];
                assert(colorIndex < colorMap->ColorCount);
                GifColorType& colorFromMap =
                    colorMap->Colors[colorIndex];

                bool isTransparent = [&]{
                    if (!hasGCB) {
                        return false;
                    }
                    return gcb.TransparentColor == colorIndex;
                }();

                GifByteType alpha = isTransparent ? 0x00 : 0xFF;

                auto rgba = Color{
                    colorFromMap.Red,
                    colorFromMap.Green,
                    colorFromMap.Blue,
                    alpha
                };

                if (!isTransparent) {
                    img->SetPixel(
                        saveImage.ImageDesc.Left + x,
                        saveImage.ImageDesc.Top + y,
                        rgba);
                }
            }
        }

        DumpExtensions(
            gifFileIn->ExtensionBlockCount,
            gifFileIn->ExtensionBlocks,
            [&](int loopCountIn) {
                result.LoopCount = loopCountIn;
            });

        const GifDuration sourceDelay{hasGCB ? gcb.DelayTime : 0};

        GifImageFrame frame;
        frame.Image = img;
        frame.Delay = sourceDelay;
        result.Frames.push_back(std::move(frame));

        prevImage = img;
    }

    return result;
}

} // namespace Pomdog

// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/Image/GIFLoader.hpp"
#include "Pomdog/Utility/Assert.hpp"
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#include <gif_lib.h>
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#include <cstring>
#include <functional>

namespace Pomdog::GIF {
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

} // namespace

std::tuple<GIFImage, std::shared_ptr<Error>>
DecodeFile(const std::string& filePath)
{
    GIFImage result;
    result.LoopCount = 0;

    if (filePath.empty()) {
        auto err = Errors::New("file path is empty");
        return std::make_tuple(std::move(result), std::move(err));
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
        auto err = Errors::New("cannot read the gif file, " + filePath);
        return std::make_tuple(std::move(result), std::move(err));
    }

    if (DGifSlurp(gifFileIn.get()) == GIF_ERROR) {
        auto err = Errors::New("invalid gif format, " + filePath);
        return std::make_tuple(std::move(result), std::move(err));
    }

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

                bool isTransparent = [&] {
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

        const GIFDuration sourceDelay{hasGCB ? gcb.DelayTime : 0};

        GIFImageFrame frame;
        frame.Image = img;
        frame.Delay = sourceDelay;
        result.Frames.push_back(std::move(frame));

        prevImage = img;
    }

    return std::make_tuple(std::move(result), nullptr);
}

} // namespace Pomdog::GIF

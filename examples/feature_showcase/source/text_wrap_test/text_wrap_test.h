#pragma once

#include "pomdog/experimental/i18n/line_break_language.h"
#include "pomdog/graphics/primitive_batch.h"
#include "pomdog/graphics/sprite_batch.h"
#include "pomdog/graphics/sprite_font.h"
#include "pomdog/graphics/sprite_font_sdf.h"
#include "pomdog/graphics/truetype_font.h"
#include "pomdog/pomdog.h"
#include "pomdog/vfs/file_system.h"

namespace feature_showcase {

using namespace pomdog;

class TextWrapTest final : public Game {
public:
    explicit TextWrapTest(const std::shared_ptr<GameHost>& gameHost, const std::shared_ptr<vfs::FileSystemContext>& fs);

    [[nodiscard]] std::unique_ptr<Error>
    initialize(const std::shared_ptr<GameHost>& gameHost) override;

    void update() override;

    void draw() override;

private:
    struct Sample final {
        std::string label;
        std::string text;
        i18n::LineBreakLanguage language = i18n::LineBreakLanguage::Default;
        std::shared_ptr<SpriteFont> font;
        std::string wrappedText;
    };

    void wrapSamples(f32 wrapWidth);

    std::shared_ptr<GameHost> gameHost_;
    std::shared_ptr<vfs::FileSystemContext> fs_;
    std::shared_ptr<gpu::GraphicsDevice> graphicsDevice_;
    std::shared_ptr<gpu::CommandQueue> commandQueue_;
    std::shared_ptr<gpu::CommandList> commandList_;
    std::shared_ptr<SpriteBatch> spriteBatch_;
    std::shared_ptr<SpritePipeline> spritePipeline_;
    std::shared_ptr<PrimitiveBatch> primitiveBatch_;
    std::shared_ptr<PrimitivePipeline> primitivePipeline_;
    std::shared_ptr<SpriteFont> hintFont_;
    std::vector<Sample> samples_;
    f32 wrapRatio_ = 0.7f;
    f32 wrappedWidth_ = 0.0f;
    bool isAnimating_ = true;
};

} // namespace feature_showcase

#include "devgui_test.h"
#include "pomdog/experimental/clipboard/platform_clipboard.h"
#include "pomdog/experimental/i18n/line_break_language.h"
#include "pomdog/experimental/text_input/platform_text_input.h"
#include "pomdog/utility/string_format.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <array>
#include <cmath>
#include <iterator>
#include <string_view>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {
namespace {

using namespace pomdog::devgui::literals;

using TextBuffer = std::array<char, 64>;

// NOTE: The default layout and the windows name the windows by these keys.
constexpr devgui::Key dockSpaceKey = "dock"_key;
constexpr devgui::Key emittersWindowKey = "emitters"_key;
constexpr devgui::Key languagesWindowKey = "languages"_key;
constexpr devgui::Key statsWindowKey = "stats"_key;
constexpr devgui::Key windowListKey = "windows"_key;
constexpr devgui::Key previewWindowKey = "preview"_key;

/// The right and bottom edges of the showcase's Back button, in logical
/// pixels. The showcase draws it over the top-left corner of the screen.
constexpr f32 backButtonRight = 100.0f;
constexpr f32 backButtonBottom = 40.0f;

/// Formats into a fixed buffer, so building a frame allocates no strings.
template <typename... Args>
[[nodiscard]] std::string_view
formatTo(TextBuffer& buffer, fmt::format_string<Args...> format, Args&&... args)
{
    const auto result = fmt::format_to_n(buffer.data(), buffer.size(), format, std::forward<Args>(args)...);
    return std::string_view{buffer.data(), std::min(result.size, buffer.size())};
}

[[nodiscard]] std::string_view
toString(textinput::FeatureSupport support) noexcept
{
    switch (support) {
    case textinput::FeatureSupport::NotImplemented:
        return "Not implemented";
    case textinput::FeatureSupport::Unsupported:
        return "Unsupported";
    case textinput::FeatureSupport::Unavailable:
        return "Unavailable";
    case textinput::FeatureSupport::Available:
        return "Available";
    }
    return "Unknown";
}

struct LanguageDesc final {
    std::string_view greeting;
    std::string_view rateTooltip;
    i18n::LineBreakLanguage lineBreak;
};

// NOTE: Language names are written in their own scripts, so the list itself
// shows that every script renders. They are display text; the list is
// identified by index.
constexpr std::array<std::string_view, 5> languageNames = {{
    "English",
    "日本語",
    "简体中文",
    "한국어",
    "ไทย",
}};

constexpr std::array<LanguageDesc, 5> languages = {{
    {
        "Hello, Pomdog! 🐶",
        "Particles emitted per second. Drag sideways to change the value, or hold Shift while dragging for finer steps.",
        i18n::LineBreakLanguage::Default,
    },
    {
        "こんにちは、ポムドッグ！🐶",
        "1秒あたりに放出するパーティクルの数です。左右にドラッグして値を変更します。Shift キーを押しながらドラッグすると細かく調整できます。",
        i18n::LineBreakLanguage::Japanese,
    },
    {
        "你好，Pomdog！🐶",
        "每秒发射的粒子数量。左右拖动可以修改数值，按住 Shift 键拖动可以进行微调。",
        i18n::LineBreakLanguage::Chinese,
    },
    {
        "안녕하세요, Pomdog! 🐶",
        "초당 방출하는 파티클 수입니다. 좌우로 드래그하여 값을 바꾸고, Shift 키를 누른 채 드래그하면 세밀하게 조정할 수 있습니다.",
        i18n::LineBreakLanguage::Korean,
    },
    {
        "สวัสดี Pomdog! 🐶",
        "จำนวนอนุภาคที่ปล่อยออกมาต่อวินาที ลากไปทางซ้ายหรือขวาเพื่อเปลี่ยนค่า กด Shift ค้างไว้ขณะลากเพื่อปรับอย่างละเอียด",
        i18n::LineBreakLanguage::Thai,
    },
}};

constexpr std::array<std::string_view, 5> shapeNames = {{
    "Box",
    "Cone",
    "Sphere",
    "Hemisphere",
    "Sector",
}};

/// Lists the modules that the search field suggests.
constexpr std::array<std::string_view, 12> moduleNames = {{
    "Color over Lifetime",
    "Color by Speed",
    "Size over Lifetime",
    "Size by Speed",
    "Rotation over Lifetime",
    "Velocity over Lifetime",
    "Limit Velocity",
    "Noise",
    "Collision",
    "Sub Emitters",
    "Texture Sheet Animation",
    "Trails",
}};

/// Returns true when text contains query, ignoring the case of ASCII letters.
///
/// NOTE: devgui leaves matching to the application. A game would choose rules
/// for its languages, such as matching kana and kanji readings.
[[nodiscard]] bool
containsIgnoringCase(std::string_view text, std::string_view query) noexcept
{
    const auto lower = [](char c) -> char {
        return ((c >= 'A') && (c <= 'Z')) ? static_cast<char>(c - 'A' + 'a') : c;
    };
    const auto iter = std::search(text.begin(), text.end(), query.begin(), query.end(), [&](char a, char b) { return lower(a) == lower(b); });
    return iter != text.end();
}

/// Displays a flat menu bar item, such as "View", highlighted while it is hot
/// or its menu is open.
///
/// NOTE: This is an application widget built from the same `UI` members as
/// the built-in widgets: `allocate`, `interact`, and the drawing functions.
[[nodiscard]] devgui::Response
menuBarItem(devgui::UI& ui, devgui::Key key, std::string_view label, bool open)
{
    const auto& style = ui.getStyle();
    const auto width = std::ceil(ui.measureText(label)) + style.framePadding.x * 2.0f;
    const auto rect = ui.allocate(width, style.getFrameHeight());
    const auto response = ui.interact(ui.makeID(key), rect);
    if (open || response.hovered || response.active) {
        ui.fillRect(rect, style.rowHoveredColor, style.frameRounding);
    }
    const auto textTop = rect.y + std::round((rect.height - style.fontSize) * 0.5f);
    ui.drawText(Vector2{rect.x + style.framePadding.x, textTop}, label, style.textColor);
    return response;
}

/// Lists the fonts of the fallback chain. Glyphs missing from one font are
/// looked up in the next.
constexpr std::array<std::string_view, 6> fontPaths = {{
    "/assets/fonts/NotoSans-Regular.ttf",
    "/assets/fonts/NotoSansJP-Regular.ttf",
    "/assets/fonts/NotoSansKR-Regular.ttf",
    "/assets/fonts/NotoSansSC-Regular.ttf",
    "/assets/fonts/NotoSansThai-Regular.ttf",
    "/assets/fonts/NotoEmoji-Medium.ttf",
}};

} // namespace

DevGUITest::DevGUITest(const std::shared_ptr<GameHost>& gameHostIn, const std::shared_ptr<vfs::FileSystemContext>& fs)
    : gameHost_(gameHostIn)
    , fs_(fs)
    , graphicsDevice_(gameHostIn->getGraphicsDevice())
    , commandQueue_(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error>
DevGUITest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/)
{
    if (auto [commandList, err] = graphicsDevice_->createCommandList(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }
    else {
        commandList_ = std::move(commandList);
    }

    // NOTE: Noto Sans covers Latin text. Japanese, Korean, Chinese, Thai, and
    // emoji fall back to the fonts after it, in this order.
    std::vector<std::shared_ptr<TrueTypeFont>> fonts;
    for (const auto path : fontPaths) {
        auto [font, fontErr] = loadTrueTypeFont(fs_, std::string{path});
        if (fontErr != nullptr) {
            return errors::wrap(std::move(fontErr), "failed to load a font file");
        }
        fonts.push_back(std::move(font));
    }

    // NOTE: The renderer scales the SDF glyphs from this raster size to the
    // style's font size.
    constexpr f32 fontRasterSize = 32.0f;
    constexpr bool useSDF = true;
    const std::vector<f32> fontSizes(fonts.size(), fontRasterSize);

    std::shared_ptr<SpriteFont> spriteFont;
    if (auto [p, err] = createSpriteFont(graphicsDevice_, fonts, fontSizes, fontRasterSize, useSDF); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create SpriteFont");
    }
    else {
        spriteFont = std::move(p);
    }

    if (auto [p, err] = devgui::createRenderer(fs_, graphicsDevice_, spriteFont, fontRasterSize); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create devgui renderer");
    }
    else {
        renderer_ = std::move(p);
    }

    context_ = std::make_unique<devgui::Context>(renderer_);
    buildDefaultLayout();

    if (auto [p, err] = textinput::createTextInputService(gameHost_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create text input service");
    }
    else {
        textInput_ = std::move(p);
    }

    // NOTE: Text fields copy to and paste from the platform's clipboard.
    if (auto [p, err] = clipboard::createClipboardService(gameHost_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create clipboard service");
    }
    else {
        clipboard_ = std::move(p);
    }
    context_->setClipboardService(clipboard_);

    message_ = "ポムドッグで遊ぼう 🎮";
    addEmitter();
    addEmitter();
    return nullptr;
}

void DevGUITest::update()
{
    const auto mouse = gameHost_->getMouse();
    if (mouse == nullptr) {
        return;
    }

    // NOTE: The back buffer size divided by the pixel ratio maps each logical
    // pixel to the same physical pixels, even when the client size in logical
    // pixels was rounded.
    const auto metrics = gameHost_->getWindow()->getDisplayMetrics();
    const auto viewportSize = Vector2{
        static_cast<f32>(metrics.backBufferWidth) / metrics.pixelRatio,
        static_cast<f32>(metrics.backBufferHeight) / metrics.pixelRatio,
    };

    // NOTE: Typed text and input method compositions reach devgui through the
    // text input service instead of the keyboard.
    textInput_->update();
    clipboard_->update();
    const auto input = devgui::makeInput(
        *mouse,
        gameHost_->getTouchscreen().get(),
        gameHost_->getKeyboard().get(),
        textInput_.get(),
        viewportSize,
        gameHost_->getClock()->getTotalGameTime().count(),
        uiScale_);

    // NOTE: Button and editing-key transitions build a frame even outside the
    // UI to record press/release history. Held game keys do not keep waking it.
    // Skipped frames keep drawing the last draw list.
    if (!context_->needsUpdate(input)) {
        ++skippedFrames_;
        updateTextInputFocus();
        return;
    }

    {
        devgui::UI ui{*context_, input};
        buildUI(ui);
    }
    updateTextInputFocus();

    // NOTE: The window keeps the requested cursor until the next request, so
    // skipped frames need none.
    gameHost_->getWindow()->requestMouseCursor(context_->getMouseCursor());

    // NOTE: Change the emitter list after the frame, not while the UI iterates it.
    if (removeRequest_) {
        const auto id = *removeRequest_;
        std::erase_if(emitters_, [id](const Emitter& emitter) { return emitter.id == id; });
        removeRequest_ = std::nullopt;
        context_->invalidate();
    }
    if (duplicateRequest_) {
        const auto id = *duplicateRequest_;
        const auto iter = std::find_if(emitters_.begin(), emitters_.end(), [id](const Emitter& emitter) { return emitter.id == id; });
        if (iter != emitters_.end()) {
            auto copy = *iter;
            copy.id = nextEmitterID_;
            copy.name = pomdog::format("{} Copy", iter->name);
            ++nextEmitterID_;
            emitters_.insert(std::next(iter), std::move(copy));
        }
        duplicateRequest_ = std::nullopt;
        context_->invalidate();
    }

    // NOTE: A layout loads between frames.
    switch (layoutRequest_) {
    case LayoutRequest::None:
        break;
    case LayoutRequest::Restore:
        if (auto err = context_->loadLayout(savedLayout_); err != nullptr) {
            Log::Warning("feature_showcase", "failed to restore the layout: " + err->toString());
        }
        break;
    case LayoutRequest::Reset:
        buildDefaultLayout();
        break;
    }
    layoutRequest_ = LayoutRequest::None;
}

void DevGUITest::updateTextInputFocus()
{
    // NOTE: devgui reports the caret in its own pixels, which the UI scale
    // enlarges to the window's logical pixels.
    if (auto field = context_->getFocusedTextField(); field) {
        const auto& caret = field->caretRect;
        field->caretRect = F32Rect2D{caret.x * uiScale_, caret.y * uiScale_, caret.width * uiScale_, caret.height * uiScale_};
        textInput_->setFocusedField(*field);
    }
    else {
        textInput_->clearFocusedField();
    }
}

void DevGUITest::buildUI(devgui::UI& ui)
{
    // NOTE: Like an editor, a menu bar spans the top of the screen and the dock
    // space fills the rest. The bar covers the showcase's Back button, which
    // is sized in logical pixels, so it shrinks in devgui pixels as the UI
    // scale grows.
    const auto& style = ui.getStyle();
    const auto& viewport = ui.getInput().viewportSize;
    const auto barHeight = std::max(style.getFrameHeight() + style.windowPadding.y * 2.0f, backButtonBottom / uiScale_);
    ui.fillRect(F32Rect2D{0.0f, 0.0f, viewport.x, barHeight}, style.titleBarColor);
    ui.fillRect(F32Rect2D{0.0f, barHeight - 1.0f, viewport.x, 1.0f}, style.borderColor);

    constexpr std::string_view title = "Particle Editor";
    const auto titleWidth = ui.measureText(title);
    ui.drawText(Vector2{std::round((viewport.x - titleWidth) * 0.5f), std::round((barHeight - style.fontSize) * 0.5f)}, title, style.textSecondaryColor);

    const auto menuLeft = backButtonRight / uiScale_ - style.windowPadding.x;
    ui.indent(menuLeft);
    buildMenuBar(ui);
    ui.indent(-menuLeft);

    const auto dockOptions = devgui::DockSpaceOptions{.showTabBars = showTabBars_};
    devgui::dockSpace(ui, dockSpaceKey, F32Rect2D{0.0f, barHeight, viewport.x, std::max(0.0f, viewport.y - barHeight)}, dockOptions);

    buildWindowList(ui);
    buildPreviewWindow(ui);
    buildEmitterWindow(ui);
    if (showLanguages_) {
        buildLanguageWindow(ui);
    }
    if (showStats_) {
        buildStatsWindow(ui);
    }
}

void DevGUITest::buildMenuBar(devgui::UI& ui)
{
    const auto menuID = ui.makeID("view.menu"_key);
    const auto view = menuBarItem(ui, "view"_key, "View", ui.isPopupOpen(menuID));
    if (view.clicked) {
        ui.openPopup(menuID);
    }
    if (auto menu = devgui::popupMenu(ui, menuID, view.rect); menu.isOpen()) {
        const auto languagesOptions = devgui::MenuItemOptions{.shortcut = {}, .checked = showLanguages_, .enabled = true};
        if (devgui::menuItem(ui, "languages"_key, "Languages", languagesOptions).clicked) {
            showLanguages_ = !showLanguages_;
        }
        const auto statsOptions = devgui::MenuItemOptions{.shortcut = {}, .checked = showStats_, .enabled = true};
        if (devgui::menuItem(ui, "stats"_key, "Stats", statsOptions).clicked) {
            showStats_ = !showStats_;
        }
        const auto tabBarOptions = devgui::MenuItemOptions{.shortcut = {}, .checked = showTabBars_, .enabled = true};
        if (devgui::menuItem(ui, "tabBars"_key, "Tab bars", tabBarOptions).clicked) {
            showTabBars_ = !showTabBars_;
        }
        if (auto scales = devgui::subMenu(ui, "uiScale"_key, "UI scale"); scales.isOpen()) {
            // NOTE: The UI scale enlarges devgui on top of the display's own
            // scaling, for example on a 4K monitor set to 100%.
            constexpr std::array<std::pair<f32, std::string_view>, 4> choices = {{
                {1.0f, "100%"},
                {1.25f, "125%"},
                {1.5f, "150%"},
                {2.0f, "200%"},
            }};
            for (std::size_t i = 0; i < choices.size(); ++i) {
                const auto [scale, label] = choices[i];
                const auto scaleOptions = devgui::MenuItemOptions{.shortcut = {}, .checked = (uiScale_ == scale), .enabled = true};
                if (devgui::menuItem(ui, "scale"_key.at(static_cast<u32>(i)), label, scaleOptions).clicked) {
                    uiScale_ = scale;
                }
            }
        }
        devgui::menuSeparator(ui);
        if (devgui::menuItem(ui, "save"_key, "Save layout").clicked) {
            savedLayout_ = context_->saveLayout();
        }
        const auto restoreOptions = devgui::MenuItemOptions{.shortcut = {}, .checked = false, .enabled = !savedLayout_.empty()};
        if (devgui::menuItem(ui, "restore"_key, "Restore saved layout", restoreOptions).clicked) {
            layoutRequest_ = LayoutRequest::Restore;
        }
        if (devgui::menuItem(ui, "reset"_key, "Reset layout").clicked) {
            layoutRequest_ = LayoutRequest::Reset;
        }
    }
}

void DevGUITest::buildEmitterContextMenu(devgui::UI& ui, const devgui::Response& response, Emitter& emitter)
{
    auto menu = devgui::contextMenu(ui, response);
    if (!menu.isOpen()) {
        return;
    }
    if (devgui::menuItem(ui, "duplicate"_key, "Duplicate").clicked) {
        duplicateRequest_ = emitter.id;
    }
    if (auto shapes = devgui::subMenu(ui, "shape"_key, "Shape"); shapes.isOpen()) {
        for (std::size_t i = 0; i < shapeNames.size(); ++i) {
            const auto index = static_cast<i32>(i);
            const auto options = devgui::MenuItemOptions{.shortcut = {}, .checked = (emitter.shapeIndex == index), .enabled = true};
            if (devgui::menuItem(ui, "shapeItem"_key.at(static_cast<u32>(i)), shapeNames[i], options).clicked) {
                emitter.shapeIndex = index;
            }
        }
    }
    const auto loopingOptions = devgui::MenuItemOptions{.shortcut = {}, .checked = emitter.looping, .enabled = true};
    if (devgui::menuItem(ui, "looping"_key, "Looping", loopingOptions).clicked) {
        emitter.looping = !emitter.looping;
    }
    devgui::menuSeparator(ui);
    const auto removeOptions = devgui::MenuItemOptions{.shortcut = {}, .checked = false, .enabled = (emitters_.size() > 1)};
    if (devgui::menuItem(ui, "remove"_key, "Remove", removeOptions).clicked) {
        removeRequest_ = emitter.id;
    }
}

void DevGUITest::buildEmitterWindow(devgui::UI& ui)
{
    const auto options = devgui::WindowOptions{
        .position = Vector2{16.0f, 48.0f},
        .size = Vector2{392.0f, 420.0f},
        .collapsed = false,
    };
    auto window = devgui::window(ui, emittersWindowKey, "Particle Emitters", options);
    if (!window.isOpen()) {
        return;
    }

    {
        auto row = devgui::horizontal(ui);
        const auto add = devgui::button(ui, "add"_key, "Add emitter");
        if (add.clicked) {
            addEmitter();
        }
        devgui::tooltip(ui, add, "Adds an emitter at the end of the list. Right-click an emitter for more actions.");
        const auto aa = devgui::checkbox(ui, "antialiasing"_key, "Contour AA", antialiasing_);
        devgui::tooltip(ui, aa, "Smooths shape outlines with a one-physical-pixel alpha fringe. Toggle to compare the check marks, rounded corners, and slider knobs.");
    }

    // NOTE: The application picks the suggestions for the current text.
    moduleMatches_.clear();
    if (!moduleQuery_.empty()) {
        for (const auto name : moduleNames) {
            if (containsIgnoringCase(name, moduleQuery_)) {
                moduleMatches_.push_back(name);
            }
        }
    }
    const auto search = devgui::inputTextWithSuggestions(
        ui, "module"_key, "Add module", moduleQuery_, moduleMatches_, devgui::InputTextOptions{.placeholder = "Search modules", .purpose = textinput::TextInputPurpose::Search});
    if (search.chosen) {
        addedModule_ = moduleMatches_[*search.chosen];
    }
    if (!addedModule_.empty()) {
        devgui::labelText(ui, "Last added", addedModule_);
    }
    devgui::separator(ui);

    const auto& language = languages[static_cast<std::size_t>(languageIndex_)];

    // NOTE: Each emitter's stable ID becomes the key index, so removing an
    // emitter does not move the open state of the others.
    for (auto& emitter : emitters_) {
        const auto nodeOptions = devgui::TreeNodeOptions{.defaultOpen = true};
        auto node = devgui::treeNode(ui, "emitter"_key.at(emitter.id), emitter.name, nodeOptions);
        buildEmitterContextMenu(ui, node.getResponse(), emitter);
        if (!node.isOpen()) {
            continue;
        }
        devgui::inputText(ui, "name"_key, "Name", emitter.name, devgui::InputTextOptions{.placeholder = "Untitled", .purpose = textinput::TextInputPurpose::Text});
        devgui::combo(ui, "shape"_key, "Shape", emitter.shapeIndex, shapeNames);
        devgui::colorEdit(ui, "color"_key, "Start color", emitter.startColor);

        const auto rate = devgui::dragFloat(ui, "rate"_key, "Emission rate", emitter.emissionRate, 0.5f, 0.0f, 1000.0f);
        devgui::tooltip(ui, rate, language.rateTooltip, language.lineBreak);

        devgui::sliderFloat(ui, "speed"_key, "Start speed", emitter.startSpeed, 0.0f, 500.0f);
        devgui::sliderFloat(ui, "lifetime"_key, "Start lifetime", emitter.startLifetime, 0.1f, 10.0f);
        devgui::dragFloat(ui, "gravity"_key, "Gravity", emitter.gravityModifier, 0.01f);
        devgui::dragInt(ui, "max"_key, "Max particles", emitter.maxParticles, 1.0f, 1, 4096);

        const auto curveOptions = devgui::CurveEditorOptions{.minY = 0.0f, .maxY = 2.0f, .height = 96.0f};
        const auto curve = devgui::curveEditor(ui, "size"_key, "Size over lifetime", emitter.sizeOverLifetime, curveOptions);
        devgui::tooltip(ui, curve, "Drag a point to move it. Double-click empty space to add a point, or a point to remove it.");

        devgui::checkbox(ui, "looping"_key, "Looping", emitter.looping);
        if (devgui::button(ui, "remove"_key, "Remove").clicked) {
            removeRequest_ = emitter.id;
        }
    }
}

void DevGUITest::buildWindowList(devgui::UI& ui)
{
    const auto options = devgui::WindowOptions{
        .position = Vector2{420.0f, 480.0f},
        .size = Vector2{240.0f, 160.0f},
        .collapsed = false,
    };
    auto window = devgui::window(ui, windowListKey, "Open Windows", options);
    if (!window.isOpen()) {
        return;
    }

    // NOTE: The list shows the dock space's tabs, group by group, so that it
    // can replace the tab bars. Selecting, closing, and reordering a row act
    // on the tab.
    context_->getDockedWindows(dockSpaceKey, dockedWindows_);
    const auto titleOf = [](devgui::Key key) -> std::string_view {
        if (key == emittersWindowKey) {
            return "Particle Emitters";
        }
        if (key == languagesWindowKey) {
            return "Languages";
        }
        if (key == statsWindowKey) {
            return "Stats";
        }
        if (key == previewWindowKey) {
            return "Preview";
        }
        return "Open Windows";
    };

    constexpr f32 closeWidth = 20.0f;
    auto list = devgui::listView(ui, "list"_key);
    for (std::size_t i = 0; i < dockedWindows_.size(); ++i) {
        const auto& docked = dockedWindows_[i];
        if ((i > 0) && !(dockedWindows_[i - 1].node == docked.node)) {
            devgui::separator(ui);
        }
        bool* open = nullptr;
        if (docked.window == languagesWindowKey) {
            open = &showLanguages_;
        }
        else if (docked.window == statsWindowKey) {
            open = &showStats_;
        }

        // NOTE: Every group has a selected window. Highlight it only in groups
        // that hold more than one, where the choice means something.
        const auto inGroup = [&](const devgui::DockedWindow& other) { return other.node == docked.node; };
        const bool shared = std::count_if(dockedWindows_.begin(), dockedWindows_.end(), inGroup) > 1;
        const auto itemOptions = devgui::ListItemOptions{.selected = docked.selected && shared, .trailingWidth = (open != nullptr) ? closeWidth : 0.0f};
        const auto row = list.item("window"_key.at(docked.window.name), titleOf(docked.window), itemOptions);
        if (row.activated) {
            context_->selectWindow(docked.window);
        }
        if ((open != nullptr) && (row.hovered || itemOptions.selected)) {
            const auto closeRect = F32Rect2D{row.rect.x + row.rect.width - closeWidth, row.rect.y, closeWidth, row.rect.height};
            if (devgui::closeButton(ui, "close"_key.at(docked.window.name), closeRect).clicked) {
                *open = false;
            }
        }
        devgui::tooltip(ui, row, "Click to show this window. Drag to reorder the tabs of its group.");
    }
    if (const auto move = list.getMove(); move) {
        const auto& from = dockedWindows_[static_cast<std::size_t>(move->from)];
        const auto& to = dockedWindows_[static_cast<std::size_t>(move->to)];
        context_->moveWindowTab(from.window, to.window);
    }
}

void DevGUITest::buildPreviewWindow(devgui::UI& ui)
{
    const auto options = devgui::WindowOptions{
        .position = Vector2{200.0f, 80.0f},
        .size = Vector2{360.0f, 300.0f},
        .collapsed = false,
    };
    auto window = devgui::window(ui, previewWindowKey, "Preview", options);
    if (!window.isOpen()) {
        return;
    }

    // NOTE: The preview keeps a 5:3 area, which scrolls in a short window.
    const auto& style = ui.getStyle();
    const auto area = ui.allocate(0.0f, std::round(ui.getAvailableWidth() * 0.6f));
    ui.fillRect(area, getBackgroundColor(), style.frameRounding);

    const auto count = static_cast<f32>(emitters_.size());
    const auto radius = std::min(area.width / (count + 1.0f) * 0.45f, area.height * 0.32f);
    for (std::size_t i = 0; i < emitters_.size(); ++i) {
        const auto& emitter = emitters_[i];
        const auto center = Vector2{
            std::round(area.x + area.width * (static_cast<f32>(i) + 1.0f) / (count + 1.0f)),
            std::round(area.y + area.height * 0.45f),
        };
        drawEmitterPreview(ui, emitter, center, radius);

        const auto nameWidth = ui.measureText(emitter.name);
        ui.drawText(Vector2{std::round(center.x - nameWidth * 0.5f), std::round(center.y + radius + style.itemSpacing.y)}, emitter.name, style.textColor);
    }
}

void DevGUITest::drawEmitterPreview(devgui::UI& ui, const Emitter& emitter, const Vector2& center, f32 radius) const
{
    // NOTE: A still picture asks for no frames, so the preview costs nothing
    // while nothing changes. A game would draw its particle system here.
    auto peakSize = 0.0f;
    for (const auto& point : emitter.sizeOverLifetime) {
        peakSize = std::max(peakSize, point.y);
    }
    const auto dotRadius = std::max(1.5f, radius * 0.06f * peakSize);
    const auto spread = radius * std::clamp(emitter.startSpeed / 250.0f, 0.3f, 1.0f);
    const auto count = std::clamp(emitter.maxParticles / 8, 8, 32);
    const auto side = static_cast<i32>(std::ceil(std::sqrt(static_cast<f32>(count))));

    for (i32 i = 0; i < count; ++i) {
        const auto t = (static_cast<f32>(i) + 0.5f) / static_cast<f32>(count);

        // NOTE: Steps of the golden ratio spread the distances evenly without
        // randomness, so the picture stays the same from frame to frame.
        const auto distance = 0.35f + 0.65f * std::fmod(static_cast<f32>(i) * 0.618034f, 1.0f);
        const auto direction = [&](f32 angle) { return Vector2{std::cos(angle), std::sin(angle)} * (spread * distance); };

        // NOTE: Screen y points down, so angles toward -y point up.
        auto offset = Vector2{0.0f, 0.0f};
        switch (emitter.shapeIndex) {
        case 0: {
            const auto column = static_cast<f32>(i % side);
            const auto row = static_cast<f32>(i / side);
            const auto cell = 2.0f / static_cast<f32>(side);
            offset = Vector2{(column + 0.5f) * cell - 1.0f, (row + 0.5f) * cell - 1.0f} * spread;
            break;
        }
        case 1:
            offset = direction(-math::PiOver2<f32> + (t - 0.5f) * math::Pi<f32> * 0.6f);
            break;
        case 3:
            offset = direction(-math::Pi<f32> + t * math::Pi<f32>);
            break;
        case 4:
            offset = direction(-math::PiOver2<f32> + t * math::PiOver2<f32>);
            break;
        default:
            offset = direction(t * math::TwoPi<f32>);
            break;
        }
        offset.y += emitter.gravityModifier * distance * spread * 0.5f;

        auto color = emitter.startColor;
        color.a = static_cast<u8>(static_cast<f32>(color.a) * (1.0f - 0.5f * distance));
        ui.fillCircle(center + offset, dotRadius * (1.0f - 0.4f * distance), color);
    }
}

void DevGUITest::buildLanguageWindow(devgui::UI& ui)
{
    const auto options = devgui::WindowOptions{
        .position = Vector2{420.0f, 48.0f},
        .size = Vector2{364.0f, 236.0f},
        .collapsed = false,
    };
    auto window = devgui::window(ui, languagesWindowKey, "Languages", showLanguages_, options);
    if (!window.isOpen()) {
        return;
    }

    const auto languageResponse = devgui::combo(ui, "language"_key, "Language", languageIndex_, languageNames);
    devgui::tooltip(ui, languageResponse, "Changes the language of the Emission rate tooltip, which wraps by that language's rules.");
    devgui::inputText(ui, "message"_key, "Message", message_, devgui::InputTextOptions{.placeholder = "Type here", .purpose = textinput::TextInputPurpose::Text});

    // NOTE: The purpose turns the input method editor off for these fields.
    // The password shows as dots, and the seed accepts only digits and signs.
    devgui::inputText(ui, "password"_key, "Password", password_, devgui::InputTextOptions{.placeholder = "Hidden", .purpose = textinput::TextInputPurpose::Password});
    devgui::inputText(ui, "seed"_key, "Seed", seed_, devgui::InputTextOptions{.placeholder = "Integer", .purpose = textinput::TextInputPurpose::Integer});
    devgui::separator(ui);

    for (std::size_t i = 0; i < languages.size(); ++i) {
        devgui::labelText(ui, languageNames[i], languages[i].greeting);
    }
}

void DevGUITest::buildStatsWindow(devgui::UI& ui)
{
    const auto options = devgui::WindowOptions{
        .position = Vector2{420.0f, 296.0f},
        .size = Vector2{364.0f, 172.0f},
        .collapsed = false,
    };
    auto window = devgui::window(ui, statsWindowKey, "Stats", showStats_, options);
    if (!window.isOpen()) {
        return;
    }

    TextBuffer buffer;
    devgui::labelText(ui, "Built frames", formatTo(buffer, "{}", context_->getFrameCount()));
    devgui::labelText(ui, "Skipped frames", formatTo(buffer, "{}", skippedFrames_));
    devgui::labelText(ui, "Emitters", formatTo(buffer, "{}", emitters_.size()));
    devgui::separator(ui);

    devgui::text(ui, "Preview background");
    devgui::sliderInt(ui, "red"_key, "Red", backgroundRed_, 0, 255);
    devgui::sliderInt(ui, "green"_key, "Green", backgroundGreen_, 0, 255);
    devgui::sliderInt(ui, "blue"_key, "Blue", backgroundBlue_, 0, 255);
    devgui::separator(ui);

    // NOTE: Platforms differ in what text input offers. A game would hide what
    // is not Available, such as a button that shows the software keyboard.
    devgui::text(ui, "Text input");
    devgui::labelText(ui, "Shortcut modifier", ui.getInput().shortcutUsesSuper ? "Command" : "Control");
    const auto capabilities = textInput_->getCapabilities();
    devgui::labelText(ui, "Committed text", toString(capabilities.committedText));
    devgui::labelText(ui, "Composition", toString(capabilities.composition));
    devgui::labelText(ui, "Candidate window", toString(capabilities.candidateWindowPosition));
    devgui::labelText(ui, "Surrounding text", toString(capabilities.surroundingText));
    devgui::labelText(ui, "Software keyboard", toString(capabilities.softwareKeyboard));
    devgui::separator(ui);

    devgui::text(ui, "Log");
    auto area = devgui::scrollArea(ui, "log"_key, 96.0f);
    for (u32 i = 0; i < 50; ++i) {
        devgui::text(ui, formatTo(buffer, "Message {}", i));
    }
}

void DevGUITest::addEmitter()
{
    // NOTE: New emitters take turns through a few colors and shapes, so that
    // the preview tells them apart.
    const std::array<Color, 4> startColors = {{
        Color{255, 196, 64, 255},
        Color{80, 180, 255, 255},
        Color{255, 110, 140, 255},
        Color{120, 230, 160, 255},
    }};
    const auto turn = static_cast<std::size_t>(nextEmitterID_ - 1);

    Emitter emitter;
    emitter.id = nextEmitterID_;
    emitter.name = pomdog::format("Emitter {}", nextEmitterID_);
    emitter.shapeIndex = static_cast<i32>(turn % shapeNames.size());
    emitter.startColor = startColors[turn % startColors.size()];
    emitter.sizeOverLifetime = {{0.0f, 1.0f}, {0.6f, 1.4f}, {1.0f, 0.0f}};
    emitters_.push_back(std::move(emitter));
    ++nextEmitterID_;
}

void DevGUITest::buildDefaultLayout()
{
    // NOTE: An editor's layout: the window list on the left, the preview in
    // the middle above a panel with the stats, and the emitter inspector on
    // the right, where the languages share its node as a tab.
    devgui::DockLayoutBuilder layout{*context_, dockSpaceKey};
    const auto [windowList, rest] = layout.split(layout.root(), devgui::DockSplit::LeftRight, 0.2f);
    const auto [center, inspector] = layout.split(rest, devgui::DockSplit::LeftRight, 0.5f);
    const auto [preview, panel] = layout.split(center, devgui::DockSplit::TopBottom, 0.6f);
    layout.dock(windowList, windowListKey);
    layout.dock(preview, previewWindowKey);
    layout.dock(panel, statsWindowKey);
    layout.dock(inspector, emittersWindowKey);
    layout.dock(inspector, languagesWindowKey);
}

Color DevGUITest::getBackgroundColor() const noexcept
{
    return Color{
        static_cast<u8>(backgroundRed_),
        static_cast<u8>(backgroundGreen_),
        static_cast<u8>(backgroundBlue_),
        255,
    };
}

void DevGUITest::draw()
{
    const auto presentationParameters = graphicsDevice_->getPresentationParameters();
    const auto window = gameHost_->getWindow();

    const auto backgroundColor = getBackgroundColor();

    gpu::Viewport viewport = {0, 0, presentationParameters.backBufferWidth, presentationParameters.backBufferHeight};
    gpu::RenderPass pass;
    pass.renderTargets[0] = {nullptr, backgroundColor.toVector4()};
    pass.depthStencilBuffer = nullptr;
    pass.clearDepth = 1.0f;
    pass.clearStencil = u8(0);
    pass.viewport = viewport;
    pass.scissorRect = viewport.getBounds();

    commandList_->reset();
    commandList_->beginRenderPass(std::move(pass));
    renderer_->draw(commandList_, context_->getDrawList(), window->getPixelRatio() * uiScale_, antialiasing_ ? 1.0f : 0.0f);
    commandList_->endRenderPass();
    commandList_->close();

    commandQueue_->pushBackCommandList(commandList_);
}

} // namespace feature_showcase

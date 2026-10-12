// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/devgui/dock_space.h"
#include "pomdog/experimental/devgui/context.h"
#include "pomdog/experimental/devgui/style.h"
#include "pomdog/experimental/devgui/ui.h"
#include "pomdog/experimental/devgui/widget_helpers.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <charconv>
#include <cmath>
#include <iterator>
#include <string>
#include <system_error>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::devgui {
namespace {

using namespace pomdog::devgui::literals;

constexpr Key splitterKey = "devgui.dock.splitter"_key;
constexpr Key tabKey = "devgui.dock.tab"_key;
constexpr Key tabCloseKey = "devgui.dock.tab.close"_key;

/// The gap between two dock nodes.
constexpr f32 splitterSize = 2.0f;

/// How far a splitter's grab area reaches into the nodes beside it.
constexpr f32 splitterGrabMargin = 4.0f;

/// The smallest size a split leaves either of its children when there is room.
constexpr f32 minNodeSize = 48.0f;

constexpr f32 minTabWidth = 40.0f;
constexpr f32 maxTabWidth = 220.0f;

/// How far the pointer drags a tab away from its tab bar before the window
/// comes out as a floating window.
constexpr f32 undockDistance = 16.0f;

/// The part of a node near each edge where a dropped window splits the node
/// instead of joining its tabs.
constexpr f32 dropEdgeRatio = 0.25f;

/// The smallest and largest share of a node that a dropped window splits off.
constexpr f32 minDropRatio = 0.2f;
constexpr f32 maxDropRatio = 0.5f;

/// The first line of a saved layout, which names its format version.
constexpr std::string_view layoutHeader = "devgui-layout 1";

/// The limits that keep a malformed layout from exhausting memory or stack.
constexpr i32 maxLayoutDepth = 32;
constexpr std::size_t maxLayoutNodes = 1024;
constexpr u32 maxLayoutTabs = 1024;

/// The ratio of a split as written in a saved layout, in parts per ten thousand.
constexpr f32 ratioScale = 10000.0f;

/// Selects where a window dropped on a dock node goes.
enum class DropZone : u8 {
    Center,
    Left,
    Right,
    Top,
    Bottom,
};

[[nodiscard]] detail::DockNode&
nodeAt(detail::DockSpaceState& space, i32 index) noexcept
{
    POMDOG_ASSERT((index >= 0) && (static_cast<std::size_t>(index) < space.nodes.size()));
    return space.nodes[static_cast<std::size_t>(index)];
}

[[nodiscard]] const detail::DockNode&
nodeAt(const detail::DockSpaceState& space, i32 index) noexcept
{
    POMDOG_ASSERT((index >= 0) && (static_cast<std::size_t>(index) < space.nodes.size()));
    return space.nodes[static_cast<std::size_t>(index)];
}

[[nodiscard]] F32Rect2D
inflate(const F32Rect2D& rect, f32 amount) noexcept
{
    return F32Rect2D{rect.x - amount, rect.y - amount, rect.width + amount * 2.0f, rect.height + amount * 2.0f};
}

[[nodiscard]] Color
withAlpha(Color color, f32 factor) noexcept
{
    color.a = static_cast<u8>(std::clamp(static_cast<f32>(color.a) * factor, 0.0f, 255.0f));
    return color;
}

[[nodiscard]] std::size_t
findWindowIndex(const std::vector<detail::WindowState>& windows, WidgetID id) noexcept
{
    const auto iter = std::find_if(windows.begin(), windows.end(), [&](const detail::WindowState& w) { return w.id == id; });
    return static_cast<std::size_t>(std::distance(windows.begin(), iter));
}

/// Returns the window named key, creating one without an area if it has not
/// appeared yet.
detail::WindowState&
ensureWindow(std::vector<detail::WindowState>& windows, u64& depthCounter, Key key)
{
    const auto id = combineID(detail::rootID, key);
    if (const auto index = findWindowIndex(windows, id); index < windows.size()) {
        return windows[index];
    }
    depthCounter += 1;
    detail::WindowState state;
    state.id = id;
    state.key = key;
    state.depth = depthCounter;
    state.hasRect = false;
    windows.push_back(std::move(state));
    return windows.back();
}

void resetNodes(detail::DockSpaceState& space)
{
    space.nodes.clear();
    space.nodes.emplace_back();
}

[[nodiscard]] i32
allocateNode(detail::DockSpaceState& space)
{
    for (std::size_t i = 1; i < space.nodes.size(); ++i) {
        if (!space.nodes[i].used) {
            space.nodes[i] = detail::DockNode{};
            return static_cast<i32>(i);
        }
    }
    space.nodes.emplace_back();
    return static_cast<i32>(space.nodes.size() - 1);
}

/// Divides the leaf node into two children. The node's tabs move to the first
/// child, or to the second when tabsToSecond is true.
DockNodePair
splitLeaf(detail::DockSpaceState& space, i32 node, DockSplit split, f32 ratio, bool tabsToSecond)
{
    POMDOG_ASSERT(nodeAt(space, node).isLeaf());

    // NOTE: Allocate both children before taking references, because
    // allocation can move the nodes.
    const auto first = allocateNode(space);
    const auto second = allocateNode(space);
    auto& parent = nodeAt(space, node);
    auto& owner = nodeAt(space, tabsToSecond ? second : first);
    owner.tabs = std::move(parent.tabs);
    owner.selectedTab = parent.selectedTab;
    parent.tabs.clear();
    parent.selectedTab = WidgetID{};
    parent.children = {{first, second}};
    parent.split = split;
    parent.ratio = std::clamp(ratio, 0.0f, 1.0f);
    nodeAt(space, first).parent = node;
    nodeAt(space, second).parent = node;
    return DockNodePair{DockNodeID{first}, DockNodeID{second}};
}

/// Replaces the parent of the empty leaf node with the leaf's sibling.
void removeEmptyLeaf(detail::DockSpaceState& space, i32 node)
{
    auto& leaf = nodeAt(space, node);
    POMDOG_ASSERT(leaf.isLeaf() && leaf.tabs.empty());
    const auto parentIndex = leaf.parent;
    if (parentIndex < 0) {
        // NOTE: The root stays, even when it is empty.
        return;
    }

    auto& parent = nodeAt(space, parentIndex);
    const auto siblingIndex = (parent.children[0] == node) ? parent.children[1] : parent.children[0];
    auto& sibling = nodeAt(space, siblingIndex);
    parent.children = sibling.children;
    parent.split = sibling.split;
    parent.ratio = sibling.ratio;
    parent.tabs = std::move(sibling.tabs);
    parent.selectedTab = sibling.selectedTab;
    for (const auto child : parent.children) {
        if (child >= 0) {
            nodeAt(space, child).parent = parentIndex;
        }
    }

    leaf = detail::DockNode{};
    leaf.used = false;
    sibling = detail::DockNode{};
    sibling.used = false;
}

/// Returns the leaf node where window is docked, or -1.
[[nodiscard]] i32
findTab(const detail::DockSpaceState& space, WidgetID window) noexcept
{
    for (std::size_t i = 0; i < space.nodes.size(); ++i) {
        const auto& node = space.nodes[i];
        if (node.used && node.isLeaf() && (std::find(node.tabs.begin(), node.tabs.end(), window) != node.tabs.end())) {
            return static_cast<i32>(i);
        }
    }
    return -1;
}

/// Removes window from the tabs of the leaf node. When the node becomes empty
/// and removeEmptyNode is true, the node goes too.
void removeTab(detail::DockSpaceState& space, i32 node, WidgetID window, bool removeEmptyNode)
{
    auto& leaf = nodeAt(space, node);
    const auto iter = std::find(leaf.tabs.begin(), leaf.tabs.end(), window);
    if (iter == leaf.tabs.end()) {
        return;
    }
    const auto position = static_cast<std::size_t>(std::distance(leaf.tabs.begin(), iter));
    leaf.tabs.erase(iter);
    if (leaf.selectedTab == window) {
        // NOTE: Select the tab that takes the removed tab's place.
        leaf.selectedTab = leaf.tabs.empty() ? WidgetID{} : leaf.tabs[std::min(position, leaf.tabs.size() - 1)];
    }
    if (leaf.tabs.empty() && removeEmptyNode) {
        removeEmptyLeaf(space, node);
    }
}

/// Removes window from the tabs of every dock space, keeping empty nodes.
void undockEverywhere(std::vector<detail::DockSpaceState>& spaces, WidgetID window)
{
    for (auto& space : spaces) {
        if (const auto node = findTab(space, window); node >= 0) {
            removeTab(space, node, window, false);
        }
    }
}

/// Returns the length of a split node's first child, out of available.
[[nodiscard]] f32
getFirstLength(f32 available, f32 ratio) noexcept
{
    const auto length = std::round(available * ratio);
    if (available < minNodeSize * 2.0f) {
        return std::clamp(length, 0.0f, std::max(0.0f, available));
    }
    return std::clamp(length, minNodeSize, available - minNodeSize);
}

/// Places node and its descendants in rect.
void layoutNodes(detail::DockSpaceState& space, i32 index, const F32Rect2D& rect, f32 tabBarHeight)
{
    auto& node = nodeAt(space, index);
    node.rect = rect;
    if (node.isLeaf()) {
        const auto barHeight = std::min(tabBarHeight, rect.height);
        node.contentRect = F32Rect2D{rect.x, rect.y + barHeight, rect.width, rect.height - barHeight};
        return;
    }
    node.contentRect = F32Rect2D{0.0f, 0.0f, 0.0f, 0.0f};

    F32Rect2D first;
    F32Rect2D second;
    if (node.split == DockSplit::LeftRight) {
        const auto available = std::max(0.0f, rect.width - splitterSize);
        const auto width = getFirstLength(available, node.ratio);
        first = F32Rect2D{rect.x, rect.y, width, rect.height};
        second = F32Rect2D{rect.x + width + splitterSize, rect.y, available - width, rect.height};
    }
    else {
        const auto available = std::max(0.0f, rect.height - splitterSize);
        const auto height = getFirstLength(available, node.ratio);
        first = F32Rect2D{rect.x, rect.y, rect.width, height};
        second = F32Rect2D{rect.x, rect.y + height + splitterSize, rect.width, available - height};
    }
    const auto children = node.children;
    layoutNodes(space, children[0], first, tabBarHeight);
    layoutNodes(space, children[1], second, tabBarHeight);
}

/// Returns the gap between the children of the split node.
[[nodiscard]] F32Rect2D
getSplitterRect(const detail::DockSpaceState& space, const detail::DockNode& node) noexcept
{
    const auto& first = nodeAt(space, node.children[0]).rect;
    if (node.split == DockSplit::LeftRight) {
        return F32Rect2D{first.x + first.width, node.rect.y, splitterSize, node.rect.height};
    }
    return F32Rect2D{node.rect.x, first.y + first.height, node.rect.width, splitterSize};
}

/// Returns the area where the pointer grabs the splitter of the split node.
[[nodiscard]] F32Rect2D
getSplitterGrabRect(const detail::DockSpaceState& space, const detail::DockNode& node) noexcept
{
    const auto gap = getSplitterRect(space, node);
    if (node.split == DockSplit::LeftRight) {
        return F32Rect2D{gap.x - splitterGrabMargin, gap.y, gap.width + splitterGrabMargin * 2.0f, gap.height};
    }
    return F32Rect2D{gap.x, gap.y - splitterGrabMargin, gap.width, gap.height + splitterGrabMargin * 2.0f};
}

/// Returns where a window dropped at point on the leaf node goes.
[[nodiscard]] DropZone
getDropZone(const detail::DockNode& node, const Vector2& point) noexcept
{
    // NOTE: An empty node and its tab bar only take tabs.
    if (node.tabs.empty() || (point.y < node.contentRect.y) || (node.rect.width <= 0.0f) || (node.rect.height <= 0.0f)) {
        return DropZone::Center;
    }
    const auto x = (point.x - node.rect.x) / node.rect.width;
    const auto y = (point.y - node.rect.y) / node.rect.height;
    const auto nearest = std::min({x, 1.0f - x, y, 1.0f - y});
    if (nearest >= dropEdgeRatio) {
        return DropZone::Center;
    }
    if (nearest == x) {
        return DropZone::Left;
    }
    if (nearest == 1.0f - x) {
        return DropZone::Right;
    }
    if (nearest == y) {
        return DropZone::Top;
    }
    return DropZone::Bottom;
}

/// Returns the share of the node that a window of size takes when it docks
/// beside the node's tabs.
[[nodiscard]] f32
getDropRatio(const detail::DockNode& node, DropZone zone, const Vector2& size) noexcept
{
    const bool leftRight = (zone == DropZone::Left) || (zone == DropZone::Right);
    const auto nodeLength = leftRight ? node.rect.width : node.rect.height;
    const auto windowLength = leftRight ? size.x : size.y;
    if (nodeLength <= 0.0f) {
        return maxDropRatio;
    }
    return std::clamp(windowLength / nodeLength, minDropRatio, maxDropRatio);
}

[[nodiscard]] F32Rect2D
getDropPreviewRect(const detail::DockNode& node, DropZone zone, f32 ratio) noexcept
{
    const auto& r = node.rect;
    switch (zone) {
    case DropZone::Center:
        break;
    case DropZone::Left:
        return F32Rect2D{r.x, r.y, std::round(r.width * ratio), r.height};
    case DropZone::Right: {
        const auto width = std::round(r.width * ratio);
        return F32Rect2D{r.x + r.width - width, r.y, width, r.height};
    }
    case DropZone::Top:
        return F32Rect2D{r.x, r.y, r.width, std::round(r.height * ratio)};
    case DropZone::Bottom: {
        const auto height = std::round(r.height * ratio);
        return F32Rect2D{r.x, r.y + r.height - height, r.width, height};
    }
    }
    return r;
}

void appendNumber(std::string& text, i64 value)
{
    text += ' ';
    text += std::to_string(value);
}

void writeNode(std::string& text, const detail::DockSpaceState& space, i32 index, const std::vector<detail::WindowState>& windows)
{
    const auto& node = nodeAt(space, index);
    if (!node.isLeaf()) {
        text += "split";
        appendNumber(text, (node.split == DockSplit::LeftRight) ? 0 : 1);
        appendNumber(text, std::lround(node.ratio * ratioScale));
        text += '\n';
        writeNode(text, space, node.children[0], windows);
        writeNode(text, space, node.children[1], windows);
        return;
    }

    // NOTE: A tab is written by its window's key. Every docked window has a
    // state, because docking creates one.
    i64 selected = -1;
    i64 count = 0;
    for (const auto tab : node.tabs) {
        if (findWindowIndex(windows, tab) < windows.size()) {
            if (tab == node.selectedTab) {
                selected = count;
            }
            ++count;
        }
    }
    text += "leaf";
    appendNumber(text, selected);
    appendNumber(text, count);
    for (const auto tab : node.tabs) {
        if (const auto windowIndex = findWindowIndex(windows, tab); windowIndex < windows.size()) {
            appendNumber(text, windows[windowIndex].key.name);
            appendNumber(text, windows[windowIndex].key.index);
        }
    }
    text += '\n';
}

/// Reads the space-separated fields of one line of a saved layout.
class FieldReader final {
public:
    explicit FieldReader(std::string_view line) noexcept
        : rest_(line)
    {
    }

    [[nodiscard]] std::string_view
    readWord() noexcept
    {
        const auto start = rest_.find_first_not_of(' ');
        if (start == std::string_view::npos) {
            rest_ = {};
            return {};
        }
        rest_.remove_prefix(start);
        const auto end = std::min(rest_.find(' '), rest_.size());
        const auto word = rest_.substr(0, end);
        rest_.remove_prefix(end);
        return word;
    }

    template <typename T>
    [[nodiscard]] bool
    read(T& value) noexcept
    {
        const auto word = readWord();
        if (word.empty()) {
            return false;
        }
        const auto [end, ec] = std::from_chars(word.data(), word.data() + word.size(), value);
        return (ec == std::errc{}) && (end == word.data() + word.size());
    }

    [[nodiscard]] bool
    readKey(Key& key) noexcept
    {
        return read(key.name) && read(key.index);
    }

    [[nodiscard]] bool
    atEnd() const noexcept
    {
        return rest_.find_first_not_of(' ') == std::string_view::npos;
    }

private:
    std::string_view rest_;
};

/// Holds a layout read from text before it replaces the context's layout.
struct LoadedLayout final {
    struct Window final {
        Key key = {};
        F32Rect2D rect = {0.0f, 0.0f, 0.0f, 0.0f};
        bool collapsed = false;
    };

    struct Tab final {
        i32 node = -1;
        Key window = {};
        bool selected = false;
    };

    struct DockSpace final {
        Key key = {};
        std::vector<detail::DockNode> nodes;
        std::vector<Tab> tabs;
    };

    std::vector<Window> windows;
    std::vector<DockSpace> dockSpaces;
};

/// Parses saved layouts line by line.
class LayoutParser final {
public:
    explicit LayoutParser(std::string_view text) noexcept
        : text_(text)
    {
    }

    [[nodiscard]] std::unique_ptr<Error>
    parse(LoadedLayout& layout)
    {
        std::string_view line;
        if (!nextLine(line) || (line != layoutHeader)) {
            return errors::make("devgui layout must start with \"" + std::string{layoutHeader} + "\"");
        }

        while (nextLine(line)) {
            FieldReader reader{line};
            const auto kind = reader.readWord();
            if (kind == "window") {
                LoadedLayout::Window window;
                i32 x = 0;
                i32 y = 0;
                i32 width = 0;
                i32 height = 0;
                i32 collapsed = 0;
                if (!reader.readKey(window.key) || !reader.read(x) || !reader.read(y) || !reader.read(width) ||
                    !reader.read(height) || !reader.read(collapsed) || !reader.atEnd() ||
                    (width < 0) || (height < 0) || (collapsed < 0) || (collapsed > 1)) {
                    return makeError("invalid window");
                }
                const auto duplicate = std::any_of(layout.windows.begin(), layout.windows.end(), [&](const LoadedLayout::Window& w) { return w.key == window.key; });
                if (duplicate) {
                    return makeError("duplicate window");
                }
                window.rect = F32Rect2D{static_cast<f32>(x), static_cast<f32>(y), static_cast<f32>(width), static_cast<f32>(height)};
                window.collapsed = (collapsed != 0);
                layout.windows.push_back(window);
            }
            else if (kind == "dock") {
                LoadedLayout::DockSpace dockSpace;
                if (!reader.readKey(dockSpace.key) || !reader.atEnd()) {
                    return makeError("invalid dock");
                }
                const auto duplicate = std::any_of(layout.dockSpaces.begin(), layout.dockSpaces.end(), [&](const LoadedLayout::DockSpace& d) { return d.key == dockSpace.key; });
                if (duplicate) {
                    return makeError("duplicate dock");
                }
                if (auto err = parseNode(dockSpace, -1, 0); err != nullptr) {
                    return err;
                }
                layout.dockSpaces.push_back(std::move(dockSpace));
            }
            else {
                return makeError("unknown entry");
            }
        }

        // NOTE: A window can be docked in only one place.
        std::vector<Key> docked;
        for (const auto& dockSpace : layout.dockSpaces) {
            for (const auto& tab : dockSpace.tabs) {
                if (std::find(docked.begin(), docked.end(), tab.window) != docked.end()) {
                    return errors::make("devgui layout docks a window twice");
                }
                docked.push_back(tab.window);
            }
        }
        return nullptr;
    }

private:
    [[nodiscard]] bool
    nextLine(std::string_view& line) noexcept
    {
        while (!text_.empty()) {
            const auto end = std::min(text_.find('\n'), text_.size());
            line = text_.substr(0, end);
            text_.remove_prefix(std::min(end + 1, text_.size()));
            ++lineNumber_;
            if (!line.empty() && (line.back() == '\r')) {
                line.remove_suffix(1);
            }
            if (!line.empty()) {
                return true;
            }
        }
        return false;
    }

    [[nodiscard]] std::unique_ptr<Error>
    makeError(std::string_view message) const
    {
        return errors::make("devgui layout line " + std::to_string(lineNumber_) + ": " + std::string{message});
    }

    [[nodiscard]] std::unique_ptr<Error>
    parseNode(LoadedLayout::DockSpace& dockSpace, i32 parent, i32 depth)
    {
        std::string_view line;
        if (!nextLine(line)) {
            return errors::make("devgui layout ends inside a dock");
        }
        if ((depth > maxLayoutDepth) || (dockSpace.nodes.size() >= maxLayoutNodes)) {
            return makeError("dock is too large");
        }

        const auto index = static_cast<i32>(dockSpace.nodes.size());
        dockSpace.nodes.emplace_back();
        dockSpace.nodes.back().parent = parent;

        FieldReader reader{line};
        const auto kind = reader.readWord();
        if (kind == "split") {
            i32 split = 0;
            i32 ratio = 0;
            if (!reader.read(split) || !reader.read(ratio) || !reader.atEnd() ||
                (split < 0) || (split > 1) || (ratio < 0) || (static_cast<f32>(ratio) > ratioScale)) {
                return makeError("invalid split");
            }
            const auto first = static_cast<i32>(dockSpace.nodes.size());
            if (auto err = parseNode(dockSpace, index, depth + 1); err != nullptr) {
                return err;
            }
            const auto second = static_cast<i32>(dockSpace.nodes.size());
            if (auto err = parseNode(dockSpace, index, depth + 1); err != nullptr) {
                return err;
            }
            auto& node = dockSpace.nodes[static_cast<std::size_t>(index)];
            node.children = {{first, second}};
            node.split = (split == 0) ? DockSplit::LeftRight : DockSplit::TopBottom;
            node.ratio = static_cast<f32>(ratio) / ratioScale;
            return nullptr;
        }
        if (kind == "leaf") {
            i32 selected = 0;
            u32 count = 0;
            if (!reader.read(selected) || !reader.read(count) || (count > maxLayoutTabs) ||
                (selected < -1) || (selected >= static_cast<i64>(count))) {
                return makeError("invalid leaf");
            }
            for (u32 i = 0; i < count; ++i) {
                LoadedLayout::Tab tab;
                tab.node = index;
                tab.selected = (static_cast<i64>(i) == selected);
                if (!reader.readKey(tab.window)) {
                    return makeError("invalid leaf");
                }
                dockSpace.tabs.push_back(tab);
            }
            if (!reader.atEnd()) {
                return makeError("invalid leaf");
            }
            return nullptr;
        }
        return makeError("expected split or leaf");
    }

private:
    std::string_view text_;
    i32 lineNumber_ = 0;
};

} // namespace

DockLayoutBuilder::DockLayoutBuilder(Context& context, Key dockSpaceKey)
    : context_(context)
    , dockSpaceID_(combineID(detail::rootID, dockSpaceKey))
{
    auto& spaces = context_.dockSpaces_;
    auto iter = std::find_if(spaces.begin(), spaces.end(), [&](const detail::DockSpaceState& s) { return s.id == dockSpaceID_; });
    if (iter == spaces.end()) {
        detail::DockSpaceState space;
        space.id = dockSpaceID_;
        space.key = dockSpaceKey;
        spaces.push_back(std::move(space));
        iter = std::prev(spaces.end());
    }
    resetNodes(*iter);

    // NOTE: The windows docked here before float until they dock again.
    for (auto& window : context_.windows_) {
        if (window.docked) {
            window.docked = std::any_of(spaces.begin(), spaces.end(), [&](const detail::DockSpaceState& s) { return findTab(s, window.id) >= 0; });
        }
    }

    // NOTE: A layout built during a frame shows from the next frame.
    context_.invalidated_ = true;
}

DockNodeID DockLayoutBuilder::root() const noexcept
{
    return DockNodeID{0};
}

DockNodePair DockLayoutBuilder::split(DockNodeID node, DockSplit split, f32 ratio)
{
    auto& spaces = context_.dockSpaces_;
    const auto iter = std::find_if(spaces.begin(), spaces.end(), [&](const detail::DockSpaceState& s) { return s.id == dockSpaceID_; });
    POMDOG_ASSERT(iter != spaces.end());
    POMDOG_ASSERT((node.index >= 0) && (static_cast<std::size_t>(node.index) < iter->nodes.size()));
    POMDOG_ASSERT(nodeAt(*iter, node.index).used && nodeAt(*iter, node.index).isLeaf());
    return splitLeaf(*iter, node.index, split, ratio, false);
}

void DockLayoutBuilder::dock(DockNodeID node, Key windowKey)
{
    auto& spaces = context_.dockSpaces_;
    const auto iter = std::find_if(spaces.begin(), spaces.end(), [&](const detail::DockSpaceState& s) { return s.id == dockSpaceID_; });
    POMDOG_ASSERT(iter != spaces.end());
    POMDOG_ASSERT((node.index >= 0) && (static_cast<std::size_t>(node.index) < iter->nodes.size()));
    POMDOG_ASSERT(nodeAt(*iter, node.index).used && nodeAt(*iter, node.index).isLeaf());

    auto& window = ensureWindow(context_.windows_, context_.depthCounter_, windowKey);
    undockEverywhere(spaces, window.id);
    window.docked = true;

    auto& leaf = nodeAt(*iter, node.index);
    leaf.tabs.push_back(window.id);
    if (leaf.selectedTab.value == 0) {
        leaf.selectedTab = window.id;
    }
}

void dockSpace(UI& ui, Key key, const F32Rect2D& rect, const DockSpaceOptions& options)
{
    ui.submitDockSpace(key, rect, options);
}

std::string Context::saveLayout() const
{
    std::string text{layoutHeader};
    text += '\n';

    // NOTE: Write windows back to front, so that loading them restores the
    // order in which they overlap.
    std::vector<const detail::WindowState*> order;
    order.reserve(windows_.size());
    for (const auto& window : windows_) {
        if (window.hasRect) {
            order.push_back(&window);
        }
    }
    std::sort(order.begin(), order.end(), [](const detail::WindowState* a, const detail::WindowState* b) { return a->depth < b->depth; });
    for (const auto* window : order) {
        text += "window";
        appendNumber(text, window->key.name);
        appendNumber(text, window->key.index);
        appendNumber(text, std::lround(window->rect.x));
        appendNumber(text, std::lround(window->rect.y));
        appendNumber(text, std::lround(window->rect.width));
        appendNumber(text, std::lround(window->rect.height));
        appendNumber(text, window->collapsed ? 1 : 0);
        text += '\n';
    }

    for (const auto& space : dockSpaces_) {
        text += "dock";
        appendNumber(text, space.key.name);
        appendNumber(text, space.key.index);
        text += '\n';
        writeNode(text, space, 0, windows_);
    }
    return text;
}

std::unique_ptr<Error> Context::loadLayout(std::string_view text)
{
    POMDOG_ASSERT(!frameActive_);

    LoadedLayout layout;
    LayoutParser parser{text};
    if (auto err = parser.parse(layout); err != nullptr) {
        return err;
    }

    for (const auto& loaded : layout.windows) {
        auto& window = ensureWindow(windows_, depthCounter_, loaded.key);
        window.rect = loaded.rect;
        window.collapsed = loaded.collapsed;
        window.hasRect = true;
        depthCounter_ += 1;
        window.depth = depthCounter_;
    }

    dockSpaces_.clear();
    for (auto& loaded : layout.dockSpaces) {
        detail::DockSpaceState space;
        space.id = combineID(detail::rootID, loaded.key);
        space.key = loaded.key;
        space.nodes = std::move(loaded.nodes);
        for (const auto& tab : loaded.tabs) {
            const auto id = ensureWindow(windows_, depthCounter_, tab.window).id;
            auto& node = nodeAt(space, tab.node);
            node.tabs.push_back(id);
            if (tab.selected) {
                node.selectedTab = id;
            }
        }
        dockSpaces_.push_back(std::move(space));
    }

    for (auto& window : windows_) {
        window.docked = std::any_of(dockSpaces_.begin(), dockSpaces_.end(), [&](const detail::DockSpaceState& s) { return findTab(s, window.id) >= 0; });
    }
    invalidated_ = true;
    return nullptr;
}

bool Context::hasDockSpace(Key key) const noexcept
{
    const auto id = combineID(detail::rootID, key);
    return std::any_of(dockSpaces_.begin(), dockSpaces_.end(), [&](const detail::DockSpaceState& s) { return s.id == id; });
}

void Context::getDockedWindows(Key dockSpaceKey, std::vector<DockedWindow>& result) const
{
    result.clear();
    const auto id = combineID(detail::rootID, dockSpaceKey);
    const auto spaceIter = std::find_if(dockSpaces_.begin(), dockSpaces_.end(), [&](const detail::DockSpaceState& s) { return s.id == id; });
    if (spaceIter == dockSpaces_.end()) {
        return;
    }

    // NOTE: List the windows whose tabs show, by the rule the tab bars use.
    // Between frames, the frame that comes next decides it.
    const auto frame = frameActive_ ? frameCount_ : (frameCount_ + 1);
    for (std::size_t n = 0; n < spaceIter->nodes.size(); ++n) {
        const auto& node = spaceIter->nodes[n];
        if (!node.used || !node.isLeaf()) {
            continue;
        }
        for (const auto tab : node.tabs) {
            const auto windowIndex = findWindowIndex(windows_, tab);
            if ((windowIndex >= windows_.size()) || (windows_[windowIndex].lastFrame + 1 < frame)) {
                continue;
            }
            result.push_back(DockedWindow{
                .window = windows_[windowIndex].key,
                .node = DockNodeID{static_cast<i32>(n)},
                .selected = (tab == node.selectedTab),
            });
        }
    }
}

void Context::selectWindow(Key windowKey) noexcept
{
    const auto id = combineID(detail::rootID, windowKey);
    for (auto& space : dockSpaces_) {
        if (const auto node = findTab(space, id); node >= 0) {
            nodeAt(space, node).selectedTab = id;
            invalidated_ = true;
            return;
        }
    }
    if (const auto index = findWindowIndex(windows_, id); index < windows_.size()) {
        depthCounter_ += 1;
        windows_[index].depth = depthCounter_;
        invalidated_ = true;
    }
}

void Context::moveWindowTab(Key windowKey, Key targetKey) noexcept
{
    const auto id = combineID(detail::rootID, windowKey);
    const auto targetID = combineID(detail::rootID, targetKey);
    for (auto& space : dockSpaces_) {
        const auto node = findTab(space, id);
        if ((node < 0) || (findTab(space, targetID) != node)) {
            continue;
        }
        auto& tabs = nodeAt(space, node).tabs;
        const auto from = std::find(tabs.begin(), tabs.end(), id);
        const auto to = std::find(tabs.begin(), tabs.end(), targetID);
        if (from < to) {
            std::rotate(from, std::next(from), std::next(to));
        }
        else if (to < from) {
            std::rotate(to, from, std::next(from));
        }
        invalidated_ = true;
        return;
    }
}

WidgetID UI::hitTestDockSpaces(const Vector2& point) const noexcept
{
    WidgetID result = {};
    for (const auto& space : context_.dockSpaces_) {
        if ((space.lastFrame + 1 != context_.frameCount_) || !space.rect.contains(point)) {
            continue;
        }
        result = space.id;

        // NOTE: Splitters reach a little into the nodes beside them, so that
        // they are easy to grab.
        const bool onSplitter = std::any_of(space.nodes.begin(), space.nodes.end(), [&](const detail::DockNode& node) {
            return node.used && !node.isLeaf() && getSplitterGrabRect(space, node).contains(point);
        });
        if (onSplitter) {
            continue;
        }
        for (const auto& node : space.nodes) {
            if (node.used && node.isLeaf() && (node.selectedTab.value != 0) && node.contentRect.contains(point)) {
                result = node.selectedTab;
            }
        }
    }
    return result;
}

bool UI::findDockedNode(WidgetID window, std::size_t& dockSpaceIndex, i32& nodeIndex) const noexcept
{
    const auto& spaces = context_.dockSpaces_;
    for (std::size_t i = 0; i < spaces.size(); ++i) {
        if (const auto node = findTab(spaces[i], window); node >= 0) {
            dockSpaceIndex = i;
            nodeIndex = node;
            return true;
        }
    }
    return false;
}

void UI::submitDockSpace(Key key, const F32Rect2D& rect, const DockSpaceOptions& options)
{
    // NOTE: Dock spaces lie at the root of the frame, below every floating
    // window.
    POMDOG_ASSERT(currentWindowIndex_ < 0);
    POMDOG_ASSERT((popupLevel_ < 0) && !inOverlay_);

    const auto id = combineID(detail::rootID, key);
    auto& spaces = context_.dockSpaces_;
    auto spaceIter = std::find_if(spaces.begin(), spaces.end(), [&](const detail::DockSpaceState& s) { return s.id == id; });
    if (spaceIter == spaces.end()) {
        detail::DockSpaceState state;
        state.id = id;
        state.key = key;
        resetNodes(state);
        spaces.push_back(std::move(state));
        spaceIter = std::prev(spaces.end());
    }
    auto& space = *spaceIter;

    // NOTE: A dock space submitted twice in one frame would draw over itself.
    POMDOG_ASSERT(space.lastFrame != context_.frameCount_);
    space.lastFrame = context_.frameCount_;
    space.rect = rect;
    space.showTabBars = options.showTabBars;

    const auto& style = context_.style_;
    auto& windows = context_.windows_;
    const auto& pointer = input_.pointerPosition;
    const auto tabBarHeight = options.showTabBars ? style.titleBarHeight : 0.0f;

    currentWindow_ = id;
    currentDepth_ = 0;
    context_.drawList_.beginRange(0);
    pushClipRect(rect);
    context_.idStack_.push_back(id);

    layoutNodes(space, 0, rect, tabBarHeight);

    // NOTE: Resize the splits before drawing, so that a dragged splitter and
    // the nodes beside it move together.
    i32 hotSplitter = -1;
    bool hotSplitterActive = false;
    for (std::size_t i = 0; i < space.nodes.size(); ++i) {
        auto& node = space.nodes[i];
        if (!node.used || node.isLeaf()) {
            continue;
        }
        const bool leftRight = (node.split == DockSplit::LeftRight);
        const auto& first = nodeAt(space, node.children[0]).rect;
        const auto available = std::max(0.0f, (leftRight ? node.rect.width : node.rect.height) - splitterSize);
        const auto firstLength = leftRight ? first.width : first.height;

        const auto response = interact(makeID(splitterKey.at(static_cast<u32>(i))), getSplitterGrabRect(space, node));
        if (response.isHeld() && !response.activated && (available > 0.0f)) {
            const auto delta = leftRight ? pointerDelta_.x : pointerDelta_.y;
            node.ratio = getFirstLength(available, (firstLength + delta) / available) / available;
        }
        if (response.hovered || response.active) {
            hotSplitter = static_cast<i32>(i);
            hotSplitterActive = response.active;
        }
    }
    layoutNodes(space, 0, rect, tabBarHeight);

    // NOTE: The background shows through the gaps between nodes.
    fillRect(rect, style.borderColor);

    struct PendingMove final {
        i32 node = -1;
        WidgetID moved = {};

        /// The tab that the moved tab goes before, or zero to go after the
        /// last shown tab, `after`.
        WidgetID before = {};
        WidgetID after = {};
    };
    struct PendingUndock final {
        i32 node = -1;
        std::size_t windowIndex = 0;
        Vector2 grabOffset = Vector2{0.0f, 0.0f};
    };
    PendingMove move;
    PendingUndock undock;

    auto& tabs = context_.dockTabs_;
    for (std::size_t n = 0; n < space.nodes.size(); ++n) {
        auto& node = space.nodes[n];
        if (!node.used || !node.isLeaf()) {
            continue;
        }

        const auto tabBar = F32Rect2D{node.rect.x, node.rect.y, node.rect.width, node.contentRect.y - node.rect.y};
        fillRect(node.contentRect, style.windowBackgroundColor);
        if (options.showTabBars) {
            fillRect(tabBar, style.titleBarColor);
            fillRect(F32Rect2D{tabBar.x, tabBar.y + tabBar.height - 1.0f, tabBar.width, 1.0f}, style.borderColor);
        }

        // NOTE: A tab shows while its window is submitted. The windows come
        // after the dock space, so use the previous frame.
        const auto paddingX = style.framePadding.x * 1.5f;
        const auto closeSize = std::round(tabBarHeight * 0.6f);
        tabs.clear();
        auto totalWidth = 0.0f;
        bool selectedVisible = false;
        for (std::size_t t = 0; t < node.tabs.size(); ++t) {
            const auto windowIndex = findWindowIndex(windows, node.tabs[t]);
            if ((windowIndex >= windows.size()) || (windows[windowIndex].lastFrame + 1 < context_.frameCount_)) {
                continue;
            }
            const auto& window = windows[windowIndex];
            const auto closeWidth = window.closable ? closeSize : 0.0f;
            const auto width = std::clamp(std::ceil(measureText(window.title)) + paddingX * 2.0f + closeWidth, minTabWidth, maxTabWidth);
            tabs.push_back(detail::DockTabLayout{
                .tabIndex = t,
                .windowIndex = windowIndex,
                .rect = F32Rect2D{0.0f, tabBar.y, width, tabBar.height},
            });
            totalWidth += width;
            selectedVisible = selectedVisible || (window.id == node.selectedTab);
        }
        if (tabs.empty()) {
            continue;
        }
        if (!selectedVisible) {
            node.selectedTab = windows[tabs.front().windowIndex].id;
        }
        if (!options.showTabBars) {
            continue;
        }

        // NOTE: Tabs shrink together when the tab bar is too narrow for them.
        const auto scale = (totalWidth > tabBar.width) ? (tabBar.width / totalWidth) : 1.0f;
        auto x = tabBar.x;
        for (auto& tab : tabs) {
            tab.rect.x = x;
            tab.rect.width = std::floor(tab.rect.width * scale);
            x += tab.rect.width;
        }

        i32 draggedTab = -1;
        i32 droppedTab = -1;
        for (std::size_t k = 0; k < tabs.size(); ++k) {
            const auto& tab = tabs[k];
            auto& window = windows[tab.windowIndex];

            // NOTE: The close button takes the press before the tab under it.
            const auto closeRect = F32Rect2D{
                tab.rect.x + tab.rect.width - paddingX * 0.5f - closeSize,
                tab.rect.y + std::round((tab.rect.height - closeSize) * 0.5f),
                closeSize,
                closeSize,
            };
            Response closeResponse;
            if (window.closable) {
                closeResponse = interact(combineID(window.id, tabCloseKey), closeRect);
                if (closeResponse.clicked) {
                    // NOTE: The window sets its open flag to false when it is
                    // submitted later in this frame. Select the tab beside it,
                    // as the tab bar will hide this one.
                    window.closeRequested = true;
                    requestUpdateAt(input_.timeSeconds);
                    if ((window.id == node.selectedTab) && (tabs.size() > 1)) {
                        const auto neighbor = (k + 1 < tabs.size()) ? tabs[k + 1] : tabs[k - 1];
                        node.selectedTab = windows[neighbor.windowIndex].id;
                    }
                }
            }

            const auto response = interact(combineID(window.id, tabKey), tab.rect);
            if (response.activated) {
                node.selectedTab = window.id;
            }

            if (response.isHeld()) {
                auto& travel = context_.activeScratch_;
                if (!response.activated) {
                    travel += std::abs(pointerDelta_.x) + std::abs(pointerDelta_.y);
                }
                if (response.active && !inflate(tabBar, undockDistance).contains(pointer)) {
                    // NOTE: The window comes out where the pointer grabbed its
                    // tab, and the pointer keeps dragging it by its title bar.
                    const auto grabX = std::clamp(getPressPosition().x - tab.rect.x, 0.0f, std::max(0.0f, window.rect.width));
                    undock = PendingUndock{
                        .node = static_cast<i32>(n),
                        .windowIndex = tab.windowIndex,
                        .grabOffset = Vector2{grabX, style.titleBarHeight * 0.5f},
                    };
                }
                else if (travel >= detail::reorderDragThreshold) {
                    // NOTE: Like VS Code, a dragged tab stays in place until
                    // the pointer drops it at the edge it shows.
                    if (response.active) {
                        draggedTab = static_cast<i32>(k);
                    }
                    else {
                        droppedTab = static_cast<i32>(k);
                    }
                }
            }

            const bool selected = (window.id == node.selectedTab);
            if (selected) {
                // NOTE: The selected tab joins the content below it.
                fillRect(tab.rect, style.windowBackgroundColor);
                fillRect(F32Rect2D{tab.rect.x, tab.rect.y, tab.rect.width, 1.0f}, style.accentColor);
            }
            else if (response.hovered || response.active) {
                fillRect(F32Rect2D{tab.rect.x, tab.rect.y, tab.rect.width, tab.rect.height - 1.0f}, style.rowHoveredColor);
            }
            fillRect(F32Rect2D{tab.rect.x + tab.rect.width - 1.0f, tab.rect.y, 1.0f, tab.rect.height}, style.borderColor);

            const auto textPadding = std::min(paddingX, tab.rect.width * 0.25f);
            const auto closeWidth = window.closable ? closeSize : 0.0f;
            const auto textColor = selected ? style.textColor : style.textSecondaryColor;
            const auto textTop = tab.rect.y + (tab.rect.height - style.fontSize) * 0.5f;
            drawText(Vector2{tab.rect.x + textPadding, textTop}, window.title, textColor, std::max(0.0f, tab.rect.width - textPadding * 2.0f - closeWidth));

            // NOTE: Like VS Code, show the close button on the selected tab and
            // on the tab under the pointer.
            if (window.closable) {
                detail::drawCloseButton(*this, closeRect, closeResponse, selected || response.hovered);
            }
        }

        if ((draggedTab >= 0) || (droppedTab >= 0)) {
            // NOTE: The drop goes to the edge between tabs nearest the pointer,
            // which lies right of every tab whose middle is left of it.
            std::size_t gap = 0;
            for (std::size_t k = 0; k < tabs.size(); ++k) {
                if (pointer.x >= tabs[k].rect.x + tabs[k].rect.width * 0.5f) {
                    gap = k + 1;
                }
            }
            if (draggedTab >= 0) {
                const auto& tab = tabs[static_cast<std::size_t>(draggedTab)];
                const auto edge = (gap == 0) ? tabs.front().rect.x : (tabs[gap - 1].rect.x + tabs[gap - 1].rect.width);
                detail::drawDropIndicator(*this, tabBar, edge, true);

                // NOTE: The copy keeps the point where the user grabbed the tab
                // under the pointer.
                const auto grab = getPressPosition() - Vector2{tab.rect.x, tab.rect.y};
                const auto ghost = F32Rect2D{pointer.x - grab.x, pointer.y - grab.y, tab.rect.width, tab.rect.height};
                detail::drawDragGhost(*this, ghost, windows[tab.windowIndex].title, paddingX);
            }
            const auto dropped = static_cast<std::size_t>(droppedTab);
            if ((droppedTab >= 0) && (gap != dropped) && (gap != dropped + 1)) {
                move = PendingMove{
                    .node = static_cast<i32>(n),
                    .moved = windows[tabs[dropped].windowIndex].id,
                    .before = (gap < tabs.size()) ? windows[tabs[gap].windowIndex].id : WidgetID{},
                    .after = windows[tabs.back().windowIndex].id,
                };
            }
        }
    }

    if (hotSplitter >= 0) {
        const auto& node = nodeAt(space, hotSplitter);
        const auto gap = getSplitterRect(space, node);
        const auto highlight = (node.split == DockSplit::LeftRight)
                                   ? F32Rect2D{gap.x - 1.0f, gap.y, gap.width + 2.0f, gap.height}
                                   : F32Rect2D{gap.x, gap.y - 1.0f, gap.width, gap.height + 2.0f};
        fillRect(highlight, hotSplitterActive ? style.accentActiveColor : style.accentColor);
    }

    if (move.node >= 0) {
        // NOTE: Hidden tabs keep their places relative to the shown tabs
        // around them.
        auto& nodeTabs = nodeAt(space, move.node).tabs;
        nodeTabs.erase(std::find(nodeTabs.begin(), nodeTabs.end(), move.moved));
        auto position = nodeTabs.end();
        if (move.before.value != 0) {
            position = std::find(nodeTabs.begin(), nodeTabs.end(), move.before);
        }
        else {
            position = std::next(std::find(nodeTabs.begin(), nodeTabs.end(), move.after));
        }
        nodeTabs.insert(position, move.moved);

        // NOTE: The tab bar was drawn in the old order.
        requestUpdateAt(input_.timeSeconds);
    }
    if (undock.node >= 0) {
        removeTab(space, undock.node, windows[undock.windowIndex].id, true);
        startWindowDrag(undock.windowIndex, undock.grabOffset);
        requestUpdateAt(input_.timeSeconds);
    }

    context_.idStack_.pop_back();
    popClipRect();
    currentWindow_ = WidgetID{};
    currentDepth_ = 0;
    context_.drawList_.beginRange(0);
}

void UI::updateWindowDocking()
{
    auto& spaces = context_.dockSpaces_;
    auto& windows = context_.windows_;
    const auto frameCount = context_.frameCount_;

    // NOTE: A tab bar showed a window that this frame did not submit. Ask for
    // one more frame so that the tab bar drops it.
    for (const auto& space : spaces) {
        if (space.lastFrame != frameCount) {
            continue;
        }
        for (const auto& node : space.nodes) {
            if (!node.used || !node.isLeaf()) {
                continue;
            }
            for (const auto tab : node.tabs) {
                if (const auto index = findWindowIndex(windows, tab); (index < windows.size()) && (windows[index].lastFrame + 1 == frameCount)) {
                    requestUpdateAt(input_.timeSeconds);
                }
            }
        }
    }

    if ((draggedWindow_.value == 0) || !input_.pointerPresent) {
        return;
    }
    const auto windowIndex = findWindowIndex(windows, draggedWindow_);
    if (windowIndex >= windows.size()) {
        return;
    }
    const auto& pointer = input_.pointerPosition;

    // NOTE: Another floating window under the pointer hides the dock spaces.
    for (const auto& window : windows) {
        if ((window.id != draggedWindow_) && !window.docked && (window.lastFrame == frameCount) &&
            context_.getWindowHitRect(window).contains(pointer)) {
            return;
        }
    }

    for (auto& space : spaces) {
        if ((space.lastFrame != frameCount) || !space.rect.contains(pointer)) {
            continue;
        }
        for (std::size_t n = 0; n < space.nodes.size(); ++n) {
            const auto& node = space.nodes[n];
            if (!node.used || !node.isLeaf() || !node.rect.contains(pointer)) {
                continue;
            }

            auto& window = windows[windowIndex];
            const auto zone = getDropZone(node, pointer);
            const auto ratio = getDropRatio(node, zone, Vector2{window.rect.width, window.rect.height});
            const auto preview = getDropPreviewRect(node, zone, ratio);

            const auto& style = context_.style_;
            beginOverlay();
            fillRect(preview, withAlpha(style.accentColor, 0.25f), style.frameRounding);
            strokeRect(preview, style.accentColor, 2.0f, style.frameRounding);
            endOverlay();

            if (released_) {
                const auto nodeIndex = static_cast<i32>(n);
                switch (zone) {
                case DropZone::Center: {
                    auto& leaf = nodeAt(space, nodeIndex);
                    leaf.tabs.push_back(window.id);
                    leaf.selectedTab = window.id;
                    break;
                }
                case DropZone::Left:
                case DropZone::Top: {
                    const auto split = (zone == DropZone::Left) ? DockSplit::LeftRight : DockSplit::TopBottom;
                    const auto children = splitLeaf(space, nodeIndex, split, ratio, true);
                    auto& leaf = nodeAt(space, children.first.index);
                    leaf.tabs.push_back(window.id);
                    leaf.selectedTab = window.id;
                    break;
                }
                case DropZone::Right:
                case DropZone::Bottom: {
                    const auto split = (zone == DropZone::Right) ? DockSplit::LeftRight : DockSplit::TopBottom;
                    const auto children = splitLeaf(space, nodeIndex, split, 1.0f - ratio, false);
                    auto& leaf = nodeAt(space, children.second.index);
                    leaf.tabs.push_back(window.id);
                    leaf.selectedTab = window.id;
                    break;
                }
                }
                window.docked = true;
                requestUpdateAt(input_.timeSeconds);
            }
            return;
        }
    }
}

} // namespace pomdog::devgui

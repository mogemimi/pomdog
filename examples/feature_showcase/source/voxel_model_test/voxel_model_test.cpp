#include "voxel_model_test.h"
#include "pomdog/experimental/clipboard/platform_clipboard.h"
#include "pomdog/experimental/voxelmodel/voxel_model_loader.h"
#include "pomdog/experimental/voxelmodel/voxel_model_writer.h"
#include "pomdog/filesystem/file_system.h"
#include "pomdog/utility/path_helper.h"
#include "pomdog/utility/string_format.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <string>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {
namespace {

using namespace pomdog::devgui::literals;

constexpr std::string_view originalFilePath = "/assets/voxel_models/maidchan.voxelmodel";
constexpr std::string_view exportDirectoryName = "pomdog_feature_showcase";
constexpr std::string_view exportFileName = "maidchan.voxelmodel";

// NOTE: The panel fits a canvas of 480 logical pixels in height.
const Vector2 panelPosition = {8.0f, 48.0f};
const Vector2 panelSize = {340.0f, 420.0f};

[[nodiscard]] f32
toRadians(f32 degrees) noexcept
{
    return degrees * (math::Pi<f32> / 180.0f);
}

[[nodiscard]] std::string
toString(const std::optional<Point3D>& cell)
{
    if (!cell.has_value()) {
        return "-";
    }
    return pomdog::format("({}, {}, {})", cell->x, cell->y, cell->z);
}

} // namespace

VoxelModelTest::VoxelModelTest(const std::shared_ptr<GameHost>& gameHostIn, const std::shared_ptr<vfs::FileSystemContext>& fs)
    : gameHost_(gameHostIn)
    , fs_(fs)
    , graphicsDevice_(gameHostIn->getGraphicsDevice())
    , commandQueue_(gameHostIn->getCommandQueue())
{
}

std::unique_ptr<Error>
VoxelModelTest::initialize(const std::shared_ptr<GameHost>& /*gameHost*/)
{
    // NOTE: Create graphics command list
    if (auto [commandList, err] = graphicsDevice_->createCommandList(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create graphics command list");
    }
    else {
        commandList_ = std::move(commandList);
    }

    // NOTE: Create PrimitiveBatch effect
    if (auto [p, err] = createPrimitivePipeline(
            fs_,
            graphicsDevice_,
            gpu::DepthStencilDesc::createDefault(),
            std::nullopt);
        err != nullptr) {
        return errors::wrap(std::move(err), "failed to create PrimitivePipeline");
    }
    else {
        primitivePipeline_ = std::move(p);
    }
    if (auto [p, err] = createPrimitiveBatch(graphicsDevice_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create PrimitiveBatch");
    }
    else {
        primitiveBatch_ = std::move(p);
    }

    // NOTE: Load the voxel model that the asset pipeline converted from a
    // MagicaVoxel file.
    if (auto [model, err] = voxelmodel::loadVoxelModel(fs_, originalFilePath); err != nullptr) {
        return errors::wrap(std::move(err), "failed to load voxel model");
    }
    else if (model.grids.empty()) {
        return errors::make("the voxel model has no grids");
    }
    else {
        setModel(std::move(model));
    }

    // NOTE: Export to a directory in the temporary directory of the OS, which
    // a file system of its own mounts with write access.
    auto [tempDir, tempErr] = platformfs::getTempDirectoryPath();
    if (tempErr != nullptr) {
        return errors::wrap(std::move(tempErr), "failed to get the temporary directory");
    }
    const auto exportDirectory = filepaths::join(tempDir, exportDirectoryName);
    if (auto [creation, err] = platformfs::createDirectories(exportDirectory); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create " + exportDirectory);
    }
    if (auto [ctx, err] = vfs::create(); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create file system");
    }
    else {
        exportFS_ = std::move(ctx);
    }
    if (auto err = vfs::mount(exportFS_, "/export", exportDirectory, {}); err != nullptr) {
        return errors::wrap(std::move(err), "failed to mount " + exportDirectory);
    }
    exportFilePath_ = filepaths::join(exportDirectory, exportFileName);

    // NOTE: GUI
    if (auto err = overlay_.initialize(gameHost_, fs_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to initialize DevGUIOverlay");
    }

    // NOTE: The text field of the export path copies to the platform's
    // clipboard.
    if (auto [p, err] = clipboard::createClipboardService(gameHost_); err != nullptr) {
        return errors::wrap(std::move(err), "failed to create clipboard service");
    }
    else {
        clipboard_ = std::move(p);
    }
    overlay_.setClipboardService(clipboard_);

    return nullptr;
}

void VoxelModelTest::setModel(voxelmodel::VoxelModel&& model)
{
    voxelModel_ = std::move(model);
    occupancyDirty_ = true;
    hover_ = {};
}

Matrix4x4 VoxelModelTest::computeGridToClip() const
{
    // NOTE: The orthographic camera zoom is derived from the logical client
    // size so the model keeps the same on-screen size across DPI levels.
    const auto clientBounds = gameHost_->getWindow()->getClientBounds();
    constexpr f32 orthographicSize = 16.0f;
    const auto projectionMatrix = Matrix4x4::createOrthographicLH(
        static_cast<f32>(clientBounds.width) / orthographicSize,
        static_cast<f32>(clientBounds.height) / orthographicSize,
        -100.0f,
        100.0f);
    const auto viewMatrix = Matrix4x4::createLookAtLH(Vector3::createZero(), Vector3{2.0f, -3.0f, -10.0f}, Vector3::createUnitY());

    // NOTE: The grid turns about its center. The z axis of the voxels points
    // up, so the grid first turns to make it point along the y axis of the
    // view, and then turns by the yaw and the pitch.
    const auto& grid = voxelModel_.grids.front();
    const auto center = Vector3{
        static_cast<f32>(grid.sizeX) * 0.5f,
        static_cast<f32>(grid.sizeY) * 0.5f,
        static_cast<f32>(grid.sizeZ) * 0.5f,
    };
    const auto modelMatrix =
        Matrix4x4::createTranslation(-center) *
        Matrix4x4::createRotationX(math::PiOver2<f32> * 3.0f) *
        Matrix4x4::createRotationY(toRadians(yawDegrees_)) *
        Matrix4x4::createRotationX(toRadians(pitchDegrees_));

    // NOTE: Move the model to the middle of the area right of the panel, so
    // that the panel does not hide the voxels to click. The offset is in the
    // clip space, where the width of the window spans 2.
    const auto panelRight = panelPosition.x + panelSize.x;
    const auto offsetMatrix = Matrix4x4::createTranslation(Vector3{
        panelRight / std::max(static_cast<f32>(clientBounds.width), panelRight * 2.0f),
        0.0f,
        0.0f,
    });
    return modelMatrix * viewMatrix * projectionMatrix * offsetMatrix;
}

bool VoxelModelTest::isOccupied(const Point3D& cell)
{
    const auto& grid = voxelModel_.grids.front();
    const auto indexOf = [&](u32 x, u32 y, u32 z) -> std::size_t {
        return (static_cast<std::size_t>(z) * grid.sizeY + y) * grid.sizeX + x;
    };
    if (occupancyDirty_) {
        occupancy_.assign(static_cast<std::size_t>(grid.sizeX) * grid.sizeY * grid.sizeZ, 0);
        for (const auto& v : grid.voxels) {
            occupancy_[indexOf(v.x, v.y, v.z)] = v.colorIndex;
        }
        occupancyDirty_ = false;
    }
    return occupancy_[indexOf(static_cast<u32>(cell.x), static_cast<u32>(cell.y), static_cast<u32>(cell.z))] != 0;
}

VoxelModelTest::PickResult
VoxelModelTest::pick(const Point2D& position)
{
    PickResult result;
    const auto clientBounds = gameHost_->getWindow()->getClientBounds();
    if ((clientBounds.width <= 0) || (clientBounds.height <= 0)) {
        return result;
    }

    // NOTE: Turn the pointer into a ray in the space of the grid. Two points
    // at different depths of the clip space give the ray, whichever depth
    // range the projection uses.
    const auto ndcX = static_cast<f32>(position.x) / static_cast<f32>(clientBounds.width) * 2.0f - 1.0f;
    const auto ndcY = 1.0f - static_cast<f32>(position.y) / static_cast<f32>(clientBounds.height) * 2.0f;
    const auto clipToGrid = math::invert(computeGridToClip());
    const auto unproject = [&](f32 depth) -> std::array<f32, 3> {
        const auto v = math::transform(Vector4{ndcX, ndcY, depth, 1.0f}, clipToGrid);
        return {v.x / v.w, v.y / v.w, v.z / v.w};
    };
    const auto origin = unproject(-1.0f);
    const auto target = unproject(1.0f);
    const std::array<f32, 3> direction = {target[0] - origin[0], target[1] - origin[1], target[2] - origin[2]};

    const auto& grid = voxelModel_.grids.front();
    const std::array<i32, 3> size = {static_cast<i32>(grid.sizeX), static_cast<i32>(grid.sizeY), static_cast<i32>(grid.sizeZ)};

    // NOTE: Find where the ray enters the box of the grid.
    constexpr f32 epsilon = 1e-6f;
    f32 tEnter = 0.0f;
    f32 tExit = 1.0f;
    i32 enterAxis = -1;
    for (i32 axis = 0; axis < 3; ++axis) {
        const auto i = static_cast<std::size_t>(axis);
        if (std::abs(direction[i]) < epsilon) {
            if ((origin[i] < 0.0f) || (origin[i] > static_cast<f32>(size[i]))) {
                return result;
            }
            continue;
        }
        auto t0 = (0.0f - origin[i]) / direction[i];
        auto t1 = (static_cast<f32>(size[i]) - origin[i]) / direction[i];
        if (t0 > t1) {
            std::swap(t0, t1);
        }
        if (t0 > tEnter) {
            tEnter = t0;
            enterAxis = axis;
        }
        tExit = std::min(tExit, t1);
    }
    if (tEnter > tExit) {
        return result;
    }

    // NOTE: Walk the cells along the ray (Amanatides and Woo).
    constexpr f32 infinity = std::numeric_limits<f32>::infinity();
    std::array<i32, 3> cell = {};
    std::array<i32, 3> step = {};
    std::array<f32, 3> tMax = {};
    std::array<f32, 3> tDelta = {};
    for (i32 axis = 0; axis < 3; ++axis) {
        const auto i = static_cast<std::size_t>(axis);
        if (axis == enterAxis) {
            cell[i] = (direction[i] > 0.0f) ? 0 : size[i] - 1;
        }
        else {
            const auto p = origin[i] + direction[i] * tEnter;
            cell[i] = std::clamp(static_cast<i32>(std::floor(p)), 0, size[i] - 1);
        }
        if (direction[i] > epsilon) {
            step[i] = 1;
            tMax[i] = (static_cast<f32>(cell[i] + 1) - origin[i]) / direction[i];
            tDelta[i] = 1.0f / direction[i];
        }
        else if (direction[i] < -epsilon) {
            step[i] = -1;
            tMax[i] = (static_cast<f32>(cell[i]) - origin[i]) / direction[i];
            tDelta[i] = -1.0f / direction[i];
        }
        else {
            tMax[i] = infinity;
            tDelta[i] = infinity;
        }
    }

    std::optional<Point3D> previous;
    for (;;) {
        const auto current = Point3D{cell[0], cell[1], cell[2]};
        if (isOccupied(current)) {
            result.voxel = current;
            result.emptyCell = previous;
            return result;
        }
        previous = current;

        std::size_t axis = 0;
        if (tMax[1] < tMax[axis]) {
            axis = 1;
        }
        if (tMax[2] < tMax[axis]) {
            axis = 2;
        }
        if (step[axis] == 0) {
            return result;
        }
        cell[axis] += step[axis];
        if ((cell[axis] < 0) || (cell[axis] >= size[axis])) {
            // NOTE: The ray leaves the grid. When it leaves through the floor,
            // the last cell lies on the floor.
            if ((axis == 2) && (step[axis] < 0)) {
                result.emptyCell = previous;
            }
            return result;
        }
        tMax[axis] += tDelta[axis];
    }
}

void VoxelModelTest::placeVoxel(const Point3D& cell)
{
    voxelModel_.grids.front().voxels.push_back(voxelmodel::Voxel{
        .x = static_cast<u8>(cell.x),
        .y = static_cast<u8>(cell.y),
        .z = static_cast<u8>(cell.z),
        .colorIndex = static_cast<u8>(colorIndex_),
    });
    occupancyDirty_ = true;
    statusText_ = pomdog::format("Placed a voxel at {}", toString(cell));
}

void VoxelModelTest::eraseVoxel(const Point3D& cell)
{
    std::erase_if(voxelModel_.grids.front().voxels, [&](const voxelmodel::Voxel& v) {
        return (v.x == cell.x) && (v.y == cell.y) && (v.z == cell.z);
    });
    occupancyDirty_ = true;
    statusText_ = pomdog::format("Erased the voxel at {}", toString(cell));
}

void VoxelModelTest::exportModel()
{
    const auto path = "/export/" + std::string{exportFileName};
    if (auto err = voxelmodel::saveVoxelModel(exportFS_, path, voxelModel_); err != nullptr) {
        statusText_ = err->toString();
        return;
    }
    statusText_ = "Exported";
}

void VoxelModelTest::reloadExportedModel()
{
    const auto path = "/export/" + std::string{exportFileName};
    auto [model, err] = voxelmodel::loadVoxelModel(exportFS_, path);
    if (err != nullptr) {
        statusText_ = err->toString();
        return;
    }
    if (model.grids.empty()) {
        statusText_ = "The exported model has no grids";
        return;
    }
    setModel(std::move(model));
    statusText_ = "Loaded the exported file";
}

void VoxelModelTest::reloadOriginalModel()
{
    auto [model, err] = voxelmodel::loadVoxelModel(fs_, originalFilePath);
    if (err != nullptr) {
        statusText_ = err->toString();
        return;
    }
    setModel(std::move(model));
    statusText_ = "Loaded the original file";
}

void VoxelModelTest::buildUI(devgui::UI& ui)
{
    const auto options = devgui::WindowOptions{
        .position = panelPosition,
        .size = panelSize,
        .collapsed = false,
    };
    auto window = devgui::window(ui, "voxelModel"_key, "Voxel Model", options);
    if (!window.isOpen()) {
        return;
    }

    const auto& grid = voxelModel_.grids.front();
    devgui::labelText(ui, "Voxels", pomdog::format("{} in {}x{}x{}", grid.voxels.size(), grid.sizeX, grid.sizeY, grid.sizeZ));

    devgui::separator(ui);
    devgui::checkbox(ui, "autoRotate"_key, "Auto Rotate", autoRotate_);
    devgui::sliderFloat(ui, "yaw"_key, "Yaw", yawDegrees_, -180.0f, 180.0f);
    devgui::sliderFloat(ui, "pitch"_key, "Pitch", pitchDegrees_, -90.0f, 90.0f);

    devgui::separator(ui);
    constexpr std::array<std::string_view, 2> editModes = {"Place", "Erase"};
    devgui::combo(ui, "editMode"_key, "Click", editMode_, editModes);
    devgui::text(ui, "Right-click erases a voxel.", ui.getStyle().textSecondaryColor);
    devgui::sliderInt(ui, "colorIndex"_key, "Color Index", colorIndex_, 1, 255);
    devgui::colorEdit(ui, "color"_key, "Color", voxelModel_.palette[static_cast<std::size_t>(colorIndex_)]);
    const auto& target = (static_cast<EditMode>(editMode_) == EditMode::Place) ? hover_.emptyCell : hover_.voxel;
    devgui::labelText(ui, "Target", toString(target));

    devgui::separator(ui);
    // NOTE: The text field shows the path so that it can be selected and
    // copied; the path resets every frame, so typing does not change it.
    exportFilePathText_ = exportFilePath_;
    devgui::inputText(ui, "exportPath"_key, "Path", exportFilePathText_);
    {
        auto row = devgui::horizontal(ui);
        if (devgui::button(ui, "export"_key, "Export").clicked) {
            exportModel();
        }
        if (devgui::button(ui, "reloadExported"_key, "Load Exported").clicked) {
            reloadExportedModel();
        }
        if (devgui::button(ui, "reloadOriginal"_key, "Reset").clicked) {
            reloadOriginalModel();
        }
    }
    if (!statusText_.empty()) {
        devgui::text(ui, statusText_, ui.getStyle().textSecondaryColor);
    }
}

void VoxelModelTest::update()
{
    overlay_.update([this](devgui::UI& ui) { buildUI(ui); });

    if (autoRotate_) {
        constexpr f32 degreesPerSecond = 30.0f;
        const auto frameDuration = static_cast<f32>(gameHost_->getClock()->getFrameDuration().count());
        yawDegrees_ += degreesPerSecond * frameDuration;
        if (yawDegrees_ >= 180.0f) {
            yawDegrees_ -= 360.0f;
        }
    }

    const auto mouse = gameHost_->getMouse();
    gestureTracker_.update(getPrimaryGestureState(*mouse, gameHost_->getTouchscreen().get()));
    const bool rightButtonDown = mouse->isButtonDown(MouseButtons::Right);
    const bool rightButtonPressed = rightButtonDown && !rightButtonWasDown_;
    rightButtonWasDown_ = rightButtonDown;

    // NOTE: The scene ignores the pointer while it is over the panel.
    if (overlay_.getInputCapture().pointer) {
        hover_ = {};
        return;
    }

    const auto position = gestureTracker_.getPosition();
    hover_ = pick(position);

    bool edited = false;
    if (gestureTracker_.pressedThisFrame()) {
        if ((static_cast<EditMode>(editMode_) == EditMode::Place) && hover_.emptyCell.has_value()) {
            placeVoxel(*hover_.emptyCell);
            edited = true;
        }
        else if ((static_cast<EditMode>(editMode_) == EditMode::Erase) && hover_.voxel.has_value()) {
            eraseVoxel(*hover_.voxel);
            edited = true;
        }
    }
    else if (rightButtonPressed && hover_.voxel.has_value()) {
        eraseVoxel(*hover_.voxel);
        edited = true;
    }
    if (edited) {
        hover_ = pick(position);
    }
}

void VoxelModelTest::draw()
{
    auto presentationParameters = graphicsDevice_->getPresentationParameters();

    gpu::Viewport viewport = {0, 0, presentationParameters.backBufferWidth, presentationParameters.backBufferHeight};
    gpu::RenderPass pass;
    pass.renderTargets[0] = {nullptr, Color::createCornflowerBlue().toVector4()};
    pass.depthStencilBuffer = nullptr;
    pass.clearDepth = 1.0f;
    pass.clearStencil = u8(0);
    pass.viewport = viewport;
    pass.scissorRect = viewport.getBounds();

    commandList_->reset();
    commandList_->beginRenderPass(std::move(pass));

    primitiveBatch_->reset();
    primitiveBatch_->setTransform(computeGridToClip());

    // NOTE: Draw voxels. A voxel spans one unit from its coordinates.
    const auto& grid = voxelModel_.grids.front();
    for (const auto& v : grid.voxels) {
        auto color = voxelModel_.palette[v.colorIndex];
        color.a = 255;
        primitiveBatch_->drawBox(
            Vector3{static_cast<f32>(v.x), static_cast<f32>(v.y), static_cast<f32>(v.z)},
            Vector3{1.0f, 1.0f, 1.0f},
            color);
    }

    // NOTE: Draw the floor of the grid as a thin plate under it.
    {
        constexpr f32 thickness = 0.05f;
        primitiveBatch_->drawBox(
            Vector3{0.0f, 0.0f, -thickness},
            Vector3{static_cast<f32>(grid.sizeX), static_cast<f32>(grid.sizeY), thickness},
            Color{255, 255, 255, 48});
    }

    // NOTE: Draw the cell that a click changes as a translucent box.
    {
        const auto drawCursor = [&](const Point3D& cell, const Color& color) {
            constexpr f32 margin = 0.05f;
            primitiveBatch_->drawBox(
                Vector3{static_cast<f32>(cell.x) - margin, static_cast<f32>(cell.y) - margin, static_cast<f32>(cell.z) - margin},
                Vector3{1.0f + margin * 2.0f, 1.0f + margin * 2.0f, 1.0f + margin * 2.0f},
                color);
        };
        if ((static_cast<EditMode>(editMode_) == EditMode::Place) && hover_.emptyCell.has_value()) {
            drawCursor(*hover_.emptyCell, Color{255, 255, 255, 120});
        }
        else if ((static_cast<EditMode>(editMode_) == EditMode::Erase) && hover_.voxel.has_value()) {
            drawCursor(*hover_.voxel, Color{255, 60, 60, 140});
        }
    }

    primitiveBatch_->flush(commandList_, primitivePipeline_);
    primitiveBatch_->submit(graphicsDevice_);

    commandList_->endRenderPass();

    // NOTE: Draw the panel over the scene.
    {
        gpu::RenderPass overlayPass;
        overlayPass.renderTargets[0] = {nullptr, std::nullopt};
        overlayPass.viewport = viewport;
        overlayPass.scissorRect = viewport.getBounds();
        commandList_->beginRenderPass(std::move(overlayPass));
    }
    overlay_.draw(commandList_);

    commandList_->endRenderPass();
    commandList_->close();

    constexpr bool isStandalone = false;
    if constexpr (isStandalone) {
        commandQueue_->reset();
        commandQueue_->pushBackCommandList(commandList_);
        commandQueue_->executeCommandLists();
        commandQueue_->present();
    }
    else {
        commandQueue_->pushBackCommandList(commandList_);
    }
}

} // namespace feature_showcase

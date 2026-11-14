#pragma once

#include "devgui_overlay/devgui_overlay.h"
#include "pomdog/experimental/clipboard/clipboard_service.h"
#include "pomdog/experimental/voxelmodel/voxel_model.h"
#include "pomdog/graphics/primitive_batch.h"
#include "pomdog/input/gesture_tracker.h"
#include "pomdog/pomdog.h"
#include "pomdog/vfs/file_system.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <optional>
#include <string>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace feature_showcase {

using namespace pomdog;

/// VoxelModelTest edits a `.voxelmodel` file that the asset pipeline
/// converted from a MagicaVoxel file. A click places or erases a voxel, and
/// the panel turns the model, changes palette colors, exports the model to the
/// temporary directory, and loads the exported file back.
class VoxelModelTest final : public Game {
public:
    explicit VoxelModelTest(const std::shared_ptr<GameHost>& gameHost, const std::shared_ptr<vfs::FileSystemContext>& fs);

    [[nodiscard]] std::unique_ptr<Error>
    initialize(const std::shared_ptr<GameHost>& gameHost) override;

    void update() override;

    void draw() override;

private:
    /// EditMode chooses what a click or a tap does.
    enum class EditMode : i32 {
        Place = 0,
        Erase = 1,
    };

    /// PickResult tells which cells of the grid lie under the pointer.
    struct PickResult final {
        /// The voxel that the pointer points at.
        std::optional<Point3D> voxel;

        /// The empty cell where a click places a voxel: the cell in front of
        /// the face that the pointer points at, or the cell on the floor of the
        /// grid when the pointer points at no voxel.
        std::optional<Point3D> emptyCell;
    };

    void buildUI(devgui::UI& ui);

    /// Replaces the model, whose first grid the test edits.
    void setModel(voxelmodel::VoxelModel&& model);

    /// Returns the matrix from the space of the first grid, where a voxel
    /// spans one unit from its coordinates, to the clip space.
    [[nodiscard]] Matrix4x4
    computeGridToClip() const;

    /// Returns the cells of the first grid under the pointer at a position in
    /// logical pixels.
    [[nodiscard]] PickResult
    pick(const Point2D& position);

    /// Returns true when a voxel of the first grid occupies the cell.
    [[nodiscard]] bool
    isOccupied(const Point3D& cell);

    void placeVoxel(const Point3D& cell);

    void eraseVoxel(const Point3D& cell);

    void exportModel();

    void reloadExportedModel();

    /// Loads the model of modelIndex_ that the asset pipeline converted, and
    /// returns false when it cannot.
    bool reloadOriginalModel();

    std::shared_ptr<GameHost> gameHost_;
    std::shared_ptr<vfs::FileSystemContext> fs_;
    std::shared_ptr<gpu::GraphicsDevice> graphicsDevice_;
    std::shared_ptr<gpu::CommandQueue> commandQueue_;
    std::shared_ptr<gpu::CommandList> commandList_;
    ConnectionList connect_;

    std::shared_ptr<PrimitiveBatch> primitiveBatch_;
    std::shared_ptr<PrimitivePipeline> primitivePipeline_;
    voxelmodel::VoxelModel voxelModel_;

    /// The color index of each cell of the first grid, which pick() reads.
    /// It is rebuilt when occupancyDirty_ is true.
    std::vector<u8> occupancy_;
    bool occupancyDirty_ = true;

    /// The file system whose "/export" directory is the export directory in
    /// the temporary directory of the OS.
    std::shared_ptr<vfs::FileSystemContext> exportFS_;
    std::string exportDirectory_;
    std::string exportFilePathText_;

    std::shared_ptr<clipboard::ClipboardService> clipboard_;
    DevGUIOverlay overlay_;
    GestureTracker gestureTracker_;
    PickResult hover_;
    std::string statusText_;
    f32 yawDegrees_ = 0.0f;
    f32 pitchDegrees_ = 0.0f;
    i32 modelIndex_ = 0;
    i32 editMode_ = static_cast<i32>(EditMode::Place);
    i32 colorIndex_ = 1;
    bool autoRotate_ = true;
    bool rightButtonWasDown_ = false;
};

} // namespace feature_showcase

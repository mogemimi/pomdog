// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/skeletal2d/animation_graph_builder.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/flatbuffers_macros.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/skeletal2d/animation_clip.h"
#include "pomdog/experimental/skeletal2d/animation_clip_data.h"
#include "pomdog/experimental/skeletal2d/animation_loader.h"
#include "pomdog/experimental/skeletal2d/blendtrees/animation_clip_node.h"
#include "pomdog/experimental/skeletal2d/blendtrees/animation_lerp_node.h"
#include "pomdog/utility/string_format.h"
#include "pomdog/vfs/file.h"
#include "pomdog/vfs/file_system.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_BEGIN
#include "pomdogschemas/skeletal2d_animgraph_generated.h"
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_FLATBUFFERS_HEADERS_END

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <string>
#include <utility>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d {
namespace {

using detail::AnimationClipNode;
using detail::AnimationLerpNode;

// NOTE: schemas/skeletal2d_animgraph.fbs limits a blend tree to this number of
// nodes, counting a shared node once for each reference to it.
constexpr u32 maxTreeNodeCount = 1024;

template <typename T>
[[nodiscard]] u32
getSize(const flatbuffers::Vector<T>* vector) noexcept
{
    if (vector == nullptr) {
        return 0;
    }
    return vector->size();
}

/// Returns true if no two of the given keys are equal.
[[nodiscard]] bool
hasUniqueKeys(std::vector<u32> keys)
{
    std::sort(keys.begin(), keys.end());
    return std::adjacent_find(keys.begin(), keys.end()) == keys.end();
}

[[nodiscard]] std::unique_ptr<Error>
validateGraph(const pomdogschemas::Skeletal2DAnimGraph& graph)
{
    const auto* nodes = graph.nodes();
    const auto nodeCount = getSize(nodes);
    const auto parameterCount = getSize(graph.parameters());

    // NOTE: The runtime creates a copy of a node for each reference to it, so
    // count the nodes of the tree under each node to limit its size.
    std::vector<u32> treeSizes(nodeCount);

    for (u32 index = 0; index < nodeCount; ++index) {
        const auto* node = nodes->Get(index);

        if (node->type() == pomdogschemas::Skeletal2DAnimGraphNodeType_Clip) {
            treeSizes[index] = 1;
        }
        else if (node->type() == pomdogschemas::Skeletal2DAnimGraphNodeType_Lerp) {
            // NOTE: Inputs that precede the node also rule out cycles.
            if (node->input0() >= index || node->input1() >= index) {
                return errors::make(pomdog::format("the inputs of node {} must precede it", index));
            }
            if (node->parameter() >= parameterCount) {
                return errors::make(pomdog::format("node {} has a parameter {} that is out of the {} parameters", index, node->parameter(), parameterCount));
            }
            treeSizes[index] = std::min(1 + treeSizes[node->input0()] + treeSizes[node->input1()], maxTreeNodeCount + 1);
        }
        else {
            return errors::make(pomdog::format("node {} has an unknown type", index));
        }
    }

    const auto* states = graph.states();
    const auto stateCount = getSize(states);
    if (stateCount == 0) {
        return errors::make("graph has no states");
    }

    std::vector<u32> stateKeys;
    stateKeys.reserve(stateCount);

    for (u32 index = 0; index < stateCount; ++index) {
        const auto* state = states->Get(index);
        stateKeys.push_back(state->key());
        if (state->tree() >= nodeCount) {
            return errors::make(pomdog::format("state {} has a tree {} that is out of the {} nodes", index, state->tree(), nodeCount));
        }
        if (treeSizes[state->tree()] > maxTreeNodeCount) {
            return errors::make(pomdog::format("the tree of state {} has more than {} nodes", index, maxTreeNodeCount));
        }
    }

    // NOTE: Animator finds a state and a parameter by its key.
    if (!hasUniqueKeys(std::move(stateKeys))) {
        return errors::make("two states have the same key");
    }
    if (const auto* parameters = graph.parameters(); parameters != nullptr) {
        if (!hasUniqueKeys(std::vector<u32>(parameters->begin(), parameters->end()))) {
            return errors::make("two parameters have the same key");
        }
    }
    return nullptr;
}

// NOTE: validateGraph() has verified the graph: a node is a clip node or a
// lerp node, and the inputs of a lerp node precede it.
[[nodiscard]] std::tuple<std::unique_ptr<AnimationNode>, std::unique_ptr<Error>>
createAnimationNode(
    const pomdogschemas::Skeletal2DAnimGraph& graph,
    u32 index,
    const AnimationClipData& clipData)
{
    const auto* node = graph.nodes()->Get(index);

    if (node->type() == pomdogschemas::Skeletal2DAnimGraphNodeType_Clip) {
        auto [animationClip, err] = createAnimationClipByKey(clipData, nullptr, node->clip_key());
        if (err != nullptr) {
            auto wrapped = errors::wrap(std::move(err), pomdog::format("failed to create clip node {}", index));
            return std::make_tuple(nullptr, std::move(wrapped));
        }
        return std::make_tuple(std::make_unique<AnimationClipNode>(animationClip), nullptr);
    }

    auto [node1, err1] = createAnimationNode(graph, node->input0(), clipData);
    if (err1 != nullptr) {
        return std::make_tuple(nullptr, std::move(err1));
    }

    auto [node2, err2] = createAnimationNode(graph, node->input1(), clipData);
    if (err2 != nullptr) {
        return std::make_tuple(nullptr, std::move(err2));
    }

    return std::make_tuple(std::make_unique<AnimationLerpNode>(std::move(node1), std::move(node2), node->parameter()), nullptr);
}

} // namespace

std::tuple<std::shared_ptr<AnimationGraph>, std::unique_ptr<Error>>
loadAnimationGraph(
    const std::shared_ptr<vfs::FileSystemContext>& fs,
    const AnimationClipData& clipData,
    std::string_view filePath)
{
    const auto path = std::string{filePath};

    auto [file, openErr] = vfs::open(fs, path);
    if (openErr != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(openErr), "cannot open animation graph file, " + path));
    }

    auto [bytes, viewErr] = file->view();
    if (viewErr != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(viewErr), "cannot read animation graph file, " + path));
    }

    flatbuffers::Verifier verifier(bytes.data(), bytes.size());
    if (!pomdogschemas::VerifySkeletal2DAnimGraphBuffer(verifier)) {
        return std::make_tuple(nullptr, errors::make("FlatBuffers verification failed for animation graph file, " + path));
    }

    const auto* graph = pomdogschemas::GetSkeletal2DAnimGraph(bytes.data());
    if (auto err = validateGraph(*graph); err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "invalid animation graph file, " + path));
    }

    auto animationGraph = std::make_shared<AnimationGraph>();

    if (const auto* parameters = graph->parameters(); parameters != nullptr) {
        animationGraph->inputs.reserve(parameters->size());
        for (const auto key : *parameters) {
            animationGraph->inputs.push_back({key, AnimationBlendInputType::Float});
        }
    }

    // NOTE: The first state is the initial state, so keep the order of the file.
    animationGraph->states.reserve(graph->states()->size());
    for (const auto* stateData : *graph->states()) {
        auto [tree, err] = createAnimationNode(*graph, stateData->tree(), clipData);
        if (err != nullptr) {
            auto wrapped = errors::wrap(std::move(err), pomdog::format("failed to create state {:#010x} of animation graph, {}", stateData->key(), path));
            return std::make_tuple(nullptr, std::move(wrapped));
        }

        AnimationGraphState state;
        state.key = stateData->key();
        state.tree = std::move(tree);
        animationGraph->states.push_back(std::move(state));
    }

    return std::make_tuple(std::move(animationGraph), nullptr);
}

} // namespace pomdog::skeletal2d

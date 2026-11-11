# animation-graph-json-to-skeletal2d

A tool that converts an animation graph written in JSON into the FlatBuffers
file that Pomdog's 2D skeletal animation runtime loads.

## Background

An animation graph has states, each of which plays a blend tree of animation
clips, and parameters that weight the blends. The runtime does not parse JSON.
This tool checks the graph and resolves what it refers to by name, so that the
runtime only verifies the file and creates the nodes:

- Nodes become an array in which the inputs of a node precede it, and states
  and nodes refer to nodes by index.
- The name of a clip becomes an XXH32 hash (seed `20160723`), which is the key
  of the clip in the `.clip_skel2d` file that
  [spine-json-to-skeletal2d](../spine-json-to-skeletal2d/README.md) writes.
- State and parameter names become XXH32 hashes too, which the application
  passes to `skeletal2d::Animator` to play a state or set a parameter.
- Parameters become an array, and lerp nodes refer to them by index. Lerp nodes
  with the same parameter name share the parameter.

The tool fails when two clips, two states, or two parameters have the same
hash. The debug file keeps the names behind the hashes.

The output is a `.animgraph_skel2d` file (`schemas/skeletal2d_animgraph.fbs`).
It does not name a clip file; the application passes the clips when it loads
the graph, and loading fails when a clip that the graph refers to is missing.

## Source format

```json
{
  "nodes": [
    {"name": "Run_Lerp", "type": "lerp", "param_name": "Run.Weight", "in": ["Run", "RunNoShot"]},
    {"name": "Run", "type": "clip"},
    {"name": "RunNoShot", "type": "clip"}
  ],
  "states": {
    "Run": {"tree": "Run_Lerp"},
    "Idle": {"tree": "RunNoShot"}
  }
}
```

| Field | Description |
|-------|-------------|
| `nodes[].name` | Name of the node, unique in the graph |
| `nodes[].type` | `"clip"` plays the animation clip that has the name of the node. `"lerp"` blends two nodes. |
| `nodes[].in` | Lerp node only: the names of the two nodes to blend |
| `nodes[].param_name` | Lerp node only: the name of the float parameter that weights the blend. 0 plays the first node of `in`, and 1 plays the second. |
| `states.<name>.tree` | Name of the root node of the blend tree that the state plays |

The first state of `states` is the initial state, so the tool keeps the states
in the order of the source.

The tool fails on a graph that the runtime cannot play: an unknown field or node
type, a duplicate name, a reference to a node that does not exist, nodes that
form a cycle, or a graph without states. A state or a node can refer to a node
that another one also refers to. The runtime creates a copy of the node for
each reference, so the tool also fails when the blend tree of a state has more
than 1024 nodes counted that way.

## Installation

```sh
cd tools/cmd/animation-graph-json-to-skeletal2d
go build -o ../../../build/tools/animation-graph-json-to-skeletal2d
```

The tool imports the Go code generated from the schemas, so run the bootstrap
or `tools/script/build_tools.sh` after changing a schema.

## Usage

```sh
animation-graph-json-to-skeletal2d -i <input.json> -o <output.animgraph_skel2d> [--out-debug <output.animgraph_skel2d-debug>]
```

### Options

| Option | Description |
|--------|-------------|
| `-i <path>` | Input animation graph JSON file (required) |
| `-o <path>` | Output animation graph file (required) |
| `--out-debug <path>` | Output debug file, which is not a runtime asset |

The debug file (`schemas/skeletal2d_animgraph_debug.fbs`) maps node indices
and the hashes of state and parameter names back to the names. Print it with
`flatc`:

```sh
flatc --json --strict-json --raw-binary -o ./build/tmp schemas/skeletal2d_animgraph_debug.fbs -- <output.animgraph_skel2d-debug>
```

### Example

```sh
./build/tools/animation-graph-json-to-skeletal2d \
    -i ./examples/feature_showcase/assets/skeletal2d/MaidGun/AnimationGraph.json \
    -o ./build/content/skeletal2d/MaidGun/AnimationGraph.animgraph_skel2d \
    --out-debug ./build/convertbuild/skeletal2d/MaidGun/AnimationGraph.animgraph_skel2d-debug
```

## Build pipeline integration

`asset-convert-ninja-gen` generates a Ninja build rule for each
`[[skeletal2d_animgraph_from_json]]` entry of a recipe:

```toml
[[skeletal2d_animgraph_from_json]]
in = "skeletal2d/MaidGun/AnimationGraph.json"
out = "skeletal2d/MaidGun/AnimationGraph.animgraph_skel2d"
```

See [asset-convert-ninja-gen](../asset-convert-ninja-gen/README.md) for the
recipe format.

## Runtime loading

```cpp
auto [clipData, clipErr] = skeletal2d::loadAnimationClipData(fs, "/assets/skeletal2d/MaidGun/MaidGun.clip_skel2d");
auto [animationGraph, graphErr] = skeletal2d::loadAnimationGraph(fs, *clipData, "/assets/skeletal2d/MaidGun/AnimationGraph.animgraph_skel2d");

auto animator = std::make_shared<skeletal2d::Animator>(skeleton, skeletonPose, animationGraph);
animator->setFloat(computeStringHash32("Run.Weight"), 0.5f);
animator->play(computeStringHash32("Idle"));
```

The snippet omits error handling for brevity.

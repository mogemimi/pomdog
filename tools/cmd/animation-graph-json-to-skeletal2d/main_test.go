// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"fmt"
	"os"
	"path/filepath"
	"strings"
	"testing"

	schemas "github.com/mogemimi/pomdog/build/schemas"
	stringhash "github.com/mogemimi/pomdog/tools/pkg/stringhash"
)

// NOTE: The lerp node comes before its inputs, and the state "Run" comes
// before "Idle" although it sorts after it.
const sampleJSON = `{
  "nodes": [
    {"name": "Run_Lerp", "type": "lerp", "param_name": "Run.Weight", "in": ["Run", "RunNoShot"]},
    {"name": "Run", "type": "clip"},
    {"name": "RunNoShot", "type": "clip"}
  ],
  "states": {
    "Run": {"tree": "Run_Lerp"},
    "Idle": {"tree": "RunNoShot"}
  }
}`

func convertJSON(t *testing.T, source string) (*output, error) {
	t.Helper()
	doc, err := parseGraphJSON([]byte(source))
	if err != nil {
		return nil, err
	}
	return convert(doc)
}

func mustConvertJSON(t *testing.T, source string) *output {
	t.Helper()
	out, err := convertJSON(t, source)
	if err != nil {
		t.Fatalf("convert() failed: %v", err)
	}
	return out
}

func TestConvert(t *testing.T) {
	out := mustConvertJSON(t, sampleJSON)
	graph := out.Graph

	// NOTE: The inputs of a node precede it.
	wantNames := []string{"Run", "RunNoShot", "Run_Lerp"}
	if strings.Join(out.Debug.Nodes, ",") != strings.Join(wantNames, ",") {
		t.Fatalf("got node names %v, want %v", out.Debug.Nodes, wantNames)
	}
	if len(graph.Nodes) != 3 {
		t.Fatalf("got %d nodes, want 3", len(graph.Nodes))
	}

	for i, name := range []string{"Run", "RunNoShot"} {
		node := graph.Nodes[i]
		if node.Type != schemas.Skeletal2DAnimGraphNodeTypeClip || node.ClipKey != stringhash.StringToHash32(name) {
			t.Errorf("node %d is not the clip node of %q: %+v", i, name, node)
		}
		if node.Input0 != 0 || node.Input1 != 0 || node.Parameter != 0 {
			t.Errorf("node %d has fields that a clip node leaves zero: %+v", i, node)
		}
	}

	lerp := graph.Nodes[2]
	if lerp.Type != schemas.Skeletal2DAnimGraphNodeTypeLerp || lerp.Input0 != 0 || lerp.Input1 != 1 || lerp.Parameter != 0 || lerp.ClipKey != 0 {
		t.Errorf("unexpected lerp node: %+v", lerp)
	}

	if len(graph.Parameters) != 1 || graph.Parameters[0] != stringhash.StringToHash32("Run.Weight") {
		t.Errorf("got parameters %v, want the key of Run.Weight", graph.Parameters)
	}

	// NOTE: The states keep the order of the source, since the first state
	// is the initial one.
	if len(graph.States) != 2 {
		t.Fatalf("got %d states, want 2", len(graph.States))
	}
	if graph.States[0].Key != stringhash.StringToHash32("Run") || graph.States[0].Tree != 2 {
		t.Errorf("unexpected first state: %+v", graph.States[0])
	}
	if graph.States[1].Key != stringhash.StringToHash32("Idle") || graph.States[1].Tree != 1 {
		t.Errorf("unexpected second state: %+v", graph.States[1])
	}

	// NOTE: The debug file maps the keys back to the names, in the same order.
	checkNames(t, "states", out.Debug.States, []string{"Run", "Idle"})
	checkNames(t, "parameters", out.Debug.Parameters, []string{"Run.Weight"})
}

func checkNames(t *testing.T, kind string, entries []*schemas.Skeletal2DAnimGraphNameDebugT, want []string) {
	t.Helper()
	if len(entries) != len(want) {
		t.Fatalf("got %d debug %s, want %d", len(entries), kind, len(want))
	}
	for i, entry := range entries {
		if entry.Name != want[i] || entry.Key != stringhash.StringToHash32(want[i]) {
			t.Errorf("debug %s #%d: got %+v, want %q", kind, i, entry, want[i])
		}
	}
}

func TestConvertSharesParameters(t *testing.T) {
	out := mustConvertJSON(t, `{
  "nodes": [
    {"name": "a", "type": "clip"},
    {"name": "b", "type": "clip"},
    {"name": "c", "type": "clip"},
    {"name": "ab", "type": "lerp", "param_name": "speed", "in": ["a", "b"]},
    {"name": "abc", "type": "lerp", "param_name": "aim", "in": ["ab", "c"]},
    {"name": "ba", "type": "lerp", "param_name": "speed", "in": ["b", "a"]}
  ],
  "states": {"move": {"tree": "abc"}, "back": {"tree": "ba"}}
}`)
	graph := out.Graph

	if len(graph.Parameters) != 2 || graph.Parameters[0] != stringhash.StringToHash32("speed") || graph.Parameters[1] != stringhash.StringToHash32("aim") {
		t.Fatalf("got parameters %v, want the keys of speed and aim", graph.Parameters)
	}
	checkNames(t, "parameters", out.Debug.Parameters, []string{"speed", "aim"})
	if graph.Nodes[3].Parameter != 0 || graph.Nodes[4].Parameter != 1 || graph.Nodes[5].Parameter != 0 {
		t.Errorf("unexpected parameter indices: %d, %d, %d", graph.Nodes[3].Parameter, graph.Nodes[4].Parameter, graph.Nodes[5].Parameter)
	}

	// NOTE: A lerp node can take another lerp node, and the order of "in" is kept.
	if graph.Nodes[4].Input0 != 3 || graph.Nodes[4].Input1 != 2 {
		t.Errorf("unexpected inputs of abc: %+v", graph.Nodes[4])
	}
	if graph.Nodes[5].Input0 != 1 || graph.Nodes[5].Input1 != 0 {
		t.Errorf("unexpected inputs of ba: %+v", graph.Nodes[5])
	}
}

func TestConvertIsDeterministic(t *testing.T) {
	first := string(packGraph(mustConvertJSON(t, sampleJSON).Graph))
	for i := 0; i < 20; i++ {
		if again := string(packGraph(mustConvertJSON(t, sampleJSON).Graph)); again != first {
			t.Fatalf("conversion #%d produced different bytes", i)
		}
	}
}

func TestConvertErrors(t *testing.T) {
	cases := []struct {
		name    string
		source  string
		wantErr string
	}{
		{
			"no states",
			`{"nodes": [{"name": "a", "type": "clip"}], "states": {}}`,
			"at least one state",
		},
		{
			"missing states",
			`{"nodes": [{"name": "a", "type": "clip"}]}`,
			"at least one state",
		},
		{
			"states as an array",
			`{"nodes": [{"name": "a", "type": "clip"}], "states": [{"tree": "a"}]}`,
			`"states" must be an object`,
		},
		{
			"duplicate state",
			`{"nodes": [{"name": "a", "type": "clip"}], "states": {"s": {"tree": "a"}, "s": {"tree": "a"}}}`,
			`state "s" is defined more than once`,
		},
		{
			"state without a tree",
			`{"nodes": [{"name": "a", "type": "clip"}], "states": {"s": {}}}`,
			`state "s" has no "tree"`,
		},
		{
			"state with an unknown node",
			`{"nodes": [{"name": "a", "type": "clip"}], "states": {"s": {"tree": "b"}}}`,
			`state "s": unknown node "b"`,
		},
		{
			"state with an unknown field",
			`{"nodes": [{"name": "a", "type": "clip"}], "states": {"s": {"tree": "a", "loop": true}}}`,
			`unknown field "loop"`,
		},
		{
			"node without a name",
			`{"nodes": [{"type": "clip"}], "states": {"s": {"tree": "a"}}}`,
			"node #0 has no name",
		},
		{
			"duplicate node",
			`{"nodes": [{"name": "a", "type": "clip"}, {"name": "a", "type": "clip"}], "states": {"s": {"tree": "a"}}}`,
			`node "a" is defined more than once`,
		},
		{
			"unknown node type",
			`{"nodes": [{"name": "a", "type": "additive"}], "states": {"s": {"tree": "a"}}}`,
			`unknown type "additive"`,
		},
		{
			"node with an unknown field",
			`{"nodes": [{"name": "a", "type": "clip", "param-name": "x"}], "states": {"s": {"tree": "a"}}}`,
			`unknown field "param-name"`,
		},
		{
			"clip node with inputs",
			`{"nodes": [{"name": "a", "type": "clip", "in": ["a"]}], "states": {"s": {"tree": "a"}}}`,
			`a clip node takes neither "in" nor "param_name"`,
		},
		{
			"lerp node with one input",
			`{"nodes": [{"name": "a", "type": "clip"}, {"name": "l", "type": "lerp", "param_name": "w", "in": ["a"]}], "states": {"s": {"tree": "l"}}}`,
			`"in" has 1`,
		},
		{
			"lerp node without a parameter",
			`{"nodes": [{"name": "a", "type": "clip"}, {"name": "l", "type": "lerp", "in": ["a", "a"]}], "states": {"s": {"tree": "l"}}}`,
			`a lerp node needs "param_name"`,
		},
		{
			"lerp node with an unknown input",
			`{"nodes": [{"name": "a", "type": "clip"}, {"name": "l", "type": "lerp", "param_name": "w", "in": ["a", "b"]}], "states": {"s": {"tree": "l"}}}`,
			`node "l": unknown input "b"`,
		},
		{
			"cycle",
			`{"nodes": [{"name": "a", "type": "clip"}, {"name": "x", "type": "lerp", "param_name": "w", "in": ["a", "y"]}, {"name": "y", "type": "lerp", "param_name": "w", "in": ["x", "a"]}], "states": {"s": {"tree": "x"}}}`,
			"nodes form a cycle: x -> y -> x",
		},
		{
			"node that takes itself",
			`{"nodes": [{"name": "x", "type": "lerp", "param_name": "w", "in": ["x", "x"]}], "states": {"s": {"tree": "x"}}}`,
			"nodes form a cycle: x -> x",
		},
	}

	for _, tc := range cases {
		t.Run(tc.name, func(t *testing.T) {
			_, err := convertJSON(t, tc.source)
			if err == nil {
				t.Fatalf("convert() succeeded, want an error with %q", tc.wantErr)
			}
			if !strings.Contains(err.Error(), tc.wantErr) {
				t.Errorf("got error %q, want it to have %q", err.Error(), tc.wantErr)
			}
		})
	}
}

func TestConvertLimitsTreeSize(t *testing.T) {
	// NOTE: Each lerp node takes the previous node twice, so the tree under
	// the node at depth n has 2^(n+1) - 1 nodes although the graph has n + 1.
	makeSource := func(depth int) string {
		nodes := []string{`{"name": "n0", "type": "clip"}`}
		for i := 1; i <= depth; i++ {
			nodes = append(nodes, fmt.Sprintf(`{"name": "n%d", "type": "lerp", "param_name": "w", "in": ["n%d", "n%d"]}`, i, i-1, i-1))
		}
		return fmt.Sprintf(`{"nodes": [%s], "states": {"s": {"tree": "n%d"}}}`, strings.Join(nodes, ", "), depth)
	}

	// NOTE: 2^10 - 1 = 1023 nodes fit, and 2^11 - 1 = 2047 nodes do not.
	if _, err := convertJSON(t, makeSource(9)); err != nil {
		t.Errorf("convert() failed for a tree of 1023 nodes: %v", err)
	}
	_, err := convertJSON(t, makeSource(10))
	if err == nil || !strings.Contains(err.Error(), "more than 1024 nodes") {
		t.Errorf("got error %v, want one about the size of the tree", err)
	}

	// NOTE: The size must not overflow for a deep graph either.
	_, err = convertJSON(t, makeSource(200))
	if err == nil || !strings.Contains(err.Error(), "more than 1024 nodes") {
		t.Errorf("got error %v, want one about the size of the tree", err)
	}
}

func TestRunWritesFiles(t *testing.T) {
	dir := t.TempDir()
	opts := &options{
		InFile:   filepath.Join(dir, "in.json"),
		OutFile:  filepath.Join(dir, "out", "model.animgraph_skel2d"),
		OutDebug: filepath.Join(dir, "debug", "model.animgraph_skel2d-debug"),
	}
	if err := os.WriteFile(opts.InFile, []byte(sampleJSON), 0o644); err != nil {
		t.Fatalf("os.WriteFile() failed: %v", err)
	}
	if err := run(opts); err != nil {
		t.Fatalf("run() failed: %v", err)
	}

	read := func(path string) []byte {
		t.Helper()
		data, err := os.ReadFile(path)
		if err != nil {
			t.Fatalf("os.ReadFile() failed: %v", err)
		}
		return data
	}

	graph := read(opts.OutFile)
	root := schemas.GetRootAsSkeletal2DAnimGraph(graph, 0)
	if root.NodesLength() != 3 || root.StatesLength() != 2 || root.ParametersLength() != 1 {
		t.Errorf("got %d nodes, %d states, %d parameters, want 3, 2, 1", root.NodesLength(), root.StatesLength(), root.ParametersLength())
	}

	debug := read(opts.OutDebug)
	if got := schemas.GetRootAsSkeletal2DAnimGraphDebug(debug, 0).NodesLength(); got != 3 {
		t.Errorf("got %d node names, want 3", got)
	}

	t.Run("the debug file is optional", func(t *testing.T) {
		opts := *opts
		opts.OutFile = filepath.Join(dir, "nodebug", "model.animgraph_skel2d")
		opts.OutDebug = ""
		if err := run(&opts); err != nil {
			t.Fatalf("run() failed: %v", err)
		}
		entries, err := os.ReadDir(filepath.Join(dir, "nodebug"))
		if err != nil {
			t.Fatalf("os.ReadDir() failed: %v", err)
		}
		if len(entries) != 1 {
			t.Errorf("got %d files, want 1", len(entries))
		}
	})

	t.Run("a missing input fails", func(t *testing.T) {
		opts := *opts
		opts.InFile = filepath.Join(dir, "missing.json")
		if err := run(&opts); err == nil {
			t.Errorf("run() succeeded, want an error")
		}
	})
}

func TestKeyTableDetectsCollisions(t *testing.T) {
	keys := newKeyTable("state")
	key, err := keys.add("Run")
	if err != nil || key != stringhash.StringToHash32("Run") {
		t.Fatalf("add() = %#x, %v, want the key of Run", key, err)
	}

	// NOTE: Adding the same name again is not a collision.
	if _, err := keys.add("Run"); err != nil {
		t.Errorf("add() failed for the same name: %v", err)
	}

	// NOTE: Forge a collision, since no short names are known to collide.
	keys.names[stringhash.StringToHash32("Idle")] = "Walk"
	_, err = keys.add("Idle")
	if err == nil || !strings.Contains(err.Error(), `hash collision`) || !strings.Contains(err.Error(), `state "Walk" and "Idle"`) {
		t.Errorf("got error %v, want a hash collision between Walk and Idle", err)
	}
}

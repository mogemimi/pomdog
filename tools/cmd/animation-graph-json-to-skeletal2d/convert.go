// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"errors"
	"fmt"
	"math"
	"strings"

	schemas "github.com/mogemimi/pomdog/build/schemas"
	stringhash "github.com/mogemimi/pomdog/tools/pkg/stringhash"
)

const (
	// maxIndexedCount is the number of nodes or parameters that a 16-bit
	// index can address.
	maxIndexedCount = math.MaxUint16

	// NOTE: schemas/skeletal2d_animgraph.fbs limits a blend tree to this number
	// of nodes, counting a shared node once for each reference to it.
	maxTreeNodeCount = 1024
)

// output holds the root table of each output file.
type output struct {
	Graph *schemas.Skeletal2DAnimGraphT
	Debug *schemas.Skeletal2DAnimGraphDebugT
}

// convert resolves the node names of doc to indices, and the clip, state, and
// parameter names to hash keys, and returns the tables to write.
func convert(doc *graphDocument) (*output, error) {
	nodesByName := map[string]*graphNode{}
	for i, node := range doc.Nodes {
		if node == nil || node.Name == "" {
			return nil, fmt.Errorf("node #%d has no name", i)
		}
		if _, ok := nodesByName[node.Name]; ok {
			return nil, fmt.Errorf("node %q is defined more than once", node.Name)
		}

		switch node.Type {
		case "clip":
			if len(node.In) != 0 || node.ParamName != nil {
				return nil, fmt.Errorf(`node %q: a clip node takes neither "in" nor "param_name"`, node.Name)
			}
		case "lerp":
			if len(node.In) != 2 {
				return nil, fmt.Errorf(`node %q: a lerp node blends two nodes, but "in" has %d`, node.Name, len(node.In))
			}
			if node.ParamName == nil || *node.ParamName == "" {
				return nil, fmt.Errorf(`node %q: a lerp node needs "param_name"`, node.Name)
			}
		default:
			return nil, fmt.Errorf("node %q: unknown type %q", node.Name, node.Type)
		}

		nodesByName[node.Name] = node
	}

	for _, node := range doc.Nodes {
		for _, input := range node.In {
			if _, ok := nodesByName[input]; !ok {
				return nil, fmt.Errorf("node %q: unknown input %q", node.Name, input)
			}
		}
	}

	if len(doc.States) == 0 {
		return nil, errors.New(`"states" must have at least one state`)
	}
	for _, state := range doc.States {
		if _, ok := nodesByName[state.Tree]; !ok {
			return nil, fmt.Errorf("state %q: unknown node %q", state.Name, state.Tree)
		}
	}

	sorted, err := sortNodes(doc.Nodes, nodesByName)
	if err != nil {
		return nil, err
	}
	if len(sorted) > maxIndexedCount {
		return nil, fmt.Errorf(`"nodes" has %d nodes, but the limit is %d`, len(sorted), maxIndexedCount)
	}

	out := &output{
		Graph: &schemas.Skeletal2DAnimGraphT{},
		Debug: &schemas.Skeletal2DAnimGraphDebugT{},
	}

	nodeIndices := map[string]uint16{}
	parameterIndices := map[string]uint16{}
	clipKeys := newKeyTable("clip")
	stateKeys := newKeyTable("state")
	parameterKeys := newKeyTable("parameter")

	// NOTE: treeSizes counts the nodes of the tree under each node, which the
	// runtime creates one by one.
	treeSizes := make([]int, len(sorted))

	for i, node := range sorted {
		nodeIndices[node.Name] = uint16(i)
		out.Debug.Nodes = append(out.Debug.Nodes, node.Name)

		if node.Type == "clip" {
			key, err := clipKeys.add(node.Name)
			if err != nil {
				return nil, err
			}

			treeSizes[i] = 1
			out.Graph.Nodes = append(out.Graph.Nodes, &schemas.Skeletal2DAnimGraphNodeT{
				Type:    schemas.Skeletal2DAnimGraphNodeTypeClip,
				ClipKey: key,
			})
			continue
		}

		// NOTE: Lerp nodes with the same parameter name share the parameter.
		parameter, ok := parameterIndices[*node.ParamName]
		if !ok {
			if len(out.Graph.Parameters) >= maxIndexedCount {
				return nil, fmt.Errorf("the graph has more than %d parameters", maxIndexedCount)
			}
			key, err := parameterKeys.add(*node.ParamName)
			if err != nil {
				return nil, err
			}
			parameter = uint16(len(out.Graph.Parameters))
			parameterIndices[*node.ParamName] = parameter
			out.Graph.Parameters = append(out.Graph.Parameters, key)
			out.Debug.Parameters = append(out.Debug.Parameters, &schemas.Skeletal2DAnimGraphNameDebugT{Key: key, Name: *node.ParamName})
		}

		input0 := nodeIndices[node.In[0]]
		input1 := nodeIndices[node.In[1]]

		// NOTE: Clamp the size so that shared nodes cannot overflow it.
		treeSizes[i] = min(1+treeSizes[input0]+treeSizes[input1], maxTreeNodeCount+1)
		out.Graph.Nodes = append(out.Graph.Nodes, &schemas.Skeletal2DAnimGraphNodeT{
			Type:      schemas.Skeletal2DAnimGraphNodeTypeLerp,
			Input0:    input0,
			Input1:    input1,
			Parameter: parameter,
		})
	}

	// NOTE: Keep the states in the order of the source, since the first state
	// is the initial state.
	for _, state := range doc.States {
		tree := nodeIndices[state.Tree]
		if treeSizes[tree] > maxTreeNodeCount {
			return nil, fmt.Errorf("state %q: the blend tree has more than %d nodes", state.Name, maxTreeNodeCount)
		}
		key, err := stateKeys.add(state.Name)
		if err != nil {
			return nil, err
		}
		out.Graph.States = append(out.Graph.States, &schemas.Skeletal2DAnimGraphStateT{
			Key:  key,
			Tree: tree,
		})
		out.Debug.States = append(out.Debug.States, &schemas.Skeletal2DAnimGraphNameDebugT{Key: key, Name: state.Name})
	}

	return out, nil
}

// keyTable maps the hash keys of one kind of name back to the names, and
// detects hash collisions.
type keyTable struct {
	kind  string
	names map[uint32]string
}

func newKeyTable(kind string) *keyTable {
	return &keyTable{kind: kind, names: map[uint32]string{}}
}

// add returns the XXH32 hash of name, or an error when another name of the
// table has the same hash.
func (t *keyTable) add(name string) (uint32, error) {
	key := stringhash.StringToHash32(name)
	if other, ok := t.names[key]; ok && other != name {
		return 0, fmt.Errorf("hash collision: XXH32 key %#x for %s %q and %q", key, t.kind, other, name)
	}
	t.names[key] = name
	return key, nil
}

// sortNodes returns the nodes ordered so that the inputs of a node precede
// it, which otherwise keeps the order of the source. It fails when the nodes
// form a cycle.
func sortNodes(nodes []*graphNode, nodesByName map[string]*graphNode) ([]*graphNode, error) {
	const (
		unvisited = iota
		visiting
		visited
	)
	marks := map[*graphNode]int{}
	sorted := make([]*graphNode, 0, len(nodes))
	path := []string{}

	var visit func(node *graphNode) error
	visit = func(node *graphNode) error {
		switch marks[node] {
		case visited:
			return nil
		case visiting:
			return fmt.Errorf("nodes form a cycle: %s", strings.Join(append(path, node.Name), " -> "))
		}

		marks[node] = visiting
		path = append(path, node.Name)
		for _, input := range node.In {
			if err := visit(nodesByName[input]); err != nil {
				return err
			}
		}
		path = path[:len(path)-1]
		marks[node] = visited

		sorted = append(sorted, node)
		return nil
	}

	for _, node := range nodes {
		if err := visit(node); err != nil {
			return nil, err
		}
	}
	return sorted, nil
}

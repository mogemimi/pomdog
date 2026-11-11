// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"bytes"
	"encoding/json"
	"errors"
	"fmt"
)

// graphDocument is an animation graph in its JSON source format.
type graphDocument struct {
	Nodes  []*graphNode `json:"nodes"`
	States graphStates  `json:"states"`
}

// graphNode is a node of a blend tree. A "clip" node plays the animation clip
// that has the name of the node. A "lerp" node blends the two nodes that "in"
// names by the parameter that "param_name" names.
type graphNode struct {
	Name      string   `json:"name"`
	Type      string   `json:"type"`
	ParamName *string  `json:"param_name"`
	In        []string `json:"in"`
}

// graphState is a state that plays the blend tree whose root node "tree" names.
type graphState struct {
	Name string
	Tree string
}

// graphStates holds the states of the "states" object in the order of the
// source, because the first state is the initial state of the graph. A Go map
// would lose that order.
type graphStates []*graphState

func (states *graphStates) UnmarshalJSON(data []byte) error {
	decoder := json.NewDecoder(bytes.NewReader(data))

	if token, err := decoder.Token(); err != nil {
		return err
	} else if token != json.Delim('{') {
		return errors.New(`"states" must be an object`)
	}

	seen := map[string]bool{}
	for decoder.More() {
		token, err := decoder.Token()
		if err != nil {
			return err
		}
		name, ok := token.(string)
		if !ok || name == "" {
			return errors.New("a state has no name")
		}
		if seen[name] {
			return fmt.Errorf("state %q is defined more than once", name)
		}
		seen[name] = true

		var raw json.RawMessage
		if err := decoder.Decode(&raw); err != nil {
			return err
		}
		body := struct {
			Tree *string `json:"tree"`
		}{}
		if err := decodeStrict(raw, &body); err != nil {
			return fmt.Errorf("state %q: %w", name, err)
		}
		if body.Tree == nil || *body.Tree == "" {
			return fmt.Errorf(`state %q has no "tree"`, name)
		}

		*states = append(*states, &graphState{Name: name, Tree: *body.Tree})
	}

	// NOTE: Consume the closing brace.
	if _, err := decoder.Token(); err != nil {
		return err
	}
	return nil
}

// parseGraphJSON decodes an animation graph. It rejects unknown fields, so
// that a misspelled field does not go unnoticed.
func parseGraphJSON(data []byte) (*graphDocument, error) {
	doc := &graphDocument{}
	if err := decodeStrict(data, doc); err != nil {
		return nil, err
	}
	return doc, nil
}

// decodeStrict decodes a JSON value that has only the fields of v.
func decodeStrict(data []byte, v any) error {
	decoder := json.NewDecoder(bytes.NewReader(data))
	decoder.DisallowUnknownFields()
	if err := decoder.Decode(v); err != nil {
		return err
	}
	if decoder.More() {
		return errors.New("unexpected data after the JSON value")
	}
	return nil
}

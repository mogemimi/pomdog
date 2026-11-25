// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"os"
	"path/filepath"
	"strings"
	"testing"

	spvparse "github.com/mogemimi/pomdog/tools/pkg/spvparse"
)

const opEntryPoint = 15

// writeModule writes a SPIR-V module that declares only the given entry points.
func writeModule(t *testing.T, entryPoints ...spvparse.EntryPoint) string {
	t.Helper()
	words := []uint32{0x07230203, 0x00010000, 0, 16, 0}
	for _, ep := range entryPoints {
		name := spvparse.EncodeString(ep.Name)
		words = append(words, uint32(3+len(name))<<16|opEntryPoint, uint32(ep.ExecutionModel), ep.FunctionID)
		words = append(words, name...)
	}
	path := filepath.Join(t.TempDir(), "shader.spv")
	if err := os.WriteFile(path, spvparse.FromWords(words), 0o644); err != nil {
		t.Fatalf("os.WriteFile() failed: %v", err)
	}
	return path
}

func TestRun(t *testing.T) {
	vertexMain := spvparse.EntryPoint{Name: "main", FunctionID: 4, ExecutionModel: spvparse.ExecutionModelVertex}
	fragmentMain := spvparse.EntryPoint{Name: "main", FunctionID: 5, ExecutionModel: spvparse.ExecutionModelFragment}
	vertexOther := spvparse.EntryPoint{Name: "other_vs", FunctionID: 6, ExecutionModel: spvparse.ExecutionModelVertex}

	tests := []struct {
		name        string
		entryPoints []spvparse.EntryPoint
		stage       string
		wantErr     string
	}{
		{name: "one vertex entry point", entryPoints: []spvparse.EntryPoint{vertexMain}, stage: "vertex"},
		{name: "one fragment entry point", entryPoints: []spvparse.EntryPoint{fragmentMain}, stage: "fragment"},
		{name: "no entry point", stage: "vertex", wantErr: "declares 0 entry points (none)"},
		{name: "two entry points of one stage", entryPoints: []spvparse.EntryPoint{vertexMain, vertexOther}, stage: "vertex", wantErr: "declares 2 entry points (vertex 'main', vertex 'other_vs')"},
		{name: "two entry points of two stages", entryPoints: []spvparse.EntryPoint{vertexMain, fragmentMain}, stage: "fragment", wantErr: "declares 2 entry points"},
		{name: "wrong stage", entryPoints: []spvparse.EntryPoint{fragmentMain}, stage: "vertex", wantErr: "is a fragment shader, want a vertex shader"},
		{name: "unknown stage", entryPoints: []spvparse.EntryPoint{vertexMain}, stage: "pixel", wantErr: "unknown stage"},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			path := writeModule(t, tt.entryPoints...)
			ep, err := run(path, tt.stage)
			if tt.wantErr == "" {
				if err != nil {
					t.Fatalf("run() failed: %v", err)
				}
				if ep.Name != tt.entryPoints[0].Name {
					t.Errorf("entry point = %q, want %q", ep.Name, tt.entryPoints[0].Name)
				}
				return
			}
			if err == nil {
				t.Fatalf("run() succeeded, want an error containing %q", tt.wantErr)
			}
			if !strings.Contains(err.Error(), tt.wantErr) {
				t.Errorf("run() error = %q, want it to contain %q", err.Error(), tt.wantErr)
			}
		})
	}
}

// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"encoding/json"
	"errors"
	"flag"
	"fmt"
	"io/fs"
	"os"
	"strings"

	spvparse "github.com/mogemimi/pomdog/tools/pkg/spvparse"
)

// ValidationResult holds the structured output of entry point validation.
type ValidationResult struct {
	Input      string `json:"input"`
	Stage      string `json:"stage"`
	EntryPoint string `json:"entry_point"`
	OK         bool   `json:"ok"`
}

func main() {
	inPath := flag.String("i", "", "path to the input SPIR-V file")
	stage := flag.String("stage", "", "expected shader stage (vertex or fragment)")
	stampPath := flag.String("stamp", "", "path to output stamp file (created on success)")
	flag.Parse()

	if *inPath == "" || *stage == "" {
		fmt.Fprintln(os.Stderr, "usage: spirv-entry-point-validate -i <in.spv> -stage <vertex|fragment> [-stamp <out.stamp>]")
		os.Exit(1)
	}

	// NOTE: Remove the stamp of an earlier success first, so that a failure never
	// leaves a stamp that claims the current input is valid.
	if *stampPath != "" {
		if err := os.Remove(*stampPath); err != nil && !errors.Is(err, fs.ErrNotExist) {
			fmt.Fprintf(os.Stderr, "error: failed to remove stamp file %s: %v\n", *stampPath, err)
			os.Exit(1)
		}
	}

	entryPoint, err := run(*inPath, *stage)
	if err != nil {
		fmt.Fprintf(os.Stderr, "error: %s: %v\n", *inPath, err)
		os.Exit(1)
	}

	if *stampPath != "" {
		result := &ValidationResult{
			Input:      *inPath,
			Stage:      *stage,
			EntryPoint: entryPoint.Name,
			OK:         true,
		}
		data, err := json.MarshalIndent(result, "", "  ")
		if err != nil {
			fmt.Fprintf(os.Stderr, "error: failed to marshal JSON: %v\n", err)
			os.Exit(1)
		}
		data = append(data, '\n')
		if err := os.WriteFile(*stampPath, data, 0644); err != nil {
			fmt.Fprintf(os.Stderr, "error: failed to write stamp file %s: %v\n", *stampPath, err)
			os.Exit(1)
		}
	}
}

func run(inPath, stage string) (spvparse.EntryPoint, error) {
	want, err := parseStage(stage)
	if err != nil {
		return spvparse.EntryPoint{}, err
	}

	module, err := spvparse.ParseFile(inPath)
	if err != nil {
		return spvparse.EntryPoint{}, fmt.Errorf("failed to parse SPIR-V: %w", err)
	}

	return validate(module.EntryPoints(), want)
}

// parseStage converts a stage name into the SPIR-V execution model of the stage.
func parseStage(stage string) (spvparse.ExecutionModel, error) {
	switch stage {
	case "vertex":
		return spvparse.ExecutionModelVertex, nil
	case "fragment":
		return spvparse.ExecutionModelFragment, nil
	}
	return 0, fmt.Errorf("unknown stage %q, want vertex or fragment", stage)
}

// validate checks that a module declares exactly one entry point and that it runs
// in the expected stage. The Metal backend takes the only function of a .metallib
// without a name, and spirv-cross picks an entry point by stage, so a module with
// more than one entry point makes the shipped function ambiguous.
func validate(entryPoints []spvparse.EntryPoint, want spvparse.ExecutionModel) (spvparse.EntryPoint, error) {
	if len(entryPoints) != 1 {
		names := make([]string, 0, len(entryPoints))
		for _, ep := range entryPoints {
			names = append(names, fmt.Sprintf("%s '%s'", ep.ExecutionModel, ep.Name))
		}
		found := "none"
		if len(names) > 0 {
			found = strings.Join(names, ", ")
		}
		return spvparse.EntryPoint{}, fmt.Errorf("the module declares %d entry points (%s), want exactly one", len(entryPoints), found)
	}

	ep := entryPoints[0]
	if ep.ExecutionModel != want {
		return spvparse.EntryPoint{}, fmt.Errorf("entry point '%s' is a %s shader, want a %s shader", ep.Name, ep.ExecutionModel, want)
	}
	return ep, nil
}

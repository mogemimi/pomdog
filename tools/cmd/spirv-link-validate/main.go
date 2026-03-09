// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"encoding/json"
	"flag"
	"fmt"
	"io"
	"os"
	"sort"

	spvparse "github.com/mogemimi/pomdog/tools/pkg/spvparse"
)

// ValidationResult holds the structured output of link validation.
type ValidationResult struct {
	VS     string   `json:"vs"`
	PS     string   `json:"ps"`
	OK     bool     `json:"ok"`
	Errors []string `json:"errors,omitempty"`
}

func main() {
	vsPath := flag.String("vs", "", "path to vertex shader SPIR-V file")
	psPath := flag.String("ps", "", "path to pixel/fragment shader SPIR-V file")
	stampPath := flag.String("stamp", "", "path to output stamp file (created on success)")
	flag.Parse()

	if *vsPath == "" || *psPath == "" {
		fmt.Fprintln(os.Stderr, "usage: spirv-link-validate -vs <vs.spv> -ps <ps.spv> [-stamp <out.stamp>]")
		os.Exit(1)
	}

	ok := validate(*vsPath, *psPath, os.Stderr)

	if ok && *stampPath != "" {
		result := &ValidationResult{
			VS: *vsPath,
			PS: *psPath,
			OK: true,
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

	if !ok {
		os.Exit(1)
	}
}

func validate(vsPath, psPath string, w io.Writer) bool {
	vsMod, err := spvparse.ParseFile(vsPath)
	if err != nil {
		fmt.Fprintf(w, "error: failed to parse VS SPIR-V %s: %v\n", vsPath, err)
		return false
	}

	psMod, err := spvparse.ParseFile(psPath)
	if err != nil {
		fmt.Fprintf(w, "error: failed to parse PS SPIR-V %s: %v\n", psPath, err)
		return false
	}

	vsOutputs := vsMod.CollectInterfaceVars(spvparse.StorageClassOutput)
	psInputs := psMod.CollectInterfaceVars(spvparse.StorageClassInput)

	// NOTE: Build location map for VS outputs
	vsOutputMap := make(map[int]spvparse.InterfaceVar)
	for _, v := range vsOutputs {
		vsOutputMap[v.Location] = v
	}

	errors := 0

	// NOTE: Check that every PS input has a matching VS output
	for _, psIn := range psInputs {
		vsOut, found := vsOutputMap[psIn.Location]
		if !found {
			fmt.Fprintf(w, "error: PS input '%s' at location %d has no matching VS output\n",
				psIn.Name, psIn.Location)
			errors++
			continue
		}

		if vsOut.TypeDesc != psIn.TypeDesc {
			fmt.Fprintf(w, "error: type mismatch at location %d: VS output '%s' is %s, PS input '%s' is %s\n",
				psIn.Location, vsOut.Name, vsOut.TypeDesc, psIn.Name, psIn.TypeDesc)
			errors++
		}
	}

	// NOTE: Warn about VS outputs not consumed by PS (informational, not an error)
	psInputMap := make(map[int]bool)
	for _, v := range psInputs {
		psInputMap[v.Location] = true
	}

	var unconsumedLocations []int
	for _, v := range vsOutputs {
		if !psInputMap[v.Location] {
			unconsumedLocations = append(unconsumedLocations, v.Location)
		}
	}
	sort.Ints(unconsumedLocations)
	for _, loc := range unconsumedLocations {
		v := vsOutputMap[loc]
		fmt.Fprintf(w, "warning: VS output '%s' (%s) at location %d is not consumed by PS\n",
			v.Name, v.TypeDesc, v.Location)
	}

	if errors > 0 {
		fmt.Fprintf(w, "link validation failed: %d error(s)\n", errors)
		return false
	}

	return true
}

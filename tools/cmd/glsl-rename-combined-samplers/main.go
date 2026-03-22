// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"encoding/json"
	"flag"
	"fmt"
	"log"
	"os"
	"os/exec"
	"regexp"
	"sort"
	"strings"

	spirvreflect "github.com/mogemimi/pomdog/tools/pkg/spirvreflect"
)

type Env struct {
	InGLSL     string
	InSPIRV    string
	OutFile    string
	SPIRVCross string
}

func main() {
	env := &Env{}
	flag.StringVar(&env.InGLSL, "i", "", "input GLSL file")
	flag.StringVar(&env.InSPIRV, "spv", "", "input SPIR-V file (*.spv)")
	flag.StringVar(&env.OutFile, "o", "", "output GLSL file")
	flag.StringVar(&env.SPIRVCross, "spirvcross", "", "specify spirv-cross executable")
	flag.Parse()

	if err := run(env); err != nil {
		log.Fatal(err)
	}
}

func run(env *Env) error {
	// NOTE: Get separate_images and separate_samplers from SPIR-V reflection.
	cmd := exec.Command(env.SPIRVCross, env.InSPIRV, "--reflect")

	var out strings.Builder
	cmd.Stdout = &out
	cmd.Stderr = os.Stderr

	if err := cmd.Run(); err != nil {
		return fmt.Errorf("spirv-cross --reflect failed: %w", err)
	}

	info := &spirvreflect.Reflect{}
	if err := json.Unmarshal([]byte(out.String()), info); err != nil {
		return fmt.Errorf("json.Unmarshal() failed: %w", err)
	}

	// NOTE: Read the input GLSL file.
	glslBytes, err := os.ReadFile(env.InGLSL)
	if err != nil {
		return fmt.Errorf("os.ReadFile() failed: %w", err)
	}
	glsl := string(glslBytes)

	// NOTE: Find all SPIRV_Cross_Combined* identifiers in the GLSL.
	re := regexp.MustCompile(`SPIRV_Cross_Combined\w+`)
	combinedNames := re.FindAllString(glsl, -1)

	// NOTE: Deduplicate.
	seen := make(map[string]bool)
	var uniqueNames []string
	for _, name := range combinedNames {
		if !seen[name] {
			seen[name] = true
			uniqueNames = append(uniqueNames, name)
		}
	}

	// NOTE: Build replacement map by matching combined names against known image+sampler pairs.
	imageNames := make([]string, 0, len(info.SeparateImages))
	for _, img := range info.SeparateImages {
		imageNames = append(imageNames, img.Name)
	}
	samplerNames := make([]string, 0, len(info.SeparateSamplers))
	for _, smp := range info.SeparateSamplers {
		samplerNames = append(samplerNames, smp.Name)
	}

	// NOTE: Sort image names by length descending to prefer longer (more specific) matches.
	sort.SliceStable(imageNames, func(i, j int) bool {
		return len(imageNames[i]) > len(imageNames[j])
	})

	replacements := make(map[string]string)
	for _, combined := range uniqueNames {
		suffix := strings.TrimPrefix(combined, "SPIRV_Cross_Combined")
		matched := false
		for _, img := range imageNames {
			if strings.HasPrefix(suffix, img) {
				remainder := suffix[len(img):]
				for _, smp := range samplerNames {
					if remainder == smp {
						replacements[combined] = img
						matched = true
						break
					}
				}
				if matched {
					break
				}
			}
		}
	}

	// NOTE: Apply replacements.
	for combined, replacement := range replacements {
		glsl = strings.ReplaceAll(glsl, combined, replacement)
	}

	// NOTE: Write output.
	if err := os.WriteFile(env.OutFile, []byte(glsl), 0o644); err != nil {
		return fmt.Errorf("os.WriteFile() failed: %w", err)
	}

	return nil
}

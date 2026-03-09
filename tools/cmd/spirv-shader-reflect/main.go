// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"encoding/json"
	"flag"
	"fmt"
	"io/fs"
	"log"
	"os"
	"os/exec"
	"sort"
	"strings"

	flatbuffers "github.com/google/flatbuffers/go"
	schemas "github.com/mogemimi/pomdog/build/schemas"
	spirvreflect "github.com/mogemimi/pomdog/tools/pkg/spirvreflect"
	stringhash "github.com/mogemimi/pomdog/tools/pkg/stringhash"
)

type Env struct {
	InSPIRV      string
	OutFile      string
	OutDebugFile string
	SPIRVCross   string
}

func main() {
	env := &Env{}
	flag.StringVar(&env.InSPIRV, "i", "", "input SPIR-V file (*.spv)")
	flag.StringVar(&env.OutFile, "o", "", "output file")
	flag.StringVar(&env.OutDebugFile, "debug", "", "output debug file")
	flag.StringVar(&env.SPIRVCross, "spirvcross", "", "specify spirv-cross executable")
	flag.Parse()

	if err := run(env); err != nil {
		log.Fatal(err)
	}
}

func run(env *Env) error {
	cmd := exec.Command(env.SPIRVCross, env.InSPIRV, "--reflect")

	var out strings.Builder
	cmd.Stdout = &out
	cmd.Stderr = os.Stderr

	if err := cmd.Run(); err != nil {
		return fmt.Errorf("cmd.Run() failed: %w", err)
	}

	info := &spirvreflect.Reflect{}

	{
		jsonStr := out.String()
		if err := json.Unmarshal([]byte(jsonStr), info); err != nil {
			return fmt.Errorf("json.Unmarshal() failed: %w", err)
		}
	}

	reflect := schemas.ShaderReflectT{}
	debugInfo := schemas.ShaderReflectDebugT{}

	// NOTE: Please refer to D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT.
	const maxConstantBufferSlotCount = 14

	// NOTE: Track hash-to-name mapping to detect collisions.
	hashToName := make(map[uint32]string)

	for _, texture := range info.Textures {
		hash := stringhash.StringToHash32(texture.Name)
		if prev, exists := hashToName[hash]; exists && prev != texture.Name {
			return fmt.Errorf("hash collision: %q and %q both hash to 0x%08x", prev, texture.Name, hash)
		}
		hashToName[hash] = texture.Name

		sampler := &schemas.SamplerSlotT{
			Name: hash,
			Slot: byte(texture.Binding),
		}
		reflect.Samplers = append(reflect.Samplers, sampler)

		samplerDebug := &schemas.SamplerSlotDebugT{
			Name: texture.Name,
			Slot: byte(texture.Binding),
		}
		debugInfo.Samplers = append(debugInfo.Samplers, samplerDebug)
	}
	for _, ubo := range info.UBOs {
		if ubo.Binding >= maxConstantBufferSlotCount {
			return fmt.Errorf("ubo.Binding must be < maxConstantBufferSlotCount but %v", ubo.Binding)
		}

		hash := stringhash.StringToHash32(ubo.Name)
		if prev, exists := hashToName[hash]; exists && prev != ubo.Name {
			return fmt.Errorf("hash collision: %q and %q both hash to 0x%08x", prev, ubo.Name, hash)
		}
		hashToName[hash] = ubo.Name

		cbuffer := &schemas.ConstantBufferSlotT{
			Name: hash,
			Slot: byte(ubo.Binding),
		}
		reflect.Cbuffers = append(reflect.Cbuffers, cbuffer)

		cbufferDebug := &schemas.ConstantBufferSlotDebugT{
			Name: ubo.Name,
			Slot: byte(ubo.Binding),
		}
		debugInfo.Cbuffers = append(debugInfo.Cbuffers, cbufferDebug)
	}

	sort.SliceStable(reflect.Samplers, func(i, j int) bool {
		return reflect.Samplers[i].Name < reflect.Samplers[j].Name
	})
	sort.SliceStable(reflect.Cbuffers, func(i, j int) bool {
		return reflect.Cbuffers[i].Name < reflect.Cbuffers[j].Name
	})
	sort.SliceStable(debugInfo.Samplers, func(i, j int) bool {
		return debugInfo.Samplers[i].Name < debugInfo.Samplers[j].Name
	})
	sort.SliceStable(debugInfo.Cbuffers, func(i, j int) bool {
		return debugInfo.Cbuffers[i].Name < debugInfo.Cbuffers[j].Name
	})

	{
		builder := flatbuffers.NewBuilder(0)
		builder.Finish(reflect.Pack(builder))

		if err := os.WriteFile(env.OutFile, builder.FinishedBytes(), fs.ModePerm); err != nil {
			return fmt.Errorf("os.WriteFile() failed: %w", err)
		}
	}

	if env.OutDebugFile != "" {
		builder := flatbuffers.NewBuilder(0)
		builder.Finish(debugInfo.Pack(builder))

		if err := os.WriteFile(env.OutDebugFile, builder.FinishedBytes(), fs.ModePerm); err != nil {
			return fmt.Errorf("os.WriteFile() failed: %w", err)
		}
	}

	return nil
}

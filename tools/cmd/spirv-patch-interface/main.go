// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"flag"
	"fmt"
	"os"
	"sort"

	spvparse "github.com/mogemimi/pomdog/tools/pkg/spvparse"
)

// SPIR-V opcodes used for patching.
const (
	opName        = 5
	opEntryPoint  = 15
	opDecorate    = 71
	opTypeFloat   = 22
	opTypeVector  = 23
	opTypePointer = 32
	opVariable    = 59
)

// SPIR-V storage classes.
const (
	storageClassInput = 1
)

// SPIR-V decorations.
const (
	decorationLocation = 30
)

func main() {
	vsPath := flag.String("vs", "", "path to vertex shader SPIR-V file")
	psPath := flag.String("ps", "", "path to fragment shader SPIR-V file")
	outPath := flag.String("o", "", "output patched SPIR-V file path")
	verbose := flag.Bool("v", false, "verbose output")
	flag.Parse()

	if *vsPath == "" || *psPath == "" || *outPath == "" {
		fmt.Fprintln(os.Stderr, "usage: spirv-patch-interface -vs <vs.spv> -ps <ps.spv> -o <out.spv>")
		os.Exit(1)
	}

	if err := run(*vsPath, *psPath, *outPath, *verbose); err != nil {
		fmt.Fprintf(os.Stderr, "error: %v\n", err)
		os.Exit(1)
	}
}

func run(vsPath, psPath, outPath string, verbose bool) error {
	// Parse both modules to find interface variables.
	vsMod, err := spvparse.ParseFile(vsPath)
	if err != nil {
		return fmt.Errorf("failed to parse VS: %w", err)
	}
	psMod, err := spvparse.ParseFile(psPath)
	if err != nil {
		return fmt.Errorf("failed to parse PS: %w", err)
	}

	vsOutputs := vsMod.CollectInterfaceVars(spvparse.StorageClassOutput)
	psInputs := psMod.CollectInterfaceVars(spvparse.StorageClassInput)

	// Build set of PS input locations.
	psLocations := make(map[int]bool)
	for _, v := range psInputs {
		psLocations[v.Location] = true
	}

	// Find missing variables: VS outputs not consumed by PS inputs.
	type missingVar struct {
		Name     string
		Location int
		TypeDesc string
	}
	var missing []missingVar
	for _, v := range vsOutputs {
		if !psLocations[v.Location] {
			missing = append(missing, missingVar{
				Name:     v.Name,
				Location: v.Location,
				TypeDesc: v.TypeDesc,
			})
		}
	}

	if len(missing) == 0 {
		// NOTE: Nothing to patch, just copy.
		data, err := os.ReadFile(psPath)
		if err != nil {
			return fmt.Errorf("failed to read PS file: %w", err)
		}
		if verbose {
			fmt.Fprintln(os.Stdout, "no missing variables found, copying PS file")
		}
		return os.WriteFile(outPath, data, 0644)
	}

	// NOTE: Sort by location for deterministic output.
	sort.SliceStable(missing, func(i, j int) bool {
		return missing[i].Location < missing[j].Location
	})

	// NOTE: Read raw PS SPIR-V as words.
	psData, err := os.ReadFile(psPath)
	if err != nil {
		return fmt.Errorf("failed to read PS file: %w", err)
	}
	words := spvparse.ToWords(psData)

	if len(words) < 5 {
		return fmt.Errorf("SPIR-V too short")
	}

	bound := words[3]

	// NOTE: Scan existing types to find IDs we can reuse.
	typeFloat32ID := uint32(0)
	typeVec2ID := uint32(0)
	typeVec3ID := uint32(0)
	typeVec4ID := uint32(0)
	ptrInputVec2ID := uint32(0)
	ptrInputVec3ID := uint32(0)
	ptrInputVec4ID := uint32(0)
	ptrInputFloat32ID := uint32(0)

	i := 5
	for i < len(words) {
		wc := int(words[i] >> 16)
		op := words[i] & 0xFFFF
		if wc == 0 {
			break
		}

		switch op {
		case opTypeFloat:
			if wc >= 3 && words[i+2] == 32 {
				typeFloat32ID = words[i+1]
			}
		case opTypeVector:
			if wc >= 4 && words[i+2] == typeFloat32ID && typeFloat32ID != 0 {
				switch words[i+3] {
				case 2:
					typeVec2ID = words[i+1]
				case 3:
					typeVec3ID = words[i+1]
				case 4:
					typeVec4ID = words[i+1]
				}
			}
		case opTypePointer:
			if wc >= 4 && words[i+2] == storageClassInput {
				pointee := words[i+3]
				switch {
				case pointee == typeFloat32ID && typeFloat32ID != 0:
					ptrInputFloat32ID = words[i+1]
				case pointee == typeVec2ID && typeVec2ID != 0:
					ptrInputVec2ID = words[i+1]
				case pointee == typeVec3ID && typeVec3ID != 0:
					ptrInputVec3ID = words[i+1]
				case pointee == typeVec4ID && typeVec4ID != 0:
					ptrInputVec4ID = words[i+1]
				}
			}
		}

		i += wc
	}

	// NOTE: For each missing variable, determine the type IDs needed and allocate new IDs.
	type patchInfo struct {
		Name      string
		Location  int
		VarID     uint32
		PtrTypeID uint32 // Pointer<Input, T>
		PointeeID uint32 // T
	}

	var patches []patchInfo
	var newTypes [][]uint32 // new type instructions to insert

	for _, m := range missing {
		var pointeeID, ptrID uint32

		switch m.TypeDesc {
		case "float":
			pointeeID = typeFloat32ID
			ptrID = ptrInputFloat32ID
		case "float2":
			pointeeID = typeVec2ID
			ptrID = ptrInputVec2ID
		case "float3":
			pointeeID = typeVec3ID
			ptrID = ptrInputVec3ID
		case "float4":
			pointeeID = typeVec4ID
			ptrID = ptrInputVec4ID
		default:
			return fmt.Errorf("unsupported type %q for missing variable %q at location %d", m.TypeDesc, m.Name, m.Location)
		}

		// NOTE: If the base type doesn't exist, create it.
		if pointeeID == 0 {
			switch m.TypeDesc {
			case "float":
				typeFloat32ID = bound
				bound++
				newTypes = append(newTypes, []uint32{(3 << 16) | opTypeFloat, typeFloat32ID, 32})
				pointeeID = typeFloat32ID
			default:
				if typeFloat32ID == 0 {
					typeFloat32ID = bound
					bound++
					newTypes = append(newTypes, []uint32{(3 << 16) | opTypeFloat, typeFloat32ID, 32})
				}
				compCount := uint32(0)
				switch m.TypeDesc {
				case "float2":
					compCount = 2
				case "float3":
					compCount = 3
				case "float4":
					compCount = 4
				}
				vecID := bound
				bound++
				newTypes = append(newTypes, []uint32{(4 << 16) | opTypeVector, vecID, typeFloat32ID, compCount})
				pointeeID = vecID
				switch m.TypeDesc {
				case "float2":
					typeVec2ID = vecID
				case "float3":
					typeVec3ID = vecID
				case "float4":
					typeVec4ID = vecID
				}
			}
		}

		// NOTE: If the pointer type doesn't exist, create it.
		if ptrID == 0 {
			ptrID = bound
			bound++
			newTypes = append(newTypes, []uint32{(4 << 16) | opTypePointer, ptrID, storageClassInput, pointeeID})
			switch m.TypeDesc {
			case "float":
				ptrInputFloat32ID = ptrID
			case "float2":
				ptrInputVec2ID = ptrID
			case "float3":
				ptrInputVec3ID = ptrID
			case "float4":
				ptrInputVec4ID = ptrID
			}
		}

		varID := bound
		bound++

		patches = append(patches, patchInfo{
			Name:      m.Name,
			Location:  m.Location,
			VarID:     varID,
			PtrTypeID: ptrID,
			PointeeID: pointeeID,
		})
	}

	// NOTE: Now build the patched word stream.
	// Strategy: collect new instructions to insert at the right positions in the SPIR-V.
	//
	// SPIR-V layout order (relevant sections):
	//   1. OpCapability, OpExtension, OpExtInstImport, OpMemoryModel
	//   2. OpEntryPoint, OpExecutionMode
	//   3. OpName, OpMemberName (debug info)
	//   4. OpDecorate, OpMemberDecorate (annotations)
	//   5. Type declarations (OpType*)
	//   6. OpVariable (global scope)
	//   7. Function definitions
	//
	// We need to:
	//   a. Extend OpEntryPoint with new variable IDs
	//   b. Insert OpName instructions (in the debug section)
	//   c. Insert OpDecorate(Location) instructions (in the annotation section)
	//   d. Insert new type instructions (if any) (in the type section)
	//   e. Insert OpVariable instructions (in the global variable section)

	var result []uint32
	result = append(result, words[0:5]...) // NOTE: header

	entryPointPatched := false
	namesInserted := false
	decoratesInserted := false
	typesInserted := false
	variablesInserted := false

	i = 5
	for i < len(words) {
		wc := int(words[i] >> 16)
		op := words[i] & 0xFFFF
		if wc == 0 {
			break
		}

		// NOTE: a. Extend OpEntryPoint
		if op == opEntryPoint && !entryPointPatched {
			// NOTE: Copy the existing instruction and append new variable IDs.
			entryWords := make([]uint32, wc)
			copy(entryWords, words[i:i+wc])

			for _, p := range patches {
				entryWords = append(entryWords, p.VarID)
			}
			// NOTE: Update word count.
			entryWords[0] = (uint32(len(entryWords)) << 16) | opEntryPoint
			result = append(result, entryWords...)
			entryPointPatched = true
			i += wc
			continue
		}

		// NOTE: b. Insert OpName instructions before the first non-Name/non-MemberName/non-EntryPoint debug instruction.
		// We insert right after the last OpName or OpMemberName.
		if !namesInserted && op != opName && op != 6 /* OpMemberName */ && op != opEntryPoint && op != 14 /* OpExecutionMode */ && op != 16 /* OpExecutionModeId */ {
			for _, p := range patches {
				strWords := spvparse.EncodeString(p.Name)
				totalWords := 2 + len(strWords)
				nameInstr := make([]uint32, totalWords)
				nameInstr[0] = (uint32(totalWords) << 16) | opName
				nameInstr[1] = p.VarID
				copy(nameInstr[2:], strWords)
				result = append(result, nameInstr...)
			}
			namesInserted = true
		}

		// NOTE: c. Insert OpDecorate instructions before the first type declaration.
		if !decoratesInserted && isTypeOp(op) {
			for _, p := range patches {
				result = append(result, (4<<16)|opDecorate, p.VarID, decorationLocation, uint32(p.Location))
			}
			decoratesInserted = true
		}

		// NOTE: d. Insert new type instructions before the first OpVariable.
		if !typesInserted && op == opVariable {
			for _, t := range newTypes {
				result = append(result, t...)
			}
			typesInserted = true
		}

		// NOTE: e. Insert OpVariable instructions before the first function definition (OpFunction = 54).
		if !variablesInserted && op == 54 /* OpFunction */ {
			for _, p := range patches {
				result = append(result, (4<<16)|opVariable, p.PtrTypeID, p.VarID, storageClassInput)
			}
			variablesInserted = true
		}

		// NOTE: Copy the original instruction.
		result = append(result, words[i:i+wc]...)
		i += wc
	}

	// NOTE: Update the bound in the header.
	result[3] = bound

	// NOTE: Write output.
	outData := spvparse.FromWords(result)
	if err := os.WriteFile(outPath, outData, 0644); err != nil {
		return fmt.Errorf("failed to write output: %w", err)
	}

	if verbose {
		for _, p := range patches {
			fmt.Fprintf(os.Stdout, "patched: added PS input '%s' (%s) at location %d\n", p.Name, missing[0].TypeDesc, p.Location)
		}
	}

	return nil
}

// isTypeOp returns true if the opcode is a type declaration instruction.
func isTypeOp(op uint32) bool {
	switch op {
	case 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33:
		// NOTE: OpTypeVoid .. OpTypePointer, OpTypeFunction, etc.
		return true
	}
	return false
}

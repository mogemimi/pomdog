// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"flag"
	"fmt"
	"os"
	"strings"

	spvparse "github.com/mogemimi/pomdog/tools/pkg/spvparse"
)

// SPIR-V opcodes for debug instructions (Section 7.a of the SPIR-V spec).
const (
	opSourceContinued = 2
	opSource          = 3
	opSourceExtension = 4
	opName            = 5
	opMemberName      = 6
	opString          = 7
	opLine            = 8
	opNoLine          = 317
	opModuleProcessed = 330
)

// Additional opcodes for non-semantic handling.
const (
	opExtInstImport = 11
	opExtInst       = 12
)

func main() {
	inPath := flag.String("i", "", "input SPIR-V file path")
	outPath := flag.String("o", "", "output SPIR-V file path")
	stripDebug := flag.Bool("strip-debug", false, "strip debug instructions (OpName, OpMemberName, OpSource, OpLine, etc.)")
	stripNonSemantic := flag.Bool("strip-nonsemantic", false, "strip SPV_KHR_non_semantic_info instructions")
	verbose := flag.Bool("v", false, "verbose output")
	flag.Parse()

	if *inPath == "" || *outPath == "" {
		fmt.Fprintln(os.Stderr, "usage: spirv-strip-debug -i <input.spv> -o <output.spv> [--strip-debug] [--strip-nonsemantic]")
		os.Exit(1)
	}

	if !*stripDebug && !*stripNonSemantic {
		fmt.Fprintln(os.Stderr, "error: at least one of --strip-debug or --strip-nonsemantic must be specified")
		os.Exit(1)
	}

	if err := run(*inPath, *outPath, *stripDebug, *stripNonSemantic, *verbose); err != nil {
		fmt.Fprintf(os.Stderr, "error: %v\n", err)
		os.Exit(1)
	}
}

func run(inPath, outPath string, stripDebug, stripNonSemantic, verbose bool) error {
	data, err := os.ReadFile(inPath)
	if err != nil {
		return fmt.Errorf("failed to read input: %w", err)
	}

	words := spvparse.ToWords(data)
	if len(words) < 5 {
		return fmt.Errorf("SPIR-V too short")
	}

	// NOTE: First pass: if stripping non-semantic, identify ExtInstImport IDs with "NonSemantic." prefix.
	nonSemanticIDs := make(map[uint32]bool)
	if stripNonSemantic {
		i := 5
		for i < len(words) {
			wc := int(words[i] >> 16)
			op := words[i] & 0xFFFF
			if wc == 0 {
				return fmt.Errorf("invalid instruction length 0 at word %d", i)
			}
			if i+wc > len(words) {
				return fmt.Errorf("instruction at word %d extends past end", i)
			}
			if op == opExtInstImport && wc >= 3 {
				resultID := words[i+1]
				name := decodeString(words[i+2 : i+wc])
				if strings.HasPrefix(name, "NonSemantic.") {
					nonSemanticIDs[resultID] = true
				}
			}
			i += wc
		}
	}

	// NOTE: Second pass: rebuild instruction stream, skipping stripped instructions.
	header := words[:5]
	var result []uint32
	result = append(result, header...)

	stripped := 0
	i := 5
	for i < len(words) {
		wc := int(words[i] >> 16)
		op := words[i] & 0xFFFF
		if wc == 0 {
			return fmt.Errorf("invalid instruction length 0 at word %d", i)
		}
		if i+wc > len(words) {
			return fmt.Errorf("instruction at word %d extends past end", i)
		}

		skip := false

		if stripDebug {
			switch op {
			case opSourceContinued, opSource, opSourceExtension,
				opName, opMemberName, opString,
				opLine, opNoLine, opModuleProcessed:
				skip = true
			}
		}

		if stripNonSemantic && !skip {
			// NOTE: Strip OpExtInstImport for NonSemantic.* sets.
			if op == opExtInstImport && wc >= 3 {
				resultID := words[i+1]
				if nonSemanticIDs[resultID] {
					skip = true
				}
			}
			// NOTE: Strip OpExtInst calls referencing NonSemantic.* sets.
			if op == opExtInst && wc >= 4 {
				setID := words[i+3]
				if nonSemanticIDs[setID] {
					skip = true
				}
			}
		}

		if skip {
			stripped++
		} else {
			result = append(result, words[i:i+wc]...)
		}

		i += wc
	}

	if stripped == 0 {
		// NOTE: Nothing changed; just copy.
		return os.WriteFile(outPath, data, 0644)
	}

	if verbose {
		fmt.Fprintf(os.Stdout, "stripped %d instructions\n", stripped)
	}
	outData := spvparse.FromWords(result)
	return os.WriteFile(outPath, outData, 0644)
}

// decodeString decodes a null-terminated UTF-8 string from SPIR-V word operands.
func decodeString(words []uint32) string {
	var buf []byte
	for _, w := range words {
		buf = append(buf, byte(w), byte(w>>8), byte(w>>16), byte(w>>24))
	}
	if idx := strings.IndexByte(string(buf), 0); idx >= 0 {
		buf = buf[:idx]
	}
	return string(buf)
}

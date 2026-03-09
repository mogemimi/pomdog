// Copyright mogemimi. Distributed under the MIT license.

package spvparse

import (
	"encoding/binary"
	"fmt"
	"os"
	"strings"
)

// SPIR-V magic number
const spvMagicNumber = 0x07230203

// SPIR-V storage classes
const (
	StorageClassInput  = 1
	StorageClassOutput = 3
)

// SPIR-V decorations
const (
	decorationLocation = 30
	decorationBuiltIn  = 11
	decorationBlock    = 2
)

// SPIR-V opcodes
const (
	opName           = 5
	opMemberName     = 6
	opEntryPoint     = 15
	opTypeVoid       = 19
	opTypeBool       = 20
	opTypeInt        = 21
	opTypeFloat      = 22
	opTypeVector     = 23
	opTypeMatrix     = 24
	opTypeArray      = 28
	opTypeStruct     = 30
	opTypePointer    = 32
	opVariable       = 59
	opDecorate       = 71
	opMemberDecorate = 72
)

type typeKind int

const (
	typeKindVoid typeKind = iota
	typeKindBool
	typeKindInt
	typeKindFloat
	typeKindVector
	typeKindMatrix
	typeKindArray
	typeKindStruct
	typeKindPointer
)

type typeInfo struct {
	kind         typeKind
	width        uint32   // bit width for Float/Int
	signedness   uint32   // for Int
	compTypeID   uint32   // component type ID for Vector
	compCount    uint32   // component count for Vector
	colTypeID    uint32   // column type ID for Matrix
	colCount     uint32   // column count for Matrix
	elemTypeID   uint32   // element type ID for Array
	memberIDs    []uint32 // member type IDs for Struct
	pointeeID    uint32   // pointee type ID for Pointer
	storageClass uint32   // for Pointer
}

// EntryPoint represents a SPIR-V entry point declaration.
type EntryPoint struct {
	Name           string
	FunctionID     uint32
	ExecutionModel ExecutionModel
}

// ExecutionModel represents a SPIR-V execution model.
type ExecutionModel uint32

const (
	ExecutionModelVertex                 ExecutionModel = 0
	ExecutionModelTessellationControl    ExecutionModel = 1
	ExecutionModelTessellationEvaluation ExecutionModel = 2
	ExecutionModelGeometry               ExecutionModel = 3
	ExecutionModelFragment               ExecutionModel = 4
	ExecutionModelGLCompute              ExecutionModel = 5
)

// String returns a human-readable name for the execution model.
func (em ExecutionModel) String() string {
	switch em {
	case ExecutionModelVertex:
		return "vertex"
	case ExecutionModelTessellationControl:
		return "tessellation_control"
	case ExecutionModelTessellationEvaluation:
		return "tessellation_evaluation"
	case ExecutionModelGeometry:
		return "geometry"
	case ExecutionModelFragment:
		return "fragment"
	case ExecutionModelGLCompute:
		return "compute"
	default:
		return fmt.Sprintf("unknown(%d)", em)
	}
}

// InterfaceVar represents a shader stage input or output variable.
type InterfaceVar struct {
	ID       uint32
	Name     string
	Location int
	TypeDesc string // e.g., "float4", "float2", "int"
	BuiltIn  bool
}

type memberDecoration struct {
	member     uint32
	decoration uint32
	value      uint32
}

// Module holds parsed SPIR-V module data.
type Module struct {
	types       map[uint32]*typeInfo
	names       map[uint32]string
	memberNames map[uint32]map[uint32]string  // id -> member -> name
	decorations map[uint32]map[uint32]uint32  // id -> decoration -> value
	memberDecos map[uint32][]memberDecoration // id -> member decorations
	variables   []variableInfo
	builtIns    map[uint32]bool
	entryPoints []EntryPoint
}

type variableInfo struct {
	resultID     uint32
	typeID       uint32
	storageClass uint32
}

// ParseFile reads and parses a SPIR-V binary file.
func ParseFile(path string) (*Module, error) {
	data, err := os.ReadFile(path)
	if err != nil {
		return nil, fmt.Errorf("failed to read file: %w", err)
	}
	return Parse(data)
}

// Parse parses raw SPIR-V binary data.
func Parse(data []byte) (*Module, error) {
	if len(data) < 20 {
		return nil, fmt.Errorf("SPIR-V data too short")
	}

	// NOTE: Determine endianness from magic number
	var order binary.ByteOrder
	magic := binary.LittleEndian.Uint32(data[0:4])
	if magic == spvMagicNumber {
		order = binary.LittleEndian
	} else {
		magic = binary.BigEndian.Uint32(data[0:4])
		if magic == spvMagicNumber {
			order = binary.BigEndian
		} else {
			return nil, fmt.Errorf("invalid SPIR-V magic number: 0x%08x", magic)
		}
	}

	// NOTE: Read words
	wordCount := len(data) / 4
	words := make([]uint32, wordCount)
	for i := 0; i < wordCount; i++ {
		words[i] = order.Uint32(data[i*4 : (i+1)*4])
	}

	// NOTE: Skip header (5 words)
	return parseInstructions(words[5:])
}

func parseInstructions(words []uint32) (*Module, error) {
	m := &Module{
		types:       make(map[uint32]*typeInfo),
		names:       make(map[uint32]string),
		memberNames: make(map[uint32]map[uint32]string),
		decorations: make(map[uint32]map[uint32]uint32),
		memberDecos: make(map[uint32][]memberDecoration),
		builtIns:    make(map[uint32]bool),
	}

	i := 0
	for i < len(words) {
		instrLen := int(words[i] >> 16)
		opcode := words[i] & 0xFFFF

		if instrLen == 0 {
			return nil, fmt.Errorf("invalid instruction length 0 at word %d", i)
		}
		if i+instrLen > len(words) {
			return nil, fmt.Errorf("instruction at word %d extends past end of data", i)
		}

		operands := words[i+1 : i+instrLen]

		switch opcode {
		case opEntryPoint:
			if len(operands) >= 3 {
				execModel := ExecutionModel(operands[0])
				funcID := operands[1]
				name := decodeString(operands[2:])
				m.entryPoints = append(m.entryPoints, EntryPoint{
					Name:           name,
					FunctionID:     funcID,
					ExecutionModel: execModel,
				})
			}

		case opName:
			if len(operands) >= 2 {
				id := operands[0]
				m.names[id] = decodeString(operands[1:])
			}

		case opMemberName:
			if len(operands) >= 3 {
				id := operands[0]
				member := operands[1]
				if m.memberNames[id] == nil {
					m.memberNames[id] = make(map[uint32]string)
				}
				m.memberNames[id][member] = decodeString(operands[2:])
			}

		case opDecorate:
			if len(operands) >= 2 {
				id := operands[0]
				decoration := operands[1]
				if m.decorations[id] == nil {
					m.decorations[id] = make(map[uint32]uint32)
				}
				value := uint32(0)
				if len(operands) >= 3 {
					value = operands[2]
				}
				m.decorations[id][decoration] = value

				if decoration == decorationBuiltIn {
					m.builtIns[id] = true
				}
			}

		case opMemberDecorate:
			if len(operands) >= 3 {
				id := operands[0]
				member := operands[1]
				decoration := operands[2]
				value := uint32(0)
				if len(operands) >= 4 {
					value = operands[3]
				}
				m.memberDecos[id] = append(m.memberDecos[id], memberDecoration{
					member:     member,
					decoration: decoration,
					value:      value,
				})
			}

		case opTypeVoid:
			if len(operands) >= 1 {
				m.types[operands[0]] = &typeInfo{kind: typeKindVoid}
			}

		case opTypeBool:
			if len(operands) >= 1 {
				m.types[operands[0]] = &typeInfo{kind: typeKindBool}
			}

		case opTypeInt:
			if len(operands) >= 3 {
				m.types[operands[0]] = &typeInfo{
					kind:       typeKindInt,
					width:      operands[1],
					signedness: operands[2],
				}
			}

		case opTypeFloat:
			if len(operands) >= 2 {
				m.types[operands[0]] = &typeInfo{
					kind:  typeKindFloat,
					width: operands[1],
				}
			}

		case opTypeVector:
			if len(operands) >= 3 {
				m.types[operands[0]] = &typeInfo{
					kind:       typeKindVector,
					compTypeID: operands[1],
					compCount:  operands[2],
				}
			}

		case opTypeMatrix:
			if len(operands) >= 3 {
				m.types[operands[0]] = &typeInfo{
					kind:      typeKindMatrix,
					colTypeID: operands[1],
					colCount:  operands[2],
				}
			}

		case opTypeArray:
			if len(operands) >= 3 {
				m.types[operands[0]] = &typeInfo{
					kind:       typeKindArray,
					elemTypeID: operands[1],
				}
			}

		case opTypeStruct:
			if len(operands) >= 1 {
				members := make([]uint32, len(operands)-1)
				copy(members, operands[1:])
				m.types[operands[0]] = &typeInfo{
					kind:      typeKindStruct,
					memberIDs: members,
				}
			}

		case opTypePointer:
			if len(operands) >= 3 {
				m.types[operands[0]] = &typeInfo{
					kind:         typeKindPointer,
					storageClass: operands[1],
					pointeeID:    operands[2],
				}
			}

		case opVariable:
			if len(operands) >= 3 {
				m.variables = append(m.variables, variableInfo{
					typeID:       operands[0],
					resultID:     operands[1],
					storageClass: operands[2],
				})
			}
		}

		i += instrLen
	}

	return m, nil
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

// EntryPoints returns all entry points declared in the SPIR-V module.
func (m *Module) EntryPoints() []EntryPoint {
	return m.entryPoints
}

// resolveTypeDesc returns a human-readable type description for a given type ID.
func (m *Module) resolveTypeDesc(id uint32) string {
	t, ok := m.types[id]
	if !ok {
		return fmt.Sprintf("unknown(id=%d)", id)
	}

	switch t.kind {
	case typeKindVoid:
		return "void"
	case typeKindBool:
		return "bool"
	case typeKindFloat:
		switch t.width {
		case 16:
			return "half"
		case 32:
			return "float"
		case 64:
			return "double"
		default:
			return fmt.Sprintf("float%d", t.width)
		}
	case typeKindInt:
		prefix := "int"
		if t.signedness == 0 {
			prefix = "uint"
		}
		switch t.width {
		case 32:
			return prefix
		default:
			return fmt.Sprintf("%s%d", prefix, t.width)
		}
	case typeKindVector:
		comp := m.resolveTypeDesc(t.compTypeID)
		return fmt.Sprintf("%s%d", comp, t.compCount)
	case typeKindMatrix:
		col := m.resolveTypeDesc(t.colTypeID)
		return fmt.Sprintf("mat(%s x %d)", col, t.colCount)
	case typeKindArray:
		elem := m.resolveTypeDesc(t.elemTypeID)
		return fmt.Sprintf("array(%s)", elem)
	case typeKindStruct:
		return "struct"
	case typeKindPointer:
		return m.resolveTypeDesc(t.pointeeID)
	default:
		return "unknown"
	}
}

// CollectInterfaceVars collects all input or output variables (excluding builtins)
// for the given storage class. Use [StorageClassInput] or [StorageClassOutput].
func (m *Module) CollectInterfaceVars(storageClass uint32) []InterfaceVar {
	var result []InterfaceVar

	for _, v := range m.variables {
		if v.storageClass != storageClass {
			continue
		}

		// NOTE: Skip builtins
		if m.builtIns[v.resultID] {
			continue
		}

		// NOTE: Get pointer type to find pointee
		ptrType, ok := m.types[v.typeID]
		if !ok || ptrType.kind != typeKindPointer {
			continue
		}

		pointeeType, ok := m.types[ptrType.pointeeID]
		if !ok {
			continue
		}

		// NOTE: Check if the pointee is a struct (interface block)
		if pointeeType.kind == typeKindStruct {
			// NOTE: Check if this struct has Block decoration (it's a UBO, not an interface block for I/O)
			if _, hasBlock := m.decorations[ptrType.pointeeID]; hasBlock {
				if _, isBlock := m.decorations[ptrType.pointeeID][decorationBlock]; isBlock {
					continue
				}
			}

			// NOTE: Flatten struct members
			vars := m.flattenStructMembers(v.resultID, ptrType.pointeeID, pointeeType)
			result = append(result, vars...)
			continue
		}

		// NOTE: Simple variable with Location decoration
		decos, ok := m.decorations[v.resultID]
		if !ok {
			continue
		}
		loc, hasLoc := decos[decorationLocation]
		if !hasLoc {
			continue
		}

		name := m.names[v.resultID]
		typeDesc := m.resolveTypeDesc(ptrType.pointeeID)

		result = append(result, InterfaceVar{
			ID:       v.resultID,
			Name:     name,
			Location: int(loc),
			TypeDesc: typeDesc,
		})
	}

	return result
}

// flattenStructMembers extracts individual (location, type) pairs from a struct interface variable.
func (m *Module) flattenStructMembers(varID, structTypeID uint32, structType *typeInfo) []InterfaceVar {
	var result []InterfaceVar

	// NOTE: Check for per-member Location decorations
	memberDecos := m.memberDecos[structTypeID]

	// NOTE: Also check per-variable Location as base location
	baseLocation := -1
	if decos, ok := m.decorations[varID]; ok {
		if loc, hasLoc := decos[decorationLocation]; hasLoc {
			baseLocation = int(loc)
		}
	}

	for i, memberTypeID := range structType.memberIDs {
		memberIdx := uint32(i)

		// NOTE: Skip builtin members
		isBuiltIn := false
		for _, md := range memberDecos {
			if md.member == memberIdx && md.decoration == decorationBuiltIn {
				isBuiltIn = true
				break
			}
		}
		if isBuiltIn {
			continue
		}

		// NOTE: Get member location
		location := -1
		for _, md := range memberDecos {
			if md.member == memberIdx && md.decoration == decorationLocation {
				location = int(md.value)
				break
			}
		}
		if location < 0 && baseLocation >= 0 {
			location = baseLocation + i
		}
		if location < 0 {
			continue
		}

		// NOTE: Get member name
		memberName := ""
		if names, ok := m.memberNames[structTypeID]; ok {
			memberName = names[memberIdx]
		}

		typeDesc := m.resolveTypeDesc(memberTypeID)

		result = append(result, InterfaceVar{
			ID:       varID,
			Name:     memberName,
			Location: location,
			TypeDesc: typeDesc,
		})
	}

	return result
}

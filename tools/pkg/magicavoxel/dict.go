package magicavoxel

import (
	"fmt"
	"strconv"
	"strings"
)

// Keys of the dictionary entries that MagicaVoxel writes.
const (
	// KeyName names a node or a layer.
	KeyName = "_name"

	// KeyHidden is "1" for a hidden node or layer, and "0" otherwise.
	KeyHidden = "_hidden"

	// KeyRotation holds the rotation of a transform frame, as the decimal
	// value of a Rotation.
	KeyRotation = "_r"

	// KeyTranslation holds the translation of a transform frame, as three
	// integers separated by spaces.
	KeyTranslation = "_t"

	// KeyFrame holds the frame number of a transform frame or of a model of a
	// shape node.
	KeyFrame = "_f"
)

// ParseInt32 parses an integer value of a dictionary entry.
func ParseInt32(s string) (int32, error) {
	v, err := strconv.ParseInt(s, 10, 32)
	if err != nil {
		return 0, fmt.Errorf("invalid integer %q", s)
	}
	return int32(v), nil
}

// ParseBool parses a "0" or "1" value of a dictionary entry.
func ParseBool(s string) (bool, error) {
	switch s {
	case "0":
		return false, nil
	case "1":
		return true, nil
	}
	return false, fmt.Errorf("invalid boolean %q; it must be 0 or 1", s)
}

// FormatBool formats a boolean the way MagicaVoxel writes it.
func FormatBool(b bool) string {
	if b {
		return "1"
	}
	return "0"
}

// ParseTranslation parses the "_t" value of a transform frame.
func ParseTranslation(s string) ([3]int32, error) {
	var t [3]int32
	fields := strings.Fields(s)
	if len(fields) != 3 {
		return t, fmt.Errorf("invalid translation %q; it must have three integers", s)
	}
	for i, field := range fields {
		v, err := ParseInt32(field)
		if err != nil {
			return t, fmt.Errorf("invalid translation %q", s)
		}
		t[i] = v
	}
	return t, nil
}

// FormatTranslation formats the "_t" value of a transform frame.
func FormatTranslation(t [3]int32) string {
	return fmt.Sprintf("%d %d %d", t[0], t[1], t[2])
}

// ParseRotation parses the "_r" value of a transform frame.
func ParseRotation(s string) (Rotation, error) {
	v, err := strconv.ParseUint(s, 10, 8)
	if err != nil {
		return 0, fmt.Errorf("invalid rotation %q", s)
	}
	r := Rotation(v)
	if !r.IsValid() {
		return 0, fmt.Errorf("invalid rotation %q", s)
	}
	return r, nil
}

// FormatRotation formats the "_r" value of a transform frame.
func FormatRotation(r Rotation) string {
	return strconv.Itoa(int(r))
}

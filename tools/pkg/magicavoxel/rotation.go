package magicavoxel

import "fmt"

// Rotation is a rotation matrix packed into a byte. Bits 0-1 and 2-3 give the
// column of the nonzero entry of the first and the second row, and bits 4, 5,
// and 6 are set when the entry of the first, the second, and the third row is
// -1. The third row has its nonzero entry in the remaining column.
type Rotation uint8

// IdentityRotation is the rotation that changes nothing.
const IdentityRotation Rotation = 4

// IsValid reports whether the byte packs a matrix whose rows have their
// nonzero entries in different columns.
func (r Rotation) IsValid() bool {
	c0 := int(r & 3)
	c1 := int((r >> 2) & 3)
	return (r&0x80) == 0 && c0 < 3 && c1 < 3 && c0 != c1
}

// Matrix returns the rotation matrix, in which m[row][column] multiplies a
// column vector: rotated[row] is the sum of m[row][column] * v[column]. It
// returns an error for an invalid rotation.
func (r Rotation) Matrix() ([3][3]int32, error) {
	var m [3][3]int32
	if !r.IsValid() {
		return m, fmt.Errorf("invalid rotation %d", r)
	}
	c0 := int(r & 3)
	c1 := int((r >> 2) & 3)
	columns := [3]int{c0, c1, 3 - c0 - c1}
	for row, column := range columns {
		m[row][column] = 1
		if r&(1<<(4+row)) != 0 {
			m[row][column] = -1
		}
	}
	return m, nil
}

// RotationFromMatrix packs a rotation matrix whose rows each have one entry
// of 1 or -1 in different columns.
func RotationFromMatrix(m [3][3]int32) (Rotation, error) {
	var r Rotation
	var columns [3]int
	used := [3]bool{}
	for row := range 3 {
		column := -1
		for c := range 3 {
			switch m[row][c] {
			case 0:
			case 1, -1:
				if column >= 0 {
					return 0, fmt.Errorf("row %d of the matrix has more than one nonzero entry", row)
				}
				column = c
			default:
				return 0, fmt.Errorf("row %d of the matrix has an entry other than 0, 1, or -1", row)
			}
		}
		if column < 0 {
			return 0, fmt.Errorf("row %d of the matrix has no nonzero entry", row)
		}
		if used[column] {
			return 0, fmt.Errorf("two rows of the matrix use column %d", column)
		}
		used[column] = true
		columns[row] = column
		if m[row][column] < 0 {
			r |= 1 << (4 + row)
		}
	}
	r |= Rotation(columns[0]) | Rotation(columns[1])<<2
	return r, nil
}

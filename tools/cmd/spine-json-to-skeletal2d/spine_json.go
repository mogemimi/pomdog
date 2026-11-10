// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"sort"
)

// spineDocument is a skeleton that Spine 2.x exported as JSON, the format
// whose weighted meshes have the "skinnedmesh" type.
type spineDocument struct {
	Bones      []*spineBone
	Slots      []*spineSlot
	Skins      map[string]spineSkin
	Animations map[string]*spineAnimation

	// Unsupported lists the top-level sections that the converter ignores,
	// in ascending order.
	Unsupported []string
}

type spineBone struct {
	Name     string   `json:"name"`
	Parent   *string  `json:"parent"`
	X        float64  `json:"x"`
	Y        float64  `json:"y"`
	Rotation float64  `json:"rotation"`
	ScaleX   *float64 `json:"scaleX"`
	ScaleY   *float64 `json:"scaleY"`
}

type spineSlot struct {
	Name       string  `json:"name"`
	Bone       string  `json:"bone"`
	Attachment *string `json:"attachment"`
	Color      *string `json:"color"`
}

// spineSkin maps a slot name to the attachments of the slot by their names.
type spineSkin map[string]map[string]*spineAttachment

type spineAttachment struct {
	Type     string   `json:"type"`
	X        float64  `json:"x"`
	Y        float64  `json:"y"`
	ScaleX   *float64 `json:"scaleX"`
	ScaleY   *float64 `json:"scaleY"`
	Rotation float64  `json:"rotation"`

	// NOTE: Only "skinnedmesh" attachments have the fields below.
	UVs       []float64 `json:"uvs"`
	Triangles []float64 `json:"triangles"`
	Vertices  []float64 `json:"vertices"`
}

// spineAnimation holds the timelines of an animation undecoded, by the name of
// the bone or slot that they animate and then by the kind of the timeline.
type spineAnimation struct {
	Bones map[string]map[string]json.RawMessage
	Slots map[string]map[string]json.RawMessage

	// Unsupported lists the sections of the animation that the converter
	// ignores, in ascending order.
	Unsupported []string
}

type spineRotateKey struct {
	Time  *float64        `json:"time"`
	Angle float64         `json:"angle"`
	Curve json.RawMessage `json:"curve"`
}

type spineTranslateKey struct {
	Time  *float64        `json:"time"`
	X     float64         `json:"x"`
	Y     float64         `json:"y"`
	Curve json.RawMessage `json:"curve"`
}

type spineScaleKey struct {
	Time  *float64        `json:"time"`
	X     *float64        `json:"x"`
	Y     *float64        `json:"y"`
	Curve json.RawMessage `json:"curve"`
}

type spineAttachmentKey struct {
	Time *float64 `json:"time"`
	Name *string  `json:"name"`
}

// parseSpineJSON decodes a skeleton exported by Spine 2.x.
func parseSpineJSON(data []byte) (*spineDocument, error) {
	sections := map[string]json.RawMessage{}
	if err := json.Unmarshal(data, &sections); err != nil {
		return nil, fmt.Errorf("json.Unmarshal() failed: %w", err)
	}

	doc := &spineDocument{}
	for name, raw := range sections {
		var err error
		switch name {
		case "bones":
			err = json.Unmarshal(raw, &doc.Bones)
		case "slots":
			err = json.Unmarshal(raw, &doc.Slots)
		case "skins":
			err = json.Unmarshal(raw, &doc.Skins)
		case "animations":
			doc.Animations, err = parseSpineAnimations(raw)
		case "skeleton":
			// NOTE: Metadata of the export, such as the Spine version.
		default:
			doc.Unsupported = append(doc.Unsupported, name)
		}
		if err != nil {
			return nil, fmt.Errorf("%q: %w", name, err)
		}
	}
	sort.Strings(doc.Unsupported)

	return doc, nil
}

func parseSpineAnimations(data []byte) (map[string]*spineAnimation, error) {
	rawAnimations := map[string]map[string]json.RawMessage{}
	if err := json.Unmarshal(data, &rawAnimations); err != nil {
		return nil, err
	}

	animations := map[string]*spineAnimation{}
	for name, sections := range rawAnimations {
		animation := &spineAnimation{}
		for section, raw := range sections {
			var err error
			switch section {
			case "bones":
				err = json.Unmarshal(raw, &animation.Bones)
			case "slots":
				err = json.Unmarshal(raw, &animation.Slots)
			default:
				animation.Unsupported = append(animation.Unsupported, section)
			}
			if err != nil {
				return nil, fmt.Errorf("animation %q: %q: %w", name, section, err)
			}
		}
		sort.Strings(animation.Unsupported)
		animations[name] = animation
	}

	return animations, nil
}

// isLinearCurve reports whether the "curve" of a key interpolates linearly to
// the next key. Spine omits it for a linear curve, and writes "stepped" or the
// control points of a Bezier curve otherwise.
func isLinearCurve(curve json.RawMessage) bool {
	curve = bytes.TrimSpace(curve)
	return len(curve) == 0 || bytes.Equal(curve, []byte("null")) || bytes.Equal(curve, []byte(`"linear"`))
}

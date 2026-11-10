// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"math"
	"os"
	"path/filepath"
	"strings"
	"testing"

	schemas "github.com/mogemimi/pomdog/build/schemas"
	stringhash "github.com/mogemimi/pomdog/tools/pkg/stringhash"
)

const sampleJSON = `{
  "skeleton": {"spine": "2.1.27"},
  "bones": [
    {"name": "root"},
    {"name": "hip", "parent": "root", "length": 10, "x": 1.5, "y": -2, "rotation": 270},
    {"name": "arm", "parent": "hip", "rotation": -190, "scaleX": 2, "scaleY": 3}
  ],
  "slots": [
    {"name": "body", "bone": "hip", "attachment": "body_image"},
    {"name": "empty", "bone": "root"},
    {"name": "cloth", "bone": "arm", "attachment": "cloth_mesh", "color": "ff0000ff"}
  ],
  "skins": {
    "default": {
      "body": {
        "body_image": {"x": 3, "rotation": 90, "width": 8, "height": 8},
        "body_blink": {"x": 3, "width": 8, "height": 8}
      },
      "cloth": {
        "cloth_mesh": {
          "type": "skinnedmesh",
          "uvs": [0, 0, 1, 0, 0, 1],
          "triangles": [0, 1, 2],
          "vertices": [
            1, 2, 5, 6, 1,
            2, 1, 10, 11, 0.25, 2, 20, 21, 0.75,
            1, 0, 7, 8, 1
          ]
        }
      }
    },
    "red": {
      "body": {
        "body_image": {"x": 4}
      }
    }
  },
  "animations": {
    "walk": {
      "bones": {
        "arm": {
          "rotate": [
            {"time": 0.5, "angle": 180},
            {"time": 0, "angle": -90, "curve": "stepped"}
          ]
        },
        "hip": {
          "translate": [{"time": 0, "x": 1, "y": 2}],
          "scale": [{"time": 0, "x": 1.5, "y": 2.5}],
          "shear": [{"time": 0}]
        }
      },
      "slots": {
        "body": {
          "attachment": [
            {"time": 0.25, "name": "body_blink"},
            {"time": 0, "name": "body_image"},
            {"time": 0.5, "name": null}
          ],
          "color": [{"time": 0, "color": "ffffffff"}]
        }
      },
      "events": []
    },
    "idle": {}
  },
  "ik": []
}`

func convertJSON(t *testing.T, source string) (*output, error) {
	t.Helper()
	doc, err := parseSpineJSON([]byte(source))
	if err != nil {
		return nil, err
	}
	return convert(doc)
}

func mustConvertJSON(t *testing.T, source string) *output {
	t.Helper()
	out, err := convertJSON(t, source)
	if err != nil {
		t.Fatalf("convert() failed: %v", err)
	}
	return out
}

func approxEqual(a, b float32) bool {
	return math.Abs(float64(a-b)) < 1e-5
}

// findByKey returns the index of key in keys, which must be sorted in
// ascending order as the runtime searches them with a binary search.
func findByKey(t *testing.T, keys []uint32, name string) int {
	t.Helper()
	for i := 1; i < len(keys); i++ {
		if keys[i-1] >= keys[i] {
			t.Fatalf("keys are not in ascending order: %v", keys)
		}
	}
	key := stringhash.StringToHash32(name)
	for i, k := range keys {
		if k == key {
			return i
		}
	}
	t.Fatalf("key of %q is not found", name)
	return -1
}

func TestConvertSkeleton(t *testing.T) {
	out := mustConvertJSON(t, sampleJSON)

	joints := out.Skeleton.Joints
	if len(joints) != 3 {
		t.Fatalf("got %d joints, want 3", len(joints))
	}
	if joints[0].Parent != noJoint || joints[1].Parent != 0 || joints[2].Parent != 1 {
		t.Errorf("unexpected parents: %d, %d, %d", joints[0].Parent, joints[1].Parent, joints[2].Parent)
	}
	if joints[0].Scale != 1 || joints[0].Rotation != 0 {
		t.Errorf("root must have the default pose: %+v", joints[0])
	}
	if joints[1].TranslateX != 1.5 || joints[1].TranslateY != -2 {
		t.Errorf("unexpected translation: %+v", joints[1])
	}

	// NOTE: 270 degrees wraps to -90 degrees, and -190 degrees to 170 degrees.
	if !approxEqual(joints[1].Rotation, -math.Pi/2) {
		t.Errorf("got rotation %v, want %v", joints[1].Rotation, -math.Pi/2)
	}
	if !approxEqual(joints[2].Rotation, 170*math.Pi/180) {
		t.Errorf("got rotation %v, want %v", joints[2].Rotation, 170*math.Pi/180)
	}

	// NOTE: "scaleX" takes precedence over "scaleY".
	if joints[2].Scale != 2 {
		t.Errorf("got scale %v, want 2", joints[2].Scale)
	}

	want := []string{"root", "hip", "arm"}
	if strings.Join(out.Debug.Joints, ",") != strings.Join(want, ",") {
		t.Errorf("got joint names %v, want %v", out.Debug.Joints, want)
	}
}

func TestConvertSkins(t *testing.T) {
	out := mustConvertJSON(t, sampleJSON)

	if len(out.SkinSet.Keys) != 2 || len(out.SkinSet.Skins) != 2 {
		t.Fatalf("got %d keys and %d skins, want 2", len(out.SkinSet.Keys), len(out.SkinSet.Skins))
	}

	t.Run("default", func(t *testing.T) {
		skin := out.SkinSet.Skins[findByKey(t, out.SkinSet.Keys, "default")]

		// NOTE: The slot "empty" shows nothing in the setup pose.
		if len(skin.Slots) != 2 {
			t.Fatalf("got %d slots, want 2", len(skin.Slots))
		}

		body := skin.Slots[0]
		if body.SlotKey != stringhash.StringToHash32("body") || body.AttachmentKey != stringhash.StringToHash32("body_image") {
			t.Errorf("unexpected keys of the first slot: %+v", body)
		}
		if body.Joint != 1 || body.Mesh != noMesh {
			t.Errorf("got joint %d and mesh %d, want 1 and %d", body.Joint, body.Mesh, noMesh)
		}
		if body.TranslateX != 3 || body.TranslateY != 0 || body.ScaleX != 1 || body.ScaleY != 1 {
			t.Errorf("unexpected placement: %+v", body)
		}
		if !approxEqual(body.Rotation, math.Pi/2) {
			t.Errorf("got rotation %v, want %v", body.Rotation, math.Pi/2)
		}

		cloth := skin.Slots[1]
		if cloth.SlotKey != stringhash.StringToHash32("cloth") || cloth.Joint != 2 || cloth.Mesh != 0 {
			t.Errorf("unexpected second slot: %+v", cloth)
		}
		if cloth.ScaleX != 1 || cloth.ScaleY != 1 || cloth.Rotation != 0 {
			t.Errorf("a mesh slot must have the identity placement: %+v", cloth)
		}

		if len(skin.Meshes) != 1 {
			t.Fatalf("got %d meshes, want 1", len(skin.Meshes))
		}
		mesh := skin.Meshes[0]
		if len(mesh.Vertices) != 3 || len(mesh.Indices) != 3 {
			t.Fatalf("got %d vertices and %d indices, want 3", len(mesh.Vertices), len(mesh.Indices))
		}

		// NOTE: The joints are in descending order of weight, and the
		// position is in the local space of the first one.
		v := mesh.Vertices[1]
		if v.Joint0 != 2 || v.Joint1 != 1 || v.Joint2 != noJoint || v.Joint3 != noJoint {
			t.Errorf("unexpected joints: %d, %d, %d, %d", v.Joint0, v.Joint1, v.Joint2, v.Joint3)
		}
		if v.PositionX != 20 || v.PositionY != 21 {
			t.Errorf("got position (%v, %v), want (20, 21)", v.PositionX, v.PositionY)
		}
		if v.Weight0 != 0.75 || v.Weight1 != 0.25 || v.Weight2 != 0 || v.Weight3 != 0 {
			t.Errorf("unexpected weights: %v, %v, %v, %v", v.Weight0, v.Weight1, v.Weight2, v.Weight3)
		}
		if v.TexcoordU != 1 || v.TexcoordV != 0 {
			t.Errorf("got texture coordinate (%v, %v), want (1, 0)", v.TexcoordU, v.TexcoordV)
		}
	})

	t.Run("falls back to the default skin", func(t *testing.T) {
		skin := out.SkinSet.Skins[findByKey(t, out.SkinSet.Keys, "red")]
		if len(skin.Slots) != 2 {
			t.Fatalf("got %d slots, want 2", len(skin.Slots))
		}
		if skin.Slots[0].TranslateX != 4 {
			t.Errorf("got x %v from the default skin, want 4 from the skin itself", skin.Slots[0].TranslateX)
		}
		if skin.Slots[1].Mesh != 0 || len(skin.Meshes) != 1 {
			t.Errorf("the mesh of the default skin is missing: %+v", skin.Slots[1])
		}
	})
}

func TestConvertClips(t *testing.T) {
	out := mustConvertJSON(t, sampleJSON)

	if len(out.ClipSet.Keys) != 2 || len(out.ClipSet.Clips) != 2 {
		t.Fatalf("got %d keys and %d clips, want 2", len(out.ClipSet.Keys), len(out.ClipSet.Clips))
	}
	clip := out.ClipSet.Clips[findByKey(t, out.ClipSet.Keys, "walk")]

	if len(clip.RotationTracks) != 1 || len(clip.ScaleTracks) != 1 || len(clip.TranslationTracks) != 1 || len(clip.AttachmentTracks) != 1 {
		t.Fatalf("unexpected track counts: %d, %d, %d, %d", len(clip.RotationTracks), len(clip.ScaleTracks), len(clip.TranslationTracks), len(clip.AttachmentTracks))
	}

	// NOTE: The keys are in ascending order of time.
	rotation := clip.RotationTracks[0]
	if rotation.Joint != 2 || len(rotation.Keys) != 2 {
		t.Fatalf("unexpected rotation track: joint %d, %d keys", rotation.Joint, len(rotation.Keys))
	}
	if rotation.Keys[0].Time != 0 || !approxEqual(rotation.Keys[0].Rotation, -math.Pi/2) {
		t.Errorf("unexpected first key: %+v", rotation.Keys[0])
	}
	if rotation.Keys[1].Time != 0.5 || !approxEqual(rotation.Keys[1].Rotation, math.Pi) {
		t.Errorf("unexpected second key: %+v", rotation.Keys[1])
	}
	if rotation.Keys[1].Rotation > float32(math.Pi) {
		t.Errorf("rotation %v must not exceed pi as a float32", rotation.Keys[1].Rotation)
	}

	translation := clip.TranslationTracks[0]
	if translation.Joint != 1 || translation.Keys[0].TranslateX != 1 || translation.Keys[0].TranslateY != 2 {
		t.Errorf("unexpected translation track: joint %d, %+v", translation.Joint, translation.Keys[0])
	}

	// NOTE: "y" takes precedence over "x" in a scale key.
	scale := clip.ScaleTracks[0]
	if scale.Joint != 1 || scale.Keys[0].Scale != 2.5 {
		t.Errorf("unexpected scale track: joint %d, %+v", scale.Joint, scale.Keys[0])
	}

	// NOTE: The key that hides the attachment is dropped.
	attachment := clip.AttachmentTracks[0]
	if attachment.SlotKey != stringhash.StringToHash32("body") || len(attachment.Keys) != 2 {
		t.Fatalf("unexpected attachment track: %+v", attachment)
	}
	if attachment.Keys[0].Time != 0 || attachment.Keys[0].AttachmentKey != stringhash.StringToHash32("body_image") {
		t.Errorf("unexpected first key: %+v", attachment.Keys[0])
	}
	if attachment.Keys[1].Time != 0.25 || attachment.Keys[1].AttachmentKey != stringhash.StringToHash32("body_blink") {
		t.Errorf("unexpected second key: %+v", attachment.Keys[1])
	}

	idle := out.ClipSet.Clips[findByKey(t, out.ClipSet.Keys, "idle")]
	if len(idle.RotationTracks)+len(idle.ScaleTracks)+len(idle.TranslationTracks)+len(idle.AttachmentTracks) != 0 {
		t.Errorf("an empty animation must have no tracks: %+v", idle)
	}
}

func TestConvertDebug(t *testing.T) {
	out := mustConvertJSON(t, sampleJSON)
	debug := out.Debug

	// NOTE: Slots are in draw order, and include the slot that shows nothing.
	if len(debug.Slots) != 3 || debug.Slots[0].Name != "body" || debug.Slots[1].Name != "empty" || debug.Slots[2].Name != "cloth" {
		t.Errorf("unexpected slots: %+v", debug.Slots)
	}

	names := func(entries []*schemas.Skeletal2DNameDebugT) map[string]bool {
		result := map[string]bool{}
		for i, entry := range entries {
			if entry.Key != stringhash.StringToHash32(entry.Name) {
				t.Errorf("key %#x does not match the name %q", entry.Key, entry.Name)
			}
			if i > 0 && entries[i-1].Key >= entry.Key {
				t.Errorf("entries are not in ascending order of key")
			}
			result[entry.Name] = true
		}
		return result
	}
	if got := names(debug.Skins); len(got) != 2 || !got["default"] || !got["red"] {
		t.Errorf("unexpected skins: %v", got)
	}
	if got := names(debug.Clips); len(got) != 2 || !got["walk"] || !got["idle"] {
		t.Errorf("unexpected clips: %v", got)
	}
	if got := names(debug.Attachments); len(got) != 3 || !got["body_image"] || !got["body_blink"] || !got["cloth_mesh"] {
		t.Errorf("unexpected attachments: %v", got)
	}

	wantNotes := []string{
		`"ik" is not supported`,
		`bone "arm": scales the axes differently`,
		`slot "cloth": color "ff0000ff" is not supported`,
		`animation "walk": "events" is not supported`,
		`animation "walk": bone "hip": "scale": 1 keys scale the axes differently`,
		`animation "walk": bone "hip": "shear" is not supported`,
		`animation "walk": slot "body": "attachment": 1 keys that hide the attachment were dropped`,
		`animation "walk": slot "body": "color" is not supported`,
		`animation "walk": 1 keys have a stepped or Bezier curve`,
	}
	notes := strings.Join(debug.Notes, "\n")
	for _, want := range wantNotes {
		if !strings.Contains(notes, want) {
			t.Errorf("notes do not have %q:\n%s", want, notes)
		}
	}
	if len(debug.Notes) != len(wantNotes) {
		t.Errorf("got %d notes, want %d:\n%s", len(debug.Notes), len(wantNotes), notes)
	}
}

func TestConvertIsDeterministic(t *testing.T) {
	// NOTE: The source has JSON objects, which Go decodes into maps without
	// an order, so the output must not depend on the iteration order.
	first := packAll(mustConvertJSON(t, sampleJSON))
	for i := 0; i < 20; i++ {
		if again := packAll(mustConvertJSON(t, sampleJSON)); again != first {
			t.Fatalf("conversion #%d produced different bytes", i)
		}
	}
}

func packAll(out *output) string {
	return string(packSkeleton(out.Skeleton)) + string(packSkinSet(out.SkinSet)) + string(packClipSet(out.ClipSet)) + string(packDebug(out.Debug))
}

func TestConvertErrors(t *testing.T) {
	// NOTE: Each case replaces one part of a valid document.
	const template = `{
  "bones": [{"name": "root"}, BONE],
  "slots": [SLOT],
  "skins": {"default": {"body": {"image": ATTACHMENT}}},
  "animations": {"walk": {"bones": {BONE_TIMELINES}, "slots": {SLOT_TIMELINES}}}
}`
	const mesh = `{"type": "skinnedmesh", "uvs": [0, 0, 1, 0, 0, 1], "triangles": [0, 1, 2], "vertices": VERTICES}`

	valid := map[string]string{
		"BONE":           `{"name": "hip", "parent": "root"}`,
		"SLOT":           `{"name": "body", "bone": "hip", "attachment": "image"}`,
		"ATTACHMENT":     `{}`,
		"BONE_TIMELINES": `"hip": {"rotate": [{"time": 0, "angle": 1}]}`,
		"SLOT_TIMELINES": `"body": {"attachment": [{"time": 0, "name": "image"}]}`,
	}

	cases := []struct {
		name    string
		part    string
		value   string
		wantErr string
	}{
		{"valid document", "BONE", valid["BONE"], ""},
		{"bone without a name", "BONE", `{"parent": "root"}`, "has no name"},
		{"duplicate bone", "BONE", `{"name": "root", "parent": "root"}`, "defined more than once"},
		{"parent after the bone", "BONE", `{"name": "hip", "parent": "tail"}`, `parent "tail" must be defined before`},
		{"second root", "BONE", `{"name": "hip"}`, "only the first bone can be without a parent"},
		{"slot on an unknown bone", "SLOT", `{"name": "body", "bone": "tail", "attachment": "image"}`, `unknown bone "tail"`},
		{"unsupported attachment type", "ATTACHMENT", `{"type": "boundingbox"}`, `type "boundingbox" is not supported`},
		{"mesh with a bone index out of range", "ATTACHMENT", strings.Replace(mesh, "VERTICES", "[1, 2, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1]", 1), "unknown bone index 2"},
		{"mesh with too few vertices", "ATTACHMENT", strings.Replace(mesh, "VERTICES", "[1, 0, 0, 0, 1, 1, 0, 0, 0, 1]", 1), `"vertices" ends before vertex #2`},
		{"mesh with a truncated vertex", "ATTACHMENT", strings.Replace(mesh, "VERTICES", "[1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 2, 0, 0, 0, 1]", 1), `"vertices" ends in the middle of vertex #2`},
		{"mesh with leftover numbers", "ATTACHMENT", strings.Replace(mesh, "VERTICES", "[1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 9]", 1), "1 numbers left over"},
		{"mesh with a triangle index out of range", "ATTACHMENT", `{"type": "skinnedmesh", "uvs": [0, 0, 1, 0, 0, 1], "triangles": [0, 1, 3], "vertices": [1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1]}`, "refers to unknown vertex 3"},
		{"animation of an unknown bone", "BONE_TIMELINES", `"tail": {"rotate": [{"time": 0}]}`, `unknown bone "tail"`},
		{"animation of an unknown slot", "SLOT_TIMELINES", `"hat": {"attachment": [{"time": 0, "name": "image"}]}`, `unknown slot "hat"`},
		{"key without a time", "BONE_TIMELINES", `"hip": {"rotate": [{"angle": 1}]}`, "key #0 has no time"},
		{"key with a negative time", "BONE_TIMELINES", `"hip": {"rotate": [{"time": -1}]}`, "must not be negative"},
		{"translation out of range", "BONE_TIMELINES", `"hip": {"translate": [{"time": 0, "x": 1025}]}`, "out of the range [-1024, 1024]"},
		{"scale out of range", "BONE_TIMELINES", `"hip": {"scale": [{"time": 0, "y": -21}]}`, "out of the range [-20, 20]"},
	}

	for _, tc := range cases {
		t.Run(tc.name, func(t *testing.T) {
			source := template
			for part, value := range valid {
				if part == tc.part {
					value = tc.value
				}
				source = strings.Replace(source, part, value, 1)
			}

			_, err := convertJSON(t, source)
			if tc.wantErr == "" {
				if err != nil {
					t.Fatalf("convert() failed: %v", err)
				}
				return
			}
			if err == nil {
				t.Fatalf("convert() succeeded, want an error with %q", tc.wantErr)
			}
			if !strings.Contains(err.Error(), tc.wantErr) {
				t.Errorf("got error %q, want it to have %q", err.Error(), tc.wantErr)
			}
		})
	}
}

func TestConvertRejectsOtherFormats(t *testing.T) {
	// NOTE: Later Spine versions write the skins as an array.
	_, err := convertJSON(t, `{"bones": [{"name": "root"}], "skins": [{"name": "default"}]}`)
	if err == nil || !strings.Contains(err.Error(), `"skins"`) {
		t.Errorf("got error %v, want one about \"skins\"", err)
	}

	_, err = convertJSON(t, `{"slots": []}`)
	if err == nil || !strings.Contains(err.Error(), "at least one bone") {
		t.Errorf("got error %v, want one about the missing bones", err)
	}
}

func TestMeshDropsLowestWeights(t *testing.T) {
	out := mustConvertJSON(t, `{
  "bones": [
    {"name": "b0"},
    {"name": "b1", "parent": "b0"},
    {"name": "b2", "parent": "b0"},
    {"name": "b3", "parent": "b0"},
    {"name": "b4", "parent": "b0"}
  ],
  "slots": [{"name": "body", "bone": "b0", "attachment": "mesh"}],
  "skins": {"default": {"body": {"mesh": {
    "type": "skinnedmesh",
    "uvs": [0, 0, 1, 0, 0, 1],
    "triangles": [0, 1, 2],
    "vertices": [
      5, 0, 0, 0, 0.125, 1, 1, 1, 0.25, 2, 2, 2, 0.0625, 3, 3, 3, 0.5, 4, 4, 4, 0.0625,
      1, 0, 0, 0, 1,
      1, 0, 0, 0, 1
    ]
  }}}}
}`)

	v := out.SkinSet.Skins[0].Meshes[0].Vertices[0]
	if v.Joint0 != 3 || v.Joint1 != 1 || v.Joint2 != 0 || v.Joint3 != 2 {
		t.Errorf("unexpected joints: %d, %d, %d, %d", v.Joint0, v.Joint1, v.Joint2, v.Joint3)
	}
	if v.PositionX != 3 || v.PositionY != 3 {
		t.Errorf("got position (%v, %v), want (3, 3)", v.PositionX, v.PositionY)
	}

	// NOTE: The four kept joints share the weight 0.0625 of the dropped one.
	if sum := v.Weight0 + v.Weight1 + v.Weight2 + v.Weight3; !approxEqual(sum, 1) {
		t.Errorf("got the sum of weights %v, want 1", sum)
	}
	if !approxEqual(v.Weight0, 0.515625) || !approxEqual(v.Weight3, 0.078125) {
		t.Errorf("unexpected weights: %v, %v, %v, %v", v.Weight0, v.Weight1, v.Weight2, v.Weight3)
	}

	notes := strings.Join(out.Debug.Notes, "\n")
	if !strings.Contains(notes, "1 vertices have more than 4 bones") {
		t.Errorf("notes do not mention the dropped bones:\n%s", notes)
	}
}

func TestRunWritesFiles(t *testing.T) {
	dir := t.TempDir()
	opts := &options{
		InFile:      filepath.Join(dir, "in.json"),
		OutSkeleton: filepath.Join(dir, "out", "model.skeleton_skel2d"),
		OutSkin:     filepath.Join(dir, "out", "model.skin_skel2d"),
		OutClip:     filepath.Join(dir, "out", "model.clip_skel2d"),
		OutDebug:    filepath.Join(dir, "debug", "model.skel2d-debug"),
	}
	if err := os.WriteFile(opts.InFile, []byte(sampleJSON), 0o644); err != nil {
		t.Fatalf("os.WriteFile() failed: %v", err)
	}
	if err := run(opts); err != nil {
		t.Fatalf("run() failed: %v", err)
	}

	read := func(path string) []byte {
		t.Helper()
		data, err := os.ReadFile(path)
		if err != nil {
			t.Fatalf("os.ReadFile() failed: %v", err)
		}
		return data
	}

	skeleton := read(opts.OutSkeleton)
	if got := schemas.GetRootAsSkeletal2DSkeleton(skeleton, 0).JointsLength(); got != 3 {
		t.Errorf("got %d joints, want 3", got)
	}

	skin := read(opts.OutSkin)
	if got := schemas.GetRootAsSkeletal2DSkinSet(skin, 0).SkinsLength(); got != 2 {
		t.Errorf("got %d skins, want 2", got)
	}

	clip := read(opts.OutClip)
	if got := schemas.GetRootAsSkeletal2DClipSet(clip, 0).ClipsLength(); got != 2 {
		t.Errorf("got %d clips, want 2", got)
	}

	debug := read(opts.OutDebug)
	if got := schemas.GetRootAsSkeletal2DDebug(debug, 0).JointsLength(); got != 3 {
		t.Errorf("got %d joint names, want 3", got)
	}

	t.Run("the debug file is optional", func(t *testing.T) {
		opts := *opts
		opts.OutDebug = ""
		opts.OutSkeleton = filepath.Join(dir, "nodebug", "model.skeleton_skel2d")
		opts.OutSkin = filepath.Join(dir, "nodebug", "model.skin_skel2d")
		opts.OutClip = filepath.Join(dir, "nodebug", "model.clip_skel2d")
		if err := run(&opts); err != nil {
			t.Fatalf("run() failed: %v", err)
		}
		entries, err := os.ReadDir(filepath.Join(dir, "nodebug"))
		if err != nil {
			t.Fatalf("os.ReadDir() failed: %v", err)
		}
		if len(entries) != 3 {
			t.Errorf("got %d files, want 3", len(entries))
		}
	})

	t.Run("a missing input fails", func(t *testing.T) {
		opts := *opts
		opts.InFile = filepath.Join(dir, "missing.json")
		if err := run(&opts); err == nil {
			t.Errorf("run() succeeded, want an error")
		}
	})
}

// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"encoding/json"
	"errors"
	"fmt"
	"math"
	"sort"
	"strings"

	schemas "github.com/mogemimi/pomdog/build/schemas"
	stringhash "github.com/mogemimi/pomdog/tools/pkg/stringhash"
)

const (
	// noJoint marks a joint without a parent, and an unused influence of a
	// mesh vertex.
	noJoint = 255

	// noMesh marks a slot that shows a region attachment.
	noMesh = 65535

	// maxJointCount is the number of joints that a joint index can address.
	maxJointCount = 255

	// maxInfluenceCount is the number of joints that can influence a vertex.
	maxInfluenceCount = 4

	// NOTE: schemas/skeletal2d_clip.fbs limits the key values to these ranges.
	maxKeyTranslation = 1024
	maxKeyScale       = 20

	// defaultSkinName is the skin whose attachments the other skins fall back to.
	defaultSkinName = "default"
)

// output holds the root table of each output file.
type output struct {
	Skeleton *schemas.Skeletal2DSkeletonT
	SkinSet  *schemas.Skeletal2DSkinSetT
	ClipSet  *schemas.Skeletal2DClipSetT
	Debug    *schemas.Skeletal2DDebugT
}

// converter holds what the conversion of one document resolves names against.
type converter struct {
	doc *spineDocument

	// joints maps a bone name to its joint index.
	joints map[string]uint8

	// slotKeys maps a slot name to the hash of the name.
	slotKeys map[string]uint32

	// attachments collects the attachment names that the output refers to.
	attachments *keyTable

	notes []string
}

// convert resolves the names of doc to indices and hash keys, and returns the
// tables to write.
func convert(doc *spineDocument) (*output, error) {
	c := &converter{
		doc:         doc,
		joints:      map[string]uint8{},
		slotKeys:    map[string]uint32{},
		attachments: newKeyTable("attachment"),
	}
	out := &output{Debug: &schemas.Skeletal2DDebugT{}}

	for _, name := range doc.Unsupported {
		c.notef("%q is not supported and was dropped", name)
	}

	var err error
	if out.Skeleton, err = c.convertSkeleton(out.Debug); err != nil {
		return nil, err
	}
	if err = c.convertSlots(out.Debug); err != nil {
		return nil, err
	}
	if out.SkinSet, err = c.convertSkins(out.Debug); err != nil {
		return nil, err
	}
	if out.ClipSet, err = c.convertClips(out.Debug); err != nil {
		return nil, err
	}

	out.Debug.Attachments = c.attachments.sorted()
	out.Debug.Notes = c.notes
	return out, nil
}

func (c *converter) notef(format string, args ...any) {
	c.notes = append(c.notes, fmt.Sprintf(format, args...))
}

func (c *converter) convertSkeleton(debug *schemas.Skeletal2DDebugT) (*schemas.Skeletal2DSkeletonT, error) {
	bones := c.doc.Bones
	if len(bones) == 0 {
		return nil, errors.New(`"bones" must have at least one bone`)
	}
	if len(bones) > maxJointCount {
		return nil, fmt.Errorf(`"bones" has %d bones, but the limit is %d`, len(bones), maxJointCount)
	}

	skeleton := &schemas.Skeletal2DSkeletonT{}
	for i, bone := range bones {
		if bone == nil || bone.Name == "" {
			return nil, fmt.Errorf("bone #%d has no name", i)
		}
		if _, ok := c.joints[bone.Name]; ok {
			return nil, fmt.Errorf("bone %q is defined more than once", bone.Name)
		}

		parent := byte(noJoint)
		if bone.Parent != nil {
			// NOTE: The runtime requires a parent to precede its children,
			// which is also the order that Spine exports bones in.
			index, ok := c.joints[*bone.Parent]
			if !ok {
				return nil, fmt.Errorf("bone %q: parent %q must be defined before the bone", bone.Name, *bone.Parent)
			}
			parent = index
		} else if i != 0 {
			return nil, fmt.Errorf("bone %q: only the first bone can be without a parent", bone.Name)
		}

		// NOTE: A joint scales both axes by the same factor, and "scaleX"
		// takes precedence over "scaleY".
		scaleX, scaleY := valueOr(bone.ScaleX, 1), valueOr(bone.ScaleY, 1)
		scale := scaleX
		if bone.ScaleX == nil {
			scale = scaleY
		}
		if scaleX != scaleY {
			c.notef("bone %q: scales the axes differently (%v, %v), and %v applies to both axes", bone.Name, scaleX, scaleY, scale)
		}

		c.joints[bone.Name] = uint8(i)
		skeleton.Joints = append(skeleton.Joints, &schemas.Skeletal2DJointT{
			TranslateX: float32(bone.X),
			TranslateY: float32(bone.Y),
			Rotation:   degreesToRadians(bone.Rotation),
			Scale:      float32(scale),
			Parent:     parent,
		})
		debug.Joints = append(debug.Joints, bone.Name)
	}

	return skeleton, nil
}

func (c *converter) convertSlots(debug *schemas.Skeletal2DDebugT) error {
	keys := newKeyTable("slot")
	for i, slot := range c.doc.Slots {
		if slot == nil || slot.Name == "" {
			return fmt.Errorf("slot #%d has no name", i)
		}
		if _, ok := c.slotKeys[slot.Name]; ok {
			return fmt.Errorf("slot %q is defined more than once", slot.Name)
		}
		if _, ok := c.joints[slot.Bone]; !ok {
			return fmt.Errorf("slot %q: unknown bone %q", slot.Name, slot.Bone)
		}

		key, err := keys.add(slot.Name)
		if err != nil {
			return err
		}
		c.slotKeys[slot.Name] = key
		debug.Slots = append(debug.Slots, &schemas.Skeletal2DNameDebugT{Key: key, Name: slot.Name})

		if slot.Color != nil && !strings.EqualFold(*slot.Color, "ffffffff") {
			c.notef("slot %q: color %q is not supported and was dropped", slot.Name, *slot.Color)
		}
	}
	return nil
}

func (c *converter) convertSkins(debug *schemas.Skeletal2DDebugT) (*schemas.Skeletal2DSkinSetT, error) {
	type entry struct {
		key  uint32
		skin *schemas.Skeletal2DSkinT
	}
	entries := []entry{}

	keys := newKeyTable("skin")
	for _, name := range sortedKeys(c.doc.Skins) {
		key, err := keys.add(name)
		if err != nil {
			return nil, err
		}
		skin, err := c.convertSkin(name)
		if err != nil {
			return nil, err
		}
		entries = append(entries, entry{key: key, skin: skin})
	}

	// NOTE: Sort by key for binary search.
	sort.Slice(entries, func(a, b int) bool {
		return entries[a].key < entries[b].key
	})

	skinSet := &schemas.Skeletal2DSkinSetT{}
	for _, e := range entries {
		skinSet.Keys = append(skinSet.Keys, e.key)
		skinSet.Skins = append(skinSet.Skins, e.skin)
	}
	debug.Skins = keys.sorted()
	return skinSet, nil
}

// convertSkin lists the slots in draw order with the attachments that they
// show in the setup pose.
func (c *converter) convertSkin(skinName string) (*schemas.Skeletal2DSkinT, error) {
	skin := &schemas.Skeletal2DSkinT{}
	for _, slot := range c.doc.Slots {
		if slot.Attachment == nil {
			// NOTE: The slot shows nothing in the setup pose.
			continue
		}
		attachmentName := *slot.Attachment

		attachment := c.findAttachment(skinName, slot.Name, attachmentName)
		if attachment == nil {
			c.notef("skin %q: slot %q shows nothing because the skin has no attachment %q", skinName, slot.Name, attachmentName)
			continue
		}
		where := fmt.Sprintf("skin %q: slot %q: attachment %q", skinName, slot.Name, attachmentName)

		attachmentKey, err := c.attachments.add(attachmentName)
		if err != nil {
			return nil, err
		}

		skinSlot := &schemas.Skeletal2DSkinSlotT{
			SlotKey:       c.slotKeys[slot.Name],
			AttachmentKey: attachmentKey,
			ScaleX:        1,
			ScaleY:        1,
			Mesh:          noMesh,
			Joint:         c.joints[slot.Bone],
		}

		switch attachment.Type {
		case "", "region":
			skinSlot.TranslateX = float32(attachment.X)
			skinSlot.TranslateY = float32(attachment.Y)
			skinSlot.ScaleX = float32(valueOr(attachment.ScaleX, 1))
			skinSlot.ScaleY = float32(valueOr(attachment.ScaleY, 1))
			skinSlot.Rotation = degreesToRadians(attachment.Rotation)
		case "skinnedmesh":
			mesh, err := c.convertMesh(attachment, where)
			if err != nil {
				return nil, err
			}
			if len(skin.Meshes) >= noMesh {
				return nil, fmt.Errorf("skin %q has too many meshes, the limit is %d", skinName, noMesh)
			}
			skinSlot.Mesh = uint16(len(skin.Meshes))
			skin.Meshes = append(skin.Meshes, mesh)
		default:
			return nil, fmt.Errorf("%s: type %q is not supported", where, attachment.Type)
		}

		skin.Slots = append(skin.Slots, skinSlot)
	}
	return skin, nil
}

// findAttachment returns the attachment of a slot in a skin, falling back to
// the default skin as Spine does, or nil when neither skin has it.
func (c *converter) findAttachment(skinName, slotName, attachmentName string) *spineAttachment {
	if attachment := c.doc.Skins[skinName][slotName][attachmentName]; attachment != nil {
		return attachment
	}
	if skinName == defaultSkinName {
		return nil
	}
	return c.doc.Skins[defaultSkinName][slotName][attachmentName]
}

// convertMesh converts a "skinnedmesh" attachment. Its "vertices" hold, for
// each vertex, the number of bones that influence it, and then the bone index,
// the position in the local space of the bone, and the weight for each bone.
func (c *converter) convertMesh(attachment *spineAttachment, where string) (*schemas.Skeletal2DMeshT, error) {
	if len(attachment.UVs) == 0 || len(attachment.UVs)%2 != 0 {
		return nil, fmt.Errorf(`%s: "uvs" must have a pair of numbers for each vertex, but has %d numbers`, where, len(attachment.UVs))
	}
	vertexCount := len(attachment.UVs) / 2
	if vertexCount > math.MaxUint16 {
		return nil, fmt.Errorf("%s: has %d vertices, but the limit is %d", where, vertexCount, math.MaxUint16)
	}

	type influence struct {
		joint  byte
		x      float32
		y      float32
		weight float32
	}

	mesh := &schemas.Skeletal2DMeshT{}
	rest := attachment.Vertices
	droppedCount := 0

	for i := 0; i < vertexCount; i++ {
		if len(rest) == 0 {
			return nil, fmt.Errorf(`%s: "vertices" ends before vertex #%d`, where, i)
		}
		count, ok := toInteger(rest[0])
		rest = rest[1:]
		if !ok || count < 1 {
			return nil, fmt.Errorf("%s: vertex #%d needs at least one bone", where, i)
		}
		if count > len(rest)/4 {
			return nil, fmt.Errorf(`%s: "vertices" ends in the middle of vertex #%d`, where, i)
		}

		influences := make([]influence, count)
		for j := range influences {
			bone, ok := toInteger(rest[0])
			if !ok || bone < 0 || bone >= len(c.doc.Bones) {
				return nil, fmt.Errorf("%s: vertex #%d: unknown bone index %v", where, i, rest[0])
			}
			influences[j] = influence{
				joint:  byte(bone),
				x:      float32(rest[1]),
				y:      float32(rest[2]),
				weight: float32(rest[3]),
			}
			rest = rest[4:]
		}

		// NOTE: Keep the joints with the highest weights. The sort is stable
		// so that equal weights keep the order of the source.
		sort.SliceStable(influences, func(a, b int) bool {
			return influences[a].weight > influences[b].weight
		})
		if count > maxInfluenceCount {
			droppedCount++
		}
		if count >= maxInfluenceCount {
			// NOTE: Spread the weight of the dropped joints over the kept ones.
			accumulated := float32(0)
			for _, v := range influences[:maxInfluenceCount] {
				accumulated += v.weight
			}
			if accumulated < 1 {
				diff := (1 - accumulated) / maxInfluenceCount
				for j := range influences {
					influences[j].weight += diff
				}
			}
		}

		joints := [maxInfluenceCount]byte{noJoint, noJoint, noJoint, noJoint}
		weights := [maxInfluenceCount]float32{}
		for j := 0; j < min(count, maxInfluenceCount); j++ {
			joints[j] = influences[j].joint
			weights[j] = influences[j].weight
		}

		// NOTE: The last weight takes the remainder so that the weights add up to 1.
		weights[3] = 1 - (weights[0] + weights[1] + weights[2])

		mesh.Vertices = append(mesh.Vertices, &schemas.Skeletal2DMeshVertexT{
			PositionX: influences[0].x,
			PositionY: influences[0].y,
			TexcoordU: float32(attachment.UVs[i*2]),
			TexcoordV: float32(attachment.UVs[i*2+1]),
			Weight0:   weights[0],
			Weight1:   weights[1],
			Weight2:   weights[2],
			Weight3:   weights[3],
			Joint0:    joints[0],
			Joint1:    joints[1],
			Joint2:    joints[2],
			Joint3:    joints[3],
		})
	}
	if len(rest) != 0 {
		return nil, fmt.Errorf(`%s: "vertices" has %d numbers left over after %d vertices`, where, len(rest), vertexCount)
	}

	if len(attachment.Triangles) == 0 || len(attachment.Triangles)%3 != 0 {
		return nil, fmt.Errorf(`%s: "triangles" must have three indices for each triangle, but has %d indices`, where, len(attachment.Triangles))
	}
	for _, value := range attachment.Triangles {
		index, ok := toInteger(value)
		if !ok || index < 0 || index >= vertexCount {
			return nil, fmt.Errorf(`%s: "triangles" refers to unknown vertex %v`, where, value)
		}
		mesh.Indices = append(mesh.Indices, uint16(index))
	}

	if droppedCount > 0 {
		c.notef("%s: %d vertices have more than %d bones, and the bones with the lowest weights were dropped", where, droppedCount, maxInfluenceCount)
	}
	return mesh, nil
}

func (c *converter) convertClips(debug *schemas.Skeletal2DDebugT) (*schemas.Skeletal2DClipSetT, error) {
	type entry struct {
		key  uint32
		clip *schemas.Skeletal2DClipT
	}
	entries := []entry{}

	keys := newKeyTable("animation")
	for _, name := range sortedKeys(c.doc.Animations) {
		key, err := keys.add(name)
		if err != nil {
			return nil, err
		}
		clip, err := c.convertClip(name, c.doc.Animations[name])
		if err != nil {
			return nil, err
		}
		entries = append(entries, entry{key: key, clip: clip})
	}

	// NOTE: Sort by key for binary search.
	sort.Slice(entries, func(a, b int) bool {
		return entries[a].key < entries[b].key
	})

	clipSet := &schemas.Skeletal2DClipSetT{}
	for _, e := range entries {
		clipSet.Keys = append(clipSet.Keys, e.key)
		clipSet.Clips = append(clipSet.Clips, e.clip)
	}
	debug.Clips = keys.sorted()
	return clipSet, nil
}

func (c *converter) convertClip(name string, animation *spineAnimation) (*schemas.Skeletal2DClipT, error) {
	where := fmt.Sprintf("animation %q", name)
	for _, section := range animation.Unsupported {
		c.notef("%s: %q is not supported and was dropped", where, section)
	}

	clip := &schemas.Skeletal2DClipT{}
	nonLinearCount := 0

	// NOTE: A Go map has no order, so emit the tracks in joint order.
	boneNames := sortedKeys(animation.Bones)
	for _, boneName := range boneNames {
		if _, ok := c.joints[boneName]; !ok {
			return nil, fmt.Errorf("%s: unknown bone %q", where, boneName)
		}
	}
	sort.SliceStable(boneNames, func(a, b int) bool {
		return c.joints[boneNames[a]] < c.joints[boneNames[b]]
	})

	for _, boneName := range boneNames {
		joint := c.joints[boneName]
		timelines := animation.Bones[boneName]
		for _, kind := range sortedKeys(timelines) {
			timelineWhere := fmt.Sprintf("%s: bone %q: %q", where, boneName, kind)
			count := 0
			var err error

			switch kind {
			case "rotate":
				var keys []*schemas.Skeletal2DRotationKeyT
				keys, count, err = convertRotationKeys(timelines[kind])
				if len(keys) > 0 {
					clip.RotationTracks = append(clip.RotationTracks, &schemas.Skeletal2DRotationTrackT{Joint: joint, Keys: keys})
				}
			case "scale":
				var keys []*schemas.Skeletal2DScaleKeyT
				nonUniformCount := 0
				keys, count, nonUniformCount, err = convertScaleKeys(timelines[kind])
				if len(keys) > 0 {
					clip.ScaleTracks = append(clip.ScaleTracks, &schemas.Skeletal2DScaleTrackT{Joint: joint, Keys: keys})
				}
				if nonUniformCount > 0 {
					c.notef("%s: %d keys scale the axes differently, and their y scale applies to both axes", timelineWhere, nonUniformCount)
				}
			case "translate":
				var keys []*schemas.Skeletal2DTranslationKeyT
				keys, count, err = convertTranslationKeys(timelines[kind])
				if len(keys) > 0 {
					clip.TranslationTracks = append(clip.TranslationTracks, &schemas.Skeletal2DTranslationTrackT{Joint: joint, Keys: keys})
				}
			default:
				c.notef("%s is not supported and was dropped", timelineWhere)
			}
			if err != nil {
				return nil, fmt.Errorf("%s: %w", timelineWhere, err)
			}
			nonLinearCount += count
		}
	}

	// NOTE: Emit the attachment tracks in draw order.
	slotNames := sortedKeys(animation.Slots)
	for _, slotName := range slotNames {
		if _, ok := c.slotKeys[slotName]; !ok {
			return nil, fmt.Errorf("%s: unknown slot %q", where, slotName)
		}
	}
	slotOrder := map[string]int{}
	for i, slot := range c.doc.Slots {
		slotOrder[slot.Name] = i
	}
	sort.SliceStable(slotNames, func(a, b int) bool {
		return slotOrder[slotNames[a]] < slotOrder[slotNames[b]]
	})

	for _, slotName := range slotNames {
		timelines := animation.Slots[slotName]
		for _, kind := range sortedKeys(timelines) {
			timelineWhere := fmt.Sprintf("%s: slot %q: %q", where, slotName, kind)
			if kind != "attachment" {
				c.notef("%s is not supported and was dropped", timelineWhere)
				continue
			}

			keys, hiddenCount, err := c.convertAttachmentKeys(timelines[kind])
			if err != nil {
				return nil, fmt.Errorf("%s: %w", timelineWhere, err)
			}
			if len(keys) > 0 {
				clip.AttachmentTracks = append(clip.AttachmentTracks, &schemas.Skeletal2DAttachmentTrackT{SlotKey: c.slotKeys[slotName], Keys: keys})
			}
			if hiddenCount > 0 {
				c.notef("%s: %d keys that hide the attachment were dropped", timelineWhere, hiddenCount)
			}
		}
	}

	if nonLinearCount > 0 {
		c.notef("%s: %d keys have a stepped or Bezier curve, which plays as a linear one", where, nonLinearCount)
	}
	return clip, nil
}

// convertRotationKeys returns the keys in ascending order of time, and the
// number of keys whose curve is not linear.
func convertRotationKeys(data json.RawMessage) ([]*schemas.Skeletal2DRotationKeyT, int, error) {
	var source []spineRotateKey
	if err := json.Unmarshal(data, &source); err != nil {
		return nil, 0, err
	}

	keys := make([]*schemas.Skeletal2DRotationKeyT, 0, len(source))
	nonLinearCount := 0
	for i, key := range source {
		time, err := keyTime(key.Time, i)
		if err != nil {
			return nil, 0, err
		}
		if !isLinearCurve(key.Curve) {
			nonLinearCount++
		}
		keys = append(keys, &schemas.Skeletal2DRotationKeyT{
			Time:     time,
			Rotation: degreesToRadians(key.Angle),
		})
	}
	sort.SliceStable(keys, func(a, b int) bool {
		return keys[a].Time < keys[b].Time
	})
	return keys, nonLinearCount, nil
}

// convertScaleKeys returns the keys in ascending order of time, the number of
// keys whose curve is not linear, and the number of keys that scale the axes
// differently.
func convertScaleKeys(data json.RawMessage) ([]*schemas.Skeletal2DScaleKeyT, int, int, error) {
	var source []spineScaleKey
	if err := json.Unmarshal(data, &source); err != nil {
		return nil, 0, 0, err
	}

	keys := make([]*schemas.Skeletal2DScaleKeyT, 0, len(source))
	nonLinearCount := 0
	nonUniformCount := 0
	for i, key := range source {
		time, err := keyTime(key.Time, i)
		if err != nil {
			return nil, 0, 0, err
		}
		if !isLinearCurve(key.Curve) {
			nonLinearCount++
		}

		// NOTE: A joint scales both axes by the same factor, and "y" takes
		// precedence over "x".
		scaleX, scaleY := valueOr(key.X, 1), valueOr(key.Y, 1)
		scale := scaleY
		if key.Y == nil {
			scale = scaleX
		}
		if scaleX != scaleY {
			nonUniformCount++
		}
		if math.Abs(scale) > maxKeyScale {
			return nil, 0, 0, fmt.Errorf("key #%d: scale %v is out of the range [-%d, %d]", i, scale, maxKeyScale, maxKeyScale)
		}

		keys = append(keys, &schemas.Skeletal2DScaleKeyT{
			Time:  time,
			Scale: float32(scale),
		})
	}
	sort.SliceStable(keys, func(a, b int) bool {
		return keys[a].Time < keys[b].Time
	})
	return keys, nonLinearCount, nonUniformCount, nil
}

// convertTranslationKeys returns the keys in ascending order of time, and the
// number of keys whose curve is not linear.
func convertTranslationKeys(data json.RawMessage) ([]*schemas.Skeletal2DTranslationKeyT, int, error) {
	var source []spineTranslateKey
	if err := json.Unmarshal(data, &source); err != nil {
		return nil, 0, err
	}

	keys := make([]*schemas.Skeletal2DTranslationKeyT, 0, len(source))
	nonLinearCount := 0
	for i, key := range source {
		time, err := keyTime(key.Time, i)
		if err != nil {
			return nil, 0, err
		}
		if !isLinearCurve(key.Curve) {
			nonLinearCount++
		}
		if math.Abs(key.X) > maxKeyTranslation || math.Abs(key.Y) > maxKeyTranslation {
			return nil, 0, fmt.Errorf("key #%d: translation (%v, %v) is out of the range [-%d, %d]", i, key.X, key.Y, maxKeyTranslation, maxKeyTranslation)
		}
		keys = append(keys, &schemas.Skeletal2DTranslationKeyT{
			Time:       time,
			TranslateX: float32(key.X),
			TranslateY: float32(key.Y),
		})
	}
	sort.SliceStable(keys, func(a, b int) bool {
		return keys[a].Time < keys[b].Time
	})
	return keys, nonLinearCount, nil
}

// convertAttachmentKeys returns the keys in ascending order of time, and the
// number of dropped keys, which hide the attachment of the slot.
func (c *converter) convertAttachmentKeys(data json.RawMessage) ([]*schemas.Skeletal2DAttachmentKeyT, int, error) {
	var source []spineAttachmentKey
	if err := json.Unmarshal(data, &source); err != nil {
		return nil, 0, err
	}

	keys := make([]*schemas.Skeletal2DAttachmentKeyT, 0, len(source))
	hiddenCount := 0
	for i, key := range source {
		time, err := keyTime(key.Time, i)
		if err != nil {
			return nil, 0, err
		}
		if key.Name == nil {
			// NOTE: The runtime cannot hide the attachment of a slot.
			hiddenCount++
			continue
		}
		attachmentKey, err := c.attachments.add(*key.Name)
		if err != nil {
			return nil, 0, err
		}
		keys = append(keys, &schemas.Skeletal2DAttachmentKeyT{
			Time:          time,
			AttachmentKey: attachmentKey,
		})
	}
	sort.SliceStable(keys, func(a, b int) bool {
		return keys[a].Time < keys[b].Time
	})
	return keys, hiddenCount, nil
}

// keyTime returns the time of the key at index in seconds.
func keyTime(time *float64, index int) (float32, error) {
	if time == nil {
		return 0, fmt.Errorf("key #%d has no time", index)
	}
	if *time < 0 {
		return 0, fmt.Errorf("key #%d: time %v must not be negative", index, *time)
	}
	return float32(*time), nil
}

// degreesToRadians converts an angle in degrees to radians within [-pi, pi].
func degreesToRadians(degrees float64) float32 {
	degrees = math.Mod(degrees, 360)
	if degrees > 180 {
		degrees -= 360
	} else if degrees < -180 {
		degrees += 360
	}
	return float32(degrees * (math.Pi / 180))
}

// toInteger converts a JSON number to an integer, and reports whether the
// number is one.
func toInteger(value float64) (int, bool) {
	if value != math.Trunc(value) || math.Abs(value) > math.MaxInt32 {
		return 0, false
	}
	return int(value), true
}

// valueOr returns the value that p points to, or fallback when p is nil.
func valueOr(p *float64, fallback float64) float64 {
	if p == nil {
		return fallback
	}
	return *p
}

// sortedKeys returns the keys of m in ascending order.
func sortedKeys[V any](m map[string]V) []string {
	keys := make([]string, 0, len(m))
	for key := range m {
		keys = append(keys, key)
	}
	sort.Strings(keys)
	return keys
}

// keyTable maps the hash keys of one kind of name back to the names, and
// detects hash collisions.
type keyTable struct {
	kind  string
	names map[uint32]string
}

func newKeyTable(kind string) *keyTable {
	return &keyTable{kind: kind, names: map[uint32]string{}}
}

// add returns the XXH32 hash of name, or an error when another name of the
// table has the same hash.
func (t *keyTable) add(name string) (uint32, error) {
	key := stringhash.StringToHash32(name)
	if other, ok := t.names[key]; ok && other != name {
		return 0, fmt.Errorf("hash collision: XXH32 key %#x for %s %q and %q", key, t.kind, other, name)
	}
	t.names[key] = name
	return key, nil
}

// sorted returns the names of the table in ascending order of their keys.
func (t *keyTable) sorted() []*schemas.Skeletal2DNameDebugT {
	entries := make([]*schemas.Skeletal2DNameDebugT, 0, len(t.names))
	for key, name := range t.names {
		entries = append(entries, &schemas.Skeletal2DNameDebugT{Key: key, Name: name})
	}
	sort.Slice(entries, func(a, b int) bool {
		return entries[a].Key < entries[b].Key
	})
	return entries
}

// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"math"
	"sort"

	schemas "github.com/mogemimi/pomdog/build/schemas"
)

const (
	// NOTE: The runtime reserves memory for this many particles at most.
	maxParticlesLimit = 4096

	// maxColorChannel is the largest value of an 8-bit color channel.
	maxColorChannel = 255
)

// degreesToRadiansScale is the factor that the runtime uses in
// math::toRadian(f32). The division runs in float32, as it does in C++, so
// that the converted angles are the same as the angles that the runtime
// computed from the JSON source before.
var degreesToRadiansScale = func() float32 {
	pi := float32(math.Pi)
	return pi / 180
}()

// valueKind is how the values of a parameter are written in the source.
type valueKind int

const (
	// floatValue is a number.
	floatValue valueKind = iota

	// angleValue is a number of degrees, which is converted to radians.
	angleValue

	// colorValue is an array of four channels within [0, 255].
	colorValue
)

// convert validates a particle clip and converts it to the table to write.
func convert(src *clipJSON) (*schemas.ParticlesClipT, error) {
	clip := &schemas.ParticlesClipT{}

	duration, err := requireNumber(src.Duration, "duration")
	if err != nil {
		return nil, err
	}
	if !(duration > 0) || math.IsInf(duration, 0) {
		return nil, fmt.Errorf(`"duration" must be greater than zero, but is %v`, duration)
	}
	clip.Duration = duration

	if clip.EmissionRate, err = requirePositiveInt32(src.EmissionRate, "emission_rate"); err != nil {
		return nil, err
	}
	if clip.EmissionRateOverTime, err = requirePositiveInt32(src.EmissionRateOverTime, "emission_rate_over_time"); err != nil {
		return nil, err
	}
	if clip.MaxParticles, err = requirePositiveInt32(src.MaxParticles, "max_particles"); err != nil {
		return nil, err
	}
	if clip.MaxParticles > maxParticlesLimit {
		return nil, fmt.Errorf(`"max_particles" must be within [1, %d], but is %d`, maxParticlesLimit, clip.MaxParticles)
	}

	if clip.StartLifetime, err = requireFloat32(src.StartLifetime, "start_lifetime"); err != nil {
		return nil, err
	}
	if !(clip.StartLifetime > 0) {
		return nil, fmt.Errorf(`"start_lifetime" must be greater than zero, but is %v`, clip.StartLifetime)
	}
	if clip.StartDelay, err = requireFloat32(src.StartDelay, "start_delay"); err != nil {
		return nil, err
	}
	if clip.GravityModifier, err = requireFloat32(src.GravityModifier, "gravity_modifier"); err != nil {
		return nil, err
	}

	if src.Looping == nil {
		return nil, fmt.Errorf(`"looping" is required`)
	}
	clip.Looping = *src.Looping

	if clip.Shape, err = convertShape(src.Shape); err != nil {
		return nil, fmt.Errorf(`"shape": %w`, err)
	}

	floatParameters := []struct {
		name   string
		source *parameterJSON
		kind   valueKind
		target **schemas.ParticlesFloatParameterT
	}{
		{"start_speed", src.StartSpeed, floatValue, &clip.StartSpeed},
		{"start_rotation", src.StartRotation, angleValue, &clip.StartRotation},
		{"rotation_over_lifetime", src.RotationOverLifetime, angleValue, &clip.RotationOverLifetime},
		{"start_size", src.StartSize, floatValue, &clip.StartSize},
		{"size_over_lifetime", src.SizeOverLifetime, floatValue, &clip.SizeOverLifetime},
	}
	for _, p := range floatParameters {
		if *p.target, err = convertFloatParameter(p.source, p.kind); err != nil {
			return nil, fmt.Errorf(`"%s": %w`, p.name, err)
		}
	}

	colorParameters := []struct {
		name   string
		source *parameterJSON
		target **schemas.ParticlesColorParameterT
	}{
		{"start_color", src.StartColor, &clip.StartColor},
		{"color_over_lifetime", src.ColorOverLifetime, &clip.ColorOverLifetime},
	}
	for _, p := range colorParameters {
		if *p.target, err = convertColorParameter(p.source); err != nil {
			return nil, fmt.Errorf(`"%s": %w`, p.name, err)
		}
	}

	return clip, nil
}

func convertShape(src *shapeJSON) (*schemas.ParticlesShapeT, error) {
	if src == nil {
		return nil, fmt.Errorf("is required")
	}

	// NOTE: Each type uses a set of the fields; the rest must be absent.
	fields := map[string]*float64{
		"width":  src.Width,
		"height": src.Height,
		"depth":  src.Depth,
		"radius": src.Radius,
		"angle":  src.Angle,
	}
	var required, optional []string
	switch src.Type {
	case "box":
		required, optional = []string{"width", "height"}, []string{"depth"}
	case "cone":
		required = []string{"radius", "height"}
	case "hemisphere", "sphere":
		required = []string{"radius"}
	case "sector":
		required = []string{"angle"}
	default:
		return nil, fmt.Errorf("unknown type %q", src.Type)
	}

	used := map[string]bool{}
	values := map[string]float32{}
	for _, name := range append(append([]string{}, required...), optional...) {
		used[name] = true
		value := fields[name]
		if value == nil {
			if contains(optional, name) {
				values[name] = 0
				continue
			}
			return nil, fmt.Errorf("a %s needs %q", src.Type, name)
		}
		v, err := toFloat32(*value, name)
		if err != nil {
			return nil, err
		}
		if v < 0 {
			return nil, fmt.Errorf("%q must not be negative, but is %v", name, v)
		}
		values[name] = v
	}
	for _, name := range sortedKeys(fields) {
		if fields[name] != nil && !used[name] {
			return nil, fmt.Errorf("a %s does not use %q", src.Type, name)
		}
	}

	switch src.Type {
	case "box":
		return &schemas.ParticlesShapeT{
			Type:  schemas.ParticlesShapeParticlesShapeBox,
			Value: &schemas.ParticlesShapeBoxT{Width: values["width"], Height: values["height"], Depth: values["depth"]},
		}, nil
	case "cone":
		return &schemas.ParticlesShapeT{
			Type:  schemas.ParticlesShapeParticlesShapeCone,
			Value: &schemas.ParticlesShapeConeT{Radius: values["radius"], Height: values["height"]},
		}, nil
	case "hemisphere":
		return &schemas.ParticlesShapeT{
			Type:  schemas.ParticlesShapeParticlesShapeHemisphere,
			Value: &schemas.ParticlesShapeHemisphereT{Radius: values["radius"]},
		}, nil
	case "sphere":
		return &schemas.ParticlesShapeT{
			Type:  schemas.ParticlesShapeParticlesShapeSphere,
			Value: &schemas.ParticlesShapeSphereT{Radius: values["radius"]},
		}, nil
	default:
		degrees := values["angle"]
		if degrees > 360 {
			return nil, fmt.Errorf(`"angle" must be within [0, 360] degrees, but is %v`, degrees)
		}
		return &schemas.ParticlesShapeT{
			Type:  schemas.ParticlesShapeParticlesShapeSector,
			Value: &schemas.ParticlesShapeSectorT{Angle: degrees * degreesToRadiansScale},
		}, nil
	}
}

func convertFloatParameter(src *parameterJSON, kind valueKind) (*schemas.ParticlesFloatParameterT, error) {
	if err := checkParameterFields(src); err != nil {
		return nil, err
	}

	parse := func(raw json.RawMessage, name string) (float32, error) {
		var value float64
		if err := decodeValue(raw, &value); err != nil {
			return 0, fmt.Errorf("%q must be a number: %w", name, err)
		}
		v, err := toFloat32(value, name)
		if err != nil {
			return 0, err
		}
		if kind == angleValue {
			v *= degreesToRadiansScale
		}
		return v, nil
	}

	switch src.Type {
	case "constant":
		value, err := parse(src.Value, "value")
		if err != nil {
			return nil, err
		}
		return &schemas.ParticlesFloatParameterT{
			Type:  schemas.ParticlesFloatParameterParticlesFloatConstant,
			Value: &schemas.ParticlesFloatConstantT{Value: value},
		}, nil
	case "random":
		min, err := parse(src.Min, "min")
		if err != nil {
			return nil, err
		}
		max, err := parse(src.Max, "max")
		if err != nil {
			return nil, err
		}
		return &schemas.ParticlesFloatParameterT{
			Type:  schemas.ParticlesFloatParameterParticlesFloatRandom,
			Value: &schemas.ParticlesFloatRandomT{Min: min, Max: max},
		}, nil
	default:
		keys := make([]*schemas.ParticlesFloatKeyT, 0, len(*src.Points))
		for i, point := range *src.Points {
			time, err := keyTime(point.T, i)
			if err != nil {
				return nil, err
			}
			value, err := parse(point.V, "v")
			if err != nil {
				return nil, fmt.Errorf("point #%d: %w", i, err)
			}
			keys = append(keys, &schemas.ParticlesFloatKeyT{Time: time, Value: value})
		}
		sort.SliceStable(keys, func(a, b int) bool { return keys[a].Time < keys[b].Time })
		return &schemas.ParticlesFloatParameterT{
			Type:  schemas.ParticlesFloatParameterParticlesFloatCurve,
			Value: &schemas.ParticlesFloatCurveT{Keys: keys},
		}, nil
	}
}

func convertColorParameter(src *parameterJSON) (*schemas.ParticlesColorParameterT, error) {
	if err := checkParameterFields(src); err != nil {
		return nil, err
	}

	switch src.Type {
	case "constant":
		value, err := parseColor(src.Value, "value")
		if err != nil {
			return nil, err
		}
		return &schemas.ParticlesColorParameterT{
			Type:  schemas.ParticlesColorParameterParticlesColorConstant,
			Value: &schemas.ParticlesColorConstantT{Value: value},
		}, nil
	case "random":
		min, err := parseColor(src.Min, "min")
		if err != nil {
			return nil, err
		}
		max, err := parseColor(src.Max, "max")
		if err != nil {
			return nil, err
		}
		return &schemas.ParticlesColorParameterT{
			Type:  schemas.ParticlesColorParameterParticlesColorRandom,
			Value: &schemas.ParticlesColorRandomT{Min: min, Max: max},
		}, nil
	default:
		keys := make([]*schemas.ParticlesColorKeyT, 0, len(*src.Points))
		for i, point := range *src.Points {
			time, err := keyTime(point.T, i)
			if err != nil {
				return nil, err
			}
			value, err := parseColor(point.V, "v")
			if err != nil {
				return nil, fmt.Errorf("point #%d: %w", i, err)
			}
			keys = append(keys, &schemas.ParticlesColorKeyT{Time: time, Value: value})
		}
		sort.SliceStable(keys, func(a, b int) bool { return keys[a].Time < keys[b].Time })
		return &schemas.ParticlesColorParameterT{
			Type:  schemas.ParticlesColorParameterParticlesColorCurve,
			Value: &schemas.ParticlesColorCurveT{Keys: keys},
		}, nil
	}
}

// checkParameterFields verifies that a parameter has the fields of its type
// and no others.
func checkParameterFields(src *parameterJSON) error {
	if src == nil {
		return fmt.Errorf("is required")
	}

	present := map[string]bool{
		"value":  src.Value != nil,
		"min":    src.Min != nil,
		"max":    src.Max != nil,
		"points": src.Points != nil,
	}
	var required []string
	switch src.Type {
	case "constant":
		required = []string{"value"}
	case "random":
		required = []string{"min", "max"}
	case "curve":
		required = []string{"points"}
	default:
		return fmt.Errorf("unknown type %q", src.Type)
	}

	for _, name := range required {
		if !present[name] {
			return fmt.Errorf("a %s parameter needs %q", src.Type, name)
		}
	}
	for _, name := range sortedKeys(present) {
		if present[name] && !contains(required, name) {
			return fmt.Errorf("a %s parameter does not use %q", src.Type, name)
		}
	}
	if src.Type == "curve" && len(*src.Points) == 0 {
		return fmt.Errorf(`a curve needs at least one point`)
	}
	return nil
}

func parseColor(raw json.RawMessage, name string) (*schemas.ParticlesColorT, error) {
	var channels []float64
	if err := decodeValue(raw, &channels); err != nil || len(channels) != 4 {
		return nil, fmt.Errorf("%q must be an array of four channels", name)
	}
	var values [4]uint8
	for i, channel := range channels {
		if channel != math.Trunc(channel) || channel < 0 || channel > maxColorChannel {
			return nil, fmt.Errorf("%q has a channel %v, which is not an integer within [0, %d]", name, channel, maxColorChannel)
		}
		values[i] = uint8(channel)
	}
	return &schemas.ParticlesColorT{R: values[0], G: values[1], B: values[2], A: values[3]}, nil
}

// decodeValue decodes a JSON value that is not null.
func decodeValue(raw json.RawMessage, v any) error {
	if bytes.Equal(bytes.TrimSpace(raw), []byte("null")) {
		return fmt.Errorf("null is not a value")
	}
	return json.Unmarshal(raw, v)
}

// keyTime returns the time of the point at index, which is within [0, 1].
func keyTime(t *float64, index int) (float32, error) {
	if t == nil {
		return 0, fmt.Errorf(`point #%d has no "t"`, index)
	}
	if !(*t >= 0 && *t <= 1) {
		return 0, fmt.Errorf(`point #%d: "t" must be within [0, 1], but is %v`, index, *t)
	}
	return float32(*t), nil
}

func requireNumber(value *float64, name string) (float64, error) {
	if value == nil {
		return 0, fmt.Errorf("%q is required", name)
	}
	return *value, nil
}

func requireFloat32(value *float64, name string) (float32, error) {
	v, err := requireNumber(value, name)
	if err != nil {
		return 0, err
	}
	return toFloat32(v, name)
}

// requirePositiveInt32 returns a field that is an integer greater than zero.
func requirePositiveInt32(value *float64, name string) (int32, error) {
	v, err := requireNumber(value, name)
	if err != nil {
		return 0, err
	}
	if v != math.Trunc(v) || v < 1 || v > math.MaxInt32 {
		return 0, fmt.Errorf("%q must be an integer greater than zero, but is %v", name, v)
	}
	return int32(v), nil
}

// toFloat32 converts a number to float32, and fails when float32 cannot
// represent its magnitude.
func toFloat32(value float64, name string) (float32, error) {
	v := float32(value)
	if math.IsInf(float64(v), 0) || math.IsNaN(float64(v)) {
		return 0, fmt.Errorf("%q is %v, which is out of the range of float32", name, value)
	}
	return v, nil
}

func contains(values []string, value string) bool {
	for _, v := range values {
		if v == value {
			return true
		}
	}
	return false
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

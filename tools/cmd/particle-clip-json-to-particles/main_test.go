// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"math"
	"os"
	"path/filepath"
	"strings"
	"testing"

	schemas "github.com/mogemimi/pomdog/build/schemas"
)

// NOTE: The curve of "size_over_lifetime" is out of order, and each
// parameter type appears at least once.
const sampleJSON = `{
  "duration": 0.5,
  "emission_rate": 450,
  "emission_rate_over_time": 100,
  "start_lifetime": 1.8,
  "start_delay": 0.25,
  "gravity_modifier": 980.0,
  "max_particles": 1024,
  "looping": true,
  "shape": {"type": "sector", "angle": 60.0},
  "start_speed": {"type": "random", "min": 100.0, "max": 500.0},
  "start_color": {"type": "constant", "value": [10, 120, 190, 0]},
  "color_over_lifetime": {
    "type": "curve",
    "points": [
      {"t": 0.0, "v": [130, 220, 190, 0]},
      {"t": 1.0, "v": [0, 0, 0, 255]}
    ]
  },
  "start_rotation": {"type": "random", "min": 0.0, "max": 360.0},
  "rotation_over_lifetime": {"type": "constant", "value": -45.0},
  "start_size": {"type": "constant", "value": 0.5},
  "size_over_lifetime": {
    "type": "curve",
    "points": [
      {"t": 1.0, "v": 0.0},
      {"t": 0.0, "v": 0.25},
      {"t": 0.5, "v": 1.0}
    ]
  }
}`

func convertJSON(t *testing.T, source string) (*schemas.ParticlesClipT, error) {
	t.Helper()
	src, err := parseClipJSON([]byte(source))
	if err != nil {
		return nil, err
	}
	return convert(src)
}

func mustConvertJSON(t *testing.T, source string) *schemas.ParticlesClipT {
	t.Helper()
	clip, err := convertJSON(t, source)
	if err != nil {
		t.Fatalf("convert() failed: %v", err)
	}
	return clip
}

// toRadians converts degrees as the runtime did, in float32.
func toRadians(degrees float32) float32 {
	return degrees * degreesToRadiansScale
}

func TestConvert(t *testing.T) {
	clip := mustConvertJSON(t, sampleJSON)

	if clip.Duration != 0.5 || clip.EmissionRate != 450 || clip.EmissionRateOverTime != 100 || clip.MaxParticles != 1024 {
		t.Errorf("unexpected emission fields: %+v", clip)
	}
	if clip.StartLifetime != float32(1.8) || clip.StartDelay != 0.25 || clip.GravityModifier != 980 || !clip.Looping {
		t.Errorf("unexpected scalar fields: %+v", clip)
	}

	t.Run("converts the sector angle to radians", func(t *testing.T) {
		if clip.Shape.Type != schemas.ParticlesShapeParticlesShapeSector {
			t.Fatalf("got shape type %v, want a sector", clip.Shape.Type)
		}
		sector := clip.Shape.Value.(*schemas.ParticlesShapeSectorT)
		if sector.Angle != toRadians(60) {
			t.Errorf("got angle %v, want %v", sector.Angle, toRadians(60))
		}
		if math.Abs(float64(sector.Angle)-math.Pi/3) > 1e-6 {
			t.Errorf("got angle %v, want about pi/3", sector.Angle)
		}
	})

	t.Run("converts rotations to radians", func(t *testing.T) {
		random := clip.StartRotation.Value.(*schemas.ParticlesFloatRandomT)
		if random.Min != 0 || random.Max != toRadians(360) {
			t.Errorf("unexpected start rotation: %+v", random)
		}
		constant := clip.RotationOverLifetime.Value.(*schemas.ParticlesFloatConstantT)
		if constant.Value != toRadians(-45) {
			t.Errorf("got rotation %v, want %v", constant.Value, toRadians(-45))
		}
	})

	t.Run("keeps other values", func(t *testing.T) {
		speed := clip.StartSpeed.Value.(*schemas.ParticlesFloatRandomT)
		if speed.Min != 100 || speed.Max != 500 {
			t.Errorf("unexpected start speed: %+v", speed)
		}
		size := clip.StartSize.Value.(*schemas.ParticlesFloatConstantT)
		if size.Value != 0.5 {
			t.Errorf("unexpected start size: %+v", size)
		}
	})

	t.Run("sorts the keys of a curve", func(t *testing.T) {
		curve := clip.SizeOverLifetime.Value.(*schemas.ParticlesFloatCurveT)
		want := []schemas.ParticlesFloatKeyT{{Time: 0, Value: 0.25}, {Time: 0.5, Value: 1}, {Time: 1, Value: 0}}
		if len(curve.Keys) != len(want) {
			t.Fatalf("got %d keys, want %d", len(curve.Keys), len(want))
		}
		for i, key := range curve.Keys {
			if *key != want[i] {
				t.Errorf("key #%d: got %+v, want %+v", i, *key, want[i])
			}
		}
	})

	t.Run("converts colors", func(t *testing.T) {
		constant := clip.StartColor.Value.(*schemas.ParticlesColorConstantT)
		if *constant.Value != (schemas.ParticlesColorT{R: 10, G: 120, B: 190, A: 0}) {
			t.Errorf("unexpected start color: %+v", *constant.Value)
		}
		curve := clip.ColorOverLifetime.Value.(*schemas.ParticlesColorCurveT)
		if len(curve.Keys) != 2 || *curve.Keys[1].Value != (schemas.ParticlesColorT{R: 0, G: 0, B: 0, A: 255}) {
			t.Errorf("unexpected color curve: %+v", curve.Keys)
		}
	})
}

func TestConvertShapes(t *testing.T) {
	cases := []struct {
		shape string
		check func(t *testing.T, shape *schemas.ParticlesShapeT)
	}{
		{`{"type": "box", "width": 20, "height": 50}`, func(t *testing.T, shape *schemas.ParticlesShapeT) {
			box := shape.Value.(*schemas.ParticlesShapeBoxT)
			if *box != (schemas.ParticlesShapeBoxT{Width: 20, Height: 50, Depth: 0}) {
				t.Errorf("unexpected box: %+v", *box)
			}
		}},
		{`{"type": "box", "width": 0.5, "height": 1, "depth": 0.25}`, func(t *testing.T, shape *schemas.ParticlesShapeT) {
			box := shape.Value.(*schemas.ParticlesShapeBoxT)
			if *box != (schemas.ParticlesShapeBoxT{Width: 0.5, Height: 1, Depth: 0.25}) {
				t.Errorf("unexpected box: %+v", *box)
			}
		}},
		{`{"type": "cone", "radius": 0.5, "height": 1}`, func(t *testing.T, shape *schemas.ParticlesShapeT) {
			cone := shape.Value.(*schemas.ParticlesShapeConeT)
			if *cone != (schemas.ParticlesShapeConeT{Radius: 0.5, Height: 1}) {
				t.Errorf("unexpected cone: %+v", *cone)
			}
		}},
		{`{"type": "hemisphere", "radius": 2}`, func(t *testing.T, shape *schemas.ParticlesShapeT) {
			if shape.Value.(*schemas.ParticlesShapeHemisphereT).Radius != 2 {
				t.Errorf("unexpected hemisphere: %+v", shape.Value)
			}
		}},
		{`{"type": "sphere", "radius": 3}`, func(t *testing.T, shape *schemas.ParticlesShapeT) {
			if shape.Value.(*schemas.ParticlesShapeSphereT).Radius != 3 {
				t.Errorf("unexpected sphere: %+v", shape.Value)
			}
		}},
		{`{"type": "sector", "angle": 360}`, func(t *testing.T, shape *schemas.ParticlesShapeT) {
			if shape.Value.(*schemas.ParticlesShapeSectorT).Angle != toRadians(360) {
				t.Errorf("unexpected sector: %+v", shape.Value)
			}
		}},
	}

	for _, tc := range cases {
		t.Run(tc.shape, func(t *testing.T) {
			source := strings.Replace(sampleJSON, `{"type": "sector", "angle": 60.0}`, tc.shape, 1)
			tc.check(t, mustConvertJSON(t, source).Shape)
		})
	}
}

func TestConvertIsDeterministic(t *testing.T) {
	first := string(packClip(mustConvertJSON(t, sampleJSON)))
	for i := 0; i < 20; i++ {
		if again := string(packClip(mustConvertJSON(t, sampleJSON))); again != first {
			t.Fatalf("conversion #%d produced different bytes", i)
		}
	}
}

func TestConvertErrors(t *testing.T) {
	cases := []struct {
		name    string
		old     string
		new     string
		wantErr string
	}{
		{"unknown field", `"looping": true,`, `"looping": true, "prewarm": false,`, `unknown field "prewarm"`},
		{"missing duration", `"duration": 0.5,`, ``, `"duration" is required`},
		{"zero duration", `"duration": 0.5,`, `"duration": 0,`, `"duration" must be greater than zero`},
		{"fractional emission rate", `"emission_rate": 450,`, `"emission_rate": 4.5,`, `"emission_rate" must be an integer greater than zero`},
		{"zero emission rate over time", `"emission_rate_over_time": 100,`, `"emission_rate_over_time": 0,`, `"emission_rate_over_time" must be an integer greater than zero`},
		{"too many particles", `"max_particles": 1024,`, `"max_particles": 4097,`, `"max_particles" must be within [1, 4096]`},
		{"zero lifetime", `"start_lifetime": 1.8,`, `"start_lifetime": 0,`, `"start_lifetime" must be greater than zero`},
		{"value out of the range of float32", `"gravity_modifier": 980.0,`, `"gravity_modifier": 1e39,`, `out of the range of float32`},
		{"missing looping", `"looping": true,`, ``, `"looping" is required`},
		{"missing shape", `"shape": {"type": "sector", "angle": 60.0},`, ``, `"shape": is required`},
		{"unknown shape", `{"type": "sector", "angle": 60.0}`, `{"type": "torus", "radius": 1}`, `unknown type "torus"`},
		{"shape without a field", `{"type": "sector", "angle": 60.0}`, `{"type": "cone", "radius": 1}`, `a cone needs "height"`},
		{"shape with an unused field", `{"type": "sector", "angle": 60.0}`, `{"type": "sphere", "radius": 1, "height": 2}`, `a sphere does not use "height"`},
		{"negative size", `{"type": "sector", "angle": 60.0}`, `{"type": "box", "width": -1, "height": 2}`, `"width" must not be negative`},
		{"sector angle over a turn", `{"type": "sector", "angle": 60.0}`, `{"type": "sector", "angle": 361}`, `"angle" must be within [0, 360] degrees`},
		{"missing parameter", `"start_size": {"type": "constant", "value": 0.5},`, ``, `"start_size": is required`},
		{"unknown parameter type", `{"type": "constant", "value": 0.5}`, `{"type": "noise", "value": 0.5}`, `unknown type "noise"`},
		{"random without max", `{"type": "random", "min": 100.0, "max": 500.0}`, `{"type": "random", "min": 100.0}`, `a random parameter needs "max"`},
		{"constant with an unused field", `{"type": "constant", "value": 0.5}`, `{"type": "constant", "value": 0.5, "min": 0}`, `a constant parameter does not use "min"`},
		{"null value", `{"type": "constant", "value": 0.5}`, `{"type": "constant", "value": null}`, `"value" must be a number`},
		{"curve without points", `{"type": "constant", "value": 0.5}`, `{"type": "curve", "points": []}`, `a curve needs at least one point`},
		{"point without t", `{"t": 0.5, "v": 1.0}`, `{"v": 1.0}`, `point #2 has no "t"`},
		{"point out of the lifetime", `{"t": 0.5, "v": 1.0}`, `{"t": 1.5, "v": 1.0}`, `"t" must be within [0, 1]`},
		{"color with three channels", `[10, 120, 190, 0]`, `[10, 120, 190]`, `"value" must be an array of four channels`},
		{"color channel out of range", `[10, 120, 190, 0]`, `[10, 120, 256, 0]`, `not an integer within [0, 255]`},
		{"fractional color channel", `[10, 120, 190, 0]`, `[10, 120.5, 190, 0]`, `not an integer within [0, 255]`},
		{"number for a color", `[10, 120, 190, 0]`, `10`, `"value" must be an array of four channels`},
	}

	for _, tc := range cases {
		t.Run(tc.name, func(t *testing.T) {
			if strings.Count(sampleJSON, tc.old) != 1 {
				t.Fatalf("the sample must have %q exactly once", tc.old)
			}
			source := strings.Replace(sampleJSON, tc.old, tc.new, 1)

			_, err := convertJSON(t, source)
			if err == nil {
				t.Fatalf("convert() succeeded, want an error with %q", tc.wantErr)
			}
			if !strings.Contains(err.Error(), tc.wantErr) {
				t.Errorf("got error %q, want it to have %q", err.Error(), tc.wantErr)
			}
		})
	}
}

func TestRunWritesFile(t *testing.T) {
	dir := t.TempDir()
	opts := &options{
		InFile:  filepath.Join(dir, "in.json"),
		OutFile: filepath.Join(dir, "out", "fire.particlesclip"),
	}
	if err := os.WriteFile(opts.InFile, []byte(sampleJSON), 0o644); err != nil {
		t.Fatalf("os.WriteFile() failed: %v", err)
	}
	if err := run(opts); err != nil {
		t.Fatalf("run() failed: %v", err)
	}

	data, err := os.ReadFile(opts.OutFile)
	if err != nil {
		t.Fatalf("os.ReadFile() failed: %v", err)
	}
	clip := schemas.GetRootAsParticlesClip(data, 0)
	if clip.MaxParticles() != 1024 || clip.ShapeType() != schemas.ParticlesShapeParticlesShapeSector {
		t.Errorf("unexpected clip: %d particles, shape %v", clip.MaxParticles(), clip.ShapeType())
	}

	t.Run("a missing input fails", func(t *testing.T) {
		opts := *opts
		opts.InFile = filepath.Join(dir, "missing.json")
		if err := run(&opts); err == nil {
			t.Errorf("run() succeeded, want an error")
		}
	})
}

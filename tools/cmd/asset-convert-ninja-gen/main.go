// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	"path/filepath"
	"runtime"
	"sort"
	"strconv"
	"strings"

	ninja "github.com/mogemimi/pomdog/tools/pkg/ninja"
	toml "github.com/pelletier/go-toml/v2"
)

type Env struct {
	InRecipe      string
	InDir         string
	ThirdPartyDir string
	OutNinjaFile  string
	OutDir        string
	IntDir        string
	ToolDir       string
	Verbose       bool
}

func main() {
	env := &Env{}
	flag.StringVar(&env.InRecipe, "recipe", "", "convert recipe file (*.toml)")
	flag.StringVar(&env.InDir, "indir", "", "input asset directory")
	flag.StringVar(&env.ThirdPartyDir, "thirdpartydir", "", "third-party directory (replaces ${POMDOG_THIRDPARTY_DIR})")
	flag.StringVar(&env.OutNinjaFile, "outninja", "", "output ninja file (build.ninja)")
	flag.StringVar(&env.OutDir, "outdir", "", "output content directory")
	flag.StringVar(&env.IntDir, "intdir", "", "intermediate directory")
	flag.StringVar(&env.ToolDir, "tooldir", "", "tool directory")
	flag.BoolVar(&env.Verbose, "verbose", false, "verbose output")
	flag.Parse()

	if env.InRecipe == "" {
		log.Fatal("-recipe is required")
	}
	if env.OutNinjaFile == "" {
		log.Fatal("-outninja is required")
	}
	if env.OutDir == "" {
		log.Fatal("-outdir is required")
	}
	if env.ToolDir == "" {
		log.Fatal("-tooldir is required")
	}

	if err := run(env); err != nil {
		log.Fatal(err)
	}
}

// ConvertRecipe represents the TOML recipe for asset conversion.
type ConvertRecipe struct {
	DistrustedKeys        []*DistrustedKeysEntry        `toml:"distrusted_keys"`
	GameControllerDB      []*GameControllerDBEntry      `toml:"game_controller_db"`
	Mesh3DFromGLTF        []*Mesh3DFromGLTFEntry        `toml:"mesh3d_from_gltf"`
	Mesh3DFromVox         []*Mesh3DFromVoxEntry         `toml:"mesh3d_from_vox"`
	ParticlesClip         []*ParticlesClipEntry         `toml:"particles_clip_from_json"`
	PEM                   []*PEMEntry                   `toml:"pem"`
	Skeletal2DFromSpine   []*Skeletal2DFromSpineEntry   `toml:"skeletal2d_from_spine_json"`
	Skeletal2DAnimGraph   []*Skeletal2DAnimGraphEntry   `toml:"skeletal2d_animgraph_from_json"`
	SoundClip             []*SoundClipEntry             `toml:"sound_clip"`
	TilesetFromPNGs       []*TilesetFromPNGsEntry       `toml:"tileset_from_pngs"`
	TilesetFromPiskel     []*TilesetFromPiskelEntry     `toml:"tileset_from_piskel"`
	TilesetFromSpineAtlas []*TilesetFromSpineAtlasEntry `toml:"tileset_from_spine_atlas"`
	VoxelModelFromVox     []*VoxelModelFromVoxEntry     `toml:"voxelmodel_from_vox"`
}

// GameControllerDBEntry defines a conversion from SDL_GameControllerDB to .gcdb binary.
type GameControllerDBEntry struct {
	In              string `toml:"in"`
	Out             string `toml:"out"`
	UseAbsPathInDir bool   `toml:"use_abs_path_indir"`
}

// DistrustedKeysEntry defines a conversion of a PEM file of distrusted
// certificates to a .dkeys binary of their key hashes.
type DistrustedKeysEntry struct {
	In  string `toml:"in"`
	Out string `toml:"out"`
}

// PEMEntry defines a conversion of a PEM file of certificates to a compact
// one, without comments and blank lines.
type PEMEntry struct {
	In  string `toml:"in"`
	Out string `toml:"out"`
}

// ParticlesClipEntry defines a conversion from a particle clip JSON to the
// FlatBuffers `.particlesclip` binary.
type ParticlesClipEntry struct {
	In  string `toml:"in"`
	Out string `toml:"out"`
}

// Skeletal2DFromSpineEntry defines a conversion from a Spine JSON skeleton to
// the FlatBuffers `.skeleton_skel2d`, `.skin_skel2d`, and `.clip_skel2d`
// binaries of a 2D skeletal animation model.
type Skeletal2DFromSpineEntry struct {
	In          string `toml:"in"`
	OutSkeleton string `toml:"out_skeleton"`
	OutSkin     string `toml:"out_skin"`
	OutClip     string `toml:"out_clip"`
}

// Skeletal2DAnimGraphEntry defines a conversion from an animation graph JSON
// to the FlatBuffers `.animgraph_skel2d` binary of a 2D skeletal animation model.
type Skeletal2DAnimGraphEntry struct {
	In  string `toml:"in"`
	Out string `toml:"out"`
}

// Mesh3DFromGLTFEntry defines a conversion from a glTF binary file to the
// FlatBuffers `.mesh3d` binary of a 3D model and the raw `.mesh3dvertices`
// and `.mesh3dindices` files that its GPU buffers are loaded from.
type Mesh3DFromGLTFEntry struct {
	In          string `toml:"in"`
	OutMesh     string `toml:"out_mesh"`
	OutVertices string `toml:"out_vertices"`
	OutIndices  string `toml:"out_indices"`
}

// Mesh3DFromVoxEntry defines a conversion from a MagicaVoxel file to the
// FlatBuffers `.mesh3d` binary of a 3D model and its raw `.mesh3dvertices`
// and `.mesh3dindices` files. Mesher names the faces that become polygons
// (see voxelmodel-to-mesh3d-source); it is optional and defaults to the
// default of the tool.
type Mesh3DFromVoxEntry struct {
	In          string `toml:"in"`
	OutMesh     string `toml:"out_mesh"`
	OutVertices string `toml:"out_vertices"`
	OutIndices  string `toml:"out_indices"`
	Mesher      string `toml:"mesher"`
}

// VoxelModelFromVoxEntry defines a conversion from a MagicaVoxel file to the
// FlatBuffers `.voxelmodel` binary that `voxelmodel::loadVoxelModel()` reads.
type VoxelModelFromVoxEntry struct {
	In  string `toml:"in"`
	Out string `toml:"out"`
}

// SoundClipEntry defines a single sound clip conversion.
type SoundClipEntry struct {
	InFile        string         `toml:"in_file"`
	OutFile       string         `toml:"out_file"`
	InFormat      string         `toml:"in_format"`
	OutFormat     string         `toml:"out_format"`
	WAVOptions    *WAVOptions    `toml:"wav_options"`
	VorbisOptions *VorbisOptions `toml:"vorbis_options"`
	Loop          *SoundClipLoop `toml:"loop"`
}

// WAVOptions specifies output options for WAV format.
type WAVOptions struct {
	Channels       int  `toml:"channels"`
	SampleRate     int  `toml:"sample_rate"`
	BitDepth       int  `toml:"bit_depth"`
	InvalidBitRate *int `toml:"bit_rate"` // NOTE: Invalid option to detect misconfiguration
}

// VorbisOptions specifies output options for Ogg Vorbis format.
type VorbisOptions struct {
	Channels        int  `toml:"channels"`
	SampleRate      int  `toml:"sample_rate"`
	BitRate         int  `toml:"bit_rate"`
	InvalidBitDepth *int `toml:"bit_depth"` // NOTE: Invalid option to detect misconfiguration
}

// SoundClipLoop defines crossfade loop parameters for a sound clip.
type SoundClipLoop struct {
	TrimStart    bool    `toml:"trim_start"`
	Seconds      float64 `toml:"seconds"`
	AlignSamples int     `toml:"align_samples"`
	FadeMethod   string  `toml:"fade_method"`
}

// TilesetFromPNGsEntry defines a conversion from a png2atlas recipe TOML to
// a PNG texture atlas + FlatBuffers `.tileset` binary.
type TilesetFromPNGsEntry struct {
	Recipe     string `toml:"recipe"`
	OutTexture string `toml:"out_texture"`
	OutTileset string `toml:"out_tileset"`
}

// TilesetFromPiskelEntry defines a conversion from a piskel2atlas recipe TOML
// to a PNG texture atlas + FlatBuffers `.tileset` binary.
type TilesetFromPiskelEntry struct {
	InRecipe   []string `toml:"in_recipe"`
	OutTexture string   `toml:"out_texture"`
	OutTileset string   `toml:"out_tileset"`
}

// TilesetFromSpineAtlasEntry defines a conversion from a Spine/libGDX
// atlas + PNG pair to a repacked PNG + FlatBuffers `.tileset` binary.
type TilesetFromSpineAtlasEntry struct {
	InAtlas            string `toml:"in_atlas"`
	InImage            string `toml:"in_image"`
	OutTileset         string `toml:"out_tileset"`
	OutTexture         string `toml:"out_texture"`
	Width              *int   `toml:"width"`               // optional output texture width
	Height             *int   `toml:"height"`              // optional output texture height
	PerTileSpacing     *int   `toml:"per_tile_spacing"`    // optional gap between sprites
	TightPacking       *bool  `toml:"tight_packing"`       // optional; crop transparent margins
	PremultipliedAlpha *bool  `toml:"premultiplied_alpha"` // optional; premultiply alpha
}

func run(env *Env) error {
	if abs, err := filepath.Abs(env.OutDir); err == nil {
		env.OutDir = filepath.Clean(abs)
	}
	if abs, err := filepath.Abs(env.OutNinjaFile); err == nil {
		env.OutNinjaFile = filepath.Clean(abs)
	}
	if abs, err := filepath.Abs(env.ToolDir); err == nil {
		env.ToolDir = filepath.Clean(abs)
	}
	if env.InDir != "" {
		if abs, err := filepath.Abs(env.InDir); err == nil {
			env.InDir = filepath.Clean(abs)
		}
	}
	if env.IntDir != "" {
		if abs, err := filepath.Abs(env.IntDir); err == nil {
			env.IntDir = filepath.Clean(abs)
		}
	}
	if env.ThirdPartyDir != "" {
		if abs, err := filepath.Abs(env.ThirdPartyDir); err == nil {
			env.ThirdPartyDir = filepath.Clean(abs)
		}
	}

	recipe, err := readRecipe(env.InRecipe, env.ThirdPartyDir)
	if err != nil {
		return fmt.Errorf("readRecipe() failed: %w", err)
	}

	gen := &ninja.Generator{}

	// NOTE: Game controller DB conversion rule
	gcdbExe := filepath.Join(env.ToolDir, "generate-game-controller-db")
	if runtime.GOOS == "windows" {
		gcdbExe += ".exe"
	}
	gen.AddVariable(ninja.NewVariableAsPath("generate_game_controller_db_exe", gcdbExe))
	gen.AddRule(&ninja.Rule{
		Name:    "generate_game_controller_db",
		Command: "$generate_game_controller_db_exe -i $in -o $out",
	})

	// NOTE: Track output directories to create
	outDirs := map[string]struct{}{}

	for _, entry := range recipe.GameControllerDB {
		inPath := entry.In
		if entry.UseAbsPathInDir {
			// NOTE: Input path is already absolute (e.g. from thirdparty dir)
		} else if env.InDir != "" {
			inPath = filepath.Join(env.InDir, entry.In)
		}

		outPath := filepath.Join(env.OutDir, entry.Out)
		outDirs[filepath.Dir(outPath)] = struct{}{}

		gen.AddBuild(&ninja.Build{
			Rule:    "generate_game_controller_db",
			OutFile: outPath,
			InFiles: []string{inPath},
		})
	}

	// NOTE: PEM compaction rule
	pemCompactExe := filepath.Join(env.ToolDir, "pem-compact")
	if runtime.GOOS == "windows" {
		pemCompactExe += ".exe"
	}
	gen.AddVariable(ninja.NewVariableAsPath("pem_compact_exe", pemCompactExe))
	gen.AddRule(&ninja.Rule{
		Name:    "pem_compact",
		Command: "$pem_compact_exe -i $in -o $out",
	})

	for _, entry := range recipe.PEM {
		inPath := entry.In
		if env.InDir != "" {
			inPath = filepath.Join(env.InDir, entry.In)
		}

		outPath := filepath.Join(env.OutDir, entry.Out)
		outDirs[filepath.Dir(outPath)] = struct{}{}

		gen.AddBuild(&ninja.Build{
			Rule:    "pem_compact",
			OutFile: outPath,
			InFiles: []string{inPath},
		})
	}

	// NOTE: Distrusted keys conversion rule
	distrustedKeysExe := filepath.Join(env.ToolDir, "distrusted-keys-gen")
	if runtime.GOOS == "windows" {
		distrustedKeysExe += ".exe"
	}
	gen.AddVariable(ninja.NewVariableAsPath("distrusted_keys_gen_exe", distrustedKeysExe))
	gen.AddRule(&ninja.Rule{
		Name:    "distrusted_keys_gen",
		Command: "$distrusted_keys_gen_exe -i $in -o $out",
	})

	for _, entry := range recipe.DistrustedKeys {
		inPath := entry.In
		if env.InDir != "" {
			inPath = filepath.Join(env.InDir, entry.In)
		}

		outPath := filepath.Join(env.OutDir, entry.Out)
		outDirs[filepath.Dir(outPath)] = struct{}{}

		gen.AddBuild(&ninja.Build{
			Rule:    "distrusted_keys_gen",
			OutFile: outPath,
			InFiles: []string{inPath},
		})
	}

	// NOTE: Create output directories
	dirs := make([]string, 0, len(outDirs))
	for d := range outDirs {
		dirs = append(dirs, d)
	}
	sort.Strings(dirs)

	for _, d := range dirs {
		if err := os.MkdirAll(d, os.ModePerm); err != nil {
			return fmt.Errorf("os.MkdirAll() failed: %w", err)
		}
	}

	// NOTE: Sound clip conversion
	if err := buildSoundClips(env, recipe, gen); err != nil {
		return fmt.Errorf("buildSoundClips() failed: %w", err)
	}

	// NOTE: PNG atlas conversion
	if err := buildTilesetFromPNGs(env, recipe, gen); err != nil {
		return fmt.Errorf("buildTilesetFromPNGs() failed: %w", err)
	}

	// NOTE: Piskel atlas conversion
	if err := buildTilesetFromPiskel(env, recipe, gen); err != nil {
		return fmt.Errorf("buildTilesetFromPiskel() failed: %w", err)
	}

	// NOTE: Spine atlas conversion
	if err := buildTilesetFromSpineAtlas(env, recipe, gen); err != nil {
		return fmt.Errorf("buildTilesetFromSpineAtlas() failed: %w", err)
	}

	// NOTE: Particle clip conversion
	if err := buildParticlesClip(env, recipe, gen); err != nil {
		return fmt.Errorf("buildParticlesClip() failed: %w", err)
	}

	// NOTE: Spine skeleton conversion
	if err := buildSkeletal2DFromSpine(env, recipe, gen); err != nil {
		return fmt.Errorf("buildSkeletal2DFromSpine() failed: %w", err)
	}

	// NOTE: Animation graph conversion
	if err := buildSkeletal2DAnimGraph(env, recipe, gen); err != nil {
		return fmt.Errorf("buildSkeletal2DAnimGraph() failed: %w", err)
	}

	// NOTE: 3D mesh and voxel model conversion, whose entries share tools
	rules := &toolRules{env: env, gen: gen, added: map[string]bool{}}
	if err := buildMesh3DFromGLTF(env, recipe, rules); err != nil {
		return fmt.Errorf("buildMesh3DFromGLTF() failed: %w", err)
	}
	if err := buildVoxelModelFromVox(env, recipe, rules); err != nil {
		return fmt.Errorf("buildVoxelModelFromVox() failed: %w", err)
	}
	if err := buildMesh3DFromVox(env, recipe, rules); err != nil {
		return fmt.Errorf("buildMesh3DFromVox() failed: %w", err)
	}

	// NOTE: Create directory for ninja file
	if dir := filepath.Dir(env.OutNinjaFile); dir != "" {
		if err := os.MkdirAll(dir, os.ModePerm); err != nil {
			return fmt.Errorf("os.MkdirAll() failed: %w", err)
		}
	}

	content := gen.Generate()

	f, err := os.Create(env.OutNinjaFile)
	if err != nil {
		return fmt.Errorf("os.Create() failed: %w", err)
	}
	defer f.Close()

	if _, err := f.WriteString(content); err != nil {
		return fmt.Errorf("f.WriteString() failed: %w", err)
	}

	return nil
}

func readRecipe(inFile string, thirdPartyDir string) (*ConvertRecipe, error) {
	buf, err := os.ReadFile(inFile)
	if err != nil {
		return nil, fmt.Errorf("os.ReadFile() failed: %w", err)
	}

	// NOTE: Substitute environment variables
	content := string(buf)
	if thirdPartyDir != "" {
		thirdPartyDir = filepath.ToSlash(filepath.Clean(thirdPartyDir))
		content = strings.ReplaceAll(content, "${POMDOG_THIRDPARTY_DIR}", thirdPartyDir)
	}

	recipe := &ConvertRecipe{}
	if err := toml.Unmarshal([]byte(content), recipe); err != nil {
		return nil, fmt.Errorf("toml.Unmarshal() failed: %w", err)
	}

	if err := validateRecipe(recipe); err != nil {
		return nil, fmt.Errorf("validateRecipe() failed: %w", err)
	}

	return recipe, nil
}

func validateRecipe(recipe *ConvertRecipe) error {
	for i, entry := range recipe.GameControllerDB {
		if entry.In == "" {
			return fmt.Errorf("[[game_controller_db]] #%d: in must not be empty", i)
		}
		if entry.Out == "" {
			return fmt.Errorf("[[game_controller_db]] #%d: out must not be empty", i)
		}
	}
	for i, entry := range recipe.DistrustedKeys {
		if entry.In == "" {
			return fmt.Errorf("[[distrusted_keys]] #%d: in must not be empty", i)
		}
		if entry.Out == "" {
			return fmt.Errorf("[[distrusted_keys]] #%d: out must not be empty", i)
		}
	}
	for i, entry := range recipe.PEM {
		if entry.In == "" {
			return fmt.Errorf("[[pem]] #%d: in must not be empty", i)
		}
		if entry.Out == "" {
			return fmt.Errorf("[[pem]] #%d: out must not be empty", i)
		}
	}
	for i, entry := range recipe.ParticlesClip {
		if entry.In == "" {
			return fmt.Errorf("[[particles_clip_from_json]] #%d: in must not be empty", i)
		}
		if entry.Out == "" {
			return fmt.Errorf("[[particles_clip_from_json]] #%d: out must not be empty", i)
		}
	}
	for i, entry := range recipe.Skeletal2DFromSpine {
		if entry.In == "" {
			return fmt.Errorf("[[skeletal2d_from_spine_json]] #%d: in must not be empty", i)
		}
		if entry.OutSkeleton == "" {
			return fmt.Errorf("[[skeletal2d_from_spine_json]] #%d: out_skeleton must not be empty", i)
		}
		if entry.OutSkin == "" {
			return fmt.Errorf("[[skeletal2d_from_spine_json]] #%d: out_skin must not be empty", i)
		}
		if entry.OutClip == "" {
			return fmt.Errorf("[[skeletal2d_from_spine_json]] #%d: out_clip must not be empty", i)
		}
	}
	for i, entry := range recipe.Skeletal2DAnimGraph {
		if entry.In == "" {
			return fmt.Errorf("[[skeletal2d_animgraph_from_json]] #%d: in must not be empty", i)
		}
		if entry.Out == "" {
			return fmt.Errorf("[[skeletal2d_animgraph_from_json]] #%d: out must not be empty", i)
		}
	}
	for i, entry := range recipe.Mesh3DFromGLTF {
		if entry.In == "" {
			return fmt.Errorf("[[mesh3d_from_gltf]] #%d: in must not be empty", i)
		}
		if entry.OutMesh == "" {
			return fmt.Errorf("[[mesh3d_from_gltf]] #%d: out_mesh must not be empty", i)
		}
		if entry.OutVertices == "" {
			return fmt.Errorf("[[mesh3d_from_gltf]] #%d: out_vertices must not be empty", i)
		}
		if entry.OutIndices == "" {
			return fmt.Errorf("[[mesh3d_from_gltf]] #%d: out_indices must not be empty", i)
		}
	}
	for i, entry := range recipe.Mesh3DFromVox {
		if entry.In == "" {
			return fmt.Errorf("[[mesh3d_from_vox]] #%d: in must not be empty", i)
		}
		if entry.OutMesh == "" {
			return fmt.Errorf("[[mesh3d_from_vox]] #%d: out_mesh must not be empty", i)
		}
		if entry.OutVertices == "" {
			return fmt.Errorf("[[mesh3d_from_vox]] #%d: out_vertices must not be empty", i)
		}
		if entry.OutIndices == "" {
			return fmt.Errorf("[[mesh3d_from_vox]] #%d: out_indices must not be empty", i)
		}
		switch entry.Mesher {
		case "", "naive", "culled", "greedy":
		default:
			return fmt.Errorf("[[mesh3d_from_vox]] #%d: mesher %q must be naive, culled, or greedy", i, entry.Mesher)
		}
	}
	for i, entry := range recipe.VoxelModelFromVox {
		if entry.In == "" {
			return fmt.Errorf("[[voxelmodel_from_vox]] #%d: in must not be empty", i)
		}
		if entry.Out == "" {
			return fmt.Errorf("[[voxelmodel_from_vox]] #%d: out must not be empty", i)
		}
	}
	for i, clip := range recipe.SoundClip {
		if clip.InFile == "" {
			return fmt.Errorf("[[sound_clip]] #%d: in_file must not be empty", i)
		}
		if clip.OutFile == "" {
			return fmt.Errorf("[[sound_clip]] #%d: out_file must not be empty", i)
		}
		if clip.InFormat == "" {
			return fmt.Errorf("[[sound_clip]] #%d: in_format must not be empty", i)
		}
		if clip.OutFormat == "" {
			return fmt.Errorf("[[sound_clip]] #%d: out_format must not be empty", i)
		}
		if clip.OutFormat != "wav" && clip.OutFormat != "vorbis" {
			return fmt.Errorf("[[sound_clip]] #%d: out_format must be 'wav' or 'vorbis'", i)
		}
		if clip.Loop != nil {
			if clip.Loop.FadeMethod != "" {
				switch clip.Loop.FadeMethod {
				case "linear", "equal_power", "logarithmic":
				default:
					return fmt.Errorf("[[sound_clip]] #%d: unsupported fade_method '%s'", i, clip.Loop.FadeMethod)
				}
			}
		}
	}
	for i, entry := range recipe.TilesetFromPNGs {
		if entry.Recipe == "" {
			return fmt.Errorf("[[tileset_from_pngs]] #%d: recipe must not be empty", i)
		}
		if entry.OutTexture == "" {
			return fmt.Errorf("[[tileset_from_pngs]] #%d: out_texture must not be empty", i)
		}
		if entry.OutTileset == "" {
			return fmt.Errorf("[[tileset_from_pngs]] #%d: out_tileset must not be empty", i)
		}
	}
	for i, entry := range recipe.TilesetFromPiskel {
		if len(entry.InRecipe) == 0 {
			return fmt.Errorf("[[tileset_from_piskel]] #%d: in_recipe must not be empty", i)
		}
		if entry.OutTexture == "" {
			return fmt.Errorf("[[tileset_from_piskel]] #%d: out_texture must not be empty", i)
		}
		if entry.OutTileset == "" {
			return fmt.Errorf("[[tileset_from_piskel]] #%d: out_tileset must not be empty", i)
		}
	}
	for i, entry := range recipe.TilesetFromSpineAtlas {
		if entry.InAtlas == "" {
			return fmt.Errorf("[[tileset_from_spine_atlas]] #%d: in_atlas must not be empty", i)
		}
		if entry.InImage == "" {
			return fmt.Errorf("[[tileset_from_spine_atlas]] #%d: in_image must not be empty", i)
		}
		if entry.OutTileset == "" {
			return fmt.Errorf("[[tileset_from_spine_atlas]] #%d: out_tileset must not be empty", i)
		}
		if entry.OutTexture == "" {
			return fmt.Errorf("[[tileset_from_spine_atlas]] #%d: out_texture must not be empty", i)
		}
	}
	return nil
}

func buildSoundClips(env *Env, recipe *ConvertRecipe, gen *ninja.Generator) error {
	if len(recipe.SoundClip) == 0 {
		return nil
	}

	// NOTE: wav-crossfade-conv tool
	wavCrossfadeExe := filepath.Join(env.ToolDir, "wav-crossfade-conv")
	if runtime.GOOS == "windows" {
		wavCrossfadeExe += ".exe"
	}
	gen.AddVariable(ninja.NewVariableAsPath("wav_crossfade_conv_exe", wavCrossfadeExe))
	gen.AddRule(&ninja.Rule{
		Name:    "wav_crossfade_conv",
		Command: "$wav_crossfade_conv_exe -i $in -o $out --trimstart=$trimstart --seconds=$seconds --alignsamples=$alignsamples --fade=$fade",
	})

	// NOTE: ffmpeg tool
	ffmpegExe := filepath.Join(env.ToolDir, "ffmpeg")
	if runtime.GOOS == "windows" {
		ffmpegExe += ".exe"
	}
	gen.AddVariable(ninja.NewVariableAsPath("ffmpeg_exe", ffmpegExe))

	ffmpegGlobalArgs := ""
	if !env.Verbose {
		ffmpegGlobalArgs = "-hide_banner -loglevel error"
	}
	gen.AddRule(&ninja.Rule{
		Name:    "ffmpeg_wav_to_wav",
		Command: fmt.Sprintf("$ffmpeg_exe %s -i $in -y -map_metadata -1 -f wav -bitexact $ffmpeg_args $out", ffmpegGlobalArgs),
	})
	gen.AddRule(&ninja.Rule{
		Name:    "ffmpeg_wav_to_vorbis",
		Command: fmt.Sprintf("$ffmpeg_exe %s -i $in -y -map_metadata -1 -f ogg -c:a libvorbis $ffmpeg_args $out", ffmpegGlobalArgs),
	})

	for _, clip := range recipe.SoundClip {
		inFile := clip.InFile
		if env.InDir != "" {
			inFile = filepath.Join(env.InDir, inFile)
		}

		outFile := filepath.Join(env.OutDir, clip.OutFile)

		if err := os.MkdirAll(filepath.Dir(outFile), os.ModePerm); err != nil {
			return fmt.Errorf("os.MkdirAll() failed: %w", err)
		}

		currentIn := inFile

		// Step 1: Crossfade loop (if loop config is specified)
		if clip.Loop != nil {
			if clip.InFormat != "wav" {
				return fmt.Errorf("crossfade loop is only supported for wav format: %s", clip.InFormat)
			}

			// NOTE: Use intermediate directory for loop output
			base := strings.TrimSuffix(filepath.Base(outFile), filepath.Ext(outFile))
			loopOutFile := filepath.Join(env.IntDir, base+"-loop.wav")

			if err := os.MkdirAll(filepath.Dir(loopOutFile), os.ModePerm); err != nil {
				return fmt.Errorf("os.MkdirAll() failed: %w", err)
			}

			fadeMethod := clip.Loop.FadeMethod
			if fadeMethod == "" {
				fadeMethod = "equal_power"
			}

			gen.AddBuild(&ninja.Build{
				Rule:    "wav_crossfade_conv",
				OutFile: loopOutFile,
				InFiles: []string{inFile},
				Variables: []*ninja.Variable{
					ninja.NewVariableAsString("trimstart", strconv.FormatBool(clip.Loop.TrimStart)),
					ninja.NewVariableAsString("seconds", strconv.FormatFloat(clip.Loop.Seconds, 'f', -1, 64)),
					ninja.NewVariableAsString("alignsamples", strconv.Itoa(clip.Loop.AlignSamples)),
					ninja.NewVariableAsString("fade", fadeMethod),
				},
			})

			currentIn = loopOutFile
		}

		// Step 2: Format conversion with ffmpeg
		if clip.InFormat == "wav" && clip.OutFormat == "vorbis" {
			if clip.WAVOptions != nil {
				return fmt.Errorf("wav_options is not supported when output format is vorbis")
			}

			ffmpegArgs := ""
			if o := clip.VorbisOptions; o != nil {
				if o.InvalidBitDepth != nil {
					return fmt.Errorf("bit_depth option is not supported for vorbis output")
				}
				if o.SampleRate == 22050 && o.BitRate >= 128 {
					return fmt.Errorf("bit_rate must be less than 128 kbps when sample_rate is 22050 Hz for vorbis output")
				}

				switch o.Channels {
				case 1, 2:
					ffmpegArgs += " -ac " + strconv.Itoa(o.Channels)
				default:
					return fmt.Errorf("not supported channels: %d", o.Channels)
				}

				switch o.SampleRate {
				case 16000, 22050, 32000, 44100, 48000:
					ffmpegArgs += " -ar " + strconv.Itoa(o.SampleRate)
				default:
					return fmt.Errorf("not supported sample-rate: %d", o.SampleRate)
				}

				switch o.BitRate {
				case 64, 96, 128, 192, 256, 320:
					ffmpegArgs += " -b:a " + strconv.Itoa(o.BitRate) + "k"
				default:
					return fmt.Errorf("not supported bit-rate: %d kbps", o.BitRate)
				}
			}
			ffmpegArgs = strings.TrimSpace(ffmpegArgs)

			gen.AddBuild(&ninja.Build{
				Rule:    "ffmpeg_wav_to_vorbis",
				OutFile: outFile,
				InFiles: []string{currentIn},
				Variables: []*ninja.Variable{
					ninja.NewVariableAsString("ffmpeg_args", ffmpegArgs),
				},
			})
		} else if clip.InFormat == "wav" && clip.OutFormat == "wav" {
			if clip.VorbisOptions != nil {
				return fmt.Errorf("vorbis_options is not supported when output format is wav")
			}

			ffmpegArgs := ""
			if o := clip.WAVOptions; o != nil {
				if o.InvalidBitRate != nil {
					return fmt.Errorf("bit_rate option is not supported for wav output")
				}

				switch o.Channels {
				case 1, 2:
					ffmpegArgs += " -ac " + strconv.Itoa(o.Channels)
				default:
					return fmt.Errorf("not supported channels: %d", o.Channels)
				}

				switch o.SampleRate {
				case 16000, 22050, 32000, 44100, 48000:
					ffmpegArgs += " -ar " + strconv.Itoa(o.SampleRate)
				default:
					return fmt.Errorf("not supported sample-rate: %d", o.SampleRate)
				}

				switch o.BitDepth {
				case 16:
					ffmpegArgs += " -c:a pcm_s16le"
				case 24:
					ffmpegArgs += " -c:a pcm_s24le"
				case 32:
					ffmpegArgs += " -c:a pcm_f32le"
				default:
					return fmt.Errorf("not supported bit-depth: %d", o.BitDepth)
				}
			}
			ffmpegArgs = strings.TrimSpace(ffmpegArgs)

			gen.AddBuild(&ninja.Build{
				Rule:    "ffmpeg_wav_to_wav",
				OutFile: outFile,
				InFiles: []string{currentIn},
				Variables: []*ninja.Variable{
					ninja.NewVariableAsString("ffmpeg_args", ffmpegArgs),
				},
			})
		} else {
			return fmt.Errorf("unsupported output format: %s to %s", clip.InFormat, clip.OutFormat)
		}
	}

	return nil
}

func buildTilesetFromPNGs(env *Env, recipe *ConvertRecipe, gen *ninja.Generator) error {
	if len(recipe.TilesetFromPNGs) == 0 {
		return nil
	}

	png2atlasExe := filepath.Join(env.ToolDir, "png2atlas")
	if runtime.GOOS == "windows" {
		png2atlasExe += ".exe"
	}
	gen.AddVariable(ninja.NewVariableAsPath("png2atlas_exe", png2atlasExe))
	gen.AddRule(&ninja.Rule{
		Name:    "png2atlas",
		Command: "$png2atlas_exe -o $out --out-tex $out_tex -d $depfile $in",
		DepFile: "$depfile",
		Deps:    "gcc",
	})

	for _, entry := range recipe.TilesetFromPNGs {
		recipePath := entry.Recipe
		if env.InDir != "" {
			recipePath = filepath.Join(env.InDir, entry.Recipe)
		}

		outTilesetPath := filepath.Join(env.OutDir, entry.OutTileset)
		outTexturePath := filepath.Join(env.OutDir, entry.OutTexture)

		depFilePath := outTilesetPath + ".d"
		if env.IntDir != "" {
			depFilePath = filepath.Join(env.IntDir, entry.OutTileset+".d")
		}

		gen.AddBuild(&ninja.Build{
			Rule:             "png2atlas",
			OutFile:          outTilesetPath,
			ImplicitOutFiles: []string{outTexturePath},
			InFiles:          []string{recipePath},
			Variables: []*ninja.Variable{
				ninja.NewVariableAsPath("out_tex", outTexturePath),
				ninja.NewVariableAsPath("depfile", depFilePath),
			},
		})
	}

	return nil
}

func buildTilesetFromPiskel(env *Env, recipe *ConvertRecipe, gen *ninja.Generator) error {
	if len(recipe.TilesetFromPiskel) == 0 {
		return nil
	}

	piskel2atlasExe := filepath.Join(env.ToolDir, "piskel2atlas")
	if runtime.GOOS == "windows" {
		piskel2atlasExe += ".exe"
	}
	gen.AddVariable(ninja.NewVariableAsPath("piskel2atlas_exe", piskel2atlasExe))
	gen.AddRule(&ninja.Rule{
		Name:    "piskel2atlas",
		Command: "$piskel2atlas_exe -o $out --out-tex $out_tex -d $depfile $in",
		DepFile: "$depfile",
		Deps:    "gcc",
	})

	for _, entry := range recipe.TilesetFromPiskel {
		inRecipePaths := make([]string, 0, len(entry.InRecipe))
		for _, r := range entry.InRecipe {
			if env.InDir != "" {
				inRecipePaths = append(inRecipePaths, filepath.Join(env.InDir, r))
			} else {
				inRecipePaths = append(inRecipePaths, r)
			}
		}

		outTilesetPath := filepath.Join(env.OutDir, entry.OutTileset)
		outTexturePath := filepath.Join(env.OutDir, entry.OutTexture)

		depFilePath := outTilesetPath + ".d"
		if env.IntDir != "" {
			depFilePath = filepath.Join(env.IntDir, entry.OutTileset+".d")
		}

		gen.AddBuild(&ninja.Build{
			Rule:             "piskel2atlas",
			OutFile:          outTilesetPath,
			ImplicitOutFiles: []string{outTexturePath},
			InFiles:          inRecipePaths,
			Variables: []*ninja.Variable{
				ninja.NewVariableAsPath("out_tex", outTexturePath),
				ninja.NewVariableAsPath("depfile", depFilePath),
			},
		})
	}

	return nil
}

func buildTilesetFromSpineAtlas(env *Env, recipe *ConvertRecipe, gen *ninja.Generator) error {
	if len(recipe.TilesetFromSpineAtlas) == 0 {
		return nil
	}

	libgdxExe := filepath.Join(env.ToolDir, "libgdx-atlas-to-tileset")
	if runtime.GOOS == "windows" {
		libgdxExe += ".exe"
	}
	gen.AddVariable(ninja.NewVariableAsPath("libgdx_atlas_to_tileset_exe", libgdxExe))
	gen.AddRule(&ninja.Rule{
		Name:    "libgdx_atlas_to_tileset",
		Command: "$libgdx_atlas_to_tileset_exe --in-atlas $in --in-tex $in_tex -o $out --out-tex $out_tex $extra_args",
	})

	for _, entry := range recipe.TilesetFromSpineAtlas {
		inAtlasPath := entry.InAtlas
		if env.InDir != "" {
			inAtlasPath = filepath.Join(env.InDir, entry.InAtlas)
		}
		inImagePath := entry.InImage
		if env.InDir != "" {
			inImagePath = filepath.Join(env.InDir, entry.InImage)
		}

		outTilesetPath := filepath.Join(env.OutDir, entry.OutTileset)
		outTexturePath := filepath.Join(env.OutDir, entry.OutTexture)

		// NOTE: Build optional extra arguments for libgdx-atlas-to-tileset.
		var extraArgs []string
		if entry.Width != nil {
			extraArgs = append(extraArgs, fmt.Sprintf("--out-width %d", *entry.Width))
		}
		if entry.Height != nil {
			extraArgs = append(extraArgs, fmt.Sprintf("--out-height %d", *entry.Height))
		}
		if entry.PerTileSpacing != nil {
			extraArgs = append(extraArgs, fmt.Sprintf("--out-per-tile-spacing %d", *entry.PerTileSpacing))
		}
		if entry.TightPacking != nil {
			if *entry.TightPacking {
				extraArgs = append(extraArgs, "--out-tight-packing=true")
			} else {
				extraArgs = append(extraArgs, "--out-tight-packing=false")
			}
		}
		if entry.PremultipliedAlpha != nil {
			if *entry.PremultipliedAlpha {
				extraArgs = append(extraArgs, "--out-premultiplied-alpha=true")
			} else {
				extraArgs = append(extraArgs, "--out-premultiplied-alpha=false")
			}
		}

		gen.AddBuild(&ninja.Build{
			Rule:             "libgdx_atlas_to_tileset",
			OutFile:          outTilesetPath,
			ImplicitOutFiles: []string{outTexturePath},
			InFiles:          []string{inAtlasPath},
			ImplicitInFiles:  []string{inImagePath},
			Variables: []*ninja.Variable{
				ninja.NewVariableAsPath("in_tex", inImagePath),
				ninja.NewVariableAsPath("out_tex", outTexturePath),
				ninja.NewVariableAsString("extra_args", strings.Join(extraArgs, " ")),
			},
		})
	}

	return nil
}

func buildSkeletal2DFromSpine(env *Env, recipe *ConvertRecipe, gen *ninja.Generator) error {
	if len(recipe.Skeletal2DFromSpine) == 0 {
		return nil
	}

	// NOTE: The debug files are not runtime assets, so they must stay out of
	// the content directory.
	if env.IntDir == "" {
		return fmt.Errorf("[[skeletal2d_from_spine_json]] requires -intdir, where the debug files are written")
	}

	exe := filepath.Join(env.ToolDir, "spine-json-to-skeletal2d")
	if runtime.GOOS == "windows" {
		exe += ".exe"
	}
	gen.AddVariable(ninja.NewVariableAsPath("spine_json_to_skeletal2d_exe", exe))
	gen.AddRule(&ninja.Rule{
		Name:    "spine_json_to_skeletal2d",
		Command: "$spine_json_to_skeletal2d_exe -i $in --out-skeleton $out --out-skin $out_skin --out-clip $out_clip --out-debug $out_debug",
	})

	for _, entry := range recipe.Skeletal2DFromSpine {
		inPath := entry.In
		if env.InDir != "" {
			inPath = filepath.Join(env.InDir, entry.In)
		}

		outSkeletonPath := filepath.Join(env.OutDir, entry.OutSkeleton)
		outSkinPath := filepath.Join(env.OutDir, entry.OutSkin)
		outClipPath := filepath.Join(env.OutDir, entry.OutClip)

		// NOTE: One debug file covers the three outputs, so name it after
		// the skeleton file without its extension.
		debugName := strings.TrimSuffix(entry.OutSkeleton, filepath.Ext(entry.OutSkeleton)) + ".skel2d-debug"
		outDebugPath := filepath.Join(env.IntDir, debugName)

		// NOTE: The converter depends on the schemas of its outputs, so
		// convert again when it is rebuilt.
		gen.AddBuild(&ninja.Build{
			Rule:             "spine_json_to_skeletal2d",
			OutFile:          outSkeletonPath,
			ImplicitOutFiles: []string{outSkinPath, outClipPath, outDebugPath},
			InFiles:          []string{inPath},
			ImplicitInFiles:  []string{exe},
			Variables: []*ninja.Variable{
				ninja.NewVariableAsPath("out_skin", outSkinPath),
				ninja.NewVariableAsPath("out_clip", outClipPath),
				ninja.NewVariableAsPath("out_debug", outDebugPath),
			},
		})
	}

	return nil
}

func buildSkeletal2DAnimGraph(env *Env, recipe *ConvertRecipe, gen *ninja.Generator) error {
	if len(recipe.Skeletal2DAnimGraph) == 0 {
		return nil
	}

	// NOTE: The debug files are not runtime assets, so they must stay out of
	// the content directory.
	if env.IntDir == "" {
		return fmt.Errorf("[[skeletal2d_animgraph_from_json]] requires -intdir, where the debug files are written")
	}

	exe := filepath.Join(env.ToolDir, "animation-graph-json-to-skeletal2d")
	if runtime.GOOS == "windows" {
		exe += ".exe"
	}
	gen.AddVariable(ninja.NewVariableAsPath("animation_graph_json_to_skeletal2d_exe", exe))
	gen.AddRule(&ninja.Rule{
		Name:    "animation_graph_json_to_skeletal2d",
		Command: "$animation_graph_json_to_skeletal2d_exe -i $in -o $out --out-debug $out_debug",
	})

	for _, entry := range recipe.Skeletal2DAnimGraph {
		inPath := entry.In
		if env.InDir != "" {
			inPath = filepath.Join(env.InDir, entry.In)
		}

		outPath := filepath.Join(env.OutDir, entry.Out)
		outDebugPath := filepath.Join(env.IntDir, entry.Out+"-debug")

		// NOTE: The converter depends on the schema of its output, so
		// convert again when it is rebuilt.
		gen.AddBuild(&ninja.Build{
			Rule:             "animation_graph_json_to_skeletal2d",
			OutFile:          outPath,
			ImplicitOutFiles: []string{outDebugPath},
			InFiles:          []string{inPath},
			ImplicitInFiles:  []string{exe},
			Variables: []*ninja.Variable{
				ninja.NewVariableAsPath("out_debug", outDebugPath),
			},
		})
	}

	return nil
}

func buildParticlesClip(env *Env, recipe *ConvertRecipe, gen *ninja.Generator) error {
	if len(recipe.ParticlesClip) == 0 {
		return nil
	}

	exe := filepath.Join(env.ToolDir, "particle-clip-json-to-particles")
	if runtime.GOOS == "windows" {
		exe += ".exe"
	}
	gen.AddVariable(ninja.NewVariableAsPath("particle_clip_json_to_particles_exe", exe))
	gen.AddRule(&ninja.Rule{
		Name:    "particle_clip_json_to_particles",
		Command: "$particle_clip_json_to_particles_exe -i $in -o $out",
	})

	for _, entry := range recipe.ParticlesClip {
		inPath := entry.In
		if env.InDir != "" {
			inPath = filepath.Join(env.InDir, entry.In)
		}

		// NOTE: The converter depends on the schema of its output, so
		// convert again when it is rebuilt.
		gen.AddBuild(&ninja.Build{
			Rule:            "particle_clip_json_to_particles",
			OutFile:         filepath.Join(env.OutDir, entry.Out),
			InFiles:         []string{inPath},
			ImplicitInFiles: []string{exe},
		})
	}

	return nil
}

// toolRules adds the variable and the rule of a tool once, so that several
// kinds of entries can share the tool.
type toolRules struct {
	env   *Env
	gen   *ninja.Generator
	added map[string]bool
}

// add adds the rule `name`, whose command refers to the tool as
// `$<name>_exe`, and returns the path of the tool.
func (r *toolRules) add(tool, name, command string) string {
	exe := filepath.Join(r.env.ToolDir, tool)
	if runtime.GOOS == "windows" {
		exe += ".exe"
	}
	if !r.added[name] {
		r.added[name] = true
		r.gen.AddVariable(ninja.NewVariableAsPath(name+"_exe", exe))
		r.gen.AddRule(&ninja.Rule{Name: name, Command: command})
	}
	return exe
}

func inputPath(env *Env, path string) string {
	if env.InDir != "" {
		return filepath.Join(env.InDir, path)
	}
	return path
}

// addMesh3DBuild adds the build that optimizes the mesh source at sourcePath
// into the mesh file, the vertices file, and the indices file. The debug file
// goes under -intdir next to the mesh source.
func addMesh3DBuild(env *Env, rules *toolRules, sourcePath, outMesh, outVertices, outIndices string) {
	exe := rules.add("mesh3d-source-to-mesh3d", "mesh3d_source_to_mesh3d",
		"$mesh3d_source_to_mesh3d_exe -i $in --out-mesh $out --out-vertices $out_vertices --out-indices $out_indices --out-debug $out_debug")

	outMeshPath := filepath.Join(env.OutDir, outMesh)
	outVerticesPath := filepath.Join(env.OutDir, outVertices)
	outIndicesPath := filepath.Join(env.OutDir, outIndices)
	outDebugPath := filepath.Join(env.IntDir, outMesh+"-debug")

	rules.gen.AddBuild(&ninja.Build{
		Rule:             "mesh3d_source_to_mesh3d",
		OutFile:          outMeshPath,
		ImplicitOutFiles: []string{outVerticesPath, outIndicesPath, outDebugPath},
		InFiles:          []string{sourcePath},
		ImplicitInFiles:  []string{exe},
		Variables: []*ninja.Variable{
			ninja.NewVariableAsPath("out_vertices", outVerticesPath),
			ninja.NewVariableAsPath("out_indices", outIndicesPath),
			ninja.NewVariableAsPath("out_debug", outDebugPath),
		},
	})
}

// intermediatePath names an intermediate file after the mesh file, with the
// given extension, under -intdir.
func intermediatePath(env *Env, outMesh, ext string) string {
	return filepath.Join(env.IntDir, strings.TrimSuffix(outMesh, filepath.Ext(outMesh))+ext)
}

// NOTE: The converters depend on the schemas of their outputs, so every build
// below lists its tool as an implicit input and converts again when the tool
// is rebuilt.

func buildMesh3DFromGLTF(env *Env, recipe *ConvertRecipe, rules *toolRules) error {
	if len(recipe.Mesh3DFromGLTF) == 0 {
		return nil
	}

	// NOTE: The mesh source and the debug file are not runtime assets, so
	// they must stay out of the content directory.
	if env.IntDir == "" {
		return fmt.Errorf("[[mesh3d_from_gltf]] requires -intdir, where the mesh source and the debug file are written")
	}

	importExe := rules.add("gltf-to-mesh3d-source", "gltf_to_mesh3d_source", "$gltf_to_mesh3d_source_exe -i $in -o $out")

	for _, entry := range recipe.Mesh3DFromGLTF {
		sourcePath := intermediatePath(env, entry.OutMesh, ".mesh3dsource")
		rules.gen.AddBuild(&ninja.Build{
			Rule:            "gltf_to_mesh3d_source",
			OutFile:         sourcePath,
			InFiles:         []string{inputPath(env, entry.In)},
			ImplicitInFiles: []string{importExe},
		})
		addMesh3DBuild(env, rules, sourcePath, entry.OutMesh, entry.OutVertices, entry.OutIndices)
	}

	return nil
}

func addVoxToVoxelModelRule(rules *toolRules) string {
	return rules.add("vox-to-voxelmodel", "vox_to_voxelmodel", "$vox_to_voxelmodel_exe -i $in -o $out")
}

func buildVoxelModelFromVox(env *Env, recipe *ConvertRecipe, rules *toolRules) error {
	if len(recipe.VoxelModelFromVox) == 0 {
		return nil
	}

	exe := addVoxToVoxelModelRule(rules)
	for _, entry := range recipe.VoxelModelFromVox {
		rules.gen.AddBuild(&ninja.Build{
			Rule:            "vox_to_voxelmodel",
			OutFile:         filepath.Join(env.OutDir, entry.Out),
			InFiles:         []string{inputPath(env, entry.In)},
			ImplicitInFiles: []string{exe},
		})
	}

	return nil
}

func buildMesh3DFromVox(env *Env, recipe *ConvertRecipe, rules *toolRules) error {
	if len(recipe.Mesh3DFromVox) == 0 {
		return nil
	}

	// NOTE: The voxel model, the mesh source, and the debug file are not
	// runtime assets, so they must stay out of the content directory.
	if env.IntDir == "" {
		return fmt.Errorf("[[mesh3d_from_vox]] requires -intdir, where the voxel model, the mesh source, and the debug file are written")
	}

	importExe := addVoxToVoxelModelRule(rules)
	meshExe := rules.add("voxelmodel-to-mesh3d-source", "voxelmodel_to_mesh3d_source", "$voxelmodel_to_mesh3d_source_exe -i $in -o $out $mesher_flags")

	for _, entry := range recipe.Mesh3DFromVox {
		voxelModelPath := intermediatePath(env, entry.OutMesh, ".voxelmodel")
		sourcePath := intermediatePath(env, entry.OutMesh, ".mesh3dsource")

		rules.gen.AddBuild(&ninja.Build{
			Rule:            "vox_to_voxelmodel",
			OutFile:         voxelModelPath,
			InFiles:         []string{inputPath(env, entry.In)},
			ImplicitInFiles: []string{importExe},
		})

		var variables []*ninja.Variable
		if entry.Mesher != "" {
			variables = append(variables, ninja.NewVariableAsString("mesher_flags", "-mesher "+entry.Mesher))
		}
		rules.gen.AddBuild(&ninja.Build{
			Rule:            "voxelmodel_to_mesh3d_source",
			OutFile:         sourcePath,
			InFiles:         []string{voxelModelPath},
			ImplicitInFiles: []string{meshExe},
			Variables:       variables,
		})
		addMesh3DBuild(env, rules, sourcePath, entry.OutMesh, entry.OutVertices, entry.OutIndices)
	}

	return nil
}

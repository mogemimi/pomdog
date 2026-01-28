// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"fmt"
	"os"
	"path/filepath"

	toml "github.com/pelletier/go-toml/v2"
)

type Config struct {
	Schemas   []*ConfigSchemas       `toml:"schemas"`
	GoTools   []*ConfigGoTools       `toml:"go_tools"`
	Downloads []*ConfigDownloadTools `toml:"download_tools"`
}

type ConfigSchemas struct {
	CppDir   string             `toml:"cpp_dir"`
	GoDir    string             `toml:"go_dir"`
	GoModDir string             `toml:"go_mod_dir"`
	Sources  []string           `toml:"sources"`
	GoModify []*ConfigSchemasGo `toml:"go_modify"`
}

type ConfigSchemasGo struct {
	Source       string `toml:"source"`
	SharedString bool   `toml:"shared_string"`
}

type ConfigGoTools struct {
	GoDir          string   `toml:"go_dir"`
	Tools          []string `toml:"tools"`
	BuildEarlyPass bool     `toml:"build_early_pass"`
}

type ConfigDownloadTools struct {
	URL      string `toml:"url"`
	Platform string `toml:"platform"`
	InFile   string `toml:"in_file"`
	OutFile  string `toml:"out_file"`
	SHA256   string `toml:"sha256"`
	MD5      string `toml:"md5"`
}

func (config *Config) ReadFile(file string) error {
	buf, err := os.ReadFile(file)
	if err != nil {
		return fmt.Errorf("os.ReadFile(): %w", err)
	}
	if err := toml.Unmarshal(buf, config); err != nil {
		return fmt.Errorf("toml.Unmarshal(): %w", err)
	}
	return nil
}

func (config *Config) ReadFiles(files []string) error {
	for _, file := range files {
		c := Config{}
		if err := c.ReadFile(file); err != nil {
			return fmt.Errorf("c.ReadFile(): %w", err)
		}
		config.Schemas = append(config.Schemas, c.Schemas...)
		config.GoTools = append(config.GoTools, c.GoTools...)
		config.Downloads = append(config.Downloads, c.Downloads...)
	}
	return nil
}

func (config *Config) ExpandEnv(appDir, buildDir, pomdogDir string) error {
	mapper := func(s string) string {
		switch s {
		case "APP_DIR":
			return appDir
		case "BUILD_DIR":
			return buildDir
		case "POMDOG_DIR":
			return pomdogDir
		case "POMDOG_BUILD_DIR":
			return filepath.Join(pomdogDir, "build")
		}
		return ""
	}

	for _, tools := range config.GoTools {
		tools.GoDir = os.Expand(tools.GoDir, mapper)
	}
	for _, schema := range config.Schemas {
		schema.CppDir = os.Expand(schema.CppDir, mapper)
		schema.GoDir = os.Expand(schema.GoDir, mapper)
		schema.GoModDir = os.Expand(schema.GoModDir, mapper)

		for i := range schema.Sources {
			schema.Sources[i] = os.Expand(schema.Sources[i], mapper)
		}
	}
	return nil
}

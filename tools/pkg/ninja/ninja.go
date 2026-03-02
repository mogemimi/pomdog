// Copyright mogemimi. Distributed under the MIT license.

package ninja

import (
	"fmt"
	"path/filepath"
	"strings"
)

type Generator struct {
	variables []*Variable
	builds    []*Build
	rules     []*Rule
}

type Rule struct {
	Name    string
	Command string
	DepFile string
	Deps    string
}

type Build struct {
	Rule             string
	InFiles          []string
	OutFile          string
	ImplicitInFiles  []string
	ImplicitOutFiles []string
	Variables        []*Variable
}

type Variable struct {
	Key    string
	Value  string
	AsPath bool
}

func NewVariableAsString(key, value string) *Variable {
	return &Variable{
		Key:    key,
		Value:  value,
		AsPath: false,
	}
}

func NewVariableAsPath(key, path string) *Variable {
	return &Variable{
		Key:    key,
		Value:  path,
		AsPath: true,
	}
}

func (gen *Generator) AddVariable(v *Variable) {
	gen.variables = append(gen.variables, v)
}

func (gen *Generator) AddRule(rule *Rule) {
	gen.rules = append(gen.rules, rule)
}

func (gen *Generator) AddBuild(build *Build) {
	gen.builds = append(gen.builds, build)
}

func escapeNinjaPath(file string) string {
	file = filepath.Clean(file)
	file = filepath.ToSlash(file)
	file = strings.ReplaceAll(file, " ", "$ ")
	file = strings.ReplaceAll(file, ":", "$:")
	return file
}

// Generate generates a Ninja build file as a string.
func (gen *Generator) Generate() string {
	result := ""

	if len(gen.variables) > 0 {
		for _, v := range gen.variables {
			value := ""
			if v.AsPath {
				value = escapeNinjaPath(v.Value)
			} else {
				value = v.Value
			}

			result += fmt.Sprintf("%s = %s\n", v.Key, value)
		}
		result += "\n"
	}

	for _, rule := range gen.rules {
		s := fmt.Sprintf("rule %s\n", rule.Name)
		s += fmt.Sprintf("  command = %s\n", rule.Command)
		if len(rule.DepFile) > 0 {
			s += fmt.Sprintf("  depfile = %s\n", rule.DepFile)
		}
		if len(rule.Deps) > 0 {
			s += fmt.Sprintf("  deps = %s\n", rule.Deps)
		}
		s += "\n"
		result += s
	}

	for _, build := range gen.builds {
		s := "build "
		if len(build.OutFile) > 0 {
			s += escapeNinjaPath(build.OutFile)
		}

		if len(build.ImplicitOutFiles) > 0 {
			s += " |"
			for _, file := range build.ImplicitOutFiles {
				s += fmt.Sprintf(" %s", escapeNinjaPath(file))
			}
		}

		s += fmt.Sprintf(": %s", build.Rule)

		for i, input := range build.InFiles {
			if i == 0 {
				if len(build.InFiles) >= 2 {
					s += " $\n "
				}
				s += fmt.Sprintf(" %s", escapeNinjaPath(input))
			} else {
				s += fmt.Sprintf(" $\n  %s", escapeNinjaPath(input))
			}
		}

		if len(build.ImplicitInFiles) > 0 {
			s += " |"
			for _, file := range build.ImplicitInFiles {
				s += fmt.Sprintf(" $\n  %s", escapeNinjaPath(file))
			}
		}
		s += "\n"

		if len(build.Variables) > 0 {
			for _, v := range build.Variables {
				value := ""
				if v.AsPath {
					value = escapeNinjaPath(v.Value)
				} else {
					value = v.Value
				}
				s += fmt.Sprintf("  %s = %s\n", v.Key, value)
			}
		}

		result += s
	}

	return result
}

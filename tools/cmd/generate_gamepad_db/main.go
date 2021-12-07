// Copyright mogemimi. Distributed under the MIT license.

package main

import (
	"bufio"
	"flag"
	"fmt"
	"io/ioutil"
	"log"
	"os"
	"strings"

	"github.com/pkg/errors"
)

var options struct {
	sourceFile string
	outputFile string
}

func main() {
	flag.StringVar(&options.sourceFile, "i", "", "source file")
	flag.StringVar(&options.outputFile, "o", "", "output file")
	flag.Parse()

	if err := generate(options.sourceFile, options.outputFile); err != nil {
		log.Fatalln(err)
	}
}

type field struct {
	UUID     string
	Name     string
	Fields   []string
	Platform string
}

func parse(src string) (fields []field) {
	scanner := bufio.NewScanner(strings.NewReader(src))
	for scanner.Scan() {
		line := scanner.Text()
		line = strings.TrimLeft(line, " \t")
		line = strings.TrimRight(line, " \t")
		if strings.HasPrefix(line, "#") {
			// NOTE: comment line
			continue
		}
		if len(line) <= 0 {
			// NOTE: empty line
			continue
		}
		fieldsStr := strings.Split(line, ",")

		f := field{
			UUID: fieldsStr[0],
			Name: fieldsStr[1],
		}

		for _, v := range fieldsStr[2:] {
			if strings.HasPrefix(v, "platform:") {
				f.Platform = strings.TrimPrefix(v, "platform:")
				continue
			}
			if len(v) <= 0 {
				continue
			}
			f.Fields = append(f.Fields, v)
		}

		fields = append(fields, f)
	}
	return fields
}

func generate(sourceFile string, outputFile string) error {
	dat, err := ioutil.ReadFile(sourceFile)
	if err != nil {
		return errors.Wrapf(err, "failed to read a file \"%s\"", sourceFile)
	}

	content := "// NOTE: This file is generated from from SDL_GameControllerDB by pomdog/tools/generate_gamepad_db.\n"
	content += "// Source: https://github.com/gabomdq/SDL_GameControllerDB\n"
	content += "\n"
	content += "const char* s_ControllerMappings[] = {\n"

	mappings := parse(string(dat))

	toPlatformMacro := func(platform string) string {
		switch platform {
		case "Windows":
			return "POMDOG_PLATFORM_WIN32"
		case "Linux":
			return "POMDOG_PLATFORM_LINUX"
		case "Mac OS X":
			return "POMDOG_PLATFORM_MACOSX"
		case "Android":
			return "POMDOG_PLATFORM_ANDROID"
		case "iOS":
			return "POMDOG_PLATFORM_APPLE_IOS"
		}
		return ""
	}

	platform := ""
	for _, m := range mappings {
		if platform != m.Platform {
			if len(platform) > 0 {
				content += "#endif\n"
			}
			content += fmt.Sprintf("#ifdef %s\n", toPlatformMacro(m.Platform))
			platform = m.Platform
		}
		content += fmt.Sprintf("    \"%s,%s,", m.UUID, m.Name)
		for _, f := range m.Fields {
			content += fmt.Sprintf("%s,", f)
		}
		content += "\",\n"
	}
	if len(platform) > 0 {
		content += "#endif\n"
	}
	content += "    nullptr\n};\n"

	f, err := os.Create(outputFile)
	if err != nil {
		return errors.Wrapf(err, "failed to create a file \"%s\"", outputFile)
	}
	defer f.Close()

	w := bufio.NewWriter(f)
	defer w.Flush()

	if _, err := w.WriteString(content); err != nil {
		return errors.Wrapf(err, "failed to write a file \"%s\"", outputFile)
	}

	return nil
}

package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	"strings"
)

type Env struct {
	InFile       string
	OutFile      string
	SharedString bool
}

func main() {
	env := &Env{}
	flag.StringVar(&env.InFile, "i", "", "input file")
	flag.StringVar(&env.OutFile, "o", "", "output file")
	flag.BoolVar(&env.SharedString, "sharedstring", false, "enable shared-string")
	flag.Parse()

	if err := run(env); err != nil {
		log.Fatal(err)
	}
}

func run(env *Env) error {
	content := ""
	{
		buf, err := os.ReadFile(env.InFile)
		if err != nil {
			return fmt.Errorf("os.ReadFile() failed: %w", err)
		}
		content = string(buf)
	}

	if env.SharedString {
		content = enableSharedString(content)
	}

	{
		f, err := os.Create(env.OutFile)
		if err != nil {
			return fmt.Errorf("os.Create() failed: %w", err)
		}
		defer f.Close()

		if _, err := f.WriteString(content); err != nil {
			return fmt.Errorf("f.WriteString() failed: %w", err)
		}
	}

	return nil
}

func enableSharedString(content string) string {
	content = strings.Replace(content, "Offset = builder.CreateString(", "Offset = builder.CreateSharedString(", -1)
	content = strings.Replace(content, "Offsets[j] = builder.CreateString(", "Offsets[j] = builder.CreateSharedString(", -1)

	return content
}

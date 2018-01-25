# Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

# Help:
# >>> python skeleton_generator.py -h
# >>> python skeleton_generator.py --help

# Version:
# >>> python skeleton_generator.py --version
# skeleton_generator version 0.1.0

# Generating sources:
# >>> python skeleton_generator.py path/to/file
# path/to: directory not exist, create new directory.
# Create implement file: path/to/file.hpp
# Create implement file: path/to/file.cpp
# >>> python skeleton_generator.py path/to/header_only --header
# Create implement file: header_only.hpp
# >>> python skeleton_generator.py path/to/implementation_only --impl
# Create implement file: path/to/implementation_only.cpp

# Objective-C++ supported:
# >>> python skeleton_generator.py ObjectiveCppFile --objc
# Create implement file: ObjectiveCppFile.hpp
# Create implement file: ObjectiveCppFile.mm


import sys
import os
import argparse
import subprocess
import uuid

from string import Template
from include_guard_uuid import GenerateHash


def CreateFilePath(directory_path, identifier, file_extension):
    path = os.path.join(directory_path, identifier + file_extension)
    if os.path.exists(path):
        print("error: {0} is exists".format(path))
        return False, ""
    if (not os.path.exists(directory_path)) and directory_path:
        os.makedirs(directory_path)
        print("{0}: directory not exist, create new directory.".format(directory_path))
    return True, path


def SaveSourceFileUTF8(path, content):
    f = open(path, 'wb')
    f.write(content)
    f.close()
    print "Create implement file: " + path


def GenerateCplusplusHeader(identifier, directory):
    # header file
    result = CreateFilePath(directory, identifier, ".hpp")
    if not result[0]:
        return
    SaveSourceFileUTF8(result[1], CreateClassHeader(identifier))


def GenerateCplusplusImplementation(identifier, directory):
    # implementation
    result = CreateFilePath(directory, identifier, ".cpp")
    if not result[0]:
        return
    SaveSourceFileUTF8(result[1], CreateImplement(identifier))


def GenerateObjectiveCppImplementation(identifier, directory):
    # implementation
    result = CreateFilePath(directory, identifier, ".mm")
    if not result[0]:
        return
    SaveSourceFileUTF8(result[1], CreateImplement(identifier))


def ParsingCommandLineAraguments():
    parser = argparse.ArgumentParser(prog='skeleton_generator',
                                     description='Generate skelton code.')
    parser.add_argument('identifier', default='def')
    parser.add_argument('--header', dest='header_flag', nargs='?',
                        default=False, const=True, help='Header file only',)
    parser.add_argument('--impl', dest='implementation_flag', nargs='?',
                        default=False, const=True, help='Implementation file only',)
    parser.add_argument('--objc', dest='objective_cpp', nargs='?',
                        default=False, const=True, help='Flag for Objective-C++ Sources')
    parser.add_argument('-v', '--version', action='version', version='%(prog)s version 0.1.0')
    #parser.add_argument('--include_guard', dest='IncludeGuard')
    #parser.add_argument('--class', dest='ClassName')
    #parser.add_argument('--pimpl')
    args = parser.parse_args()
    return args


def ParsingTargetFileFlags(args):
    enable_header_ = True
    enable_implementation_ = True

    if args.implementation_flag and args.header_flag:
        enable_header_ = True
        enable_implementation_ = True
    elif args.header_flag:
        enable_header_ = True
        enable_implementation_ = False
    elif args.implementation_flag:
        enable_header_ = False
        enable_implementation_ = True

    return enable_header_, enable_implementation_


def GetGitUserName():
    process = subprocess.Popen(["git", "config", "user.name"], stdout=subprocess.PIPE)
    username = process.communicate()[0]
    return username.rstrip()


def CreateHeader(copyright_holder):
    return Template("""// Copyright (c) $year $copyright_holder. Distributed under the MIT license.
""").substitute(year="2013-2018", copyright_holder=copyright_holder)


def CreateNamespace(name, content="\n// Insert code here to run your application\n"):
    return Template("""namespace $name {
$content
} // namespace $name""").substitute(name=name, content=content)


def CreateIncludeGuardSection(content=""):
    return Template("""#pragma once

$content""").substitute(content=content)


def CreateIncludeHeader(identifier):
    return Template("#include \"$header_file\"\n").substitute(header_file=(identifier+".hpp"));


def CreateClassHeader(identifier_without_directory):
    return_code = '\n'
    return CreateHeader(GetGitUserName()) +\
           return_code +\
           CreateIncludeGuardSection(
               CreateNamespace("Pomdog", CreateNamespace("Detail")) + return_code)


def CreateImplement(identifier_without_directory):
    return_code = '\n'
    return CreateHeader(GetGitUserName()) +\
           return_code +\
		   CreateIncludeHeader(identifier_without_directory) +\
		   return_code +\
           CreateNamespace("Pomdog", CreateNamespace("Detail")) +\
           return_code


def main():
    args = ParsingCommandLineAraguments()

    #print("args = {0}".format(vars(args))) # for Debug

    identifier = os.path.basename(args.identifier)
    directory = os.path.dirname(args.identifier)

    enable_header_, enable_implementation_ = ParsingTargetFileFlags(args)

    if enable_header_:
        GenerateCplusplusHeader(identifier, directory)

    if args.objective_cpp and enable_implementation_:
        GenerateObjectiveCppImplementation(identifier, directory)
    elif enable_implementation_:
        GenerateCplusplusImplementation(identifier, directory)


if __name__ == '__main__':
    main()

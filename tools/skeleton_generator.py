#
#  Copyright (C) 2013 mogemimi.
#
#  Distributed under the MIT License.
#  See accompanying file LICENSE.md or copy at
#  http://enginetrouble.net/pomdog/LICENSE.md for details.
#

# examples - Version:
# >>> python skeleton_generator.py --version
# skeleton_generator version 0.1.0
#
# examples - Generating sources:
# >>> python skeleton_generator.py path/to/file
# path/to: directory not exist, create new directory.
# Create implement file: path/to/file.hpp
# Create implement file: path/to/file.cpp
# >>> python skeleton_generator.py path/to/header_only --header
# Create implement file: header_only.hpp
# >>> python skeleton_generator.py path/to/implementation_only --impl
# Create implement file: path/to/implementation_only.cpp
#
# examples - Objective-C++ supported:
# >>> python skeleton_generator.py ObjectiveCppFile --objc
# Create implement file: ObjectiveCppFile.hpp
# Create implement file: ObjectiveCppFile.mm
#
# examples - Help:
# >>> python skeleton_generator.py -h
# >>> python skeleton_generator.py --help

import sys
import os
import argparse
import subprocess
import uuid
from string import Template

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
    bom = u'\ufeff'
    f = open(path, 'wb')
    f.write(bom.encode('utf-8'))
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
    SaveSourceFileUTF8(result[1], CreateImplement())

def GenerateObjectiveCppImplementation(identifier, directory):
    # implementation
    result = CreateFilePath(directory, identifier, ".mm")
    if not result[0]:
        return
    SaveSourceFileUTF8(result[1], CreateImplement())

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

def Run():
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

def GetGitUserName():
    process = subprocess.Popen(["git", "config", "user.name"], stdout=subprocess.PIPE)
    username = process.communicate()[0]
    return username.rstrip()

def CreateHeader(copyright_holder):
    return Template("""//
//  Copyright (C) $year $copyright_holder.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//
""").substitute(year="2013", copyright_holder=copyright_holder)

def CreateNamespace(name, content="    // Insert code here to run your application"):
    return Template("""namespace $name {
$content
}// namespace $name""").substitute(name=name, content=content)

def CreateIncludeGuardUUID(identifier):
    prefix_ = "POMDOG_"
    suffix_ = "_HPP"
    uuid_ = str(uuid.uuid1()).replace('-', '_').upper()
    include_guard = prefix_ + str(identifier).upper() + '_' + uuid_ + suffix_
    return include_guard

def CreateIncludeGuardMSVCStyle():
    return """#if (_MSC_VER > 1000)
#	pragma once
#endif
"""

def CreateIncludeGuardSection(identifier, content=""):
    return Template("""#ifndef $include_guard
#define $include_guard

$msvc_style_pragma_once
$content
#endif // !defined($include_guard)""").substitute(
        include_guard=CreateIncludeGuardUUID(identifier),
        msvc_style_pragma_once=CreateIncludeGuardMSVCStyle(),
        content=content)

def CreateClassHeader(identifier_without_directory):
    return_code = '\n'
    return CreateHeader(GetGitUserName()) +\
           return_code +\
           CreateIncludeGuardSection(
               identifier_without_directory,
               CreateNamespace("Pomdog", CreateNamespace("Details")) + return_code) +\
           return_code

def CreateImplement():
    return_code = '\n'
    return CreateHeader(GetGitUserName()) +\
           return_code +\
           CreateNamespace("Pomdog", CreateNamespace("Details")) +\
           return_code

Run()

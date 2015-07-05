# Copyright (c) 2013-2015 mogemimi.
# Distributed under the MIT license. See LICENSE.md file for details.

import sys
import os
import platform
import argparse
import subprocess
import shutil


from download_dependencies import DownloadDependencies


def CreateGYPCommands(gypfile, format, option=None):
    gypCommand = [
        'python tools/gyp/gyp_main.py',
        gypfile,
        '--depth=.',
    ]
    msvsOptions = [
        '-f msvs -G msvs_version=2015',
        '--generator-output=build.msvs',
        '-Dcomponent=static_library',
    ]
    xcodeOptions = [
        '-f xcode',
        '--generator-output=build.xcodefiles',
        '-Dcomponent=shared_library',
    ]
    makefilesOptions = [
        '-f make',
        '--generator-output=build.makefiles',
        '-Dcomponent=static_library',
    ]

    if 'msvs' == format:
        gypCommand = gypCommand + msvsOptions
    elif 'xcode' == format:
        gypCommand = gypCommand + xcodeOptions
    elif 'make' == format:
        gypCommand = gypCommand + makefilesOptions

    if option:
        gypCommand = gypCommand + [option]

    command = ' '.join(gypCommand)
    return command


def ParsingCommandLineAraguments():
    parser = argparse.ArgumentParser(description='Generate your projects for Pomdog.')
    parser.add_argument('gypFile', default='def')
    parser.add_argument('-f', '--format', dest='format', default=None)
    parser.add_argument('-D', dest='defines', nargs='?', default=None)
    parser.add_argument('-v', '--version', action='version', version='%(prog)s version 0.1.0')
    args = parser.parse_args()
    return args


def ChooseProjectFileFormat(format = None):
    if format != None:
        return format
    if platform.system() == 'Windows':
        return 'msvs'
    elif platform.system() == 'Darwin':
        return 'xcode'
    elif platform.system() == 'Linux':
        return 'make'
    return 'make'


def GetOutputDirectory(format):
    if format == 'msvs':
        return 'build.msvs'
    elif format == 'xcode':
        return 'build.xcodefiles'
    elif format == 'make':
        return 'build.makefiles'
    return 'build'


def RunGYP(command):
    process = subprocess.Popen(command.split(), stdout=subprocess.PIPE)
    message = process.communicate()[0]
    return message


def CreateVcxprojUserFile(outputDirectory, gypFile):
    pathBase, ext = os.path.splitext(gypFile)
    userFile = os.path.normpath(os.path.join(
        outputDirectory, pathBase + '.vcxproj.user'))
    directory = os.path.dirname(userFile)

    if not os.path.exists(directory):
      return
    if not os.path.isdir(directory):
      return
    if os.path.exists(userFile):
      return

    workingDirectory = ('$(ProjectDir)..\\..\\..\\' +
        os.path.normcase(os.path.dirname(gypFile)))

    content = """<?xml version="1.0" encoding="utf-8"?>
<Project xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">
    <LocalDebuggerWorkingDirectory>{0}</LocalDebuggerWorkingDirectory>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">
    <LocalDebuggerWorkingDirectory>{0}</LocalDebuggerWorkingDirectory>
  </PropertyGroup>
</Project>
""".format(workingDirectory)

    f = open(userFile, 'w')
    f.write(content)
    f.close()


def GenerateProjects(gypFile, format, options = None, verbose = False):
    head, tail = os.path.split(os.path.normpath(os.path.dirname(gypFile)))
    if head == 'examples':
        pathOption = ' -Dpomdog_dir=../../'
        options = options + pathOption if options else pathOption

    gypCommand = CreateGYPCommands(gypFile, format, options)

    outputDirectory = GetOutputDirectory(format)
    if verbose:
        print('==> ' + 'Generating ' + format + ' projects in ' + outputDirectory)
        print('==> ' + gypCommand)
    RunGYP(gypCommand)

    if format == 'msvs':
        CreateVcxprojUserFile(outputDirectory, gypFile)


def main():
    args = ParsingCommandLineAraguments()

    format = ChooseProjectFileFormat(args.format)
    options = ('-D' + args.defines) if args.defines else None

    pomdogPath = os.path.normpath(os.path.join(os.path.dirname(__file__), '..'))
    DownloadDependencies(pomdogPath)

    GenerateProjects(args.gypFile, format, options, True)


if __name__ == '__main__':
    main()

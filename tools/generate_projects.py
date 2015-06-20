# Copyright (c) 2013-2015 mogemimi.
# Distributed under the MIT license. See LICENSE.md file for details.

import sys
import os
import platform
import argparse
import subprocess
import uuid
import shutil


from download_dependencies import DownloadDependencies


def GenerateXcodeProjects(gypfile, option=None):
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

    if platform.system() == 'Windows':
        gypCommand = gypCommand + msvsOptions
    elif platform.system() == 'Darwin':
        gypCommand = gypCommand + xcodeOptions

    if option:
        gypCommand = gypCommand + [option]

    command = ' '.join(gypCommand)
    process = subprocess.Popen(command.split(), stdout=subprocess.PIPE)
    message = process.communicate()[0]
    return message


def CreateNewMSVSUserFile(userFilePath, workingDirectory):
    if not os.path.exists('build.msvs'):
      return
    if not os.path.isdir('build.msvs'):
      return

    userFileName = os.path.join('build.msvs', userFilePath)
    if os.path.exists(userFileName):
      return

    localDebuggerWorkingDirectory = workingDirectory

    content = """<?xml version="1.0" encoding="utf-8"?>
<Project xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">
    <LocalDebuggerWorkingDirectory>{0}</LocalDebuggerWorkingDirectory>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">
    <LocalDebuggerWorkingDirectory>{0}</LocalDebuggerWorkingDirectory>
  </PropertyGroup>
</Project>
""".format(localDebuggerWorkingDirectory)

    f = open(userFileName, 'w')
    f.write(content)
    f.close()

def main():
    pomdogPath = os.path.join(os.path.dirname(__file__), "..")
    pomdogPath = os.path.normpath(pomdogPath)
    DownloadDependencies(pomdogPath)

    GenerateXcodeProjects('test/TestApp/TestApp.gyp')
    GenerateXcodeProjects('examples/QuickStart/QuickStart.gyp', '-D pomdog_dir=../../')

    CreateNewMSVSUserFile(
      'test/TestApp/TestApp.vcxproj.user',
      '..\\..\\..\\test\\TestApp')
    CreateNewMSVSUserFile(
      'examples/QuickStart/QuickStart.vcxproj.user',
      '..\\..\\..\\examples\\QuickStart')


if __name__ == '__main__':
    main()

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


def main():
    pomdogPath = os.path.join(os.path.dirname(__file__), "..")
    pomdogPath = os.path.normpath(pomdogPath)
    DownloadDependencies(pomdogPath)

    GenerateXcodeProjects('test/TestApp/TestApp.gyp')
    GenerateXcodeProjects('examples/QuickStart/QuickStart.gyp', '-D pomdog_dir=../../')


if __name__ == '__main__':
    main()

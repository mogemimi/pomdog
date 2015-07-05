# Copyright (c) 2013-2015 mogemimi.
# Distributed under the MIT license. See LICENSE.md file for details.

import sys
import os
import shutil


from gyp import ChooseProjectFileFormat
from gyp import GenerateProjects
from download_dependencies import DownloadDependencies


def main():
    pomdogPath = os.path.normpath(os.path.join(os.path.dirname(__file__), '..'))
    DownloadDependencies(pomdogPath)

    format = ChooseProjectFileFormat()

    print('==> ' + 'Generating ' + format + ' projects')
    GenerateProjects('test/FrameworkTest/unittest.gyp', format)
    GenerateProjects('test/TestApp/TestApp.gyp', format)
    GenerateProjects('examples/QuickStart/QuickStart.gyp', format)
    GenerateProjects('examples/Pong/Pong.gyp', format)


if __name__ == '__main__':
    main()

# Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

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
    GenerateProjects('examples/TestGame/TestGame.gyp', format)


if __name__ == '__main__':
    main()

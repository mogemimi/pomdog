# Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

import sys
import os
import platform
import argparse
import subprocess
import uuid
import shutil


pomdogDependencies = [
    {
        'dest': 'tools/gyp',
        'repository': 'https://chromium.googlesource.com/external/gyp.git',
    },
]


def GitCloneRepository(url, dest):
    command = ' '.join(["git clone --depth=1", url, dest])
    print(command)
    process = subprocess.Popen(command.split(), stdout=subprocess.PIPE)
    message = process.communicate()[0]
    return message


def DownloadDependencies(pomdogPath):
    for dependency in pomdogDependencies:
        dest = os.path.normpath(os.path.join(pomdogPath, dependency['dest']))
        if not os.path.exists(dest):
            repositoryUrl = dependency['repository']
            GitCloneRepository(repositoryUrl, dest)
        if not os.path.isdir(dest):
            print("Error: {0} is not directory".format(dest))
            return False
    return True


def main():
    pomdogPath = os.path.join(os.path.dirname(__file__), "..")
    pomdogPath = os.path.normpath(pomdogPath)
    DownloadDependencies(pomdogPath)


if __name__ == '__main__':
    main()

# Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

# Help:
# >>> python include_guard_uuid.py -h
# >>> python include_guard_uuid.py --help

# Version:
# >>> python skeleton_generator.py --version
# skeleton_generator version 0.1.0

# Generating uuid:
# >>> python tools/include_guard_uuid.py
# 2A348002_A1F6_4C68_9769_EF075B9A917A
# >>> python tools/include_guard_uuid.py foo.txt
# >>> cat foo.txt
# 93EBA48F_4E7C_4419_BAB2_8026EB30ECA1


import sys
import argparse
import uuid


def ParsingCommandLineAraguments():
    parser = argparse.ArgumentParser(prog='include_guard_uuid',
                                     description='Generate a uuid for include guard.')
    parser.add_argument('outfile', nargs='?', type=argparse.FileType('w'), default=sys.stdout)
    parser.add_argument('-v', '--version', action='version', version='%(prog)s version 0.1.0')
    args = parser.parse_args()
    return args

def GenerateUUID():
    return str(uuid.uuid4()).replace('-', '_').upper()

def GenerateHash():
    return str(uuid.uuid4())[0:8].upper()

def main():
    args = ParsingCommandLineAraguments()

    #print("args = {0}".format(vars(args))) # for Debug

    uuid_ = GenerateHash()
    args.outfile.write(uuid_ + '\n')

if __name__ == '__main__':
    main()

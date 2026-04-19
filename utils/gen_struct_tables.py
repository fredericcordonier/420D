from typing import NamedTuple, TextIO, BinaryIO

from pathlib import Path
from argparse import ArgumentParser

from struct_tables import read_structures, Struct
from file_writer import SrcFileWriter, HeaderFileWriter


def parse_args() -> NamedTuple:
    parser = ArgumentParser(description="Supported arguments")
    parser.add_argument('-f', "--infile", type=Path, help='Input file name')
    parser.add_argument('-s', "--srcdir", type=Path, default=Path("../src/c"), nargs='?', help='Source directory')
    parser.add_argument('-i', "--incdir", type=Path, default=Path("../src/inc"), help="Include directory")
    parser.add_argument("--list-headers", default=False, action='store_true', help="List generated headers, don't generate")
    parser.add_argument("--list-sources", default=False, action='store_true', help="List generated sources, don't generate")
    parser.add_argument("--gen-mkfile",   default=False, action='store_true', help="Generate makefile for generated files")
    return parser.parse_args()

def write_struct_files(args, structures: list[Struct]):
    if args.list_headers:
        print(' '.join([str(args.incdir / f'{struct.name}.h') for struct in structures]))
    elif args.list_sources:
        print(' '.join([str(args.srcdir / f'{struct.name}.c') for struct in structures]))
    elif args.gen_mkfile:
        with open("generated.mk", 'w') as mk_file:
            header_list = ' '.join([str(args.incdir / f'{struct.name}.h') for struct in structures])
            sources_list = ' '.join([str(args.srcdir / f'{struct.name}.c') for struct in structures])
            mk_file.write(f'GEN_HEADERS := {header_list}\n')
            mk_file.write(f'GEN_SOURCES := {sources_list}')
    else:
        for struct in structures:
            ## Generate structure include file
            header_file = HeaderFileWriter(args.incdir, struct.name)
            header_file.included_files = [
                "param_def.h"               # Needed to know the structure field_def.h
            ] + struct.included_in_h
            header_file.add_lines(struct.get_header_file_content())
            header_file.write()

            ## Generate structure source file
            src_file = SrcFileWriter(args.srcdir, struct.name)
            src_file.included_files = [
                    "vxworks.h",                # Needed for NULL definition
                    "param_def.h",              # Needed to know the structure field_def.h
                    f'{struct.name}.h'
                ] + struct.included_in_c
            src_file.add_lines(struct.get_src_file_content())
            src_file.write()

def generate_struct_files(args: NamedTuple):
    structures = read_structures(args)
    write_struct_files(args, structures)

def main():
    args = parse_args()
    generate_struct_files(args)


if __name__ == "__main__":
    main()

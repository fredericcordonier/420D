from typing import Dict, Union, TextIO

import json
from pathlib import Path
from argparse import ArgumentParser

INC_FILE_HEADER = """/***********************************************************************************************************
*   File name: {}
*
*   File used for the serialization of the structure {}. Structure and field names are hashed, the hash is
*     used as a tag for serialized object. The data in this file are the hash, the index of the field in
*     the structure, and the size of the field.
*
*   This file is generated automatically. It should not be modified, as modifications will be overwritten.
************************************************************************************************************/
#pragma once

"""

SRC_FILE_HEADER = """/***********************************************************************************************************
*   File name: {}
*
*   File used for the serialization of the structure {}. Structure and field names are hashed, the hash is
*     used as a tag for serialized object. The data in this file are the hash, the index of the field in
*     the structure, and the size of the field.
*
*   This file is generated automatically. It should not be modified, as modifications will be overwritten.
************************************************************************************************************/

"""

class DuplicateHashValueException(Exception): pass


def parse_args():
    parser = ArgumentParser(description="Supported arguments")
    parser.add_argument('-f', "--infile", type=Path, help='Input file name')
    parser.add_argument('-s', "--srcdir", type=Path, default=Path("../src/c"), nargs='?', help='Source directory')
    parser.add_argument('-i', "--incdir", type=Path, default=Path("../src/inc"), help="Include directory")
    parser.add_argument("--list-headers", default=False, action='store_true', help="List generated headers, don't generate")
    parser.add_argument("--list-sources", default=False, action='store_true', help="List generated sources, don't generate")
    parser.add_argument("--gen-mkfile",   default=False, action='store_true', help="Generate makefile for generated files")
    return parser.parse_args()


## Compute a basic hash of an identifier name, to build hash tables used for saved params files parsing
def compute_hash(data: str) -> int:
    MULTIPLIER = 31
    h = 0
    for c in data:
        h = 0xFFFFFFFF & (MULTIPLIER * h + ord(c))
    return h


class Type:
    def __init__(self, base_type: str='', size: str=''):
        self.base_type = base_type
        self.size = size

    def to_string(self, f_name: str) -> str:
        raise TypeError('Method must be surcharged')


class IntType(Type):
    def __init__(self):
        super().__init__('int', '1')

    def to_string(self, f_name: str) -> str:
        return f'{self.base_type} {f_name}'

class IntArrayType(Type):
    def __init__(self, size: str):
        super().__init__('int', size)

    def to_string(self, f_name: str) -> str:
        return f'{self.base_type} {f_name}[{self.size}]'


class Field:
    def __init__(self, name: str, field_type: Type):
        self.name = name
        self.field_type: Type = field_type

    def has_default_value(self) -> bool:
        return False

    def __str__(self) -> str:
        return self.field_type.to_string(self.name)


class FieldWithDefaultValue(Field):
    def __init__(self, name: str, field_type: Type, default_value: str):
        super().__init__(name, field_type)
        self.default_value: str = default_value

    def has_default_value(self) -> bool:
        return True


class Struct:
    def __init__(self, name: str):
        self.name = name
        self.fields: list[Field] = []
        self.included_in_c: list[str] = []
        self.included_in_h: list[str] = []
        self.name_hash: int = 0
        self.hashes: dict[int, Field] = {}
        self.hashtable_def = f'const list_field_def_t s_g_{self.name}_hashtable'
        self.struct_default_name = f'const {self.name} s_g_{self.name}_default'

    def append(self, new_field: Field):
        self.fields.append(new_field)

    def add_include_file(self, filename: str, dest: str):
        if dest == 'src':
            self.included_in_c.append(filename)
        elif dest == 'header':
            self.included_in_h.append(filename)

    def structure_definition(self) -> str:
        if len(self.fields) > 0:
            ret_value = 'typedef struct {\n'
            ret_value += '\n'.join([f'  {str(f)};' for f in self.fields])
            ret_value += f'\n}} {self.name};\n'
        else:
            ret_value = ''
        return ret_value

    def compute_hashes(self, existing_hashes: list[int]) -> list[int]:
        self.name_hash = compute_hash(self.name)
        for field in self.fields:
            hash = compute_hash(f'{self.name}.{field.name}')
            if hash in existing_hashes:
                raise DuplicateHashValueException((f'{self.name}.{field.name:<40}: {hash:08x}'))
            else:
                existing_hashes.append(hash)
                self.hashes.update({hash: field})
        return existing_hashes

    def structure_hash_table(self) -> str:
        ret_val: str = f'static const field_def_t as_{self.name}_hashtable[] = {{\n'
        for hash in sorted(self.hashes.keys()):
            field = self.hashes[hash]
            ret_val += f'    {{0x{hash:08x}, (long)(&((({self.name} *)NULL)->{field.name})), sizeof((({self.name} *)NULL)->{field.name})/sizeof(int)' + '},\n'
        ret_val += '};\n\n'
        ret_val += f'{self.hashtable_def} = LIST(as_{self.name}_hashtable);'
        return ret_val

    def structure_init_table(self) -> str:
        init_fields = [f for f in self.fields if f.has_default_value()]
        if len(init_fields):
            ret_val: str = f'{self.struct_default_name} = {{\n'
            ret_val += '\n'.join([f'    .{f.name} = {f.default_value},' for f in init_fields])
            ret_val += '\n'
            ret_val += '};\n\n'
        else:
            ret_val = ''
        return ret_val

    def write_header_file(self, inc_file: TextIO):
        inc_file.write(INC_FILE_HEADER.format((f'{self.name}.h'), f'{self.name}'))
        included_files: list[str] = [
            "param_def.h"               # Needed to know the structure field_def.h
        ] + self.included_in_h
        inc_file.write('\n'.join([f'#include "{filename}"' for filename in included_files]))
        inc_file.write('\n')
        inc_file.write(self.structure_definition())
        inc_file.write(f'\n\n#define C_{self.name.upper()}_TAG  0x{self.name_hash:08x}')
        inc_file.write(f'\n\nextern {self.hashtable_def};')
        inc_file.write(f'\n\nextern {self.struct_default_name};')
        inc_file.write('\n\n')

    def write_src_file(self, src_file: TextIO):
        src_file.write(SRC_FILE_HEADER.format((f'{self.name}.c'), f'{self.name}'))
        included_files = [
            "vxworks.h",                # Needed for NULL definition
            "param_def.h",              # Needed to know the structure field_def.h
            f'{self.name}.h'
        ] + self.included_in_c
        src_file.write('\n'.join([f'#include "{filename}"' for filename in included_files]))
        src_file.write('\n\n')
        src_file.write(self.structure_init_table())
        src_file.write('\n\n')
        src_file.write(self.structure_hash_table())
        src_file.write('\n\n')


def read_struct_json(struct_name: str, included: dict[str, list[str]], fields_json: list[dict[str, Union[str, Dict[str, str]]]]) -> Struct:
    s = Struct(struct_name)
    [s.add_include_file(f, d) for d in included.keys() for f in included[d]]
    for f in fields_json:
        field_name: str = f['name']
        if f['type']['base_type'] == 'int':
            if f['type']['size'] == '1':
                field_type = IntType()
            else:
                field_type = IntArrayType(f['type']['size'])
        else:
            continue
        if 'default' in f.keys():
            new_field = FieldWithDefaultValue(field_name, field_type, f['default'])
        else:
            new_field = Field(field_name, field_type)
        s.append(new_field)
    return s


def main():
    structures: list[Struct] = []
    # Must check that there is no duplicate in hashes
    existing_hashes = []
    args = parse_args()
    with open(args.infile, 'r') as infile:
        struct_json = json.load(infile)
        struct_list = struct_json['structs']
        for s in struct_list:
            new_struct = read_struct_json(s['name'], s['included'], s['fields'])
            existing_hashes = new_struct.compute_hashes(existing_hashes)
            structures.append(new_struct)
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
            with open(args.incdir / f'{struct.name}.h', 'w') as inc_file:
                struct.write_header_file(inc_file)
            ## Generate structure include file
            with open(args.srcdir / f'{struct.name}.c', 'w') as src_file:
                src_file.write(SRC_FILE_HEADER.format((f'{struct.name}.c'), f'{struct.name}'))
                included_files = [
                    "vxworks.h",                # Needed for NULL definition
                    "param_def.h",              # Needed to know the structure field_def.h
                    f'{struct.name}.h'
                ] + struct.included_in_c
                src_file.write('\n'.join([f'#include "{filename}"' for filename in included_files]))
                src_file.write('\n\n')
                # for ifile in struct.included:
                #     src_file.write(f'#include "{ifile}"\n')
                src_file.write(struct.structure_init_table())
                src_file.write('\n\n')
                src_file.write(struct.structure_hash_table())
                src_file.write('\n\n')
    # print('\n\n'.join([s.structure_definition() for s in structures]))



if __name__ == "__main__":
    main()

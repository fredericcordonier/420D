from typing import Dict
import os
import pathlib
import re

SRC_FOLDER = pathlib.Path('../src/')
C_FOLDER   = SRC_FOLDER / 'c'
DEF_FOLDER = SRC_FOLDER / 'def'
INC_FOLDER = SRC_FOLDER / 'inc'

FILE_HEADER = """/***********************************************************************************************************
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

class StructDef:
    def __init__(self, struct_name: str, inc_file_name: str):
        self.struct_name: str = struct_name
        self.hash_table: Dict[int, str] = {}
        self.inc_file_name = inc_file_name

    def append_hash(self, key: int, val: str):
        self.hash_table[key] = val

    def get_inc_out_name(self):
        return INC_FOLDER / f'{self.struct_name}.h'

    def get_src_out_name(self) -> pathlib.Path :
        return C_FOLDER / f'{self.struct_name}.c'

    def write_hash_table(self):
        lines: list[str] = []
        hashes = sorted(self.hash_table.keys())
        inc_out = self.get_inc_out_name()
        with open(inc_out, 'w') as out_file:
            out_file.write(FILE_HEADER.format(inc_out.name, self.struct_name))
            out_file.write('#pragma once\n\n')
            included_files = ['string.h',
                              'stdlib.h',
                            #   'param_def.h',
                              self.inc_file_name,
                              inc_out.name]
            for incf in included_files:
                out_file.write(f'#include "{incf}"\n')
            out_file.write(f'#define C_SIZE_{self.struct_name.upper()}_DEF      {len(hashes) + 1}\n\n')
            out_file.write(f'extern const field_def {self.struct_name}_structure[C_SIZE_{self.struct_name.upper()}_DEF];\n')
            out_file.write('\n\n')
        src_out = self.get_src_out_name()
        with open(src_out, 'w') as out_file:
            out_file.write(FILE_HEADER.format(src_out.name, self.struct_name))
            included_files = ['string.h',
                              'stdlib.h',
                              'param_def.h',
                              self.inc_file_name,
                              inc_out.name]
            for incf in included_files:
                out_file.write(f'#include "{incf}"\n')
            out_file.write(f'const field_def {self.struct_name}_structure[C_SIZE_{self.struct_name.upper()}_DEF] = ' + '{\n')
            for hash in hashes:
                field_name = self.hash_table[hash]
                lines.append('  {' + f'0x{hash:08x}, (long)(&((({self.struct_name} *)NULL)->{field_name})), sizeof((({self.struct_name} *)NULL)->{field_name})/sizeof(int)' + '},')
                # lines.append('  {' + f'0x{hash:08x}, (&((({self.struct_name} *)NULL)->{field_name})), sizeof((({self.struct_name} *)NULL)->{field_name})' + '},')
            lines.append('  {0, 0, 0}')
            out_file.write('\n'.join(lines))
            out_file.write('\n};\n\n')


class DefFile:
    def __init__(self, file_name: pathlib.Path):
        self.file_name = file_name
        self.structures: list[StructDef] = []
        self.hash_table: Dict[int, str] = {}
        self.hash_values: list[int] = []

    def _compute_hash(self, name: str) -> int:
        MULTIPLIER = 31
        h = 0
        for c in name:
            h = 0xFFFFFFFF & (MULTIPLIER * h + ord(c))
        return h

    def read_structure(self, struct_name: str, file_name: pathlib.Path, inc_file_name: str):
        new_struct = StructDef(struct_name, inc_file_name)
        with open(file_name) as i_f:
            lines = i_f.read()
        pattern = r'PARAM_INT_DEF\(\s*(\w+)\s*,\s*(\w+)\s*\)'
        struct_fields = [field for struct, field in re.findall(pattern, lines) if struct == struct_name]
        pattern = r'PARAM_INT_ARRAY_DEF\(\s*(\w+)\s*,\s*(\w+)\s*,\s*\w+\)'
        struct_fields += [field for struct, field in re.findall(pattern, lines) if struct == struct_name]
        for field in struct_fields:
            param_name = f'{struct_name}.{field}'
            param_name_hash = self._compute_hash(param_name)
            if param_name_hash in self.hash_values:
                raise DuplicateHashValueException(f'{param_name:<40}: {param_name_hash:08x}')
            new_struct.append_hash(param_name_hash, field)
        self.structures.append(new_struct)

    def write_hash_tables(self):
        with open(self.file_name, 'w') as out_file:
            out_file.write(FILE_HEADER.format(self.file_name.name, 'containing parameters within 420D'))
            out_file.write('#pragma once\n\n')
            # for struct in self.structures:
            #     out_file.write(f'#include "{struct.get_inc_out_name().name}"\n')
            out_file.write('\n\n')
            out_file.write("""// Struct used to define a structure field
typedef struct {
    unsigned int field_name_hash;
    int field_offset_in_struct;
    int field_size;
} field_def;

                           """)
            for struct in self.structures:
                struct.write_hash_table()
                tag_name = f'{struct.struct_name}_tag'
                out_file.write(f'#define {tag_name:<40}0x{self._compute_hash(struct.struct_name):08x}\n')


def main():
    cur_dir = pathlib.Path.cwd()
    os.chdir(pathlib.Path(__file__).parent.resolve())
    def_file = DefFile(INC_FOLDER / 'param_def.h')
    def_file.read_structure('settings_t', DEF_FOLDER / 'settings_t.def', 'settings.h')
    # def_file.read_structure('dpr_data_t', DEF_FOLDER / 'dprdata.def', 'firmware/camera.h')
    def_file.read_structure('menu_order_t', DEF_FOLDER / 'menu_order_t.def', 'settings.h')
    def_file.write_hash_tables()
    os.chdir(cur_dir)

if __name__  == "__main__":
    main()

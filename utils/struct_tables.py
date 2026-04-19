from typing import Dict, Union, TextIO
import json
from file_writer import SrcFileWriter, HeaderFileWriter

class DuplicateHashValueException(Exception): pass


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

    def structure_definition(self) -> list[str]:
        ret_value: list[str] = []
        if len(self.fields) > 0:
            ret_value.append('typedef struct {')
            ret_value += [f'  {str(f)};' for f in self.fields]
            ret_value.append(f'}} {self.name};')
        return ret_value

    def structure_hash_table(self) -> list[str]:
        ret_val: list[str] = [f'static const field_def_t as_{self.name}_hashtable[] = {{']
        for hash in sorted(self.hashes.keys()):
            field = self.hashes[hash]
            ret_val.append(f'    {{0x{hash:08x}, (long)(&((({self.name} *)NULL)->{field.name})), sizeof((({self.name} *)NULL)->{field.name})/sizeof(int)' + '},')
        ret_val.append('};')
        ret_val  += (['', ''])
        ret_val.append(f'{self.hashtable_def} = LIST(as_{self.name}_hashtable);')
        return ret_val

    def structure_init_table(self) -> list[str]:
        ret_val: list[str] = []
        init_fields = [f for f in self.fields if f.has_default_value()]
        if len(init_fields):
            ret_val.append(f'{self.struct_default_name} = {{')
            ret_val += [f'    .{f.name} = {f.default_value},' for f in init_fields]
            ret_val += ['', '};', '', '']
        else:
            ret_val = []
        return ret_val

    def get_header_file_content(self) -> list[str]:
        ret_val = self.structure_definition()
        ret_val.append(f'#define C_{self.name.upper()}_TAG  0x{self.name_hash:08x}')
        ret_val.append(f'extern {self.hashtable_def};')
        ret_val.append(f'extern {self.struct_default_name};')
        ret_val += ['', '']
        return ret_val

    def get_src_file_content(self) -> list[str]:
        ret_val = self.structure_init_table()
        ret_val += ['', '']
        ret_val += self.structure_hash_table()
        ret_val += ['', '']
        return ret_val

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

def read_structures(args) -> list[Struct]:
    structures: list[Struct] = []
    # Must check that there is no duplicate in hashes
    existing_hashes = []
    with open(args.infile, 'r') as infile:
        struct_json = json.load(infile)
        struct_list = struct_json['structs']
        for s in struct_list:
            new_struct = read_struct_json(s['name'], s['included'], s['fields'])
            existing_hashes = new_struct.compute_hashes(existing_hashes)
            structures.append(new_struct)
    return structures


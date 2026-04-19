from typing import NamedTuple, TextIO, BinaryIO

from pathlib import Path
from argparse import ArgumentParser

from struct_tables import read_structures


class SettingsFile:
    def __init__(self, filename: str):
        self.filename = filename
        with open(self.filename, "rb") as in_file:
            self.data = in_file.read()

    def read_int(self) -> int:
        pow = [256 ** i for i in range(4)]
        int_data = [pow[i] * self.data[i] for i in range(4)]
        value = sum(int_data)
        self.data = self.data[4:]
        return value

    def read_data(self, len: int):
        data = self.data[:len]
        self.data = self.data[len:]

    def read_tl(self) -> tuple[int, int]:
        tag = self.read_int()
        length = self.read_int()
        return tag, length


def parse_args() -> NamedTuple:
    parser = ArgumentParser(description="Supported arguments")
    parser.add_argument("binary_file", type=Path, help='Binary file to read')
    parser.add_argument('-f', "--infile", type=Path, help='Structure definition file')
    return parser.parse_args()

def generate_ini_files(args: NamedTuple):
    structures = read_structures(args)
    binary_file = SettingsFile(args.binary_file)
    tag, length = binary_file.read_tl()
    found_struct = [s for s in structures if s.name_hash == tag]
    if found_struct:
        fstruct = found_struct[0]
        print(f'[{fstruct.name}]')
        read_len = 0
        while read_len <= length:
            t, l = binary_file.read_tl()
            data = [binary_file.read_int() for i in range(l)]
            read_len += 2 + l
            if t in fstruct.hashes:
                print(f'{fstruct.hashes[t].name:<30}=  {", ".join([str(i) for i in data])}')


def main():
    args = parse_args()
    generate_ini_files(args)


if __name__ == "__main__":
    main()

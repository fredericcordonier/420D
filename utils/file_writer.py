from typing import Dict, Union, TextIO
from pathlib import Path


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

class CFileWriter:
    def __init__(self, file_path: Path, filename: str):
        self.file_path = file_path
        self.filename = filename
        self.included_files: list[str] = []
        self.lines: list[str] = []

    def add_lines(self, lines: list[str]):
        self.lines += lines


class SrcFileWriter(CFileWriter):
    def __init__(self, file_path: Path, filename: str):
        super().__init__(file_path, filename)

    def write(self):
        with open(self.file_path / (self.filename + '.c'), 'w') as src_file:
            src_file.write(SRC_FILE_HEADER.format((f'{self.filename}.c'), f'{self.filename}'))
            included_files = [
                "vxworks.h",                # Needed for NULL definition
                "param_def.h",              # Needed to know the structure field_def.h
                f'{self.filename}.h'
            ] + self.included_files
            src_file.write('\n'.join([f'#include "{filename}"' for filename in included_files]))
            src_file.write('\n\n')
            src_file.write('\n'.join([l for l in self.lines]))


class HeaderFileWriter(CFileWriter):
    def __init__(self, file_path: Path, filename: str):
        super().__init__(file_path, filename)

    def write(self):
        with open(self.file_path / (self.filename + '.h'), 'w') as src_file:
            src_file.write(INC_FILE_HEADER.format((f'{self.filename}.h'), f'{self.filename}'))
            included_files = [
                "param_def.h"               # Needed to know the structure field_def.h
            ] + self.included_files
            src_file.write('\n'.join([f'#include "{filename}"' for filename in included_files]))
            src_file.write('\n\n')
            src_file.write('\n'.join([l for l in self.lines]))


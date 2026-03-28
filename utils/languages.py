import os
import subprocess
import re
import locale
from pathlib import Path

from deep_translator import GoogleTranslator, exceptions as dp_exceptions

src_folder = Path('../src/')
include_file = src_folder / 'inc'
language_header = include_file / 'languages.h'
language_ini_folder = src_folder / 'languages'


class SubprocessResult:
    # Replace Subprocess result object by handling encoding
    def __init__(self, sp_result: subprocess.CompletedProcess[bytes]):
        try:
            self.stdout = sp_result.stdout.decode("utf-8")
        except UnicodeDecodeError:
            self.stdout = sp_result.stdout.decode(locale.getpreferredencoding(False), errors="replace")
        try:
            self.stderr = sp_result.stderr.decode('utf-8')
        except UnicodeDecodeError:
            self.stderr = sp_result.stderr.decode(locale.getpreferredencoding(False), errors="replace")
        self.returncode = sp_result.returncode


class Grep:
    def __init__(self):
        self.encoding = 'utf-8'
        self.exe_name = 'grep'
        # self.command_path = executable_path / self.exe_name
        self.command_path = self.exe_name

    def search_string(self, search_string: str, search_path: Path):
        command = [search_string, search_path.as_posix() + '/*.c']
        return self.run_command(command)

    def run_command(self, command: list[str]):
        command = [str(self.command_path)] + command
        result = SubprocessResult(subprocess.run(command, capture_output=True))
        return result


class LangFile:
    def __init__(self, lang_file_path: Path):
        self.file_path = lang_file_path
        with open(self.file_path) as in_file:
            self.lines = in_file.read()
        self.strings = None
        self.string_usage = None

    def get_lang_strings(self):
        pattern = r'LANG_PAIR\(\s*(\w+)\s*,\s*"([^"]*)"\s*\)'
        self.strings = {key: value for key, value in re.findall(pattern, self.lines)}

    def get_lang_defs_usage(self):
        if self.strings is None:
            self.get_lang_strings()
        grep = Grep()
        self.string_usage = {}
        for k in self.strings.keys():
            results = [l for l in grep.search_string(k, src_folder).stdout.split('\n') if len(l) > 0]
            if len(results):
                results = [l.split(':')[0] for l in results]
            self.string_usage.update({k: results})


class LocaleFile:
    def __init__(self, language: str, lang_folder: Path, language_string: str):
        self.language = language
        self.language_string = language_string
        self.input_file = lang_folder / f'{language_string}.ini'
        self.active_messages = {}
        self.inactive_messages = {}
        self.to_be_translated = {}

    def read(self):
        # Read existing locale file and recover the existing translations
        lines = open(self.input_file, encoding='utf-8').read()
        pattern = r'\[(\w+)\]'
        results = re.findall(pattern, lines)
        if len(results) != 1:
            return
        self.language_string = results[0]
        strings = {l.split('=')[0]: l.split('=')[1] for l in lines.split('\n') if '=' in l}
        self.active_messages = {k.strip(): v.strip() for k, v in strings.items() if k[0] != ';'}
        # Keep translations for commented messages (no more used in software)
        self.inactive_messages = {k[1:].strip(): v.strip() for k, v in strings.items() if k[0] == ';'}

    def exists(self):
        return self.input_file.exists()

    def update(self, strings_dict: dict):
        # Update messages with new strings_dict
        # Go through strings and check if they are new (to be translated), or were inactive
        for s_name, s_value in strings_dict.items():
            if s_name in self.inactive_messages.keys():
                # If the message was inactive (commented), put it back into active messages
                self.active_messages.update({s_name: self.inactive_messages[s_name]})
                del(self.inactive_messages[s_name])
            elif s_name not in self.active_messages.keys():
                # New message --> to be translated
                self.to_be_translated.update({s_name: s_value})
        # Identify the list of messages that are no more used in software, and put them as inactive (commented)
        to_be_deleted = {s_name: s_value for s_name, s_value in self.active_messages.items() if s_name not in strings_dict.keys()}
        for s_name, s_value in to_be_deleted.items():
            self.inactive_messages.update({s_name: s_value})
            del(self.active_messages[s_name])

    def translate_messages(self):
        # Translate new messages (using GoogleTranslator, shall be reviewed because out of photo context)
        if len(self.to_be_translated):
            # Translate messages
            keys = list(self.to_be_translated.keys())
            values = list(self.to_be_translated.values())
            try:
                translations = GoogleTranslator(source='en', target=self.language).translate_batch(values)
                translations = [f'{t} ; Google Translated' for t in translations]
            except dp_exceptions.TranslationNotFound:
                self.active_messages.update(self.to_be_translated)
            else:
                self.active_messages.update(dict(zip(keys, translations)))
            self.to_be_translated = {}

    def write(self, out_file_name: str = ''):
        # Write strings in ini file
        self.message_names = list(self.active_messages.keys()) + list(self.inactive_messages.keys())
        if not out_file_name:
            out_file_name = f'{self.language_string}_new.ini'
        with open(self.input_file.parent / out_file_name, 'w', "utf-8") as out_file:
            # Write section name
            out_file.write(f'[{self.language_string}]\n')
            out_file.write('\n'.join([f'{k_name:<22} = {self.active_messages[k_name]}' if k_name in self.active_messages.keys() else f';{k_name:<21} = {self.inactive_messages[k_name]}'
                                       for k_name in sorted(self.message_names)]))


if __name__ == "__main__":
    lang_file = LangFile(language_header)
    print('Parsing language strings')
    lang_file.get_lang_defs_usage()
    print('Generate new_lang ini file')
    locale_file = LocaleFile('xx', language_ini_folder, 'MYLANG')
    if locale_file.exists():
        locale_file.read()
    locale_file.update(lang_file.strings)
    # Don't translate, just copy new messages
    locale_file.active_messages.update(locale_file.to_be_translated)
    locale_file.to_be_translated = {}
    # Write new lang file
    locale_file.write('new_lang.ini')
    # lang_file.generate_new_lang_ini()
    print('Translating language strings')
    locales = {
               'fr': 'FRENCH',
               'it': 'ITALIAN',
               'es': 'SPANISH',
               'zh-CN': 'SIMPLIFIED_CHINESE',
               'nl': 'DUTCH',
               'fi': 'FINNISH',
               'is': 'ÍSLENSKA',
               'pl': 'POLISH',
               'ru': 'RUSSIAN',
               'sv': 'SWEDISH',
                # # galego.ini
                # # magyar.ini
                # # new_lang.ini
                # # srpski.ini
               'de': 'GERMAN'
            }
    for code, string in locales.items():
        print(f'Language {string} ({code})')
        locale_file = LocaleFile(code, language_ini_folder, string)
        if locale_file.exists():
            locale_file.read()
        locale_file.update(lang_file.strings)
        locale_file.translate_messages()
        locale_file.write()

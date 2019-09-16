# libbxml

Tiny BinaryXML parser library

## Usage

`bxml <filename>` where `<filename>` is file path to _BinaryXML_ `.bxml` file

Regular XML file is printed to STDOUT.

## Build

`make` - compile all

`make run` - run `./bin/bxml` on `example.bxml` file

`make compile-cli` - compile `./bin/bxml` example program

`make bin/libbxml.a` - compile static library `./bin/libbxml.a`

`make bin/libbxml.so` - compile dynamic library `./bin/libbxml.so`

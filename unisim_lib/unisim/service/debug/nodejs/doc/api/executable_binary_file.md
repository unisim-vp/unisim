# Class ExecutableBinaryFile

This class is not construtible.
It is created when calling `loadDebugInfo`.

## `(read-only) id: Number`

The executable binary file identifier.

## `(read-only) file: string`

Absolute path of executable binary file.

## `(read-only) fileFormat: string`

File format (e.g. '`ELF64`')

## `(read/write) enable: boolean`

Whether this executable binary file is considered when searching in debug infos.

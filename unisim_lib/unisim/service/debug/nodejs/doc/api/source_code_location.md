# Class SourceCodeLocation

## `(constructor) SourceCodeLocation(loc: string) => SourceCodeLocation`

Create a source code location pointing from a formated string such as '`main.c:1234`'.

## `(constructor) SourceCodeLocation(sourceCodeFile: string, lineno: Number, [colno: Number]) => SourceCodeLocation`

Create a source code location pointing to source code file `sourceCodeFile` at line number `lineno` and column number `colno`.

## `(read-only) sourceCodeFilename: string`

Filename of source code.

## `(read-only) lineno: Number`

Line number in source code file (first line is line #1).

## `(read-only) colno: Number`

Column number in source code file (first col is line #1).

Zero means no column number.

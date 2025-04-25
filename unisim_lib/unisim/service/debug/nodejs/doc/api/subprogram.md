# Class SubProgram

## `(read-only) name: string`

Name of subprogram.

## `(read-only) file: string`

Executable binary file that this subprogram belongs to.

## `(read-only) isExternal: boolean`

Indicates whether this subprogram is `extern`.

## `(read-only) isDeclaration: boolean`

Indicates whether this subprogram instance is `extern`.

## `(read-only) isInline: boolean`

Indicates whether this subprogram is `inline`.

## `(read-only) isInlined: boolean`

Indicates whether this subprogram is effectively `inlined`.

## `(read-only) returType: Type`

Type of subprogram return value.

## `(read-only) arity: Number`

Number of formal parameters.

## `(read-only) formalParameters: Array of FormalParameter`

Ordered list of formal parameters.

## `(read-only) declLoc: SourceCodeLocation`

Declaration location.

## `(read-only) address: Number`

Address of subprogram.

## `(read-only) cdecl: string`

C declaration.

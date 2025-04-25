# Class DataObject

## `(constructor) DataObject(processor: Processor, expr: string) => DataObject`

Create a data object handle tracking an expression `exp` from processor `processor` point of view.

Valid expressions are C-like expressions such as '`*s.p->a[2]`'.

## `set(value: *, [options: Object])`

Assign a value to a data object.
The value is crawled recursively to write (with conversion when possible) each parts in the target data object.

Options:

* `strict: boolean`, it controls how out-of-bound subscripts and missing properties in the target data object are handled.
	* `true`: While crawling value, attempting to write items or properties not present in target data object results in a thrown exception.
	* `false` (default): While crawling value, anything not present in target data object is ignored.

## `get([options: Object]) => *`

Retrieve the data value.
The data object is crawled recursively to build the returned value.

Options:

* `flatten: boolean`, it controls how to handle pointers:
	* `true`: follow pointers and references, effectively flattening the data object representation into the returned value.
	* `false` (default): do not follow pointers and references, effectively replacing each pointer in the data object with an instance of class Pointer into the returned value.
* `raw: boolean`, it controls how to represent values:
	* `true`: anything that is neither an array nor an object is returned as a raw Node.js Buffer.
	* `false` (default): anything that is neither an array nor an object is returned as a Javascript value.
* `cstring: boolean`, it controls how to represent a `char pointer` (this option has no effect when `raw=true`):
	* `true` (default): a char pointer is returned when possible as a Javascript string, otherwise an instance of class Pointer is returned.
	* `false`: a char pointer is returned as an instance of class Pointer.

## `(read-only) processor: Processor`

The processor to which this data object handle is attached.

## `(read-only) expression: string`

The C-like expression such as '`*s.p->a[2]`'.

## `(read-only) name: string`

The name of the data object (should looks like `expression` when data object handle is valid).

## `(read-only) bitSize: number`

Size of data object in bits.

## `(read-only) type: Type`

Type of data object.

## `(read-only) cvUnqualifiedType: Type`

Cv-unqualified type (without any const and volatile qualifiers) of data object.

## `(read-only) endian: string`

Endian of data object (one of '`big-endian`', '`little-endian`', or '`unknown-endian`').

## `(read-only) exists: boolean`

whether the data object that this handle tracks exists.

## `(read-only) isOptimizedOut: boolean`

whether the data object that this handle tracks is optimized out.

## `(read-only) isObject: boolean`

Indicates whether data object is either a structure, a union, a class, or an interface.

## `(read-only) isStructure: boolean`

Indicates whether data object is a structure.

## `(read-only) isUnion: boolean`

Indicates whether data object is a union.

## `(read-only) isClass: boolean`

Indicates whether data object is a class instance.

## `(read-only) isInterface: boolean`

Indicates whether data object is an interface.

## `(read-only) isBasic: boolean`

Indicates whether data object is either of type `char`, an integer, a boolean, or a floating-point number.

## `(read-only) isChar: boolean`

Indicates whether data object is of type `char`.

## `(read-only) isInteger: boolean`

Indicates whether data object is an integer.

## `(read-only) isBoolean: boolean`

Indicates whether data object is a boolean.

## `(read-only) isFloat: boolean`

Indicates whether data object is a floating-point number.

## `(read-only) isPointer: boolean`

Indicates whether data object is a pointer.

## `(read-only) isFunction: boolean`

Indicates whether data object is a function.

## `(read-only) isConst: boolean`

Indicates whether data object type has `const` modifier.

## `(read-only) isEnum: boolean`

Indicates whether data object type is an `enum`.

## `(read-only) isVoid: boolean`

Indicates whether data object type is `void`.

## `(read-only) isVolatile: boolean`

Indicates whether data object type has `volatile` modifier.

## `(read-only) isSigned: boolean`

Indicates whether data object type is signed.

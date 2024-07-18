# Class DataObject

## `(constructor) DataObject(processor: Processor, expr: string) => DataObject`

Create a data object handle tracking an expression `exp` from processor `processor` point of view.

Valid expressions are C-like expressions such as '`*s.p->a[2]`'.

## `set(value: *, [options: Object])`

Assign a value to a data object.
When `options.flatten` is true, the data object and the value are crawled recursively, following pointers and references, to write each properties.

## `get([options: Object]) => *`

Retrieve the data value.
The data object is crawled recursively to build the returned value.

`options.flatten` controls how to handle pointers:

* `options.flatten=true`: follow pointers and references, effectively flattening the data object representation into the returned value,
* `options.flatten=false`: do not follow pointers and references, effectively replacing each pointer in the data object with an instance of class Pointer into the returned value.

## `(read-only) processor: Processor`

The processor to which this data object handle is attached.

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

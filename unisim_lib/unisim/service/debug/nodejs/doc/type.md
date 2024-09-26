# Class Type

Class Type describes programming language types.

It is the superclass of all type description classes.

	                                                                                                   Type
	                                                                                                     ^
	                                                                                                     |
	                                                              +--------------------------------------+-----------------+---------+------------+-----------+------------+-------------+
	                                                              |                                                        |         |            |           |            |             |
	                                                          NamedType                                                    |         |            |           |            |             |
	                                                              ^                                                        |         |            |           |            |             |
	                                                              |                                                        |         |            |           |            |             |
	                       +--------------------------------------+-----------+---------------------------+---------+      |         |            |           |            |             |
	                       |                                                  |                           |         |      |         |            |           |            |             |
	                    BaseType                                        CompositeType                     |         |      |         |            |           |            |             |
	                       ^                                                  ^                           |         |      |         |            |           |            |             |
	                       |                                                  |                           |         |      |         |            |           |            |             |
	     +----------+------------+-------------+            +------------+----+----+----------+           |         |      |         |            |           |            |             |
	     |          |            |             |            |            |         |          |           |         |      |         |            |           |            |             |
	IntegerType CharType FloatingPointType BooleanType StructureType UnionType ClassType InterfaceType EnumType Typedef ArrayType PointerType FunctionType ConstType UnspecifiedType VolatileType

## `(read-only) isComposite: boolean`

Indicates whether type is a boolean type.

## `(read-only) isStructure: boolean`

Indicates whether type is a structure.

## `(read-only) isUnion: boolean`

Indicates whether type is a union.

## `(read-only) isClass: boolean`

Indicates whether type is a class.

## `(read-only) isInterface: boolean`

Indicates whether type is an interface.

## `(read-only) isBase: boolean`

Indicates whether type is either a `char`, integer, boolean, or floating-point type.

## `(read-only) isChar: boolean`

Indicates whether type is a `char` type.

## `(read-only) isInteger: boolean`

Indicates whether type is an integer type.

## `(read-only) isBoolean: boolean`

Indicates whether type is a boolean type.

## `(read-only) isFloat: boolean`

Indicates whether type is a floating-point type.

## `(read-only) isPointer: boolean`

Indicates whether type is a pointer type.

## `(read-only) isTypedef: boolean`

Indicates whether type is a `typedef`.

## `(read-only) isFunction: boolean`

Indicates whether type is a function type.

## `(read-only) isArray: boolean`

Indicates whether type is an array type.

## `(read-only) isConst: boolean`

Indicates whether type represents a type with a `const` modifier.

## `(read-only) isEnum: boolean`

Indicates whether type is an `enum`.

## `(read-only) isUnspecified: boolean`

Indicates whether data object type is unspecified (such as `void`).

## `(read-only) isVolatile: boolean`

Indicates whether type represents a type with a `volatile` modifier.

## `(read-only) isNamed: boolean`

Indicates whether type may have a name.

## `(read-only) declLoc: SourceCodeLocation`

Declaration location of the type.

## `(read-only) cdecl: string`

C declaration of type.

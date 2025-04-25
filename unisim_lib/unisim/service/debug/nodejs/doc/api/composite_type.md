# Class CompositeType

This class describes composite types (such as a structure).

It is the superclass of `StructureType`, `UnionType`, `ClassType`, and `InterfaceType`.

## `(read-only) isIncomplete: boolean`

Indicates whether definition of composite type is incomplete.

## `(read-only) members: Object`

This objects has one property per composite type member (of type `Member`).
Each property is named as the member it describes.

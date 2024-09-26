# Class Pointer

This class is not construtible.
It is created when calling `get` method (with `options.flatten` evaluated as `false`) of a DataObject instance, see `DataObject` class description for details.

## `set(value: Number)`

Set value of pointer (i.e. address of pointed value).

## `get() => Number`

Get value of pointer (i.e. address of pointed value).

## `deref() => DataObject`

Dereference a pointer, then return the pointed data object.

# CDC
A C11 implementation of deck

# Types
- Strings
- Integers
- Symbols
- Arrays

# Features
- Static typing
- Symbols
- Let bindings
- Functions
- No control flow primitives

# IR Representation
Doubly linked list of "blocks" which contain a linked-list of
operations and a linked-list of constant data.

# Questions
- Do we want stack frames?

# Syntax
```
1 2 + 5 +
"hello" print

$(int int) +   #! type assertions

dup * #sqr def   #! function definition with symbols
4 #x let



1 2 3 4 5

( a b c d e -> a b a b a )   #! swizzling
( a b _ _ _ -> a b a )

1 2 1

4 move/pick/del   #! random access

```

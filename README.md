# deck
Minimalistic stack based language with unstructured control flow that compiles to x64 assembly.

Deck is designed to be very platform agnostic by avoiding platform specific oddities.

### Examples
##### Factorial
```
@fact >| dup 1 = &.end if dup -- fact * @.end |> .
```

# deck
Minimalistic concatenative language with unstructured control flow that
compiles to x86-64 assembly. The Deck runtime currently assumes that it's
running under Linux and so tries to use Linux specific systerm calls.

The current compiler uses Bash to tokenise the input stream and recognise
some special tokens like labels or functions. NASM is used both as a macro
preprocessor and the code generator. This version of the compiler mostly
serves as a very rough testbed for ideas and philosophy. A new version of
the compiler is being worked on in C++ which should hopefully be far more
ergonomic to use than a cobbled together shell script.

You can check out the C++ rewrite in the `cdeck` directory.

![c++](https://img.shields.io/badge/c%2B%2B-17-blue.svg?style=flat)
[![issues](https://img.shields.io/github/issues/Jackojc/deck.svg?style=flat)](https://github.com/Jackojc/deck/issues)
[![pull requests](https://img.shields.io/github/issues-pr/Jackojc/deck?style=flat)](https://github.com/Jackojc/deck/pulls)
[![license](https://img.shields.io/github/license/Jackojc/deck.svg?style=flat)](./LICENSE)

### Examples
> Some of these examples require functions defined in the prelude.

##### Factorial
```
5 fact io_intln sys_ok
@fact >| dup 1 = &.end if dup -- fact * @.end |> .
```

##### Sum & Product
```
[ 1 2 3 4 5 sum ] io_intln
[ 1 2 3 4 5 product ] io_intln
sys_ok

@sum &+ swp &reduce .
@product &* swp &reduce .
```

##### Min/Max
```
11 23 max io_intln
64 73 min io_intln
sys_ok
@min >| dupp > rotr ? ret #! ( a b cont -> q )
@max >| dupp < rotr ? ret #! ( a b cont -> q )
```

### Design & Rationale
Deck is an exercise in minimalism and so constructs common in more mainstream
languages are entirely missing from Deck.

The most obvious example is structured
control flow. It has long been considered that unstructured control flow is "harmful"
after Edsgar Dijkstra published his "Go To Statement Considered Harmful" letter.
Due to this, Deck has no concept of for-loops, while-loops or if-else branches.
All we have to use is the choice operator (`?`) which collapses to one of two values
based on a flag condition and labels aswell as functions. Classical loops and branching
can be emulated by building abstractions on top of the choice operators, labels and
functions.

Deck also has no concept of types, less so than even a dynamically typed language.
Everything in Deck is normally considered to be a signed integer unless the operator
decides to cast it to some other type (i.e. a pointer). As with everything else in
Deck, this has potential for bugs but also gives you plenty of control.

One of the nice things that comes along with the lack of types is a runtime determined
stack size which allows you to do some fun things like push a variable number of values
to the stack based on a runtime value. This is usually not possible in most languages
unless you make use of heap allocated memory but that incurs a runtime cost and
is not as ergonomic to use.

The return address is passed to functions explicitly which allows the user to
decide where it goes or what to do with it. You can pop the return address,
push it to the other side of the deque or just leave it on the stack and juggle
it around. It's also possible to call a function _without_ pushing the return
address using the go operator (`.`) if you want to try doing direct threading or
"computed goto".

### Goals
- Register Allocation
- Function Inlining
- Constant Folding
- C++ Rewrite (check the `cdeck` directory)

### Requirements
- NASM
- Bash
- ld (GNU Linker)
- m4 (Macro Processor)

### Build & Run
To run the compiler you simply have to pipe your source file into
the compiler script (`dcc`) like so:
```sh
$ ./dcc bin < main.dk
$ ./bin
```
This will spit out a compiled ELF64 binary with the name `bin` in the
local directory.

### Resources
https://blog.rchapman.org/posts/Linux_System_Call_Table_for_x86_64/
https://www.nasm.us/xdoc/2.13.03/html/nasmdoc0.html
https://www.felixcloutier.com/x86/
https://arxiv.org/pdf/2011.05608v1.pdf
https://legacy.cs.indiana.edu/~dyb/pubs/ddcg.pdf
https://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.156.2546&rep=rep1&type=pdf

### License
This project uses the GPL-3.0 license. (check [LICENSE](LICENSE))


## n->D Language

This is my beautiful nD language.

Inspired by the immutability of SMLNJ and the memory ownership model of Rust, I have come to create my own language. 

My language puts as many errors as possible to the compiler to find out, thanks to immutability being the default for every variable declared. 
However, unlike C or Rust with immutability, or SMLNJ with mutability, I am creating a system that is easy for both. 
Instead of an afterthought or a feature, mutability and memory control should be an easily accessible and safe feature for any programmer. 
Same with mathematical correctness and functional programming; you should be able to freely utilize it when it is needed, such as lambda functions, HOFs, and CPS style programming.


Feature #1
Block Declaration of Variables

Instead of cluttering up variable declaration throughout the function body, we declare variables at the start of any scope with the square brackets.

This actually makes it very easy to visually inspect variable declarations and their scopes. It also forces the programmer to organize their body in a way that only has logical code, pushing away from imperative programming; however, I try to keep it natively easy to do such memory control and alterations. 

Feature #2
Function/Program Arena Allocator

My interpreter (JIT later on) will allocate each body. Each of these bodies will have their respective stacks and such, but the main difference is that any allocations will be completely handled by the compiler.

Instead of malloc, every pointer is allocated by the 'new' keyword and has two pathways in its life:

1. Get freed once out of scope
2. Get assigned to a variable in the body that called the function that returns this pointer

This fixes a big problem in C: use-after-free and memory leaks.

This makes mutable data a lot less problematic; however, we usually need dynamic data as well.

Feature #3
Dynamic Heap Pool

My language will ship with the 'list' type, a UBA-implemented array that dynamically resizes and works very much like vectors in cpp. Currently not implemented, but it will always try its best to maintain an uninterrupted length of memory in the array, allowing it to live completely in cache and be able to bump pointers for incrementation.

Lists will also have a lifespan similar to the pointers, except the body allocations will only allocate the pointer of the list; at every exit of scope, the interpreter will see it is a list type, access each list index, and free everything. 



The project has many holes currently:

- Lambdas unimplemented
- Function as variable type implementation
- Needs more love on functions as values
- Lists implementation
- No runtime implementation yet.

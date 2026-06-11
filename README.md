This is my beautiful nD language.

Inspired by the immutability of SMLNJ and the memory ownership model of Rust, I have come to create my own language. 

My language puts as many errors as possible to the compiler to find out, thanks to immutability being the default for every variable declared. 
However, unlike C or Rust with immutability, or SMLNJ with mutability, I am creating a system that is easy for both. 
Instead of an afterthought or a feature, mutability and memory control should be an easily accessible and safe feature for any programmer. 
Same with mathematical correctness and functional programming, you should be able to freely utilize it when it is needed, such as lambda functions, HOFs, and CPS style programming.


Feature #1
Block delcaration of variables

Instead of cluttering out variable declaration throughout the function body, we declare variables at the start of any scope with the square brackets.

This actually makes it very easy to visually inspect variable delcaration and their scopes. It also forces the programmer to organize their body in a way it only has logical code, pushing away from imperitive programming, however I try to keep it natively easy to do such memory control and altercations. 

Feature #2
idk
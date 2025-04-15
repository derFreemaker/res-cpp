# res-cpp
Is a simple header-only Result type for C++.

# TODO
- auto convert ErrorT from one result to ErrorT of other if possible
- more tests
- compiler compatible? (should be gcc/clang right now)

# Limitations

## Optimization
In order to realise the try macro the value from the result type is not stored on the stack.
It is stored on a static thread_local variable, this is slower a little bit.
And has some performance optimization cases cannot be performed by the compiler (most propably).


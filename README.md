# res-cpp
Is a simple header-only Result type for C++.

# Limitations

## Optimization
In order to realise the try macro the value from the result type is not stored on the stack.
It is stored on a static thread_local variable, this is slower a little bit.
And has some performance optimization cases cannot be performed by the compiler (most propably).


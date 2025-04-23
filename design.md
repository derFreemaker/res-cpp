# Design Choices

## result signature
``result<T, E>``

### type ``T``
Should be compatible everything.

### type ``E``
Every struct (no references or pointer),
as well as enums (use a wrapper here).

## memory layout

### result\<T, E>
- union
    - E error
    - T value
- bool has_error

this should minimize our memory to maximum of ``E`` and ``T``
plus the boolean.

### result\<void, E>
- std::optional\<E> error

this makes it easy for handling ok result,
since optional handles every thing we want

## normal usage
```c++
enum class parse_error { none, to_slow };
result<int, parse_error> parse_int(std::string value) {
    if (/*fail condition*/(false)) {
        return parse_error::none; // fail (implicit convertion) or maybe
        // return result::fail(parse_error::none); // more explicit
    }
    return 123; // success (implicit convertion)
}
```
We could make implicit convertion possible for both success and fail.
But we need a second way to make it explicit if it is ok
or fail when 'T' and 'E' are the same type. Or we restrict it from doing that.

we could use tags?
```c++
return { result::fail, parse_error::none };
return { result::ok, 123 };
```

or just functions
```c++
return result::fail(parse_error::none);
return result::ok(123);
```
basically the same thing just that the functions provide more possibilities.

### solution
we are using the function ``rescpp::fail(...)`` to indicate a fail
and just return is success.

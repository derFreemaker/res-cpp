# Design Choices

## normal usage
```c++
enum class parse_error { none, to_slow };
result<int, parse_error> parse_int(std::string value) {
    if (/*fail condition*/(false)) {
        // return result::fail(parse_error::none); // more explicit
        return parse_error::none; // fail (implicit convertion)
    }
    return 123; // success (implicit convertion)
}
```
We could make implicit convertion possible for both success and fail.
But we need a second way to make it explicit if it is success
or fail if 'T' and 'E' types are the same.

we could use tags?
```c++
return { result::fail, parse_error::none };

return { result::success, parse_error::none };
```



radix_tree
=====

[ ![Codeship Status for kpp/radix_tree](https://codeship.com/projects/d47b4290-e2fe-0132-77af-0e8782f5ebb8/status?branch=master)](https://codeship.com/projects/81626)

STL like container of radix tree in C++

Usage
=====
It's a header-only library. Just include it. See [examples](examples/).

Develop
=====
Requirements: any C++98 compiler (`g++` or `clang++`), `cmake`, `Google Test Framework`

```
~/radix_tree $ mkdir build && cd build
~/radix_tree/build $ cmake .. -DBUILD_TESTS=On
~/radix_tree/build $ make check
```

Copyright
=====
See [COPYING](COPYING).

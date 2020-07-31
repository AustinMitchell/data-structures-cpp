# Data Structures CPP (dsc)

A project implementing various data structures to practice C++17 with some modern features.

Currently implemented structures:
  - ring_vector
    - Dynamically resizing array-backed structure supporting random access
    - Interfaces similar to a vector, but has O(1) insertion and removal on both back and front
  - splay_tree
    - Sorted self-balancing binary tree
    - Can select between full splay tree or semi-splay trees. Full splay operations splay the node all the way to the top of the tree while semi-splay will splay the node half way up the tree. Both display different performance metrics depending on the access sequence.
    - Supports iteration in-order
    - https://en.wikipedia.org/wiki/Splay_tree
  - box
    - Thread safe storage which can have one item emplaced with `put` and retrieved with `get`
    - If there is no item in the box, `get` will block until an item is placed with `put`, and if there is an item then `put` will block until an item is taken with `get`

## Building Tests:

A custom build script is used to create tests. Invoke `./build` to execute. This build script is a shorthand for invoking the <span>make.py</span> script from the C_build_script submodule. Requires Python3.

```
./build -h
usage: build [-h] [-s S] [-d] [-c]

optional arguments:
  -h, --help  show this help message and exit
  -s SOURCE   Source file to generate executable from
  -d          Sets build to debug mode
  -c          Cleans all output files
                 Sets build to debug mode
```

For example, to build ring_vector_test.cpp (full path is optional):
```bash
./build -s ring_vector_test.cpp
# or
./build -s src/ring_vector_test.cpp
```

Building ring_vector_test in debug mode:
```bash
./build -d -s ring_vector_test.cpp
```

To remove the folders containing the executables and the object files:
```bash
./build -c
```

Building ring_vector_test.cpp, and clean before building:
```bash
./build -c -s ring_vector_test.cpp
```

# Data Structures CPP (dsc)

A project implementing various data structures to practice C++23 with some modern features.

Currently implemented structures:
  - ring_vector
    - Dynamically resizing array-backed structure supporting random access
    - Interfaces similar to a vector, but has O(1) insertion and removal on both back and front
  - splay_tree
    - Sorted self-balancing binary tree
    - Can select between full splay tree or semi-splay trees. Full splay operations splay the node all the way to the top of the tree while semi-splay will splay the node half way up the tree. Both display different performance metrics depending on the access sequence.
    - Supports iteration in-order
    - https://en.wikipedia.org/wiki/Splay_tree
  - safe_box
    - Thread safe storage which can have one item emplaced with `put` and retrieved with `get`
    - If there is no item in the box, `get` will block until an item is placed with `put`, and if there is an item then `put` will block until an item is taken with `get`
  - safe_queue
    - Thread safe FIFO queue
    - `get` calls will block until there are items to take. `try_get` will fail and exit the function if there are no items.
  - heap
    - Array-backed heap structure providing O(log n) insertion and removal.
    - Supports min and max heap for any T that supports operators `<` and `>`

## Building Tests:

A custom build script is used to create tests. Invoke `./build` to execute. This build script is a shorthand for invoking the `make.py` script from the C_build_script submodule. Requires Python3.

```
❯ python3 .\build -h
usage: build [-h] [-s SOURCE] [-o EXE NAME] [-a] [-d] [-c]

options:
  -h, --help   show this help message and exit
  -s SOURCE    Source file to generate executable from
  -o EXE NAME  Executable output name
  -a           Build all and skip linking
  -d           Sets build to debug mode
  -c           Cleans all output files
```

Build all source files, no executable:
```bash
./build -a
```

Build executable for ring_vector_test.cpp:
```bash
./build -s src/ring_vector_test.cpp
```

Building executable for ring_vector_test in debug mode:
```bash
./build -d -s src/ring_vector_test.cpp
```

To remove the folders containing the executables and the object files:
```bash
./build -c
```

Building executable for ring_vector_test.cpp, and clean before building:
```bash
./build -c -s src/ring_vector_test.cpp
```

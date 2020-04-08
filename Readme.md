# Data Structures CPP (dsc)

A project implementing various data structures to practice C++17 with some modern features. These structures are intended to be compatible with STL container interfaces.

Currently implemented structures:
  - ring_vector
    - Dynamically resizing array-backed structure supporting random access
    - Interfaces similar to a vector, but has O(1) insertion and removal on both back and front
  - splay_tree
    - Sorted self-balancing binary tree
    - Can select between full splay tree or semi-splay trees. Full splay operations splay the node all the way to the top of the tree while semi-splay will splay the node half way up the tree. Both display different performance metrics depending on the access sequence.
    - Supports iteration in-order
    - https://en.wikipedia.org/wiki/Splay_tree

## Building Tests:

A custom build script is used to create tests. Invoke `./build` to execute. This build script is a shorthand for invoking the <span>make.py</span> script from the C_build_script submodule and passing in a customized configuration file, located under `config/`. Requires Python3.

```
./build -h
usage: build [-h] [-d]
             {splay-tree-test,splay-tree-perf,ring-vector-test,clean}

positional arguments:
  {splay-tree-test,splay-tree-perf,ring-vector-test,clean}
                        Config type. clean will delete bin and object folders.

optional arguments:
  -h, --help            show this help message and exit
  -d                    Sets build to debug mode
```

For example, to build ring_vector_test:
```
./build ring-vector-test
```

To remove the folders containing the executables and the object files:

```
./build clean
```
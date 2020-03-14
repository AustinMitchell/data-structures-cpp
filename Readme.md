A project implementing various data structures to practice C++17 with some modern features.

Currently implemented structures:
  - ring_vector
    - Dynamically resizing array-backed structure supporting random access
    - Interfaces similar to a vector, but has O(1) insertion and removal on both back and front
  - splay_tree
    - Sorted self-balancing binary tree
    - Can select between full splay tree or semi-splay trees. Full splay operations splay the node all the way to the top of the tree while semi-splay will splay the node half way up the tree. Both display different performance metrics depending on the access sequence.
    - https://en.wikipedia.org/wiki/Splay_tree
